# frozen_string_literal: true

require_relative "../test_helper"
require_relative "../../lib/mlc/common/index"

class MLCCryptoStdlibTest < Minitest::Test
  def test_crypto_sha256_hmac_codegen
    source = <<~MLC
      import Crypto::{sha256, hmac_sha256, random_bytes, pwhash, pwhash_verify, last_error}
      fn main() -> i32 = do
        let _digest = sha256("")
        let _mac = hmac_sha256("k", "m")
        let _rand = random_bytes(16)
        let _hash = pwhash("pw")
        let _ok = pwhash_verify("x", "pw")
        let _err = last_error()
        0
      end
    MLC
    cpp = MLC.compile(source).to_source
    assert_includes cpp, "mlc/crypto/sodium_bridge.hpp"
    assert_includes cpp, "mlc::crypto::sha256"
    assert_includes cpp, "mlc::crypto::hmac_sha256"
    assert_includes cpp, "mlc::crypto::random_bytes"
    assert_includes cpp, "mlc::crypto::pwhash"
    assert_includes cpp, "mlc::crypto::pwhash_verify"
    assert_includes cpp, "mlc::crypto::last_error"
  end
end
