# Track: debugging story (stack trace → MLC source)

Parent: [../PLAN.md](../PLAN.md). Trigger: обзор пробелов 2026-07-10 — нет
ни одного упоминания debugger/source map/stack trace во всей документации
проекта, при том что кодовая база на MLC (сам `compiler/`) уже 50+ модулей.

## Status: **active** (2026-07-13) — STEP=3 self-hosted `#line` emit next

Activated by Planner after TEXT_GLYPH_CACHE_SCALING Critic OK. Queue ahead of
`GUI_CANVAS_GRAPH` (Phase A still pending; do not start until this track
closes or blocks).

## Next step

**STEP=3** — Codegen (self-hosted): emit `#line` in `compiler/codegen/` per
Decision (STEP=1).

### STEP=2 done (2026-07-13)

- `Context#lower_statement` + `attach_line_directive` / `leading_trivia`.
- Bypass paths: single-expr `return`, block result, `if_rule` / expr
  `block_rule` result stmts.
- Test: `test/mlc/line_directive_test.rb` (with/without `filename:`).

## Decision (STEP=1) — frozen 2026-07-13

### Span source of truth

| Layer | Location | Fields used for `#line` |
|-------|----------|-------------------------|
| Self-hosted AST | `compiler/frontend/ast.mlc` `Span` | `file: string`, `line: i32` (1-based); unknown = `span_unknown()` → `file==""`, `line==0` |
| Self-hosted SemanticIR | `compiler/ir/semantic_ir.mlc` | Every `SemanticStatement*` carries trailing `Span`; exprs via `sexpr_span`. No `sstmt_span` helper yet — STEP=3 may add one mirroring `sexpr_span` / `sdecl_span`. |
| Ruby AST | `lib/mlc/source/ast/nodes.rb` `SourceOrigin` on nodes (`attr_reader :origin`) | `origin.file`, `origin.line` (1-based). Missing/`nil` line → treat as unknown (skip). |

### Emit form

```
#line <N> "<path>"
```

- `<N>` = `span.line` (MLC 1-based line).
- `"<path>"` = `span.file` as stored by the parser (relative or absolute — do not rewrite). Escape `\` and `"` for a valid C string literal.
- One directive immediately **before** the C++ text for that statement (same indentation level as the statement is fine; preprocessor ignores leading whitespace).

### Granularity

- **Per `SemanticStatement`** in function / block bodies (Let, LetPattern, LetConst, Expr, Return, Break, Continue, and any future stmt variants with Span).
- **Not** per subexpression inside a statement (avoids `#line` spam and compile-time bloat).
- Nested blocks (`if`/`while`/`for`/`match` arms, expression-blocks lowered to stmts): still one `#line` per nested statement when that statement is emitted.
- Single-expression function bodies lowered as `return <expr>;`: one `#line` from the **expression** span (or fn decl span if expr span unknown) immediately before that `return`.

### Skip rules

Emit **nothing** when:

- `line <= 0`, or
- `file` is empty / missing (`nil` on Ruby),

i.e. `span_unknown()` and synthetic IR without source. Never emit `#line 0` or `#line 1 ""`.

### Hook points (for STEP=2 / STEP=3)

| Compiler | Insert `#line` when |
|----------|---------------------|
| Ruby | Lowering each statement in `lib/mlc/backends/cpp/` (statement rules / block lower that produce `raw_statement` or AST stmt nodes) — prepend directive string or a dedicated printer node. |
| Self-hosted | `compiler/codegen/stmt_cpp.mlc` `gen_stmts_cpp` / `print_cpp_statement_line` (or new `CppLineDirective` in `compiler/cpp_ir/cpp_ast.mlc`) so every printed stmt can be preceded by `#line`. |

Default: **always on** when span is known — no mlc CLI flag in this track (`-g` is a C++ compile flag for DWARF; `#line` is independent and cheap).

### Non-goals (unchanged)

Full debugger; MIR VM stacks; IDE breakpoint UI guarantees — see **Out of scope**.

## Проблема

Сгенерированный C++ — единственный слой, который видит компилятор/ОС при
crash. Сегодня:

- `panic`/uncaught exception → C++ stack trace (если вообще есть символы) —
  строки/функции C++ codegen, не строки `.mlc`. Пользователь видит
  `mlc_main::Handler_process(Handler const&, ...)`, не строку в своём файле.
- Нет `#line`-директив в сгенерированном `.cpp`, связывающих C++ строку с
  исходной `.mlc` строкой (стандартный механизм C preprocessor, GCC/Clang
  понимают `#line N "file.mlc"` в диагностике и в DWARF).
- Диагностики checker (`E0xx`) уже носят span (`docs/PLAN.md` метрика
  "Диагностики со span: частично") — инфраструктура span в AST/SemanticIR
  уже есть, просто не прокинута в codegen как `#line`.

## Задача трека

Минимальный, дёшево реализуемый шаг: `#line`-директивы. Полный debugger —
явно out of scope (см. ниже).

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Design: формат `#line <N> "<original.mlc>"`; гранулярность per-statement; skip unknown span. Записать **Decision** в этот TRACK. | **done** (2026-07-13) |
| 2 | Codegen (Ruby): эмит `#line` в `lib/mlc/backends/cpp/` для statement-level nodes со span. | **done** (2026-07-13: `Context#attach_line_directive`; `line_directive_test.rb`) |
| 3 | Codegen (self-hosted): аналогично в `compiler/codegen/`, после Ruby. | pending |
| 4 | Проверка: программа с `-g`, `abort()`/panic, `gdb`/`addr2line` → `.mlc` file+line. | pending |
| 5 | Документация (`docs/DEBUGGING.md` или `docs/MLC.md`) + verify-gate + close → Critic. | pending |

## Out of scope (явно, не в этом треке)

- Полноценный source-level debugger (аналог интеграции rust-gdb/lldb с
  DWARF-расширениями типов) — непропорциональный эффорт, не в roadmap.
- Отображение стека для MIR VM (`compiler/vm/`) — у VM интерпретатора нет
  C++ stack frame на MIR-инструкцию, отдельная задача (VM уже носит номера
  MIR-инструкций внутри себя, вопрос в user-facing репортинге — не
  затрагивать в этом треке).
- Watch-выражения, breakpoints по `.mlc`-строке в существующих C++
  debugger'ах "из коробки" через `#line` — заявка на дальнюю перспективу
  (`#line` даёт файл/строку в error-репортинге и backtrace, но не гарантирует
  корректную работу breakpoint UI во всех IDE — не тестировать это в рамках
  трека, только backtrace/addr2line сценарий).
