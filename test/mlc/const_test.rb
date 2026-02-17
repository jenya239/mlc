# frozen_string_literal: true

require "minitest/autorun"
require_relative "../../lib/mlc/common/index"

class ConstTest < Minitest::Test
  def test_const_keyword_parsed
    source = <<~MLC
      fn main() -> i32 = do
        const x = 42
        x
      end
    MLC
    cpp = MLC.to_cpp(source)
    assert_includes cpp, "constexpr"
  end

  def test_const_integer
    source = <<~MLC
      fn main() -> i32 = do
        const x = 42
        x
      end
    MLC
    cpp = MLC.to_cpp(source)
    assert_includes cpp, "constexpr int x = 42"
  end

  def test_const_float
    source = <<~MLC
      fn main() -> f32 = do
        const pi = 3.14159
        pi
      end
    MLC
    cpp = MLC.to_cpp(source)
    assert_includes cpp, "constexpr float pi = 3.14159"
  end

  def test_const_bool
    source = <<~MLC
      fn main() -> bool = do
        const flag = true
        flag
      end
    MLC
    cpp = MLC.to_cpp(source)
    assert_includes cpp, "constexpr bool flag = true"
  end

  def test_regular_let_no_constexpr
    source = <<~MLC
      fn main() -> i32 = do
        let x = 42
        x
      end
    MLC
    cpp = MLC.to_cpp(source)
    refute_includes cpp, "constexpr"
  end

  def test_mut_no_constexpr
    source = <<~MLC
      fn main() -> i32 = do
        let mut x = 42
        x
      end
    MLC
    cpp = MLC.to_cpp(source)
    refute_includes cpp, "constexpr"
  end

  def test_const_string_literal
    source = <<~MLC
      fn main() -> str = do
        const msg = "hello"
        msg
      end
    MLC
    cpp = MLC.to_cpp(source)
    assert_includes cpp, "constexpr"
    assert_includes cpp, "msg"
  end

  def test_const_arithmetic
    source = <<~MLC
      fn main() -> i32 = do
        const x = 10 + 20
        x
      end
    MLC
    cpp = MLC.to_cpp(source)
    assert_includes cpp, "constexpr"
  end
end
