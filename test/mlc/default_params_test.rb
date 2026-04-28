# frozen_string_literal: true

require_relative "../test_helper"
require_relative "../../lib/mlc/common/index"

class MLCDefaultParamsTest < Minitest::Test
  def test_call_uses_default_and_generates_cpp
    src = <<~MLC
      fn g(a: i32, b: i32 = 0) -> i32 = a + b
      fn main() -> i32 = g(1)
    MLC

    cpp = MLC.to_cpp(src)
    assert_includes cpp, "g(1)"
    assert_match(/=\s*0/, cpp)
  end

  def test_explicit_second_arg
    src = <<~MLC
      fn g(a: i32, b: i32 = 0) -> i32 = a + b
      fn main() -> i32 = g(1, 2)
    MLC

    cpp = MLC.to_cpp(src)
    assert_includes cpp, "g(1, 2)"
  end

  def test_parse_error_required_after_default
    err = assert_raises(MLC::ParseError) do
      MLC.to_cpp(<<~MLC)
        fn f(a: i32 = 0, b: i32) -> i32 = 1
        fn main() -> i32 = 0
      MLC
    end
    assert_match(/required parameter|default value/i, err.message)
  end

  def test_extern_cannot_have_default
    err = assert_raises(MLC::CompileError) do
      MLC.to_cpp(<<~MLC)
        extern fn f(x: i32 = 1) -> i32
        fn main() -> i32 = 0
      MLC
    end
    assert_match(/extern function cannot have default/i, err.message)
  end

  def test_generic_cannot_have_default
    err = assert_raises(MLC::CompileError) do
      MLC.to_cpp(<<~MLC)
        fn id<T>(x: T, y: i32 = 0) -> T = x
        fn main() -> i32 = 0
      MLC
    end
    assert_match(/generic functions cannot have default/i, err.message)
  end

  def test_too_few_arguments_rejected
    err = assert_raises(MLC::CompileError) do
      MLC.to_cpp(<<~MLC)
        fn g(a: i32, b: i32) -> i32 = a + b
        fn main() -> i32 = g(1)
      MLC
    end
    assert_match(/between 2 and 2|expects between/i, err.message)
  end
end
