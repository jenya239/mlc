#!/usr/bin/env ruby
# frozen_string_literal: true

# Local HTTPS fixture for the curl ABI smoke.
# Invariants:
# - certificate_authority_path is a private CA, not the system trust store
# - good_port presents SAN IP:127.0.0.1
# - wrong_port presents SAN DNS:wrong.test and is signed by the same CA
# - connection_count increments on TCP accept, before the handshake
# - request_count increments only after a full HTTP header block is read
# - /silent accepts TLS and then does not read or write
# - /retry and /retry-single each return 503 with Retry-After: 0 once, then 200
# - /once returns 404 once, then 200
# - /redirect/same, /redirect/cross, /redirect/http, /redirect/loop, /redirect/keep
#   answer with Location. /landed reports the method and whether Authorization
#   or Cookie arrived. It does not copy those header values.
# - /session_echo answers 200 with Connection: keep-alive and the request body,
#   then reads another request on the same TLS connection, up to 8 times.

require 'openssl'
require 'socket'
require 'thread'

directory = ARGV[0]
abort 'usage: https_test_server.rb DIRECTORY' if directory.nil? || directory.empty?

Dir.mkdir(directory) unless Dir.exist?(directory)

module HttpsTestServer
  module_function

  def generate_authority
    key = OpenSSL::PKey::RSA.new(2048)
    name = OpenSSL::X509::Name.parse('/CN=mlc-https-test-authority')
    certificate = OpenSSL::X509::Certificate.new
    certificate.version = 2
    certificate.serial = 1
    certificate.subject = name
    certificate.issuer = name
    certificate.public_key = key.public_key
    certificate.not_before = Time.now - 3600
    certificate.not_after = Time.now + 3600
    factory = OpenSSL::X509::ExtensionFactory.new
    factory.subject_certificate = certificate
    factory.issuer_certificate = certificate
    certificate.add_extension(factory.create_extension('basicConstraints', 'CA:TRUE', true))
    certificate.add_extension(factory.create_extension('keyUsage', 'keyCertSign, cRLSign', true))
    certificate.add_extension(factory.create_extension('subjectKeyIdentifier', 'hash', false))
    certificate.sign(key, OpenSSL::Digest::SHA256.new)
    [key, certificate]
  end

  def generate_leaf(authority_key, authority_certificate, subject_alternative_name, serial)
    key = OpenSSL::PKey::RSA.new(2048)
    certificate = OpenSSL::X509::Certificate.new
    certificate.version = 2
    certificate.serial = serial
    certificate.subject = OpenSSL::X509::Name.parse('/CN=mlc-https-test-leaf')
    certificate.issuer = authority_certificate.subject
    certificate.public_key = key.public_key
    certificate.not_before = Time.now - 3600
    certificate.not_after = Time.now + 3600
    factory = OpenSSL::X509::ExtensionFactory.new
    factory.subject_certificate = certificate
    factory.issuer_certificate = authority_certificate
    certificate.add_extension(factory.create_extension('basicConstraints', 'CA:FALSE', true))
    certificate.add_extension(factory.create_extension('keyUsage', 'digitalSignature, keyEncipherment', true))
    certificate.add_extension(factory.create_extension('extendedKeyUsage', 'serverAuth', false))
    certificate.add_extension(factory.create_extension('subjectAltName', subject_alternative_name, false))
    certificate.add_extension(factory.create_extension('authorityKeyIdentifier', 'keyid:always', false))
    certificate.sign(authority_key, OpenSSL::Digest::SHA256.new)
    [key, certificate]
  end

  def context_for(certificate, key)
    context = OpenSSL::SSL::SSLContext.new
    context.cert = certificate
    context.key = key
    context.min_version = OpenSSL::SSL::TLS1_2_VERSION
    context.verify_mode = OpenSSL::SSL::VERIFY_NONE
    context
  end
end

authority_key, authority_certificate = HttpsTestServer.generate_authority
good_key, good_certificate = HttpsTestServer.generate_leaf(
  authority_key, authority_certificate, 'IP:127.0.0.1', 2
)
wrong_key, wrong_certificate = HttpsTestServer.generate_leaf(
  authority_key, authority_certificate, 'DNS:wrong.test', 3
)

certificate_authority_path = File.join(directory, 'certificate_authority.pem')
File.write(certificate_authority_path, authority_certificate.to_pem)
File.write(File.join(directory, 'certificate_authority_path'), certificate_authority_path)

