current_step: 398
status: ready
last_result: done step 398: executor_host_call_suspend копирует аргументы и ставит Execution Waiting + HostRequest (timer.after); import не вызывается на стеке интерпретатора
last_verify: ruby scripts/run_script_vm_gate.rb host_wait_unit host_call_opcode_unit => script_vm_gate ok units=2; ruby scripts/run_script_vm_queue200_gate.rb => unit host_wait_unit ok, затем lower_stage1 FAIL syntax_error code, script_vm_queue200_gate FAIL queue
next_step: 399
consult_count: 0
consult_response:
consult_verdict:
