# frozen_string_literal: true

require "test_helper"

class C4NumericTest < Minitest::Test
  def test_i64_type_annotation
    cpp = MLC.to_cpp("fn f(x: i64) -> i64 = x")
    assert_includes cpp, "int64_t"
  end

  def test_i64_literal_suffix
    cpp = MLC.to_cpp("fn f() -> i64 = 42i64")
    assert_includes cpp, "int64_t"
    assert_includes cpp, "42"
  end

  def test_u8_literal_suffix
    cpp = MLC.to_cpp("fn f() -> u8 = 255u8")
    assert_includes cpp, "uint8_t"
    assert_includes cpp, "255"
  end

  def test_usize_literal_suffix
    cpp = MLC.to_cpp("fn f() -> usize = 42usize")
    assert_includes cpp, "size_t"
    assert_includes cpp, "42"
  end

  def test_f64_type_annotation
    cpp = MLC.to_cpp("fn f(x: f64) -> f64 = x")
    assert_includes cpp, "double"
  end

  def test_f64_literal_suffix
    cpp = MLC.to_cpp("fn f() -> f64 = 3.14f64")
    assert_includes cpp, "double"
    assert_includes cpp, "3.14"
  end

  def test_float_literal_needs_suffix_for_f64
    cpp = MLC.to_cpp("fn f() -> f64 = 2.71f64")
    assert_includes cpp, "double"
    assert_includes cpp, "2.71"
  end

  def test_char_literal
    cpp = MLC.to_cpp("fn f() -> char = 'a'")
    assert_includes cpp, "char32_t"
    assert_includes cpp, "97"
  end

  def test_i64_arithmetic
    cpp = MLC.to_cpp("fn f(x: i64, y: i64) -> i64 = x + y")
    assert_includes cpp, "int64_t"
  end

  def test_hex_literal_with_suffix
    cpp = MLC.to_cpp("fn f() -> u8 = 0xFFu8")
    assert_includes cpp, "uint8_t"
    assert_includes cpp, "255"
  end

  def test_type_mismatch_i64_i32_raises
    assert_raises(MLC::CompileError) do
      MLC.to_cpp("fn f(x: i32) -> i64 = x")
    end
  end
end
