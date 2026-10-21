# Шаг 441 — sabotage-прогон

Каждое повреждение применялось к HEAD `0eea861e`, unit краснел, затем `git checkout --` того файла. После всех откатов рабочие деревья sabotage-файлов чистые.

Фаза generics (361–374) не повреждалась: шаг 302 исход (b), `lower_generic_method_unit` не существует и в `run_script_vm_gate.rb` не зарегистрирован. Карточный вызов с этим именем дал бы exit 2.

| Фаза | Приём из карточки | Файл | Unit | Red |
|---|---|---|---|---|
| module hardening | `module_blob_exceeds_limit` всегда false | `script_vm/module_limits.mlc` | `module_budget_unit` | EXIT:1 `blob_over_maximum_blob_bytes code=0` |
| persistent instance | `instance_call_export` вызывает `heap_new()` вместо `instance.heap` | `script_vm/instance.mlc` | `persistent_instance_unit` | EXIT:1 `fresh heap dropped rooted array code=3 value=-1` |
| slicing | `materialize_current_frame` не пишет последний регистр | `script_vm/executor_slice.mlc` | `execution_frame_yield_unit` | EXIT:1 `resume after CALL yield value=-1 code=type at=3 opcode=3 slices=5 quantum=1 expected=41` |
| generics | не применялся | — | `lower_generic_method_unit` | skipped, Phase 4 blocked |
| GUI thin slice | `service_thread_issues_submit() = true` | `script_host/service.mlc` | `counter_actor_demo_unit` | EXIT:1 `submit left ui thread` |
| actors | `scheduler_try_dispatch` всегда `allow_normal = true` | `script_host/scheduler.mlc` | `scheduler_unit` | EXIT:1 `second turn started before first completed waiting_timer=true counter=0 messages=2` |
| sandbox | `sandbox_trusted_in_process_fallback` возвращает 1 | `runtime/include/mlc/script_host/launcher.hpp` | `sandbox_policy_unit` | EXIT:1 `trusted-in-process fallback` |
| durable state | `heap_incremental_write_barrier` пустое тело | `script_vm/heap_collect.mlc` | `gc_slice_unit` | EXIT:1 `incremental collector lost live object after mutation between mark slices` |

После отката:

```
ruby scripts/run_script_vm_gate.rb module_budget_unit module_verify_unit persistent_instance_unit execution_frame_yield_unit counter_actor_demo_unit scheduler_unit sandbox_policy_unit gc_slice_unit
```

exit 0, без `lower_generic_method_unit`.

`ruby scripts/run_script_vm_queue200_gate.rb`: `gc_slice` и остальные queue units зелёные до `lower_stage1 FAIL: syntax_error code`; последняя строка `script_vm_queue200_gate FAIL queue` (прежний блокер, не sabotage).
