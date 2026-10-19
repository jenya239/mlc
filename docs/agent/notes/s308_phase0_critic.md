# Шаг 308 — критика фазы 0

Нарушений нет. Ни одна карточка не возвращается.

Исход 302, дословно: «Исход (b). Generics метода в `extend` не доходят до `script_vm/lower`. Фаза 361–374 blocked. Шаг 303 не выполняется: его карточка начинается только при исходе (a). Следующий шаг — 304. `compiler/` и `lib/mlc/` не правятся.»

Шаг 303 пропущен по этому исходу. Файла `docs/agent/notes/s303_generics_scope.md` нет. Его `allowed_files` — только этот отсутствующий note, не `compiler/`.

`allowed_files` карточек 301–307:

| Шаг | kind | allowed_files |
|---|---|---|
| 301 | measure | `docs/agent/notes/s301_baseline.md` |
| 302 | decision | `docs/agent/notes/s302_generics_decision.md` |
| 303 | analysis | `docs/agent/notes/s303_generics_scope.md` |
| 304 | analysis | `docs/agent/notes/s304_vm_dump_format.md` |
| 305 | analysis | `docs/agent/notes/s305_fibers_mapping.md` |
| 306 | analysis | `docs/agent/notes/s306_gc_memory_order.md` |
| 307 | red | `script_vm/tests/module_budget_unit.mlc` |

Ни один путь не лежит в `compiler/` или `lib/mlc/`. Текст `s302_generics_decision.md` называет `compiler/frontend/parser/decls.mlc` как место, которое очередь не правит. Это не строка `allowed_files`.

`grep -L` из карточки 308 перевёрнут: `-L` печатает файлы без совпадения, а пояснение требует обратного. Проверялись строки `allowed_files`, не тела notes.

`module_budget` нет в `scripts/run_script_vm_gate.rb` и нет в `scripts/run_script_vm_queue200_gate.rb`. Красный unit 307 в общий gate не входит.
