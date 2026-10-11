#!/usr/bin/env ruby
require "fileutils"
require "timeout"

root_directory = File.expand_path("..", __dir__)
required_metric_keys = [
  "B1.ns_per_insn",
  "B1.total_us",
  "B2.ns_per_insn",
  "B3.ns_per_call",
  "B4.ns_per_alloc_n1000",
  "B4.ns_per_alloc_n4000",
  "B4.scaling_ratio_x100",
  "B5.total_us",
  "B5.ratio_x100"
]
run_count = 5

def capture_process(arguments, timeout_seconds)
  output_reader, output_writer = IO.pipe
  error_reader, error_writer = IO.pipe
  child_process_identifier = Process.spawn(*arguments, out: output_writer, err: error_writer)
  output_writer.close
  error_writer.close
  output_text = +""
  error_text = +""
  output_reader_thread = Thread.new { output_text.replace(output_reader.read.to_s) }
  error_reader_thread = Thread.new { error_text.replace(error_reader.read.to_s) }
  process_status = nil
  begin
    Timeout.timeout(timeout_seconds) do
      _, process_status = Process.wait2(child_process_identifier)
    end
  rescue Timeout::Error
    Process.kill("KILL", child_process_identifier)
    begin
      Process.wait(child_process_identifier)
    rescue Errno::ECHILD
      nil
    end
    output_reader_thread.join
    error_reader_thread.join
    output_reader.close
    error_reader.close
    return ["", "", :timeout]
  end
  output_reader_thread.join
  error_reader_thread.join
  output_reader.close
  error_reader.close
  [output_text, error_text, process_status]
end

def process_failed?(process_status)
  return true if process_status == :timeout
  return true unless process_status.is_a?(Process::Status)
  return true unless process_status.exited?
  process_status.exitstatus != 0
end

def parse_bench_values(output_text, required_metric_keys)
  values = {}
  metric_order = []
  output_text.each_line do |line|
    match = line.match(/\A\[script_vm\] bench name=(B\d+) metric=([a-z0-9_]+) value=(-?\d+)\s*\z/)
    next if match.nil?
    metric_key = "#{match[1]}.#{match[2]}"
    if values.key?(metric_key)
      warn "duplicate metric #{metric_key}"
      return nil
    end
    values[metric_key] = Integer(match[3])
    metric_order << metric_key
  end
  required_metric_keys.each do |metric_key|
    unless values.key?(metric_key)
      warn "missing metric #{metric_key}"
      return nil
    end
  end
  [values, metric_order]
end

def read_reference(path)
  unless File.file?(path)
    warn "missing reference #{path}"
    return nil
  end
  values = {}
  File.readlines(path).each do |line|
    stripped = line.strip
    next if stripped.empty?
    match = stripped.match(/\A(\S+) (-?\d+)\z/)
    if match.nil?
      warn "bad reference line #{stripped}"
      return nil
    end
    values[match[1]] = Integer(match[2])
  end
  values
end

def reference_value(root_directory, source, metric_key)
  unless source == "baseline" || source == "current"
    warn "unknown reference #{source}"
    return nil
  end
  values = read_reference(File.join(root_directory, "script_vm", "tests", "bench_#{source}.txt"))
  return nil if values.nil?
  unless values.key?(metric_key)
    warn "reference #{source} has no #{metric_key}"
    return nil
  end
  values[metric_key]
end

record_kind = nil
require_lower_checks = []
maximum_ratio_checks = []
maximum_absolute_checks = []
argument_index = 0
while argument_index < ARGV.length
  argument = ARGV[argument_index]
  if argument == "--record"
    record_kind = ARGV[argument_index + 1]
    argument_index += 2
  elsif argument == "--require-lower"
    require_lower_checks.push(ARGV[argument_index + 1])
    argument_index += 2
  elsif argument == "--max-ratio"
    maximum_ratio_checks.push(ARGV[argument_index + 1])
    argument_index += 2
  elsif argument == "--max-absolute"
    maximum_absolute_checks.push(ARGV[argument_index + 1])
    argument_index += 2
  else
    warn "unknown argument #{argument}"
    exit 1
  end
  if argument != "--record" && argument != "--require-lower" && argument != "--max-ratio" && argument != "--max-absolute"
    next
  end
  if ARGV[argument_index - 1].nil?
    warn "missing value for #{argument}"
    exit 1
  end
end

if record_kind && record_kind != "baseline" && record_kind != "current"
  warn "record target must be baseline or current"
  exit 1
end

compiler_directory = File.join(root_directory, "compiler")
mlcc_path = File.join(compiler_directory, "out", "mlcc")
unless File.executable?(mlcc_path)
  warn "mlcc not found at #{mlcc_path}"
  exit 1
end

entry_path = File.join(root_directory, "script_vm", "tests", "bench_entry.mlc")
unless File.file?(entry_path)
  warn "No such file: #{entry_path}"
  exit 1
end

ENV["TMPDIR"] = File.join(root_directory, "tmp")
ENV["MLCC_OBJ_CLEAN"] = "1"
ENV["MLCC_PCH"] = "0"
FileUtils.mkdir_p(ENV["TMPDIR"])

output_directory = File.join(root_directory, "tmp", "script_vm_bench")
binary_path = File.join(output_directory, "bin")
FileUtils.rm_rf(output_directory)
FileUtils.mkdir_p(output_directory)

compile_output, compile_error, compile_status = capture_process(
  [mlcc_path, "-o", output_directory, entry_path],
  600
)
if process_failed?(compile_status)
  print compile_output
  warn compile_error unless compile_error.empty?
  warn "mlcc failed"
  exit 1
end

link_output, link_error, link_status = capture_process(
  [File.join(compiler_directory, "build_bin.sh"), output_directory, binary_path],
  600
)
if process_failed?(link_status)
  print link_output
  warn link_error unless link_error.empty?
  warn "build_bin failed"
  exit 1
end

minimum_values = {}
metric_order = nil
run_index = 0
while run_index < run_count
  binary_output, binary_error, binary_status = capture_process([binary_path], 30)
  if process_failed?(binary_status)
    print binary_output
    warn binary_error unless binary_error.empty?
    warn binary_status == :timeout ? "binary timeout" : "binary failed"
    exit 1
  end
  parsed = parse_bench_values(binary_output, required_metric_keys)
  if parsed.nil?
    warn binary_error unless binary_error.empty?
    exit 1
  end
  parsed_values, parsed_order = parsed
  if metric_order.nil?
    metric_order = parsed_order
  elsif parsed_order.sort != metric_order.sort
    warn "metric set changed"
    exit 1
  end
  parsed_order.each do |metric_key|
    value = parsed_values[metric_key]
    current_minimum = minimum_values[metric_key]
    minimum_values[metric_key] = value if current_minimum.nil? || value < current_minimum
  end
  run_index += 1
end

handwritten_microseconds = minimum_values["B1.total_us"]
lowered_microseconds = minimum_values["B5.total_us"]
if handwritten_microseconds.nil? || lowered_microseconds.nil? || handwritten_microseconds <= 0
  warn "cannot compute B5.ratio_x100"
  exit 1
end
minimum_values["B5.ratio_x100"] = lowered_microseconds * 100 / handwritten_microseconds

if record_kind
  record_path = File.join(root_directory, "script_vm", "tests", "bench_#{record_kind}.txt")
  File.write(record_path, metric_order.map { |metric_key| "#{metric_key} #{minimum_values[metric_key]}\n" }.join)
end

comparison_failed = false
require_lower_checks.each do |specification|
  metric_key, source = specification.to_s.split("=", 2)
  reference = reference_value(root_directory, source, metric_key)
  if reference.nil? || !(minimum_values[metric_key] && minimum_values[metric_key] < reference)
    warn "require-lower failed #{specification} value=#{minimum_values[metric_key]} reference=#{reference}"
    comparison_failed = true
  end
end

maximum_ratio_checks.each do |specification|
  metric_key, remainder = specification.to_s.split("=", 2)
  source, percent_text = remainder.to_s.split(":", 2)
  percent = Integer(percent_text) rescue nil
  reference = reference_value(root_directory, source, metric_key)
  value = minimum_values[metric_key]
  if percent.nil? || reference.nil? || value.nil? || value * 100 > reference * percent
    warn "max-ratio failed #{specification} value=#{value} reference=#{reference}"
    comparison_failed = true
  end
end

maximum_absolute_checks.each do |specification|
  metric_key, limit_text = specification.to_s.split("=", 2)
  limit = Integer(limit_text) rescue nil
  value = minimum_values[metric_key]
  if limit.nil? || value.nil? || value > limit
    warn "max-absolute failed #{specification} value=#{value}"
    comparison_failed = true
  end
end

metric_order.each do |metric_key|
  puts "#{metric_key} #{minimum_values[metric_key]}"
end

exit 1 if comparison_failed