connection_count_path = File.join(directory, 'connection_count')
request_count_path = File.join(directory, 'request_count')
File.write(connection_count_path, '0')
File.write(request_count_path, '0')

counter_mutex = Mutex.new
connection_count = 0
request_count = 0
retry_budget = { '/retry' => 1, '/retry-single' => 1, '/once' => 1 }

increment_connections = lambda do
  counter_mutex.synchronize do
    connection_count += 1
    File.write(connection_count_path, connection_count.to_s)
  end
end

increment_requests = lambda do
  counter_mutex.synchronize do
    request_count += 1
    File.write(request_count_path, request_count.to_s)
  end
end

read_http_request = lambda do |secure_socket|
  collected = +''
  until collected.include?("\r\n\r\n")
    piece = secure_socket.readpartial(4096)
    collected << piece
    break if collected.bytesize > 1_048_576
  end
  header_text, remainder = collected.split("\r\n\r\n", 2)
  remainder = '' if remainder.nil?
  content_length = 0
  header_text.each_line do |line|
    next unless line.downcase.start_with?('content-length:')
    content_length = line.split(':', 2)[1].to_i
  end
  while remainder.bytesize < content_length
    remainder << secure_socket.read(content_length - remainder.bytesize)
  end
  body = remainder.byteslice(0, content_length)
  request_line = header_text.lines.first.to_s
  request_parts = request_line.split(' ')
  method = request_parts[0].to_s
  path = request_parts[1].to_s
  authorization = header_text.lines.any? { |line| line.downcase.start_with?('authorization:') }
  cookie = header_text.lines.any? { |line| line.downcase.start_with?('cookie:') }
  [method, path, body, authorization, cookie]
end

write_bytes = lambda do |secure_socket, bytes|
  written = 0
  while written < bytes.bytesize
    count = secure_socket.write(bytes.byteslice(written, bytes.bytesize - written))
    break if count.nil? || count <= 0
    written += count
  end
end

respond_redirect = lambda do |secure_socket, status_line, location, body|
  header = "#{status_line}\r\nLocation: #{location}\r\nContent-Type: text/plain\r\nContent-Length: #{body.bytesize}\r\nConnection: close\r\n\r\n"
  write_bytes.call(secure_socket, header)
  write_bytes.call(secure_socket, body)
end

respond_fixed = lambda do |secure_socket, status_line, body, content_type|
  header = "#{status_line}\r\nContent-Type: #{content_type}\r\nContent-Length: #{body.bytesize}\r\nConnection: close\r\n\r\n"
  write_bytes.call(secure_socket, header)
  write_bytes.call(secure_socket, body)
end

respond_chunked = lambda do |secure_socket, body|
  write_bytes.call(
    secure_socket,
    "HTTP/1.1 200 OK\r\nContent-Type: application/octet-stream\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"
  )
  offset = 0
  while offset < body.bytesize
    slice = body.byteslice(offset, 16_384)
    write_bytes.call(secure_socket, format("%x\r\n", slice.bytesize))
    write_bytes.call(secure_socket, slice)
    write_bytes.call(secure_socket, "\r\n")
    offset += slice.bytesize
  end
  write_bytes.call(secure_socket, "0\r\n\r\n")
end

binary_body = "a\0b\0c".b
big_body = ('B' * (2 * 1024 * 1024)).b
cross_port = 0

serve_socket = lambda do |tcp_socket, context, silent|
  increment_connections.call
  secure_socket = OpenSSL::SSL::SSLSocket.new(tcp_socket, context)
  secure_socket.sync_close = true
  secure_socket.sync = true
  begin
    secure_socket.accept
    if silent
      sleep 10
      return
    end
    keep_reading = true
    session_reads = 0
    while keep_reading
    method, path, body, authorization, cookie = read_http_request.call(secure_socket)
    increment_requests.call
    if path == '/session_echo'
      session_reads += 1
      write_bytes.call(
        secure_socket,
        "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: #{body.bytesize}\r\nConnection: keep-alive\r\n\r\n"
      )
      write_bytes.call(secure_socket, body)
      keep_reading = session_reads < 8
      next
    end
    keep_reading = false
    case path
    when '/echo'
      respond_fixed.call(secure_socket, 'HTTP/1.1 200 OK', body, 'text/plain')
    when '/status/404'
      respond_fixed.call(secure_socket, 'HTTP/1.1 404 Not Found', 'missing', 'text/plain')
    when '/big'
      respond_fixed.call(secure_socket, 'HTTP/1.1 200 OK', big_body, 'application/octet-stream')
    when '/chunked_big'
      respond_chunked.call(secure_socket, big_body)
    when '/binary'
      respond_fixed.call(secure_socket, 'HTTP/1.1 200 OK', binary_body, 'application/octet-stream')
    when '/retry', '/retry-single'
      waiting = counter_mutex.synchronize do
        if retry_budget[path].to_i > 0
          retry_budget[path] -= 1
          true
        else
          false
        end
      end
      if waiting
        write_bytes.call(
          secure_socket,
          "HTTP/1.1 503 Service Unavailable\r\nContent-Type: text/plain\r\nContent-Length: 4\r\nRetry-After: 0\r\nConnection: close\r\n\r\nwait"
        )
      else
        respond_fixed.call(secure_socket, 'HTTP/1.1 200 OK', 'ready', 'text/plain')
      end
    when '/landed'
      auth_flag = authorization ? 'yes' : 'no'
      cookie_flag = cookie ? 'yes' : 'no'
      respond_fixed.call(
        secure_socket,
        'HTTP/1.1 200 OK',
        "method=#{method} auth=#{auth_flag} cookie=#{cookie_flag}",
        'text/plain'
      )
    when '/redirect/same'
      respond_redirect.call(secure_socket, 'HTTP/1.1 302 Found', '/landed', '')
    when '/redirect/cross'
      respond_redirect.call(
        secure_socket,
        'HTTP/1.1 302 Found',
        "https://127.0.0.1:#{cross_port}/landed",
        ''
      )
    when '/redirect/http'
      respond_redirect.call(secure_socket, 'HTTP/1.1 302 Found', 'http://127.0.0.1/landed', 'stop')
    when '/redirect/loop'
      respond_redirect.call(secure_socket, 'HTTP/1.1 302 Found', '/redirect/loop', '')
    when '/redirect/keep'
      respond_redirect.call(secure_socket, 'HTTP/1.1 307 Temporary Redirect', '/echo', '')
    when '/once'
      first = counter_mutex.synchronize do
        if retry_budget[path].to_i > 0
          retry_budget[path] -= 1
          true
        else
          false
        end
      end
      if first
        respond_fixed.call(secure_socket, 'HTTP/1.1 404 Not Found', 'missing', 'text/plain')
      else
        respond_fixed.call(secure_socket, 'HTTP/1.1 200 OK', 'retried-once', 'text/plain')
      end
    else
      respond_fixed.call(secure_socket, 'HTTP/1.1 404 Not Found', 'unknown', 'text/plain')
    end
    end
  rescue StandardError
    nil
  ensure
    secure_socket.close rescue nil
    tcp_socket.close rescue nil
  end
end

accept_loop = lambda do |tcp_server, context, silent|
  loop do
    begin
      client_socket = tcp_server.accept
    rescue StandardError
      break
    end
    Thread.new(client_socket) do |socket|
      serve_socket.call(socket, context, silent)
    end
  end
end

good_server = TCPServer.new('127.0.0.1', 0)
wrong_server = TCPServer.new('127.0.0.1', 0)
cross_server = TCPServer.new('127.0.0.1', 0)
cross_port = cross_server.addr[1]
good_server.setsockopt(Socket::IPPROTO_TCP, Socket::TCP_NODELAY, 1) rescue nil
wrong_server.setsockopt(Socket::IPPROTO_TCP, Socket::TCP_NODELAY, 1) rescue nil
cross_server.setsockopt(Socket::IPPROTO_TCP, Socket::TCP_NODELAY, 1) rescue nil

File.write(File.join(directory, 'good_port'), good_server.addr[1].to_s)
File.write(File.join(directory, 'wrong_port'), wrong_server.addr[1].to_s)
File.write(File.join(directory, 'ready'), '1')

trap('TERM') { exit! 0 }
trap('INT') { exit! 0 }

good_context = HttpsTestServer.context_for(good_certificate, good_key)
wrong_context = HttpsTestServer.context_for(wrong_certificate, wrong_key)

Thread.new { accept_loop.call(good_server, good_context, false) }
Thread.new { accept_loop.call(wrong_server, wrong_context, false) }
Thread.new { accept_loop.call(cross_server, good_context, false) }

# /silent is a third listener so a stalled handshake cannot block /echo.
silent_server = TCPServer.new('127.0.0.1', 0)
File.write(File.join(directory, 'silent_port'), silent_server.addr[1].to_s)
silent_context = good_context
Thread.new { accept_loop.call(silent_server, silent_context, true) }

sleep
