current_step: 430
status: ready
last_result: done step 430: green — effect_outbox.mlc: intent до dispatch, reconcile sent без status-check → Unknown, не auto-retry; idempotency_key
last_verify: bash scripts/run_sqlite_gate.sh effect_outbox_unit => OK; bash scripts/run_sqlite_gate.sh all => OK; ruby scripts/run_script_vm_queue200_gate.rb => effect_outbox_unit ok, last line script_vm_queue200_gate FAIL queue (known lower_stage1 syntax_error code)
next_step: 431
consult_count: 0
consult_response:
consult_verdict:
