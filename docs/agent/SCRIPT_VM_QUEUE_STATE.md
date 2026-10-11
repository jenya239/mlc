current_step: 55
status: ready
last_result: done step 55: allocation safepoint collects from stack and frame roots
last_verify: ruby scripts/run_script_vm_unit.rb script_vm/tests/gc_roots_unit.mlc => exit 0 checks=5; ruby scripts/run_script_vm_gate.rb => script_vm_gate ok units=26; ruby scripts/run_script_vm_bench.rb --max-ratio B1.ns_per_insn=baseline:50 --max-ratio B3.ns_per_call=baseline:50 => exit 0 B1.ns_per_insn 56 B3.ns_per_call 1964; sabotage empty live roots => RUN exit 1 FAIL live_root_keeps_field result=-1, restored, gates green
next_step: 56
consult_count: 0
consult_response: 
consult_verdict: 
