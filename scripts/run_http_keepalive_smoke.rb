#!/usr/bin/env ruby
# frozen_string_literal: true

# TRACK_STDLIB_HTTP_HARDENING STEP=1 — two GETs on one persistent connection.
require "net/http"
require "uri"
require "fileutils"

root = File.expand_path("..", __dir__)
mlcc = ENV.fetch("MLCC", File.join(root, "compiler/out/mlcc"))
out_dir = ENV.fetch(
  "HTTP_KEEPALIVE_SMOKE_OUT",
  File.join(ENV.fetch("TMPDIR", File.join(root, "tmp")), "http_keepalive_smoke")
)
entry = File.join(root, "misc/examples/http_keepalive_smoke.mlc")
port_file = File.join(out_dir, "http_keepalive_port.txt")
binary = File.join(out_dir, "http_keepalive_smoke")

abort("[http keepalive] FAIL: mlcc missing at #{mlcc}") unless File.executable?(mlcc)

FileUtils.rm_rf(out_dir)
FileUtils.mkdir_p(out_dir)

system(mlcc, "-o", out_dir, entry) || abort("[http keepalive] FAIL: mlcc codegen")
env = {
  "MLCC_PCH" => "0",
  "MLCC_DEV" => "1",
  "MLCC_ENTRY_BASENAME" => "http_keepalive_smoke",
  "TMPDIR" => ENV.fetch("TMPDIR", File.join(root, "tmp"))
}
system(env, File.join(root, "compiler/build_bin.sh"), out_dir, binary) ||
  abort("[http keepalive] FAIL: link")

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
    abort("[http keepalive] FAIL: server exited early")
  end
  sleep 0.05
end
abort("[http keepalive] FAIL: no port file") if port.nil? || port.empty?

uri = URI("http://127.0.0.1:#{port}/")
Net::HTTP.start(uri.host, uri.port) do |http|
  http.keep_alive_timeout = 10
  first = http.get("/one")
  second = http.get("/two")
  unless first.code == "200" && first.body == "/one"
    abort("[http keepalive] FAIL: first response #{first.code} #{first.body.inspect}")
  end
  unless second.code == "200" && second.body == "/two"
    abort("[http keepalive] FAIL: second response #{second.code} #{second.body.inspect}")
  end
  connection = first["connection"].to_s.downcase
  unless connection.include?("keep-alive")
    abort("[http keepalive] FAIL: first Connection=#{connection.inspect}")
  end
end

status = Process.wait2(server_pid)[1].exitstatus
at_exit { true } # already waited
abort("[http keepalive] FAIL: server exit=#{status}") unless status == 0
warn "[http keepalive] ok (2 GETs on one connection)"
