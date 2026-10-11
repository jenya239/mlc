current_step: 47
status: ready
last_result: done step 47: recorded current; B1 176->30, B2 302->28, B3 8023->1072, B4.ns_per_alloc_n1000 22066->796, B4.ns_per_alloc_n4000 163446->806, scaling 671->94; sabotage reference 1 exit 1 then restored
last_verify: ruby scripts/run_script_vm_bench.rb --record current => B1 30, B2 28, B3 1072, B4 n1000 796, B4 n4000 806, scaling 94; ruby scripts/run_script_vm_bench.rb --require-lower B1.ns_per_insn=baseline --require-lower B2.ns_per_insn=baseline --require-lower B3.ns_per_call=baseline --require-lower B4.ns_per_alloc_n4000=baseline --max-absolute B4.scaling_ratio_x100=250 => exit 0
next_step: 48
consult_count: 0
consult_response:
consult_verdict:
