# MLC / MLCC Development Plan

## 1. Текущее состояние

### Архитектура mlcc (self-hosted)

```
Source → Lexer → Tokens → Parser → AST → Checker → SemanticIR → MIR → CppAST → emit → C++ source
```

Codegen строит **CppAST** (`compiler/cpp/cpp_ast.mlc`) и печатает через `cpp_printer.mlc`. String-шаблоны в `codegen/expr/expr.mlc` и `expr_fragment_codegen` остаются только для bootstrap edge cases (stmt/decl bridges). Tracks [TRACK_CPPEXPR.md](archive/tracks/TRACK_CPPEXPR.md) + [TRACK_CPPGEN.md](archive/tracks/TRACK_CPPGEN.md) **closed** (2026-05).

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
  → Lowering → [MLC MIR]               # in progress: `compiler/mir/` (TRACK_MIR)
  → C++ Backend → [CppAST]             # NEW: типизированное представление C++
  → Printer → C++ source               # простой pretty-printer
```

### MLC MIR ([TRACK_MIR](archive/tracks/TRACK_MIR.md) — closed, STEP=10)

Простой flat IR, вдохновлённый Rust MIR (`compiler/mir/`):
- Explicit временные переменные вместо вложенных выражений
- Явные блоки (`BasicBlock`) с терминаторами (`Jump`, `CondJump`, `Return`)
- Нет побочных эффектов, скрытых в операторах
- Позволяет: dead code elimination, constant folding, inlining

Типы, dump, structural verifier, lowering, `--dump-mir` — все 10 шагов done. Продолжение (VM/интерпретатор без g++) — [TRACK_MIR_VM_FULL](agent/TRACK_MIR_VM_FULL.md) (open, Epic 0–4 **done** STEP=12; Epic 5 **NOT authorized**. [TRACK_VM_TRAMPOLINE](archive/tracks/TRACK_VM_TRAMPOLINE.md) **closed**; [TRACK_VM_BLOCK_ID_COLLISION](archive/tracks/TRACK_VM_BLOCK_ID_COLLISION.md) **closed**; [TRACK_VM_LOWERING_GAPS](archive/tracks/TRACK_VM_LOWERING_GAPS.md) **closed**; [TRACK_CLI_STDIN](archive/tracks/TRACK_CLI_STDIN.md) **closed**; [TRACK_LANG_INT_OVERFLOW](archive/tracks/TRACK_LANG_INT_OVERFLOW.md) **closed**).

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
| **1** Стабилизация | **done** | [TRACK_PHASE1](archive/tracks/TRACK_PHASE1.md) closed |
| **2** C++ AST backend | **done** | TRACK_CPPEXPR + TRACK_CPPGEN closed |
| **2.5** Языковые улучшения | **done** | ParseResult family closed; [TRACK_PARSE_PROGRAM_RESULT](archive/tracks/TRACK_PARSE_PROGRAM_RESULT.md) **closed** (2026-06-19) |
| **2.6** Структурный рефакторинг | **partial** | ExprVisitor infer/transform/codegen done; [TRACK_CODE_QUALITY](archive/tracks/TRACK_CODE_QUALITY.md) **closed** (`36a6e8cc`); names/mutations/context → [TRACK_PHASE26_REMAINING](archive/tracks/TRACK_PHASE26_REMAINING.md) |
| **3** Инструментарий | **done** | Formatter → [TRACK_FORMATTER](archive/tracks/TRACK_FORMATTER.md) closed; LSP → [TRACK_LSP](archive/tracks/TRACK_LSP.md) closed (STEP=1-5) |
| **3.5** C++ header import (minimal) | **done** | [TRACK_CPP_HEADER_IMPORT](archive/tracks/TRACK_CPP_HEADER_IMPORT.md) — subset для `import "foo.h"` |
| **3.6** Full C++ header parser | **done** | [TRACK_CPP_PARSER_FULL](archive/tracks/TRACK_CPP_PARSER_FULL.md) closed (STEP=1-8) |
| **2.8** Compiler architecture | **done** | [TRACK_CLEAN_ARCHITECTURE](archive/tracks/TRACK_CLEAN_ARCHITECTURE.md) — IR layers, passes, verifiers (**1290/0**) |
| **2.9** Build speed | **done** | [TRACK_BUILD_SPEED](archive/tracks/TRACK_BUILD_SPEED.md) closed; [TRACK_BUILD_SPEED2](archive/tracks/TRACK_BUILD_SPEED2.md) closed; [TRACK_CLANG_MIGRATION](archive/tracks/TRACK_CLANG_MIGRATION.md) closed 2026-07-03; [TRACK_BUILD_SPEED3](archive/tracks/TRACK_BUILD_SPEED3.md) **closed** 2026-07-09 (CI ccache; ftime-trace → decision **c**: no extern-template/ninja; residual AST `std::variant` not in-scope). C++20 modules — out of scope. MIR_VM_FULL ≠ build speed. |
| **4** Self-host bootstrap | **done** | [TRACK_SELF_HOST_BOOTSTRAP](archive/tracks/TRACK_SELF_HOST_BOOTSTRAP.md) |
| **5** Reddit / demo | **done** | [TRACK_REDDIT_DEMO](archive/tracks/TRACK_REDDIT_DEMO.md) — closed |
| **6** Concurrency | **done** | [TRACK_CONCURRENCY](archive/tracks/TRACK_CONCURRENCY.md) — Channel, spawn, Arc, Mutex |
| **7** Language design audit (2026-07) | **partial** | [LANGUAGE_AUDIT_2026_07.md](LANGUAGE_AUDIT_2026_07.md); 7/8 треков closed (ARRAY_HOF, OR_PATTERNS, WEAK_SUGAR, CYCLE_LINT, RESULT_COMBINATORS, ORPHAN_RULE, [TRACK_LANG_CLOSURE_ESCAPE](archive/tracks/TRACK_LANG_CLOSURE_ESCAPE.md) **closed** 2026-07-09); [TRACK_LANG_REGION_ARENA](agent/TRACK_LANG_REGION_ARENA.md) open (гипотеза, дорогой прототип, низкий приоритет) |
| **8** Concurrency v2 (Send/Sync, structured concurrency) | **partial** | [CONCURRENCY_V2.md](CONCURRENCY_V2.md); V2/TASKSCOPE/ISOLATE **closed**; SPAWN_DOUBLE_EXEC **closed**; [TRACK_CONCURRENCY_RUBY_PARITY](archive/tracks/TRACK_CONCURRENCY_RUBY_PARITY.md) **closed** 2026-07-10. SUPERVISOR deferred. MVP: [TRACK_CONCURRENCY](archive/tracks/TRACK_CONCURRENCY.md) closed |
| **8a** `spawn do <tail-call> end` выполняет тело дважды (codegen) | **closed** | [TRACK_LANG_SPAWN_DOUBLE_EXEC](archive/tracks/TRACK_LANG_SPAWN_DOUBLE_EXEC.md) **closed** 2026-07-10 — `expr_spawn_body_statements`; e2e gate; self-host identical; regression 20/0 |
| **8b** `spawn`/`Mutex`/`Channel` только self-hosted; `Tcp` stdlib только Ruby | **closed** | [TRACK_CONCURRENCY_RUBY_PARITY](archive/tracks/TRACK_CONCURRENCY_RUBY_PARITY.md) **closed** 2026-07-10 — Decision C; `block_on`/`is_ready`; MLC.md matrix |
| **8c** Слить `Tcp` + `spawn` в одном компиляторе (многопоточный HTTP-сервер целиком на MLC) | **done** | [TRACK_PIPELINE_MERGE_TCP_SPAWN](archive/tracks/TRACK_PIPELINE_MERGE_TCP_SPAWN.md) **closed** 2026-07-10 (STEP=1–6: Decision A; bare `Tcp`; echo; Tcp+spawn e2e; docs; self-host identical; regression 20/0) |
| **9** FFI-слой (RawPointer, extern codegen, линковка, C function pointer) | **done** | [FFI_LAYER.md](FFI_LAYER.md); [TRACK_FFI_LAYER](archive/tracks/TRACK_FFI_LAYER.md) **closed** 2026-07-09 (STEP=1–8: RawPointer, extern fn/lib/type, C fptr, concurrency attrs; self-host diff identical; regression_gate 20/0). Deferred: `owned` return-marker, ASan drop smoke |
| **10** Text rendering (HarfBuzz+FreeType+OpenGL) | **done** | [TEXT_RENDERING.md](TEXT_RENDERING.md); [TRACK_TEXT_RENDERING](archive/tracks/TRACK_TEXT_RENDERING.md) **closed** 2026-07-10 (STEP=0–8; MAE ≤ 8.0/255) |
| **10a** Text rendering целиком на MLC + окно (фундамент GUI-фреймворка) | **done** | [TRACK_TEXT_RENDERING_NATIVE](archive/tracks/TRACK_TEXT_RENDERING_NATIVE.md) **closed** 2026-07-11 (STEP=1–8: GLFW+GL dispatch+GlRenderer+TextRenderer+demo; self-host identical; regression 20/0); docs §8 in [TEXT_RENDERING.md](TEXT_RENDERING.md) |
| **10b** GUI framework (layout/widgets/easing) | **done** | [TRACK_GUI_FRAMEWORK](archive/tracks/TRACK_GUI_FRAMEWORK.md) **closed** 2026-07-11 (STEP=0–6: IM layout/input/Button; `misc/gui/`; docs [GUI.md](GUI.md); smokes ok) |
| **11** Stdlib для backend-приложений (TCP/HTTP сервер, Postgres, crypto, WS, job queue) | **partial** | [STDLIB_BACKEND.md](STDLIB_BACKEND.md); NET/POSTGRES/CRYPTO/WEBSOCKET/JOB_QUEUE/ENV_LOGGING/VALIDATION **closed**. Active Driver: [TRACK_STDLIB_HTTP_MLC](agent/TRACK_STDLIB_HTTP_MLC.md) STEP=3 |
| **11a** HTTP-парсер/роутер доступны из MLC (сейчас C++-only) | **open, active** | [TRACK_STDLIB_HTTP_MLC](agent/TRACK_STDLIB_HTTP_MLC.md) **active** STEP=2 (Decision locked: `HttpServer` pure MLC) |
| **12** API-клиенты (derive Json, OpenAPI codegen) | **done** | [API_CLIENT.md](API_CLIENT.md); [TRACK_API_CLIENT](archive/tracks/TRACK_API_CLIENT.md) **closed** 2026-07-09 (STEP=1–6: Json sync, JsonError, record/sum derive Json Ruby+self-host, OpenAPI codegen MVP; self-host diff identical; regression_gate 20/0). Deferred: §8.4 mock `fetch` |
| **13a** MIR VM crash на >~1500 шагов (trampoline fix) | **done** | [TRACK_VM_TRAMPOLINE](archive/tracks/TRACK_VM_TRAMPOLINE.md) **closed** 2026-07-10 (STEP=1–5: trampoline host loop, corpus, 100k depth gate, re-bench, self-host diff identical, regression_gate 20/0) |
| **13a-2** MIR block-id collision на вложенном `if` (VM зависает) | **done** | [TRACK_VM_BLOCK_ID_COLLISION](archive/tracks/TRACK_VM_BLOCK_ID_COLLISION.md) **closed** 2026-07-10 (STEP=1–5: `else_block_step.state`; classify/deep gates; corpus; self-host identical; regression_gate 20/0) |
| **13a-3** VM lowering: unary операторы, `if` не в tail-позиции | **done** (2026-07-10) | [TRACK_VM_LOWERING_GAPS](archive/tracks/TRACK_VM_LOWERING_GAPS.md) — STEP=1–4 **closed** (`MirRvalueUnary`; if-as-statement; if-as-rvalue; verify-gate) |
| **13b** `mlcc --run` stdin (crash fix + `-` convention) | **done** (2026-07-10) | [TRACK_CLI_STDIN](archive/tracks/TRACK_CLI_STDIN.md) — STEP=1–5 **closed** (streambuf; `read_all`; `-` stdin; gate; usage; verify) |
| **13c** VM: массив/map только `i32` (не point-fix, value-model) | **done** (2026-07-11) | [TRACK_VM_TYPED_COLLECTIONS](archive/tracks/TRACK_VM_TYPED_COLLECTIONS.md) **closed** STEP=1–4; Critic OK (`cf613f0b`…`cdffcbf8`; gate 6/0) |
| **14** FFI safety contract | **open, низкий приоритет** | [TRACK_FFI_SAFETY](agent/TRACK_FFI_SAFETY.md) — `extern`/`RawPointer` unsafe без маркера; диагностики + документация, без нового codegen |
| **15** Debugging story (`#line` → `.mlc` в stack trace) | **open, низкий приоритет, research** | [TRACK_DEBUG_SOURCE_MAP](agent/TRACK_DEBUG_SOURCE_MAP.md) — поднять приоритет когда появится первый внешний проект на MLC |
| **16** Integer overflow semantics | **closed** | [TRACK_LANG_INT_OVERFLOW](archive/tracks/TRACK_LANG_INT_OVERFLOW.md) **closed** 2026-07-10 — signed debug-panic/release-UB; unsigned wrap; div0 panic; `mlc::int_arith` i32 |
| **17** `T!E` error-union sugar | **open, низкий приоритет, чистый сахар** | [TRACK_LANG_ERROR_UNION](agent/TRACK_LANG_ERROR_UNION.md) — desugar в `Result<T,E>`, без зависимостей |
| **18** Package manager (design) | **open, design-only, самый низкий приоритет** | [TRACK_PACKAGE_MANAGER](agent/TRACK_PACKAGE_MANAGER.md) — реализация не авторизована без отдельной команды |
| **19** Автоматическое обнаружение циклов в рантайме | **open, design-only, вероятный won't-do** | [TRACK_LANG_AUTO_CYCLE](agent/TRACK_LANG_AUTO_CYCLE.md) — одна design-сессия закрывает вопрос из §10, противоречит принципу "без GC" |
| **20** Стратегия «без hand-written C++» (FFI-shim/бизнес-логика → mlcc/MLC) | **open, высокий приоритет** | [FFI_LAYER.md](FFI_LAYER.md) §8 — граница: рантайм языка остаётся C++, FFI-адаптеры и дублирующая бизнес-логика на C++ убираются. 6 подчинённых треков ниже |
| **20a** Postgres/Crypto/Tcp — прямой `extern fn` вместо `.hpp`-shim | **open** | [TRACK_FFI_SHIM_MIGRATION](agent/TRACK_FFI_SHIM_MIGRATION.md) — инфраструктура уже закрыта (`FFI_LAYER` STEP=1-8), чистая замена; `tcp.hpp` добавлен 2026-07-11 (тот же handle-table паттерн, syscall'ы сами не убираются — libc) |
| **20b** MSDF (EDT/SDF) алгоритм — порт на MLC | **open** | [TRACK_TEXT_MSDF_TO_MLC](agent/TRACK_TEXT_MSDF_TO_MLC.md) — не биндинг, собственная математика |
| **20c** WebSocket framing/handshake — порт на MLC | **open, blocked on 11a** | [TRACK_STDLIB_WEBSOCKET_TO_MLC](agent/TRACK_STDLIB_WEBSOCKET_TO_MLC.md) — зависит от `HttpRequest`/`HttpResponse` на MLC |
| **20d** Env/Log/Validation — mlcc-пайплайн + порт логики | **open** | [TRACK_STDLIB_LOGIC_TO_MLC](agent/TRACK_STDLIB_LOGIC_TO_MLC.md) — сейчас только Ruby-тесты, нет `mlcc`-пути |
| **20e** GL-вызовы через GLAD2, без ручного C++ dispatch | **open** | [TRACK_GL_GLAD_MIGRATION](agent/TRACK_GL_GLAD_MIGRATION.md) — заменяет отменённые `FFI_POINTER_CAST`/`GL_LOADER_TO_MLC` (superseded 2026-07-11: GLAD2/epoxy резолвят function pointers сами, каст/таблица на MLC не нужны) |

**Приоритет очереди (строгий порядок + зависимости):**

```
PARSE_PROGRAM_RESULT → CODE_QUALITY → FORMATTER → PHASE26_REMAINING
  → SELF_HOST_BOOTSTRAP → LSP → CPP_HEADER_IMPORT (minimal)
  → CPP_PARSER_FULL → CLEAN_ARCHITECTURE → REDDIT_DEMO → CONCURRENCY
  → LANG_CLOSURE_ESCAPE (**closed** 2026-07-09, STEP=4 verify-gate)
  → CONCURRENCY_TEST_HARNESS T1-T4 (**done** 2026-07-09, sanitize CI)
  → CONCURRENCY_V2 STEP=1–6 (**closed** 2026-07-09)
  → CONCURRENCY_TASKSCOPE STEP=1–4 (**closed** 2026-07-09)
  → CONCURRENCY_TEST_HARNESS T5 (**done** 2026-07-09); T6 deferred
  → CONCURRENCY_ISOLATE STEP=1–4 (**closed** 2026-07-09)
  → FFI_LAYER STEP=1–8 (**closed** 2026-07-09: RawPointer, extern fn/lib/type,
    C fptr, concurrency attrs; self-host diff identical; regression_gate 20/0)
  → API_CLIENT STEP=1–6 (**closed** 2026-07-09: Json sync, JsonError, record/sum
    derive Json Ruby+self-host, OpenAPI codegen MVP; self-host diff identical;
    regression_gate 20/0; §8.4 mock fetch deferred)
  → MIR_VM_FULL Epic 0 STEP A–C (**done** 2026-07-09: variant ctor → `__mir_variant_new`);
    Epic 1 STEP=1 (**done** 2026-07-09: VmFieldI32/Bool/String flat slots);
    Epic 1 STEP=2 (**done** 2026-07-09: guards, PatternOr, nested ctors + VmFieldVariant);
    Epic 1 STEP=3 (**done** 2026-07-09: VmRecord + `__mir_record_new`/`__mir_record_field`);
    Epic 1 STEP=4 (**done** 2026-07-09: Option/Result corpus + Shared.new identity);
    Epic 2 STEP=5 (**done** 2026-07-09: bound/immediate lambda inline + captures);
    Epic 2 STEP=6 (**done** 2026-07-09: mutual recursion + MirParam defaults in VM);
    Epic 2 STEP=7 (**done** 2026-07-09: `?` → `__mir_result_is_err`/`ok_value`);
    Epic 3 STEP=8 (**done** 2026-07-09: collections corpus + map/string/array_index diff);
    Epic 3 STEP=9 (**done** 2026-07-09: for-loop + pop + string.contains);
    Epic 4 STEP=10 (**done** 2026-07-09: `run_single_file_vm_gate.sh` 18+diff; Tier B p1≡p2);
    Epic 4 STEP=11 (**done** 2026-07-09: `vm_multi` + export unwrap in MIR lower);
    Epic 4 STEP=12 (**done** 2026-07-10: `run_examples_vm_gate.sh` 28 programs; Epic 4 closed);
    **HARD STOP — Epic 5 (STEP 13-20) НЕ авторизован** без явной команды
    пользователя (2026-07-09); не открывать STEP=13
  → VM_TRAMPOLINE STEP=1–5 (**closed** 2026-07-10: trampoline host loop;
    corpus; 100k depth; re-bench 1k/2e6; self-host diff identical;
    regression_gate 20/0) — Epic 4 `--run` stability claim was false until this
  → VM_BLOCK_ID_COLLISION STEP=1–5 (**closed** 2026-07-10: `else_block_step.state`;
    classify/deep gates; corpus; self-host identical; regression_gate 20/0)
  → VM_LOWERING_GAPS STEP=1–4 (**closed** 2026-07-10: `MirRvalueUnary`;
    if-as-statement; if-as-rvalue shared local; self-host DIFF identical;
    regression_gate 20/0; VM corpus elif6/depth/examples28/single18/cpp_diff18)
  → CLI_STDIN STEP=1–5 (**closed** 2026-07-10: streambuf `file.hpp`;
    `read_all` builtin; `"-"` → stdin; `run_cli_stdin_gate.sh`; usage
    `<source.mlc|->`; self-host DIFF identical; regression_gate 20/0)
  → VM_TYPED_COLLECTIONS STEP=1–4 (open, 2026-07-10: `VmArrayValue`/
    `VmMapValue` hard-coded `[i32]`; design-step first — recursive `VmValue`
    in container breaks C++ codegen same as §5.2 variant-field issue;
    not a point-fix, do before further VM/Epic-5 work)
  → **TEXT_RENDERING STEP=1 (**done** 2026-07-10: §5.1 RawPointer+length view; §5.3 msdf_shim at STEP=7; §5.2 → STEP=5);
    STEP=2 (**done** 2026-07-10: freetype_shim + glyph smoke);
    STEP=3 (**done** 2026-07-10: harfbuzz_shim + TextShaper Cyrillic; string literal byte_size);
    STEP=5.1 (**done** 2026-07-10: GlyphAtlas shelf pack + smoke);
    STEP=5.2 (**done** 2026-07-10: GlyphCache LRU + smoke);
    STEP=5.3 (**done** 2026-07-10: text shim ASan/UBSan gate);
    STEP=5 (**done** 2026-07-10: §5.2 EGL `eglGetProcAddress` shim + smoke + CI);
    STEP=6.1 (**done** 2026-07-10: TextRenderer A8 FBO + MAE vs CPU blit tol 8);
    STEP=6.2 (**done** 2026-07-10: shaped «Привет» + golden `text_a8_privet_24.rgba`);
    STEP=7.1 (**done** 2026-07-10: msdf_shim SDF→RGB8 + numerical smoke);
    STEP=7.2 (**done** 2026-07-10: MSDF median FBO + RGB8 atlas pack);
    STEP=7.3 (**done** 2026-07-10: RenderMode A8/MSDF threshold 28);
    STEP=8 (**done** 2026-07-10: self-host identical; regression 20/0; MAE≤8;
      track **closed** → archive)
  → CONCURRENCY_SUPERVISOR (deferred; after chat-server gate)
  → LANG_REGION_ARENA (ЗАБЛОКИРОВАН — 3 design-вопроса в самом треке не решены,
    не начинать реализацию, максимум — отдельный design-turn)
  → LANG_INT_OVERFLOW (**closed** 2026-07-10: signed debug-panic/release-UB;
      unsigned wrap; div0 panic; `mlc::int_arith` i32; tests + DIFF + regression 20/0)
  → STDLIB_NET_SERVER (**closed** 2026-07-10: TCP+HTTP parse/router/ThreadPool;
      `std/net/tcp`; `misc/examples/tcp_echo_demo.mlc`; regression 20/0
      → [archive/tracks/TRACK_STDLIB_NET_SERVER.md](archive/tracks/TRACK_STDLIB_NET_SERVER.md))
  → LANG_SPAWN_DOUBLE_EXEC (**closed** 2026-07-10: `expr_spawn_body_statements`;
      e2e `run_spawn_side_effect_gate.sh`; self-host p1≡p2; regression 20/0
      → [archive/tracks/TRACK_LANG_SPAWN_DOUBLE_EXEC.md](archive/tracks/TRACK_LANG_SPAWN_DOUBLE_EXEC.md))
  → CONCURRENCY_RUBY_PARITY (**closed** 2026-07-10: Decision C; `block_on`/`is_ready`;
      MLC.md § «Два пайплайна» + README
      → [archive/tracks/TRACK_CONCURRENCY_RUBY_PARITY.md](archive/tracks/TRACK_CONCURRENCY_RUBY_PARITY.md))
  → **PIPELINE_MERGE_TCP_SPAWN (**closed** 2026-07-10: Tcp into mlcc + spawn e2e;
      self-host identical; regression 20/0
      → [archive/tracks/TRACK_PIPELINE_MERGE_TCP_SPAWN.md](archive/tracks/TRACK_PIPELINE_MERGE_TCP_SPAWN.md))**
  → STDLIB_POSTGRES (**closed** 2026-07-10: `mlc::db` + `std/db/postgres`;
      gate `run_postgres_gate.sh`; example `postgres_select_demo.mlc`
      → [archive/tracks/TRACK_STDLIB_POSTGRES.md](archive/tracks/TRACK_STDLIB_POSTGRES.md))
  → STDLIB_CRYPTO (**closed** 2026-07-10: `mlc::crypto` + `std/crypto/crypto`;
      gate `run_crypto_gate.sh`; example `crypto_sha256_demo.mlc`
      → [archive/tracks/TRACK_STDLIB_CRYPTO.md](archive/tracks/TRACK_STDLIB_CRYPTO.md))
  → STDLIB_WEBSOCKET (**closed** 2026-07-11: `mlc::websocket` + `std/net/websocket`;
      gate `run_websocket_gate.sh`; example `websocket_echo_demo.mlc`
      → [archive/tracks/TRACK_STDLIB_WEBSOCKET.md](archive/tracks/TRACK_STDLIB_WEBSOCKET.md))
  → STDLIB_JOB_QUEUE (**closed** 2026-07-11: `mlc::concurrency::JobQueue`;
      gate `run_job_queue_gate.sh`; example `job_queue_demo.cpp`; C++-only
      → [archive/tracks/TRACK_STDLIB_JOB_QUEUE.md](archive/tracks/TRACK_STDLIB_JOB_QUEUE.md))
  → STDLIB_ENV_LOGGING (**closed** 2026-07-11: `mlc::env`/`mlc::log` + MLC
      `Env`/`Log`; gate `run_env_log_gate.sh`; example `env_log_demo.mlc`
      → [archive/tracks/TRACK_STDLIB_ENV_LOGGING.md](archive/tracks/TRACK_STDLIB_ENV_LOGGING.md))
  → STDLIB_VALIDATION (**closed** 2026-07-11: `mlc::validate` + MLC `Validate`;
      gate `run_validate_gate.sh`; example `validate_demo.mlc`
      → [archive/tracks/TRACK_STDLIB_VALIDATION.md](archive/tracks/TRACK_STDLIB_VALIDATION.md))
  → TEXT_RENDERING_NATIVE (**closed** 2026-07-11: self-host identical; regression 20/0;
      demo+smokes ok
      → [archive/tracks/TRACK_TEXT_RENDERING_NATIVE.md](archive/tracks/TRACK_TEXT_RENDERING_NATIVE.md))
  → GUI_FRAMEWORK (**closed** 2026-07-11: IM Button v0; `misc/gui/`; GUI.md;
      layout/input/button smokes ok
      → [archive/tracks/TRACK_GUI_FRAMEWORK.md](archive/tracks/TRACK_GUI_FRAMEWORK.md))
  → VM_TYPED_COLLECTIONS (**closed** 2026-07-11: Critic OK; self-host identical;
      regression 20/0; typed/nested smokes
      → [archive/tracks/TRACK_VM_TYPED_COLLECTIONS.md](archive/tracks/TRACK_VM_TYPED_COLLECTIONS.md))
  → STDLIB_HTTP_MLC (**active** STEP=2 — MLC parser/records after Decision
      → [agent/TRACK_STDLIB_HTTP_MLC.md](agent/TRACK_STDLIB_HTTP_MLC.md))
  → **«без hand-written C++» инициатива (2026-07-11, высокий приоритет,
    после STDLIB_HTTP_MLC, порядок между собой не строгий, независимые
    треки):**
  → FFI_SHIM_MIGRATION (open: Postgres/Crypto/Tcp прямой `extern fn`, инфра
      уже закрыта → [agent/TRACK_FFI_SHIM_MIGRATION.md](agent/TRACK_FFI_SHIM_MIGRATION.md))
  → TEXT_MSDF_TO_MLC (open: EDT/SDF алгоритм на MLC
      → [agent/TRACK_TEXT_MSDF_TO_MLC.md](agent/TRACK_TEXT_MSDF_TO_MLC.md))
  → STDLIB_LOGIC_TO_MLC (open: Env/Log/Validation на mlcc + порт логики
      → [agent/TRACK_STDLIB_LOGIC_TO_MLC.md](agent/TRACK_STDLIB_LOGIC_TO_MLC.md))
  → STDLIB_WEBSOCKET_TO_MLC (blocked on STDLIB_HTTP_MLC closed
      → [agent/TRACK_STDLIB_WEBSOCKET_TO_MLC.md](agent/TRACK_STDLIB_WEBSOCKET_TO_MLC.md))
  → GL_GLAD_MIGRATION (open: GLAD2 вендоренный вместо ручного
      `glfw_gl_dispatch.cpp`/`loader_shim.cpp`; заменяет отменённые
      FFI_POINTER_CAST/GL_LOADER_TO_MLC (superseded 2026-07-11)
      → [agent/TRACK_GL_GLAD_MIGRATION.md](agent/TRACK_GL_GLAD_MIGRATION.md))
  → FFI_SAFETY / LANG_ERROR_UNION / DEBUG_SOURCE_MAP (низкий приоритет,
    без зависимостей друг от друга)
  → PACKAGE_MANAGER / LANG_AUTO_CYCLE (design-only, не начинать реализацию
    без отдельной команды пользователя)
```

Качество кода (деструктуризация, HOF, string-match) — до форматтера; форматтер — до LSP; self-host bootstrap — до community demo.

---

### Phase 1: Стабилизация — **done**

**Цель**: надёжный mlcc без регрессий. Закрыто: диагностика, fuzz, `--check-only`, crash-safety, out-dir, build determinism. См. [TRACK_PHASE1](archive/tracks/TRACK_PHASE1.md).

### Phase 2: C++ AST backend — **done**

**Статус:** TRACK_CPPEXPR + TRACK_CPPGEN closed. Codegen строит CppAST; string-bridge только bootstrap edge cases.

### Phase 2.5: Языковые улучшения — **done**

**Статус:** string match, type aliases, generics, ParseResult migration — closed (`bf6c46e8`, `ProgramParseValue` 2026-06-19).

**Отложено:** parser `ref mut` (отдельная ветка); regex match-паттерны.

### Phase 2.6: Структурный рефакторинг — **partial**

**Сделано:** folder restructure; ExprVisitor для infer/transform/codegen ([TRACK_VISITOR_PATTERN](archive/tracks/TRACK_VISITOR_PATTERN.md) closed).

**Остаток:** names + mutations + transform passes на ExprVisitor; методы на CodegenContext → [TRACK_PHASE26_REMAINING](archive/tracks/TRACK_PHASE26_REMAINING.md). См. [CODE_REVIEW_2026_06.md](CODE_REVIEW_2026_06.md).

### Phase 3: Инструментарий — **planned**

1. **Форматтер** — [TRACK_FORMATTER](archive/tracks/TRACK_FORMATTER.md) (зависит от CODE_QUALITY audit).
2. **LSP** — [TRACK_LSP](archive/tracks/TRACK_LSP.md) (зависит от formatter + стабильных диагностик).
3. Playground / package manager — после LSP.

### Phase 2.7: Языковые улучшения II — **planned**

Scala-inspired фичи с высоким ROI для читаемости и корректности:

1. **`?`-оператор / for-comprehensions на Result/Option** — цепочки `Result` без вложенных `match`. Аналог Rust `?` или Scala `for { x <- ... } yield`. Зависит: Phase 2.6 done.
2. **Opaque types (newtype)** — `opaque type NodeId = i32`. Нулевые накладные расходы; компилятор различает `NodeId`, `Span`, `i32`. Зависит: Phase 2.6 done.
3. **`lazy val`** — отложенное вычисление с мемоизацией. Полезно в checker для циклических зависимостей типов. Зависит: Phase 2.7 п.1.

Треки: создать при старте фазы.

### Phase 3.5: C++ Header Import (minimal) — **done**

Subset-парсер для `import "foo.h"`: include, using, struct, fn proto, enum, namespace.
Трек: [TRACK_CPP_HEADER_IMPORT](archive/tracks/TRACK_CPP_HEADER_IMPORT.md). Известные пробелы → Phase 3.6.

### Phase 3.6: Full C++ Header Parser — **planned**

**Цель:** полноценный парсер заголовков с trivia roundtrip, expressions, class bodies, template decls — паритет с Ruby `lib/cpp_ast/` (~11k LOC). Нужен для реальных STL/third-party headers.

Дизайн: [CPP_PARSER_DESIGN.md](CPP_PARSER_DESIGN.md). Трек: [TRACK_CPP_PARSER_FULL](archive/tracks/TRACK_CPP_PARSER_FULL.md).

**Зависит от:** Phase 3.5 (minimal subset), LSP STEP=5.

**Не входит:** C++20 modules/coroutines, вычисление `#ifdef`/`#define`.

### Phase 2.8: Compiler architecture (IR + passes) — **done**

**Цель:** границы IR (AST → Typed → CoreIR → CppAST), verified passes, pass manager, driver/core split, dump/trace, test pyramid. Не enterprise-слои.

Документ: [ARCHITECTURE.md](ARCHITECTURE.md). Трек: [TRACK_CLEAN_ARCHITECTURE](archive/tracks/TRACK_CLEAN_ARCHITECTURE.md) — **closed** (2026-05-19).

**Зависит от:** CPP_PARSER_FULL STEP=1, LSP closed.

**Язык:** Phase 2.7 opaque IDs (`NodeId`, `TypeId`) — параллельно, до CoreIR sketch.

**Future:** TRACK_CORE_IR, [QUERY_ENGINE.md](QUERY_ENGINE.md) / TRACK_QUERY_ENGINE, TRACK_INCREMENTAL.

### Phase 2.9: Build speed — **done**

**Цель:** убрать bottleneck g++ link (90–200s): persistent obj, ccache, mold/lld, dev `-O0`. mlcc codegen ~2s — не трогать.

Треки: [TRACK_BUILD_SPEED](archive/tracks/TRACK_BUILD_SPEED.md),
[TRACK_BUILD_SPEED2](archive/tracks/TRACK_BUILD_SPEED2.md),
[TRACK_CLANG_MIGRATION](archive/tracks/TRACK_CLANG_MIGRATION.md),
[TRACK_BUILD_SPEED3](archive/tracks/TRACK_BUILD_SPEED3.md) — все **closed**.
clang++ — дефолт (`compiler/scripts/select_cxx.sh`). BUILD_SPEED3: CI ccache +
ftime-trace after CLOSURE_ESCAPE → decision **c** (no Shared/Array/`std::function`
`extern template`; no ninja). **Не путать** с [TRACK_MIR_VM_FULL](agent/TRACK_MIR_VM_FULL.md)
(интерпретация без g++, не скорость *сборки*). C++20 modules — вне скоупа.

### Phase 4: Self-hosting completeness — **done** ([TRACK_BOOTSTRAP_LINK](archive/tracks/TRACK_BOOTSTRAP_LINK.md) closed 2026-07-03)

**Цель:** `compiler/build.sh` без Ruby. Трек: [TRACK_SELF_HOST_BOOTSTRAP](archive/tracks/TRACK_SELF_HOST_BOOTSTRAP.md). Bootstrap link regression (open 2026-06-26) closed 2026-07-03: `MLCC_BOOTSTRAP=1` builds, `run_mlcc_bootstrap_parity.sh` green, 144/144 clang `-fsyntax-only`, `mlcc`→`mlcc2` identity diff clean.

### Phase 5: Reddit / Community — **done** ([TRACK_REDDIT_DEMO](archive/tracks/TRACK_REDDIT_DEMO.md) closed)

Трек закрыт: demo script, baseline, media checklist, BLOG draft. Публикация поста — manual (`docs/archive/tracks/REDDIT_DEMO_MEDIA.md`).

### Phase 6: Concurrency — **done** ([TRACK_CONCURRENCY](archive/tracks/TRACK_CONCURRENCY.md) closed)

**Качество кода (после 2.5):** [TRACK_CODE_QUALITY](archive/tracks/TRACK_CODE_QUALITY.md) — **closed** (`36a6e8cc`): деструктуризация, HOF, or-patterns, string-match, audit.

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

## 9. Orchestrator / Multi-agent System (discontinued)

Отдельный `cr` orchestrator (agent-loop MCP, tokens, CDP, watchdog, overnight guard) — заброшен. Текущий подход: обычная очередь сообщений Cursor, без внешнего сервера/MCP-роутинга. Архив: `docs/archive/TRACK_ORCH_DEV.md`, `docs/archive/CONTINUITY_AGENT_LOOP_MCP.md`.

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
