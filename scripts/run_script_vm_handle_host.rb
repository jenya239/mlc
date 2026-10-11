#!/usr/bin/env ruby
require "fileutils"
require_relative "script_vm_host_support"

root_directory = ScriptVmHostSupport.repository_root
host_source_path = File.join(root_directory, "script_vm", "tests", "handle_misuse_host.cpp")
work_directory = File.join(root_directory, "tmp", "script_vm_handle_host")
host_directory = File.join(work_directory, "host")
host_binary = File.join(work_directory, "host_bin")

def fail_runner(message)
  warn message
  exit 1
end

ScriptVmHostSupport.prepare_environment(root_directory)
FileUtils.rm_rf(work_directory)

built, build_message = ScriptVmHostSupport.build_embedding_host(
  root_directory,
  host_source_path,
  host_directory,
  host_binary
)
fail_runner(build_message) unless built

host_output, host_error, host_status = ScriptVmHostSupport.run_built_host(host_binary, [], 20)
print host_output
warn host_error unless host_error.empty?
if host_status == :timeout
  fail_runner("host timeout")
end
unless ScriptVmHostSupport.status_code(host_status) == 0
  fail_runner("host exit #{ScriptVmHostSupport.status_code(host_status).inspect}")
end

case_ok_count = host_output.lines.count { |line| line.match?(/\Acase \S+ ok\s*\z/) }
unless case_ok_count == 10
  fail_runner("expected 10 case ok lines, got #{case_ok_count}")
end
puts "handle_host ok cases=10"
