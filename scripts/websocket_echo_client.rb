#!/usr/bin/env ruby
# frozen_string_literal: true
# Minimal RFC6455 client for websocket_echo_demo (TRACK_STDLIB_WEBSOCKET_TO_MLC STEP=5).

require "socket"

host = ARGV[0] || "127.0.0.1"
port = Integer(ARGV[1])
payload = ARGV[2] || "hello-ws"

socket = TCPSocket.new(host, port)
key = "dGhlIHNhbXBsZSBub25jZQ=="
socket.write(
  "GET /chat HTTP/1.1\r\n" \
  "Host: #{host}\r\n" \
  "Upgrade: websocket\r\n" \
  "Connection: Upgrade\r\n" \
  "Sec-WebSocket-Key: #{key}\r\n" \
  "Sec-WebSocket-Version: 13\r\n" \
  "\r\n"
)

response = +""
deadline = Time.now + 5
while Time.now < deadline && !response.include?("\r\n\r\n")
  chunk = socket.read_nonblock(512, exception: false)
  if chunk == :wait_readable
    IO.select([socket], nil, nil, 0.05)
    next
  end
  break if chunk.nil? || chunk.empty?

  response << chunk
end

abort "upgrade failed: #{response.inspect}" unless response.include?("101")
abort "bad accept" unless response.include?("s3pPLMBiTxaQ9kYGzzhZRbK+xOo=")

mask = [0x12, 0x34, 0x56, 0x78].pack("C*")
frame = +""
frame << 0x81.chr
frame << (0x80 | payload.bytesize).chr
frame << mask
payload.bytes.each_with_index do |byte, index|
  frame << (byte ^ mask.getbyte(index % 4)).chr
end
socket.write(frame)

echo = socket.readpartial(256)
abort "short echo" if echo.bytesize < 2
second = echo.getbyte(1)
abort "masked server frame" unless (second & 0x80).zero?
length = second & 0x7f
body = echo.byteslice(2, length)
abort "echo mismatch: #{body.inspect}" unless body == payload
socket.close
puts "websocket client ok"
