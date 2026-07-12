#!/usr/bin/env ruby
# frozen_string_literal: true

# TRACK_STDLIB_HTTP_HARDENING STEP=6 — N concurrent GETs vs forever-demo accept loop.
require "net/http"
require "uri"
require "fileutils"

root = File.expand_path("..", __dir__)
mlcc = ENV.fetch("MLCC", File.join(root, "compiler/out/mlcc"))
out_dir = ENV.fetch(
  "HTTP_LOAD_SMOKE_OUT",
  File.join(ENV.fetch("TMPDIR", File.join(root, "tmp")), "http_load_smoke")
)
entry = File.join(root, "misc/examples/http_server_forever_demo.mlc")
port_file = File.join(out_dir, "http_forever_port.txt")
binary = File.join(out_dir, "http_server_forever_demo")
connection_count = Integer(ENV.fetch("HTTP_LOAD_N", "8"))
# Forever handlers have no artificial sleep; require all succeed and finish well
# under a generous bound (serial connection overhead would still fit here).
max_wall_ms = Integer(ENV.fetch("HTTP_LOAD_MAX_MS", "5000"))

abort("[http load] FAIL: mlcc missing at #{mlcc}") unless File.executable?(mlcc)

FileUtils.rm_rf(out_dir)
FileUtils.mkdir_p(out_dir)

system(mlcc, "-o", out_dir, entry) || abort("[http load] FAIL: mlcc codegen")
unless Dir[File.join(out_dir, "*.cpp")].any? { |path| File.read(path).include?("TaskScope") }
  abort("[http load] FAIL: generated C++ missing TaskScope")
end

env = {
  "MLCC_PCH" => "0",
  "MLCC_DEV" => "1",
  "MLCC_ENTRY_BASENAME" => "http_server_forever_demo",
  "TMPDIR" => ENV.fetch("TMPDIR", File.join(root, "tmp"))
}
system(env, File.join(root, "compiler/build_bin.sh"), out_dir, binary) ||
  abort("[http load] FAIL: link")

server_log = File.join(out_dir, "server.log")
server_pid = spawn(binary, chdir: out_dir, out: server_log, err: server_log)
at_exit do
  Process.kill("TERM", server_pid) rescue nil
  Process.wait(server_pid) rescue nil
end

port = nil
deadline = Time.now + 30
while Time.now < deadline
  if File.file?(port_file) && !File.zero?(port_file)
    port = File.read(port_file).strip
    break
  end
  begin
    Process.waitpid(server_pid, Process::WNOHANG)
  rescue Errno::ECHILD
    warn File.read(server_log) rescue nil
    abort("[http load] FAIL: server exited early")
  end
  sleep 0.05
end
abort("[http load] FAIL: no port file") if port.nil? || port.empty?

uri = URI("http://127.0.0.1:#{port}/health")
warn "[http load] #{connection_count} concurrent GETs → #{uri}"
started = Process.clock_gettime(Process::CLOCK_MONOTONIC)
threads = Array.new(connection_count) do |index|
  Thread.new do
    response = Net::HTTP.get_response(uri)
    [index, response.code, response.body]
  end
end
results = threads.map(&:value)
elapsed_ms = ((Process.clock_gettime(Process::CLOCK_MONOTONIC) - started) * 1000).round

results.each do |index, code, body|
  unless code == "200" && body.include?("alive")
    abort("[http load] FAIL curl ##{index + 1} code=#{code} body=#{body.inspect}")
  end
end

if elapsed_ms >= max_wall_ms
  abort("[http load] FAIL: wall #{elapsed_ms}ms >= #{max_wall_ms}ms")
end

follow = Net::HTTP.get_response(uri)
unless follow.code == "200" && follow.body.include?("alive")
  abort("[http load] FAIL: post-burst request #{follow.code}")
end

Process.kill("TERM", server_pid) rescue nil
Process.wait(server_pid) rescue nil
at_exit { true }

warn "[http load] ok (#{connection_count} concurrent, wall #{elapsed_ms}ms < #{max_wall_ms}ms)"
