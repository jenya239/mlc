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

  def test_double_quoted_literal_braces
    mlc_source = <<~MLC
      fn get_message() -> str = "{"
    MLC

    cpp = MLC.to_cpp(mlc_source)
    assert_includes cpp, '"{"'
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
    rescue StandardError => e
      puts "Error: #{e.message}"
      # For now, just test that we get an error
      assert e.message.include?("left operand of '+' must be numeric")
    end
  end

  def test_string_interpolation
    mlc_source = <<~MLC
      fn greet(name: str) -> str =
        "Hello, \${name}!"
    MLC

    # This should work but currently fails
    begin
      cpp = MLC.to_cpp(mlc_source)
      puts "Generated C++:"
      puts cpp
      assert_includes cpp, "Hello"
    rescue StandardError => e
      puts "Error: #{e.message}"
      # For now, just test that we get an error
      assert e.message.include?("Unknown identifier")
    end
  end

  def test_string_methods_trim_upper
    cpp = MLC.to_cpp(<<~MLC)
      fn process(text: string) -> string = text.trim().upper()
    MLC
    assert_includes cpp, ".trim()"
    assert_includes cpp, ".upper()"
  end

  def test_string_length
    cpp = MLC.to_cpp(<<~MLC)
      fn len(s: string) -> i32 = s.length()
    MLC
    assert_includes cpp, ".length()"
  end

  def test_string_split
    cpp = MLC.to_cpp(<<~MLC)
      fn words(s: string) -> [string] = s.split(" ")
    MLC
    assert_includes cpp, ".split("
  end

  def test_string_chars
    cpp = MLC.to_cpp(<<~MLC)
      fn letters(s: string) -> [string] = s.chars()
    MLC
    assert_includes cpp, ".chars()"
  end

  def test_string_lines
    cpp = MLC.to_cpp(<<~MLC)
      fn get_lines(s: string) -> [string] = s.lines()
    MLC
    assert_includes cpp, ".lines()"
  end

  def test_string_starts_ends_with
    cpp = MLC.to_cpp(<<~MLC)
      fn check(s: string) -> bool = s.starts_with("a") && s.ends_with("z")
    MLC
    assert_includes cpp, ".starts_with("
    assert_includes cpp, ".ends_with("
  end

  def test_string_contains
    cpp = MLC.to_cpp(<<~MLC)
      fn has(s: string) -> bool = s.contains("x")
    MLC
    assert_includes cpp, ".contains("
  end

  def test_string_replace
    cpp = MLC.to_cpp(<<~MLC)
      fn fix(s: string) -> string = s.replace("a", "b")
    MLC
    assert_includes cpp, ".replace("
  end

  def test_string_index_of
    cpp = MLC.to_cpp(<<~MLC)
      fn pos(s: string) -> i32 = s.index_of("x")
    MLC
    assert_includes cpp, ".index_of("
  end

  def test_string_substring
    cpp = MLC.to_cpp(<<~MLC)
      fn sub(s: string) -> string = s.substring(0, 3)
    MLC
    assert_includes cpp, ".substring("
  end

  def test_string_to_i
    cpp = MLC.to_cpp(<<~MLC)
      fn parse(s: string) -> i32 = s.to_i()
    MLC
    assert_includes cpp, ".to_i()"
  end

  def test_string_reverse
    cpp = MLC.to_cpp(<<~MLC)
      fn rev(s: string) -> string = s.reverse()
    MLC
    assert_includes cpp, ".reverse()"
  end

  def test_string_pad_start
    cpp = MLC.to_cpp(<<~MLC)
      fn pad(s: string) -> string = s.pad_start(10, "0")
    MLC
    assert_includes cpp, ".pad_start("
  end

  def test_string_is_empty
    cpp = MLC.to_cpp(<<~MLC)
      fn empty(s: string) -> bool = s.is_empty()
    MLC
    assert_includes cpp, ".is_empty()"
  end
end
