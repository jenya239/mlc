# Шаг 443 — файловые бюджеты 301–440

`lower_generic_method.mlc` нет: фаза 361–374 blocked (шаг 302 исход b). Sabotage шёл на `script_vm/heap_collect.mlc` (шаг 438).

## Gate на фактическом дереве

```
ruby scripts/run_script_vm_file_size.rb
script_vm file size: ok
exit 0

ruby scripts/run_textui_file_size.rb
textui file size: ok (kit max 400, misc/textui/main.mlc freeze 637)
exit 0
```

`misc/textui/main.mlc` = 637 (freeze, не рос). Остальные `misc/textui/**/*.mlc` ≤ 400. Все `script_vm/**/*.mlc` ≤ 400.

На границе 400: `script_vm/lower/lower_array.mlc`, `script_vm/lower/lower_lambda.mlc`, `script_vm/lower/lower_match.mlc`, `misc/textui/app.mlc`.

## Sabotage

В `script_vm/heap_collect.mlc` добавлено пустых строк 281 → 401.

```
ruby scripts/run_script_vm_file_size.rb
script_vm/heap_collect.mlc 401 > 400
exit 1
```

`git checkout -- script_vm/heap_collect.mlc` → 281 строк. Повтор обоих size gate: exit 0. Diff файла пустой.
