# frozen_string_literal: true

require_relative "../test_helper"
require_relative "../../lib/mlc/common/index"

class MLCIntegerOverflowCodegenTest < Minitest::Test
  def test_signed_add_uses_checked_add
    cpp = MLC.compile("fn add(a: i32, b: i32) -> i32 = a + b").to_source
    assert_includes cpp, "mlc::arith::checked_add(a, b)"
    assert_includes cpp, "mlc/core/arith.hpp"
    refute_match(/return a \+ b/, cpp)
  end

  def test_signed_sub_mul_use_helpers
    cpp = MLC.compile(<<~MLC).to_source
      fn sub(a: i64, b: i64) -> i64 = a - b
      fn mul(a: i8, b: i8) -> i8 = a * b
    MLC
    assert_includes cpp, "mlc::arith::checked_sub(a, b)"
    assert_includes cpp, "mlc::arith::checked_mul(a, b)"
  end

  def test_unsigned_add_wraps_raw
    cpp = MLC.compile("fn add(a: u32, b: u32) -> u32 = a + b").to_source
    assert_includes cpp, "a + b"
    refute_includes cpp, "checked_add"
  end

  def test_integer_div_mod_use_helpers
    cpp = MLC.compile(<<~MLC).to_source
      fn div(a: i32, b: i32) -> i32 = a / b
      fn mod(a: u32, b: u32) -> u32 = a % b
    MLC
    assert_includes cpp, "mlc::arith::checked_div(a, b)"
    assert_includes cpp, "mlc::arith::checked_mod(a, b)"
  end

  def test_float_add_unchanged
    cpp = MLC.compile("fn add(a: f64, b: f64) -> f64 = a + b").to_source
    assert_includes cpp, "a + b"
    refute_includes cpp, "checked_add"
  end
end
