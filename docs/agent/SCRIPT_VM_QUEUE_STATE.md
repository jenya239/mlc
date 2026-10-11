current_step: 54
status: consult_ready
last_result: blocked step 55: BENCH B1 49 > 33 and B3 1861 > 1179; the same bench on the tree before this step also failed (B1 60, B3 2251)
last_verify: ruby scripts/run_script_vm_bench.rb --max-ratio B1.ns_per_insn=current:110 --max-ratio B3.ns_per_call=current:110 => exit 1; max-ratio failed B1.ns_per_insn=current:110 value=49 reference=30; max-ratio failed B3.ns_per_call=current:110 value=1861 reference=1072
next_step: 55
consult_count: 2
consult_response: /home/jenya/workspaces/current/mlc-support/responses/script_vm_queue_consult_20261011_130424.md
consult_verdict: 
