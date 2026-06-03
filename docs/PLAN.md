# MLC / MLCC Development Plan

## 1. Текущее состояние

### Архитектура mlcc (self-hosted)

```
Source → Lexer → Tokens → Parser → AST → Checker → SemanticIR → Codegen → C++ source
```

Codegen строит **CppAST** (`compiler/cpp/cpp_ast.mlc`) и печатает через `cpp_printer.mlc`. String-шаблоны в `codegen/expr/expr.mlc` и `expr_fragment_codegen` остаются только для bootstrap edge cases (stmt/decl bridges). Tracks [TRACK_CPPEXPR.md](agent/TRACK_CPPEXPR.md) + [TRACK_CPPGEN.md](agent/TRACK_CPPGEN.md) **closed** (2026-05).

### Производительность

- Маленькие программы (например `fibonacci.mlc`): нативный `mlcc` на порядки быстрее полного запуска Ruby (у Ruby большой startup).
- Полная трансляция `compiler/main.mlc`: ориентир порядка **нескольких секунд** у `mlcc` (native) против **десятков секунд** у Ruby только на `ModularCompiler#compile` (без g++); замеры держать в правиле `.cursor/rules/mlcc-self-host-verification.mdc`.
- Рост числа деклараций: избегать наивного `fold` по большим структурам с COW-`Map` при копировании аккумулятора (пример исправления: `build_registry` — мутация одного `TypeRegistry` через `ref mut`, см. `compiler/checker/registry.mlc`).

### Проблемы структуры

- Codegen = строковая конкатенация. Нет гарантий корректности C++ без компиляции.
- Нет форматирования вывода (C++ выход нечитаем).
- Диагностика ошибок слабая: нет span в части сообщений.
- `out/` содержит ~150 файлов .cpp/.hpp — артефакты, не версионируются, засоряют workspace.
- Отсутствует fuzzing / property-based тесты.
- E2E через `compiler/tests/e2e/`; unit+integration self-hosted стека: **`rake test_compiler_mlc`** (511 pass, baseline post-CPPGEN).
- После изменений **checker/codegen/main.mlc**: обязательно **`compiler/build.sh`**, трансляция `compiler/main.mlc` и при необходимости проверка **идентичности** выхода mlcc и mlcc2 (`diff -rq`), см. `README.md`.

---

## 2. Целевая архитектура (референсы: Clang, Swift, Rust MIR)

### Референсы

**Clang**: `AST → CodeGenAction → LLVM IR`. Codegen работает с полным типизированным AST, не со строками.

**Swift SIL**: между AST и LLVM IR. SIL — explicit ownership, generic specialization, инлайнинг. Позволяет оптимизации до LLVM.

**Rust MIR**: между HIR и LLVM IR. Control-flow граф. Borrow checker работает на MIR, не на AST.

**LLVM**: многоуровневые IR + pass manager. Каждый уровень изолирован, тестируется отдельно.

### Целевой пайплайн MLC

```
Source
  → Lexer → [Tokens]
  → Parser → [AST]                     # уже есть
  → Checker → [SemanticIR]             # уже есть, типизированный AST
  → Lowering → [MLC IR]                # NEW: flat, explicit, control-flow граф
  → C++ Backend → [CppAST]             # NEW: типизированное представление C++
  → Printer → C++ source               # простой pretty-printer
```

### MLC IR (приоритет после стабилизации)

Простой flat IR, вдохновлённый Rust MIR:
- Explicit временные переменные вместо вложенных выражений
- Явные блоки (`BasicBlock`) с терминаторами (`Jump`, `CondJump`, `Return`)
- Нет побочных эффектов, скрытых в операторах
- Позволяет: dead code elimination, constant folding, inlining

Это не обязательно для Phase 1. Делается после стабилизации.

### C++ AST (приоритет: Phase 2)

`compiler/cpp/ast.mlc` — расширить до полного AST для эмиссии:

