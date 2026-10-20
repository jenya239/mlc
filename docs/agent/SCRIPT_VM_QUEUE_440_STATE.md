current_step: 330
status: ready
last_result: done step 330: value handle проверяет owner и generation; чужой instance, stale generation и повторный release дают emb_bad_handle
last_verify: ruby scripts/run_script_vm_gate.rb handle_generation_unit persistent_instance_unit => script_vm_gate ok units=2; bash scripts/run_script_vm_embedding_abi_unit.sh => exit 0; ruby scripts/run_script_vm_queue200_gate.rb => script_vm_queue200_gate FAIL queue (unit lower_stage1 FAIL, syntax_error code, вне шага, compiler/ не правился)
next_step: 331
consult_count: 0
consult_response:
consult_verdict:
