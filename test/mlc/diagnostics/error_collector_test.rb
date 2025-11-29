# frozen_string_literal: true

require_relative "../../test_helper"
require_relative "../../../lib/mlc/common/diagnostics/error_collector"

class ErrorCollectorTest < Minitest::Test
  def setup
    @collector = MLC::Diagnostics::ErrorCollector.new
  end

  def test_add_error
    @collector.add_error("Test error")
    assert @collector.has_errors?
    assert_equal 1, @collector.error_count
    assert_equal 1, @collector.errors.count
  end

  def test_add_warning
    @collector.add_warning("Test warning")
    refute @collector.has_errors?
    assert_equal 1, @collector.warnings.count
  end

  def test_add_note
    @collector.add_note("Test note")
    refute @collector.has_errors?
    assert_equal 1, @collector.diagnostics.count
  end

  def test_multiple_errors
    @collector.add_error("Error 1")
    @collector.add_error("Error 2")
    @collector.add_error("Error 3")

    assert_equal 3, @collector.error_count
    assert_equal 3, @collector.errors.count
  end

  def test_mixed_diagnostics
    @collector.add_error("Error 1")
    @collector.add_warning("Warning 1")
    @collector.add_error("Error 2")
    @collector.add_note("Note 1")

    assert_equal 2, @collector.error_count
    assert_equal 1, @collector.warnings.count
    assert_equal 4, @collector.diagnostics.count
  end

  def test_error_with_origin
    origin = MLC::SourceOrigin.new(
      file: "test.mlc",
      line: 10,
      column: 5
    )

    @collector.add_error("Error with origin", origin: origin)

    error = @collector.errors.first
    assert_equal origin, error.origin
  end

  def test_error_with_code_and_help
    @collector.add_error(
      "Type mismatch",
      error_code: "E0308",
      help: "Try converting the type"
    )

    error = @collector.errors.first
    assert_equal "E0308", error.error_code
    assert_equal "Try converting the type", error.help
  end

  def test_max_errors_limit
    collector = MLC::Diagnostics::ErrorCollector.new(max_errors: 3)

    5.times { |i| collector.add_error("Error #{i}") }

    assert_equal 3, collector.error_count
    assert_equal 3, collector.errors.count
  end

  def test_clear
    @collector.add_error("Error 1")
    @collector.add_warning("Warning 1")

    @collector.clear

    refute @collector.has_errors?
    assert_equal 0, @collector.error_count
    assert_equal 0, @collector.diagnostics.count
  end

  def test_summary_no_errors
    assert_equal "no errors", @collector.summary
  end

  def test_summary_one_error
    @collector.add_error("Error")
    assert_equal "1 error", @collector.summary
  end

  def test_summary_multiple_errors
    @collector.add_error("Error 1")
    @collector.add_error("Error 2")
    assert_equal "2 errors", @collector.summary
  end

  def test_summary_errors_and_warnings
    @collector.add_error("Error")
    @collector.add_warning("Warning 1")
    @collector.add_warning("Warning 2")
    assert_equal "1 error, 2 warnings", @collector.summary
  end

  def test_raise_if_errors_with_errors
    @collector.add_error("Test error")

    assert_raises(MLC::CompileError) do
      @collector.raise_if_errors!
    end
  end

  def test_raise_if_errors_without_errors
    @collector.add_warning("Just a warning")

    # Should not raise
    @collector.raise_if_errors!
  end

  def test_format_all
    origin = MLC::SourceOrigin.new(
      file: "test.mlc",
      line: 5,
      column: 10,
      line_text: "let x = bad_call()"
    )

    @collector.add_error("Undefined function 'bad_call'", origin: origin)

    output = @collector.format_all(use_color: false)

    assert_includes output, "error:"
    assert_includes output, "Undefined function"
    assert_includes output, "test.mlc:5:10"
  end

  def test_diagnostic_severity_methods
    @collector.add_error("Error")
    @collector.add_warning("Warning")

    assert @collector.errors.first.error?
    refute @collector.errors.first.warning?
    assert @collector.warnings.first.warning?
    refute @collector.warnings.first.error?
  end
end
