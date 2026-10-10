current_step: 35
status: ready
last_result: done step 35: allocation moved to heap_alloc.mlc; char_frames keeps a failed check status
last_verify: ruby scripts/run_script_vm_gate.rb heap_gc_arena arrays_records char_heap_ops closures_fibers char_frames => script_vm_gate ok units=5 exit 0; ruby scripts/run_script_vm_file_size.rb => ok exit 0; sabotage proto_index in shape_or_meta => FAIL call_argument expected RunOk got=stack, script_vm_gate FAIL char_frames exit 1; restored; gate ok units=5; SIZE ok; wc -l script_vm/heap.mlc 327 script_vm/heap_alloc.mlc 96
next_step: 36
consult_count: 0
consult_response:
consult_verdict:
