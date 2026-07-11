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

## Status: **active** — STEP=8 done; STEP=9 next

## Next step

**STEP=9** — fix cluster C: ABI smokes (`crypto_mlc_abi_smoke`, `postgres_mlc_abi_smoke`) soft-skip/deps.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | `scripts/run_examples_compile_sweep.sh` | **done** (2026-07-12) |
| 2 | Soft-skip sysdeps + allowlist | **done** (2026-07-12) |
| 3 | Full sweep inventory | **done** (2026-07-12) — ok=77 fail=30 skip=0 |
| 7 | Fix cluster A: `…/examples/.mlc` bare Ruby imports | **done** (2026-07-12) |
| 8 | Fix cluster B: `to_string` demos → `.to_string()` + mlcc syntax | **done** (2026-07-12) |
| 9 | Fix cluster C: ABI smokes needing link flags (`crypto_mlc_abi_smoke`, `postgres_mlc_abi_smoke`) — soft-skip or document deps | pending |
| 10 | Fix cluster D: type/demo one-offs (`array_hof_demo`, `debug_*`, `index_access_demo`, `pointer_types_demo`, `safety_demo`, `todo_summary`) | pending |
| 11 | Fix cluster E: `vm_*` C++ codegen/link (`vm_match_*`, `vm_pop`, `vm_question*`, `vm_record_match`) | pending |
| 4 | Wire sweep в `scripts/regression_gate.sh` (только после fail=0 или осознанный allowlist) | pending |
| 5 | Docs: одна строка про sweep-gate | pending |
| 6 | Verify-gate: `regression_gate.sh` exit 0 | pending |

### STEP=8 done notes

- Replaced free `to_string(...)` with `.to_string()`; rewrote demos for mlcc (`string`, `let mut`, match `{…}`, `x =>`).
- Verify: ONLY seven → ok=7 fail=0.

### STEP=9 sub-steps (Driver)

1. Confirm soft-skip for sodium/libpq covers abi smokes (or extend).
2. If headers present: ensure compile+link; else SKIP only.
3. Re-sweep those two → fail=0.