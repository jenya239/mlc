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
]
unit_lines, unit_status = run_command(["ruby", "scripts/run_script_vm_gate.rb", *unit_names])
unless unit_status&.success? && last_line(unit_lines) == "script_vm_gate ok units=#{unit_names.length}"
  fail_gate("script_vm_queue200_gate FAIL units")
end

_cli_lines, cli_status = run_command(["ruby", "scripts/run_script_vm_repl_cli.rb"])
unless cli_status&.success?
  fail_gate("script_vm_queue200_gate FAIL repl_cli")
end

_host_lines, host_status = run_command(["ruby", "scripts/run_script_vm_repl_host.rb"])
unless host_status&.success?
  fail_gate("script_vm_queue200_gate FAIL repl_host")
end

puts "script_vm_queue200_gate ok"
exit 0
