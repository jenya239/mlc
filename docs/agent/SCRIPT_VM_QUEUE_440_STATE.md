current_step: 386
status: ready
last_result: done step 386: SetLabelText даёт DamageKind Layout+Paint через script_damage_revision, listing_revision не трогается
last_verify: ruby scripts/run_script_vm_gate.rb script_frame_damage_unit script_transaction_unit => script_vm_gate ok units=2; ruby scripts/run_textui_file_size.rb => textui file size: ok; bash scripts/run_textui_hover_bench.sh => hover_damage_min_us=178 budget_us=1000, [textui hover] ok; ruby scripts/run_script_vm_queue200_gate.rb => unit script_frame_damage_unit ok, затем lower_stage1 FAIL syntax_error code, script_vm_queue200_gate FAIL queue
next_step: 387
consult_count: 0
consult_response:
consult_verdict:
