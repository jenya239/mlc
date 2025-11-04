# frozen_string_literal: true

require_relative "../test_helper"

class StdlibStringTest < Minitest::Test
  def test_string_module_parses
    source = <<~MLCORA
      import { upper, lower } from "String"

      fn test() -> str =
        upper("hello")
    MLCORA

    ast = MLC.parse(source)
    assert_equal 1, ast.imports.length
    assert_equal "String", ast.imports.first.path
  end

  def test_string_upper
    source = <<~MLCORA
      import { upper } from "String"

      fn test() -> str =
        upper("hello")
    MLCORA

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "upper"
    assert_includes cpp, "hello"
  end

  def test_string_lower
    source = <<~MLCORA
      import { lower } from "String"

      fn test() -> str =
        lower("HELLO")
    MLCORA

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "lower"
    assert_includes cpp, "HELLO"
  end

  def test_string_trim
    source = <<~MLCORA
      import { trim } from "String"

      fn test() -> str =
        trim("  hello  ")
    MLCORA

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "trim"
  end

  def test_string_split
    source = <<~MLCORA
      import { split } from "String"

      fn test() -> str[] =
        split("a,b,c", ",")
    MLCORA

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "split"
  end

  def test_string_contains
    source = <<~MLCORA
      import { contains } from "String"

      fn test() -> bool =
        contains("hello world", "world")
    MLCORA

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "contains"
  end

  def test_string_starts_with
    source = <<~MLCORA
      import { starts_with } from "String"

      fn test() -> bool =
        starts_with("hello", "he")
    MLCORA

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "starts_with"
  end

  def test_string_ends_with
    source = <<~MLCORA
      import { ends_with } from "String"

      fn test() -> bool =
        ends_with("hello", "lo")
    MLCORA

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "ends_with"
  end

  def test_string_lines
    source = <<~MLCORA
      import { lines } from "String"

      fn test() -> str[] =
        lines("line1\\nline2\\nline3")
    MLCORA

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "lines"
  end

  def test_string_words
    source = <<~MLCORA
      import { words } from "String"

      fn test() -> str[] =
        words("  hello   world  ")
    MLCORA

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "words"
  end

  def test_string_join
    source = <<~MLCORA
      import { join } from "String"

      fn test() -> str =
        join(["a", "b", "c"], ", ")
    MLCORA

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "join"
  end

  def test_string_repeat
    source = <<~MLCORA
      import { repeat } from "String"

      fn test() -> str =
        repeat("ab", 3)
    MLCORA

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "repeat"
  end

  def test_multiple_imports
    source = <<~MLCORA
      import { upper, lower, trim } from "String"

      fn test(s: str) -> str =
        upper(trim(s))
    MLCORA

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "upper"
    assert_includes cpp, "trim"
  end

  def test_string_with_do_blocks
    source = <<~MLCORA
      import { upper, trim } from "String"

      fn process(s: str) -> str = do
        let trimmed = trim(s);
        let result = upper(trimmed);
        result
      end
    MLCORA

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "trim"
    assert_includes cpp, "upper"
  end
end