```
type CppType = CppTypeName(string) | CppTypeTemplate(string, [CppType]) | CppTypeRef(CppType) | ...

type CppExpr =
  | CppInt(i64) | CppStr(string) | CppBool(bool)
  | CppIdent(string)
  | CppCall(CppExpr, [CppExpr])
  | CppMember(CppExpr, string, bool)   # bool = pointer (->)
  | CppIndex(CppExpr, CppExpr)
  | CppBinary(string, CppExpr, CppExpr)
  | CppUnary(string, CppExpr)
  | CppTernary(CppExpr, CppExpr, CppExpr)
  | CppLambda([CppCapture], [CppParam], CppType, [CppStmt])
  | CppInitList([CppExpr])
  | CppStdVisit(CppExpr, [CppExpr])    # std::visit(overloaded{...}, x)
  | CppCast(CppCastKind, CppType, CppExpr)

type CppStmt =
  | CppAutoDecl(string, CppExpr)
  | CppConstDecl(string, CppType, CppExpr)
  | CppReturn(CppExpr)
  | CppExprStmt(CppExpr)
  | CppIf(CppExpr, [CppStmt], [CppStmt])
  | CppWhile(CppExpr, [CppStmt])
  | CppFor(string, CppExpr, [CppStmt]) # range-for
  | CppBlock([CppStmt])

type CppDecl =
  | CppInclude(bool, string)
  | CppUsing(string, CppType)
  | CppStruct(string, [string], [CppField], [CppDecl])  # generics, fields, methods
  | CppFnDecl(string, CppType, [CppParam], [string])    # name, return, params, attrs
  | CppFnDef(string, CppType, [CppParam], [CppStmt])
  | CppNamespace(string, [CppDecl])
  | CppVariant(string, [CppVariantArm])

type CppFile = CppFile { header: [CppDecl], source: [CppDecl] }
```

Printer — отдельный модуль `compiler/cpp/printer.mlc` → детерминированный, форматированный вывод.

Преимущества:
- Ошибки в codegen = compile-time вместо неправильного C++ кода
- Тестируется отдельно от SemanticIR
- Форматирование управляется через printer, а не через строки
- Позволяет будущий WASM/LLVM backend

---

## 3. Архитектурные паттерны

### Процедурность: проблема или нет?

Функции над алгебраическими типами — зрелый подход. GHC, Elm, ранний Rust compiler устроены так же.
Проблема не в отсутствии классов, а в конкретных запахах текущего кода.

### Текущие запахи

