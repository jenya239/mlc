# Track: debugging story (stack trace → MLC source)

Parent: [../PLAN.md](../PLAN.md). Trigger: обзор пробелов 2026-07-10 — нет
ни одного упоминания debugger/source map/stack trace во всей документации
проекта, при том что кодовая база на MLC (сам `compiler/`) уже 50+ модулей.

## Status: **open, низкий приоритет, research** — до появления реального
пользовательского кода на MLC вне самого компилятора эффект ограничен;
поднять приоритет, когда появится первый внешний проект на MLC
(`TEXT_RENDERING`/`STDLIB_BACKEND` — оба кандидаты).

## Проблема

Сгенерированный C++ — единственный слой, который видит компилятор/ОС при
crash. Сегодня:

- `panic`/necaught exception → C++ stack trace (если вообще есть символы) —
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
| 1 | Design: формат маппинга — `#line <N> "<original.mlc>"` перед каждым сгенерированным stmt/expr, у которого есть span в SemanticIR/AST. Решить гранулярность (per-statement, не per-expression — иначе объём `.cpp` и время компиляции растут). | pending |
| 2 | Codegen (Ruby): эмит `#line` в `lib/mlc/backends/cpp/codegen.rb` для statement-level nodes, у которых есть `span`/`line`. | pending |
| 3 | Codegen (self-hosted): аналогично в `compiler/codegen/`, после верификации на Ruby-стороне. | pending |
| 4 | Проверка: собрать программу с `-g`, вызвать `abort()`/panic внутри, `gdb`/`addr2line` на бинарнике — убедиться, что репортится `.mlc`-имя файла и строка, не `.cpp`. | pending |
| 5 | Документация: раздел в `docs/MLC.md` или новый `docs/DEBUGGING.md` — как включить (`-g` уже должен работать "из коробки" после STEP=2-3, без флага компилятора mlc). Verify-gate + close. | pending |

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
