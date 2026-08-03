# MLC / MLCC Development Plan

## 1. Текущее состояние

### Архитектура mlcc (self-hosted)

```
Source → Lexer → Tokens → Parser → AST → Checker → SemanticIR → MIR → CppAST → emit → C++ source
```

Codegen строит **CppAST** (`compiler/cpp_ir/cpp_ast.mlc`) и печатает через `cpp_emit/print.mlc`. Модуль `codegen/expr/expr.mlc` удалён. `GenStmtsResult` / `GenModuleOut` — AST lists; residual: `CppStatementFragment` + print-at-call-site helpers (не 0% string bridges). Tracks [TRACK_CPPEXPR.md](archive/tracks/TRACK_CPPEXPR.md) + [TRACK_CPPGEN.md](archive/tracks/TRACK_CPPGEN.md) **closed** (2026-05); [TRACK_CODEGEN_CPPAST_ONLY](archive/tracks/TRACK_CODEGEN_CPPAST_ONLY.md) **closed** 2026-07-17 (Critic OK).

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
| **7** Language design audit (2026-07) | **done** | [LANGUAGE_AUDIT_2026_07.md](LANGUAGE_AUDIT_2026_07.md); 8/8 треков closed; [TRACK_LANG_REGION_ARENA](archive/tracks/TRACK_LANG_REGION_ARENA.md) **closed** Critic OK (`3b4df688`…`9fc9c890`; STEP=10 skipped) |
| **8** Concurrency v2 (Send/Sync, structured concurrency) | **partial** | [CONCURRENCY_V2.md](CONCURRENCY_V2.md); V2/TASKSCOPE/ISOLATE **closed**; SPAWN_DOUBLE_EXEC **closed**; [TRACK_CONCURRENCY_RUBY_PARITY](archive/tracks/TRACK_CONCURRENCY_RUBY_PARITY.md) **closed** 2026-07-10. [TRACK_CONCURRENCY_SUPERVISOR](archive/tracks/TRACK_CONCURRENCY_SUPERVISOR.md) **closed** Critic OK 2026-07-12. MVP: [TRACK_CONCURRENCY](archive/tracks/TRACK_CONCURRENCY.md) closed |
| **8a** `spawn do <tail-call> end` выполняет тело дважды (codegen) | **closed** | [TRACK_LANG_SPAWN_DOUBLE_EXEC](archive/tracks/TRACK_LANG_SPAWN_DOUBLE_EXEC.md) **closed** 2026-07-10 — `expr_spawn_body_statements`; e2e gate; self-host identical; regression 20/0 |
| **8b** `spawn`/`Mutex`/`Channel` только self-hosted; `Tcp` stdlib только Ruby | **closed** | [TRACK_CONCURRENCY_RUBY_PARITY](archive/tracks/TRACK_CONCURRENCY_RUBY_PARITY.md) **closed** 2026-07-10 — Decision C; `block_on`/`is_ready`; MLC.md matrix |
| **8c** Слить `Tcp` + `spawn` в одном компиляторе (многопоточный HTTP-сервер целиком на MLC) | **done** | [TRACK_PIPELINE_MERGE_TCP_SPAWN](archive/tracks/TRACK_PIPELINE_MERGE_TCP_SPAWN.md) **closed** 2026-07-10 (STEP=1–6: Decision A; bare `Tcp`; echo; Tcp+spawn e2e; docs; self-host identical; regression 20/0) |
| **9** FFI-слой (RawPointer, extern codegen, линковка, C function pointer) | **done** | [FFI_LAYER.md](FFI_LAYER.md); [TRACK_FFI_LAYER](archive/tracks/TRACK_FFI_LAYER.md) **closed** 2026-07-09 (STEP=1–8: RawPointer, extern fn/lib/type, C fptr, concurrency attrs; self-host diff identical; regression_gate 20/0). Deferred: `owned` return-marker, ASan drop smoke |
| **10** Text rendering (HarfBuzz+FreeType+OpenGL) | **done** | [TEXT_RENDERING.md](TEXT_RENDERING.md); [TRACK_TEXT_RENDERING](archive/tracks/TRACK_TEXT_RENDERING.md) **closed** 2026-07-10 (STEP=0–8; MAE ≤ 8.0/255) |
| **10a** Text rendering целиком на MLC + окно (фундамент GUI-фреймворка) | **done** | [TRACK_TEXT_RENDERING_NATIVE](archive/tracks/TRACK_TEXT_RENDERING_NATIVE.md) **closed** 2026-07-11 (STEP=1–8: GLFW+GL dispatch+GlRenderer+TextRenderer+demo; self-host identical; regression 20/0); docs §8 in [TEXT_RENDERING.md](TEXT_RENDERING.md) |
| **10b** GUI framework (layout/widgets/easing) | **done** | [TRACK_GUI_FRAMEWORK](archive/tracks/TRACK_GUI_FRAMEWORK.md) **closed** 2026-07-11 (STEP=0–6: IM layout/input/Button; `misc/gui/`; docs [GUI.md](GUI.md); smokes ok) |
| **11** Stdlib для backend-приложений (TCP/HTTP сервер, Postgres, crypto, WS, job queue) | **partial** | [STDLIB_BACKEND.md](STDLIB_BACKEND.md); NET/…/GL_GLAD **closed**. §20 initiative **done**. §14 FFI_SAFETY **closed** |
| **11a** HTTP-парсер/роутер доступны из MLC (сейчас C++-only) | **done** (2026-07-11) | [TRACK_STDLIB_HTTP_MLC](archive/tracks/TRACK_STDLIB_HTTP_MLC.md) **closed** STEP=1–7; Critic OK (`2fdc8c83`…`34977011`; parse+curl EXIT 0). Residual: no `[HttpRoute]` API |
| **11b** `spawn` fire-and-forget блокирует (Task-деструктор ждёт `std::future`) — реального многопоточного сервера сегодня нет | **done** (2026-07-11) | [TRACK_CONCURRENCY_SPAWN_DETACH](archive/tracks/TRACK_CONCURRENCY_SPAWN_DETACH.md) **closed** STEP=1–5; Critic OK; E089+`scope`→TaskScope; parallel sleep+accept-loop curl; REG 20/0; self-host identical |
| **10c** Retained scene-graph фундамент (classic UI + game UI + Flash-rich + Figma/blueprint canvas — один фреймворк) | **done** (2026-07-16) Phase A–D Critic OK | [archive/tracks/TRACK_GUI_CANVAS_GRAPH](archive/tracks/TRACK_GUI_CANVAS_GRAPH.md); Phase A–D → [archive/tracks/](archive/tracks/); `d9cc60de`…`9e3eaebf` (Phase D) |
| **12** API-клиенты (derive Json, OpenAPI codegen) | **done** | [API_CLIENT.md](API_CLIENT.md); [TRACK_API_CLIENT](archive/tracks/TRACK_API_CLIENT.md) **closed** 2026-07-09 (STEP=1–6: Json sync, JsonError, record/sum derive Json Ruby+self-host, OpenAPI codegen MVP; self-host diff identical; regression_gate 20/0). Deferred: §8.4 mock `fetch` |
| **13a** MIR VM crash на >~1500 шагов (trampoline fix) | **done** | [TRACK_VM_TRAMPOLINE](archive/tracks/TRACK_VM_TRAMPOLINE.md) **closed** 2026-07-10 (STEP=1–5: trampoline host loop, corpus, 100k depth gate, re-bench, self-host diff identical, regression_gate 20/0) |
| **13a-2** MIR block-id collision на вложенном `if` (VM зависает) | **done** | [TRACK_VM_BLOCK_ID_COLLISION](archive/tracks/TRACK_VM_BLOCK_ID_COLLISION.md) **closed** 2026-07-10 (STEP=1–5: `else_block_step.state`; classify/deep gates; corpus; self-host identical; regression_gate 20/0) |
| **13a-3** VM lowering: unary операторы, `if` не в tail-позиции | **done** (2026-07-10) | [TRACK_VM_LOWERING_GAPS](archive/tracks/TRACK_VM_LOWERING_GAPS.md) — STEP=1–4 **closed** (`MirRvalueUnary`; if-as-statement; if-as-rvalue; verify-gate) |
| **13b** `mlcc --run` stdin (crash fix + `-` convention) | **done** (2026-07-10) | [TRACK_CLI_STDIN](archive/tracks/TRACK_CLI_STDIN.md) — STEP=1–5 **closed** (streambuf; `read_all`; `-` stdin; gate; usage; verify) |
| **13c** VM: массив/map только `i32` (не point-fix, value-model) | **done** (2026-07-11) | [TRACK_VM_TYPED_COLLECTIONS](archive/tracks/TRACK_VM_TYPED_COLLECTIONS.md) **closed** STEP=1–4; Critic OK (`cf613f0b`…`cdffcbf8`; gate 6/0) |
| **14** FFI safety contract | **done** (2026-07-11) | [TRACK_FFI_SAFETY](archive/tracks/TRACK_FFI_SAFETY.md) **closed** STEP=1–5; Critic OK (`e1db7d81`…`3806d49c`); W-EXTERN-ATTR/ARITY; FFI_LAYER §9; REG 20/0; self-host DIFF 0 |
| **15** Debugging story (`#line` → `.mlc` в stack trace) | **closed** (Critic OK 2026-07-13; STEP=1–5) | [TRACK_DEBUG_SOURCE_MAP](archive/tracks/TRACK_DEBUG_SOURCE_MAP.md) — Ruby+mlcc `#line`; `docs/DEBUGGING.md`; `195d48a0`…`b7b6d878` |
| **16** Integer overflow semantics | **closed** | [TRACK_LANG_INT_OVERFLOW](archive/tracks/TRACK_LANG_INT_OVERFLOW.md) **closed** 2026-07-10 — signed debug-panic/release-UB; unsigned wrap; div0 panic; `mlc::int_arith` i32 |
| **17** `T!E` error-union sugar | **done** (2026-07-11) | [TRACK_LANG_ERROR_UNION](archive/tracks/TRACK_LANG_ERROR_UNION.md) **closed** STEP=1–5; Critic OK (`57913a3f`…`4ee642cc`); Ruby+mlcc desugar; e2e+`?`; REG 20/0; self-host DIFF 0 |
| **18** Package manager | **closed** (Critic OK 2026-07-13; STEP=1–10) | [TRACK_PACKAGE_MANAGER](archive/tracks/TRACK_PACKAGE_MANAGER.md) — `5e101b04`…`ad0ff1bf`; REG 20/0; p1≡p2; E2E 42 |
| **19** Автоматическое обнаружение циклов в рантайме | **open, design-only, вероятный won't-do** | [TRACK_LANG_AUTO_CYCLE](agent/TRACK_LANG_AUTO_CYCLE.md) — gated: не без команды пользователя (2026-07-11) |
| **20** Стратегия «без hand-written C++» (FFI-shim/бизнес-логика → mlcc/MLC) | **done** (2026-07-11) | [FFI_LAYER.md](FFI_LAYER.md) §8; подтреки 20a–e **closed** (Critic OK где применимо). Рантайм языка остаётся C++ (won't-do self-host runtime). Residuals: bridges/TcpStream, thin abi, smoke names |
| **20a** Postgres/Crypto/Tcp — прямой `extern fn` вместо `.hpp`-shim | **done** (2026-07-11) | [TRACK_FFI_SHIM_MIGRATION](archive/tracks/TRACK_FFI_SHIM_MIGRATION.md) **closed** STEP=1–7; Critic OK (`8ffe67b8`…`8b21220a`). Residual: bridges/TcpStream |
| **20b** MSDF (EDT/SDF) алгоритм — порт на MLC | **done** (2026-07-11) | [TRACK_TEXT_MSDF_TO_MLC](archive/tracks/TRACK_TEXT_MSDF_TO_MLC.md) **closed** STEP=1–6; Critic OK; MLC EDT + mask bridge; MAE=0; REG 20/0; self-host identical |
| **20c** WebSocket framing/handshake — порт на MLC | **closed** | [TRACK_STDLIB_WEBSOCKET_TO_MLC](archive/tracks/TRACK_STDLIB_WEBSOCKET_TO_MLC.md) **closed** 2026-07-11 (MLC bodies; hpp gone) |
| **20d** Env/Log/Validation — mlcc-пайплайн + порт логики | **done** (2026-07-11) | [TRACK_STDLIB_LOGIC_TO_MLC](archive/tracks/TRACK_STDLIB_LOGIC_TO_MLC.md) **closed** STEP=1–6; Critic OK; env_abi + Log/Validate MLC; bare names; REG 20/0; DIFF 0 |
| **20e** GL-вызовы через GLAD2, без ручного C++ dispatch | **done** (2026-07-11) | [TRACK_GL_GLAD_MIGRATION](archive/tracks/TRACK_GL_GLAD_MIGRATION.md) **closed** STEP=1–7; Critic OK (`0b613af4`…`855c7485`); vendored glad; dispatch/shim deleted; REG 20/0; self-host DIFF 0 |
| — | самохостинг `core`/`concurrency` рантайма | **won't-do** | [archive/tracks/TRACK_LANG_SELF_HOSTED_RUNTIME](archive/tracks/TRACK_LANG_SELF_HOSTED_RUNTIME.md) — рассмотрен и отклонён 2026-07-11 (рантайм остаётся C++, стандартная практика, риск/выгода не в пользу переписывания) |
| **21** | Compile-smoke coverage для `misc/examples`/`misc/gui` (regression молчала на `gui_button_demo.mlc`) | **done** (2026-07-12) | [TRACK_EXAMPLES_CI](archive/tracks/TRACK_EXAMPLES_CI.md) **closed** STEP=1–6; Critic OK (`cd598a50`…`d48ec41c`); REG 20/0; sweep ok=106 skip=1 |
| **22** | Дублирующийся `extern fn ... from "<header>"` в графе импортов → clang error вместо диагностики mlcc | **done** (2026-07-12) | [TRACK_FFI_EXTERN_DEDUP](archive/tracks/TRACK_FFI_EXTERN_DEDUP.md) **closed** Critic OK; Hybrid + E090; REG 20/0; sweep 106/0/1; DIFF=0 |
| **23** | GUI input robustness (debounce клика, keyboard text, resize) | **done** (2026-07-12) | [TRACK_GUI_INPUT_ROBUSTNESS](archive/tracks/TRACK_GUI_INPUT_ROBUSTNESS.md) **closed** Critic OK (`e86090cd`…`a4ec69f1`); STEP=2 N/A; 7 smokes ok |
| **24** | HTTP server hardening (keep-alive, лимиты, static files, graceful shutdown doc) | **done** (2026-07-12) | [TRACK_STDLIB_HTTP_HARDENING](archive/tracks/TRACK_STDLIB_HTTP_HARDENING.md) **closed** Critic OK (`aa5721f1`…`8e2d1b30`); REG 20/0; sweep 113/0/1 |
| **25** | Concurrency Supervisor — реализация (гейт снят) | **done** (2026-07-12) | [TRACK_CONCURRENCY_SUPERVISOR](archive/tracks/TRACK_CONCURRENCY_SUPERVISOR.md) **closed** Critic OK (`149899a2`…`44334e2d`); smoke+sanitize; C++-only |
| **26** | Concurrency test harness T6 (nightly fuzz) + T7 (`TestRuntime` MLC-level) | **done** (2026-07-12) | [TRACK_CONCURRENCY_TEST_HARNESS](archive/tracks/TRACK_CONCURRENCY_TEST_HARNESS.md) **closed** Critic OK (`24486b14`…`0d362257`); T7 C++-only; fuzz gate re-OK |
| **27** | Language reference manual (`docs/LANGUAGE_REFERENCE.md`) | **done** (2026-07-12) | [TRACK_LANG_DOCS](archive/tracks/TRACK_LANG_DOCS.md) **closed** Critic OK (`328cb686`…`022402ad`); `lang_ref_lint` 33/0 |
| **28** | Stdlib module reference (`docs/STDLIB_REFERENCE.md`) | **done** (2026-07-12) | [TRACK_STDLIB_DOCS](archive/tracks/TRACK_STDLIB_DOCS.md) **closed** Critic OK (`e47e22c5`…`8b2ae9a8`); snippet 10/0 |
| **21b** | GL text pipeline: per-call FreeType/HarfBuzz re-init (CPU load) + отсутствие baseline bearing (кривое выравнивание букв) | **done** (2026-07-12) Critic OK; STEP=1–14; REG 20/0; sweep 113/0/1 | [TRACK_TEXT_GL_PERF_BASELINE](archive/tracks/TRACK_TEXT_GL_PERF_BASELINE.md) — face/font cache (~47× user CPU); `glyph_bearing_*` + GL demos baseline; `text_a8_hxpjy_24.rgba` |
| **29** | Retained affine-transform scene graph (Figma/blueprint canvas + classic + game + Flash-rich UI — один фундамент) | **done** (2026-07-16) Phase A–D Critic OK | [archive/tracks/TRACK_GUI_CANVAS_GRAPH](archive/tracks/TRACK_GUI_CANVAS_GRAPH.md); Phase D [archive/tracks/TRACK_GUI_SCENE_PHASE_D.md](archive/tracks/TRACK_GUI_SCENE_PHASE_D.md); `d9cc60de`…`9e3eaebf` |
| **30** | HarfBuzz/FreeType шимы: §8 «без hand-written C++» пропустил их — face/font handle-кеш и pitch-copy loop остаются ручным C++ | **done** (2026-07-13) Critic OK; STEP=1–10 | [TRACK_TEXT_SHIM_TO_MLC](archive/tracks/TRACK_TEXT_SHIM_TO_MLC.md) — abi+text_shaping; ~27× user vs pre-cache; REG 20/0 |
| **32** | `text_ide_panels_demo` ~72ms/frame — `GlyphCache` O(n) + per-frame reshape | **done** (2026-07-13) Critic OK; STEP=1–3; corpus PASS | [TRACK_TEXT_GLYPH_CACHE_SCALING](archive/tracks/TRACK_TEXT_GLYPH_CACHE_SCALING.md) — HashMap+FIFO + layout cache; `c323556f`…`14972c49` |
| **33** | Native code editor MVP + mlc-support preflight; editor-first GUI architecture frozen | **done** (2026-07-14) Critic OK; STEP=pre…29; unit/model MVP | [EDITOR.md](EDITOR.md) + [GUI_ARCHITECTURE.md](GUI_ARCHITECTURE.md) + [TRACK_EDITOR_MVP](archive/tracks/TRACK_EDITOR_MVP.md) **closed**; live UX → §33a |
| **33a** | Headless UX harness (semantic scenarios ★, inject, probes; pixel MAE rare) | **done** (2026-07-14) Critic OK; STEP=0–15; `a159e85d`…`09e33f3e` | [GUI_UX_TESTING.md](GUI_UX_TESTING.md) + [TRACK_UX_HEADLESS](archive/tracks/TRACK_UX_HEADLESS.md) **closed**; residuals L8 blink + live GLFW/FBO |
| **33b** | Editor/GUI **live polish** (scissor, cursor, clipboard, command bus, frame split) — model≠product gap | **done** (2026-07-15) Critic OK; STEP=0–10 | [archive/tracks/TRACK_EDITOR_LIVE_POLISH](archive/tracks/TRACK_EDITOR_LIVE_POLISH.md); `011cdbb1`…`f3a8cdbc` |
| **34** | Phase B widget smokes: non-exhaustive `match` after `Path` (E084) | **done** (2026-07-16) Critic OK; STEP=1–2 | [archive/tracks/TRACK_GUI_SCENE_PATH_MATCH](archive/tracks/TRACK_GUI_SCENE_PATH_MATCH.md); `57015273`…`227a82c4` |
| **35** | Scene Phase B form live demo (Path arms + absorb WIP) | **done** (2026-07-16) Critic OK; STEP=1–2 | [archive/tracks/TRACK_SCENE_FORM_LIVE](archive/tracks/TRACK_SCENE_FORM_LIVE.md); `7969cd73`…`47839f7f` |
| **36** | Stdlib IO/FS: `list_dir` / `is_directory` / `is_regular_file` / `create_directories` + editor tree wire | **done** (2026-07-16) Critic OK; STEP=0–6; STEP=7 deferred | [archive/tracks/TRACK_STDLIB_IO_FS](archive/tracks/TRACK_STDLIB_IO_FS.md) |
| **37** | GLFW `content_scale` (HiDPI) — residual from §36 STEP=7 | **done** (2026-07-16) Critic OK; STEP=0–3 | [archive/tracks/TRACK_GLFW_CONTENT_SCALE](archive/tracks/TRACK_GLFW_CONTENT_SCALE.md) |
| **38** | Editor folder browser (breadcrumbs + listing; absorb WIP) | **done** (2026-07-16) Critic OK; STEP=0–4 | [archive/tracks/TRACK_EDITOR_FOLDER_BROWSER](archive/tracks/TRACK_EDITOR_FOLDER_BROWSER.md) |
| **39** | Editor font config path (DejaVuMono default; no Fontconfig) | **done** (2026-07-16) Critic OK; STEP=0–2 | [archive/tracks/TRACK_EDITOR_FONT_CONFIG](archive/tracks/TRACK_EDITOR_FONT_CONFIG.md) |
| **40** | Editor UTF-8 codepoint columns (`line_index`; grapheme deferred) | **done** (2026-07-16) Critic OK; STEP=0–3 | [archive/tracks/TRACK_EDITOR_UTF8_COLUMNS](archive/tracks/TRACK_EDITOR_UTF8_COLUMNS.md) |
| **41** | Editor caret blink (UX L8 `caret_blink_phases` + live helper) | **done** (2026-07-16) Critic OK; STEP=0–3 | [archive/tracks/TRACK_EDITOR_CARET_BLINK](archive/tracks/TRACK_EDITOR_CARET_BLINK.md) |
| **42** | Editor soft word-wrap (`layout/word_wrap`; absorb WIP) | **done** (2026-07-16) Critic OK; STEP=0–3 | [archive/tracks/TRACK_EDITOR_WORD_WRAP](archive/tracks/TRACK_EDITOR_WORD_WRAP.md) |
| **43** | Editor folder nav history (back/forward; absorb WIP) | **closed** (2026-07-18) Critic OK; STEP=0–3 | [archive/tracks/TRACK_EDITOR_FOLDER_NAV](archive/tracks/TRACK_EDITOR_FOLDER_NAV.md) |
| **44** | Codegen: eliminate string-concatenation, CppAST only (`expr.mlc` deleted; DeclFragments→AST; `GenStmtsResult`/`GenModuleOut` AST; residual Fragment/print bridges — not 0%) | **closed** (2026-07-17) Critic OK; STEP=1…9 | [archive/tracks/TRACK_CODEGEN_CPPAST_ONLY](archive/tracks/TRACK_CODEGEN_CPPAST_ONLY.md) |
| **45** | Editor development discipline — TDD-scenario-first standing rule (`GUI_UX_TESTING.md`) + `demo_live.mlc` Clean Architecture debt (refined: replace duplicated inline logic with existing `ux/*` calls, not just split `main()`) | **closed** (2026-07-18) Critic OK; STEP=0+3+4; STEP=1/2 → §46 #1 | [archive/tracks/TRACK_EDITOR_CLEAN_ARCHITECTURE](archive/tracks/TRACK_EDITOR_CLEAN_ARCHITECTURE.md) |
| **46** | Editor UX backlog — #1…#38 **done** (`#37 EDITOR_WRAPPED_TEXT_BLEEDS_INTO_MINIMAP` Critic OK) | **done** (2026-07-22) | [TRACK_EDITOR_UX_BACKLOG](agent/TRACK_EDITOR_UX_BACKLOG.md) · [archive/TRACK_EDITOR_WRAPPED_TEXT_BLEEDS_INTO_MINIMAP](archive/tracks/TRACK_EDITOR_WRAPPED_TEXT_BLEEDS_INTO_MINIMAP.md) · [archive/TRACK_EDITOR_IDLE_BUSY_LOOP_92PCT_CPU](archive/tracks/TRACK_EDITOR_IDLE_BUSY_LOOP_92PCT_CPU.md) · [archive/TRACK_EDITOR_WRAP_PER_FRAME_ON_LARGE_FILE](archive/tracks/TRACK_EDITOR_WRAP_PER_FRAME_ON_LARGE_FILE.md) · [archive/TRACK_EDITOR_MINIMAP](archive/tracks/TRACK_EDITOR_MINIMAP.md) |
| **47** | MLC concurrency refinement backlog — `#1`–`#11` **done** (`#11 CONCURRENCY_TESTRUNTIME_MLC_SURFACE` Critic OK) | **done** (2026-07-22) | [TRACK_MLC_CONCURRENCY_REFINEMENT](agent/TRACK_MLC_CONCURRENCY_REFINEMENT.md) · [archive/tracks/TRACK_CONCURRENCY_TESTRUNTIME_MLC_SURFACE](archive/tracks/TRACK_CONCURRENCY_TESTRUNTIME_MLC_SURFACE.md) · [archive/tracks/TRACK_CONCURRENCY_SUPERVISOR_MLC_SURFACE](archive/tracks/TRACK_CONCURRENCY_SUPERVISOR_MLC_SURFACE.md) · [CONCURRENCY_V2.md](CONCURRENCY_V2.md) |
| **48** | UX gate auto-discover full suite (63 scenarios; was hand-maintained 14) | **closed** (2026-07-22) Critic OK; STEP=0–2 | [archive/tracks/TRACK_UX_GATE_AUTO_DISCOVER](archive/tracks/TRACK_UX_GATE_AUTO_DISCOVER.md) |
| **49** | Editor solid-pass content clip (minimap residual of §46 #37) | **closed** (2026-07-22) Critic OK; STEP=0–3 | [archive/tracks/TRACK_EDITOR_SOLID_PASS_CONTENT_CLIP](archive/tracks/TRACK_EDITOR_SOLID_PASS_CONTENT_CLIP.md) |
| **50** | Editor idle caret blink (#38 residual: blink freezes on idle early-out) | **closed** (2026-07-22) Critic OK; STEP=0–3 | [archive/tracks/TRACK_EDITOR_IDLE_CARET_BLINK](archive/tracks/TRACK_EDITOR_IDLE_CARET_BLINK.md) |
| **51** | Editor idle CPU budget (live `/proc` sample; #38 residual) | **closed** (2026-07-22) Critic OK; STEP=0–3 | [archive/tracks/TRACK_EDITOR_IDLE_CPU_BUDGET](archive/tracks/TRACK_EDITOR_IDLE_CPU_BUDGET.md) |
| **52** | Editor wrap budget by glyph advance (#37 residual) | **closed** (2026-07-24) Critic OK; STEP=0–3 | [archive/tracks/TRACK_EDITOR_WRAP_BUDGET_ADVANCE](archive/tracks/TRACK_EDITOR_WRAP_BUDGET_ADVANCE.md) |
| **53** | Editor proportional hit-test / caret / selection (§52 residual) | **closed** (2026-07-24) Critic OK; STEP=0–3 | [archive/tracks/TRACK_EDITOR_PROPORTIONAL_HIT_TEST](archive/tracks/TRACK_EDITOR_PROPORTIONAL_HIT_TEST.md) |
| **54** | Editor selection highlight on visual wrap rows (§53 residual) | **closed** (2026-07-24) Critic OK; STEP=0–3 | [archive/tracks/TRACK_EDITOR_WRAP_SELECTION_VISUAL](archive/tracks/TRACK_EDITOR_WRAP_SELECTION_VISUAL.md) |
| **55** | Editor multi-cursor selection visual fills (§54 residual) | **closed** (2026-07-24) Critic OK; STEP=0–3 | [archive/tracks/TRACK_EDITOR_MULTI_SELECTION_VISUAL](archive/tracks/TRACK_EDITOR_MULTI_SELECTION_VISUAL.md) |
| **56** | Editor multi-caret draw + blink (§55 residual) | **closed** (2026-07-24) Critic OK; STEP=0–3 | [archive/tracks/TRACK_EDITOR_MULTI_CARET_DRAW](archive/tracks/TRACK_EDITOR_MULTI_CARET_DRAW.md) |
| **57** | Editor idle multi-caret paint overlay (§56 residual) | **closed** (2026-07-24) Critic OK; STEP=0–3 | [archive/tracks/TRACK_EDITOR_IDLE_MULTI_CARET_OVERLAY](archive/tracks/TRACK_EDITOR_IDLE_MULTI_CARET_OVERLAY.md) |
| **58** | Editor wrap hit-test respects scroll_x (§46 #17 residual) | **closed** (2026-07-24) Critic OK; STEP=0–3 | [archive/tracks/TRACK_EDITOR_WRAP_HIT_SCROLL_X](archive/tracks/TRACK_EDITOR_WRAP_HIT_SCROLL_X.md) |
| **59** | Editor text draw respects scroll_x (§58 residual) | **closed** (2026-07-24) Critic OK; STEP=0–3 | [archive/tracks/TRACK_EDITOR_TEXT_DRAW_SCROLL_X](archive/tracks/TRACK_EDITOR_TEXT_DRAW_SCROLL_X.md) |
| **60** | Editor horizontal content scrollbar (§59 / #33e residual) | **closed** (2026-07-24) Critic OK; STEP=0–3 | [archive/tracks/TRACK_EDITOR_HORIZONTAL_SCROLLBAR](archive/tracks/TRACK_EDITOR_HORIZONTAL_SCROLLBAR.md) |
| **61** | Editor content scrollbar thumb drag (§60 / #33e residual) | **closed** (2026-07-24) Critic OK; STEP=0–3 | [archive/tracks/TRACK_EDITOR_SCROLLBAR_THUMB_DRAG](archive/tracks/TRACK_EDITOR_SCROLLBAR_THUMB_DRAG.md) |
| **62** | Editor scrollbar thumb theme tint (§61 / #33e residual) | **closed** (2026-07-24) Critic OK; STEP=0–3 | [archive/tracks/TRACK_EDITOR_SCROLLBAR_THEME_TINT](archive/tracks/TRACK_EDITOR_SCROLLBAR_THEME_TINT.md) |
| **63** | Editor folder scrollbar thumb drag (§62 / §61 residual) | **closed** (2026-07-24) Critic OK; STEP=0–3 | [archive/tracks/TRACK_EDITOR_FOLDER_SCROLLBAR_THUMB_DRAG](archive/tracks/TRACK_EDITOR_FOLDER_SCROLLBAR_THUMB_DRAG.md) |
| **64** | Editor overlay/menu/nav theme tint (§46 #33c residual) | **closed** (2026-07-24) Critic OK; STEP=0–3 | [archive/tracks/TRACK_EDITOR_OVERLAY_THEME_TINT](archive/tracks/TRACK_EDITOR_OVERLAY_THEME_TINT.md) |
| **65** | Editor minimap indicator theme tint (§64 residual) | **closed** (2026-07-24) Critic OK; STEP=0–3 | [archive/tracks/TRACK_EDITOR_MINIMAP_INDICATOR_THEME_TINT](archive/tracks/TRACK_EDITOR_MINIMAP_INDICATOR_THEME_TINT.md) |
| **66** | Editor chrome hover default from Theme (§46 #33c residual) | **closed** (2026-07-24) Critic OK; STEP=0–3 | [archive/tracks/TRACK_EDITOR_CHROME_HOVER_DEFAULT_THEME](archive/tracks/TRACK_EDITOR_CHROME_HOVER_DEFAULT_THEME.md) |
| **67** | Editor discard danger from Theme (§66 residual) | **closed** (2026-07-24) Critic OK; STEP=0–3 | [archive/tracks/TRACK_EDITOR_DISCARD_DANGER_THEME](archive/tracks/TRACK_EDITOR_DISCARD_DANGER_THEME.md) |
| **68** | Editor active-tab idle tint (§46 #32 residual) | **closed** (2026-07-25) Critic OK; STEP=0–3 | [archive/tracks/TRACK_EDITOR_ACTIVE_TAB_TINT](archive/tracks/TRACK_EDITOR_ACTIVE_TAB_TINT.md) |
| **69** | Editor folder nav/breadcrumb hover (§46 #32 residual) | **closed** (2026-07-25) Critic OK; STEP=0–3 | [archive/tracks/TRACK_EDITOR_NAV_CRUMB_HOVER](archive/tracks/TRACK_EDITOR_NAV_CRUMB_HOVER.md) |
| **70** | Editor active-tab title without brackets (§68 residual) | **closed** (2026-07-25) Critic OK; STEP=0–3 | [archive/tracks/TRACK_EDITOR_ACTIVE_TAB_TITLE_PLAIN](archive/tracks/TRACK_EDITOR_ACTIVE_TAB_TITLE_PLAIN.md) |
| **71** | Editor scrollbar thumb hover lift (§46 #32 residual) | **closed** (2026-07-25) Critic OK; STEP=0–3 | [archive/tracks/TRACK_EDITOR_SCROLLBAR_THUMB_HOVER](archive/tracks/TRACK_EDITOR_SCROLLBAR_THUMB_HOVER.md) |
| **72** | Editor context-menu item hover (§46 #32 residual) | **closed** (2026-07-25) Critic OK; STEP=0–3 | [archive/tracks/TRACK_EDITOR_CONTEXT_MENU_ITEM_HOVER](archive/tracks/TRACK_EDITOR_CONTEXT_MENU_ITEM_HOVER.md) |
| **73** | Editor dirty-close button hover (§72 / #32 residual) | **closed** (2026-07-25) Critic OK; STEP=0–3 | [archive/tracks/TRACK_EDITOR_DIRTY_CLOSE_BUTTON_HOVER](archive/tracks/TRACK_EDITOR_DIRTY_CLOSE_BUTTON_HOVER.md) |
| **74** | Editor minimap syntax tags (§46 #35 residual) | **closed** (2026-07-25) Critic OK; STEP=0–3 | [archive/tracks/TRACK_EDITOR_MINIMAP_SYNTAX_TAGS](archive/tracks/TRACK_EDITOR_MINIMAP_SYNTAX_TAGS.md) |
| **75** | Editor minimap zoom-linked scale (§74 / #35 residual) | **closed** (2026-07-25) Critic OK; STEP=0–3 | [archive/tracks/TRACK_EDITOR_MINIMAP_ZOOM_SCALE](archive/tracks/TRACK_EDITOR_MINIMAP_ZOOM_SCALE.md) |
| **76** | Editor minimap strip width × font zoom (§75 residual) | **closed** (2026-07-25) Critic OK; STEP=0–3 | [archive/tracks/TRACK_EDITOR_MINIMAP_WIDTH_ZOOM](archive/tracks/TRACK_EDITOR_MINIMAP_WIDTH_ZOOM.md) |
| **77** | Editor syntax highlight span cache (§76 residual) | **closed** (2026-07-25) Critic OK; STEP=0–3 | [archive/tracks/TRACK_EDITOR_SYNTAX_HIGHLIGHT_CACHE](archive/tracks/TRACK_EDITOR_SYNTAX_HIGHLIGHT_CACHE.md) |
| **78** | Editor minimap highlight span cache (§77 residual) | **closed** (2026-07-25) Critic OK; STEP=0–3 | [archive/tracks/TRACK_EDITOR_MINIMAP_HIGHLIGHT_CACHE](archive/tracks/TRACK_EDITOR_MINIMAP_HIGHLIGHT_CACHE.md) |
| **79** | Editor shared full-buffer syntax span cache (§78 residual) | **closed** (2026-07-25) Critic OK; STEP=0–3 | [archive/tracks/TRACK_EDITOR_SHARED_SYNTAX_SPAN_CACHE](archive/tracks/TRACK_EDITOR_SHARED_SYNTAX_SPAN_CACHE.md) |
| **80** | Editor paint reuses frame LineIndex (#1d residual) | **closed** (2026-07-25) Critic OK; STEP=0–3 | [archive/tracks/TRACK_EDITOR_PAINT_REUSE_FRAME_LINE_INDEX](archive/tracks/TRACK_EDITOR_PAINT_REUSE_FRAME_LINE_INDEX.md) |
| **81** | Editor scroll reuses frame LineIndex (§80 residual) | **closed** (2026-07-25) Critic OK; STEP=0–3 | [archive/tracks/TRACK_EDITOR_SCROLL_REUSE_FRAME_LINE_INDEX](archive/tracks/TRACK_EDITOR_SCROLL_REUSE_FRAME_LINE_INDEX.md) |
| **82** | Editor selection reuses frame LineIndex (§81 residual) | **closed** (2026-07-25) Critic OK; STEP=0–3 | [archive/tracks/TRACK_EDITOR_SELECTION_REUSE_FRAME_LINE_INDEX](archive/tracks/TRACK_EDITOR_SELECTION_REUSE_FRAME_LINE_INDEX.md) |
| **83** | Editor goto-line reuses frame LineIndex (§82 residual) | **closed** (2026-07-25) Critic OK; STEP=0–3 | [archive/tracks/TRACK_EDITOR_GOTO_LINE_REUSE_FRAME_LINE_INDEX](archive/tracks/TRACK_EDITOR_GOTO_LINE_REUSE_FRAME_LINE_INDEX.md) |
| **84** | Editor overflow reuses frame LineIndex (§83 residual) | **closed** (2026-07-25) Critic OK; STEP=0–3 | [archive/tracks/TRACK_EDITOR_OVERFLOW_REUSE_FRAME_LINE_INDEX](archive/tracks/TRACK_EDITOR_OVERFLOW_REUSE_FRAME_LINE_INDEX.md) |
| **85** | Editor probe reuses frame LineIndex (§84 residual) | **closed** (2026-07-25) Critic OK; STEP=0–3 | [archive/tracks/TRACK_EDITOR_PROBE_REUSE_FRAME_LINE_INDEX](archive/tracks/TRACK_EDITOR_PROBE_REUSE_FRAME_LINE_INDEX.md) |
| **86** | Editor probe reuses LineIndex text (§85 residual) | **closed** (2026-07-25) Critic OK; STEP=0–3 | [archive/tracks/TRACK_EDITOR_PROBE_REUSE_LINE_INDEX_TEXT](archive/tracks/TRACK_EDITOR_PROBE_REUSE_LINE_INDEX_TEXT.md) |
| **87** | Editor drop-file opens without stringify (§86 residual) | **closed** (2026-07-25) Critic OK; STEP=0–3 | [archive/tracks/TRACK_EDITOR_DROP_FILE_NO_STRINGIFY](archive/tracks/TRACK_EDITOR_DROP_FILE_NO_STRINGIFY.md) |
| **88** | Editor session restore opens without stringify (§87 residual) | **closed** (2026-07-25) Critic OK; STEP=0–3 | [archive/tracks/TRACK_EDITOR_SESSION_RESTORE_NO_STRINGIFY](archive/tracks/TRACK_EDITOR_SESSION_RESTORE_NO_STRINGIFY.md) |
| **89** | Editor demo save avoids double stringify (§88 residual) | **closed** (2026-07-25) Critic OK; STEP=0–3 | [archive/tracks/TRACK_EDITOR_DEMO_SAVE_NO_DOUBLE_STRINGIFY](archive/tracks/TRACK_EDITOR_DEMO_SAVE_NO_DOUBLE_STRINGIFY.md) |
| **90** | Editor demo open-path without stringify (§89 residual) | **closed** (2026-07-25) Critic OK; STEP=0–3 | [archive/tracks/TRACK_EDITOR_DEMO_OPEN_PATH_NO_STRINGIFY](archive/tracks/TRACK_EDITOR_DEMO_OPEN_PATH_NO_STRINGIFY.md) |
| **91** | Editor tree-click opens without stringify (§90 residual) | **closed** (2026-07-25) Critic OK; STEP=0–3 | [archive/tracks/TRACK_EDITOR_TREE_CLICK_NO_STRINGIFY](archive/tracks/TRACK_EDITOR_TREE_CLICK_NO_STRINGIFY.md) |
| **92** | Editor clipboard slice without full stringify (§91 residual) | **closed** (2026-07-25) Critic OK; STEP=0–3 | [archive/tracks/TRACK_EDITOR_CLIPBOARD_SLICE_NO_FULL_STRINGIFY](archive/tracks/TRACK_EDITOR_CLIPBOARD_SLICE_NO_FULL_STRINGIFY.md) |
| **93** | Editor edit autoclose without full stringify (§92 residual) | **closed** (2026-07-25) Critic OK; STEP=0–3 | [archive/tracks/TRACK_EDITOR_EDIT_AUTOCLOSE_NO_FULL_STRINGIFY](archive/tracks/TRACK_EDITOR_EDIT_AUTOCLOSE_NO_FULL_STRINGIFY.md) |
| **94** | **superseded by §97** (2026-07-25) — root cause is the architectural gap §97 fixes directly (no single live `EditorAppState`/draw-op list), not an isolated bug worth patching separately | superseded | [agent/TRACK_EDITOR_HOVER_SCROLLBAR_PAINT_GAP](agent/TRACK_EDITOR_HOVER_SCROLLBAR_PAINT_GAP.md) |
| **95** | **superseded by §97** (2026-07-25) — same root cause as §94 | superseded | [agent/TRACK_EDITOR_DRAG_SELECTION_PAINT_GAP](agent/TRACK_EDITOR_DRAG_SELECTION_PAINT_GAP.md) |
| **96** | **superseded by §97** (2026-07-25) — folded into §97's real-state/real-perf work; still a valid cheap regression scenario, pick up as part of §97c | superseded | [agent/TRACK_EDITOR_WHEEL_HOVER_FOCUS_INDEPENDENT](agent/TRACK_EDITOR_WHEEL_HOVER_FOCUS_INDEPENDENT.md) |
| **97** (priority override, 2026-07-25, user: "тормозит всё адски, скроллы нормально не работают... системный подход к быстрому рендерингу... clean architecture на максималках") | **Editor render architecture** — execute the P0 backlog from the 2026-07-15 architecture review (`mlc-support/responses/gui_editor_architecture_20260715_105621.md`) that was never actioned: items #6/#7/#10 (`EDITOR_FRAME_SPLIT`, `EDITOR_UX_PROBE_FROM_LIVE_STATE`, `EDITOR_REAL_PERF_HARNESS`). Root cause confirmed 2026-07-25: `demo_live.mlc` is 2684 lines, calls `frame_snapshot_cache_tick` from **25 separate scattered call sites** (every edit branch threads it by hand) plus 4 more independently-threaded ad-hoc caches (`wrap_count_cache`/`shared_span_cache`/`minimap_cache`), no single frame-level dirty/state model — this is *why* narrow patches (§48-§96, ~50 tracks) kept surfacing new symptoms in the same area instead of converging. Sub-tracks below, in this order (perf baseline before refactor, per project's own "measure first" discipline) | **closed** 2026-07-25 (Critic OK) — P0 §97a/b/c+§96; residual later folds not queued | [agent/TRACK_EDITOR_RENDER_ARCHITECTURE](agent/TRACK_EDITOR_RENDER_ARCHITECTURE.md) |
| **98** | Editor newline auto-indent without full stringify (§93 / #1d residual) | **closed** (2026-07-25) Critic OK; STEP=0–3 | [archive/tracks/TRACK_EDITOR_NEWLINE_INDENT_NO_FULL_STRINGIFY](archive/tracks/TRACK_EDITOR_NEWLINE_INDENT_NO_FULL_STRINGIFY.md) |
| **99** | Editor word-delete without full stringify (§98 / #1d residual) | **closed** (2026-07-25) Critic OK; STEP=0–3 | [archive/tracks/TRACK_EDITOR_WORD_DELETE_NO_FULL_STRINGIFY](archive/tracks/TRACK_EDITOR_WORD_DELETE_NO_FULL_STRINGIFY.md) |
| **100** | Editor comment toggle without full stringify (§99 / #1d residual) | **closed** 2026-07-28 (Critic OK). Archived. Per-line slice, zero full-stringify in `edit.mlc` | [archive/tracks/TRACK_EDITOR_COMMENT_TOGGLE_NO_FULL_STRINGIFY](archive/tracks/TRACK_EDITOR_COMMENT_TOGGLE_NO_FULL_STRINGIFY.md) |
| **101** (§97a residual; queued after §104 Wave 1 per same-day priority override; **root-cause attribution corrected 2026-07-31, see §105**) | `visual_row_index_for_caret_pixel_budget` (`layout/word_wrap.mlc:422`), called once per caret every frame from `demo_live.mlc:~2212`, walks `line = 0 .. caret_line` calling `visual_rows_for_line_pixel_budget` for every line before the caret, uncached, every frame — a real defect, same disease class as §36/§105. **Correction:** the 2026-07-28 "confirmed by code read" claim that this "accounts for essentially all of the measured `draw_us`" on the 100k-line perf fixture was never actually measured — direct instrumentation 2026-07-31 shows `caret_line=0` throughout that fixture's 30 frames (fresh open, no session/scroll/caret-move) and this function costs only 74–330 µs/frame there; the fixture's real dominant cost is a different function, fixed as new §105. §101 remains open as a legitimate architecture cleanup (uncached O(caret_line) walk will matter once caret is genuinely deep in a large file); §105 CLOSED 2026-07-31. Red/green done same day: new `document_visual_row_prefix_pixel_budget` (`word_wrap.mlc`, same O(n) walk as the existing count function, computed once) + `visual_row_index_for_caret_pixel_budget_cached` (O(1) prefix lookup + single-line scan, falls back to the uncached walk when the prefix table does not cover `caret_line`); new `prefix_visual_rows: [i32]` field on `DocumentWrapCountCache` (`wrap_cache.mlc`), computed in the pixel-budget constructors/tick only (column-budget path stays `[]`, out of scope); `demo_live.mlc`'s 1 live call site routed through the cached function. New scenario `caret_visual_row_cache_stable` (cached vs uncached match at line 0/mid/last + fallback path + idle/edit invalidation). `run_ux_gate.sh` ×2 clean (114/114, 0 fail); perf smoke unregressed vs §105's post-fix baseline (caret stays at line 0 on that fixture, so this fix's own win is not visible there — expected) | **CLOSED** 2026-07-31 — Critic-audited (independent rebuild+rerun of the new scenario and perf smoke in separate output dirs, `run_ux_gate.sh` ×2 independently, 114/114 both times, 0 failures; `wrap_cache.mlc`/`word_wrap.mlc` diffs read directly confirming no added asymptotic cost and byte-identical tail logic; no false-done found). Queue head is now §102a | [agent/TRACK_EDITOR_CARET_VISUAL_ROW_CACHE](agent/TRACK_EDITOR_CARET_VISUAL_ROW_CACHE.md) |
| **102** (authorized 2026-07-28, user: "можно добавить треки по терминалу") | **Editor terminal panel** — libvterm-backed PTY terminal as a new panel/tab inside the existing (post-§97) unified `EditorAppState`/paint path, no parallel widget system. Sub-tracks §102a `TERMINAL_LIBVTERM_FFI` → §102b `TERMINAL_PTY_SPAWN` → §102c `TERMINAL_CELL_GRID_RENDER` → §102d `TERMINAL_INPUT_FORWARD` → §102e `TERMINAL_RESIZE_SCROLLBACK` → §102f `TERMINAL_PANEL_INTEGRATION` → §102g `TERMINAL_PERF_BUDGET`, strict order, each with its own gate (see track file). §102a **CLOSED** 2026-07-31: `runtime/{include,src}/terminal/vterm_abi.{hpp,cpp}` shim (freetype_abi/harfbuzz_abi pattern — raw-pointer `i64` handles, thread-local last-cell slot, damage-callback counter, an `vterm_escape_byte` 1-byte-ESC helper since MLC string literals have no hex-escape syntax); `compiler/build_bin.sh` gained a `pkg-config --exists vterm` detection block (same shape as freetype2/harfbuzz); `misc/editor/terminal/vterm_ffi.mlc` bindings; new unit `misc/editor/tests/terminal_libvterm_ffi_unit.mlc` (feeds plain text + 1 SGR true-color escape through `vterm_write_input`, asserts cell codepoints+RGB foreground+damage-callback fired) + `scripts/run_editor_terminal_libvterm_ffi_unit.sh` — passed first attempt. `run_ux_gate.sh` ×2 clean (114/114, 0 fail). Critic-audited same day: independent fresh `mlcc` rebuild, independent test rerun in a separate output dir, an independent probe exercising 3 error/edge paths + an indexed-SGR-color case not covered by the Driver's own test (all passed), independent `run_ux_gate.sh` ×2 (114/114 both times), independent `dev_gate_fast.sh` rerun with a clean environment reconfirming the disclosed stale-`TMPDIR` false alarm — no false-done found. §102b `TERMINAL_PTY_SPAWN` **CLOSED** 2026-07-31: new `runtime/{include,src}/terminal/pty_abi.{hpp,cpp}` shim (`forkpty` — owns slave tty setup internally, no manual `posix_openpt`/`grantpt`/`unlockpt` needed; `pty_spawn` execs the command through `/bin/sh -c` rather than raw `execvp`, no array-typed extern fn parameter precedent in this codebase; `pty_read_until_eof` loops poll+read until a timeout with no new data, draining bursts from still-running processes too); `compiler/build_bin.sh` gained an unconditional `pty_abi.cpp` compile + `-lutil` link (standard glibc, not an optional package); `misc/editor/terminal/pty_ffi.mlc` bindings; new unit `misc/editor/tests/terminal_pty_spawn_unit.mlc` asserting exact byte output **empirically measured** via a standalone probe before writing assertions (`/bin/echo hello` → `"hello\r\n"` via `ONLCR`; `/bin/cat` echo round-trip → `"ping\r\nping\r\n"`, both tty-echo and cat's own copy) + `scripts/run_editor_terminal_pty_spawn_unit.sh` — passed first attempt. `run_ux_gate.sh` ×2 clean (114/114, 0 fail); `dev_gate_fast.sh` clean environment 1471/0. Critic-audited same day: independent fresh `mlcc` rebuild, independent test rerun in a separate output dir, an independent probe exercising 3 error/edge paths (invalid-fd write/read/close, spawn-of-nonexistent-command, double-close) not covered by the Driver's own test (all passed), independent `run_ux_gate.sh` ×2 (114/114 both times), independent `dev_gate_fast.sh` rerun clean (1471/0) — no false-done found. §102c `TERMINAL_CELL_GRID_RENDER` **CLOSED** 2026-07-31: new `misc/editor/terminal/terminal_grid_render.mlc` bridge (pure MLC, reuses `static_text_draw_lines_colored`/`solid_renderer_rect`, no new draw path); found+fixed 2 pre-existing `vterm_abi.cpp` defects while wiring it (indexed/default colors never resolved to RGB via `vterm_screen_convert_color_to_rgb`; `vterm_set_utf8` never enabled, so multi-byte PTY output decoded as raw Latin-1); new `gl_read_pixel_component` GL binding (`glad_gl_abi.hpp`/`glad_gl.mlc`, touches `lib/mlc/`); gate `misc/examples/terminal_cell_grid_render_smoke.mlc` — spawns `echo` printing a background-colored cell run + a colored word through the full §102a/§102b/§102c pipeline, asserts exact `glReadPixels` match on the background rect and a non-black scan on the glyph ink, both assertions verified to actually fail when sabotaged before being reverted. `scripts/run_editor_terminal_cell_grid_render_smoke.sh` passed first attempt; §102a/§102b's own tests still pass after the `vterm_abi.cpp` fixes; `dev_gate_fast.sh` 1471/0; `run_ux_gate.sh` 114/114; `lib/mlc/` touched → `scripts/regression_gate.sh` run: Ruby-vs-mlcc parity ok, examples sweep ok=147 fail=0 skip=3 (same pre-existing skips). Critic-audited same day: independent fresh `mlcc` rebuild, independent smoke-test rerun in a separate output dir, the 3 `vterm_abi.cpp` fixes verified by reading the diff directly, a new independent probe proving the indexed-color path (not just truecolor) renders correctly end-to-end, 2 independent sabotages of the actual smoke test (different from the Driver's own) confirming both assertions load-bearing, independent `dev_gate_fast.sh`/`run_ux_gate.sh`/`regression_gate.sh` all green matching the Driver's numbers exactly — no false-done found. §102d `TERMINAL_INPUT_FORWARD` Driver Decision/red/green done 2026-07-31: new pure-encoder module `misc/editor/terminal/terminal_input_forward.mlc` (`terminal_keyboard_forward_poll`, same poll/pure-transform split as `frame_input.mlc`'s own `frame_input_poll`) — reuses the *existing* `glfw_gl_take_text`/`_key_backspace_down`/`_key_enter_down`/`_take_binding_key`/`_mod_ctrl_down` poll surface (no new input system; arrows/Tab reuse the existing "command-bus chords" `glfw_gl_take_binding_key()` channel already used for editor nav); byte values reuse the existing `mlc::net::string_from_byte_u8` header inline via a fresh `extern fn` (no new C++ binding). Byte mapping **empirically measured**, not assumed, via a standalone `pty_abi.cpp` probe against this codebase's own `forkpty` default termios: Backspace→DEL 0x7f (`stty -a` reports `erase = ^?`, confirmed erasing 2 chars via a `cat` round-trip), Enter→CR 0x0d (`icrnl` set), Ctrl+D 0x04 correctly EOFs a `cat` child, Ctrl+C 0x03 correctly SIGINTs a `sleep` child before completion, Tab→0x09 raw (`tab0`, no expansion), arrows→VT100 normal-mode CSI (`ESC[A/B/C/D` via the existing §102a `vterm_control_escape_byte()`). One small `runtime/src/gl/glfw_window_gl.cpp` addition: `GLFW_KEY_D` was never tracked in the binding-key channel (only C/Z, for copy/undo) — added for live-input symmetry, since Ctrl+D has no substitute. Gate: new `misc/examples/terminal_input_forward_smoke.mlc` + `scripts/run_editor_terminal_input_forward_smoke.sh` (same `MLC_GLFW_VISIBLE=0`/glfw3-skip shape as §102c's own runner) — types `"echo hi"` + Enter into a real interactive `sh` PTY session via the same `glfw_gl_input_test_*`/`glfw_gl_binding_key_test_push` hooks `editor_command_bus_live_smoke.mlc` already uses, asserts the vterm screen has a row whose trimmed text is exactly `"hi"` (distinct from the locally-echoed `"echo hi"` line); 5 supplementary scenarios (Backspace correction, Ctrl+D EOF, Ctrl+C SIGINT, Tab/arrow raw-byte encoding against a real `cat` child, Ctrl+Z byte independently re-derived) — passed first attempt after 1 self-caught scenario-authoring bug (typed `"hix"` instead of `"echo hix"`, fixed). Regression: §102a/§102b/§102c's own tests all still pass after the `GLFW_KEY_D` addition; `dev_gate_fast.sh` first run showed the 2 known stale-`TMPDIR`-from-a-prior-turn failures (confirmed via `git stash`, same root cause already documented at §102a), clean 1471/0 after unsetting; `run_ux_gate.sh` 114/114. No `lib/mlc/**`/`compiler/**` files touched — no `regression_gate.sh`/self-host diff required. **§102d CLOSED** 2026-07-31, Critic-audited same day: independent fresh `mlcc` rebuild, independent smoke rerun in a separate output dir, byte mapping independently re-derived with a standalone probe (own scenario shapes: `stty -a` parsed programmatically, a combined backspace+tab+enter session against `cat`, Ctrl+D against `wc -l` for a stronger EOF signal than the Driver's own `cat` check) — all confirmed; `GLFW_KEY_D`/binding-key-channel reuse spot-checked against `command_bus_default_bindings()` — no existing chord binds bare `"d"`, no collision. **1 real defect found and fixed**: sabotage-testing every byte constant found scenario 4 (Ctrl+C)'s `after_sigint` read window (1000ms) shorter than the `sleep 3` child it targets, so the assertion passed even with a wrong interrupt byte (5 instead of 3) — the preceding `!=` check is a tautological self-comparison, not a real behavioral check. Fixed by widening the window to 4000ms; re-verified correct byte passes, sabotaged byte now correctly fails. Independent `dev_gate_fast.sh` 1471/0, `run_ux_gate.sh` 114/114 after the fix. §102e `TERMINAL_RESIZE_SCROLLBACK` Driver Decision/red/green done 2026-07-31: `vterm_resize`/`vterm_screen_enable_reflow(true)` (rejoins auto-wrapped rows on widen) + `pty_resize` (`ioctl(TIOCSWINSZ)`, kernel delivers `SIGWINCH`) — both empirically measured via a standalone probe before writing assertions; new bounded scrollback ring buffer fed by libvterm's own `sb_pushline` callback, capped via `vterm_screen_set_scrollback_capacity` (oldest-first eviction, immediate on shrink), cleaned up on `vterm_destroy` to avoid a stale-entry-after-pointer-reuse bug (verified via a 200-iteration create/destroy loop). New glue `misc/editor/terminal/terminal_resize.mlc` (`terminal_panel_resize`, 1 call site for both primitives). Gate: new plain-unit test `misc/editor/tests/terminal_resize_scrollback_unit.mlc` (5 scenarios: reflow rejoin, scrollback cap+oldest-line-content check, immediate shrink eviction, real PTY `stty size` after resize, combined-glue call) + `scripts/run_editor_terminal_resize_scrollback_unit.sh` — passed first attempt. Sabotage-tested all 3 load-bearing behaviors: disabling reflow **crashes** (SIGSEGV inside libvterm's own color resolver, confirmed via `gdb`, a real bug beyond the rejoin-behavior argument alone); disabling immediate-shrink eviction and swapping the `ioctl` row/column order both correctly failed their scenarios; all reverted. §102a/§102b/§102c/§102d's own tests still pass unchanged (reflow only affects behavior during an actual resize call, none of which those tests make). `dev_gate_fast.sh` 1471/0; `run_ux_gate.sh` 114/114. No `lib/mlc/**`/`compiler/**` files touched — no `regression_gate.sh`/self-host diff required. **§102e CLOSED 2026-07-31** (Critic): independent ABI probe (shrink+grow reflow, capacity=3/50 lines, capacity=0, 80-iter destroy cleanup, `stty size` 24×80) all green; sabotage with *different* mutations than Driver (`pop_back` eviction → exit 7 `line0`; noop `vterm_resize` → exit 3; skip pty in `terminal_panel_resize` → exit 14 `0 0`) all load-bearing, reverted; §102a damage_count still fires under combined callbacks; first `run_ux_gate` with Critic TMPDIR set failed `newline_indent_no_full_stringify_stable` (stale-TMPDIR class, same as §102a/§102d precedent) — unset+rerun 114/114, no code change. **§102f CLOSED 2026-07-31** (Critic): found+fixed idle `terminal_frame("",0,0)` edge-state reset bug → `editor_app_terminal_drain`; sabotages write/drain/tab-path all load-bearing; `run_ux_gate` ×2 = 115/115; `enter_keeps_indent` ok. §102g next | **CLOSED** 2026-07-31 — §102a–§102g all Critic-audited (git `44b66105`+); epic complete | [agent/TRACK_EDITOR_TERMINAL](agent/TRACK_EDITOR_TERMINAL.md) |
| **103** (authorized 2026-07-28, user: "можно добавить треки по... script-vm") | **MLC Script VM, Phase 1** — embeddable dynamic-profile VM (design doc [MLC_SCRIPT_VM.md](MLC_SCRIPT_VM.md)), HARD STOP GATE lifted for Phase 1 only (§12 фазы 2-5 remain design-only, not authorized). Sub-tracks §103a `SCRIPT_VM_VALUE_REP` → §103b `SCRIPT_VM_BYTECODE_FORMAT` → §103c `SCRIPT_VM_VERIFIER` → §103d `SCRIPT_VM_INTERPRETER_ARITHMETIC` → §103e `SCRIPT_VM_CONTROL_FLOW` → §103f `SCRIPT_VM_HEAP_GC_ARENA` → §103g `SCRIPT_VM_ARRAYS_RECORDS` → §103h `SCRIPT_VM_CLOSURES_FIBERS` → §103i `SCRIPT_VM_EMBEDDING_ABI`, strict order (verifier before interpreter, per design doc §10), each with its own gate | **open** — §103e CLOSED; §103f Decision next; Phase 2–5 remain design-only | [agent/TRACK_MLC_SCRIPT_VM](agent/TRACK_MLC_SCRIPT_VM.md) |
| — | MLC Script VM Phase 2-5 (adaptive interpreter, baseline JIT, opcode DSL, tier 2) | **design-only, NOT authorized** | [MLC_SCRIPT_VM.md](MLC_SCRIPT_VM.md) §12 |
| **104** (authorized 2026-07-28, user: "отдельный трек чисто по улучшению производительности, архитектуре и тестированию"; **PRIORITY override same day**, user: "это должно быть приоритетом сейчас") | **Compiler core architecture/perf/testing hygiene** — execute the 24-step plan from the 2026-06-29 review (`mlc-support/responses/review_20260629_144027.md`), scope `compiler/**`, distinct from editor tracks (§97/§101) and new-feature epics (§102/§103). Confirmed 2026-07-28: **0 of 24 steps ever actioned** — **correction same day (Driver STEP=0 audit): wrong**, Steps 1/3 (`compiler/infrastructure/file_store.mlc` commit `36a1e372` 2026-06-30, `compiler/infrastructure/intern.mlc` commit `ab088d90` 2026-07-01) and Step 2's concrete code (`Span` offsets + `span_make`) were already done under different paths/names than the review suggested, missed by the file-existence check; verified green (`run_tests` → 1471 passed, 0 failed). See track file's "Correction 2026-07-28" section. God-files the review flagged have **grown** since — `transform.mlc` 1558→1765, `decl_cpp.mlc` 1119→1666, `match_gen.mlc` 907→1403, `registry.mlc` 870→1060, `infer.mlc` 786→962 lines. Step 17 (`CppInvokedBlock` string→AST) already done independently via §44, not reopened. **Wave 1** (§104-1/2/3 foundation — **done**, §104-12/13/14/15/16 god-file splits, §104-18/19/20 build-speed, §104-22/23 tooling) **moved to queue head, ahead of §101/§102/§103** → Wave 2 (§104-6/7/8/9/10/11 MIR-as-layer, stays after §103) → Wave 3 deferred/high-risk (§104-5/21/24, needs re-authorization at pickup) | **open** — §104-1/2/3 done, §104-12 slice-1 (`transform_coerce.mlc`) **closed** (Critic-audited: independent function-set diff, independent full `rake test_compiler_mlc` rerun 1471/0, independent mlcc translation spot-check all pass); §104-12 slice-2 (`transform_context.mlc`) **closed** (Critic-audited: independent function/type-set diff, independent full `rake test_compiler_mlc` rerun 1471/0, independent mlcc translation spot-check all pass); §104-12 slice-3 (`transform_call_args.mlc`) **closed** same day (Critic-audited: independent function/type-set diff, independent fresh mlcc translation + stray-reference grep, independent full `rake test_compiler_mlc` rerun 1471/0); §104-12 slice-4 (`transform_method.mlc` — 12 items, not the roughly-estimated 5) **closed** same day (Critic-audited: independent function/type-set diff, independent fresh mlcc translation + stray-reference grep, independent full `rake test_compiler_mlc` rerun 1471/0) — **§104-12 itself CLOSED** (`transform.mlc` split 1765→881 lines across 4 new modules); §104-13 (`codegen/decl_cpp.mlc` split, 1666 lines) slice 1 (`decl_cpp_helpers.mlc` — 4 pure-leaf declaration-list helpers, zero external callers) **closed** same day (Critic-audited: independent function/type-set diff, independent fresh mlcc translation + stray-reference grep, independent full `rake test_compiler_mlc` rerun 1471/0), `decl_cpp.mlc` 1666→1641 lines; slice 2 (`decl_cpp_type.mlc` — 13-item type/variant struct codegen group, zero circular dependency, no injection needed) **closed** same day (Critic-audited: independent function/type-set diff, independent fresh mlcc translation + stray-reference grep, independent full `rake test_compiler_mlc` rerun 1471/0), `decl_cpp.mlc` 1641→1463 lines; slice 3 (`decl_cpp_trait.mlc` — 8-item trait decl codegen group, needed `is_semantic_declaration_fn` relocated to `decl_cpp_helpers.mlc` first to avoid a two-way import cycle) **closed** same day (Critic-audited: independent function/type-set diff, independent fresh mlcc translation + stray-reference grep, independent full `rake test_compiler_mlc` rerun 1471/0), `decl_cpp.mlc` 1463→1360 lines; slice 4 (`decl_cpp_fn.mlc`) closed 2026-07-28; slice 5 (`decl_cpp_ffi.mlc`) closed 2026-07-28 (independent re-audit: name-set diff empty, export-status diff matches Decision exactly, fresh mlcc translation stray-reference check clean, independent `rake test_compiler_mlc` rerun exit 0; `decl_cpp.mlc` now 959 lines); slice 6 (`decl_cpp_extend.mlc` — hub `gen_decl_cpp`/`gen_proto_cpp` moved together with the extend/impl group to break a real mutual dependency found during green) **closed** 2026-07-29 — **§104-13 itself CLOSED** (`decl_cpp.mlc` split 1666→355 lines across 6 modules, all ≤800, allowlist entry removed); §104-12 slice 5 (`transform_support.mlc` — 9 pure-leaf helpers) **closed** 2026-07-30, Critic-audited same day (independent function/type-set diff 24/24 empty, export-status diff exactly the 8 documented, byte-level function-body diff all 9 verbatim modulo `export`, fresh mlcc translation + stray-`transform_support::`-reference grep clean, independent `rake test_compiler_mlc` rerun 1471/0) — **§104-12 itself now CLOSED**; `transform.mlc` 881→753 lines, allowlist entry removed; §104-14 (`codegen/expr/match_gen.mlc` split, 1403 lines) slice 1 (`match_result_type.mlc` — 13-item match-result C++ return-type resolution group, zero injected function parameters) **closed** 2026-07-30, Critic-audited same day (independent function/type-set diff 94/94 empty, export-status diff exactly the 1 documented item, byte-level function-body diff all 13 verbatim modulo `export`, fresh mlcc translation + stray-`match_result_type::`-reference grep clean, duplicate-symbol check clean, independent `rake test_compiler_mlc` rerun 1471/0, mlcc2 self-host diff IDENTICAL) — 1 correction found during implementation (a name collision between the moved-and-exported `match_return_cpp_type` and an unrelated same-named local helper in `expr_visitor_cpp.mlc` triggered a real mlcc codegen bug misattributing the local definition's namespace; fixed by renaming the local helper to `match_default_block_return_cpp_type`), `match_gen.mlc` 1403→1240 lines (still above 800, allowlisted, more slices needed); slice 2 (`match_arm_lambda.mlc` — 12-item pure C++ lambda-string template builder group, zero type imports) **closed** 2026-07-30, Critic-audited same day (independent function/type-set diff empty, export-status diff exactly the 8 newly-documented exports, fresh mlcc translation + stray-`match_arm_lambda::`-reference check confined to the split modules, independent `rake test_compiler_mlc` rerun 1471/0, mlcc2 self-host diff IDENTICAL) — zero repo-wide naming collisions found this slice (explicitly re-checked), `compiler/tests/test_codegen.mlc` left untouched (transitive import forwarding held), `match_gen.mlc` 1240→1185 lines (still above 800, allowlisted, more slices needed) — **§104-14 slice 2 CLOSED**, Critic-audited 2026-07-30 (independent function/type-set diff 81/81 empty, export-status diff exactly the 8 documented, byte-level function-body diff all 12 verbatim, fresh mlcc translation + stray-`match_arm_lambda::`-reference grep clean, duplicate-symbol check clean, independent `rake test_compiler_mlc` rerun exit 0, mlcc2 self-host diff independently rebuilt and IDENTICAL); slice 4 (`match_generic_ctor_type.mlc` — 2-way-shared generic-ctor-type-argument resolution group, 11 items, 1 exported) **closed** 2026-07-30, Critic-audited same day (independent function/type-set diff 65/65 empty, export-status diff exactly the 1 documented, byte-level function-body diff all 11 verbatim, dropped-import absence confirmed, `type_parameter_name_index` duplicate confirmed pre-existing/namespace-isolated not a regression, fresh mlcc translation + duplicate-symbol check clean, independent `rake test_compiler_mlc` rerun exit 0, mlcc2 self-host diff independently rebuilt and IDENTICAL) — `match_gen.mlc` 1129→1015 lines (still above 800, allowlisted, more slices needed — only the 3 codegen strategies threading `gen_stmts`/`eval_expr_fn` remain); slice 5 (`match_guarded_gen.mlc` — the guarded-if-chain + string-literal-if-chain statement-builder core shared by both remaining flavor pairs via a `print_expr` adapter, 33 items + 1 type, 7 exported) **closed** 2026-07-30, Critic-audited same day (independent function/type-set diff 60/60 real names empty, export-status diff exactly the 2 documented new exports, byte-level function/type-body diff all 31 verbatim modulo `export`, fresh mlcc translation + duplicate-symbol check clean, zero stray `match_codegen.`/`match_gen::` references, independent `rake test_compiler_mlc` rerun 1471/0 arch lint failures=0 warnings=10, mlcc2 self-host diff independently rebuilt and IDENTICAL) — `match_gen.mlc` 1015→414 lines, **drops below the 800-line allowlist threshold, entry removed** — **§104-14 itself CLOSED** (`match_gen.mlc` split 1403→414 lines across 5 slices/modules); §104-15 (`checker/registry.mlc` split, 1060 lines) slice 1 (`registry_type.mlc` — the `Type`/`TypeRegistry` core) Decision/red/green done 2026-07-30: confirmed via the self-hosted parser source that MLC has no `export { X } from './m'` re-export syntax (the review's assumed prerequisite), then empirically proved (3-file scratch probe, real `g++` compile+link+run) that plain `import { X } from './m'` forwarding — already load-bearing in §104-14 — extends to `type` declarations too, making the review's Часть 3 language extension unnecessary; `registry_type.mlc` created (339 lines), `registry.mlc` 1060→728 lines (**already ≤800, exit criterion met by 1 slice alone**, `registry_annotation.mlc`/`registry_build.mlc` no longer required by the gate). 1 real defect found and fixed: `infer_literals.mlc` used `Type`'s variant constructors with zero import of any registry name (the only file in the tree doing this), broke once the transitive-resolution chain hit 3 hops — fixed by adding the missing import. `rake test_compiler_mlc` 1471/0 arch lint failures=0 warnings=9 (down from 10); mlcc2 self-host diff IDENTICAL. Also: mid-slice incident (accidental `git stash pop` of an unrelated stash + a `git checkout --theirs -- .` mistake reverted 3 non-track WIP files; recovered from Cursor local-history, documented in the track file) and a separate, unrelated root-disk-full episode (freed via `ccache -C` + stale `.tmp/` cleanup) — both resolved, no data lost. **§104-15 slice 1 CLOSED** 2026-07-30, Critic-audited same day (independent function/type-name-set diff 68/68 empty at every nesting depth, independent byte-level body diff all 9 moved items verbatim, independent full-file reconstruction diff confirming zero unrelated changes to the remaining 724 lines, independent from-scratch Ruby-bootstrap `mlcc` rebuild + fresh translation + duplicate-symbol check clean, independent `rake test_compiler_mlc` rerun 1471/0, independent mlcc2 self-host diff rebuilt from scratch and IDENTICAL) — **§104-15 itself CLOSED**, 1 slice sufficient (`registry.mlc` 1060→728 lines, already ≤800). §104-16 (`checker/infer/infer.mlc` split, 962 lines) Decision/red/green done 2026-07-30: found the one self-contained sub-domain fitting the directory's existing `infer_expr_fn` injection convention (used by every sibling `infer_*.mlc` module already) — record-literal/record-update field-value inference, 9 items, 2 exported, zero external callers outside `infer.mlc` (confirmed by grep). `infer_record.mlc` created (255 lines); `infer.mlc` 962→747 lines (**already ≤800, exit criterion met by 1 slice alone**). Bootstrap diff scoped to exactly `infer.cpp/.hpp` + new `infer_record.cpp/.hpp`; caught and fixed a path-inconsistency false-positive mid-verification (worktree baseline translated with an absolute path vs current tree with a relative one — same class of `#line`-only spurious-diff mistake as during §104-15, corrected before drawing conclusions by translating both from matching relative paths). `rake test_compiler_mlc` 1471/0 arch lint failures=0 warnings=8 (down from 9); mlcc2 self-host diff IDENTICAL. Incidental hygiene fix: removed 4 stale `file_size:` allowlist entries found inert while touching that file — `infer.mlc` itself, `registry.mlc` (missed during §104-15's own close), `cpp_ast.mlc`/`print.mlc` (pre-existing since before this track, both already well under 800). **§104-16 CLOSED** 2026-07-30, Critic-audited same day (independent function/type-name-set diff 41/41 empty at top level, 71/71 at full nesting depth; independent byte-level body diff all 8 moved items verbatim modulo the documented injection edits; independent full-file reconstruction diff confirming zero unrelated changes; independent from-scratch Ruby-bootstrap rebuild + fresh translation + duplicate-symbol check clean; independent full `rake test_compiler_mlc` rerun 1471/0; independent mlcc2 self-host diff rebuilt from scratch — IDENTICAL). §104-18 (`--emit-layout=hybrid`) Decision/red/green done 2026-07-30: new `compiler/cpp_emit/layout.mlc` (`layout_group_for_path`/`layout_group_names`, 5 fixed groups by path prefix per the review's own branch order); `emit_layout: string` field threaded through `CompileOptions`/`ModularCompileInput`/`compile_modular` (default `'split'`, unchanged); `run_codegen_pass` gains a hybrid branch that concatenates each module's printed `.cpp` body into 1 of 5 group files instead of 1-per-module (`.hpp` per-module writes unchanged). Review's own literal acceptance test passed: `--emit-layout=hybrid` on `compiler/main.mlc` → exactly 5 `.cpp` files; a binary built from those 5 files re-translates `compiler/main.mlc` byte-identically to one built from the 185 split-mode files, empirically confirming the review's own namespace-concatenation-safety argument. New test coverage added (`test_layout.mlc`, 2 assertions in `test_dump_flags.mlc`) but a pre-existing, unrelated bug in the `tests_main.mlc` rebuild path (both `build_tests_self.sh` and direct Ruby `ModularCompiler` invocation fail identically on the pre-change baseline) meant the new tests could not be executed via the standard harness this turn — substituted a standalone probe under `compiler/` running the identical assertions through the full working `mlcc`→C++→binary pipeline, all passed; not a regression, disclosed in the track file. mlcc2 self-host diff (default split mode) IDENTICAL. **§104-18 CLOSED** 2026-07-30, Critic-audited same day (independent diff-review of all 13 touched files against the Decision's own Module-touch list, exact match; independent from-scratch Ruby-bootstrap rebuild; independent split-mode 185/185 + hybrid-mode exactly-5-files translations; 2 independently-built binaries from split/hybrid source whose own re-translations of `compiler/main.mlc` are byte-identical; total `.cpp` byte count preserved exactly across split/hybrid, 3328011 both ways; independent `rake test_compiler_mlc` rerun 1471/0; an independent probe built from the actual committed `test_layout.mlc`/`test_dump_flags.mlc` source, not a re-implementation, exercising all 25 assertions end-to-end through a fresh `mlcc`→C++→binary pipeline — 25/0; independent mlcc1→mlcc2 self-hosting round-trip diff IDENTICAL; confirmed the disclosed `tests_main.mlc` limitation is genuine/pre-existing via `build_tests.sh`'s own inline comment — no false-done found). §104-19 (include planner/forward-decls) **REJECTED** 2026-07-30 — survey before implementing found the technique structurally inapplicable: the 2 most heavily cross-module-shared types (`ast::Expr`/`Stmt`, `registry_type::Type`) compile to C++ `std::variant` type aliases, ineligible for forward-declaration, ruling out ~90% (4365/4811) of measured `Shared<T>` cross-module usages; separately, 66% (749/1127) of all direct `.hpp` includes are already transitively redundant today. No `compiler/**` code changed, no self-host diff applicable. §104-20 (`--cpp-mode=fast-build`) **CLOSED** 2026-07-30, Critic-audited (independent default-path diff-empty re-verification from a fresh `git worktree` baseline, independent self-host round-trip on 2 independently-built binaries, independent `rake test_compiler_mlc` rerun 1471/0, independent compile-time re-measurement with a different methodology confirming the same direction/magnitude: g++ −4.22%/clang++ −5.66% vs the Driver's −3.0%/−4.8%). §104-22 (`bootstrap-fast.sh`/`bootstrap-full.sh` tooling) **CLOSED** 2026-07-30, Critic-audited (independent fresh `mlcc` rebuild, independent script reruns — `bootstrap-fast.sh` timing within noise, `bootstrap-full.sh` printed `STAGE IDENTICAL` again — independent fail-fast-path test, independent Bash-language-choice confirmation via `file`). §104-23 (determinism checks `--dump-mir`/`--dump-sem`) **CLOSED** 2026-07-31: new `compiler/scripts/check-determinism.sh` + `rake test_determinism` task, no `.mlc` touched; ran against the real `compiler/main.mlc` — both `MIR DETERMINISTIC`/`SEM DETERMINISTIC` printed on the first attempt (38441/3753-line dumps), no hidden `Map`-ordering non-determinism found. Critic-audited same day (independent fresh `mlcc` rebuild, independent script + `rake` reruns with an exact dump-size match, independent fail-fast-path test, independent confirmation both dump printers are Map-free — no false-done found). **This closes the entire authorized Wave 1 list.** Queue head → **§101 Driver STEP=0** (`EDITOR_CARET_VISUAL_ROW_CACHE`) — **superseded same day**: investigating §101 found its own "confirmed root cause" unmeasured/wrong, real dominant bug fixed as new **§105**, queue head is now §105 Critic, §101 behind it | [agent/TRACK_COMPILER_ARCHITECTURE_HYGIENE](agent/TRACK_COMPILER_ARCHITECTURE_HYGIENE.md) |
| **105** (found 2026-07-31 while investigating §101 — same-day, no separate user authorization needed per the standing "производительность/архитектура/тестирование — приоритет" directive) | **Editor horizontal-scrollbar max-line-columns cache** — `editor_ux_max_line_columns_from_index` (`ux/overflow.mlc:44`) is an uncached O(document) full rescan (UTF-8 column count of every line), called unconditionally every draw frame from `demo_live.mlc` for the horizontal scrollbar (+3 more event-driven call sites). Empirically the **actual** dominant cost of `scripts/run_editor_demo_live_perf_smoke.sh`'s 100k-line fixture — isolated timer showed 249370–320719 µs/frame, >95% of `draw_us`, while §101's caret walk (previously blamed) cost only 74–330 µs/frame there. Fixed: new `MaxLineColumnsCache` (`ux/overflow.mlc`) wired into `EditorFrameLayout` (`app/frame_layout.mlc`, §97b) as `max_columns_cache` + `frame_layout_tick_max_columns`, ticked once per frame (not gated by `skip_full_pixel_wrap_now` — this scan is cheap arithmetic, not HarfBuzz shaping, so paying it once per text-change and amortizing to zero beats skipping it and reporting a wrong `max_columns=0`); all 4 `demo_live.mlc` call sites routed through the cache. New L1 scenario `max_line_columns_cache_stable`. Measured: `frames=30 layout_us=59041 draw_us=8927341 total_us=9064023` → `frames=30 layout_us=375140 draw_us=84844 total_us=532723` (**draw_us 105× lower, total_us 17× lower**) | **CLOSED** 2026-07-31 — Critic-audited (independent rebuild + re-measure: `draw_us=78062 total_us=501532`, same order of magnitude as the Driver's numbers; `run_ux_gate.sh` ×2, 113/113 both times, 0 failures; no false-done found). Queue head is now §101 | [agent/TRACK_EDITOR_HORIZONTAL_SCROLLBAR_MAX_COLUMNS_CACHE](agent/TRACK_EDITOR_HORIZONTAL_SCROLLBAR_MAX_COLUMNS_CACHE.md) |
| **106** (incident-driven, shipped 2026-07-31 as commit `47b4f134`, documented 2026-08-01; residual of §97) | **Editor idle/hover CPU — `pointer_dirty` split.** Live dogfood showed the editor pinning `frame_input_poll` raised a single `content_dirty` for any mouse move, forcing a full content frame (HarfBuzz pixel-wrap retick + visible-row collect + snapshot tick + full chrome repaint), and visible windows painted uncapped (`glfwSwapInterval(0)`). Shipped fix: new `pointer_dirty: i32` on `EditorFrameInput` (`app/frame_input.mlc`) raised by mouse-move alone, with out-of-client-area jitter ignored unless the new or previous position is inside the window; `demo_live.mlc` returns to `glfw_gl_context_wait_events_timeout(0.05)` **before** any layout/chrome work when both flags are clear; new `layout_skip` path repaints from `cached_visual_rows` and sets `skip_full_pixel_wrap_now` when only the pointer moved and the cached rows are valid; `glfw_gl_context_begin` uses `glfwSwapInterval(1)` when `MLC_GLFW_VISIBLE` is set (headless/perf/pixel paths stay at 0). Module touch: `app/frame_input.mlc`, `demo_live.mlc`, `runtime/src/gl/glfw_window_gl.cpp` — no `compiler/**`, no `lib/mlc/**`. **Disclosed gaps (not papered over):** no dedicated gate exists — `run_ux_idle_cpu_budget_stable.sh` measures idle *without* pointer motion and would have stayed green through the incident, and the perf smoke cannot observe the fix because `MLC_EDITOR_PERF=1` forces `skip_full_pixel_wrap_now`; no before/after CPU number was recorded; no Critic turn was run (incident fix). Residuals routed, not closed: hover-frame `frame_layout_tick_spans` still runs outside `layout_skip` → §107i; full-text cache invalidation → §107e; overlay/context-menu forcing `content_dirty` → §107 backlog B7; triple `collect_visible_visual_rows_pixel_budget` → §107j; missing idle-CPU-under-pointer-load gate → §107r. Explicitly **not** a perf result by itself: `SwapInterval(1)` caps frame count, it does not make a frame cheaper | **CLOSED** 2026-08-01 (recorded; fix shipped `47b4f134`) — closed as an incident fix with gaps disclosed above, **not** Critic-audited; all residuals owned by §107 | [agent/TRACK_EDITOR_IDLE_POINTER_DIRTY](agent/TRACK_EDITOR_IDLE_POINTER_DIRTY.md) |
| **107** (authorized 2026-08-01 as **queue head**, ahead of §103a and §104 Wave 2, per the standing "производительность / архитектура / тестирование — приоритет" directive; source: `mlc-support/responses/editor_hygiene_audit_20260801_103839.md`, Opus 5, findings EHA-01…EHA-28) | **Editor hygiene epic** — remediation of the 2026-08-01 audit. The audit's verdict: the model layers (`document/`, `layout/`, `workspace/`, `ux/`) are healthy (SRP kept, files small, units exist), the *product* layer is not — `misc/editor/demo_live.mlc` (~2788 lines) still owns all command dispatch, all paint, and all cache invalidation; §97 extracted input polling and the layout caches but not commands and not draw. Three "the product does not do what it claims" defects, none caught by any gate: **(1) Ctrl+S does not save the open file** — `CmdSave` is mapped to `save_demo_session`, which writes `.tmp/editor_live_*` copies via `dump_tab_file`; `open_buffer_save` is called from nowhere else, so the edited file on disk never changes and the active tab's dirty flag is never cleared (data loss, P0). **(2) Scroll is O(document) with HarfBuzz per frame** — `collect_visible_visual_rows_pixel_budget` (`layout/word_wrap.mlc`) walks from line 0 shaping every line and ignores the `prefix_visual_rows` table §101 already builds; the existing perf smoke only scrolls ~90 lines of the 100k-line fixture, so it never fires. **(3) The perf gate measures a path with the hot phases off** — under `MLC_EDITOR_PERF=1` `demo_live` sets `skip_full_pixel_wrap_now = 1`, skips `frame_layout_tick_spans` and skips the minimap, so a green `draw_us` says nothing about a real frame (class: false-green). Structural root cause behind most perf findings: **cache invalidation by full-text comparison** — four independent caches (`wrap_count_cache_needs_recompute_pixel`, `max_line_columns_cache_needs_recompute`, `editor_ux_syntax_span_cache_tick`'s `state.text == text`, `editor_ux_minimap_needs_rebuild`) each doing up to O(n) memcmp per frame, with the spans tick running even on hover frames (outside `layout_skip`, §106 residual). Ordered sub-tracks, strict order, each with its own gate (see track file): **P0** §107a `EDITOR_SAVE_ACTIVE_FILE` (EHA-01, gate `run_ux_save_writes_file_to_disk` — reads the file back **from disk**) → §107b `EDITOR_SESSION_ORIGINAL_PATHS` (EHA-02) → §107c `EDITOR_VISIBLE_ROWS_PREFIX_JUMP` (EHA-03, deep-scroll perf gate) → §107d `EDITOR_PERF_SMOKE_FULL_PATH` (EHA-04, new `MLC_EDITOR_PERF_FULL=1` with no skip branches, ceiling measured then written) → §107e `EDITOR_DOCUMENT_VERSION` (EHA-05, monotonic `version: i64` on `TextDocument`, all four caches re-keyed; unblocks §107f/i/j and the minimap work); **P1** in audit-roadmap order §107f `EDITOR_INCREMENTAL_LINE_INDEX` (EHA-06, §46 #1d residual) → §107g `EDITOR_TERMINAL_TEARDOWN` (EHA-08 + EHA-09, §102f/§102b residual — tab-strip "x" and window close bypass `terminal_panel_session_close`; `pty_close` never signals, orphaning `sh`) → §107h `EDITOR_SHAPE_SEGMENT_BUDGET` (EHA-07, the one real untrusted-content DoS vector) → §107i `EDITOR_SPANS_TICK_UNDER_LAYOUT_SKIP` (EHA-05 hover half, §97c/§106 residual) → §107j `EDITOR_VISIBLE_ROWS_SINGLE_COLLECT` (EHA-18, §97b residual) → §107k `EDITOR_TERMINAL_DAMAGE_REPAINT` (EHA-11, `vterm_damage_count` from §102a exists and is unconsumed) → §107l `EDITOR_INDENT_REPLACE_NO_STRINGIFY` (EHA-12, continues §93/§98/§99/§100) → §107m `EDITOR_OPEN_SIZE_GUARD` (EHA-13) → §107n `EDITOR_PROBE_GEOMETRY_PARITY` (EHA-14 — live `tab_strip_height: 0` vs scenario `28`, a whole class of false-green; 2026-07-15 #7 residual) → §107o `EDITOR_VIEWPORT_RECT_SINGLE` (EHA-15 — 8 copies of `text_viewport_rect`, was 5 in 2026-07-17, got worse) → §107p `EDITOR_TOOLBAR_COMMAND_TABLE` (EHA-16, 2026-07-15 #5 residual) → §107q `EDITOR_DRAW_OPS` (EHA-17, 2026-07-15 #4/#6 residual — the only large one, must land behaviour-preserving one paint region at a time) → §107r `EDITOR_UX_GATE_BEHAVIORAL` (EHA-19 — replace `*_stable.sh` source-grep gates with behavioural L1/L2, add the missing gate types: idle-CPU under pointer load (§106's own gap), deep-scroll perf, untrusted-content DoS, resource lifecycle, filesystem side effects, path/UTF-8 security, multi-frame L1.5). **P2 is a backlog table in the track file (B1–B11), not PLAN rows** — pull only after §107r or on explicit override; B10 (non-atomic save, audit §6.1) and B11 (EHA-10 `pty_spawn` shell-exec, catalog severity P1 but absent from the audit's own roadmap) are recorded there so no catalog finding is dropped. Binding non-goals: no wholesale `demo_live.mlc` rewrite; no `SceneNode` migration of editor chrome (the `GUI_ARCHITECTURE.md` Deviation stands); `SwapInterval(1)` is not a perf result; no second toolkit; no closing a sub-track on unit-only green where a side effect (file, fd, process) exists; no new grep-shaped `*_stable.sh` UX gates; no `compiler/**` changes in this epic | **CLOSED** 2026-08-03 — §107a–§107r Critic-audited; P2 B1–B11 backlog remains in track | [agent/TRACK_EDITOR_HYGIENE](agent/TRACK_EDITOR_HYGIENE.md) |
| **108** (authorized 2026-08-03 as **queue head**, ahead of remaining §107q/§107r; user dogfood: hover still «чудовищно тормозит»; measured idle-away ~0.5% / hover ~77%; `layout_skip` skips HarfBuzz but full-frame paint remains; implements present-pipeline from [GUI_ARCHITECTURE.md](GUI_ARCHITECTURE.md) §2/§6/§7) | **Editor retained paint / damage** — granular invalidation + retained layers without SceneNode chrome migration. Sub-tracks strict: §108a `EDITOR_DIRTY_TAXONOMY` (content/chrome/present_only + hit-stable no-frame) → §108b `EDITOR_RETAIN_TEXT_LAYER` (reuse text+gutter batch/FBO across chrome-only frames) → §108c `EDITOR_COMPOSE_PRESENT` (compose text/chrome/overlay; caret present_only) → §108d `EDITOR_HOVER_CPU_GATE` (behavioural hover/idle CPU ceilings measured then written). Non-goals: no SceneNode chrome migration; no Flutter/Qt/Skia; SwapInterval≠perf; no wholesale demo_live rewrite. After §108: resume §107q remainder → §107r → §103a → §104 Wave 2 | **CLOSED** 2026-08-03 — §108a–§108d all Critic-audited; epic complete | [agent/TRACK_EDITOR_RETAINED_PAINT](agent/TRACK_EDITOR_RETAINED_PAINT.md) |

**Приоритет очереди (строгий порядок + зависимости):**

```
PARSE_PROGRAM_RESULT → CODE_QUALITY → FORMATTER → PHASE26_REMAINING
  → SELF_HOST_BOOTSTRAP → LSP → CPP_HEADER_IMPORT (minimal)
  → CPP_PARSER_FULL → CLEAN_ARCHITECTURE → REDDIT_DEMO → CONCURRENCY
  → LANG_CLOSURE_ESCAPE (**closed** 2026-07-09, STEP=4 verify-gate)
  → CONCURRENCY_TEST_HARNESS T1-T4 (**done** 2026-07-09, sanitize CI)
  → CONCURRENCY_V2 STEP=1–6 (**closed** 2026-07-09)
  → CONCURRENCY_TASKSCOPE STEP=1–4 (**closed** 2026-07-09)
  → CONCURRENCY_TEST_HARNESS T5 (**done** 2026-07-09); T6–T7 (**done** 2026-07-12, Critic OK)
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
  → CONCURRENCY_SUPERVISOR (**closed** 2026-07-12: Critic OK; STEP=1–7; C++-only)
  → LANG_REGION_ARENA (**closed** 2026-07-12: Critic OK; STEP=1–9; STEP=10 skipped
      → [archive/tracks/TRACK_LANG_REGION_ARENA.md](archive/tracks/TRACK_LANG_REGION_ARENA.md))
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
  → STDLIB_HTTP_MLC (**closed** 2026-07-11: Critic OK; STEP=1–7; self-host
      identical; REG 20/0; parse+curl
      → [archive/tracks/TRACK_STDLIB_HTTP_MLC.md](archive/tracks/TRACK_STDLIB_HTTP_MLC.md))
  → CONCURRENCY_SPAWN_DETACH (**closed** 2026-07-11: Critic OK; STEP=1–5; E089+scope TaskScope;
      parallel sleep+accept-loop curl; REG 20/0; self-host identical
      → [archive/tracks/TRACK_CONCURRENCY_SPAWN_DETACH.md](archive/tracks/TRACK_CONCURRENCY_SPAWN_DETACH.md))
  → **«без hand-written C++» инициатива (2026-07-11, высокий приоритет;
    `LANG_SELF_HOSTED_RUNTIME` = won't-do):**
  → FFI_SHIM_MIGRATION (**closed** 2026-07-11: STEP=1–7; abi+bridges; self-host
      identical; regression 20/0
      → [archive/tracks/TRACK_FFI_SHIM_MIGRATION.md](archive/tracks/TRACK_FFI_SHIM_MIGRATION.md))
  → STDLIB_WEBSOCKET_TO_MLC (**closed** 2026-07-11: MLC upgrade/frames/handshake;
      `websocket.hpp`/`websocket_http.hpp` deleted; gate MLC echo+Ruby client;
      residual Ruby stubs in bridge
      → [archive/tracks/TRACK_STDLIB_WEBSOCKET_TO_MLC.md](archive/tracks/TRACK_STDLIB_WEBSOCKET_TO_MLC.md))
  → TEXT_MSDF_TO_MLC (**closed** 2026-07-11: Critic OK; STEP=1–6; MLC EDT; msdf_shim gone;
      MAE=0; REG 20/0; self-host identical
      → [archive/tracks/TRACK_TEXT_MSDF_TO_MLC.md](archive/tracks/TRACK_TEXT_MSDF_TO_MLC.md))
  → **STDLIB_LOGIC_TO_MLC (**closed** 2026-07-11: Critic OK; STEP=1–6;
      → [archive/tracks/TRACK_STDLIB_LOGIC_TO_MLC.md](archive/tracks/TRACK_STDLIB_LOGIC_TO_MLC.md)):**
  → **GL_GLAD_MIGRATION (**closed** 2026-07-11: Critic OK; STEP=1–7; glad vendor;
      dispatch/shim gone; REG 20/0; self-host DIFF 0
      → [archive/tracks/TRACK_GL_GLAD_MIGRATION.md](archive/tracks/TRACK_GL_GLAD_MIGRATION.md)):**
  → **§20 initiative closed** (20a–e done; runtime stays C++)
  → **FFI_SAFETY (**closed** 2026-07-11: Critic OK; STEP=1–5; W-EXTERN-ATTR/ARITY;
      FFI_LAYER §9; REG 20/0; self-host DIFF 0;
      → [archive/tracks/TRACK_FFI_SAFETY.md](archive/tracks/TRACK_FFI_SAFETY.md)):**
  → **LANG_ERROR_UNION (**closed** 2026-07-11: Critic OK; STEP=1–5; T!E→Result;
      e2e+`?`; REG 20/0; self-host DIFF 0;
      → [archive/tracks/TRACK_LANG_ERROR_UNION.md](archive/tracks/TRACK_LANG_ERROR_UNION.md)):**
  → LANG_AUTO_CYCLE (design-only, вероятный won't-do, не начинать реализацию
    без отдельной команды пользователя — не выбран пользователем 2026-07-11
    в списке активированных резервуаров, остаётся как есть)

  ↓ (2026-07-11, ~300-step horizon backlog — найдено вручную вне очереди
    при демо-сессии с пользователем, формализовано в треки для Grok)

  → **EXAMPLES_CI (**closed** 2026-07-12: Critic OK; STEP=1–6; REG 20/0;
      sweep ok=106 skip=1;
      → [archive/tracks/TRACK_EXAMPLES_CI.md](archive/tracks/TRACK_EXAMPLES_CI.md)):**
  → FFI_EXTERN_DEDUP (**done** 2026-07-12: Hybrid skip-emit + E090;
      → [archive/tracks/TRACK_FFI_EXTERN_DEDUP.md](archive/tracks/TRACK_FFI_EXTERN_DEDUP.md))
  → GUI_INPUT_ROBUSTNESS (**done** 2026-07-12: Critic OK; STEP=1–7; click-edge OK /
      keyboard / resize; 7 smokes green;
      → [archive/tracks/TRACK_GUI_INPUT_ROBUSTNESS.md](archive/tracks/TRACK_GUI_INPUT_ROBUSTNESS.md))
  → STDLIB_HTTP_HARDENING (**done** 2026-07-12: Critic OK; STEP=1–8; REG 20/0; sweep 113/0/1;
      → [archive/tracks/TRACK_STDLIB_HTTP_HARDENING.md](archive/tracks/TRACK_STDLIB_HTTP_HARDENING.md))
  → CONCURRENCY_SUPERVISOR (**done** 2026-07-12: Critic OK; STEP=1–7; C++-only; smoke+sanitize;
      → [archive/tracks/TRACK_CONCURRENCY_SUPERVISOR.md](archive/tracks/TRACK_CONCURRENCY_SUPERVISOR.md))
  → CONCURRENCY_TEST_HARNESS (**done** 2026-07-12: Critic OK; T1–T7; T7 C++-only; fuzz nightly;
      → [archive/tracks/TRACK_CONCURRENCY_TEST_HARNESS.md](archive/tracks/TRACK_CONCURRENCY_TEST_HARNESS.md))
  → LANG_DOCS (**done** 2026-07-12: Critic OK; STEP=1–11; `lang_ref_lint` 33/0;
      → [archive/tracks/TRACK_LANG_DOCS.md](archive/tracks/TRACK_LANG_DOCS.md))
  → STDLIB_DOCS (**done** 2026-07-12: Critic OK; STEP=1–7; snippet 10/0;
      → [archive/tracks/TRACK_STDLIB_DOCS.md](archive/tracks/TRACK_STDLIB_DOCS.md))

  ↓ (2026-07-12, пользователь нашёл 2 реальных бага в text_dashboard_demo,
    root cause уже найден при постановке трека (implementation-ready, не
    design-степ) — **вставлен перед `LANG_REGION_ARENA` STEP=2**: STEP=1
    того трека уже в работе (uncommitted WIP на момент вставки, 2026-07-12
    12:37) — дать ему завершиться и закоммититься, **не откатывать**, но
    следующий Planner-выбор — этот трек, не `LANG_REGION_ARENA` STEP=2)

  → **TEXT_GL_PERF_BASELINE (**closed** 2026-07-12: Critic OK; STEP=1–14;
    REG 20/0; sweep ok=113 fail=0 skip=1;
    → [archive/tracks/TRACK_TEXT_GL_PERF_BASELINE.md](archive/tracks/TRACK_TEXT_GL_PERF_BASELINE.md))**

  → **LANG_REGION_ARENA (**closed** 2026-07-12: Critic OK; STEP=1–9; STEP=10 skipped;
      → [archive/tracks/TRACK_LANG_REGION_ARENA.md](archive/tracks/TRACK_LANG_REGION_ARENA.md))**

  → **TEXT_SHIM_TO_MLC (**closed** 2026-07-13: Critic OK; STEP=1–10;
      `75263977`…`21b33afb`; vs_shim+field re-OK; REG 20/0; p1≡p2;
      → [archive/tracks/TRACK_TEXT_SHIM_TO_MLC.md](archive/tracks/TRACK_TEXT_SHIM_TO_MLC.md))**

  → **TEXT_GLYPH_CACHE_SCALING (**closed** 2026-07-13: Critic OK; STEP=1–3;
      `c323556f`…`14972c49`; ide 21.65→0.97s; corpus PASS; residuals: layout
      cache demo-only, corpus not in regression_gate;
      → [archive/tracks/TRACK_TEXT_GLYPH_CACHE_SCALING.md](archive/tracks/TRACK_TEXT_GLYPH_CACHE_SCALING.md))**

  → **PACKAGE_MANAGER (**closed** 2026-07-13: Critic OK; STEP=1–10;
      `5e101b04`…`ad0ff1bf`; REG 20/0; p1≡p2; E2E 42;
      → [archive/tracks/TRACK_PACKAGE_MANAGER.md](archive/tracks/TRACK_PACKAGE_MANAGER.md))**

  → **DEBUG_SOURCE_MAP (**closed** 2026-07-13: Critic OK; STEP=1–5;
      `195d48a0`…`b7b6d878`; REG 20/0; `docs/DEBUGGING.md`;
      → [archive/tracks/TRACK_DEBUG_SOURCE_MAP.md](archive/tracks/TRACK_DEBUG_SOURCE_MAP.md))**
  → **GUI_CANVAS_GRAPH Phase A (**closed** 2026-07-13: STEP=1–7; Critic OK (`9d1fd06e`…`1a010299`); reopen: none;
      → [archive/tracks/TRACK_GUI_SCENE_PHASE_A.md](archive/tracks/TRACK_GUI_SCENE_PHASE_A.md))**
  → **EDITOR_MVP (**closed** 2026-07-14: Critic OK; STEP=pre…29; `80a76d99`…`75fc5142`;
      → [EDITOR.md](EDITOR.md), [archive/tracks/TRACK_EDITOR_MVP.md](archive/tracks/TRACK_EDITOR_MVP.md))**
  → **UX_HEADLESS (§33a, **closed** 2026-07-14: Critic OK; STEP=0–15; `a159e85d`…`09e33f3e`;
      → [GUI_UX_TESTING.md](GUI_UX_TESTING.md), [archive/tracks/TRACK_UX_HEADLESS.md](archive/tracks/TRACK_UX_HEADLESS.md))**
  → **GUI_CANVAS_GRAPH Phase B (**closed** 2026-07-14: Critic OK; STEP=1–7; `c28c620b`…`31d9299a`;
      → [archive/tracks/TRACK_GUI_SCENE_PHASE_B.md](archive/tracks/TRACK_GUI_SCENE_PHASE_B.md))**
  → **GUI_CANVAS_GRAPH Phase C (**closed** 2026-07-15: Critic OK; STEP=1–7; `d0a3996f`…`64014467`;
      → [archive/tracks/TRACK_GUI_SCENE_PHASE_C.md](archive/tracks/TRACK_GUI_SCENE_PHASE_C.md))**
  → **EDITOR_LIVE_POLISH (§33b, **closed** 2026-07-15: Critic OK; STEP=0–10; `011cdbb1`…`f3a8cdbc`;
      → [archive/tracks/TRACK_EDITOR_LIVE_POLISH.md](archive/tracks/TRACK_EDITOR_LIVE_POLISH.md))**
  → **GUI_CANVAS_GRAPH Phase D (**closed** 2026-07-16: Critic OK; STEP=1–7; `d9cc60de`…`9e3eaebf`;
      → [archive/tracks/TRACK_GUI_SCENE_PHASE_D.md](archive/tracks/TRACK_GUI_SCENE_PHASE_D.md))**
  → **GUI_SCENE_PATH_MATCH (§34, **closed** 2026-07-16: Critic OK; STEP=1–2; `57015273`…`227a82c4`;
      → [archive/tracks/TRACK_GUI_SCENE_PATH_MATCH.md](archive/tracks/TRACK_GUI_SCENE_PATH_MATCH.md))**
  → **SCENE_FORM_LIVE (§35, **closed** 2026-07-16: Critic OK; STEP=1–2; `7969cd73`…`47839f7f`;
      → [archive/tracks/TRACK_SCENE_FORM_LIVE.md](archive/tracks/TRACK_SCENE_FORM_LIVE.md))**
  → **STDLIB_IO_FS (§36, **closed** 2026-07-16: Critic OK; STEP=0–6; STEP=7 deferred; `e05ae187`…`ecf8568f`;
      → [archive/tracks/TRACK_STDLIB_IO_FS.md](archive/tracks/TRACK_STDLIB_IO_FS.md))**
  → **GLFW_CONTENT_SCALE (§37, **closed** 2026-07-16: Critic OK; STEP=0–3; `651b6b2b`…`06e84ebe`;
      → [archive/tracks/TRACK_GLFW_CONTENT_SCALE.md](archive/tracks/TRACK_GLFW_CONTENT_SCALE.md))**
  → **EDITOR_FOLDER_BROWSER (§38, **closed** 2026-07-16: Critic OK; STEP=0–4; `9dcb6bfb`…`d33a90eb`;
      → [archive/tracks/TRACK_EDITOR_FOLDER_BROWSER.md](archive/tracks/TRACK_EDITOR_FOLDER_BROWSER.md))**
  → **EDITOR_FONT_CONFIG (§39, **closed** 2026-07-16: Critic OK; STEP=0–2; `279025c9`…`8176d8c5`;
      → [archive/tracks/TRACK_EDITOR_FONT_CONFIG.md](archive/tracks/TRACK_EDITOR_FONT_CONFIG.md))**
  → **EDITOR_UTF8_COLUMNS (§40, **closed** 2026-07-16: Critic OK; STEP=0–3; `83fd96f1`…`bd92d925`;
      → [archive/tracks/TRACK_EDITOR_UTF8_COLUMNS.md](archive/tracks/TRACK_EDITOR_UTF8_COLUMNS.md))**
  → **EDITOR_CARET_BLINK (§41, **closed** 2026-07-16: Critic OK; STEP=0–3; `de3a1dfc`…`9e758204`;
      → [archive/tracks/TRACK_EDITOR_CARET_BLINK.md](archive/tracks/TRACK_EDITOR_CARET_BLINK.md))**
  → **EDITOR_WORD_WRAP (§42, **closed** 2026-07-16: Critic OK; STEP=0–3; `a67cf4d1`…`65d77eec`;
      → [archive/tracks/TRACK_EDITOR_WORD_WRAP.md](archive/tracks/TRACK_EDITOR_WORD_WRAP.md))**
  → **EDITOR_FOLDER_NAV (§43, **closed** 2026-07-18: Critic OK; STEP=0–3; `d082a24e`…`b0a7afbf`;
      → [archive/tracks/TRACK_EDITOR_FOLDER_NAV.md](archive/tracks/TRACK_EDITOR_FOLDER_NAV.md))**
  → **CODEGEN_CPPAST_ONLY (§44, **closed** 2026-07-17: Critic OK; STEP=1…9; `expr.mlc` deleted;
      residual Fragment/print bridges — not 0%; archive;
      → [archive/tracks/TRACK_CODEGEN_CPPAST_ONLY.md](archive/tracks/TRACK_CODEGEN_CPPAST_ONLY.md))**
  → **EDITOR_CLEAN_ARCHITECTURE (§45, **closed** 2026-07-18: Critic OK; STEP=0+3+4;
      STEP=1/2 reframed → §46 #1; `caf412f4`…`0c03bfe2`;
      → [archive/tracks/TRACK_EDITOR_CLEAN_ARCHITECTURE.md](archive/tracks/TRACK_EDITOR_CLEAN_ARCHITECTURE.md))**
  → **EDITOR_UX_BACKLOG (§46, **done** 2026-07-22: #1…#38 closed; `#37` Critic OK; #1…#36 closed (#22 superseded);
      → [agent/TRACK_EDITOR_UX_BACKLOG.md](agent/TRACK_EDITOR_UX_BACKLOG.md) ·
        [archive/tracks/TRACK_EDITOR_WRAPPED_TEXT_BLEEDS_INTO_MINIMAP.md](archive/tracks/TRACK_EDITOR_WRAPPED_TEXT_BLEEDS_INTO_MINIMAP.md) ·
        [archive/tracks/TRACK_EDITOR_IDLE_BUSY_LOOP_92PCT_CPU.md](archive/tracks/TRACK_EDITOR_IDLE_BUSY_LOOP_92PCT_CPU.md) ·
        [archive/tracks/TRACK_EDITOR_WRAP_PER_FRAME_ON_LARGE_FILE.md](archive/tracks/TRACK_EDITOR_WRAP_PER_FRAME_ON_LARGE_FILE.md) ·
        [archive/tracks/TRACK_EDITOR_MINIMAP.md](archive/tracks/TRACK_EDITOR_MINIMAP.md) ·
        [archive/tracks/TRACK_EDITOR_SYNTAX_HIGHLIGHT_MLC_RICHER.md](archive/tracks/TRACK_EDITOR_SYNTAX_HIGHLIGHT_MLC_RICHER.md) ·
        [archive/tracks/TRACK_EDITOR_CONTENT_SCROLLBAR.md](archive/tracks/TRACK_EDITOR_CONTENT_SCROLLBAR.md) ·
        [archive/tracks/TRACK_EDITOR_TREE_PARENT_DOUBLE_CLICK.md](archive/tracks/TRACK_EDITOR_TREE_PARENT_DOUBLE_CLICK.md) ·
        [archive/tracks/TRACK_EDITOR_CHROME_THEME_DRIFT.md](archive/tracks/TRACK_EDITOR_CHROME_THEME_DRIFT.md) ·
        [archive/tracks/TRACK_EDITOR_ROW_BYTE_RANGE_BLEED.md](archive/tracks/TRACK_EDITOR_ROW_BYTE_RANGE_BLEED.md) ·
        [archive/tracks/TRACK_EDITOR_TREE_EXPAND_COLLAPSE.md](archive/tracks/TRACK_EDITOR_TREE_EXPAND_COLLAPSE.md) ·
        [archive/tracks/TRACK_EDITOR_CHROME_HOVER_STATE.md](archive/tracks/TRACK_EDITOR_CHROME_HOVER_STATE.md) ·
        [archive/tracks/TRACK_EDITOR_TAB_STRIP_CLASSIC_LAYOUT.md](archive/tracks/TRACK_EDITOR_TAB_STRIP_CLASSIC_LAYOUT.md) ·
        [archive/tracks/TRACK_EDITOR_COLUMN_PIXEL_MODEL_FIX.md](archive/tracks/TRACK_EDITOR_COLUMN_PIXEL_MODEL_FIX.md) ·
        [archive/tracks/TRACK_EDITOR_UX_L1_BACKFILL.md](archive/tracks/TRACK_EDITOR_UX_L1_BACKFILL.md) ·
        [archive/tracks/TRACK_LANG_RECORD_UPDATE.md](archive/tracks/TRACK_LANG_RECORD_UPDATE.md) ·
        [archive/tracks/TRACK_EDITOR_WIDE_CHAR_COLUMNS.md](archive/tracks/TRACK_EDITOR_WIDE_CHAR_COLUMNS.md) ·
        [archive/tracks/TRACK_EDITOR_DROP_FILE.md](archive/tracks/TRACK_EDITOR_DROP_FILE.md) ·
        [archive/tracks/TRACK_EDITOR_INDENT_MODEL.md](archive/tracks/TRACK_EDITOR_INDENT_MODEL.md) ·
        [archive/tracks/TRACK_EDITOR_CONTEXT_MENU.md](archive/tracks/TRACK_EDITOR_CONTEXT_MENU.md) ·
        [archive/tracks/TRACK_EDITOR_TRAILING_WS_VIZ.md](archive/tracks/TRACK_EDITOR_TRAILING_WS_VIZ.md) ·
        [archive/tracks/TRACK_EDITOR_MULTI_CURSOR.md](archive/tracks/TRACK_EDITOR_MULTI_CURSOR.md) ·
        [archive/tracks/TRACK_EDITOR_DIRTY_CLOSE_L1.md](archive/tracks/TRACK_EDITOR_DIRTY_CLOSE_L1.md) ·
        [archive/tracks/TRACK_EDITOR_SESSION_CARET_RESTORE.md](archive/tracks/TRACK_EDITOR_SESSION_CARET_RESTORE.md) ·
        [archive/tracks/TRACK_EDITOR_COMMENT_TOGGLE.md](archive/tracks/TRACK_EDITOR_COMMENT_TOGGLE.md) ·
        [archive/tracks/TRACK_EDITOR_BRACKET_AUTOCLOSE.md](archive/tracks/TRACK_EDITOR_BRACKET_AUTOCLOSE.md) ·
        [archive/tracks/TRACK_EDITOR_HORIZONTAL_SCROLL.md](archive/tracks/TRACK_EDITOR_HORIZONTAL_SCROLL.md) ·
        [archive/tracks/TRACK_EDITOR_FONT_ZOOM.md](archive/tracks/TRACK_EDITOR_FONT_ZOOM.md) ·
        [archive/tracks/TRACK_EDITOR_TAB_REORDER.md](archive/tracks/TRACK_EDITOR_TAB_REORDER.md) ·
        [archive/tracks/TRACK_EDITOR_REPLACE.md](archive/tracks/TRACK_EDITOR_REPLACE.md) ·
        [archive/tracks/TRACK_EDITOR_ENCODING_GUARD.md](archive/tracks/TRACK_EDITOR_ENCODING_GUARD.md) ·
        [archive/tracks/TRACK_EDITOR_CRLF_PRESERVE.md](archive/tracks/TRACK_EDITOR_CRLF_PRESERVE.md) ·
        [archive/tracks/TRACK_EDITOR_CURRENT_LINE_HL.md](archive/tracks/TRACK_EDITOR_CURRENT_LINE_HL.md) ·
        [archive/tracks/TRACK_EDITOR_AUTO_INDENT.md](archive/tracks/TRACK_EDITOR_AUTO_INDENT.md) ·
        [archive/tracks/TRACK_EDITOR_DRAG_AUTOSCROLL.md](archive/tracks/TRACK_EDITOR_DRAG_AUTOSCROLL.md) ·
        [archive/tracks/TRACK_EDITOR_GOTO_LINE.md](archive/tracks/TRACK_EDITOR_GOTO_LINE.md) ·
        [archive/tracks/TRACK_EDITOR_FIND.md](archive/tracks/TRACK_EDITOR_FIND.md) ·
        [archive/tracks/TRACK_EDITOR_UNDO_COALESCE.md](archive/tracks/TRACK_EDITOR_UNDO_COALESCE.md) ·
        [archive/tracks/TRACK_EDITOR_SHIFT_CLICK_EXTEND.md](archive/tracks/TRACK_EDITOR_SHIFT_CLICK_EXTEND.md) ·
        [archive/tracks/TRACK_EDITOR_MOUSE_WORD_LINE_SELECT.md](archive/tracks/TRACK_EDITOR_MOUSE_WORD_LINE_SELECT.md) ·
        [archive/tracks/TRACK_EDITOR_WORD_BOUNDARIES.md](archive/tracks/TRACK_EDITOR_WORD_BOUNDARIES.md) ·
        [archive/tracks/TRACK_EDITOR_KEYBOARD_NAV_WIRE.md](archive/tracks/TRACK_EDITOR_KEYBOARD_NAV_WIRE.md) ·
        [archive/tracks/TRACK_EDITOR_LARGE_FILE_NO_FULL_STRINGIFY.md](archive/tracks/TRACK_EDITOR_LARGE_FILE_NO_FULL_STRINGIFY.md) ·
        [archive/tracks/TRACK_EDITOR_STALE_HELP_TEXT.md](archive/tracks/TRACK_EDITOR_STALE_HELP_TEXT.md) ·
        [archive/tracks/TRACK_EDITOR_LIVE_SOLARIZED_TEXT.md](archive/tracks/TRACK_EDITOR_LIVE_SOLARIZED_TEXT.md) ·
        [archive/tracks/TRACK_EDITOR_DEMO_ORCHESTRATOR.md](archive/tracks/TRACK_EDITOR_DEMO_ORCHESTRATOR.md))**
  → **MLC_CONCURRENCY_REFINEMENT (§47, **done** 2026-07-22: `#1`–`#11` closed, `#11 CONCURRENCY_TESTRUNTIME_MLC_SURFACE` Critic OK;
      order: mutable-capture check → general
      `Send` bound → move-tracking → `Sync` trait → cancellation-wakes-blocking →
      rendezvous/unbounded channel → atomics → FFI concurrency metadata →
      Isolate/Supervisor/TestRuntime MLC surface;
      → [agent/TRACK_MLC_CONCURRENCY_REFINEMENT.md](agent/TRACK_MLC_CONCURRENCY_REFINEMENT.md) ·
        [archive/tracks/TRACK_CONCURRENCY_TESTRUNTIME_MLC_SURFACE.md](archive/tracks/TRACK_CONCURRENCY_TESTRUNTIME_MLC_SURFACE.md) ·
        [archive/tracks/TRACK_CONCURRENCY_SUPERVISOR_MLC_SURFACE.md](archive/tracks/TRACK_CONCURRENCY_SUPERVISOR_MLC_SURFACE.md) ·
        [archive/tracks/TRACK_CONCURRENCY_ISOLATE_MLC_SURFACE.md](archive/tracks/TRACK_CONCURRENCY_ISOLATE_MLC_SURFACE.md) ·
        [archive/tracks/TRACK_CONCURRENCY_FFI_METADATA.md](archive/tracks/TRACK_CONCURRENCY_FFI_METADATA.md) ·
        [archive/tracks/TRACK_CONCURRENCY_ATOMICS.md](archive/tracks/TRACK_CONCURRENCY_ATOMICS.md) ·
        [archive/tracks/TRACK_CONCURRENCY_CHANNEL_RENDEZVOUS_UNBOUNDED.md](archive/tracks/TRACK_CONCURRENCY_CHANNEL_RENDEZVOUS_UNBOUNDED.md))**
  → **UX_GATE_AUTO_DISCOVER (§48, **closed** 2026-07-22: Critic OK; STEP=0–2; 63 scenarios ×2;
      `6422b6c4`…`8d8d154e`;
      → [archive/tracks/TRACK_UX_GATE_AUTO_DISCOVER.md](archive/tracks/TRACK_UX_GATE_AUTO_DISCOVER.md))**
  → **EDITOR_SOLID_PASS_CONTENT_CLIP (§49, **closed** 2026-07-22: Critic OK; STEP=0–3;
      solid `content_clip` + minimap indicator under `minimap_rect`; `915dc910`…`eb7ba4f6`;
      → [archive/tracks/TRACK_EDITOR_SOLID_PASS_CONTENT_CLIP.md](archive/tracks/TRACK_EDITOR_SOLID_PASS_CONTENT_CLIP.md))**
  → **EDITOR_IDLE_CARET_BLINK (§50, **closed** 2026-07-22: Critic OK; STEP=0–3;
      `clock_ms` + `paint_dirty`; `d6168cb4`…`6a9cf56c`;
      → [archive/tracks/TRACK_EDITOR_IDLE_CARET_BLINK.md](archive/tracks/TRACK_EDITOR_IDLE_CARET_BLINK.md))**
  → **EDITOR_IDLE_CPU_BUDGET (§51, **closed** 2026-07-22: Critic OK; STEP=0–3;
      paint-only caret overlay + `/proc` CPU% ≤10; `9ed7c2c3`…`d9150fb0`;
      → [archive/tracks/TRACK_EDITOR_IDLE_CPU_BUDGET.md](archive/tracks/TRACK_EDITOR_IDLE_CPU_BUDGET.md))**
  → **EDITOR_WRAP_BUDGET_ADVANCE (§52, **closed** 2026-07-24: Critic OK; STEP=0–3;
      pixel-budget wrap via shaped advances; `717a690d`…`af121425`;
      → [archive/tracks/TRACK_EDITOR_WRAP_BUDGET_ADVANCE.md](archive/tracks/TRACK_EDITOR_WRAP_BUDGET_ADVANCE.md))**
  → **EDITOR_PROPORTIONAL_HIT_TEST (§53, **closed** 2026-07-24: Critic OK; STEP=0–3;
      advance-sum hit/caret/selection; `a2424a12`…`6a1b1ce9`;
      → [archive/tracks/TRACK_EDITOR_PROPORTIONAL_HIT_TEST.md](archive/tracks/TRACK_EDITOR_PROPORTIONAL_HIT_TEST.md))**
  → **EDITOR_WRAP_SELECTION_VISUAL (§54, **closed** 2026-07-24: Critic OK; STEP=0–3;
      visual-row selection fill; `7c078e1e`…`33fcee28`;
      → [archive/tracks/TRACK_EDITOR_WRAP_SELECTION_VISUAL.md](archive/tracks/TRACK_EDITOR_WRAP_SELECTION_VISUAL.md))**
  → **EDITOR_MULTI_SELECTION_VISUAL (§55, **closed** 2026-07-24: Critic OK; STEP=0–3;
      multi-range visual fills; `c2223498`…`298aa166`;
      → [archive/tracks/TRACK_EDITOR_MULTI_SELECTION_VISUAL.md](archive/tracks/TRACK_EDITOR_MULTI_SELECTION_VISUAL.md))**
  → **EDITOR_MULTI_CARET_DRAW (§56, **closed** 2026-07-24: Critic OK; STEP=0–3;
      multi-caret draw + blink; `728d014a`…`a163d58f`;
      → [archive/tracks/TRACK_EDITOR_MULTI_CARET_DRAW.md](archive/tracks/TRACK_EDITOR_MULTI_CARET_DRAW.md))**
  → **EDITOR_IDLE_MULTI_CARET_OVERLAY (§57, **closed** 2026-07-24: Critic OK; STEP=0–3;
      idle multi-caret paint overlay; `22339446`…`f61f51e3`;
      → [archive/tracks/TRACK_EDITOR_IDLE_MULTI_CARET_OVERLAY.md](archive/tracks/TRACK_EDITOR_IDLE_MULTI_CARET_OVERLAY.md))**
  → **EDITOR_WRAP_HIT_SCROLL_X (§58, **closed** 2026-07-24: Critic OK; STEP=0–3;
      wrap pixel hit + scroll_offset_x; `e00cc359`…`c35bbb67`;
      → [archive/tracks/TRACK_EDITOR_WRAP_HIT_SCROLL_X.md](archive/tracks/TRACK_EDITOR_WRAP_HIT_SCROLL_X.md))**
  → **EDITOR_TEXT_DRAW_SCROLL_X (§59, **closed** 2026-07-24: Critic OK; STEP=0–3;
      text_pen_x_scrolled on find/fill/caret/glyphs; `b950bc84`…`836e0e1f`;
      → [archive/tracks/TRACK_EDITOR_TEXT_DRAW_SCROLL_X.md](archive/tracks/TRACK_EDITOR_TEXT_DRAW_SCROLL_X.md))**
  → **EDITOR_HORIZONTAL_SCROLLBAR (§60, **closed** 2026-07-24: Critic OK; STEP=0–3;
      content_scrollbar_thumb_x + demo hover draw; `9d3287e3`…`8f6e230e`;
      → [archive/tracks/TRACK_EDITOR_HORIZONTAL_SCROLLBAR.md](archive/tracks/TRACK_EDITOR_HORIZONTAL_SCROLLBAR.md))**
  → **EDITOR_SCROLLBAR_THUMB_DRAG (§61, **closed** 2026-07-24: Critic OK; STEP=0–3;
      scroll_at_y/x + demo drag wire; `30c7388f`…`f24dcdd7`;
      → [archive/tracks/TRACK_EDITOR_SCROLLBAR_THUMB_DRAG.md](archive/tracks/TRACK_EDITOR_SCROLLBAR_THUMB_DRAG.md))**
  → **EDITOR_SCROLLBAR_THEME_TINT (§62, **closed** 2026-07-24: Critic OK; STEP=0–3;
      thumbs draw theme.muted_*; `8cf6c637`…`d4835288`;
      → [archive/tracks/TRACK_EDITOR_SCROLLBAR_THEME_TINT.md](archive/tracks/TRACK_EDITOR_SCROLLBAR_THEME_TINT.md))**
  → **EDITOR_FOLDER_SCROLLBAR_THUMB_DRAG (§63, **closed** 2026-07-24: Critic OK; STEP=0–3;
      folder_scrollbar_scroll_at_y + demo drag; `4afce0fe`…`84253492`;
      → [archive/tracks/TRACK_EDITOR_FOLDER_SCROLLBAR_THUMB_DRAG.md](archive/tracks/TRACK_EDITOR_FOLDER_SCROLLBAR_THUMB_DRAG.md))**
  → **EDITOR_OVERLAY_THEME_TINT (§64, **closed** 2026-07-24: Critic OK; STEP=0–3;
      dirty-close/menu/nav → theme.panel_/muted_*; `d77fa0a3`…`c72841c5`;
      → [archive/tracks/TRACK_EDITOR_OVERLAY_THEME_TINT.md](archive/tracks/TRACK_EDITOR_OVERLAY_THEME_TINT.md))**
  → **EDITOR_MINIMAP_INDICATOR_THEME_TINT (§65, **closed** 2026-07-24: Critic OK; STEP=0–3;
      indicator → theme.muted_*; `e3e34e2c`…`a21da97b`;
      → [archive/tracks/TRACK_EDITOR_MINIMAP_INDICATOR_THEME_TINT.md](archive/tracks/TRACK_EDITOR_MINIMAP_INDICATOR_THEME_TINT.md))**
  → **EDITOR_CHROME_HOVER_DEFAULT_THEME (§66, **closed** 2026-07-24: Critic OK; STEP=0–3;
      chrome_hover default → theme_solarized_light; `8f177065`…`ae896b1a`;
      → [archive/tracks/TRACK_EDITOR_CHROME_HOVER_DEFAULT_THEME.md](archive/tracks/TRACK_EDITOR_CHROME_HOVER_DEFAULT_THEME.md))**
  → **EDITOR_DISCARD_DANGER_THEME (§67, **closed** 2026-07-24: Critic OK; STEP=0–3;
      Theme.danger_* + discard button; `8ce50d63`…`c9190ac0`;
      → [archive/tracks/TRACK_EDITOR_DISCARD_DANGER_THEME.md](archive/tracks/TRACK_EDITOR_DISCARD_DANGER_THEME.md))**
  → **EDITOR_ACTIVE_TAB_TINT (§68, **closed** 2026-07-25: Critic OK; STEP=0–3;
      active tab idle → theme.selection_*; `7058b240`…`c786e5ec`;
      → [archive/tracks/TRACK_EDITOR_ACTIVE_TAB_TINT.md](archive/tracks/TRACK_EDITOR_ACTIVE_TAB_TINT.md))**
  → **EDITOR_NAV_CRUMB_HOVER (§69, **closed** 2026-07-25: Critic OK; STEP=0–3;
      nav/breadcrumb chrome_hover; `0d14ad1c`…`d0268f6e`;
      → [archive/tracks/TRACK_EDITOR_NAV_CRUMB_HOVER.md](archive/tracks/TRACK_EDITOR_NAV_CRUMB_HOVER.md))**
  → **EDITOR_ACTIVE_TAB_TITLE_PLAIN (§70, **closed** 2026-07-25: Critic OK; STEP=0–3;
      drop `[title]` brackets; `95c15893`…`1f45a8f7`;
      → [archive/tracks/TRACK_EDITOR_ACTIVE_TAB_TITLE_PLAIN.md](archive/tracks/TRACK_EDITOR_ACTIVE_TAB_TITLE_PLAIN.md))**
  → **EDITOR_SCROLLBAR_THUMB_HOVER (§71, **closed** 2026-07-25: Critic OK; STEP=0–3;
      thumb chrome_hover; `c6edcfc2`…`1e49413a`;
      → [archive/tracks/TRACK_EDITOR_SCROLLBAR_THUMB_HOVER.md](archive/tracks/TRACK_EDITOR_SCROLLBAR_THUMB_HOVER.md))**
  → **EDITOR_CONTEXT_MENU_ITEM_HOVER (§72, **closed** 2026-07-25: Critic OK; STEP=0–3;
      menu item chrome_hover; `598bba3d`…`74fb0662`;
      → [archive/tracks/TRACK_EDITOR_CONTEXT_MENU_ITEM_HOVER.md](archive/tracks/TRACK_EDITOR_CONTEXT_MENU_ITEM_HOVER.md))**
  → **EDITOR_DIRTY_CLOSE_BUTTON_HOVER (§73, **closed** 2026-07-25: Critic OK; STEP=0–3;
      discard/cancel chrome_hover; `e6d88360`…`b58523b3`;
      → [archive/tracks/TRACK_EDITOR_DIRTY_CLOSE_BUTTON_HOVER.md](archive/tracks/TRACK_EDITOR_DIRTY_CLOSE_BUTTON_HOVER.md))**
  → **EDITOR_MINIMAP_SYNTAX_TAGS (§74, **closed** 2026-07-25: Critic OK; STEP=0–3;
      minimap syntax glyphs; `8a1837d9`…`70c91b60`;
      → [archive/tracks/TRACK_EDITOR_MINIMAP_SYNTAX_TAGS.md](archive/tracks/TRACK_EDITOR_MINIMAP_SYNTAX_TAGS.md))**
  → **EDITOR_MINIMAP_ZOOM_SCALE (§75, **closed** 2026-07-25: Critic OK; STEP=0–3;
      minimap pixel size × font zoom; `1707178f`…`47385f14`;
      → [archive/tracks/TRACK_EDITOR_MINIMAP_ZOOM_SCALE.md](archive/tracks/TRACK_EDITOR_MINIMAP_ZOOM_SCALE.md))**
  → **EDITOR_MINIMAP_WIDTH_ZOOM (§76, **closed** 2026-07-25: Critic OK; STEP=0–3;
      strip width × font zoom; `46a9c264`…`9403d8f0`;
      → [archive/tracks/TRACK_EDITOR_MINIMAP_WIDTH_ZOOM.md](archive/tracks/TRACK_EDITOR_MINIMAP_WIDTH_ZOOM.md))**
  → **EDITOR_SYNTAX_HIGHLIGHT_CACHE (§77, **closed** 2026-07-25: Critic OK; STEP=0–3;
      cache visible-range highlight spans; `8fc5f83f`…`d4805de7`;
      → [archive/tracks/TRACK_EDITOR_SYNTAX_HIGHLIGHT_CACHE.md](archive/tracks/TRACK_EDITOR_SYNTAX_HIGHLIGHT_CACHE.md))**
  → **EDITOR_MINIMAP_HIGHLIGHT_CACHE (§78, **closed** 2026-07-25: Critic OK; STEP=0–3;
      cache full-buffer spans for minimap rebuild; `bbff3983`…`4848ccdb`;
      → [archive/tracks/TRACK_EDITOR_MINIMAP_HIGHLIGHT_CACHE.md](archive/tracks/TRACK_EDITOR_MINIMAP_HIGHLIGHT_CACHE.md))**
  → **EDITOR_SHARED_SYNTAX_SPAN_CACHE (§79, **closed** 2026-07-25: Critic OK; STEP=0–3;
      one full-buffer cache for paint+minimap; `77e83af9`…`69e862b8`;
      → [archive/tracks/TRACK_EDITOR_SHARED_SYNTAX_SPAN_CACHE.md](archive/tracks/TRACK_EDITOR_SHARED_SYNTAX_SPAN_CACHE.md))**
  → **EDITOR_PAINT_REUSE_FRAME_LINE_INDEX (§80, **closed** 2026-07-25: Critic OK; STEP=0–3;
      paint reuses frame LineIndex; `2968b98c`…`094e0c4d`;
      → [archive/tracks/TRACK_EDITOR_PAINT_REUSE_FRAME_LINE_INDEX.md](archive/tracks/TRACK_EDITOR_PAINT_REUSE_FRAME_LINE_INDEX.md))**
  → **EDITOR_SCROLL_REUSE_FRAME_LINE_INDEX (§81, **closed** 2026-07-25: Critic OK; STEP=0–3;
      scroll reuses frame LineIndex; `bbdcccf5`…`672fe791`;
      → [archive/tracks/TRACK_EDITOR_SCROLL_REUSE_FRAME_LINE_INDEX.md](archive/tracks/TRACK_EDITOR_SCROLL_REUSE_FRAME_LINE_INDEX.md))**
  → **EDITOR_SELECTION_REUSE_FRAME_LINE_INDEX (§82, **closed** 2026-07-25: Critic OK; STEP=0–3;
      selection reuses frame LineIndex; `56473a41`…`e55bca9c`;
      → [archive/tracks/TRACK_EDITOR_SELECTION_REUSE_FRAME_LINE_INDEX.md](archive/tracks/TRACK_EDITOR_SELECTION_REUSE_FRAME_LINE_INDEX.md))**
  → **EDITOR_GOTO_LINE_REUSE_FRAME_LINE_INDEX (§83, **closed** 2026-07-25: Critic OK; STEP=0–3;
      goto-line reuses frame LineIndex; `0f6806e8`…`051d9440`;
      → [archive/tracks/TRACK_EDITOR_GOTO_LINE_REUSE_FRAME_LINE_INDEX.md](archive/tracks/TRACK_EDITOR_GOTO_LINE_REUSE_FRAME_LINE_INDEX.md))**
  → **EDITOR_OVERFLOW_REUSE_FRAME_LINE_INDEX (§84, **closed** 2026-07-25: Critic OK; STEP=0–3;
      overflow reuses frame LineIndex; `93490972`…`348ffc43`;
      → [archive/tracks/TRACK_EDITOR_OVERFLOW_REUSE_FRAME_LINE_INDEX.md](archive/tracks/TRACK_EDITOR_OVERFLOW_REUSE_FRAME_LINE_INDEX.md))**
  → **EDITOR_PROBE_REUSE_FRAME_LINE_INDEX (§85, **closed** 2026-07-25: Critic OK; STEP=0–3;
      probe snapshot reuses frame LineIndex; `f866a137`…`121ebb52`;
      → [archive/tracks/TRACK_EDITOR_PROBE_REUSE_FRAME_LINE_INDEX.md](archive/tracks/TRACK_EDITOR_PROBE_REUSE_FRAME_LINE_INDEX.md))**
  → **EDITOR_PROBE_REUSE_LINE_INDEX_TEXT (§86, **closed** 2026-07-25: Critic OK; STEP=0–3;
      snapshot document_text from line_index.text; `cdd07cdc`…`6740640a`;
      → [archive/tracks/TRACK_EDITOR_PROBE_REUSE_LINE_INDEX_TEXT.md](archive/tracks/TRACK_EDITOR_PROBE_REUSE_LINE_INDEX_TEXT.md))**
  → **EDITOR_DROP_FILE_NO_STRINGIFY (§87, **closed** 2026-07-25: Critic OK; STEP=0–3;
      open dropped path without document_to_string; `cca9e565`…`cd5a0a61`;
      → [archive/tracks/TRACK_EDITOR_DROP_FILE_NO_STRINGIFY.md](archive/tracks/TRACK_EDITOR_DROP_FILE_NO_STRINGIFY.md))**
  → **EDITOR_SESSION_RESTORE_NO_STRINGIFY (§88, **closed** 2026-07-25: Critic OK; STEP=0–3;
      restore tabs via tab_set_open_buffer; `9013de25`…`4f95fc34`;
      → [archive/tracks/TRACK_EDITOR_SESSION_RESTORE_NO_STRINGIFY.md](archive/tracks/TRACK_EDITOR_SESSION_RESTORE_NO_STRINGIFY.md))**
  → **EDITOR_DEMO_SAVE_NO_DOUBLE_STRINGIFY (§89, **closed** 2026-07-25: Critic OK; STEP=0–3;
      one flatten per tab in save_demo_session; `aaa6a995`…`a7695b8b`;
      → [archive/tracks/TRACK_EDITOR_DEMO_SAVE_NO_DOUBLE_STRINGIFY.md](archive/tracks/TRACK_EDITOR_DEMO_SAVE_NO_DOUBLE_STRINGIFY.md))**
  → **EDITOR_DEMO_OPEN_PATH_NO_STRINGIFY (§90, **closed** 2026-07-25: Critic OK; STEP=0–3;
      disk opens via tab_set_open_buffer; `a7a0c9ac`…`24002446`;
      → [archive/tracks/TRACK_EDITOR_DEMO_OPEN_PATH_NO_STRINGIFY.md](archive/tracks/TRACK_EDITOR_DEMO_OPEN_PATH_NO_STRINGIFY.md))**
  → **EDITOR_TREE_CLICK_NO_STRINGIFY (§91, **closed** 2026-07-25: Critic OK; STEP=0–3;
      tree click via tab_set_open_buffer; `3c4cbf99`…`b298e29d`;
      → [archive/tracks/TRACK_EDITOR_TREE_CLICK_NO_STRINGIFY.md](archive/tracks/TRACK_EDITOR_TREE_CLICK_NO_STRINGIFY.md))**
  → **EDITOR_CLIPBOARD_SLICE_NO_FULL_STRINGIFY (§92, **closed** 2026-07-25: Critic OK; STEP=0–3;
      piece-range document_byte_slice for clipboard; `eb759388`…`79e1a78c`;
      → [archive/tracks/TRACK_EDITOR_CLIPBOARD_SLICE_NO_FULL_STRINGIFY.md](archive/tracks/TRACK_EDITOR_CLIPBOARD_SLICE_NO_FULL_STRINGIFY.md))**
  → **EDITOR_EDIT_AUTOCLOSE_NO_FULL_STRINGIFY (§93, **closed** 2026-07-25: Critic OK; STEP=0–3;
      autoclose via document_byte_slice; `0d3dd8a8`…`3bf241e4`;
      → [archive/tracks/TRACK_EDITOR_EDIT_AUTOCLOSE_NO_FULL_STRINGIFY.md](archive/tracks/TRACK_EDITOR_EDIT_AUTOCLOSE_NO_FULL_STRINGIFY.md))**
  → **EDITOR_NEWLINE_INDENT_NO_FULL_STRINGIFY (§98, **closed** 2026-07-25: Critic OK; STEP=0–3;
      line-start scan + document_byte_slice; `fbcc9570`…`21b04e45`;
      → [archive/tracks/TRACK_EDITOR_NEWLINE_INDENT_NO_FULL_STRINGIFY.md](archive/tracks/TRACK_EDITOR_NEWLINE_INDENT_NO_FULL_STRINGIFY.md))**
  → **EDITOR_WORD_DELETE_NO_FULL_STRINGIFY (§99, **closed** 2026-07-25: Critic OK; STEP=0–3;
      word_boundary_*_document; `fc00e47b`…`741ec105`;
      → [archive/tracks/TRACK_EDITOR_WORD_DELETE_NO_FULL_STRINGIFY.md](archive/tracks/TRACK_EDITOR_WORD_DELETE_NO_FULL_STRINGIFY.md))**
  → **EDITOR_COMMENT_TOGGLE_NO_FULL_STRINGIFY (§100, closed 2026-07-28 Critic OK:
      per-line slice + local replace; [archive/tracks/TRACK_EDITOR_COMMENT_TOGGLE_NO_FULL_STRINGIFY](archive/tracks/TRACK_EDITOR_COMMENT_TOGGLE_NO_FULL_STRINGIFY.md))**
  → **COMPILER_ARCHITECTURE_HYGIENE Wave 1 only (§104, PRIORITY override 2026-07-28,
      user: "это должно быть приоритетом сейчас" — moved ahead of §101/§102/§103;
      §100 closed 2026-07-28; §104-1/2/3 found already implemented same day
      (Driver STEP=0 audit — wrong file-existence check in the original review,
      see track file); §104-12 slice 1 (`transform_coerce.mlc`) closed same day
      (Critic-audited); §104-12 slice 2 (`transform_context.mlc`) closed
      same day, Critic-audited; §104-12 slice 3 (`transform_call_args.mlc`)
      closed same day, Critic-audited; §104-12 slice 4
      (`transform_method.mlc`, 12 items) closed same day, Critic-audited —
      **§104-12 marked CLOSED same day, corrected 2026-07-29: premature** —
      `transform.mlc` 1765→881 lines is still **>800**, the project's own
      arch-lint hard gate (`compiler/tests/run_architecture_lint.sh:7`), and
      its `file_size:` allowlist entry was never removed; reopen as slice 5
      Decision before treating §104-12 as done, see track file's exit
      criterion added 2026-07-29; **§104-13 (`codegen/decl_cpp.mlc` split,
      1666 lines) CLOSED 2026-07-29 — 6 slices, 1666→355 lines across
      `decl_cpp.mlc` + 5 new modules, all ≤800, allowlist entry removed,
      Critic-audited**; **§104-12 slice 5 done 2026-07-30** —
      `transform_support.mlc` extracted (9 pure-leaf helpers, 139 lines),
      `transform.mlc` 881→753 lines, allowlist entry removed, mlcc2
      self-host diff IDENTICAL, `1471 passed, 0 failed` —
      **§104-12 itself now CLOSED, Critic-confirmed same day** (independent
      function/type-set diff 24/24 empty, export-status diff exactly the 8
      documented, byte-level function-body diff all 9 verbatim, fresh mlcc
      translation + stray-reference grep clean, independent test rerun
      1471/0); **§104-14 (`codegen/expr/match_gen.mlc` split, 1403 lines)
      CLOSED 2026-07-30 — 5 slices, 1403→414 lines across `match_gen.mlc` +
      5 new modules (`match_result_type.mlc`/`match_arm_lambda.mlc`/
      `match_field_binding.mlc`/`match_generic_ctor_type.mlc`/
      `match_guarded_gen.mlc`), ≤800, allowlist entry removed,
      Critic-audited**; **§104-15 CLOSED** 2026-07-30 (`registry_type.mlc`
      slice, `registry.mlc` 1060→728 lines, ≤800, gate met, 1 slice
      sufficient, Critic-audited); **§104-16 CLOSED** 2026-07-30
      (`infer_record.mlc` slice, `infer.mlc` 962→747 lines,
      ≤800, gate met, 1 slice sufficient; also removed 4 stale
      `file_size:` allowlist entries found inert: `infer.mlc` itself,
      `registry.mlc`, `cpp_ast.mlc`, `print.mlc`; Critic-audited);
      **§104-18 CLOSED 2026-07-30** (`--emit-layout=hybrid`: new
      `cpp_emit/layout.mlc`, hybrid codegen branch in `run_codegen_pass`,
      review's own literal acceptance test passed — 5 group `.cpp` files,
      byte-identical re-translation split-built vs hybrid-built binary;
      Critic-audited same day: independent diff-review of all 13 touched
      files, independent from-scratch rebuild, independent split/hybrid
      translations + 2 independently-built binaries byte-identical,
      total `.cpp` byte count preserved exactly (3328011 both ways),
      independent `rake test_compiler_mlc` rerun, an independent probe
      built from the actual committed test source (not a
      re-implementation) exercising all 25 assertions end-to-end —
      25/0,       independent mlcc1→mlcc2 self-host round-trip diff IDENTICAL,
      no false-done found); **§104-19 CLOSED (REJECTED) 2026-07-30** —
      survey before implementing found the review's include-planner
      technique structurally inapplicable to this codebase: its 2 most
      heavily cross-module-shared types (`ast::Expr`/`Stmt`,
      `registry_type::Type`) compile to C++ `std::variant` type aliases,
      which cannot be forward-declared at all, ruling out ~90% (4365/4811)
      of measured `Shared<T>` cross-module usages; separately, 66%
      (749/1127) of all direct `.hpp` `#include` lines are already
      transitively redundant today, so even eligible forward-declarations
      would mostly have no compile-time effect. No `compiler/**` code
      changed, no self-host diff applicable (survey-only); **§104-20 CLOSED
      2026-07-30** — opt-in `--cpp-mode=fast-build` flag, default path
      diff-empty independently re-verified twice (Driver's `git stash`-based
      baseline + Critic's own from-scratch `git worktree` baseline — both
      confirm only the 9 scoped files differ, purely additive struct-field/
      parameter threading + 1 new dispatch branch), `rake test_compiler_mlc`
      1471/1471 unaffected (rerun independently by Critic too), self-host
      round-trip byte-identical both modes (2 independently-built `mlcc2`
      binaries), measured real C++ compile-time delta on the affected
      files, twice with different methodologies: Driver g++ −3.0%/clang++
      −4.8%, Critic (real `-c` compiles, quiet machine, child-CPU-time)
      g++ −4.22%/clang++ −5.66% — same direction/magnitude, confirms the
      payoff is real but modest, not the dramatic reduction a naive read of
      "removes a 75-arm `std::visit`" might suggest; **§104-22 tooling
      CLOSED same day** — 2 new scripts
      (`compiler/scripts/bootstrap-fast.sh`/`bootstrap-full.sh`), no `.mlc`
      touched; found and fixed a link failure in the review's own literal
      Шаг 22 snippet (missing runtime `.cpp` sources); both scripts verified
      exit 0 independently twice (Driver + Critic, fresh `mlcc` rebuilds
      each time), `bootstrap-full.sh` printed `STAGE IDENTICAL` from 2
      independent genuine `mlcc`→`mlcc2` self-host round-trips; Critic also
      exercised the missing-`mlcc` fail-fast path (exit 1, correct message)
      not explicitly scoped by the review; **§104-23 (determinism checks)
      Decision+Red+Green done same day** — new
      `compiler/scripts/check-determinism.sh` + `rake test_determinism`
      task, no `.mlc` touched; ran `--dump-mir`/`--dump-sem` twice each
      against the real `compiler/main.mlc` — both `MIR DETERMINISTIC`/`SEM
      DETERMINISTIC` printed on the first attempt (38441/3753-line dumps,
      non-trivial), no hidden `Map`-ordering non-determinism found; Critic
      independently re-ran everything from a fresh `mlcc` rebuild — exact
      dump-size match, `rake test_determinism` reproduced, fail-fast path and
      Map-free printer claim both independently re-confirmed — **§104-23
      CLOSED 2026-07-31, this finishes the entire authorized Wave 1 list**;
      queue head → **§101 Driver STEP=0**
      (`EDITOR_CARET_VISUAL_ROW_CACHE`); only
      §104-1/2/3/12(done)/13(done)/14(done)/15(done)/16(done)/18(done)/19(rejected)/20(done)/22(done)/23(done)
      pulled forward here, Wave 2 (§104-6..11) stays after §103, Wave 3 stays
      gated; [TRACK_COMPILER_ARCHITECTURE_HYGIENE](agent/TRACK_COMPILER_ARCHITECTURE_HYGIENE.md))**
  → **EDITOR_CARET_VISUAL_ROW_CACHE (§101, CLOSED 2026-07-31 —
      caret pixel-budget cache added, Critic-audited, no false-done;
      see §499 row and [TRACK_EDITOR_CARET_VISUAL_ROW_CACHE](agent/TRACK_EDITOR_CARET_VISUAL_ROW_CACHE.md))**
  → EDITOR_TERMINAL (**closed** 2026-07-31: §102 epic complete — §102a `TERMINAL_LIBVTERM_FFI`,
      §102b `TERMINAL_PTY_SPAWN`, §102c `TERMINAL_CELL_GRID_RENDER`, §102d `TERMINAL_INPUT_FORWARD`,
      §102e `TERMINAL_RESIZE_SCROLLBACK`, §102f `TERMINAL_PANEL_INTEGRATION`,
      §102g `TERMINAL_PERF_BUDGET` all CLOSED and Critic-audited (git `44b66105`+);
      ceilings DOC_TOTAL_US_MAX=1500000 / TERM_TOTAL_US_MAX=2000000 measured, not guessed;
      `run_ux_gate.sh` ×2 = 115/115; `dev_gate_fast.sh` 1471/0
      → [agent/TRACK_EDITOR_TERMINAL](agent/TRACK_EDITOR_TERMINAL.md))
  → EDITOR_IDLE_POINTER_DIRTY (**closed** 2026-08-01, shipped `47b4f134`: `pointer_dirty` split,
      out-of-window jitter ignored, idle wait before layout, `layout_skip` reusing cached rows,
      `SwapInterval(1)` only when `MLC_GLFW_VISIBLE`; **no dedicated gate, no Critic turn** —
      gaps disclosed, residuals owned by §107e/§107i/§107j/§107r + backlog B7
      → [agent/TRACK_EDITOR_IDLE_POINTER_DIRTY](agent/TRACK_EDITOR_IDLE_POINTER_DIRTY.md))
  → EDITOR_RETAINED_PAINT (**closed** 2026-08-03 — §108a–§108d Critic-audited; hover CPU gate L1+L2 floor=10
      → [agent/TRACK_EDITOR_RETAINED_PAINT](agent/TRACK_EDITOR_RETAINED_PAINT.md))
  → EDITOR_HYGIENE (§107, **CLOSED** 2026-08-03 — §107a–§107r Critic-audited;
      source audit `mlc-support/responses/editor_hygiene_audit_20260801_103839.md`):
      §107a–§107r CLOSED
        → §107e EDITOR_DOCUMENT_VERSION (CLOSED)
        → §107f EDITOR_INCREMENTAL_LINE_INDEX (CLOSED)
        → §107g EDITOR_TERMINAL_TEARDOWN (CLOSED)
        → §107h EDITOR_SHAPE_SEGMENT_BUDGET (CLOSED)
        → §107i EDITOR_SPANS_TICK_UNDER_LAYOUT_SKIP (CLOSED)
        → §107j EDITOR_VISIBLE_ROWS_SINGLE_COLLECT (CLOSED)
        → §107k EDITOR_TERMINAL_DAMAGE_REPAINT (CLOSED)
        → §107l EDITOR_INDENT_REPLACE_NO_STRINGIFY (CLOSED)
        → §107m EDITOR_OPEN_SIZE_GUARD (CLOSED)
        → §107n EDITOR_PROBE_GEOMETRY_PARITY (CLOSED)
        → §107o EDITOR_VIEWPORT_RECT_SINGLE (CLOSED)
        → §107p EDITOR_TOOLBAR_COMMAND_TABLE (CLOSED)
        → §107q EDITOR_DRAW_OPS (CLOSED)
        → §107r EDITOR_UX_GATE_BEHAVIORAL (CLOSED)
      (P2 backlog B1–B11 lives in the track file, not in this queue)
      → [agent/TRACK_EDITOR_HYGIENE](agent/TRACK_EDITOR_HYGIENE.md))
  → **MLC_SCRIPT_VM Phase 1 (§103f `SCRIPT_VM_HEAP_GC_ARENA` — QUEUE HEAD; §103e CLOSED;
      Phase 2–5 of `MLC_SCRIPT_VM.md` §12 remain design-only, NOT authorized
      → [agent/TRACK_MLC_SCRIPT_VM](agent/TRACK_MLC_SCRIPT_VM.md))**
  → COMPILER_ARCHITECTURE_HYGIENE Wave 2 (§104 — Wave 1 CLOSED; Wave 2 **queued last**,
      behind §103a; pulling it ahead requires a new explicit user override
      → [agent/TRACK_COMPILER_ARCHITECTURE_HYGIENE](agent/TRACK_COMPILER_ARCHITECTURE_HYGIENE.md))
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
| Codegen: строки vs CppAST | `expr.mlc` gone; GenStmts/ModuleOut AST; residual Fragment/print bridges. [TRACK_CODEGEN_CPPAST_ONLY](archive/tracks/TRACK_CODEGEN_CPPAST_ONLY.md) closed (Critic OK) | 0% string bridges |
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
