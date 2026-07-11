# frozen_string_literal: true

require_relative "../test_helper"
require_relative "../../lib/mlc/common/index"

class MLCValidateStdlibTest < Minitest::Test
  def test_validate_extern_codegen
    source = <<~MLC
      import Validate::{non_empty, min_length, max_length, range_i32}
      fn main() -> i32 = do
        let _a = non_empty("x")
        let _b = min_length("ab", 2)
        let _c = max_length("ab", 2)
        let _d = range_i32(5, 1, 10)
        0
      end
    MLC
    cpp = MLC.compile(source).to_source
    refute_includes cpp, "mlc/validate/validate.hpp"
    # Ruby still maps Validate.* → mlc::validate::* (no body emit); logic is mlcc MLC.
    assert_includes cpp, "mlc::validate::non_empty"
    assert_includes cpp, "mlc::validate::min_length"
    assert_includes cpp, "mlc::validate::max_length"
    assert_includes cpp, "mlc::validate::range_i32"
  end
end
