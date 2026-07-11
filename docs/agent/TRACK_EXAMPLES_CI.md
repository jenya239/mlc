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

## Status: **active** — STEP=3 done; STEP=7 next (fix clusters before wire)

## Next step

**STEP=7** — fix cluster A: `file not found: …/misc/examples/.mlc` (bare `import Env` / similar).

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
| 7 | Fix cluster A: `file not found …/.mlc` (`crypto_sha256_demo`, `dynrecord_demo`, `env_log_demo`, `postgres_select_demo`, `tcp_echo_demo`, `validate_demo`) | pending |
| 8 | Fix cluster B: `undefined: to_string` / related demos (`demo`, `generics_demo`, `list_comprehension_demo`, `loops_demo`, `match_demo`, `string_operations_demo`, `unary_ops_demo`) | pending |
| 9 | Fix cluster C: ABI smokes needing link flags (`crypto_mlc_abi_smoke`, `postgres_mlc_abi_smoke`) — soft-skip or document deps | pending |
| 10 | Fix cluster D: type/demo one-offs (`array_hof_demo`, `debug_*`, `index_access_demo`, `pointer_types_demo`, `safety_demo`, `todo_summary`) | pending |
| 11 | Fix cluster E: `vm_*` C++ codegen/link (`vm_match_*`, `vm_pop`, `vm_question*`, `vm_record_match`) | pending |
| 4 | Wire sweep в `scripts/regression_gate.sh` (только после fail=0 или осознанный allowlist) | pending |
| 5 | Docs: одна строка про sweep-gate | pending |
| 6 | Verify-gate: `regression_gate.sh` exit 0 | pending |

### STEP=3 inventory (2026-07-12)

`bash scripts/run_examples_compile_sweep.sh` → **ok=77 fail=30 skip=0**.

Failures (basename):

```
array_hof_demo.mlc
crypto_mlc_abi_smoke.mlc
crypto_sha256_demo.mlc
debug_fold.mlc
debug_status.mlc
demo.mlc
dynrecord_demo.mlc
env_log_demo.mlc
generics_demo.mlc
index_access_demo.mlc
list_comprehension_demo.mlc
loops_demo.mlc
match_demo.mlc
pointer_types_demo.mlc
postgres_mlc_abi_smoke.mlc
postgres_select_demo.mlc
safety_demo.mlc
string_operations_demo.mlc
tcp_echo_demo.mlc
todo_summary.mlc
unary_ops_demo.mlc
validate_demo.mlc
vm_match_guard.mlc
vm_match_nested.mlc
vm_match_or.mlc
vm_match_variant.mlc
vm_pop.mlc
vm_question_err.mlc
vm_question.mlc
vm_record_match.mlc
```

Кластеры (первый error из `tmp/examples_sweep/<stem>/sweep.log`):

| Cluster | Symptom | Files |
|---------|---------|-------|
| A | `file not found: …/misc/examples/.mlc` | crypto_sha256_demo, dynrecord_demo, env_log_demo, postgres_select_demo, tcp_echo_demo, validate_demo |
| B | `undefined: to_string` | demo, generics_demo, list_comprehension_demo, loops_demo, match_demo, string_operations_demo, unary_ops_demo |
| C | ABI header `#error` / missing lib | crypto_mlc_abi_smoke, postgres_mlc_abi_smoke |
| D | misc type/undefined | array_hof_demo, debug_fold, debug_status, index_access_demo, pointer_types_demo, safety_demo, todo_summary |
| E | C++ from vm_* (Result/match/pop) | vm_match_guard, vm_match_nested, vm_match_or, vm_match_variant, vm_pop, vm_question, vm_question_err, vm_record_match |

### STEP=7 sub-steps (Driver)

1. Inspect one A-file import (`env_log_demo`: bare `import Env::{…}`) vs working stdlib import path.
2. Fix all six A-files the same way (explicit path or package root).
3. Re-sweep `EXAMPLES_SWEEP_ONLY` on those six → ok=6.

## Out of scope

- Headless GUI run.
- New demos auto-covered by sweep.
