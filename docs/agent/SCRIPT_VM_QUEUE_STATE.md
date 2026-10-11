current_step: 90
status: ready
last_result: done step 90: emb_call_export_*_limited и коды 9–12; безлимитные делегируют в _limited(0, 0)
last_verify: ruby scripts/run_script_vm_unit.rb script_vm/tests/embedding_limits_unit.mlc => exit 0, embedding_limits_unit ok; ruby scripts/run_script_vm_gate.rb embedding_abi char_blob embedding_limits instruction_limit heap_limit => script_vm_gate ok units=5; ruby scripts/run_script_vm_file_size.rb => script_vm file size: ok; sabotage div_zero→emb_type => FAIL divide_zero code=3 exit 1; restored; повтор gate зелёный
next_step: 91
consult_count: 0
consult_response:
consult_verdict:
