current_step: 406
status: ready
last_result: done step 406: green — script_transaction_commit_with_capability повторно вызывает admit перед publish; revoke до publish отклоняет queued batch; уже опубликованный эффект revoke не откатывает; ungated commit без изменений
last_verify: ruby scripts/run_script_vm_gate.rb capability_unit script_transaction_unit => script_vm_gate ok units=2; ruby scripts/run_script_vm_queue200_gate.rb => capability_unit ok, script_transaction_unit ok, lower_stage1 FAIL syntax_error code, script_vm_queue200_gate FAIL queue
next_step: 407
consult_count: 0
consult_response:
consult_verdict:
