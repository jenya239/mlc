# frozen_string_literal: true

require_relative "../test_helper"
require_relative "../../lib/mlc/common/index"

class MLCStringInterpolationTest < Minitest::Test
  def test_plain_backtick_string
    cpp = MLC.to_cpp(<<~MLC)
      fn f() -> string = `hello`
    MLC
    assert_includes cpp, '"hello"'
  end

  def test_single_interpolation
    cpp = MLC.to_cpp(<<~MLC)
      fn f(x: i32) -> string = `${x}`
    MLC
    assert_match(/to_string/, cpp)
  end

  def test_interpolation_with_text
    cpp = MLC.to_cpp(<<~MLC)
      fn f(x: i32) -> string = `a ${x} b`
    MLC
    assert_includes cpp, '"a "'
    assert_includes cpp, '" b"'
    assert_match(/to_string/, cpp)
  end

  def test_multiple_interpolations
    cpp = MLC.to_cpp(<<~MLC)
      fn f(x: i32, y: i32) -> string = `x=${x} y=${y}`
    MLC
    assert_match(/to_string.*to_string/m, cpp)
  end

  def test_escaped_backtick
    cpp = MLC.to_cpp(<<~'MLC')
      fn f() -> string = `a\`b`
    MLC
    assert_match(/a`b/, cpp)
  end

  def test_escaped_dollar_brace
    cpp = MLC.to_cpp(<<~'MLC')
      fn f() -> string = `\${not interp}`
    MLC
    assert_match(/\$\{not interp\}/, cpp)
  end

  def test_expression_arithmetic
    cpp = MLC.to_cpp(<<~MLC)
      fn f(x: i32) -> string = `${x + 1}`
    MLC
    assert_match(/to_string/, cpp)
    assert_match(/\+\s*1|1\s*\+/, cpp)
  end

  def test_nested_braces_in_interpolation
    cpp = MLC.to_cpp(<<~MLC)
      type Pair = Pair { a: i32, b: i32 }
      fn f(p: Pair) -> string = `${p.a}`
    MLC
    assert_match(/to_string/, cpp)
  end

  def test_multiline_template
    cpp = MLC.to_cpp(<<~MLC)
      fn f() -> string = `line1
line2`
    MLC
    assert_match(/line1/, cpp)
    assert_match(/line2/, cpp)
  end

  def test_empty_template
    cpp = MLC.to_cpp(<<~MLC)
      fn f() -> string = ``
    MLC
    assert_includes cpp, '""'
  end

  def test_unterminated_template_raises
    err = assert_raises(MLC::ParseError) do
      MLC.to_cpp(<<~MLC)
        fn f() -> string = `unterminated
      MLC
    end
    assert_match(/unterminated template literal/i, err.message)
  end

  def test_interpolation_with_string_arg
    cpp = MLC.to_cpp(<<~MLC)
      fn f(name: string) -> string = `Hello, ${name}!`
    MLC
    assert_includes cpp, '"Hello, "'
    assert_includes cpp, '"!"'
    assert_includes cpp, 'name'
  end
end
