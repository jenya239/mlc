#!/usr/bin/env ruby
require "rbconfig"

root_directory = File.expand_path("..", __dir__)

bench_scripts = [
  {
    label: "float",
    relative_path: "scripts/run_script_vm_float_bench.rb",
    metric_prefixes: [
      "float_ns_per_insn=",
      "B1.ns_per_insn=",
      "B3.ns_per_call="
    ]
  },
  {
    label: "match",
    relative_path: "scripts/run_script_vm_match_bench.rb",
    metric_prefixes: [
      "match_ns_per_insn=",
      "if_ns_per_insn="
    ]
  },
  {
    label: "lambda",
    relative_path: "scripts/run_script_vm_lambda_bench.rb",
    metric_prefixes: [
      "direct_ns_per_call=",
      "value_ns_per_call=",
      "indirect_overhead_ns=",
      "closure_make_3up_ns_per_call=",
      "lambda_layer_baseline_ns=",
      "baseline_b3_ns_per_call="
    ]
  },
  {
    label: "method",
    relative_path: "scripts/run_script_vm_method_bench.rb",
    metric_prefixes: [
      "direct_ns_per_call=",
      "method_ns_per_call=",
      "method_minus_direct_ns=",
      "baseline_direct_ns_per_call="
    ]
  }
]

def run_script(script_path)
  output_text = +""
  IO.popen([RbConfig.ruby, script_path, err: [:child, :out]]) do |output_stream|
    output_stream.each_line do |line|
      output_text << line
      print line
      $stdout.flush
    end
  end
  [output_text, $?.exitstatus]
end

def last_metric_line(output_text, metric_prefix)
  selected = nil
  output_text.each_line do |line|
    stripped = line.strip
    selected = stripped if stripped.start_with?(metric_prefix)
  end
  selected
end

$stdout.sync = true
summary_lines = []
failed = false

bench_scripts.each do |bench_script|
  script_path = File.join(root_directory, bench_script[:relative_path])
  puts "script_vm_s300_bench_summary running #{bench_script[:relative_path]}"
  output_text, exit_status = run_script(script_path)
  failed = true if exit_status != 0
  bench_script[:metric_prefixes].each do |metric_prefix|
    metric_line = last_metric_line(output_text, metric_prefix)
    if metric_line.nil?
      failed = true
      warn "missing #{bench_script[:label]} #{metric_prefix}"
    else
      summary_lines.push("#{bench_script[:label]} #{metric_line}")
    end
  end
end

puts "script_vm_s300_bench_summary begin"
puts "historical_b1_ns_per_insn=35/176"
puts "historical_b3_ns_per_call=1242/8023"
puts "historical_repl_language_us=1152"
summary_lines.each { |summary_line| puts summary_line }
baseline_lines = summary_lines.select { |summary_line| summary_line.include?("ratio=baseline:50") }
failed = true if baseline_lines.length < 2
if failed
  warn "script_vm_s300_bench_summary failed"
  exit 1
end
puts "script_vm_s300_bench_summary ok"
