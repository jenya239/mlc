# Track: доверенная сессия Script VM

Parent: PLAN §113. Не продолжение [TRACK_MLC_SCRIPT_VM](TRACK_MLC_SCRIPT_VM.md) (§103 закрыт).
Не очередь `SCRIPT_VM_QUEUE_100` / `200` / `300` / `440`: все четыре `queue_complete`, не открывать.

Авторизация: 2026-10-24. Доверенный гость в процессе интерфейса. Отдельный процесс — другой профиль, не этот трек.

## Status: **done** — STEP=1–4

Гость окна — одна `GalleryGuest`: куча и `GuestHalt` живут между кадрами, `lower_source` только при открытии. **Ping** вызывает `gallery_guest_ping` → `script_frame_accept_label_text`, ревизия повреждения кадра растёт, раскладка идёт через `frame_request`. `guest_timer_poll` сравнивает срок с переданными часами и не спит. Квант стоит на основном `continue_halt`; `run_slice` вызывает его, отдельного интерпретатора больше нет. Лимит инструкций галереи — 100000, квант — 256.

Проверено: `host_label_unit`, `host_guest_unit`, `guest_quantum_unit`, `persistent_instance_unit`, `execution_slice_unit`, `execution_frame_yield_unit`, `suspended_roots_unit`, `execution_budget_unit`, `execution_slice_differential_unit`, `run_script_vm_file_size.rb`, `run_textui_file_size.rb`. Галерея собрана (`tmp/textui_trusted_session/bin`). Hover: `hover_damage_min_us=281`, `hover_full_min_us=3009`.

## Уже есть в дереве

- `HostCall`: `timer.after` (индекс 0) и `label.set_text` (индекс 1).
- `GalleryGuest` держит `GuestHalt`, кучу и прототипы. Окно открывается с `VM: idle`. **Ping** принимает ту же сессию и пишет `VM: scripted`.
- `instance_call_export` на ожидании возвращает `emb_waiting` и ту же кучу.
- `guest_timer_poll` возобновляет гостя, когда переданные часы дошли до срока.
- `continue_halt` останавливается по `quantum_limit`. `run_slice` идёт через него.
- Лимит инструкций галереи конечный (`gallery_instruction_limit`), не 0.

## Non-goals

- Процесс-песочница, файловые и сетевые пространства имён, IPC недоверенного гостя.
- `trusted-in-process` как запасной путь при отказе песочницы.
- Quickening, shapes, inline caches, baseline JIT, tier-2, GLSL.
- Очереди 100–440, SVGUI-30, generics 361–374, `misc/editor/`.
- Второй интерпретатор. Квант и `HostCall` живут в одном исполнительном пути.

## Шаги

Один шаг — один коммит. Файлы `script_vm/` и `misc/textui/` не больше 400 строк (`scripts/run_script_vm_file_size.rb`, `scripts/run_textui_file_size.rb`). `app.mlc` только связывает кадр.

### STEP=1. Сессия переживает кадр

`GuestHalt`, куча и прототипы остаются у владельца окна. Повторный приём того же ожидания не создаёт новую машину.

`instance_call_export` больше не превращает `Waiting` в ошибку типа: вызывающий получает ожидание и ту же кучу.

Готово, когда юнит дважды смотрит одну сессию: первый раз `waiting` на `label.set_text`, второй раз та же куча и тот же аргумент, без нового `lower_source`.

```bash
ruby scripts/run_script_vm_unit.rb script_vm/tests/host_label_unit.mlc
ruby scripts/run_script_vm_file_size.rb
```

`misc/textui/app.mlc` этот шаг не меняет.

### STEP=2. Ping продолжает ту же сессию

Окно открывается с меткой `VM: idle`. **Ping** продолжает уже запущенного гостя. `script_frame_accept_label_text` пишет `VM: scripted`. Подпись кадра получает новый `script_damage_revision`, следующий кадр делает раскладку через существующий `frame_request`.

Поток кадра не вызывает `lower_source` на каждый кадр и не запускает вторую машину.

Готово, когда юнит кадра видит смену текста и revision, а живое окно после **Ping** показывает `VM: scripted`.

Правка цикла кадра, paint, submit или hit требует:

```bash
bash scripts/run_textui_hover_bench.sh
```

Потолок `hover_damage_min_us` ≤ 1000 и `hover_damage_min_us * 3 < hover_full_min_us`. Потолок не поднимать. `glfw_gl_swap_interval(0)` в галерее не менять.

```bash
ruby scripts/run_textui_file_size.rb
```

### STEP=3. `timer_after` завершают часы

Гость `timer_after(100)` остаётся `waiting`, пока не прошло 100 мс от момента приостановки. Кадр сравнивает срок с монотонными часами и тогда вызывает `resume_guest`. Кадр не вызывает блокирующее ожидание.

Юнит, который сам подставляет ответ без часов, этот контракт не закрывает.

Готово, когда юнит с подменёнными часами не возобновляет гостя на 99 мс и возобновляет на 100 мс. Повторный ответ с тем же epoch отвергается, как в `host_guest_unit`.

```bash
ruby scripts/run_script_vm_unit.rb script_vm/tests/host_guest_unit.mlc
ruby scripts/run_script_vm_file_size.rb
```

### STEP=4. Квант, затем продолжение

Гость с циклом без host-вызова останавливается по кванту инструкций в состоянии, которое следующий кадр продолжает. Это не `instruction_limit` как окончательная ошибка и не лимит 0 на потоке интерфейса.

Тот же путь исполняет `HostCall`. Отдельный `executor_slice` без `HostCall` не остаётся вторым исполнителем.

Готово, когда юнит делает два кванта одной программы и получает тот же результат, что один прогон с лимитом больше длины программы. Программа с `set_label_text` по-прежнему приходит в `waiting`, а не в ошибку лимита, если квант больше её префикса.

```bash
ruby scripts/run_script_vm_unit.rb script_vm/tests/host_label_unit.mlc
ruby scripts/run_script_vm_file_size.rb
```

Галерея после этого шага не вызывает гостя с лимитом 0.
