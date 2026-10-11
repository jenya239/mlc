#!/usr/bin/env ruby
require "fileutils"
require_relative "script_vm_host_support"

root_directory = ScriptVmHostSupport.repository_root
tool_source_path = File.join(root_directory, "script_vm", "tools", "script_tool.mlc")
host_source_path = File.join(root_directory, "script_vm", "tests", "call_overhead_host.cpp")
fixture_directory = File.join(root_directory, "script_vm", "tests", "fixtures")
work_directory = File.join(root_directory, "tmp", "script_vm_call_overhead")
tool_directory = File.join(work_directory, "tool")
tool_binary = File.join(tool_directory, "bin")
host_directory = File.join(work_directory, "host")
host_binary = File.join(work_directory, "host_bin")
blob_directory = File.join(work_directory, "blobs")

compiled_fixtures = [
  ["call_add1.mlc", "call_add1.txt"],
  ["sum_loop.mlc", "sum_loop.txt"]
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

_compile_output, compile_error, compile_status = ScriptVmHostSupport.capture_process(
  [mlcc_path, "-o", tool_directory, tool_source_path],
  600
)
fail_runner("mlcc failed\n#{compile_error}") unless ScriptVmHostSupport.status_code(compile_status) == 0

_link_output, link_error, link_status = ScriptVmHostSupport.capture_process(
  [File.join(compiler_directory, "build_bin.sh"), tool_directory, tool_binary],
  600
)
fail_runner("build_bin failed\n#{link_error}") unless ScriptVmHostSupport.status_code(link_status) == 0

compiled_fixtures.each do |fixture_name, blob_name|
  fixture_path = File.join(fixture_directory, fixture_name)
  blob_path = File.join(blob_directory, blob_name)
  unless File.file?(fixture_path)
    fail_runner("missing fixture #{fixture_name}")
  end
  _tool_output, tool_error, tool_status = ScriptVmHostSupport.capture_process(
    [tool_binary, "compile", fixture_path, blob_path],
    20
  )
  unless ScriptVmHostSupport.status_code(tool_status) == 0 && File.file?(blob_path)
    fail_runner("compile #{fixture_name} failed\n#{tool_error}")
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
warn host_error unless host_error.empty?
if host_status == :timeout
  fail_runner("host timeout")
end
unless ScriptVmHostSupport.status_code(host_status) == 0
  print host_output
  fail_runner("host exit #{ScriptVmHostSupport.status_code(host_status).inspect}")
end

measurements = {}
host_output.each_line do |line|
  match = line.match(/\A(trivial_call_nanoseconds|sum_call_nanoseconds) (\d+)\s*\z/)
  measurements[match[1]] = match[2].to_i if match
end
trivial_call_nanoseconds = measurements["trivial_call_nanoseconds"]
sum_call_nanoseconds = measurements["sum_call_nanoseconds"]
if trivial_call_nanoseconds.nil? || sum_call_nanoseconds.nil? || trivial_call_nanoseconds <= 0 || sum_call_nanoseconds <= 0
  print host_output
  fail_runner("missing positive timings")
end
fixed_share_times_100 = trivial_call_nanoseconds * 100 / sum_call_nanoseconds
if fixed_share_times_100 <= 0
  fail_runner("fixed share is not positive")
end

puts "S95.trivial_call_ns=#{trivial_call_nanoseconds}"
puts "S95.sum_call_ns=#{sum_call_nanoseconds}"
puts "S95.fixed_share_x100=#{fixed_share_times_100}"
