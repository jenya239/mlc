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

## Status: **active** — STEP=1 done; STEP=2 next

## Next step

**STEP=2** — allowlist / soft-skip for sysdeps (`pkg-config glfw3`, fonts).

## Goal

Каждый `.mlc` файл в `misc/examples/` и `misc/gui/`, у которого есть
`fn main()`, должен компилироваться (`mlcc -o <tmp> <file>`) и линковаться
(`compiler/build_bin.sh`) в CI/`regression_gate.sh`. Не запускать GUI/GL
файлы headless-принудительно (многим нужен `pkg-config glfw3`/дисплей) —
достаточно compile+link smoke, не exit-code запуска (тот уже покрыт
индивидуальными `scripts/run_*_gate.sh` там где они есть).

## Известный факт (2026-07-11, найдено вручную, не Driver-турном)

Пофиксено вручную в этой сессии (не через очередь): `gui_button_demo.mlc`
переписан — локальные `extern fn glfw_gl_context_*`/`gl_viewport`/
`gl_clear`/`gl_clear_color_f64` заменены на `import { ... } from
'../../lib/mlc/common/stdlib/gl/gl_window.mlc'` / `.../glad_gl.mlc`. Проверено:
`scripts/run_gui_button_demo.sh` зелёный. **Driver должен считать этот файл
уже исправленным** (`5fb3b2ce`) — не ределать, но включить в sweep как
целевой файл.

`misc/examples/text_window_demo.mlc` — тем же паттерном **не задет**
(не импортирует `gui/input`), проверено вручную, компилируется.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | `scripts/run_examples_compile_sweep.sh` — discover `fn main`, `mlcc`+`build_bin.sh`, summary; env `EXAMPLES_SWEEP_DRY`/`ONLY`/`MAX` | **done** (2026-07-12) |
| 2 | Allowlist / soft-skip для sysdeps (`pkg-config glfw3`, fonts) — warning, не fail; паттерн как `run_gui_button_demo.sh` | pending |
| 3 | Один ручной прогон sweep → список failures; каждый сломанный файл — отдельный fix sub-step (prefer `import` over duplicate `extern fn ... from header`) | pending |
| 4 | Wire sweep в `scripts/regression_gate.sh` (после существующих gates) | pending |
| 5 | Docs: одна строка в `README.md` и/или `DEVELOPMENT.md` про sweep-gate | pending |
| 6 | Verify-gate: `regression_gate.sh` exit 0; self-host N/A (scripts/docs only) | pending |

### STEP=1 done notes

- Dry: 107 entries, `gui_button_demo.mlc` present.
- Smoke: `EXAMPLES_SWEEP_ONLY='minimal.mlc gui_button_demo.mlc'` → ok=2 fail=0.

### STEP=2 sub-steps (Driver)

1. Detect missing `glfw3` / optional font paths; classify GUI/GL examples that need them.
2. Soft-skip those with warning when deps absent; still hard-fail compile errors on machines with deps.
3. Document skip reasons in script stderr + optional allowlist file if needed.

## Out of scope

- Headless-запуск/визуальная проверка GUI демо (уже покрыто отдельными
  `run_*_gate.sh` для тех файлов, у которых они есть).
- Новые демо-файлы (`gui_button_interactive_demo.mlc`, `text_dashboard_demo.mlc`,
  `http_server_forever_demo.mlc` — добавлены вручную в этой сессии, не
  Driver-турном) — попадут под sweep автоматически, отдельных шагов не
  требуют.
