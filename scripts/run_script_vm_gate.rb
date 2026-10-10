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
