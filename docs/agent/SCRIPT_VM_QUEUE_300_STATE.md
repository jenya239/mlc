current_step: 227
status: consult_ready
last_result: blocked step 228: pre-float 7065c2e1 B1 86 B3 2797 passes baseline:50 (limits 88 and 4011); current tree B1 125 B3 4099 exceeds those limits
last_verify: ruby scripts/run_script_vm_bench.rb --max-ratio B1.ns_per_insn=baseline:50 --max-ratio B3.ns_per_call=baseline:50 at 7065c2e1 => B1.ns_per_insn 86 B3.ns_per_call 2797 GATE_EXIT:0; ruby scripts/run_script_vm_float_bench.rb => B1 125 B3 4099 GATE_EXIT:1
next_step: 228
consult_count: 2
consult_response: /home/jenya/workspaces/current/mlc-support/responses/script_vm_queue_consult_20261015_153647.md
consult_verdict:
