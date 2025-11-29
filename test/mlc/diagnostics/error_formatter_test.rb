# frozen_string_literal: true

require_relative "../../test_helper"
require_relative "../../../lib/mlc/common/diagnostics/error_formatter"

class ErrorFormatterTest < Minitest::Test
  def setup
    @formatter = MLC::Diagnostics::ErrorFormatter.new(use_color: false)
  end

  def test_format_basic_error
    origin = MLC::SourceOrigin.new(
      file: "test.mlc",
      line: 15,
      column: 10,
      end_column: 13,
      line_text: '  let x = add("hello", 5)'
    )

    output = @formatter.format_error(
      message: "Type mismatch",
      origin: origin,
      error_code: "E0308"
    )

    assert_includes output, "error[E0308]: Type mismatch"
    assert_includes output, "--> test.mlc:15:10"
    assert_includes output, 'let x = add("hello", 5)'
    assert_includes output, "^^^^"
  end

  def test_format_error_with_help
    origin = MLC::SourceOrigin.new(
      file: "program.mlc",
      line: 5,
      column: 15,
      line_text: "let result = x + y"
    )

    output = @formatter.format_error(
      message: "Cannot add str and i32",
      origin: origin,
      help: "Consider converting types"
    )

    assert_includes output, "error: Cannot add str and i32"
    assert_includes output, "= help: Consider converting types"
  end

  def test_format_type_error
    origin = MLC::SourceOrigin.new(
      file: "demo.mlc",
      line: 10,
      column: 5,
      end_column: 10,
      line_text: "let x: i32 = 3.14"
    )

    output = @formatter.format_type_error(
      expected: "i32",
      found: "f32",
      origin: origin
    )

    assert_includes output, "error[E0308]: Type mismatch"
    assert_includes output, "expected `i32`, found `f32`"
    assert_includes output, "= help:"
  end

  def test_format_undefined_var
    origin = MLC::SourceOrigin.new(
      file: "app.mlc",
      line: 20,
      column: 12,
      line_text: "  return nam + suffix"
    )

    output = @formatter.format_undefined_var(
      name: "nam",
      origin: origin,
      similar_names: ["name", "num"]
    )

    assert_includes output, "error[E0425]"
    assert_includes output, "Cannot find variable `nam`"
    assert_includes output, "Did you mean: `name`, `num`?"
  end

  def test_format_undefined_function
    origin = MLC::SourceOrigin.new(
      file: "main.mlc",
      line: 8,
      column: 10,
      line_text: "  let y = prnit(x)"
    )

    output = @formatter.format_undefined_function(
      name: "prnit",
      origin: origin,
      similar_names: ["print"]
    )

    assert_includes output, "error[E0424]"
    assert_includes output, "Cannot find function `prnit`"
    assert_includes output, "Did you mean: `print`?"
  end

  def test_format_syntax_error
    origin = MLC::SourceOrigin.new(
      file: "broken.mlc",
      line: 3,
      column: 10,
      line_text: "fn foo( -> i32 ="
    )

    output = @formatter.format_syntax_error(
      message: "Unexpected token `->`",
      origin: origin,
      expected: "parameter list or `)`"
    )

    assert_includes output, "error[E0001]: Syntax error"
    assert_includes output, "Unexpected token `->`"
    assert_includes output, "= help: Expected parameter list or `)`"
  end

  def test_format_error_without_origin
    output = @formatter.format_error(
      message: "Internal compiler error"
    )

    assert_includes output, "error: Internal compiler error"
    refute_includes output, "-->"
  end

  def test_format_error_with_notes
    origin = MLC::SourceOrigin.new(
      file: "test.mlc",
      line: 1,
      column: 1,
      line_text: "fn main() -> i32 = 42"
    )

    output = @formatter.format_error(
      message: "Unused function",
      origin: origin,
      notes: ["Function `main` is never called", "Consider removing or using it"]
    )

    assert_includes output, "= note: Function `main` is never called"
    assert_includes output, "= note: Consider removing or using it"
  end

  def test_type_suggestion_int_to_string
    output = @formatter.format_type_error(
      expected: "str",
      found: "i32",
      origin: nil
    )

    assert_includes output, "to_string()"
  end

  def test_type_suggestion_float_int
    output = @formatter.format_type_error(
      expected: "i32",
      found: "f32",
      origin: nil
    )

    assert_includes output, "explicit type conversion"
  end
end
