#!/usr/bin/env ruby
require "fileutils"
require_relative "script_vm_host_support"

root_directory = ScriptVmHostSupport.repository_root
host_source_path = File.join(root_directory, "script_vm", "tests", "repl_host.cpp")
work_directory = File.join(root_directory, "tmp", "script_vm_repl_host_budget")
host_directory = File.join(work_directory, "host")
host_binary = File.join(work_directory, "host_bin")
run_count = 5

def parse_maximum_microseconds(arguments)
  return nil if arguments.empty?
  unless arguments.length == 2 && arguments[0] == "--max-absolute"
    warn "unknown argument #{arguments[0]}"
    exit 1
  end
  metric_name, limit_text = arguments[1].to_s.split("=", 2)
  limit = Integer(limit_text) rescue nil
  if metric_name != "repl_host_submit_us" || limit.nil?
    warn "max-absolute #{arguments[1]}"
    exit 1
  end
  limit
end

def fail_runner(message)
  warn message
  exit 1
end

def submit_microseconds(output_text)
  measured_microseconds = nil
  output_text.each_line do |line|
    stripped = line.strip
    next unless stripped.match?(/\Arepl_host_submit_us=\d+\z/)
    return nil unless measured_microseconds.nil?
    measured_microseconds = Integer(stripped.split("=", 2)[1])
  end
  measured_microseconds
end

maximum_microseconds = parse_maximum_microseconds(ARGV)

ScriptVmHostSupport.prepare_environment(root_directory)
FileUtils.rm_rf(work_directory)

built, build_message = ScriptVmHostSupport.build_embedding_host(
  root_directory,
  host_source_path,
  host_directory,
  host_binary
)
fail_runner(build_message) unless built

samples = []
run_count.times do
  host_output, host_error, host_status = ScriptVmHostSupport.run_built_host(host_binary, ["budget"], 20)
  warn host_error unless host_error.empty?
  if host_status == :timeout
    fail_runner("host timeout")
  end
  unless ScriptVmHostSupport.status_code(host_status) == 0
    print host_output
    fail_runner("host exit #{ScriptVmHostSupport.status_code(host_status).inspect}")
  end
  measured_microseconds = submit_microseconds(host_output)
  fail_runner("missing repl_host_submit_us") if measured_microseconds.nil?
  samples << measured_microseconds
end

minimum_microseconds = samples.min
puts "repl_host_submit_us=#{minimum_microseconds}"
if !maximum_microseconds.nil? && minimum_microseconds > maximum_microseconds
  warn "max-absolute failed repl_host_submit_us=#{minimum_microseconds} limit=#{maximum_microseconds}"
  exit 1
end
exit 0
