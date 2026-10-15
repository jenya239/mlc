#!/usr/bin/env ruby
require "fileutils"
require "timeout"

root_directory = File.expand_path("..", __dir__)
run_count = 5
baseline_nanoseconds_per_instruction = 35
maximum_ratio = 3
SAMPLE_PREFIX = "float_sample_ns_per_insn="

entry_source = <<'MLC'
import { Value, ValueFloat64, value_encode, value_decode } from '../script_vm/value'
import {
  instruction_words_load_const,
  instruction_words_return,
  instruction_words_binary,
  instruction_words_jump,
  instruction_words_jump_if_false,
  instruction_opcode_le
} from '../script_vm/bytecode'
import { RunResult, RunOk, RunErr, run_arithmetic } from '../script_vm/interpreter'
import { float_add_opcode, float_multiply_opcode } from '../script_vm/exec_float'

extern fn script_vm_perf_now_us() -> i32 =
  "mlc::profile::monotonic_micros_i32" from "mlc/core/profile.hpp" thread_safe

extern fn integer_to_float64(value: i32) -> f64 =
  "mlc::math::i32_to_f64" from "mlc/math/math.hpp" thread_safe

fn iteration_limit() -> i32 = 60000

fn instruction_count() -> i32 =
  5 + 6 * iteration_limit() + 3

fn expected_sum() -> f64 =
  integer_to_float64(iteration_limit() / 2 * (iteration_limit() + 1))

fn append_words(destination: [i32], source: [i32]) -> [i32] = do
  let mut index = 0
  while index < source.length() do
    destination.push(source[index])
    index = index + 1
  end
  destination
end

fn push_float_constant(constants: [Value], number: f64) -> [Value] = do
  let mut next = constants
  next.push(value_encode(ValueFloat64(number)))
  next
end

fn float_loop_words() -> [i32] = do
  let mut words: [i32] = []
  words = append_words(words, instruction_words_load_const(0, 0))
  words = append_words(words, instruction_words_load_const(1, 1))
  words = append_words(words, instruction_words_load_const(2, 2))
  words = append_words(words, instruction_words_load_const(4, 1))
  words = append_words(words, instruction_words_load_const(5, 1))
  words = append_words(words, instruction_words_binary(instruction_opcode_le(), 3, 1, 2))
  words = append_words(words, instruction_words_jump_if_false(3, 4))
  words = append_words(words, instruction_words_binary(float_add_opcode(), 0, 0, 1))
  words = append_words(words, instruction_words_binary(float_multiply_opcode(), 6, 0, 5))
  words = append_words(words, instruction_words_binary(float_add_opcode(), 1, 1, 4))
  words = append_words(words, instruction_words_jump(0 - 6))
  words = append_words(words, instruction_words_return(0))
  words
end

fn float_loop_constants() -> [Value] = do
  let mut constants: [Value] = []
  constants = push_float_constant(constants, integer_to_float64(0))
  constants = push_float_constant(constants, integer_to_float64(1))
  constants = push_float_constant(constants, integer_to_float64(iteration_limit()))
  constants
end

fn sum_matches(result: RunResult, expected: f64) -> bool =
  match result {
    RunOk(value) => match value_decode(value) {
      ValueFloat64(number) => number == expected,
      _ => false
    },
    RunErr(_code, _word_index) => false
  }

fn nanoseconds_per_instruction(elapsed_microseconds: i32, accepted: bool) -> i32 =
  if !accepted || elapsed_microseconds <= 0 || elapsed_microseconds > 2000000 then
    0 - 1
  else
    elapsed_microseconds * 1000 / instruction_count()
  end

fn measure_float_arithmetic() -> i32 = do
  const started = script_vm_perf_now_us()
  const result = run_arithmetic(float_loop_words(), float_loop_constants(), 7)
  const elapsed = script_vm_perf_now_us() - started
  nanoseconds_per_instruction(elapsed, sum_matches(result, expected_sum()))
end

fn main() -> i32 = do
  const nanoseconds = measure_float_arithmetic()
  println("float_sample_ns_per_insn=" + nanoseconds.to_string())
  if nanoseconds < 0 then
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

def sample_nanoseconds(output_text)
  values = []
  output_text.each_line do |line|
    next unless line.start_with?(SAMPLE_PREFIX)
    values.push(Integer(line.delete_prefix(SAMPLE_PREFIX).strip))
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

entry_path = File.join(ENV["TMPDIR"], "script_vm_float_bench_entry.mlc")
output_directory = File.join(ENV["TMPDIR"], "script_vm_float_bench")
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

minimum_nanoseconds = nil
run_index = 0
while run_index < run_count
  binary_output, binary_error, binary_status = capture_process([binary_path], 20)
  if process_failed?(binary_status)
    print binary_output
    warn binary_error unless binary_error.empty?
    warn binary_status == :timeout ? "binary timeout" : "binary failed"
    exit 1
  end
  sample = sample_nanoseconds(binary_output)
  if sample.nil? || sample < 0
    print binary_output
    warn binary_error unless binary_error.empty?
    warn "float sample missing"
    exit 1
  end
  minimum_nanoseconds = sample if minimum_nanoseconds.nil? || sample < minimum_nanoseconds
  run_index += 1
end

puts "float_ns_per_insn=#{minimum_nanoseconds}"
limit = baseline_nanoseconds_per_instruction * maximum_ratio
if minimum_nanoseconds > limit
  warn "float_ns_per_insn #{minimum_nanoseconds} exceeds B1 #{baseline_nanoseconds_per_instruction} by more than #{maximum_ratio}x (limit #{limit})"
  exit 1
end

baseline_ratio_percent = 50

def read_baseline_metric(root_directory, metric_name)
  path = File.join(root_directory, "script_vm", "tests", "bench_baseline.txt")
  unless File.file?(path)
    warn "missing baseline #{path}"
    return nil
  end
  found = nil
  prefix = "#{metric_name} "
  File.readlines(path).each do |line|
    stripped = line.strip
    next unless stripped.start_with?(prefix)
    return nil unless found.nil?
    found = Integer(stripped.delete_prefix(prefix))
  end
  found
end

def metric_from_bench(output_text, metric_name)
  values = []
  prefix = "#{metric_name} "
  output_text.each_line do |line|
    stripped = line.strip
    next unless stripped.start_with?(prefix)
    values.push(Integer(stripped.delete_prefix(prefix)))
  end
  return nil unless values.length == 1
  values[0]
end

def ratio_limit(baseline, percent)
  baseline * percent / 100
end

def ratio_exceeded?(value, baseline, percent)
  value * 100 > baseline * percent
end

instruction_baseline = read_baseline_metric(root_directory, "B1.ns_per_insn")
call_baseline = read_baseline_metric(root_directory, "B3.ns_per_call")
if instruction_baseline.nil? || call_baseline.nil?
  warn "baseline metrics missing"
  exit 1
end

bench_script = File.join(root_directory, "scripts", "run_script_vm_bench.rb")
bench_output, bench_error, bench_status = capture_process(
  [
    "ruby", bench_script,
    "--max-ratio", "B1.ns_per_insn=baseline:#{baseline_ratio_percent}",
    "--max-ratio", "B3.ns_per_call=baseline:#{baseline_ratio_percent}"
  ],
  2400
)
print bench_output
warn bench_error unless bench_error.empty?

instruction_nanoseconds = metric_from_bench(bench_output, "B1.ns_per_insn")
call_nanoseconds = metric_from_bench(bench_output, "B3.ns_per_call")
instruction_limit = ratio_limit(instruction_baseline, baseline_ratio_percent)
call_limit = ratio_limit(call_baseline, baseline_ratio_percent)
puts "B1.ns_per_insn=#{instruction_nanoseconds} baseline=#{instruction_baseline} ratio=baseline:#{baseline_ratio_percent} limit=#{instruction_limit}"
puts "B3.ns_per_call=#{call_nanoseconds} baseline=#{call_baseline} ratio=baseline:#{baseline_ratio_percent} limit=#{call_limit}"
if instruction_nanoseconds.nil? || call_nanoseconds.nil? || process_failed?(bench_status) || ratio_exceeded?(instruction_nanoseconds, instruction_baseline, baseline_ratio_percent) || ratio_exceeded?(call_nanoseconds, call_baseline, baseline_ratio_percent)
  warn bench_status == :timeout ? "queue bench timeout" : "queue bench failed baseline:#{baseline_ratio_percent}"
  exit 1
end
