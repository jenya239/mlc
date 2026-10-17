#!/usr/bin/env ruby
require "pty"

$stdout.sync = true
$stderr.sync = true

root_directory = File.expand_path("..", __dir__)
Dir.chdir(root_directory)

def run_command(command_arguments)
  lines = []
  status = nil
  PTY.spawn(*command_arguments) do |output, _input, process_identifier|
    begin
      output.each_line do |line|
        print line
        lines << line
      end
    rescue Errno::EIO
      nil
    end
    _, status = Process.wait2(process_identifier)
  end
  [lines, status]
end

def last_line(lines)
  lines.map(&:strip).reject(&:empty?).last
end

def fail_gate(message)
  puts message
  exit 1
end

def script_vm_gate_registers_float_unit(unit_name)
  unit_name == "lower_float_literal" || unit_name == "lower_float_arith" || unit_name == "lower_float_array"
end

float_unit_names = [
  "lower_float_literal",
  "lower_float_arith",
  "lower_float_mixed_reject",
  "lower_float_compare",
  "lower_float_record_field",
  "lower_float_array",
  "lower_float_function",
  "lower_float_critic",
  "lower_float_if_condition"
]
unless float_unit_names.length == 9
  fail_gate("script_vm_queue200_gate FAIL float unit count")
end

match_unit_names = [
  "lower_match_int",
  "lower_match_no_wildcard_reject",
  "lower_match_bool",
  "lower_match_binding",
  "lower_match_statement",
  "lower_match_in_loop",
  "lower_match_critic",
  "lower_match_record_reject"
]
unless match_unit_names.length == 8
  fail_gate("script_vm_queue200_gate FAIL match unit count")
end

lambda_unit_names = [
  "lower_lambda_basic",
  "lower_lambda_capture",
  "lower_lambda_multi_capture",
  "lower_lambda_as_argument",
  "lower_function_as_value",
  "lower_lambda_nested",
  "lower_lambda_verifier_reject",
  "lower_lambda_capture_record",
  "lower_lambda_near_loop",
  "lower_lambda_break_boundary_reject",
  "lower_lambda_critic"
]
unless lambda_unit_names.length == 11
  fail_gate("script_vm_queue200_gate FAIL lambda unit count")
end

method_unit_names = [
  "lower_method_basic",
  "lower_method_with_args",
  "lower_method_mut_self",
  "lower_method_name_disambiguation",
  "lower_method_unknown_reject",
  "lower_field_name_diagnostic"
]
unless method_unit_names.length == 6
  fail_gate("script_vm_queue200_gate FAIL method unit count")
end

queue_lines, queue_status = run_command(["ruby", "scripts/run_script_vm_queue_gate.rb"])
unless queue_status&.success? && last_line(queue_lines) == "script_vm_queue_gate ok stages=7"
  fail_gate("script_vm_queue200_gate FAIL queue")
end

unit_names = [
  "repl_balance",
  "repl_session",
  "string_heap",
  "string_opcode",
  "lower_text",
  "lower_array",
  "lower_iteration",
  "lower_for",
  "repl_language"
] + float_unit_names.select { |unit_name| script_vm_gate_registers_float_unit(unit_name) }
unit_lines, unit_status = run_command(["ruby", "scripts/run_script_vm_gate.rb", *unit_names])
unless unit_status&.success? && last_line(unit_lines) == "script_vm_gate ok units=#{unit_names.length}"
  fail_gate("script_vm_queue200_gate FAIL units")
end

float_unit_names.each do |unit_name|
  next if script_vm_gate_registers_float_unit(unit_name)
  direct_lines, direct_status = run_command(["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/#{unit_name}_unit.mlc"])
  unless direct_status&.success? && direct_lines.any? { |line| line.include?("[script_vm] #{unit_name}_unit ok") }
    fail_gate("script_vm_queue200_gate FAIL #{unit_name}")
  end
end

match_unit_names.each do |unit_name|
  direct_lines, direct_status = run_command(["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/#{unit_name}_unit.mlc"])
  unless direct_status&.success? && direct_lines.any? { |line| line.include?("[script_vm] #{unit_name}_unit ok") }
    fail_gate("script_vm_queue200_gate FAIL #{unit_name}")
  end
end

lambda_unit_names.each do |unit_name|
  direct_lines, direct_status = run_command(["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/#{unit_name}_unit.mlc"])
  unless direct_status&.success? && direct_lines.any? { |line| line.include?("[script_vm] #{unit_name}_unit ok") }
    fail_gate("script_vm_queue200_gate FAIL #{unit_name}")
  end
end

method_unit_names.each do |unit_name|
  direct_lines, direct_status = run_command(["ruby", "scripts/run_script_vm_unit.rb", "script_vm/tests/#{unit_name}_unit.mlc"])
  unless direct_status&.success? && direct_lines.any? { |line| line.include?("[script_vm] #{unit_name}_unit ok") }
    fail_gate("script_vm_queue200_gate FAIL #{unit_name}")
  end
end

_cli_lines, cli_status = run_command(["ruby", "scripts/run_script_vm_repl_cli.rb"])
unless cli_status&.success?
  fail_gate("script_vm_queue200_gate FAIL repl_cli")
end

_host_lines, host_status = run_command(["ruby", "scripts/run_script_vm_repl_host.rb"])
unless host_status&.success?
  fail_gate("script_vm_queue200_gate FAIL repl_host")
end

s200_unit_count = 51
slice_unit_count = float_unit_names.length + match_unit_names.length + lambda_unit_names.length + method_unit_names.length
puts "script_vm_queue200_gate units=#{s200_unit_count + slice_unit_count} s200=#{s200_unit_count} slice=#{slice_unit_count}"
puts "script_vm_queue200_gate ok"
exit 0
