#!/usr/bin/env ruby
require "pty"

$stdout.sync = true
$stderr.sync = true

root_directory = File.expand_path("..", __dir__)
Dir.chdir(root_directory)

with_differential = false
ARGV.each do |argument|
  if argument == "--with-differential"
    with_differential = true
  else
    warn "unknown argument #{argument}"
    exit 1
  end
end

stages = [
  ["size", ["ruby", "scripts/run_script_vm_file_size.rb"]],
  ["units", ["ruby", "scripts/run_script_vm_gate.rb"]],
  ["embedding_abi_unit", ["bash", "scripts/run_script_vm_embedding_abi_unit.sh"]],
  ["lowered_host", ["ruby", "scripts/run_script_vm_lowered_host.rb"]],
  ["handle_host", ["ruby", "scripts/run_script_vm_handle_host.rb"]],
  ["lower_tool", ["ruby", "scripts/run_script_vm_lower_tool.rb"]]
]
if with_differential
  stages.push(["differential", ["ruby", "scripts/run_script_vm_differential.rb"]])
end
stages.push([
  "bench",
  [
    "ruby", "scripts/run_script_vm_bench.rb",
    "--max-ratio", "B1.ns_per_insn=current:110",
    "--max-ratio", "B3.ns_per_call=current:110"
  ]
])

def run_stage(stage_name, command_arguments)
  puts "stage #{stage_name}"
  status = nil
  PTY.spawn(*command_arguments) do |output, _input, process_identifier|
    begin
      output.each_line { |line| print line }
    rescue Errno::EIO
      nil
    end
    _, status = Process.wait2(process_identifier)
  end
  return if status&.success?
  puts "script_vm_queue_gate FAIL stage=#{stage_name}"
  exit 1
end

begin
  stages.each do |stage_name, command_arguments|
    run_stage(stage_name, command_arguments)
  end
rescue StandardError => error
  warn "script_vm_queue_gate FAIL stage=internal #{error.class}: #{error.message}"
  exit 1
end

puts "script_vm_queue_gate ok stages=#{stages.length}"
