#!/usr/bin/env ruby
require "fileutils"
require "timeout"

root_directory = File.expand_path("..", __dir__)
compiler_directory = File.join(root_directory, "compiler")
mlcc_path = File.join(compiler_directory, "out", "mlcc")
unless File.executable?(mlcc_path)
  warn "mlcc not found at #{mlcc_path}"
  exit 1
end

ENV["TMPDIR"] = File.join(root_directory, "tmp")
ENV["MLCC_PCH"] = "0"
ENV["MLCC_OBJ_CLEAN"] = "1"
FileUtils.mkdir_p(ENV["TMPDIR"])

cairo_include = File.join(root_directory, "tmp", "debs", "extract", "usr", "include")
if File.directory?(cairo_include)
  existing_include = ENV["CPLUS_INCLUDE_PATH"]
  ENV["CPLUS_INCLUDE_PATH"] = [cairo_include, existing_include].compact.reject(&:empty?).join(":")
end

def capture_process(arguments, environment, timeout_seconds)
  output_reader, output_writer = IO.pipe
  error_reader, error_writer = IO.pipe
  child_process_identifier = Process.spawn(environment, *arguments, out: output_writer, err: error_writer)
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

def compile_and_link(mlcc_path, compiler_directory, entry_path, output_directory, binary_path, entry_basename)
  FileUtils.rm_rf(output_directory)
  FileUtils.mkdir_p(output_directory)
  compile_output, compile_error, compile_status = capture_process(
    [mlcc_path, "-o", output_directory, entry_path],
    ENV.to_h,
    600
  )
  print compile_output
  warn compile_error unless compile_error.empty?
  if process_failed?(compile_status)
    warn "mlcc failed for #{entry_path}"
    exit 1
  end
  link_environment = ENV.to_h.merge("MLCC_ENTRY_BASENAME" => entry_basename)
  link_output, link_error, link_status = capture_process(
    [File.join(compiler_directory, "build_bin.sh"), output_directory, binary_path],
    link_environment,
    600
  )
  print link_output
  warn link_error unless link_error.empty?
  if process_failed?(link_status)
    warn "build_bin failed for #{entry_path}"
    exit 1
  end
end

sandbox_entry = File.join(root_directory, "script_host", "tools", "sandbox_main.mlc")
sandbox_directory = File.join(root_directory, "tmp", "script_vm_sandbox_main")
sandbox_binary = File.join(sandbox_directory, "bin")
compile_and_link(
  mlcc_path,
  compiler_directory,
  sandbox_entry,
  sandbox_directory,
  sandbox_binary,
  "sandbox_main"
)

entry_argument = ARGV[0]
if entry_argument.nil? || entry_argument.empty?
  entry_argument = "script_host/tests/sandbox_child_unit.mlc"
end
unit_entry = File.expand_path(entry_argument, root_directory)
unless File.file?(unit_entry)
  warn "No such file: #{unit_entry}"
  exit 1
end
entry_basename = File.basename(unit_entry, ".mlc")
unit_name = entry_basename.sub(/_unit\z/, "")
unit_directory = File.join(root_directory, "tmp", "script_vm_#{unit_name}")
unit_binary = File.join(unit_directory, "bin")
compile_and_link(
  mlcc_path,
  compiler_directory,
  unit_entry,
  unit_directory,
  unit_binary,
  entry_basename
)

binary_environment = ENV.to_h
binary_environment["MLC_SANDBOX_CHILD"] = sandbox_binary
binary_environment["DISPLAY"] = ":99"
binary_environment["WAYLAND_DISPLAY"] = "wayland-99"
binary_environment["DBUS_SESSION_BUS_ADDRESS"] = "unix:path=/tmp/mlc-sandbox-should-not-pass"
binary_environment["XAUTHORITY"] = "/tmp/mlc-sandbox-should-not-pass"
binary_environment["HOME"] = "/tmp/mlc-sandbox-should-not-pass"

binary_output, binary_error, binary_status = capture_process([unit_binary], binary_environment, 60)
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

success_marker = "[script_vm] #{entry_basename} ok"
unless binary_output.match?(/#{Regexp.escape(success_marker)}(?:\r?\n|\z)/)
  warn "missing #{success_marker}"
  exit 1
end

puts "unit ok #{unit_name}"
exit 0
