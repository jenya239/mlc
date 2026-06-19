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

**Сводка (2026-06-19, replan sync):**

| Фаза | Статус | Трек / примечание |
|------|--------|-------------------|
| **1** Стабилизация | **done** | [TRACK_PHASE1](agent/TRACK_PHASE1.md) closed |
| **2** C++ AST backend | **done** | TRACK_CPPEXPR + TRACK_CPPGEN closed |
| **2.5** Языковые улучшения | **nearly done** | ParseResult family → [TRACK_PARSE_PROGRAM_RESULT](agent/TRACK_PARSE_PROGRAM_RESULT.md) **open** |
| **2.6** Структурный рефакторинг | **partial** | ExprVisitor infer/transform/codegen done; [TRACK_CODE_QUALITY](agent/TRACK_CODE_QUALITY.md) **closed** (`36a6e8cc`); names/mutations/context → [TRACK_PHASE26_REMAINING](agent/TRACK_PHASE26_REMAINING.md) |
| **3** Инструментарий | **planned** | Formatter → [TRACK_FORMATTER](agent/TRACK_FORMATTER.md); LSP → [TRACK_LSP](agent/TRACK_LSP.md) |
| **3.5** C++ header import | **planned** | [TRACK_CPP_HEADER_IMPORT](agent/TRACK_CPP_HEADER_IMPORT.md); дизайн: [CPP_PARSER_DESIGN.md](CPP_PARSER_DESIGN.md) |
| **4** Self-host bootstrap | **planned** | [TRACK_SELF_HOST_BOOTSTRAP](agent/TRACK_SELF_HOST_BOOTSTRAP.md) — убрать Ruby из `build.sh` |
| **5** Reddit / demo | **planned** | [TRACK_REDDIT_DEMO](agent/TRACK_REDDIT_DEMO.md) |

**Приоритет очереди (строгий порядок + зависимости):**

```
PARSE_PROGRAM_RESULT → CODE_QUALITY → FORMATTER → PHASE26_REMAINING
  → SELF_HOST_BOOTSTRAP → LSP → CPP_HEADER_IMPORT → REDDIT_DEMO
```

Качество кода (деструктуризация, HOF, string-match) — до форматтера; форматтер — до LSP; self-host bootstrap — до community demo.

---

### Phase 1: Стабилизация — **done**

**Цель**: надёжный mlcc без регрессий. Закрыто: диагностика, fuzz, `--check-only`, crash-safety, out-dir, build determinism. См. [TRACK_PHASE1](agent/TRACK_PHASE1.md).

### Phase 2: C++ AST backend — **done**

**Статус:** TRACK_CPPEXPR + TRACK_CPPGEN closed. Codegen строит CppAST; string-bridge только bootstrap edge cases.

### Phase 2.5: Языковые улучшения — **nearly done**

**Статус:** string match, type aliases, generics, ParseResult migration (ExprResult, TypeParamsResult) — closed (`bf6c46e8`). Остаток: `ProgramParseValue` / `parse_program_with_errors` → [TRACK_PARSE_PROGRAM_RESULT](agent/TRACK_PARSE_PROGRAM_RESULT.md).

**Отложено:** parser `ref mut` (отдельная ветка); regex match-паттерны.

### Phase 2.6: Структурный рефакторинг — **partial**

**Сделано:** folder restructure; ExprVisitor для infer/transform/codegen ([TRACK_VISITOR_PATTERN](agent/TRACK_VISITOR_PATTERN.md) closed).

**Остаток:** names + mutations + transform passes на ExprVisitor; методы на CodegenContext → [TRACK_PHASE26_REMAINING](agent/TRACK_PHASE26_REMAINING.md). См. [CODE_REVIEW_2026_06.md](CODE_REVIEW_2026_06.md).

### Phase 3: Инструментарий — **planned**

1. **Форматтер** — [TRACK_FORMATTER](agent/TRACK_FORMATTER.md) (зависит от CODE_QUALITY audit).
2. **LSP** — [TRACK_LSP](agent/TRACK_LSP.md) (зависит от formatter + стабильных диагностик).
3. Playground / package manager — после LSP.

### Phase 3.5: C++ Header Import — **planned**

Дизайн: [CPP_PARSER_DESIGN.md](CPP_PARSER_DESIGN.md). Трек: [TRACK_CPP_HEADER_IMPORT](agent/TRACK_CPP_HEADER_IMPORT.md).

### Phase 4: Self-hosting completeness — **planned**

**Цель:** `compiler/build.sh` без Ruby. Трек: [TRACK_SELF_HOST_BOOTSTRAP](agent/TRACK_SELF_HOST_BOOTSTRAP.md).

### Phase 5: Reddit / Community — **planned**

Трек: [TRACK_REDDIT_DEMO](agent/TRACK_REDDIT_DEMO.md). После self-host bootstrap + форматтер.

**Качество кода (после 2.5):** [TRACK_CODE_QUALITY](agent/TRACK_CODE_QUALITY.md) — **closed** (`36a6e8cc`): деструктуризация, HOF, or-patterns, string-match, audit.

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

## 9. Orchestrator / Multi-agent System

**Параллельный трек** — развивается вместе с MLC, не блокирует язык. Ресурсы: ~20% Driver-ходов (каждый 5-й ход OrchestratorDev, см. ROLES.md).

### Текущее состояние (cr)

- `cr` server: watchdog, CDP, SSE, web UI (progress, tracks, commits, agent turns)
- Агент работает в Cursor через MCP `user-cr-cursor`
- Overnight loop: supervisor + guard каждые 5 минут
- Роли: Driver, Planner, Critic, Cleaner, Backlog, Meta, Orchestrator, Researcher, Reviewer, Blogger

### Pending (трек: TRACK_ORCH_DEV.md)

| # | Item | Status |
|---|------|--------|
| 1 | Billing: SQLite `cost_entries` + дашборд в web UI | pending |
| 2 | Meeting rooms: `docs/agent/meetings/`, MEETING_PROTOCOL.md, render в progress | pending |
| 3 | Terminal agents: tmux transport (аналог CDP для non-Cursor агентов) | pending |
| 4 | Multi-agent: общий `AGENT_TARGETS` для Cursor + tmux агентов | pending |
| 5 | Session DB: SQLite кэш для SESSION.md (быстрый query вместо парсинга) | pending |

### Архитектура

**Транспорт агентов:**
- Cursor agents → CDP (существующий путь через `user-cr-cursor` MCP)
- Terminal agents (Claude в терминале, скрипты) → **tmux**: `send-keys` для отправки, `capture-pane` для чтения

**Гибридное хранилище:**

| Слой | Хранилище | Что лежит |
|------|-----------|-----------|
| Инструкции, планы, треки | markdown + git | PLAN.md, TRACK_*.md, ROLES.md, SESSION.md — agent-native, история в git |
| Оперативные данные | **SQLite** (`cr/db/cr.db`, `better-sqlite3`) | turns, cost_entries, agent_states, meetings |

DB можно дропнуть и пересобрать из markdown-файлов — это кэш, не источник истины.

### Роль OrchestratorDev

Выделенная роль для работ по `cr/` и оркестрации.  
STEP = `orch-dev-N` (берёт шаги из TRACK_ORCH_DEV.md).  
Частота: каждые 5 Driver-ходов mlc — 1 ход OrchestratorDev.  
Касается только `cr/` workspace, не `compiler/` / `lib/mlc/`.

---

## 10. Заимствования из других языков

Фильтр: только то что транслируется в C++ без введения отдельного рантайма или интерпретатора.

### Nim — C++ interop (приоритет)

Nim имеет `{.importcpp.}` для вызова C++ кода без биндингов. В mlc нет никакого способа вызвать C++ библиотеку из mlc-кода — это принципиальное ограничение для реального использования.

Предлагаемый синтаксис:
```mlc
extern fn sqrt(x: f64) -> f64 = "sqrt" from "<cmath>"
extern type FILE = "FILE" from "<stdio.h>"
```

Транслируется в `#include <cmath>` + вызов напрямую. Без промежуточных биндингов.

### Nim — ARC cycle detection (долгосрочно)

Nim ORC автоматически детектирует циклы в reference-counted структурах. В mlc циклы требуют ручного `Weak<T>`. Автоматическое обнаружение — отдельный pass в кодогенераторе. Не в текущем roadmap, но стоит учитывать при проектировании IR.

### Zig — Error unions (применимо)

Zig использует `T!E` вместо `Result<T, E>`. Транслируется в `std::expected<T,E>` (C++23) или `std::variant<T,E>`.

```mlc
fn divide(a: i32, b: i32) -> i32!DivError = ...
```

Как сахар над существующим `Result<T,E>` — минимальные изменения в checker и codegen.

---

## 8. Что НЕ делать сейчас

- Стандартная библиотека (только по запросу)
- Оптимизации (constant folding, inlining) — после MLC IR
- GC / borrow checker — не в roadmap
- Полный WASM backend — после CppAST printer
- Package manager — после Phase 3
- Переписать checker — он работает, трогать последним
