# frozen_string_literal: true

require "minitest/autorun"
require_relative "../../lib/mlc/common/index"

class ConstTest < Minitest::Test
  # let const → compile-time constexpr

  def test_let_const_integer
    source = <<~MLC
      fn main() -> i32 = do
        let const x = 42
        x
      end
    MLC
    cpp = MLC.to_cpp(source)
    assert_includes cpp, "constexpr int x = 42"
  end

  def test_let_const_float
    source = <<~MLC
      fn main() -> f32 = do
        let const pi = 3.14159
        pi
      end
    MLC
    cpp = MLC.to_cpp(source)
    assert_includes cpp, "constexpr float pi = 3.14159"
  end

  def test_let_const_bool
    source = <<~MLC
      fn main() -> bool = do
        let const flag = true
        flag
      end
    MLC
    cpp = MLC.to_cpp(source)
    assert_includes cpp, "constexpr bool flag = true"
  end

  def test_let_const_arithmetic
    source = <<~MLC
      fn main() -> i32 = do
        let const x = 10 + 20
        x
      end
    MLC
    cpp = MLC.to_cpp(source)
    assert_includes cpp, "constexpr"
  end

  # const → non-rebindable, but NOT constexpr

  def test_const_no_constexpr
    source = <<~MLC
      fn main() -> i32 = do
        const x = 42
        x
      end
    MLC
    cpp = MLC.to_cpp(source)
    refute_includes cpp, "constexpr"
    assert_includes cpp, "int x = 42"
  end

  def test_const_cannot_rebind
    source = <<~MLC
      fn main() -> i32 = do
        const x = 1
        x = 2
        x
      end
    MLC
    error = assert_raises(MLC::CompileError) { MLC.to_cpp(source) }
    assert_match(/cannot rebind 'const x'/, error.message)
  end

  # let → rebindable, NOT constexpr

  def test_let_no_constexpr
    source = <<~MLC
      fn main() -> i32 = do
        let x = 42
        x
      end
    MLC
    cpp = MLC.to_cpp(source)
    refute_includes cpp, "constexpr"
  end

  def test_let_mut_can_rebind
    source = <<~MLC
      fn main() -> i32 = do
        let mut x = 1
        x = 2
        x
      end
    MLC
    cpp = MLC.to_cpp(source)
    assert_includes cpp, "x = 2"
  end
end
