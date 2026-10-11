current_step: 50
status: consult_ready
last_result: blocked step 51: INT_MIN/-1 returns overflow; RUN and GATE(ARITH int32_div_overflow) passed; BENCH max-ratio failed B1.ns_per_insn value=35 reference=30 limit 110%
last_verify: ruby scripts/run_script_vm_bench.rb --max-ratio B1.ns_per_insn=current:110 => exit 1; max-ratio failed B1.ns_per_insn=current:110 value=35 reference=30; B1.ns_per_insn 35
next_step: 51
consult_count: 2
consult_response: /home/jenya/workspaces/current/mlc-support/responses/script_vm_queue_consult_20261011_104000.md
consult_verdict:
