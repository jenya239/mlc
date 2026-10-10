current_step: 16
status: ready
last_result: done step 16: baseline B1.ns_per_insn 176, B2.ns_per_insn 302, B3.ns_per_call 8023, B4.ns_per_alloc_n1000 22066, B4.ns_per_alloc_n4000 163446, B4.scaling_ratio_x100 671
last_verify: test -s script_vm/tests/bench_baseline.txt and wc -l => 6; ruby scripts/run_script_vm_bench.rb --max-ratio B1.ns_per_insn=baseline:300 => exit 0, B1.ns_per_insn 313; sabotage ruby scripts/run_script_vm_bench.rb --require-lower B1.ns_per_insn=current => exit 1, value=260 reference=1; restored bench_current.txt; gate green
next_step: 17
consult_count: 0
consult_response:
consult_verdict:
