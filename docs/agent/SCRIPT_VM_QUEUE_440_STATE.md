current_step: 332
status: ready
last_result: done step 332: MlcVmConfigV2 отвергает struct_size меньше текущей структуры; std::bad_alloc на границе становится MLC_HEAP_LIMIT; старый MlcVmConfig не менялся
last_verify: ruby scripts/run_script_vm_unit.rb script_vm/tests/embedding_v2_host_unit.mlc => exit 0, allocation_status=ok; bash scripts/run_script_vm_embedding_abi_unit.sh => exit 0; ruby scripts/run_script_vm_gate.rb embedding_limits => script_vm_gate ok units=1; ruby scripts/run_script_vm_queue200_gate.rb => script_vm_queue200_gate FAIL queue (unit lower_stage1 FAIL, syntax_error code, вне шага, compiler/ не правился)
next_step: 333
consult_count: 0
consult_response:
consult_verdict:
