#!/usr/bin/env ruby
require "fileutils"
require "timeout"

root_directory = File.expand_path("..", __dir__)
run_count = 5
baseline_direct_nanoseconds_per_call = 1242
direct_sample_prefix = "direct_sample_ns_per_call="
value_sample_prefix = "value_sample_ns_per_call="

entry_source = <<'MLC'
import { Value, ValueInt32, value_decode } from '../script_vm/value'
import { lower_source } from '../script_vm/lower/lower_module'
import { LowerDiagnostic } from '../script_vm/lower/lower_types'
import { emb_ok, emb_call_export_bytes } from '../script_vm/embedding'

extern fn script_vm_perf_now_us() -> i32 =
  "mlc::profile::monotonic_micros_i32" from "mlc/core/profile.hpp" thread_safe

fn call_count() -> i32 = 100000

fn expected_total() -> i32 = call_count() * 2

fn direct_source() -> string =
  "fn successor(input: i32) -> i32 = input + 1\nexport fn main() -> i32 = do\nlet mut index = 0\nlet mut total = 0\nwhile index < " + call_count().to_string() + " do\ntotal = total + successor(1)\nindex = index + 1\nend\ntotal\nend"

fn value_source() -> string =
  "fn successor(input: i32) -> i32 = input + 1\nexport fn main() -> i32 = do\nlet wrapped = successor\nlet mut index = 0\nlet mut total = 0\nwhile index < " + call_count().to_string() + " do\ntotal = total + wrapped(1)\nindex = index + 1\nend\ntotal\nend"

fn decoded_integer(raw: i64) -> i32 =
  match value_decode(Value { raw: raw }) {
    ValueInt32(number) => number,
    _ => 0 - 1
  }

fn diagnostic_codes(diagnostics: [LowerDiagnostic]) -> string = do
  let mut text = ""
  let mut index = 0
  while index < diagnostics.length() do
    if index > 0 then
      text = text + ","
    end
    text = text + diagnostics[index].code
    index = index + 1
  end
  text
end

fn empty_arguments() -> [i64] = do
  let mut arguments: [i64] = []
  arguments
end

fn nanoseconds_per_call(elapsed_microseconds: i32, accepted: bool) -> i32 =
  if !accepted || elapsed_microseconds <= 0 || elapsed_microseconds > 2000000 then
    0 - 1
  else
    elapsed_microseconds * 1000 / call_count()
  end

fn measure_source(source: string, label: string) -> i32 = do
  const lowered = lower_source(source)
  if lowered.diagnostics.length() != 0 then
    println(label + " diagnostics=" + diagnostic_codes(lowered.diagnostics))
    return 0 - 1
  end
  const started = script_vm_perf_now_us()
  const called = emb_call_export_bytes(lowered.bytes, "main", empty_arguments())
  const elapsed = script_vm_perf_now_us() - started
  const accepted = called.code == emb_ok() && decoded_integer(called.raw) == expected_total()
  nanoseconds_per_call(elapsed, accepted)
end

fn main() -> i32 = do
  const direct_nanoseconds = measure_source(direct_source(), "direct")
  const value_nanoseconds = measure_source(value_source(), "value")
  println("direct_sample_ns_per_call=" + direct_nanoseconds.to_string())
  println("value_sample_ns_per_call=" + value_nanoseconds.to_string())
  if direct_nanoseconds < 0 || value_nanoseconds < 0 then
    1
  else
    0
  end
end
MLC

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

def sample_nanoseconds(output_text, prefix)
  values = []
  output_text.each_line do |line|
    next unless line.start_with?(prefix)
    values.push(Integer(line.delete_prefix(prefix).strip))
  end
  return nil unless values.length == 1
  values[0]
end

compiler_directory = File.join(root_directory, "compiler")
mlcc_path = File.join(compiler_directory, "out", "mlcc")
unless File.executable?(mlcc_path)
  warn "mlcc not found at #{mlcc_path}"
  exit 1
end

ENV["TMPDIR"] = File.join(root_directory, "tmp")
ENV["MLCC_OBJ_CLEAN"] = "1"
ENV["MLCC_PCH"] = "0"
FileUtils.mkdir_p(ENV["TMPDIR"])

entry_path = File.join(ENV["TMPDIR"], "script_vm_lambda_bench_entry.mlc")
output_directory = File.join(ENV["TMPDIR"], "script_vm_lambda_bench")
binary_path = File.join(output_directory, "bin")
File.write(entry_path, entry_source)
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

minimum_direct_nanoseconds = nil
minimum_value_nanoseconds = nil
run_index = 0
while run_index < run_count
  binary_output, binary_error, binary_status = capture_process([binary_path], 20)
  if process_failed?(binary_status)
    print binary_output
    warn binary_error unless binary_error.empty?
    warn binary_status == :timeout ? "binary timeout" : "binary failed"
    exit 1
  end
  direct_sample = sample_nanoseconds(binary_output, direct_sample_prefix)
  value_sample = sample_nanoseconds(binary_output, value_sample_prefix)
  if direct_sample.nil? || value_sample.nil? || direct_sample < 0 || value_sample < 0
    print binary_output
    warn binary_error unless binary_error.empty?
    warn "lambda bench sample missing"
    exit 1
  end
  minimum_direct_nanoseconds = direct_sample if minimum_direct_nanoseconds.nil? || direct_sample < minimum_direct_nanoseconds
  minimum_value_nanoseconds = value_sample if minimum_value_nanoseconds.nil? || value_sample < minimum_value_nanoseconds
  run_index += 1
end

indirect_overhead_nanoseconds = minimum_value_nanoseconds - minimum_direct_nanoseconds
puts "direct_ns_per_call=#{minimum_direct_nanoseconds}"
puts "value_ns_per_call=#{minimum_value_nanoseconds}"
puts "indirect_overhead_ns=#{indirect_overhead_nanoseconds}"
puts "baseline_b3_ns_per_call=#{baseline_direct_nanoseconds_per_call}"
puts "indirect_overhead_is_expected=true"
