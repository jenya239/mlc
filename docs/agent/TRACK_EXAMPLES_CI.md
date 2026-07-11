# Track: Compile-smoke coverage for misc/examples + misc/gui

Parent: [../PLAN.md](../PLAN.md), [../GUI.md](../GUI.md).
Trigger: 2026-07-11 (несвязанная задача — демо для пользователя). `misc/examples/gui_button_demo.mlc`
не компилировался вообще (`cannot define or redeclare 'glfw_gl_context_begin'
... because namespace 'gui_button_demo' does not enclose namespace
'gl_window'`) — файл, покрытый собственным скриптом
(`scripts/run_gui_button_demo.sh`), но не вызываемым ни из
`scripts/regression_gate.sh`, ни из CI. Регрессия молчала неизвестное
число turn'ов. Причина: локальный `extern fn ... from "<header>"` в самом
демо дублирует функцию, уже объявленную транзитивно через `import` другого
модуля (`misc/gui/input.mlc` → `lib/mlc/common/stdlib/gl/gl_window.mlc`) —
см. [TRACK_FFI_EXTERN_DEDUP](TRACK_FFI_EXTERN_DEDUP.md) для корневой причины.
Этот трек — про **покрытие**, не про сам баг компилятора.

## Status: **active** — STEP=7 done; STEP=8 next

## Next step

**STEP=8** — fix cluster B: `undefined: to_string` demos.

## Goal

Каждый `.mlc` файл в `misc/examples/` и `misc/gui/`, у которого есть
`fn main()`, должен компилироваться (`mlcc -o <tmp> <file>`) и линковаться
(`compiler/build_bin.sh`) в CI/`regression_gate.sh`. Не запускать GUI/GL
файлы headless-принудительно (многим нужен `pkg-config glfw3`/дисплей) —
достаточно compile+link smoke, не exit-code запуска (тот уже покрыт
индивидуальными `scripts/run_*_gate.sh` там где они есть).

## Известный факт (2026-07-11, найдено вручную, не Driver-турном)

Пофиксено вручную: `gui_button_demo.mlc` (`5fb3b2ce`) — не ределать.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | `scripts/run_examples_compile_sweep.sh` | **done** (2026-07-12) |
| 2 | Soft-skip sysdeps + allowlist | **done** (2026-07-12) |
| 3 | Full sweep inventory | **done** (2026-07-12) — ok=77 fail=30 skip=0 |
| 7 | Fix cluster A: `…/examples/.mlc` bare Ruby imports | **done** (2026-07-12) |
| 8 | Fix cluster B: `undefined: to_string` / related demos (`demo`, `generics_demo`, `list_comprehension_demo`, `loops_demo`, `match_demo`, `string_operations_demo`, `unary_ops_demo`) | pending |
| 9 | Fix cluster C: ABI smokes needing link flags (`crypto_mlc_abi_smoke`, `postgres_mlc_abi_smoke`) — soft-skip or document deps | pending |
| 10 | Fix cluster D: type/demo one-offs (`array_hof_demo`, `debug_*`, `index_access_demo`, `pointer_types_demo`, `safety_demo`, `todo_summary`) | pending |
| 11 | Fix cluster E: `vm_*` C++ codegen/link (`vm_match_*`, `vm_pop`, `vm_question*`, `vm_record_match`) | pending |
| 4 | Wire sweep в `scripts/regression_gate.sh` (только после fail=0 или осознанный allowlist) | pending |
| 5 | Docs: одна строка про sweep-gate | pending |
| 6 | Verify-gate: `regression_gate.sh` exit 0 | pending |

### STEP=7 done notes

- Converted A-files to mlcc imports (`from 'Env'|'Log'|'Validate'|'Tcp'` or relative stdlib paths).
- Soft-skip: missing libsodium / libpq.
- `dynrecord.mlc` stdlib: `std/…` → relative `./map.mlc` / `../core/option.mlc`.
- `dynrecord_demo.mlc` allowlisted (mlcc segfault on dynrecord graph).
- Verify: ONLY six → ok=3 fail=0 skip=3.

### STEP=8 sub-steps (Driver)

1. Inspect one B-file (`demo.mlc`) — how `to_string` is used vs Conv/builtin.
2. Fix all seven B-files the same way.
3. Re-sweep `EXAMPLES_SWEEP_ONLY` on those seven → fail=0.

## Out of scope

- Headless GUI run.
- New demos auto-covered by sweep.
