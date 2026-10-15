#!/usr/bin/env ruby
require "fileutils"
require "timeout"

root_directory = File.expand_path("..", __dir__)
run_count = 5
match_prefix = "match_ns_per_insn="
conditional_prefix = "if_ns_per_insn="

entry_source = <<'MLC'
import { Value, ValueInt32, value_encode, value_decode } from '../script_vm/value'
import { FunctionProto } from '../script_vm/run_result'
import { decode_module } from '../script_vm/module_blob'
import { lower_source } from '../script_vm/lower/lower_module'
import { emb_ok, emb_call_export_bytes } from '../script_vm/embedding'
import { apply_binary, apply_compare, value_jump_decision } from '../script_vm/arith'
import {
  Instruction,
  instruction_decode,
  instruction_word_span,
  instruction_jump_offset,
  instruction_load_const_index,
  instruction_opcode_load_const,
  instruction_opcode_move,
  instruction_opcode_add,
  instruction_opcode_sub,
  instruction_opcode_mul,
  instruction_opcode_div,
  instruction_opcode_return,
  instruction_opcode_jump,
  instruction_opcode_jump_if_false,
  instruction_opcode_eq,
  instruction_opcode_ne,
  instruction_opcode_lt,
  instruction_opcode_le,
  instruction_opcode_gt,
  instruction_opcode_ge
} from '../script_vm/bytecode'

extern fn script_vm_perf_now_us() -> i32 =
  "mlc::profile::monotonic_micros_i32" from "mlc/core/profile.hpp" thread_safe

type CounterMove = { registers: [i64], program_counter: i32, status: i32 }

fn iteration_limit() -> i32 = 50000

fn expected_total() -> i32 = iteration_limit() / 5 * 15

fn step_limit() -> i32 = 10000000

fn match_source() -> string =
  "export fn run() -> i32 = do\nlet mut total = 0\nlet mut index = 0\nlet mut remainder = 0\nwhile index < " + iteration_limit().to_string() + " do\nremainder = index - index / 5 * 5\ntotal = total + match remainder do\n0 -> 1\n1 -> 2\n2 -> 3\n3 -> 4\n_ -> 5\nend\nindex = index + 1\nend\ntotal\nend"

fn conditional_source() -> string =
  "export fn run() -> i32 = do\nlet mut total = 0\nlet mut index = 0\nlet mut remainder = 0\nwhile index < " + iteration_limit().to_string() + " do\nremainder = index - index / 5 * 5\ntotal = total + if remainder == 0 do\n1\nelse if remainder == 1 do\n2\nelse if remainder == 2 do\n3\nelse if remainder == 3 do\n4\nelse\n5\nend\nindex = index + 1\nend\ntotal\nend"

fn decoded_integer(raw: i64) -> i32 =
  match value_decode(Value { raw: raw }) {
    ValueInt32(number) => number,
    _ => 0 - 1
  }

fn opcode_is_binary(opcode: i32) -> bool =
  opcode == instruction_opcode_add() || opcode == instruction_opcode_sub() || opcode == instruction_opcode_mul() || opcode == instruction_opcode_div()

fn opcode_is_compare(opcode: i32) -> bool =
  opcode == instruction_opcode_eq() || opcode == instruction_opcode_ne() || opcode == instruction_opcode_lt() || opcode == instruction_opcode_le() || opcode == instruction_opcode_gt() || opcode == instruction_opcode_ge()

fn zero_raw_registers(register_count: i32) -> [i64] = do
  let mut registers: [i64] = []
  let mut index = 0
  while index < register_count do
    registers.push(0)
    index = index + 1
  end
  registers
end

fn write_raw(registers: [i64], slot_index: i32, raw: i64) -> [i64] = do
  let mut updated = registers
  updated.set(slot_index, raw)
  updated
end

fn failed_move(registers: [i64]) -> CounterMove =
  CounterMove { registers: registers, program_counter: 0, status: 2 }

fn returned_move(registers: [i64]) -> CounterMove =
  CounterMove { registers: registers, program_counter: 0, status: 1 }

fn continue_move(registers: [i64], program_counter: i32) -> CounterMove =
  CounterMove { registers: registers, program_counter: program_counter, status: 0 }

fn slot_in_range(registers: [i64], slot_index: i32) -> bool =
  slot_index >= 0 && slot_index < registers.length()

fn return_move(registers: [i64], slot_index: i32) -> CounterMove =
  if !slot_in_range(registers, slot_index) then
    failed_move(registers)
  else if registers[slot_index] != value_encode(ValueInt32(expected_total())).raw then
    failed_move(registers)
  else
    returned_move(registers)
  end

fn jump_if_false_move(registers: [i64], words: [i32], slot_index: i32, program_counter: i32, span: i32) -> CounterMove =
  if !slot_in_range(registers, slot_index) then
    failed_move(registers)
  else
    do
      let decision = value_jump_decision(Value { raw: registers[slot_index] })
      if decision == 1 then
        continue_move(registers, program_counter + span + instruction_jump_offset(words, program_counter))
      else if decision == 0 then
        continue_move(registers, program_counter + span)
      else
        failed_move(registers)
      end
    end
  end

fn load_constant_move(registers: [i64], words: [i32], constants: [Value], slot_index: i32, program_counter: i32, span: i32) -> CounterMove = do
  let constant_index = instruction_load_const_index(words, program_counter)
  if !slot_in_range(registers, slot_index) || constant_index < 0 || constant_index >= constants.length() then
    failed_move(registers)
  else
    continue_move(write_raw(registers, slot_index, constants[constant_index].raw), program_counter + span)
  end
end

fn move_register(registers: [i64], destination: i32, source_index: i32, program_counter: i32, span: i32) -> CounterMove =
  if !slot_in_range(registers, destination) || !slot_in_range(registers, source_index) then
    failed_move(registers)
  else
    continue_move(write_raw(registers, destination, registers[source_index]), program_counter + span)
  end

fn binary_move(registers: [i64], opcode: i32, destination: i32, left_index: i32, right_index: i32, program_counter: i32, span: i32) -> CounterMove =
  if !slot_in_range(registers, destination) || !slot_in_range(registers, left_index) || !slot_in_range(registers, right_index) then
    failed_move(registers)
  else
    do
      let outcome = apply_binary(opcode, Value { raw: registers[left_index] }, Value { raw: registers[right_index] })
      if outcome.err_code != "" then
        failed_move(registers)
      else
        continue_move(write_raw(registers, destination, outcome.value.raw), program_counter + span)
      end
    end
  end

fn compare_move(registers: [i64], opcode: i32, destination: i32, left_index: i32, right_index: i32, program_counter: i32, span: i32) -> CounterMove =
  if !slot_in_range(registers, destination) || !slot_in_range(registers, left_index) || !slot_in_range(registers, right_index) then
    failed_move(registers)
  else
    do
      let outcome = apply_compare(opcode, Value { raw: registers[left_index] }, Value { raw: registers[right_index] })
      if outcome.err_code != "" then
        failed_move(registers)
      else
        continue_move(write_raw(registers, destination, outcome.value.raw), program_counter + span)
      end
    end
  end

fn apply_step(registers: [i64], words: [i32], constants: [Value], program_counter: i32) -> CounterMove = do
  let decoded = instruction_decode(Instruction { word: words[program_counter] })
  let span = instruction_word_span(words, program_counter)
  if span <= 0 then
    failed_move(registers)
  else if decoded.opcode == instruction_opcode_return() then
    return_move(registers, decoded.a)
  else if decoded.opcode == instruction_opcode_jump() then
    continue_move(registers, program_counter + span + instruction_jump_offset(words, program_counter))
  else if decoded.opcode == instruction_opcode_jump_if_false() then
    jump_if_false_move(registers, words, decoded.a, program_counter, span)
  else if decoded.opcode == instruction_opcode_load_const() then
    load_constant_move(registers, words, constants, decoded.a, program_counter, span)
  else if decoded.opcode == instruction_opcode_move() then
    move_register(registers, decoded.a, decoded.b, program_counter, span)
  else if opcode_is_binary(decoded.opcode) then
    binary_move(registers, decoded.opcode, decoded.a, decoded.b, decoded.c, program_counter, span)
  else if opcode_is_compare(decoded.opcode) then
    compare_move(registers, decoded.opcode, decoded.a, decoded.b, decoded.c, program_counter, span)
  else
    failed_move(registers)
  end
end

fn count_instructions(words: [i32], constants: [Value], register_count: i32) -> i32 = do
  let mut registers = zero_raw_registers(register_count)
  let mut program_counter = 0
  let mut instruction_steps = 0
  let mut status = 0
  while status == 0 && instruction_steps < step_limit() && program_counter >= 0 && program_counter < words.length() do
    instruction_steps = instruction_steps + 1
    let counter_move = apply_step(registers, words, constants, program_counter)
    registers = counter_move.registers
    program_counter = counter_move.program_counter
    status = counter_move.status
  end
  if status == 1 then
    instruction_steps
  else
    0 - 1
  end
end

fn export_prototype(bytes: [i32]) -> FunctionProto = do
  let decoded = decode_module(bytes)
  let mut words: [i32] = []
  let mut constants: [Value] = []
  let mut prototype = FunctionProto { words: words, constants: constants, register_count: 0 }
  let mut index = 0
  if decoded.code == emb_ok() then
    while index < decoded.module.exports.length() do
      if decoded.module.exports[index].name == "run" then
        prototype = decoded.module.protos[decoded.module.exports[index].proto_index]
      end
      index = index + 1
    end
  end
  prototype
end

fn timed_call(bytes: [i32], prototype: FunctionProto) -> i32 = do
  let instruction_count = count_instructions(prototype.words, prototype.constants, prototype.register_count)
  if instruction_count <= 0 then do
    println("instruction_count_failed")
    0 - 1
  end else
    do
      let mut arguments: [i64] = []
      let started = script_vm_perf_now_us()
      let called = emb_call_export_bytes(bytes, "run", arguments)
      let elapsed = script_vm_perf_now_us() - started
      let value = decoded_integer(called.raw)
      if called.code != emb_ok() || value != expected_total() || elapsed < 0 || elapsed > 2000000 then do
        println("run_failed code=" + called.code.to_string() + " value=" + value.to_string() + " elapsed=" + elapsed.to_string() + " instructions=" + instruction_count.to_string())
        0 - 1
      end else
        elapsed * 1000 / instruction_count
      end
    end
  end
end

fn measure_source(source: string) -> i32 = do
  let lowered = lower_source(source)
  if lowered.diagnostics.length() != 0 then do
    println("diagnostic=" + lowered.diagnostics[0].code)
    0 - 1
  end else
    do
      let prototype = export_prototype(lowered.bytes)
      if prototype.words.length() == 0 then do
        println("export_missing")
        0 - 1
      end else
        timed_call(lowered.bytes, prototype)
      end
    end
  end
end

fn main() -> i32 = do
  let match_nanoseconds = measure_source(match_source())
  let conditional_nanoseconds = measure_source(conditional_source())
  if match_nanoseconds < 0 || conditional_nanoseconds < 0 then
    1
  else
    do
      println("match_ns_per_insn=" + match_nanoseconds.to_string())
      println("if_ns_per_insn=" + conditional_nanoseconds.to_string())
      0
    end
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

def sample_value(output_text, prefix)
  values = []
  output_text.each_line do |line|
    stripped = line.strip
    next unless stripped.start_with?(prefix)
    values.push(Integer(stripped.delete_prefix(prefix)))
  end
  return nil unless values.length == 1
  values[0]
end

def within_ten_percent?(match_nanoseconds, conditional_nanoseconds)
  return false if match_nanoseconds <= 0 || conditional_nanoseconds <= 0
  difference = (match_nanoseconds - conditional_nanoseconds).abs
  difference * 100 <= conditional_nanoseconds * 10
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

entry_path = File.join(ENV["TMPDIR"], "script_vm_match_bench_entry.mlc")
output_directory = File.join(ENV["TMPDIR"], "script_vm_match_bench")
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

minimum_match_nanoseconds = nil
minimum_conditional_nanoseconds = nil
run_index = 0
while run_index < run_count
  binary_output, binary_error, binary_status = capture_process([binary_path], 60)
  if process_failed?(binary_status)
    print binary_output
    warn binary_error unless binary_error.empty?
    warn binary_status == :timeout ? "binary timeout" : "binary failed"
    exit 1
  end
  match_nanoseconds = sample_value(binary_output, match_prefix)
  conditional_nanoseconds = sample_value(binary_output, conditional_prefix)
  if match_nanoseconds.nil? || conditional_nanoseconds.nil? || match_nanoseconds < 0 || conditional_nanoseconds < 0
    print binary_output
    warn binary_error unless binary_error.empty?
    warn "match bench sample missing"
    exit 1
  end
  minimum_match_nanoseconds = match_nanoseconds if minimum_match_nanoseconds.nil? || match_nanoseconds < minimum_match_nanoseconds
  minimum_conditional_nanoseconds = conditional_nanoseconds if minimum_conditional_nanoseconds.nil? || conditional_nanoseconds < minimum_conditional_nanoseconds
  run_index += 1
end

puts "match_ns_per_insn=#{minimum_match_nanoseconds}"
puts "if_ns_per_insn=#{minimum_conditional_nanoseconds}"
unless within_ten_percent?(minimum_match_nanoseconds, minimum_conditional_nanoseconds)
  warn "match_ns_per_insn #{minimum_match_nanoseconds} differs from if_ns_per_insn #{minimum_conditional_nanoseconds} by more than 10%"
  exit 1
end
