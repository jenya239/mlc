# frozen_string_literal: true

require_relative "../test_helper"
require_relative "../../lib/mlc/common/index"

class MLCWebsocketStdlibTest < Minitest::Test
  # Ruby MLC.compile still emits mlc::websocket::* calls (registry :extern),
  # like Crypto→sodium_bridge. Real protocol bodies are MLC (mlcc path).
  def test_websocket_bridge_codegen
    source = <<~MLC
      import WebSocket::{upgrade, read_text, write_text, close, last_error}
      fn main() -> i32 = do
        let _connection = upgrade(1)
        let _text = read_text(1)
        let _wrote = write_text(1, "hi")
        close(1)
        let _err = last_error()
        0
      end
    MLC
    cpp = MLC.compile(source).to_source
    assert_includes cpp, "mlc/net/websocket_bridge.hpp"
    assert_includes cpp, "mlc::websocket::upgrade"
    assert_includes cpp, "mlc::websocket::read_text"
    assert_includes cpp, "mlc::websocket::write_text"
    assert_includes cpp, "mlc::websocket::close"
    assert_includes cpp, "mlc::websocket::last_error"
    refute_includes cpp, "mlc/net/websocket.hpp"
    refute_includes cpp, "upgrade_mlc"
  end
end
