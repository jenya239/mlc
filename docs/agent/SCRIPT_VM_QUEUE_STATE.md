current_step: 46
status: ready
last_result: done step 46: executor heap opcodes use in-place API; B4.ns_per_alloc_n4000 163446 -> 809; scaling_ratio_x100 47; sabotage record_round_trip exit 1 then restored
last_verify: ruby scripts/run_script_vm_gate.rb => script_vm_gate ok units=22; ruby scripts/run_script_vm_bench.rb --require-lower B4.ns_per_alloc_n4000=baseline --max-absolute B4.scaling_ratio_x100=250 --max-ratio B1.ns_per_insn=current:110 --max-ratio B2.ns_per_insn=current:110 --max-ratio B3.ns_per_call=current:110 => exit 0; B4.ns_per_alloc_n4000 baseline 163446 -> 809; B4.scaling_ratio_x100 47
next_step: 47
consult_count: 0
consult_response:
consult_verdict:
