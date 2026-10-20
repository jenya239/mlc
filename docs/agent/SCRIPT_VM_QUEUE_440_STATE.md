current_step: 328
status: ready
last_result: done step 328: ReplSession держит один ScriptInstance; let mut items = [1] остаётся в heap, следующий ввод пишет items[0] = 9; accepted_source не растёт
last_verify: ruby scripts/run_script_vm_gate.rb repl_persistent_memory_unit persistent_instance_unit => script_vm_gate ok units=2; ruby scripts/run_script_vm_queue200_gate.rb => script_vm_queue200_gate FAIL queue (unit lower_stage1 FAIL, syntax_error code, вне шага, compiler/ не правился)
next_step: 329
consult_count: 0
consult_response:
consult_verdict:
