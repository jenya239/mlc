current_step: 411
status: ready
last_result: done step 411: red — out-of-order worker: fast публикует revision=1 «fast», медленный job с base_revision=0 затем затирает label на «slow» без проверки непрерывности
last_verify: ruby scripts/run_script_vm_unit.rb script_host/tests/subscription_unit.mlc => EXIT:1, [script_vm subscription_unit] FAIL: stale worker result applied over newer revision shown=slow slow_base_revision=0 current_revision=1 handle_slot=0
next_step: 412
consult_count: 0
consult_response:
consult_verdict:
