current_step: 94
status: ready
last_result: done step 94: mlc_vm_get_export проверяет emb_has_export_ptr; argc без args даёт MLC_BOUNDS
last_verify: ruby scripts/run_script_vm_handle_host.rb => exit 0, handle_host ok cases=10; ruby scripts/run_script_vm_lowered_host.rb => exit 0, lowered_host ok cases=6; bash scripts/run_script_vm_embedding_abi_unit.sh => exit 0; ruby scripts/run_script_vm_file_size.rb => script_vm file size: ok; sabotage без emb_has_export_ptr => get_export_missing_export got=1 exit 1; restored; повтор gate зелёный
next_step: 95
consult_count: 0
consult_response:
consult_verdict:
