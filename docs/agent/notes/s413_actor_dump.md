# Шаг 413 — дамп VM и ActorRecord

Решение: расширять `script_vm/vm_dump.mlc`, отдельный `actor_dump` не вводить. Шаг 414 выполняется (red: в дампе нет полей ActorRecord). Шаг 415 закрывает red в том же файле.

## Почему переиспользование

Формат шагов 304/335 — фиксированный текст, не бинарный снимок и не Checkpoint SVGUI-27. `dump_instance(instance: ScriptInstance) -> string` печатает только instance / module / roots / heap / gc. Новых полей у `ScriptInstance` дамп не вводит. Секция actor в этот список не входит, поэтому её можно дописать как опциональный хвост, не ломая уже зафиксированные строки.

`script_vm/vm_dump.mlc` — 166 строк. Лимит `scripts/run_script_vm_file_size.rb` — 400 на `script_vm/**/*.mlc`. Секция actor (mailbox depth, `active_execution`, `waiting_requests`, плюс identity/incarnation/code_epoch/principal/lifecycle/budgets) укладывается в этот файл. Вынос в `script_host/actor_dump.mlc` по карточке 415 нужен только если хвост не влезает; сейчас не влезает.

`ActorRecord` живёт в `script_host/actor.mlc`. У него нет `ScriptInstance`: поле `instance` — `i32`. Связь dump↔scheduler — аргумент контекста (список actors), не поле кучи. `dump_instance` без этого аргумента остаётся текстом шага 335. Порядок строк actor — детерминированный (индекс входного массива, без перестановки по hash).

## Что печатать в шаге 415

Одна строка на actor, поля из `ActorRecord` и `mailbox_count`:

`actor identity <logical_identity> incarnation <incarnation> code_epoch <code_epoch> principal <principal> mailbox <count> active <0|1> waiting <waiting_requests>`

`active` — `active_execution`. Checkpoint SVGUI-27 по-прежнему не читает этот текст обратно.

## 414

Карточка 414 не skipped. Red: `script_vm/tests/vm_dump_unit.mlc` фиксирует отсутствие `mailbox`, `active_execution`, `waiting_requests` в текущем `dump_instance`.
