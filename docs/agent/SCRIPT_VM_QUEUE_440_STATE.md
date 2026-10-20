current_step: 338
status: ready
last_result: done step 338: identity i64 и incarnation i32 в дампе те же поля, что у ValueHandle; расхождения типа нет
last_verify: ruby scripts/run_script_vm_gate.rb persistent_instance_unit loaded_module_cache_unit repl_persistent_memory_unit handle_generation_unit vm_dump_unit => script_vm_gate ok units=5; bash scripts/run_script_vm_embedding_abi_unit.sh => exit 0; ruby scripts/run_script_vm_queue200_gate.rb => script_vm_queue200_gate FAIL queue (unit lower_stage1 FAIL, syntax_error code, вне шага, compiler/ не правился)
next_step: 339
consult_count: 0
consult_response:
consult_verdict:
