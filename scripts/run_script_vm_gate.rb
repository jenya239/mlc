#!/usr/bin/env ruby

root_directory = File.expand_path("..", __dir__)

registered_units = [
  ["value_rep", ["bash", "scripts/run_script_vm_value_rep_unit.sh"]],
  ["bytecode_format", ["bash", "scripts/run_script_vm_bytecode_format_unit.sh"]],
  ["verifier", ["bash", "scripts/run_script_vm_verifier_unit.sh"]],
  ["interpreter_arithmetic", ["bash", "scripts/run_script_vm_interpreter_arithmetic_unit.sh"]],
  ["control_flow", ["bash", "scripts/run_script_vm_control_flow_unit.sh"]],
  ["heap_gc_arena", ["bash", "scripts/run_script_vm_heap_gc_arena_unit.sh"]],
  ["arrays_records", ["bash", "scripts/run_script_vm_arrays_records_unit.sh"]],
  ["closures_fibers", ["bash", "scripts/run_script_vm_closures_fibers_unit.sh"]],
  ["embedding_abi", ["bash", "scripts/run_script_vm_embedding_abi_unit.sh"]],
  ["runner_selfcheck", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/runner_selfcheck_unit.mlc"]],
  ["frontend_probe", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/frontend_probe_unit.mlc"]],
  ["char_arith", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/char_arith_unit.mlc"]],
  ["char_heap_ops", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/char_heap_ops_unit.mlc"]],
  ["char_frames", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/char_frames_unit.mlc"]],
  ["char_blob", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/char_blob_unit.mlc"]],
  ["bench_fixtures", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/bench_fixtures_unit.mlc"]],
  ["inplace_probe", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/inplace_probe_unit.mlc"]],
  ["language_probe", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/language_probe_unit.mlc"]],
  ["vm_state", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/vm_state_unit.mlc"]],
  ["heap_inplace", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/heap_inplace_unit.mlc"]],
  ["heap_reuse", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/heap_reuse_unit.mlc"]],
  ["heap_collect", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/heap_collect_unit.mlc"]],
  ["falsy_contract", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/falsy_contract_unit.mlc"]],
  ["int32_div_overflow", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/int32_div_overflow_unit.mlc"]],
  ["instruction_limit", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/instruction_limit_unit.mlc"]],
  ["gc_roots", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/gc_roots_unit.mlc"]],
  ["heap_limit", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/heap_limit_unit.mlc"]],
  ["verifier_hardening", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/verifier_hardening_unit.mlc"]],
  ["module_verify_unit", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/module_verify_unit.mlc"]],
  ["blob_limits", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/blob_limits_unit.mlc"]],
  ["module_budget_unit", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/module_budget_unit.mlc"]],
  ["module_constant_policy_unit", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/module_constant_policy_unit.mlc"]],
  ["loaded_module_cache_unit", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/loaded_module_cache_unit.mlc"]],
  ["persistent_instance_unit", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/persistent_instance_unit.mlc"]],
  ["handle_generation_unit", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/handle_generation_unit.mlc"]],
  ["vm_dump_unit", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/vm_dump_unit.mlc"]],
  ["execution_split_unit", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/execution_split_unit.mlc"]],
  ["execution_slice_unit", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/execution_slice_unit.mlc"]],
  ["execution_frame_yield_unit", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/execution_frame_yield_unit.mlc"]],
  ["execution_slice_differential_unit", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/execution_slice_differential_unit.mlc"]],
  ["repl_persistent_memory_unit", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/repl_persistent_memory_unit.mlc"]],
  ["sweep_gap", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/sweep_gap_unit.mlc"]],
  ["lower_builder", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/lower_builder_unit.mlc"]],
  ["module_encode", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/module_encode_unit.mlc"]],
  ["lower_stage1", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/lower_stage1_unit.mlc"]],
  ["lower_arithmetic", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/lower_arithmetic_unit.mlc"]],
  ["lower_text", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/lower_text_unit.mlc"]],
  ["lower_array", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/lower_array_unit.mlc"]],
  ["lower_control", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/lower_control_unit.mlc"]],
  ["lower_logic", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/lower_logic_unit.mlc"]],
  ["lower_loops", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/lower_loops_unit.mlc"]],
  ["lower_iteration", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/lower_iteration_unit.mlc"]],
  ["lower_for", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/lower_for_unit.mlc"]],
  ["lower_returns", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/lower_returns_unit.mlc"]],
  ["lower_calls", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/lower_calls_unit.mlc"]],
  ["lower_records", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/lower_records_unit.mlc"]],
  ["lower_diagnostics", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/lower_diagnostics_unit.mlc"]],
  ["embedding_limits", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/embedding_limits_unit.mlc"]],
  ["repl_balance", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/repl_balance_unit.mlc"]],
  ["repl_session", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/repl_session_unit.mlc"]],
  ["repl_language", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/repl_language_unit.mlc"]],
  ["string_heap", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/string_heap_unit.mlc"]],
  ["string_opcode", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/string_opcode_unit.mlc"]],
  ["lower_float_literal", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/lower_float_literal_unit.mlc"]],
  ["lower_float_arith", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/lower_float_arith_unit.mlc"]],
  ["lower_float_array", ["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/lower_float_array_unit.mlc"]],
]

def command_for_unit(registered_units, unit_name)
  registered_units.each do |registered_name, command_arguments|
    return command_arguments if registered_name == unit_name
  end
  nil
end

if ARGV == ["--list"]
  registered_units.each do |unit_name, command_arguments|
    puts "#{unit_name} #{command_arguments.join(" ")}"
  end
  exit 0
end

selected_unit_names = ARGV.empty? ? registered_units.map { |unit_name, _command_arguments| unit_name } : ARGV

selected_unit_names.each do |unit_name|
  if command_for_unit(registered_units, unit_name).nil?
    warn "unknown unit #{unit_name}"
    exit 2
  end
end

selected_unit_names.each do |unit_name|
  command_arguments = command_for_unit(registered_units, unit_name)
  child_success = system(*command_arguments, chdir: root_directory)
  if child_success
    puts "unit #{unit_name} ok"
  else
    puts "unit #{unit_name} FAIL"
    puts "script_vm_gate FAIL #{unit_name}"
    exit 1
  end
end

puts "script_vm_gate ok units=#{selected_unit_names.length}"
exit 0
