#!/usr/bin/env ruby
# frozen_string_literal: true

# TRACK_STDLIB_HTTP_HARDENING STEP=2 — Content-Length > 1MiB → HTTP 413.
require "socket"
require "fileutils"

root = File.expand_path("..", __dir__)
mlcc = ENV.fetch("MLCC", File.join(root, "compiler/out/mlcc"))
out_dir = ENV.fetch(
  "HTTP_413_SMOKE_OUT",
  File.join(ENV.fetch("TMPDIR", File.join(root, "tmp")), "http_413_smoke")
)
entry = File.join(root, "misc/examples/http_keepalive_smoke.mlc")
port_file = File.join(out_dir, "http_keepalive_port.txt")
binary = File.join(out_dir, "http_keepalive_smoke")

abort("[http 413] FAIL: mlcc missing at #{mlcc}") unless File.executable?(mlcc)

FileUtils.rm_rf(out_dir)
FileUtils.mkdir_p(out_dir)

system(mlcc, "-o", out_dir, entry) || abort("[http 413] FAIL: mlcc codegen")
env = {
  "MLCC_PCH" => "0",
  "MLCC_DEV" => "1",
  "MLCC_ENTRY_BASENAME" => "http_keepalive_smoke",
  "TMPDIR" => ENV.fetch("TMPDIR", File.join(root, "tmp"))
}
system(env, File.join(root, "compiler/build_bin.sh"), out_dir, binary) ||
  abort("[http 413] FAIL: link")

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
    abort("[http 413] FAIL: server exited early")
  end
  sleep 0.05
end
abort("[http 413] FAIL: no port file") if port.nil? || port.empty?

socket = TCPSocket.new("127.0.0.1", Integer(port))
socket.write("POST /upload HTTP/1.1\r\nHost: localhost\r\nContent-Length: 1048577\r\nConnection: close\r\n\r\n")
socket.flush
response = socket.read
socket.close
abort("[http 413] FAIL: empty response") if response.nil? || response.empty?
unless response.start_with?("HTTP/1.1 413")
  abort("[http 413] FAIL: expected 413, got #{response.lines.first.inspect}")
end
unless response.include?("Payload Too Large")
  abort("[http 413] FAIL: missing Payload Too Large body")
end

# Server may exit non-zero (served < 2); that is OK for this probe.
Process.wait(server_pid) rescue nil
warn "[http 413] ok (Content-Length 1048577 → 413)"
