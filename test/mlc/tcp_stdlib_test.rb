# frozen_string_literal: true

require_relative "../test_helper"
require_relative "../../lib/mlc/common/index"

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
end
