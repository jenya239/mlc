#!/usr/bin/env ruby
require "fileutils"
require "timeout"

root_directory = File.expand_path("..", __dir__)
entry_argument = ARGV[0]
if entry_argument.nil? || entry_argument.empty?
  warn "No such file"
  exit 1
end

entry_path = File.expand_path(entry_argument, root_directory)
unless File.file?(entry_path)
  warn "No such file: #{entry_path}"
  exit 1
end

entry_basename = File.basename(entry_path)
unless entry_basename.end_with?("_unit.mlc")
  warn "entry name must end with _unit.mlc"
  exit 1
end
unit_name = entry_basename.sub(/_unit\.mlc\z/, "")

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

output_directory = File.join(root_directory, "tmp", "script_vm_#{unit_name}")
binary_path = File.join(output_directory, "bin")
FileUtils.rm_rf(output_directory)
FileUtils.mkdir_p(output_directory)

def capture_process(arguments, timeout_seconds)
  output_reader, output_writer = IO.pipe
  error_reader, error_writer = IO.pipe
  child_process_identifier = Process.spawn(*arguments, out: output_writer, err: error_writer)
  output_writer.close
  error_writer.close
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
    output_reader.close
    error_reader.close
    return ["", "", :timeout]
  end
  output_text = output_reader.read
  error_text = error_reader.read
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

compile_output, compile_error, compile_status = capture_process(
  [mlcc_path, "-o", output_directory, entry_path],
  600
)
print compile_output
warn compile_error unless compile_error.empty?
if process_failed?(compile_status)
  warn "mlcc failed"
  exit 1
end

link_output, link_error, link_status = capture_process(
  [File.join(compiler_directory, "build_bin.sh"), output_directory, binary_path],
  600
)
print link_output
warn link_error unless link_error.empty?
if process_failed?(link_status)
  warn "build_bin failed"
  exit 1
end

binary_output, binary_error, binary_status = capture_process([binary_path], 60)
print binary_output
warn binary_error unless binary_error.empty?
if binary_status == :timeout
  warn "binary timeout"
  exit 1
end
if process_failed?(binary_status)
  warn "binary failed"
  exit 1
end

success_marker = "[script_vm] #{unit_name}_unit ok"
unless binary_output.match?(/#{Regexp.escape(success_marker)}(?:\r?\n|\z)/)
  warn "missing #{success_marker}"
  exit 1
end

puts "unit ok #{unit_name}"
exit 0