| Запах | Где | Решение |
|---|---|---|
| Дублирование `match SExpr` | все модули codegen | Visitor trait |
| God object `CodegenContext` в каждом параметре | codegen/* | Scoped context с методами через `extend` |
| Инъекция `gen_stmts` как параметр-функция | eval.mlc | Pass trait устраняет import cycle структурно |
| Неявный pipeline | main.mlc → прямые вызовы | Явная цепочка `Pass<In, Out>` |

### Visitor pattern (через MLC trait)

```mlc
trait ExprVisitor<Result> {
  fn visit_int(self, value: i32, semantic_type: Shared<Type>) -> Result
  fn visit_str(self, value: string, semantic_type: Shared<Type>) -> Result
  fn visit_call(self, function_expr: Shared<SExpr>, args: [Shared<SExpr>], semantic_type: Shared<Type>) -> Result
  fn visit_if(self, condition: Shared<SExpr>, then_branch: Shared<SExpr>, else_branch: Shared<SExpr>, semantic_type: Shared<Type>) -> Result
  fn visit_match(self, subject: Shared<SExpr>, arms: [Shared<SMatchArm>], semantic_type: Shared<Type>) -> Result
  # ...
}

fn dispatch_expr<Result>(visitor: ExprVisitor<Result>, expr: Shared<SExpr>) -> Result = match expr {
  SExprInt(value, semantic_type, _)   => visitor.visit_int(value, semantic_type),
  SExprStr(value, semantic_type, _)   => visitor.visit_str(value, semantic_type),
  SExprCall(function_expr, args, _, semantic_type, _) => visitor.visit_call(function_expr, args, semantic_type),
  # ...
}
```

`dispatch_expr` написан один раз. Каждый codegen-модуль реализует только `ExprVisitor<string>` или `ExprVisitor<CppExpr>`.

### Pass pipeline

```mlc
trait Pass<Input, Output> {
  fn run(self, input: Input) -> Result<Output, [Diagnostic]>
}

type CompilerPipeline = CompilerPipeline {
  checker_pass: Pass<Program, SemanticProgram>,
  transform_pass: Pass<SemanticProgram, SemanticProgram>,
  codegen_pass: Pass<SemanticProgram, CppFile>
}

fn run_pipeline(pipeline: CompilerPipeline, program: Program) -> Result<CppFile, [Diagnostic]> = do
  let semantic_program = pipeline.checker_pass.run(program)?
  let transformed = pipeline.transform_pass.run(semantic_program)?
  pipeline.codegen_pass.run(transformed)
end
```

Это устраняет import cycle в `eval.mlc` структурно — checker и codegen знают только об интерфейсе Pass, не друг о друге.

### Что НЕ нужно брать из OOP

- **Factory**: нет, sum types + match лучше
- **Inheritance**: нет, traits + extend лучше и точнее
- **Mutable shared state**: нет, передавать контекст явно
- **Singleton registries**: нет, передавать как параметр Pass

### Применимые паттерны из Ruby-версии

| Ruby-паттерн | Аналог в MLC |
|---|---|
| Rule Engine (один файл на AST-узел) | `ExprVisitor` impl в отдельном модуле |
| Visitor Pattern | `trait ExprVisitor<T>` + `dispatch_expr` |
| Registry Pattern | `TypeRegistry` как immutable record, передаётся в Pass |
| PassManager | `CompilerPipeline` + `Pass<In, Out>` |

### Порядок внедрения

1. `trait Pass<In, Out>` — добавить в `compiler/` как тип, не меняя существующий код
2. Visitor для codegen — начать с `ExprVisitor<string>`, потом мигрировать на `ExprVisitor<CppExpr>`
3. `extend CodegenContext` с методами вместо свободных функций где это читаемее
4. Явный pipeline в `main.mlc`

---

## 4. Структура файлов и папок

### Текущая структура (проблемы)

```
compiler/
  ast.mlc                        # корень — не ясен слой
  ast_tokens.mlc                 # корень
  semantic_ir.mlc                # корень
  lexer.mlc                      # корень
  record_field_default_initializer.mlc  # корень, длинное имя
  main.mlc
  main_modular_test.mlc          # тест в корне
  test_lexer.mlc                 # тест в корне
  test_trait_e2e.mlc             # тест в корне
  checker/                       # смешаны 3 разных прохода
    infer*.mlc                   # type inference
    transform*.mlc               # AST → SemanticIR
    check*.mlc                   # валидация
    names.mlc, registry.mlc, ...
  codegen/
    eval.mlc                     # "binding layer" — неочевидно
    expr.mlc                     # "string helpers" — неочевидно
    expr_eval.mlc                # "expression traversal" — неочевидно
    stmt_eval.mlc
    decl.mlc, decl_extend.mlc
    ...
  cpp/                           # изолирован, не растёт
  parser/
```

### Целевая структура

```
compiler/
  main.mlc

  frontend/                      # всё что принимает MLC source
    lexer.mlc                    # ← compiler/lexer.mlc
    ast.mlc                      # ← compiler/ast.mlc
    ast_tokens.mlc               # ← compiler/ast_tokens.mlc
    parser/
      decls.mlc
      exprs.mlc
      types.mlc
      predicates.mlc             # ← preds.mlc

  ir/                            # промежуточные представления
    semantic_ir.mlc              # ← compiler/semantic_ir.mlc
    record_defaults.mlc          # ← record_field_default_initializer.mlc

  checker/
    registry.mlc                 # типы + TypeRegistry
    names.mlc                    # name resolution
    type_structure.mlc           # ← semantic_type_structure.mlc

    infer/                       # type inference
      infer.mlc
      call.mlc                   # ← infer_call.mlc
      call_support.mlc           # ← infer_call_support.mlc
      expr_ident.mlc             # ← infer_expr_ident.mlc
      for_support.mlc            # ← infer_for_support.mlc
      lambda_context.mlc         # ← infer_lambda_context.mlc
      literals.mlc               # ← infer_literals.mlc
      match.mlc                  # ← infer_match.mlc
      operand_combine.mlc        # ← infer_operand_combine.mlc
      question_expr.mlc          # ← infer_question_expression.mlc
      result.mlc                 # ← infer_result.mlc
      result_option_method.mlc   # ← infer_result_option_method.mlc
      array_method.mlc           # ← infer_array_method.mlc
      trait_bounds.mlc           # ← infer_trait_bounds.mlc

    transform/                   # AST → SemanticIR
      transform.mlc
      decls.mlc                  # ← transform_decl.mlc
      stmts.mlc                  # ← transform_stmts.mlc
      let_pattern.mlc            # ← let_pattern_infer.mlc
      partial_application.mlc    # ← partial_application_desugar.mlc
      param_destructure.mlc      # ← param_destructure_expand.mlc
      trait_param.mlc            # ← trait_param_expand.mlc
      record_lit_merge.mlc
      pattern_env.mlc

    check/                       # валидационные проходы
      check.mlc
      context.mlc                # ← check_context.mlc
      mutations.mlc              # ← check_mutations.mlc
      derive.mlc                 # ← derive_validation.mlc
      call_arguments.mlc         # ← call_argument_unify.mlc
      diagnostics/
        binary.mlc               # ← binary_diagnostics.mlc
        types.mlc                # ← type_diagnostics.mlc
        method_receiver.mlc      # ← method_receiver_diagnostics.mlc
      method_types/
        array.mlc                # ← array_method_types.mlc
        result_option.mlc        # ← result_option_method_types.mlc

  codegen/
    module.mlc                   # top-level entry (без изменений)
    dispatch.mlc                 # ← eval.mlc (binding layer, явное имя)
    context.mlc
    cpp_naming.mlc

    expr/                        # генерация выражений
      gen.mlc                    # ← expr_eval.mlc (основной traversal)
      helpers.mlc                # ← expr.mlc (string helpers)
      support.mlc                # ← expression_support.mlc
      literals.mlc
      identifiers.mlc
      method.mlc                 # ← method_gen.mlc
      record.mlc                 # ← record_gen.mlc
      match_gen.mlc
      match_analysis.mlc
      let_pat.mlc
      mut_argument.mlc           # ← mut_actual_argument.mlc

    stmt/                        # генерация statements
      gen.mlc                    # ← stmt_eval.mlc
      context.mlc                # ← statement_context.mlc
      return_body.mlc

    decl/                        # генерация деклараций
      gen.mlc                    # ← decl.mlc
      extend.mlc                 # ← decl_extend.mlc
      index.mlc                  # ← decl_index.mlc
      type_gen.mlc
      type_index.mlc
      ctor_info.mlc
      param_analysis.mlc

  cpp/                           # C++ AST + printer (расширяется в Phase 2)
    ast.mlc
    printer.mlc                  # NEW
    lexer.mlc
    parser.mlc
    predicates.mlc

  tests/                         # все тесты (уже есть)
    ...

  out/                           # артефакты (gitignored)
```

### Принципы переименования

- Убрать prefix из имён файлов внутри папки: `infer_call.mlc` → `infer/call.mlc`
- Суффикс `_eval` → нет (папка задаёт контекст): `stmt_eval.mlc` → `stmt/gen.mlc`
- Расплывчатые `eval.mlc`, `expr.mlc` → `dispatch.mlc`, `helpers.mlc`
- Тесты только в `tests/`, не в корне

### Порядок миграции

Это большая рефакторинг-задача — все пути `import` нужно обновить. Делается отдельным коммитом, без изменения логики:

1. Перенести `frontend/` (lexer, ast, parser) — минимум внешних зависимостей
2. Перенести `ir/` (semantic_ir, record_defaults)
3. Разбить `checker/` на `infer/`, `transform/`, `check/`
4. Разбить `codegen/` на `expr/`, `stmt/`, `decl/`
5. Убрать тесты из корня в `tests/`

После каждого шага: `bundle exec rake test_compiler_mlc` должен проходить 447/447.

---

## 5. Фазы разработки

### Phase 1: Стабилизация (приоритет сейчас)

**Цель**: надёжный mlcc без регрессий, полное покрытие тестами.

1. **Диагностика**: все ошибки должны иметь точный `span` (файл:строка:колонка).
   - Формат rustc-style message + location — **step 1 done** ([TRACK_PHASE1](agent/TRACK_PHASE1.md) `a74d480`).
   - Коды ошибок `error[E001]:` — step 2 in progress.
   - Добавить span к типам в `SemanticIR` где отсутствует.
   - Унифицировать формат: `error[E001]: message\n  --> file:line:col`.

2. **Тесты**: расширить `test_checker.mlc` и `test_codegen.mlc`.
   - Цель: каждый вариант каждого `SExpr`/`SStmt` покрыт хотя бы одним тестом.
   - Negative tests: невалидный ввод должен давать конкретную ошибку, не crash.

3. **Fuzzing**: написать простой random AST generator на MLC.
   - Генерировать случайные синтаксически корректные программы.
   - Проверять: mlcc не падает, вывод компилируется g++.

4. **Crash-safety**: mlcc не должен производить core dump ни на каком вводе.
   - Все `unreachable` ветки → `panic` с сообщением.
   - Добавить `--check-only` флаг: только checker, без codegen.

5. **Out-директория**: убрать `out/` из workspace.
   - Писать в `--output` или в `/tmp/mlc_XXXXX/` по умолчанию.
   - Или: `--output-dir path`.

6. **Сборка**: `build.sh` должен быть детерминированным.
   - Зафиксировать порядок линковки.
   - Добавить `--emit-compile-commands` для clangd.

### Phase 2: C++ AST backend (архитектурное улучшение)

**Статус:** основной переход выполнен (TRACK_CPPEXPR + TRACK_CPPGEN closed). Остатки string-bridge — bootstrap edge cases.

**Цель**: заменить строковую конкатенацию в codegen на построение CppAST.

1. Расширить `compiler/cpp/ast.mlc` до полного AST (см. выше).
2. Написать `compiler/cpp/printer.mlc` — форматированный pretty-printer.
3. Перевести `codegen/` модули с `-> string` на `-> CppExpr` / `-> CppStmt`.
   - Начать с листьев: литералы, идентификаторы.
   - Затем выражения, затем statements, затем declarations.
4. Параллельно запускать старый и новый backend, сравнивать вывод (diff-тест).
5. После полного перехода — удалить старый string-based codegen.

Это основная архитектурная работа. Делается инкрементально, без регрессий.

### Phase 2.5: Языковые улучшения

**Цель**: убрать boilerplate, который заметен уже в текущем коде компилятора.

1. **Match по строкам** — сейчас все `keyword_kind`-функции это длинные `if/else if` цепочки. Нужно:
   - Parser: разрешить строковые литералы как паттерны в `match`-армах.
   - Checker: тип subject должен быть `string`; паттерны проверяются на уникальность.
   - Codegen: генерить `if/else if` или lookup-таблицу.
   - Пример после:
     ```
     fn keyword_kind(word: string) -> TKind =
       match word {
       | "fn"     -> KFn
       | "type"   -> KType
       | "let"    -> KLet
       | _        -> KIdent(word)
       }
     ```

2. **Match со строковыми regex-паттернами** (опционально, после п.1):
   - Синтаксис: `| /^[0-9]+$/ -> ...` в match-армах.
   - Codegen: вызов `std::regex_match` или `re2` в C++ выходе.
   - Полезно для лексеров, валидации, CLI-парсеров.

3. **Деструктуризация в let** — `let { st, tok } = scan(state)` — синтаксис уже в parser/checker (тесты есть), но в коде компилятора не используется. Задача: пройтись по `compiler/` и заменить `result.field` паттерн.

4. **Type aliases** — `type CppExprs = [Shared<CppExpr>]`. Нужно в parser + codegen для устранения повторений длинных generic-типов.

5. **ParseResult<T>** — сейчас 15 одинаковых структур `{ value: T, parser: Parser }` в `frontend/`. После generics на record — одна. Зависит от полных generics.

### Phase 2.6: Структурный рефакторинг компилятора

**Цель**: перейти от процедурного стиля (свободные функции `fn f(ctx, node)`) к методам через `extend` и трейты. Сделать добавление новых форм AST явным на уровне системы типов.

**Контекст**: сейчас в 138 файлах компилятора только 22 использования `extend`. Основная масса кода — свободные функции. Каждый проход (names, mutations, infer, transform, codegen) содержит свой полный `match expression { ... }` на все ~25 форм AST — итого 4–5 копий одинаковой структуры.

1. **Visitor-паттерн через трейт** — `ExprVisitor<Result>` уже есть в `expr_visitor.mlc`. Перевести все проходы:
   ```
   // Сейчас — fn infer_expr(ctx, expr) с match внутри на 25 веток
   // После:
   extend InferPass : ExprVisitor<InferResult> {
     fn visit_call(self, ...) -> InferResult
     fn visit_match(self, ...) -> InferResult
     // компилятор скажет, если какой-то формы нет
   }
   extend CodegenPass : ExprVisitor<Shared<CppExpr>> { ... }
   extend NamesPass   : ExprVisitor<NameCheckResult>  { ... }
   ```
   Файлы: `checker/infer/infer.mlc`, `checker/names.mlc`, `checker/check_mutations.mlc`, `transform/transform.mlc`, `codegen/eval.mlc`.

2. **Методы на контексте** — перевести `fn gen_expr(context, expr)` в `extend CodegenContext { fn gen_expr(self, expr) }`. Уже начато в `context.mlc:129`, нужно распространить на весь `codegen/`.

3. **Методы на типах результатов** — `extend InferResult`, `extend NameCheckResult` уже частично сделаны. Дополнить: `.and_then`, `.map_type`, `.merge_errors`.

4. **Display-трейт для CppAST** — вместо `fn print_expr(expr) -> string` в `cpp_printer.mlc`:
   ```
   extend CppExpr : Display {
     fn display(self) -> string = match self { ... }
   }
   ```
   Это позволит `expr.display()` и использовать в string interpolation.

5. **Рефакторинг checker/transform** — разбить `transform/transform.mlc` (828 строк) по форме AST по аналогии с существующими `infer_call.mlc`, `infer_match.mlc`. Один файл = одна форма или группа форм.

6. **TypeRegistry → подструктуры** — разбить god-object (15+ map) на `FnIndex`, `AdtIndex`, `RecordIndex`. Каждый index — своя структура с методами.

7. **Устранение сокращений** — все переменные, параметры, поля и типы должны иметь полные имена. Текущий масштаб (~1000 вхождений):

   | Сокращение | Замена | Примечание |
   |------------|--------|------------|
   | `ctx` | `context` | переменные и типы |
   | `expr` / `SExpr` / `CppExpr` | `expression` / `SemanticExpression` / `CppExpression` | |
   | `decl` / `SDecl` / `CppDecl` | `declaration` / `SemanticDeclaration` / `CppDeclaration` | |
   | `pat` / `Pat` | `pattern` / `Pattern` | |
   | `typ` | `type_value` | `type` — ключевое слово, поэтому суффикс `_value` |
   | `st` | `state` | |
   | `tok` / `TKind` / `CppTKind` | `token` / `TokenKind` / `CppTokenKind` | |
   | `stmt` / `SStmt` / `CppStmt` | `statement` / `SemanticStatement` / `CppStatement` | |
   | `col` | `column` | |
   | `ident` | `identifier` | |
   | `src` | `source` | |
   | `arg` | `argument` | |
   | `pos` | `position` | |
   | `val` | `value` | |
   | `err` | `error` | |
   | `ret` | `return_value` | |

   Делать пофайлово, верифицировать selfhost diff после каждого файла.

8. **Кавычки строковых литералов** — единое правило:
   - Одинарные кавычки `'text'` — все строки без интерполяции, включая `'\n'`, `'\t'`, `'\0'`.
   - Backticks `` `Hello ${name}` `` — только при интерполяции.
   - Двойные кавычки — не используются.

   Сейчас: 69 файлов содержат ~1602 строки с двойными кавычками там, где интерполяция не нужна. Заменить пофайлово вместе с п.7.

**Верификация**: `compiler/build.sh` + selfhost diff пустой после каждого шага.
**Ссылка**: полный анализ → `docs/CODE_REVIEW_2026_06.md`.

### Phase 3: Инструментарий

**Цель**: сделать язык пригодным для использования другими.

1. **Форматтер** (`mlc fmt`): детерминированное форматирование .mlc файлов.
   - Основан на AST (уже есть parser).
   - Правила: отступы 2 пробела, выравнивание полей record, etc.

2. **LSP-сервер** (`mlc lsp`): Language Server Protocol.
   - Минимум: go-to-definition, hover types, диагностика.
   - Используется как clangd для C++.
   - Строится поверх checker (уже есть type inference).

3. **Playground** (веб): компиляция MLC в браузере.
   - mlcc → WASM (после WASM backend) или через сервер.
   - Показывает: MLC source | SemanticIR | C++ output | вывод программы.

4. **Пакетный менеджер**: минималистичный (только для демонстрации).
   - `mlc.toml` с зависимостями.
   - Fetch из git, compile, link.

### Phase 4: Self-hosting completeness

**Цель**: mlcc компилирует 100% собственного кода без Ruby.

- Сейчас: mlcc компилирует весь `compiler/`, E2E работает, но bootstrap через Ruby.
- Цель: `compiler/build.sh` с `MLCC_BOOTSTRAP=1` → mlcc_bootstrap через mlcc.
- Критерий: `mlcc_bootstrap compiler/main.mlc` → результат идентичен mlcc.

### Phase 5: Reddit / Community

**Цель**: убедительная демонстрация self-hosted компилятора.

---

## 6. Reddit презентация

### Что показать

1. **Self-hosting**: mlcc компилирует сам себя (~23K строк MLC кода).
2. **Скорость**: mlcc компилирует себя за X секунд. Ruby bootstrap: 19+ с на фибоначчи.
3. **Язык**: алгебраические типы, type inference, Result/Option, traits, lambdas.
4. **Runtime**: COW-семантика Array/Map, zero-overhead abstractions поверх C++20.
5. **Playground**: ссылка где можно попробовать.

### Демонстрации (конкретные, воспроизводимые)

```bash
# Self-hosting demo
time compiler/out/mlcc compiler/main.mlc   # компилирует точку входа компилятора

# Fibonacci benchmark
time ./fibonacci_mlc   # vs Python/Ruby/Java

# Алгебраические типы
type Shape = Circle(f64) | Rect(f64, f64)
fn area(shape: Shape) -> f64 = match shape {
  Circle(radius) => 3.14159 * radius * radius,
  Rect(width, height) => width * height
}
```

### Что не показывать на Reddit (пока)

- LSP (нет)
- Package manager (нет)
- Стандартная библиотека (минимальная)
- Документация языка (нет)

### Временная шкала для поста

- Зима 2025/26: первый пост (self-hosted анонс)
- После Phase 1: второй пост с диагностикой и тестами
- После Phase 2 + форматтер: третий пост "MLC has a formatter and clean C++ output"
- После LSP: "MLC has a language server"

---

## 7. Метрики качества (измеряемые)

| Метрика | Сейчас | Цель |
|---|---|---|
| E2E тесты | 6 | 20+ |
| Unit тесты | 447 (0 failed) | 600+ |
| Crashes на валидном вводе | ? | 0 |
| Crashes на невалидном вводе | ? | 0 (panic с сообщением) |
| Диагностики со span | частично | 100% |
| Codegen: строки vs CppAST | CppAST default; string bridges for edge cases | 0% string bridges |
| mlcc компилирует себя | да | да + детерминировано |
| Время компиляции mlcc собой | ? измерить | < 1 с |

---

## 8. Что НЕ делать сейчас

- Стандартная библиотека (только по запросу)
- Оптимизации (constant folding, inlining) — после MLC IR
- GC / borrow checker — не в roadmap
- Полный WASM backend — после CppAST printer
- Package manager — после Phase 3
- Переписать checker — он работает, трогать последним
