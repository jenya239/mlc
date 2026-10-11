#!/usr/bin/env ruby
require "fileutils"
require "timeout"

root_directory = File.expand_path("..", __dir__)
tool_source_path = File.join(root_directory, "script_vm", "tools", "script_tool.mlc")
fixture_directory = File.join(root_directory, "script_vm", "tests", "fixtures")
expected_fixtures = [
  ["sum_loop.mlc", 55],
  ["fib.mlc", 55],
  ["gcd.mlc", 6],
  ["point.mlc", 7],
  ["nested_loops.mlc", 100]
]

def capture_process(arguments, timeout_seconds)
  output_reader, output_writer = IO.pipe
  error_reader, error_writer = IO.pipe
  child_process_identifier = Process.spawn(*arguments, out: output_writer, err: error_writer)
  output_writer.close
  error_writer.close
  standard_output = +""
  standard_error = +""
  output_reader_thread = Thread.new { standard_output.replace(output_reader.read.to_s) }
  error_reader_thread = Thread.new { standard_error.replace(error_reader.read.to_s) }
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

output_directory = File.join(root_directory, "tmp", "script_vm_tool")
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

def run_tool(tool_binary, arguments)
  capture_process([tool_binary, *arguments], 20)
end

def require_status(process_status, expected_code, label)
  actual_code = status_code(process_status)
  return if actual_code == expected_code
  if process_status == :timeout
    fail_tool("#{label} timeout")
  else
    fail_tool("#{label} exit #{actual_code.inspect}")
  end
end

no_argument_output, no_argument_error, no_argument_status = run_tool(tool_binary, [])
warn no_argument_error unless no_argument_error.empty?
print no_argument_output
require_status(no_argument_status, 2, "no arguments")

expected_fixtures.each do |fixture_name, expected_result|
  fixture_path = File.join(fixture_directory, fixture_name)
  unless File.file?(fixture_path)
    fail_tool("fixture missing: #{fixture_name}")
  end
  if expected_result < 0 || expected_result >= 256
    fail_tool("#{fixture_name} result out of range")
  end
  run_output, run_error, run_status = run_tool(tool_binary, ["run", fixture_path, "run"])
  warn run_error unless run_error.empty?
  print run_output
  require_status(run_status, 0, "#{fixture_name} run")
  unless run_output.lines.any? { |line| line.strip == "result=#{expected_result}" }
    fail_tool("#{fixture_name} result")
  end
  compile_output_path = File.join(output_directory, "#{fixture_name}.txt")
  compile_run_output, compile_run_error, compile_run_status = run_tool(
    tool_binary,
    ["compile", fixture_path, compile_output_path]
  )
  warn compile_run_error unless compile_run_error.empty?
  print compile_run_output
  require_status(compile_run_status, 0, "#{fixture_name} compile")
  unless File.file?(compile_output_path)
    fail_tool("#{fixture_name} compile missing")
  end
  compile_text = File.read(compile_output_path).strip
  unless compile_text.match?(/\A-?\d+(?: -?\d+)*\z/)
    fail_tool("#{fixture_name} compile bytes")
  end
end

bad_fixture_path = File.join(fixture_directory, "bad_unsupported.mlc")
bad_output, bad_error, bad_status = run_tool(tool_binary, ["run", bad_fixture_path, "run"])
print bad_output
require_status(bad_status, 1, "bad_unsupported")
unless bad_error.lines.any? { |line| line.strip.match?(/\A.+:\d+:\d+: unsupported_string: .+\z/) }
  fail_tool("bad_unsupported diagnostic")
end

puts "lower_tool ok fixtures=#{expected_fixtures.length}"
exit 0
