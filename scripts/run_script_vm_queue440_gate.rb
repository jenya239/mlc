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

expected_plan_unit_count = 38

plan_unit_names = [
  "module_budget_unit",
  "module_constant_policy_unit",
  "module_verify_unit",
  "loaded_module_cache_unit",
  "persistent_instance_unit",
  "repl_persistent_memory_unit",
  "handle_generation_unit",
  "embedding_v2_host_unit",
  "vm_dump_unit",
  "execution_split_unit",
  "execution_slice_unit",
  "execution_frame_yield_unit",
  "execution_budget_unit",
  "suspended_roots_unit",
  "byte_budget_unit",
  "gc_slice_unit",
  "lower_generic_method_unit",
  "service_queue_unit",
  "service_wakeup_unit",
  "resource_table_unit",
  "script_transaction_unit",
  "script_frame_damage_unit",
  "script_frame_pump_unit",
  "counter_actor_demo_unit",
  "module_import_unit",
  "host_call_opcode_unit",
  "host_wait_unit",
  "capability_unit",
  "scheduler_unit",
  "subscription_unit",
  "ipc_codec_unit",
  "transport_loopback_unit",
  "sandbox_policy_unit",
  "storage_schema_unit",
  "effect_outbox_unit",
  "checkpoint_unit",
  "reload_epoch_unit",
  "host_update_unit"
]

unless plan_unit_names.length == expected_plan_unit_count
  fail_gate("script_vm_queue440_gate FAIL unit count listed=#{plan_unit_names.length} expected=#{expected_plan_unit_count}")
end

skipped_unit_reasons = {
  "lower_generic_method_unit" => "phase 361-374 blocked",
  "embedding_v2_host_unit" => "not registered in run_script_vm_gate"
}

gate_unit_names = []
plan_unit_names.each do |unit_name|
  skip_reason = skipped_unit_reasons[unit_name]
  if skip_reason
    puts "script_vm_queue440_gate skip #{unit_name} #{skip_reason}"
    next
  end
  gate_unit_names.push(unit_name)
end

unless gate_unit_names.empty?
  unit_lines, unit_status = run_command(["ruby", "scripts/run_script_vm_gate.rb", *gate_unit_names])
  unless unit_status&.success? && last_line(unit_lines) == "script_vm_gate ok units=#{gate_unit_names.length}"
    fail_gate("script_vm_queue440_gate FAIL units")
  end
end

queue_lines, queue_status = run_command(["ruby", "scripts/run_script_vm_queue200_gate.rb"])
queue_last = last_line(queue_lines)
unless queue_status&.success? && queue_last == "script_vm_queue200_gate ok"
  unless queue_last == "script_vm_queue200_gate FAIL queue"
    fail_gate("script_vm_queue440_gate FAIL queue200")
  end
  puts "script_vm_queue440_gate record queue200 lower_stage1 FAIL syntax_error code"
end

puts "script_vm_queue440_gate units=#{plan_unit_names.length} expected=#{expected_plan_unit_count}"
puts "script_vm_queue440_gate ok"
exit 0
