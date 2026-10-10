current_step: 33
status: ready
last_result: done step 33: Int32 add/sub/mul/div and compares skip ValueRep before the general path
last_verify: ruby scripts/run_script_vm_gate.rb interpreter_arithmetic control_flow char_arith value_rep => script_vm_gate ok units=4 exit 0; ruby scripts/run_script_vm_bench.rb --require-lower B1.ns_per_insn=current --require-lower B2.ns_per_insn=current => B1 33 B2 32 exit 0; ruby scripts/run_script_vm_bench.rb --record current => B1.ns_per_insn 34 to 33, B2.ns_per_insn 35 to 32; sabotage fast ADD left-right => FAIL int_add got=-2 exit 1; restored; gate ok units=4; require-lower B1 31 B2 31 exit 0
next_step: 34
consult_count: 0
consult_response:
consult_verdict:
