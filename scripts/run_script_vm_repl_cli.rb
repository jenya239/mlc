#!/usr/bin/env ruby
require "fileutils"
require "io/wait"
require "pty"
require "timeout"

root_directory = File.expand_path("..", __dir__)
tool_source_path = File.join(root_directory, "script_vm", "tools", "script_tool.mlc")
identity_declaration = "export fn identity(number: i32) -> i32 = number"
identity_call = "identity(4)"
accepted_line = "repl answer=accepted"
value_line = "repl answer=value kind=int32 number=4"

def capture_process(arguments, timeout_seconds, standard_input_text = nil)
  output_reader, output_writer = IO.pipe
  error_reader, error_writer = IO.pipe
  input_reader = nil
  input_writer = nil
  unless standard_input_text.nil?
    input_reader, input_writer = IO.pipe
  end
  spawn_arguments = { out: output_writer, err: error_writer }
  spawn_arguments[:in] = input_reader unless input_reader.nil?
  child_process_identifier = Process.spawn(*arguments, **spawn_arguments)
  output_writer.close
  error_writer.close
  input_reader.close unless input_reader.nil?
  standard_output = +""
  standard_error = +""
  output_reader_thread = Thread.new { standard_output.replace(output_reader.read.to_s) }
  error_reader_thread = Thread.new { standard_error.replace(error_reader.read.to_s) }
  unless input_writer.nil?
    input_writer.write(standard_input_text)
    input_writer.close
  end
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
  [standard_output, standard_error, process_status]
end

def status_code(process_status)
  return nil if process_status == :timeout
  return nil unless process_status.is_a?(Process::Status)
  return nil unless process_status.exited?
  process_status.exitstatus
end

def fail_tool(message)
  warn message
  exit 1
end

TERMINAL_PROMPT = "> "
CONTINUATION_PROMPT = "... "
END_OF_TRANSMISSION = "\x04"
INCOMPLETE_END_MARKER = "repl answer=diagnostic code=repl_incomplete_eof"
ADD_VALUE_LINE = "repl answer=value kind=int32 number=42"

def stop_terminal(process_identifier)
  Process.kill("KILL", process_identifier)
  begin
    Process.wait(process_identifier)
  rescue Errno::ECHILD
    nil
  end
rescue Errno::ESRCH
  nil
end

def fail_terminal(process_identifier, message)
  stop_terminal(process_identifier)
  fail_tool(message)
end

def wait_for_terminal_marker(output, accumulated, start_index, marker, process_identifier)
  deadline = Process.clock_gettime(Process::CLOCK_MONOTONIC) + 20
  found_index = accumulated.index(marker, start_index)
  while found_index.nil?
    remaining_seconds = deadline - Process.clock_gettime(Process::CLOCK_MONOTONIC)
    if remaining_seconds <= 0
      fail_terminal(process_identifier, "terminal timed out waiting for #{marker.inspect}\n#{accumulated}")
    end
    unless output.wait_readable(remaining_seconds)
      fail_terminal(process_identifier, "terminal timed out waiting for #{marker.inspect}\n#{accumulated}")
    end
    begin
      accumulated << output.readpartial(4096)
    rescue EOFError, Errno::EIO
      fail_terminal(process_identifier, "terminal closed waiting for #{marker.inspect}\n#{accumulated}")
    end
    found_index = accumulated.index(marker, start_index)
  end
  found_index + marker.length
end

def write_terminal_text(input, text)
  input.write(text)
  input.flush
end

def read_terminal_rest(output, process_identifier)
  remainder = +""
  deadline = Process.clock_gettime(Process::CLOCK_MONOTONIC) + 20
  loop do
    remaining_seconds = deadline - Process.clock_gettime(Process::CLOCK_MONOTONIC)
    break if remaining_seconds <= 0
    break unless output.wait_readable(remaining_seconds)
    begin
      remainder << output.readpartial(4096)
    rescue EOFError, Errno::EIO
      break
    end
  end
  process_status = nil
  begin
    Timeout.timeout(20) do
      _, process_status = Process.wait2(process_identifier)
    end
  rescue Timeout::Error
    fail_terminal(process_identifier, "terminal repl did not exit\n#{remainder}")
  end
  [remainder, process_status]
end

def reject_answer_before_prompt(accumulated, start_index, prompt_index, process_identifier)
  return if prompt_index <= start_index
  segment = accumulated[start_index...prompt_index]
  return unless segment.include?("repl answer=")
  fail_terminal(process_identifier, "incomplete line printed an answer\n#{accumulated}")
end

def check_terminal_session(tool_binary)
  PTY.spawn(tool_binary, "repl") do |output, input, process_identifier|
    output.binmode
    input.binmode
    input.sync = true
    accumulated = +""
    cursor = wait_for_terminal_marker(output, accumulated, 0, TERMINAL_PROMPT, process_identifier)
    write_terminal_text(input, "export fn identity(number: i32) -> i32 = number\n")
    cursor = wait_for_terminal_marker(output, accumulated, cursor, "repl answer=accepted", process_identifier)
    cursor = wait_for_terminal_marker(output, accumulated, cursor, TERMINAL_PROMPT, process_identifier)
    write_terminal_text(input, "export fn add(left: i32, right: i32) -> i32 = do\n")
    continuation_cursor = wait_for_terminal_marker(output, accumulated, cursor, CONTINUATION_PROMPT, process_identifier)
    reject_answer_before_prompt(accumulated, cursor, continuation_cursor, process_identifier)
    cursor = continuation_cursor
    write_terminal_text(input, "left + right\n")
    continuation_cursor = wait_for_terminal_marker(output, accumulated, cursor, CONTINUATION_PROMPT, process_identifier)
    reject_answer_before_prompt(accumulated, cursor, continuation_cursor, process_identifier)
    cursor = continuation_cursor
    write_terminal_text(input, "end\n")
    cursor = wait_for_terminal_marker(output, accumulated, cursor, "repl answer=accepted", process_identifier)
    cursor = wait_for_terminal_marker(output, accumulated, cursor, TERMINAL_PROMPT, process_identifier)
    write_terminal_text(input, "add(20, 22)\n")
    cursor = wait_for_terminal_marker(output, accumulated, cursor, ADD_VALUE_LINE, process_identifier)
    wait_for_terminal_marker(output, accumulated, cursor, TERMINAL_PROMPT, process_identifier)
    write_terminal_text(input, END_OF_TRANSMISSION)
    remainder, process_status = read_terminal_rest(output, process_identifier)
    accumulated << remainder
    exit_code = status_code(process_status)
    fail_tool("terminal repl exit #{exit_code.inspect}\n#{accumulated}") unless exit_code == 0
    fail_tool("terminal repl reported incomplete at end\n#{accumulated}") if accumulated.include?(INCOMPLETE_END_MARKER)
  end
end

def check_terminal_incomplete_end(tool_binary)
  PTY.spawn(tool_binary, "repl") do |output, input, process_identifier|
    output.binmode
    input.binmode
    input.sync = true
    accumulated = +""
    cursor = wait_for_terminal_marker(output, accumulated, 0, TERMINAL_PROMPT, process_identifier)
    write_terminal_text(input, "export fn add(left: i32, right: i32) -> i32 = do\n")
    cursor = wait_for_terminal_marker(output, accumulated, cursor, CONTINUATION_PROMPT, process_identifier)
    write_terminal_text(input, END_OF_TRANSMISSION)
    wait_for_terminal_marker(output, accumulated, cursor, INCOMPLETE_END_MARKER, process_identifier)
    remainder, process_status = read_terminal_rest(output, process_identifier)
    accumulated << remainder
    exit_code = status_code(process_status)
    fail_tool("terminal incomplete exit #{exit_code.inspect}\n#{accumulated}") unless exit_code == 1
  end
end

unless File.file?(tool_source_path)
  fail_tool("script_tool missing: script_vm/tools/script_tool.mlc")
end

compiler_directory = File.join(root_directory, "compiler")
mlcc_path = File.join(compiler_directory, "out", "mlcc")
unless File.executable?(mlcc_path)
  fail_tool("mlcc not found at #{mlcc_path}")
end

ENV["TMPDIR"] = File.join(root_directory, "tmp")
ENV["MLCC_OBJ_CLEAN"] = "1"
ENV["MLCC_PCH"] = "0"
FileUtils.mkdir_p(ENV["TMPDIR"])

output_directory = File.join(root_directory, "tmp", "script_vm_repl_cli")
tool_binary = File.join(output_directory, "bin")
FileUtils.rm_rf(output_directory)
FileUtils.mkdir_p(output_directory)

compile_output, compile_error, compile_status = capture_process(
  [mlcc_path, "-o", output_directory, tool_source_path],
  600
)
warn compile_error unless compile_error.empty?
fail_tool("mlcc failed") unless status_code(compile_status) == 0
print compile_output

link_output, link_error, link_status = capture_process(
  [File.join(compiler_directory, "build_bin.sh"), output_directory, tool_binary],
  600
)
warn link_error unless link_error.empty?
fail_tool("build_bin failed") unless status_code(link_status) == 0
print link_output

standard_input_text = [identity_declaration, identity_call].join("\n") + "\n"
repl_output, repl_error, repl_status = capture_process(
  [tool_binary, "repl"],
  20,
  standard_input_text
)
warn repl_error unless repl_error.empty?
print repl_output
repl_exit_code = status_code(repl_status)
output_lines = repl_output.lines.map(&:strip)
unless repl_exit_code == 0 && output_lines.include?(accepted_line) && output_lines.include?(value_line)
  fail_tool("repl cli exit #{repl_exit_code.inspect}")
end
check_terminal_session(tool_binary)
check_terminal_incomplete_end(tool_binary)
puts "repl_cli ok"
