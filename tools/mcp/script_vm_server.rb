#!/usr/bin/env ruby
require "json"
require "timeout"

repository_root = File.expand_path("../..", __dir__)
GUEST_BINARY = ENV["SCRIPT_VM_GUEST"] || File.join(repository_root, "compiler", "out", "script_vm_guest")
SOURCE_BYTE_LIMIT = 1_048_576
GUEST_TIMEOUT_SECONDS = 10

TOOLS = [
  {
    "name" => "run_guest",
    "description" => "Run one trusted MLC Script VM guest. Entry is export fn run() -> i32 or export fn run() -> bool with no parameters. Host control literals: set_label_text(\"text\"), set_effect_mode(0..3) where 0 is Off, 1 Lens, 2 Cyber, 3 Fractal, reload_listing(), log_host(\"text\"), timer_after(milliseconds). Control calls are queued for the open gallery. timer_after is answered immediately here. Instruction limit 100000. At most 32 host resumes. Result lines: outcome=, value=, instructions=, diagnostic=, error=, host=.",
    "inputSchema" => {
      "type" => "object",
      "properties" => {
        "source" => {
          "type" => "string",
          "description" => "Guest source. Entry is export fn run() -> i32 or bool."
        }
      },
      "required" => ["source"],
      "additionalProperties" => false
    }
  }
].freeze

def send_message(message)
  $stdout.write(JSON.generate(message) + "\n")
  $stdout.flush
end

def run_guest(source)
  text = source.to_s
  if text.bytesize > SOURCE_BYTE_LIMIT
    return "outcome=source_too_large\nlimit=#{SOURCE_BYTE_LIMIT}"
  end
  unless File.executable?(GUEST_BINARY)
    return "outcome=runner_missing\npath=#{GUEST_BINARY}"
  end

  output_reader, output_writer = IO.pipe
  error_reader, error_writer = IO.pipe
  input_reader, input_writer = IO.pipe
  child_environment = ENV.to_h
  child_environment["SCRIPT_VM_HOST_QUEUE"] = File.join(repository_root, "tmp", "script_vm_host_queue")
  child_process_identifier = Process.spawn(
    child_environment,
    GUEST_BINARY,
    in: input_reader,
    out: output_writer,
    err: error_writer
  )
  input_reader.close
  output_writer.close
  error_writer.close
  input_writer.write(text)
  input_writer.close
  output_text = +""
  error_text = +""
  output_thread = Thread.new { output_text.replace(output_reader.read.to_s) }
  error_thread = Thread.new { error_text.replace(error_reader.read.to_s) }
  begin
    Timeout.timeout(GUEST_TIMEOUT_SECONDS) { Process.wait(child_process_identifier) }
  rescue Timeout::Error
    Process.kill("KILL", child_process_identifier)
    begin
      Process.wait(child_process_identifier)
    rescue Errno::ECHILD
      nil
    end
    output_thread.join
    error_thread.join
    output_reader.close
    error_reader.close
    return "outcome=timeout\nseconds=#{GUEST_TIMEOUT_SECONDS}"
  end
  output_thread.join
  error_thread.join
  output_reader.close
  error_reader.close
  combined = output_text
  combined += error_text unless error_text.empty?
  combined.empty? ? "outcome=empty_output" : combined
rescue StandardError => error
  "outcome=runner_failed\nerror=#{error.message}"
end

def call_tool(name, arguments)
  raise "unknown tool #{name}" unless name == "run_guest"

  run_guest(arguments["source"])
end

def handle_request(request)
  request_identifier = request["id"]
  method_name = request["method"]
  if method_name == "initialize"
    protocol_version = request.dig("params", "protocolVersion") || "2024-11-05"
    send_message(
      "jsonrpc" => "2.0",
      "id" => request_identifier,
      "result" => {
        "protocolVersion" => protocol_version,
        "capabilities" => { "tools" => {} },
        "serverInfo" => { "name" => "script-vm", "version" => "1.0.0" }
      }
    )
  elsif method_name == "notifications/initialized" || method_name == "notifications/cancelled"
    nil
  elsif method_name == "tools/list"
    send_message("jsonrpc" => "2.0", "id" => request_identifier, "result" => { "tools" => TOOLS })
  elsif method_name == "tools/call"
    parameters = request["params"] || {}
    begin
      text = call_tool(parameters["name"], parameters["arguments"] || {})
      send_message(
        "jsonrpc" => "2.0",
        "id" => request_identifier,
        "result" => { "content" => [{ "type" => "text", "text" => text }] }
      )
    rescue StandardError => error
      send_message(
        "jsonrpc" => "2.0",
        "id" => request_identifier,
        "result" => {
          "content" => [{ "type" => "text", "text" => "outcome=tool_failed\nerror=#{error.message}" }],
          "isError" => true
        }
      )
    end
  elsif !request_identifier.nil?
    send_message(
      "jsonrpc" => "2.0",
      "id" => request_identifier,
      "error" => { "code" => -32601, "message" => "Method not found: #{method_name}" }
    )
  end
end

def main
  $stdin.each_line do |line|
    stripped = line.strip
    next if stripped.empty?

    begin
      handle_request(JSON.parse(stripped))
    rescue JSON::ParserError
      nil
    end
  end
end

main if $PROGRAM_NAME == __FILE__
