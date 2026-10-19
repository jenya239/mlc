# Шаг 302 — generics методов

Исход (b). Generics метода в `extend` не доходят до `script_vm/lower`. Фаза 361–374 blocked. Шаг 303 не выполняется: его карточка начинается только при исходе (a). Следующий шаг — 304. `compiler/` и `lib/mlc/` не правятся.

Символы:

- `compiler/frontend/ast.mlc`: `DeclFn` несёт `type_parameters: [string]`, bounds `[[string]]` и `[WhereClauseBound]`.
- `compiler/frontend/parser/decls.mlc` `parse_function_declaration` вызывает `parse_type_params_opt` и записывает список в `DeclFn`. Это путь top-level `fn`, не метода.
- Там же `parse_extend_method` — единственный разбор метода внутри `extend` (вызов из строки 319). Он создаёт `DeclFn(mangled_name, [], [], params, ..., [])`. Список параметров типа всегда пустой, bounds и where тоже. Конструкция `fn name<T>(...)` на методе этим разбором не сохраняется.
- `script_vm/lower/lower_method.mlc` `extend_methods` / `receiver_method` копируют `type_parameters`, `bounds` и `where_bounds` как есть.
- `script_vm/lower/lower_module.mlc` `consider_extend_methods` передаёт методы в `consider_method_declaration`. Тот матчит `DeclFn(name, type_parameters, _, ...)` и зовёт `consider_function`.
- `note_generic_function` при `type_parameters.length() > 0` добавляет диагностику `unsupported_generic`. Для метода из `extend` список пустой, поэтому эта диагностика не срабатывает: параметров типа в узле нет.
- `consider_method_declaration` отбрасывает bounds и where (`_`). Даже после появления списка имён bounds и where-clause остаются отказом.

Лоуэр умеет отказать на уже заполненном списке имён. Заполнить список у метода может только правка `parse_extend_method` в `compiler/frontend/parser/decls.mlc`. Это не шаг очереди.
