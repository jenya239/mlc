#!/usr/bin/env ruby
require "fileutils"
require_relative "script_vm_host_support"

root_directory = ScriptVmHostSupport.repository_root
tool_source_path = File.join(root_directory, "script_vm", "tools", "script_tool.mlc")
host_source_path = File.join(root_directory, "script_vm", "tests", "lowered_host.cpp")
fixture_directory = File.join(root_directory, "script_vm", "tests", "fixtures")
work_directory = File.join(root_directory, "tmp", "script_vm_lowered_host")
tool_directory = File.join(work_directory, "tool")
tool_binary = File.join(tool_directory, "bin")
host_directory = File.join(work_directory, "host")
host_binary = File.join(work_directory, "host_bin")
blob_directory = File.join(work_directory, "blobs")

compiled_fixtures = [
  ["sum_loop.mlc", "sum_loop.txt"],
  ["fib.mlc", "fib.txt"],
  ["bad_divide.mlc", "bad_divide.txt"],
  ["bad_two_records.mlc", "bad_two_records.txt"],
  ["bad_spin.mlc", "bad_spin.txt"]
]

def fail_runner(message)
  warn message
  exit 1
end

unless File.file?(tool_source_path)
  fail_runner("script_tool missing: script_vm/tools/script_tool.mlc")
end

ScriptVmHostSupport.prepare_environment(root_directory)
compiler_directory = File.join(root_directory, "compiler")
mlcc_path = File.join(compiler_directory, "out", "mlcc")
unless File.executable?(mlcc_path)
  fail_runner("mlcc not found at #{mlcc_path}")
end

FileUtils.rm_rf(work_directory)
FileUtils.mkdir_p(tool_directory)
FileUtils.mkdir_p(blob_directory)

compile_output, compile_error, compile_status = ScriptVmHostSupport.capture_process(
  [mlcc_path, "-o", tool_directory, tool_source_path],
  600
)
warn compile_error unless compile_error.empty?
print compile_output
fail_runner("mlcc failed") unless ScriptVmHostSupport.status_code(compile_status) == 0

link_output, link_error, link_status = ScriptVmHostSupport.capture_process(
  [File.join(compiler_directory, "build_bin.sh"), tool_directory, tool_binary],
  600
)
warn link_error unless link_error.empty?
print link_output
fail_runner("build_bin failed") unless ScriptVmHostSupport.status_code(link_status) == 0

compiled_fixtures.each do |fixture_name, blob_name|
  fixture_path = File.join(fixture_directory, fixture_name)
  blob_path = File.join(blob_directory, blob_name)
  unless File.file?(fixture_path)
    fail_runner("missing fixture #{fixture_name}")
  end
  tool_output, tool_error, tool_status = ScriptVmHostSupport.capture_process(
    [tool_binary, "compile", fixture_path, blob_path],
    20
  )
  print tool_output
  warn tool_error unless tool_error.empty?
  unless ScriptVmHostSupport.status_code(tool_status) == 0 && File.file?(blob_path)
    fail_runner("compile #{fixture_name} failed")
  end
end

built, build_message = ScriptVmHostSupport.build_embedding_host(
  root_directory,
  host_source_path,
  host_directory,
  host_binary
)
fail_runner(build_message) unless built

blob_arguments = compiled_fixtures.map { |_fixture_name, blob_name| File.join(blob_directory, blob_name) }
host_output, host_error, host_status = ScriptVmHostSupport.run_built_host(host_binary, blob_arguments, 20)
print host_output
warn host_error unless host_error.empty?
if host_status == :timeout
  fail_runner("host timeout")
end
unless ScriptVmHostSupport.status_code(host_status) == 0
  fail_runner("host exit #{ScriptVmHostSupport.status_code(host_status).inspect}")
end

case_ok_count = host_output.lines.count { |line| line.match?(/\Acase \S+ ok\s*\z/) }
unless case_ok_count == 6
  fail_runner("expected 6 case ok lines, got #{case_ok_count}")
end
puts "lowered_host ok cases=6"
