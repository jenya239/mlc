# frozen_string_literal: true

require_relative "../test_helper"
require_relative "../../lib/mlc/common/index"

class MLCCharLiteralsTest < Minitest::Test
  # ========== Lexer Tests ==========

  def test_lexer_simple_char
    source = "'a'"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    char_tokens = tokens.select { |t| t.type == :CHAR_LITERAL }
    assert_equal 1, char_tokens.size
    assert_equal 97, char_tokens.first.value  # 'a' = 97
  end

  def test_lexer_uppercase_char
    source = "'A'"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    char_tokens = tokens.select { |t| t.type == :CHAR_LITERAL }
    assert_equal 1, char_tokens.size
    assert_equal 65, char_tokens.first.value  # 'A' = 65
  end

  def test_lexer_digit_char
    source = "'0'"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    char_tokens = tokens.select { |t| t.type == :CHAR_LITERAL }
    assert_equal 1, char_tokens.size
    assert_equal 48, char_tokens.first.value  # '0' = 48
  end

  def test_lexer_space_char
    source = "' '"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    char_tokens = tokens.select { |t| t.type == :CHAR_LITERAL }
    assert_equal 1, char_tokens.size
    assert_equal 32, char_tokens.first.value  # ' ' = 32
  end

  # ========== Escape Sequences ==========

  def test_lexer_newline_escape
    source = "'\\n'"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    char_tokens = tokens.select { |t| t.type == :CHAR_LITERAL }
    assert_equal 1, char_tokens.size
    assert_equal 10, char_tokens.first.value  # '\n' = 10
  end

  def test_lexer_tab_escape
    source = "'\\t'"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    char_tokens = tokens.select { |t| t.type == :CHAR_LITERAL }
    assert_equal 1, char_tokens.size
    assert_equal 9, char_tokens.first.value  # '\t' = 9
  end

  def test_lexer_carriage_return_escape
    source = "'\\r'"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    char_tokens = tokens.select { |t| t.type == :CHAR_LITERAL }
    assert_equal 1, char_tokens.size
    assert_equal 13, char_tokens.first.value  # '\r' = 13
  end

  def test_lexer_null_escape
    source = "'\\0'"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    char_tokens = tokens.select { |t| t.type == :CHAR_LITERAL }
    assert_equal 1, char_tokens.size
    assert_equal 0, char_tokens.first.value  # '\0' = 0
  end

  def test_lexer_backslash_escape
    source = "'\\\\'"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    char_tokens = tokens.select { |t| t.type == :CHAR_LITERAL }
    assert_equal 1, char_tokens.size
    assert_equal 92, char_tokens.first.value  # '\\' = 92
  end

  def test_lexer_single_quote_escape
    source = "'\\''"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    char_tokens = tokens.select { |t| t.type == :CHAR_LITERAL }
    assert_equal 1, char_tokens.size
    assert_equal 39, char_tokens.first.value  # '\'' = 39
  end

  # ========== Multi-char -> Raw String ==========

  def test_lexer_multi_char_becomes_string
    source = "'abc'"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    # Multiple characters -> STRING_LITERAL
    string_tokens = tokens.select { |t| t.type == :STRING_LITERAL }
    assert_equal 1, string_tokens.size
    assert_equal "abc", string_tokens.first.value
  end

  def test_lexer_two_char_becomes_string
    source = "'ab'"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    string_tokens = tokens.select { |t| t.type == :STRING_LITERAL }
    assert_equal 1, string_tokens.size
    assert_equal "ab", string_tokens.first.value
  end

  # ========== Parser Tests ==========

  def test_parser_char_literal
    source = <<~MLC
      fn test() -> i32 = 'a'
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.first
    body = func.body

    assert_instance_of MLC::Source::AST::IntLit, body
    assert_equal 97, body.value
  end

  def test_parser_escape_in_function
    source = <<~MLC
      fn newline() -> i32 = '\\n'
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.first
    body = func.body

    assert_instance_of MLC::Source::AST::IntLit, body
    assert_equal 10, body.value
  end

  # ========== C++ Codegen Tests ==========

  def test_cpp_codegen_char_literal
    source = <<~MLC
      fn test() -> i32 = 'a'
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "return 97"
  end

  def test_cpp_codegen_escape_sequence
    source = <<~MLC
      fn test() -> i32 = '\\n'
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "return 10"
  end

  def test_cpp_codegen_null_char
    source = <<~MLC
      fn test() -> i32 = '\\0'
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "return 0"
  end

  # ========== Arithmetic with Char Literals ==========

  def test_char_arithmetic
    source = <<~MLC
      fn to_uppercase(c: i32) -> i32 = c - 'a' + 'A'
    MLC

    cpp = MLC.to_cpp(source)
    # 'a' = 97, 'A' = 65
    assert_includes cpp, "c - 97 + 65"
  end

  def test_char_comparison
    source = <<~MLC
      fn is_digit(c: i32) -> bool = c >= '0' && c <= '9'
    MLC

    cpp = MLC.to_cpp(source)
    # '0' = 48, '9' = 57
    assert_includes cpp, "c >= 48"
    assert_includes cpp, "c <= 57"
  end

  # ========== Unicode Support ==========

  def test_lexer_unicode_char
    source = "'Ω'"  # Greek capital letter Omega
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    char_tokens = tokens.select { |t| t.type == :CHAR_LITERAL }
    assert_equal 1, char_tokens.size
    assert_equal 937, char_tokens.first.value  # Ω = U+03A9 = 937
  end

  def test_lexer_emoji_becomes_string
    # Emoji is multi-byte, so it's more than one "character" in terms of bytes
    # but for simplicity, our implementation treats single logical character as CHAR_LITERAL
    source = "'😀'"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    # Single Unicode code point should be CHAR_LITERAL
    char_tokens = tokens.select { |t| t.type == :CHAR_LITERAL }
    assert_equal 1, char_tokens.size
    assert_equal 128512, char_tokens.first.value  # 😀 = U+1F600 = 128512
  end
end
