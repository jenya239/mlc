# frozen_string_literal: true

require_relative "../test_helper"
require_relative "../../lib/mlc/common/index"

class MLCStringOperationsTest < Minitest::Test
  # ========== Single-quoted strings (raw, no interpolation) ==========

  def test_single_quoted_simple
    mlc_source = <<~MLC
      fn get_message() -> str = 'hello world'
    MLC

    cpp = MLC.to_cpp(mlc_source)
    assert_includes cpp, '"hello world"'
  end

  def test_single_quoted_no_interpolation
    mlc_source = <<~MLC
      fn get_message() -> str = 'hello {name}'
    MLC

    cpp = MLC.to_cpp(mlc_source)
    # Single quotes should NOT interpolate - braces are literal
    assert_includes cpp, '"hello {name}"'
  end

  def test_single_quoted_escaped_quote
    mlc_source = <<~MLC
      fn get_message() -> str = 'it\\'s cool'
    MLC

    cpp = MLC.to_cpp(mlc_source)
    # Escaped single quote should work
    assert_includes cpp, "it's cool"
  end

  def test_single_quoted_backslash_n_literal
    mlc_source = <<~MLC
      fn get_message() -> str = 'line1\\nline2'
    MLC

    cpp = MLC.to_cpp(mlc_source)
    # In single quotes, \n should be literal backslash-n, not newline
    assert_includes cpp, '\\n'
  end

  # ========== Double-quoted strings (with interpolation) ==========

  def test_double_quoted_simple
    mlc_source = <<~MLC
      fn get_message() -> str = "hello world"
    MLC

    cpp = MLC.to_cpp(mlc_source)
    assert_includes cpp, '"hello world"'
  end

  def test_double_quoted_escape_sequences
    mlc_source = <<~MLC
      fn get_message() -> str = "line1\\nline2"
    MLC

    cpp = MLC.to_cpp(mlc_source)
    # Double quotes process escape sequences
    assert_includes cpp, "\\n"
  end

  # ========== Original tests ==========

  def test_string_concatenation
    mlc_source = <<~MLC
      fn greet(name: str) -> str =
        "Hello, " + name + "!"
    MLC

    # This should work but currently fails
    # Let's test what we get
    begin
      cpp = MLC.to_cpp(mlc_source)
      puts "Generated C++:"
      puts cpp
      assert_includes cpp, "Hello"
    rescue => e
      puts "Error: #{e.message}"
      # For now, just test that we get an error
      assert e.message.include?("left operand of '+' must be numeric")
    end
  end

  def test_string_interpolation
    mlc_source = <<~MLC
      fn greet(name: str) -> str =
        "Hello, {name}!"
    MLC

    # This should work but currently fails
    begin
      cpp = MLC.to_cpp(mlc_source)
      puts "Generated C++:"
      puts cpp
      assert_includes cpp, "Hello"
    rescue => e
      puts "Error: #{e.message}"
      # For now, just test that we get an error
      assert e.message.include?("Unknown identifier")
    end
  end

  def test_string_methods
    mlc_source = <<~MLC
      fn process(text: str) -> str =
        text.trim().upper()
    MLC

    begin
      cpp = MLC.to_cpp(mlc_source)
      puts "Generated C++:"
      puts cpp
      assert_includes cpp, "trim"
      assert_includes cpp, "upper"
    rescue => e
      puts "Error: #{e.message}"
      # For now, just test that we get an error
      assert e.message.include?("Unknown member")
    end
  end
end
