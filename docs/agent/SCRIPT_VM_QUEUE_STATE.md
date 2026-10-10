current_step: 32
status: ready
last_result: done step 32: run_* call the unified executor; hot opcodes keep frame registers local in executor.mlc
last_verify: ruby scripts/run_script_vm_gate.rb => script_vm_gate ok units=19 exit 0; ruby scripts/run_script_vm_bench.rb --require-lower B1.ns_per_insn=baseline --require-lower B2.ns_per_insn=baseline --require-lower B3.ns_per_call=baseline --max-ratio B4.ns_per_alloc_n1000=baseline:130 => exit 0; ruby scripts/run_script_vm_bench.rb --record current => exit 0; wc -l script_vm/interpreter.mlc => 46; baseline to current B1.ns_per_insn 176 to 34, B2.ns_per_insn 302 to 35, B3.ns_per_call 8023 to 1684, B4.ns_per_alloc_n1000 22066 to 16420, B4.ns_per_alloc_n4000 163446 to 125277, B4.scaling_ratio_x100 671 to 676; sabotage empty args => script_vm_gate FAIL char_frames host_arguments expected Int32 exit 1; restored; gate ok units=19
next_step: 33
consult_count: 0
consult_response:
consult_verdict:
