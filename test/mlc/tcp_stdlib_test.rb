# frozen_string_literal: true

require_relative "../test_helper"
require_relative "../../lib/mlc/common/index"
require "fileutils"
require "open3"
require "tmpdir"

class MLCTcpStdlibTest < Minitest::Test
  def test_tcp_bind_accept_read_write_codegen
    source = <<~MLC
      import Tcp::{bind, accept, read, write_all, close_listener, close_stream, port, last_error}
      fn main() -> i32 = do
        let listener = bind("127.0.0.1", 0)
        let _port = port(0)
        let _err = last_error()
        let _stream = accept(0)
        let _data = read(0, 64)
        let _ok = write_all(0, "x")
        close_listener(0)
        close_stream(0)
        0
      end
    MLC
    cpp = MLC.compile(source).to_source
    assert_includes cpp, "mlc/net/tcp.hpp"
    assert_includes cpp, "mlc::net::bind"
    assert_includes cpp, "mlc::net::accept"
    assert_includes cpp, "mlc::net::read"
    assert_includes cpp, "mlc::net::write_all"
    assert_includes cpp, "mlc::net::close_listener"
    assert_includes cpp, "mlc::net::close_stream"
    assert_includes cpp, "mlc::net::port"
    assert_includes cpp, "mlc::net::last_error"
  end

  def test_echo_server_roundtrip_via_client
    Dir.mktmpdir do |dir|
      src_dir = File.join(dir, "src")
      out_dir = File.join(dir, "out")
      port_file = File.join(dir, "port.txt")
      FileUtils.mkdir_p(src_dir)
      File.write(File.join(src_dir, "main.mlc"), <<~MLC)
        import Tcp::{bind, accept, read, write_all, close_listener, close_stream, port, last_error}
        import Conv::{to_string_i32}
        import File::{write_string}
        import IO::{eprintln}

        fn unwrap_handle(value: Option<i32>) -> i32 =
          match value { Some(handle) => handle, None => -1 }

        fn unwrap_text(value: Option<string>) -> string =
          match value { Some(text) => text, None => "" }

        fn main() -> i32 = do
          let listener = unwrap_handle(bind("127.0.0.1", 0))
          if listener < 0 then
            eprintln(last_error())
            return 1
          end
          let _wrote_port = write_string("#{port_file}", to_string_i32(port(listener)))
          let stream = unwrap_handle(accept(listener))
          if stream < 0 then
            eprintln(last_error())
            close_listener(listener)
            return 2
          end
          let data = unwrap_text(read(stream, 64))
          let wrote = write_all(stream, data)
          close_stream(stream)
          close_listener(listener)
          if wrote then 0 else 3 end
        end
      MLC

      result = MLC.build_project(
        entry_path: File.join(src_dir, "main.mlc"),
        out_dir: out_dir,
        root_dir: src_dir,
        binary_name: "tcp_echo"
      )

      client_source = File.expand_path("../../runtime/test/tcp_echo_client.cpp", __dir__)
      client_binary = File.join(dir, "tcp_echo_client")
      compile_ok = system(
        "g++", "-std=c++20", "-O0",
        "-o", client_binary,
        client_source
      )
      assert compile_ok, "failed to compile tcp_echo_client"

      server_pid = spawn(
        result[:binary],
        out: File.join(dir, "server.out"),
        err: File.join(dir, "server.err")
      )

      begin
        port = nil
        deadline = Time.now + 5
        while Time.now < deadline
          if File.file?(port_file) && !File.read(port_file).strip.empty?
            port = Integer(File.read(port_file).strip)
            break
          end
          sleep 0.01
        end
        assert port, "server did not write port; err=#{File.read(File.join(dir, 'server.err')) rescue ''}"

        client_status = system(client_binary, port.to_s)
        assert client_status, "echo client failed against port #{port}"

        Process.wait(server_pid)
        assert_equal 0, $?.exitstatus, File.read(File.join(dir, "server.err"))
      ensure
        begin
          Process.kill("TERM", server_pid) if server_pid
        rescue Errno::ESRCH
        end
      end
    end
  end
end
