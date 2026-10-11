#!/usr/bin/env ruby
require "fileutils"
require "timeout"

root_directory = File.expand_path("..", __dir__)
fixture_directory = File.join(root_directory, "script_vm", "tests", "fixtures")
tool_source_path = File.join(root_directory, "script_vm", "tools", "script_tool.mlc")
compiler_directory = File.join(root_directory, "compiler")
mlcc_path = File.join(compiler_directory, "out", "mlcc")
build_script = File.join(compiler_directory, "build_bin.sh")
build_timeout_seconds = 60
run_timeout_seconds = 20

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

def fail_differential(message)
  warn message
  exit 1
end

def first_lines(text)
  text.to_s.lines.map(&:strip).reject(&:empty?).first(4).join(" | ")
end

unless File.file?(tool_source_path)
  fail_differential("script_tool missing: script_vm/tools/script_tool.mlc")
end
unless File.executable?(mlcc_path)
  fail_differential("mlcc not found at #{mlcc_path}")
end

ENV["TMPDIR"] = File.join(root_directory, "tmp")
ENV["MLCC_OBJ_CLEAN"] = "1"
ENV["MLCC_PCH"] = "0"
FileUtils.mkdir_p(ENV["TMPDIR"])

tool_directory = File.join(root_directory, "tmp", "script_vm_tool")
tool_binary = File.join(tool_directory, "bin")
FileUtils.rm_rf(tool_directory)
FileUtils.mkdir_p(tool_directory)

tool_compile_output, tool_compile_error, tool_compile_status = capture_process(
  [mlcc_path, "-o", tool_directory, tool_source_path],
  build_timeout_seconds
)
unless status_code(tool_compile_status) == 0
  fail_differential("script_tool mlcc failed: #{first_lines(tool_compile_error + tool_compile_output)}")
end

tool_link_output, tool_link_error, tool_link_status = capture_process(
  [build_script, tool_directory, tool_binary],
  build_timeout_seconds
)
unless status_code(tool_link_status) == 0
  fail_differential("script_tool build_bin failed: #{first_lines(tool_link_error + tool_link_output)}")
end

def native_result(mlcc_path, build_script, output_directory, binary_path, fixture_path, build_timeout_seconds, run_timeout_seconds)
  FileUtils.rm_rf(output_directory)
  FileUtils.mkdir_p(output_directory)
  compile_output, compile_error, compile_status = capture_process(
    [mlcc_path, "-o", output_directory, fixture_path],
    build_timeout_seconds
  )
  unless status_code(compile_status) == 0
    return ["native compile", first_lines(compile_error + compile_output)]
  end
  link_output, link_error, link_status = capture_process(
    [build_script, output_directory, binary_path],
    build_timeout_seconds
  )
  unless status_code(link_status) == 0
    return ["native link", first_lines(link_error + link_output)]
  end
  _run_output, run_error, run_status = capture_process([binary_path], run_timeout_seconds)
  native_code = status_code(run_status)
  return ["native run", first_lines(run_error)] if native_code.nil?
  ["ok", native_code]
end

def script_result(tool_binary, fixture_path, run_timeout_seconds)
  standard_output, standard_error, process_status = capture_process(
    [tool_binary, "run", fixture_path, "run"],
    run_timeout_seconds
  )
  unless status_code(process_status) == 0
    return ["script run", first_lines(standard_error + standard_output)]
  end
  result_line = standard_output.lines.map(&:strip).find { |line| line.match?(/\Aresult=-?\d+\z/) }
  return ["script result", first_lines(standard_output)] if result_line.nil?
  ["ok", result_line.delete_prefix("result=").to_i]
end

fixture_paths = Dir[File.join(fixture_directory, "*.mlc")].reject { |fixture_path|
  File.basename(fixture_path).start_with?("bad_")
}.sort
difference_directory = File.join(root_directory, "tmp", "script_vm_diff")
FileUtils.mkdir_p(difference_directory)

fixture_paths.each do |fixture_path|
  fixture_name = File.basename(fixture_path, ".mlc")
  output_directory = File.join(difference_directory, fixture_name)
  binary_path = File.join(output_directory, "bin")
  native_thread = Thread.new {
    native_result(mlcc_path, build_script, output_directory, binary_path, fixture_path, build_timeout_seconds, run_timeout_seconds)
  }
  script_thread = Thread.new {
    script_result(tool_binary, fixture_path, run_timeout_seconds)
  }
  native_outcome = native_thread.value
  script_outcome = script_thread.value
  if native_outcome[0] != "ok"
    fail_differential("#{fixture_name} #{native_outcome[0]}: #{native_outcome[1]}")
  end
  if script_outcome[0] != "ok"
    fail_differential("#{fixture_name} #{script_outcome[0]}: #{script_outcome[1]}")
  end
  native_value = native_outcome[1]
  script_value = script_outcome[1]
  puts "differential fixture=#{fixture_name} native=#{native_value} script=#{script_value}"
  if native_value != script_value
    exit 1
  end
end

puts "differential ok fixtures=#{fixture_paths.length}"
exit 0
