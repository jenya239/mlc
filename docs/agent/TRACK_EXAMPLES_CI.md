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

## Status: open, высокий приоритет

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
уже исправленным** (commit будет виден в `git log -- misc/examples/gui_button_demo.mlc`)
— не ределать, но включить в новый sweep-скрипт (Step 1) как один из целевых
файлов, чтобы регрессия не повторилась незаметно.

`misc/examples/text_window_demo.mlc` — тем же паттерном **не задет**
(не импортирует `gui/input`), проверено вручную, компилируется.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | `scripts/run_examples_compile_sweep.sh` — итерирует `misc/examples/*.mlc` + `misc/gui/*.mlc` с `fn main()`, для каждого: `mlcc -o <tmp>` + `build_bin.sh`; собирает список failures; exit 1 если непустой список **и** файл не в explicit allowlist (см. Step 2) | pending |
| 2 | Allowlist для файлов, которым для линковки нужны системные зависимости, которых может не быть в CI (`pkg-config glfw3`, конкретные шрифты в `/usr/share/fonts/...`) — skip с warning, не fail, тот же паттерн что `run_gui_button_demo.sh` (`pkg-config --exists glfw3` guard) | pending |
| 3 | Запустить sweep один раз вручную, зафиксировать список текущих failures (кроме известного Step в п.1, если он уже исправлен) — если найдутся другие сломанные файлы, каждый — отдельный sub-step с минимальным фиксом (тот же паттерн: заменить локальный `extern fn ... from header` на `import` там, где символ уже объявлен транзитивно) | pending |
| 4 | Wire `run_examples_compile_sweep.sh` в `scripts/regression_gate.sh` (после существующих gate-шагов, не блокирует TRACK-специфичные gate) | pending |
| 5 | Docs: `README.md`/`DEVELOPMENT.md` — одна строка про новый sweep-gate | pending |
| 6 | Verify-gate: `scripts/regression_gate.sh` exit 0; self-host diff не требуется (docs+scripts only, не `compiler/`) | pending |

## Out of scope

- Headless-запуск/визуальная проверка GUI демо (уже покрыто отдельными
  `run_*_gate.sh` для тех файлов, у которых они есть).
- Новые демо-файлы (`gui_button_interactive_demo.mlc`, `text_dashboard_demo.mlc`,
  `http_server_forever_demo.mlc` — добавлены вручную в этой сессии, не
  Driver-турном) — попадут под sweep автоматически, отдельных шагов не
  требуют.
