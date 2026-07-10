# frozen_string_literal: true

require_relative "../test_helper"
require_relative "../../lib/mlc/common/index"

class MLCEnvLogStdlibTest < Minitest::Test
  def test_env_log_extern_codegen
    source = <<~MLC
      import Env::{get, get_or, has}
      import Log::{error, warn, info, debug}
      fn main() -> i32 = do
        let _present = has("PATH")
        let _value = get("PATH")
        let _or = get_or("MLC_MISSING_ENV_KEY", "fallback")
        info("hello")
        warn("w")
        error("e")
        debug("d")
        0
      end
    MLC
    cpp = MLC.compile(source).to_source
    assert_includes cpp, "mlc/env/env.hpp"
    assert_includes cpp, "mlc/log/log.hpp"
    assert_includes cpp, "mlc::env::get"
    assert_includes cpp, "mlc::env::get_or"
    assert_includes cpp, "mlc::env::has"
    assert_includes cpp, "mlc::log::info"
    assert_includes cpp, "mlc::log::warn"
    assert_includes cpp, "mlc::log::error"
    assert_includes cpp, "mlc::log::debug"
  end
end
