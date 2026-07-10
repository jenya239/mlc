# frozen_string_literal: true

require_relative "../test_helper"
require_relative "../../lib/mlc/common/index"

class MLCIntArithStdlibTest < Minitest::Test
  def test_wrapping_saturating_checked_codegen
    source = <<~MLC
      import IntArith::{wrapping_add, saturating_add, checked_add}
      fn unwrap_or_zero(value: Option<i32>) -> i32 =
        match value { Some(v) => v, None => 0 }
      fn main() -> i32 = do
        let wrapped = wrapping_add(2147483647, 1)
        let saturated = saturating_add(2147483647, 1)
        unwrap_or_zero(checked_add(2, 3))
      end
    MLC
    cpp = MLC.compile(source).to_source
    assert_includes cpp, "mlc/core/int_arith.hpp"
    assert_includes cpp, "mlc::int_arith::wrapping_add"
    assert_includes cpp, "mlc::int_arith::saturating_add"
    assert_includes cpp, "mlc::int_arith::checked_add"
  end

  def test_all_nine_i32_ops_resolve
    source = <<~MLC
      import IntArith::{
        wrapping_add, wrapping_sub, wrapping_mul,
        saturating_add, saturating_sub, saturating_mul,
        checked_add, checked_sub, checked_mul
      }
      fn main() -> i32 = do
        let a = wrapping_add(1, 2)
        let b = wrapping_sub(3, 1)
        let c = wrapping_mul(2, 3)
        let d = saturating_add(1, 2)
        let e = saturating_sub(3, 1)
        let f = saturating_mul(2, 3)
        let _g = checked_add(1, 2)
        let _h = checked_sub(3, 1)
        let _i = checked_mul(2, 3)
        a + b + c + d + e + f
      end
    MLC
    cpp = MLC.compile(source).to_source
    %w[wrapping_add wrapping_sub wrapping_mul
       saturating_add saturating_sub saturating_mul
       checked_add checked_sub checked_mul].each do |name|
      assert_includes cpp, "mlc::int_arith::#{name}"
    end
  end
end
