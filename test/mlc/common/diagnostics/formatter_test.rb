# frozen_string_literal: true

require_relative "../../../test_helper"

class DiagnosticFormatterTest < Minitest::Test
  def setup
    @source = <<~SOURCE
      fn main() -> i32 =
        let x = 42
        let y = x + 10
        x + y
    SOURCE
  end

  def test_format_error_with_origin_and_source
    origin = MLC::SourceOrigin.new(
      file: "test.mlc",
      line: 3,
      column: 13,
      end_column: 13,
      line_text: "  let y = x + 10"
    )

    formatter = MLC::Common::Diagnostics::Formatter.new(color: :never, context_lines: 1)
    result = formatter.format(
      severity: :error,
      message: "undefined variable 'z'",
      origin: origin,
      source: @source
    )

    assert_match(/test\.mlc:3:13 error: undefined variable 'z'/, result)
    assert_match(/let x = 42/, result) # context line
    assert_match(/let y = x \+ 10/, result) # error line
    assert_match(/x \+ y/, result) # context line
    assert_match(/\^/, result) # indicator
  end

  def test_format_error_without_source
    origin = MLC::SourceOrigin.new(
      file: "test.mlc",
      line: 3,
      column: 13,
      line_text: "  let y = x + 10"
    )

    formatter = MLC::Common::Diagnostics::Formatter.new(color: :never)
    result = formatter.format(
      severity: :error,
      message: "undefined variable 'z'",
      origin: origin
    )

    assert_match(/test\.mlc:3:13 error: undefined variable 'z'/, result)
    assert_match(/let y = x \+ 10/, result)
    assert_match(/\^/, result)
  end

  def test_format_warning
    origin = MLC::SourceOrigin.new(
      file: "test.mlc",
      line: 2,
      column: 7
    )

    formatter = MLC::Common::Diagnostics::Formatter.new(color: :never)
    result = formatter.format(
      severity: :warning,
      message: "unused variable 'x'",
      origin: origin
    )

    assert_match(/test\.mlc:2:7 warning: unused variable 'x'/, result)
  end

  def test_format_without_origin
    formatter = MLC::Common::Diagnostics::Formatter.new(color: :never)
    result = formatter.format(
      severity: :error,
      message: "compilation failed"
    )

    assert_match(/error: compilation failed/, result)
    refute_match(/\^/, result)
  end

  def test_multi_line_context
    origin = MLC::SourceOrigin.new(
      file: "test.mlc",
      line: 3,
      column: 13
    )

    formatter = MLC::Common::Diagnostics::Formatter.new(color: :never, context_lines: 2)
    result = formatter.format(
      severity: :error,
      message: "type error",
      origin: origin,
      source: @source
    )

    # Should show 2 lines before and after
    assert_match(/fn main/, result)  # line 1
    assert_match(/let x = 42/, result)  # line 2
    assert_match(/let y = x \+ 10/, result)  # line 3 (error)
    assert_match(/x \+ y/, result)  # line 4
  end

  def test_indicator_width
    origin = MLC::SourceOrigin.new(
      file: "test.mlc",
      line: 2,
      column: 7,
      end_column: 8,
      line_text: "  let x = 42"
    )

    formatter = MLC::Common::Diagnostics::Formatter.new(color: :never)
    result = formatter.format(
      severity: :error,
      message: "error",
      origin: origin
    )

    # Indicator should be 2 characters wide (columns 7-8)
    lines = result.split("\n")
    indicator_line = lines.find { |line| line.include?("^^") }
    assert indicator_line, "Should have multi-character indicator"
  end

  def test_color_enabled_when_requested
    formatter = MLC::Common::Diagnostics::Formatter.new(color: :always)
    assert formatter.color_enabled
  end

  def test_color_disabled_when_requested
    formatter = MLC::Common::Diagnostics::Formatter.new(color: :never)
    refute formatter.color_enabled
  end

  def test_class_method_format_error
    origin = MLC::SourceOrigin.new(
      file: "test.mlc",
      line: 1,
      column: 1
    )

    result = MLC::Common::Diagnostics::Formatter.format_error(
      "error message",
      origin: origin,
      color: :never
    )

    assert_match(/test\.mlc:1:1 error: error message/, result)
  end

  def test_class_method_format_warning
    result = MLC::Common::Diagnostics::Formatter.format_warning(
      "warning message",
      color: :never
    )

    assert_match(/warning: warning message/, result)
  end

  def test_class_method_format_info
    result = MLC::Common::Diagnostics::Formatter.format_info(
      "info message",
      color: :never
    )

    assert_match(/info: info message/, result)
  end

  def test_class_method_format_note
    result = MLC::Common::Diagnostics::Formatter.format_note(
      "note message",
      color: :never
    )

    assert_match(/note: note message/, result)
  end

  def test_handles_edge_case_first_line_error
    source = "bad syntax\nsecond line"
    origin = MLC::SourceOrigin.new(
      file: "test.mlc",
      line: 1,
      column: 1
    )

    formatter = MLC::Common::Diagnostics::Formatter.new(color: :never, context_lines: 2)
    result = formatter.format(
      severity: :error,
      message: "syntax error",
      origin: origin,
      source: source
    )

    assert_match(/bad syntax/, result)
    assert_match(/second line/, result)
  end

  def test_handles_edge_case_last_line_error
    source = "first line\nsecond line\nlast line"
    origin = MLC::SourceOrigin.new(
      file: "test.mlc",
      line: 3,
      column: 1
    )

    formatter = MLC::Common::Diagnostics::Formatter.new(color: :never, context_lines: 2)
    result = formatter.format(
      severity: :error,
      message: "error at end",
      origin: origin,
      source: source
    )

    assert_match(/first line/, result)
    assert_match(/second line/, result)
    assert_match(/last line/, result)
  end
end
