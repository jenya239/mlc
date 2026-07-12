#!/usr/bin/env ruby
# frozen_string_literal: true

# TRACK_STDLIB_HTTP_HARDENING STEP=3 — keep-alive idle → SO_RCVTIMEO closes read.
require "socket"
require "fileutils"

root = File.expand_path("..", __dir__)
mlcc = ENV.fetch("MLCC", File.join(root, "compiler/out/mlcc"))
out_dir = ENV.fetch(
  "HTTP_IDLE_TIMEOUT_SMOKE_OUT",
  File.join(ENV.fetch("TMPDIR", File.join(root, "tmp")), "http_idle_timeout_smoke")
)
entry = File.join(root, "misc/examples/http_idle_timeout_smoke.mlc")
port_file = File.join(out_dir, "http_idle_timeout_port.txt")
binary = File.join(out_dir, "http_idle_timeout_smoke")

abort("[http idle] FAIL: mlcc missing at #{mlcc}") unless File.executable?(mlcc)

FileUtils.rm_rf(out_dir)
FileUtils.mkdir_p(out_dir)

system(mlcc, "-o", out_dir, entry) || abort("[http idle] FAIL: mlcc codegen")
env = {
  "MLCC_PCH" => "0",
  "MLCC_DEV" => "1",
  "MLCC_ENTRY_BASENAME" => "http_idle_timeout_smoke",
  "TMPDIR" => ENV.fetch("TMPDIR", File.join(root, "tmp"))
}
system(env, File.join(root, "compiler/build_bin.sh"), out_dir, binary) ||
  abort("[http idle] FAIL: link")

server_log = File.join(out_dir, "server.log")
started = Process.clock_gettime(Process::CLOCK_MONOTONIC)
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
    abort("[http idle] FAIL: server exited early")
  end
  sleep 0.05
end
abort("[http idle] FAIL: no port file") if port.nil? || port.empty?

socket = TCPSocket.new("127.0.0.1", Integer(port))
socket.write("GET /idle HTTP/1.1\r\nHost: localhost\r\nConnection: keep-alive\r\n\r\n")
socket.flush
response = +""
deadline = Time.now + 5
while Time.now < deadline && !response.include?("\r\n\r\n")
  chunk = socket.read_nonblock(4096, exception: false)
  response << chunk if chunk.is_a?(String)
  sleep 0.01 if chunk == :wait_readable
end
abort("[http idle] FAIL: empty/partial response #{response.inspect}") unless response.start_with?("HTTP/1.1 200")
abort("[http idle] FAIL: expected keep-alive") unless response.downcase.include?("connection: keep-alive")

# Do not send a second request — server must time out (~1s) and exit 0.
status = Process.wait2(server_pid)[1].exitstatus
elapsed = Process.clock_gettime(Process::CLOCK_MONOTONIC) - started
at_exit { true }
socket.close rescue nil

abort("[http idle] FAIL: server exit=#{status} log=#{File.read(server_log) rescue nil}") unless status == 0
if elapsed < 0.8 || elapsed > 8.0
  abort("[http idle] FAIL: elapsed=#{elapsed.round(2)}s (expected ~1–3s idle timeout)")
end
warn "[http idle] ok (SO_RCVTIMEO closed keep-alive after ~#{elapsed.round(2)}s)"
