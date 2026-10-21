current_step: 391
status: ready
last_result: done step 391: chat_host дренирует через drain_control_and_script_commands внутри chat_frames; unit не импортирует chat_host (parse update_pointer_click); submit остаётся на UI-потоке
last_verify: ruby scripts/run_script_vm_gate.rb script_frame_pump_unit => script_vm_gate ok units=1; ruby scripts/run_textui_file_size.rb => textui file size: ok; ruby scripts/run_script_vm_queue200_gate.rb => unit script_frame_pump_unit ok, затем lower_stage1 FAIL syntax_error code, script_vm_queue200_gate FAIL queue
next_step: 392
consult_count: 0
consult_response:
consult_verdict:
