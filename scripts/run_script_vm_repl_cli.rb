#!/usr/bin/env ruby
require "fileutils"
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
if repl_exit_code == 0 && output_lines.include?(accepted_line) && output_lines.include?(value_line)
  puts "repl_cli ok"
  exit 0
end
fail_tool("repl cli exit #{repl_exit_code.inspect}")
