# Track: debugging story (stack trace → MLC source)

Parent: [../PLAN.md](../PLAN.md). Trigger: обзор пробелов 2026-07-10 — нет
ни одного упоминания debugger/source map/stack trace во всей документации
проекта, при том что кодовая база на MLC (сам `compiler/`) уже 50+ модулей.

## Status: **active** (2026-07-13) — STEP=1 Design Decision next

Activated by Planner after TEXT_GLYPH_CACHE_SCALING Critic OK. Queue ahead of
`GUI_CANVAS_GRAPH` (Phase A still pending; do not start until this track
closes or blocks).

## Next step

**STEP=1** — Design Decision: `#line` mapping format + emit granularity.

Sub-steps for Driver (docs-only; **no** `compiler/` / `lib/mlc/` this step):
1. Confirm span source of truth: self-hosted `Span { file, line, column, … }`
   in `compiler/frontend/ast.mlc` (and SemanticIR `sexpr_span` / stmt spans);
   note Ruby AST equivalent fields for STEP=2.
2. Freeze emit rules in this TRACK under **Decision (STEP=1)**:
   - form: `#line <N> "<path.mlc>"` (N = 1-based MLC line);
   - granularity: **per statement** in function bodies (not per expression);
   - skip when span is unknown (`line == 0` / empty file) — no bogus `#line`;
   - path: use span `file` as stored by parser (relative or absolute as today).
3. Explicit non-goals stay (full debugger, MIR VM stacks, IDE breakpoints).
4. Gate: **Decision** section present in this file; PLAN §15 + queue say
   STEP=1 done → Driver STEP=2.

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
| 1 | Design: формат `#line <N> "<original.mlc>"`; гранулярность per-statement; skip unknown span. Записать **Decision** в этот TRACK. | **pending** (active) |
| 2 | Codegen (Ruby): эмит `#line` в `lib/mlc/backends/cpp/` для statement-level nodes со span. | pending |
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
