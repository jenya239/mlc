#!/usr/bin/env ruby
require "fileutils"
require "timeout"

root_directory = File.expand_path("..", __dir__)
run_count = 5
entry_path = File.join(root_directory, "script_vm", "tests", "repl_budget_unit.mlc")
success_marker = "[script_vm] repl_budget_unit ok"

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

def fail_runner(message)
  warn message
  exit 1
end

def submit_microseconds(output_text, success_marker)
  measured_microseconds = nil
  confirmed = false
  output_text.each_line do |line|
    stripped = line.strip
    if stripped.match?(/\Arepl_submit_us=\d+\z/)
      return nil unless measured_microseconds.nil?
      measured_microseconds = Integer(stripped.split("=", 2)[1])
    elsif stripped == success_marker
      confirmed = true
    end
  end
  return nil unless confirmed && !measured_microseconds.nil?
  measured_microseconds
end

compiler_directory = File.join(root_directory, "compiler")
mlcc_path = File.join(compiler_directory, "out", "mlcc")
fail_runner("mlcc not found at #{mlcc_path}") unless File.executable?(mlcc_path)
fail_runner("missing #{entry_path}") unless File.file?(entry_path)

ENV["TMPDIR"] = File.join(root_directory, "tmp")
ENV["MLCC_OBJ_CLEAN"] = "1"
ENV["MLCC_PCH"] = "0"
FileUtils.mkdir_p(ENV["TMPDIR"])

output_directory = File.join(root_directory, "tmp", "script_vm_repl_budget")
binary_path = File.join(output_directory, "bin")
FileUtils.rm_rf(output_directory)
FileUtils.mkdir_p(output_directory)

compile_output, compile_error, compile_status = capture_process(
  [mlcc_path, "-o", output_directory, entry_path],
  600
)
warn compile_error unless compile_error.empty?
fail_runner("mlcc failed") if process_failed?(compile_status)
print compile_output

link_output, link_error, link_status = capture_process(
  [File.join(compiler_directory, "build_bin.sh"), output_directory, binary_path],
  600
)
warn link_error unless link_error.empty?
fail_runner("build_bin failed") if process_failed?(link_status)
print link_output

samples = []
run_count.times do
  binary_output, binary_error, binary_status = capture_process([binary_path], 120)
  warn binary_error unless binary_error.empty?
  fail_runner("binary failed") if process_failed?(binary_status)
  measured_microseconds = submit_microseconds(binary_output, success_marker)
  fail_runner("missing repl_submit_us") if measured_microseconds.nil?
  samples << measured_microseconds
end

minimum_microseconds = samples.min
puts "repl_submit_us=#{minimum_microseconds}"
exit 0
