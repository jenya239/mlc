# Track: integer overflow semantics

Parent: [../PLAN.md](../PLAN.md), [../MLC.md](../MLC.md) (типы `i32`/`i64`/
`u32`/`u64`). Trigger: обзор пробелов 2026-07-10 — семантика переполнения
целочисленной арифметики не зафиксирована ни в одном документе.

## Status: **open** — STEP=1–2 **done**; STEP=3.1 **done** (Ruby+runtime); STEP=3.2 next (self-hosted)

## Decision (STEP=1, 2026-07-10)

Default for `+` / `-` / `*` / `/` / `%` on integer types:

| Class | Default operators | Notes |
|-------|-------------------|--------|
| Signed (`i32`, `i64`, …) | **Debug:** panic with message on overflow. **Release:** C++ signed overflow remains UB (UBSan-detectable; CI may enable `-fsanitize=undefined`). | Matches PLAN §7 “panic с сообщением”; no silent wrap on signed. |
| Unsigned (`u32`, `u64`, …) | **Wrap** mod 2ⁿ in all builds (C++ defined). | No panic on unsigned wrap. |
| `/` and `%` | **Panic if divisor is 0** in all builds. Signed `MIN / -1` (and `%`) treated as overflow: debug panic / release UB like `*`. | Division-by-zero was also unspecified; fixed here. |

Explicit stdlib methods (not operators) — **STEP=4**, do not block STEP=2–3:
`wrapping_add` / `checked_add` / `saturating_add` (and `-`/`*` analogues).

Rejected for v1 default: always-wrap signed; always-trap in release (hot-path cost);
operator split beyond the debug-panic / release-UB pair above.

## Проблема

MLC транслирует `i32 + i32` напрямую в C++ `int32_t + int32_t`. C++
signed overflow — undefined behavior. Никакой документ проекта (`MLC.md`,
`PLAN.md`) не фиксирует, что должно происходить при `i32::MAX + 1`:

- **UB как в C++** (текущее де-факто поведение, не решение) — быстро, но
  непредсказуемо между `-O0`/`-O2` (UBSan уже используется в проекте для
  concurrency-гейта, но не для обычной арифметики).
- **Wrap** (как Rust `wrapping_add` или Zig `+%`) — предсказуемо,
  требует явных wrapping-операций в codegen (`static_cast` trick или
  `unsigned` промежуточный тип).
- **Trap/panic** (как Swift `+`, как Rust debug-режим) — самое безопасное
  для корректности программ, дороже по codegen (проверка перед каждой
  операцией) и по runtime cost.
- **Разные операторы для разных режимов** (Rust: `+` panics в debug/wraps в
  release, отдельно `checked_add`/`wrapping_add`/`saturating_add`) — самый
  гибкий, но самый сложный вариант для первой версии.

## Задача трека

Зафиксировать одно поведение по умолчанию для `+`/`-`/`*` на `i32`/`i64`/
`u32`/`u64`, задокументировать, реализовать в codegen, покрыть тестом.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Design-решение: выбрать один вариант по умолчанию (рекомендация: panic в debug-сборке/UBSan-детектируемый UB в release — соответствует уже принятому в проекте паттерну "panic с сообщением" из `PLAN.md` §7 метрики "Crashes на невалидном вводе: 0 (panic с сообщением)"); отдельные `wrapping_add`/`checked_add`/`saturating_add` как явные stdlib-методы, не операторы, — второй приоритет, не блокирует STEP=1. | **done** (2026-07-10: see Decision; signed debug-panic / release-UB; unsigned wrap; `/` `%` div0 panic) |
| 2 | Документация: раздел в `MLC.md` — зафиксировать выбранную семантику для `+`/`-`/`*`/`/` (включая целочисленное деление на 0 — тоже не зафиксировано, тот же трек, тот же STEP). | **done** (2026-07-10: `MLC.md` C4 «Целочисленная арифметика») |
| 3.1 | Ruby codegen + runtime: `mlc::arith::checked_{add,sub,mul,div,mod}`; `mlc::io::panic`; signed `+`/`-`/`*` → helpers; unsigned wrap raw; `/` `%` → helpers (div0 all builds). | **done** (2026-07-10) |
| 3.2 | Self-hosted codegen parity (`compiler/` expr visitor / mir_to_cpp). Benchmark if hot path. | pending |
| 3 | Codegen: debug panic checks for signed `+`/`-`/`*` and all-builds div0 for `/` `%`; unsigned wrap unchanged. Ruby then self-hosted. | **partial** (3.1 done; 3.2 pending) |
| 4 | Stdlib: `wrapping_add`/`checked_add`/`saturating_add`/аналоги для `-`/`*`. | pending |
| 5 | Тесты: unit-тесты на overflow/underflow/деление на 0 для всех целочисленных типов; verify-gate self-host (`mlcc`→`mlcc2`→`diff`), `regression_gate.sh`; close. | pending |

## Out of scope

- Произвольная точность (bignum) — не в roadmap языка.
- Checked-арифметика для `f32`/`f64` (NaN/Inf) — отдельная тема, не
  пересекается с целочисленным overflow.
