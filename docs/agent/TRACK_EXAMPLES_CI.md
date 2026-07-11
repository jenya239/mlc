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

## Status: **active** — STEP=2 done; STEP=3 next

## Next step

**STEP=3** — full sweep run → failure list; fix broken examples as needed.

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
| 2 | Soft-skip: missing `glfw3`/`freetype2`/`harfbuzz`/font path; optional `examples_compile_sweep_allowlist.txt` | **done** (2026-07-12) |
| 3 | Один ручной прогон sweep → список failures; каждый сломанный файл — отдельный fix sub-step (prefer `import` over duplicate `extern fn ... from header`) | pending |
| 4 | Wire sweep в `scripts/regression_gate.sh` (после существующих gates) | pending |
| 5 | Docs: одна строка в `README.md` и/или `DEVELOPMENT.md` про sweep-gate | pending |
| 6 | Verify-gate: `regression_gate.sh` exit 0; self-host N/A (scripts/docs only) | pending |

### STEP=1 done notes

- Dry: 107 entries, `gui_button_demo.mlc` present.
- Smoke: `EXAMPLES_SWEEP_ONLY='minimal.mlc gui_button_demo.mlc'` → ok=2 fail=0.

### STEP=2 done notes

- Soft-skip via `soft_skip_reason` (pkg-config + `/usr/share/fonts/...` paths + allowlist file).
- Verify: fake pkg-config → SKIP gui/freetype, RUN minimal; allowlist → SKIP gui + OK minimal.

### STEP=3 sub-steps (Driver)

1. Full `bash scripts/run_examples_compile_sweep.sh` (deps present on this machine).
2. Record failure list in TRACK; if empty → mark STEP=3 done and proceed.
3. If failures: one fix per subsequent turn (or batch tiny identical fixes).

## Out of scope

- Headless-запуск/визуальная проверка GUI демо (уже покрыто отдельными
  `run_*_gate.sh` для тех файлов, у которых они есть).
- Новые демо-файлы (`gui_button_interactive_demo.mlc`, `text_dashboard_demo.mlc`,
  `http_server_forever_demo.mlc` — добавлены вручную в этой сессии, не
  Driver-турном) — попадут под sweep автоматически, отдельных шагов не
  требуют.
