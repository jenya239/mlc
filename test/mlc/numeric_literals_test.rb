# frozen_string_literal: true

require_relative "../test_helper"
require_relative "../../lib/mlc/common/index"

class MLCNumericLiteralsTest < Minitest::Test
  # ========== Binary Literals (0b) ==========

  def test_lexer_binary_simple
    source = "0b1010"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    int_tokens = tokens.select { |t| t.type == :INT_LITERAL }
    assert_equal 1, int_tokens.size
    assert_equal 10, int_tokens.first.value  # 0b1010 = 10
  end

  def test_lexer_binary_with_underscore
    source = "0b1111_0000"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    int_tokens = tokens.select { |t| t.type == :INT_LITERAL }
    assert_equal 1, int_tokens.size
    assert_equal 240, int_tokens.first.value  # 0b11110000 = 240
  end

  def test_lexer_binary_uppercase
    source = "0B1100"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    int_tokens = tokens.select { |t| t.type == :INT_LITERAL }
    assert_equal 1, int_tokens.size
    assert_equal 12, int_tokens.first.value  # 0B1100 = 12
  end

  # ========== Octal Literals (0o) ==========

  def test_lexer_octal_simple
    source = "0o755"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    int_tokens = tokens.select { |t| t.type == :INT_LITERAL }
    assert_equal 1, int_tokens.size
    assert_equal 493, int_tokens.first.value  # 0o755 = 493
  end

  def test_lexer_octal_with_underscore
    source = "0o7_5_5"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    int_tokens = tokens.select { |t| t.type == :INT_LITERAL }
    assert_equal 1, int_tokens.size
    assert_equal 493, int_tokens.first.value
  end

  def test_lexer_octal_uppercase
    source = "0O777"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    int_tokens = tokens.select { |t| t.type == :INT_LITERAL }
    assert_equal 1, int_tokens.size
    assert_equal 511, int_tokens.first.value  # 0O777 = 511
  end

  # ========== Hexadecimal Literals (0x) ==========

  def test_lexer_hex_simple
    source = "0xFF"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    int_tokens = tokens.select { |t| t.type == :INT_LITERAL }
    assert_equal 1, int_tokens.size
    assert_equal 255, int_tokens.first.value
  end

  def test_lexer_hex_with_underscore
    source = "0xFF_FF"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    int_tokens = tokens.select { |t| t.type == :INT_LITERAL }
    assert_equal 1, int_tokens.size
    assert_equal 65535, int_tokens.first.value
  end

  def test_lexer_hex_uppercase
    source = "0XABCD"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    int_tokens = tokens.select { |t| t.type == :INT_LITERAL }
    assert_equal 1, int_tokens.size
    assert_equal 43981, int_tokens.first.value  # 0xABCD = 43981
  end

  def test_lexer_hex_lowercase
    source = "0xdeadbeef"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    int_tokens = tokens.select { |t| t.type == :INT_LITERAL }
    assert_equal 1, int_tokens.size
    assert_equal 3735928559, int_tokens.first.value
  end

  # ========== Decimal with Underscore ==========

  def test_lexer_decimal_with_underscore
    source = "1_000_000"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    int_tokens = tokens.select { |t| t.type == :INT_LITERAL }
    assert_equal 1, int_tokens.size
    assert_equal 1000000, int_tokens.first.value
  end

  def test_lexer_float_with_underscore
    source = "3.141_592"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    float_tokens = tokens.select { |t| t.type == :FLOAT_LITERAL }
    assert_equal 1, float_tokens.size
    assert_in_delta 3.141592, float_tokens.first.value, 0.000001
  end

  # ========== Parser Integration ==========

  def test_parser_binary_literal
    source = <<~MLC
      fn test() -> i32 = 0b1010
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.first
    body = func.body

    assert_instance_of MLC::Source::AST::IntLit, body
    assert_equal 10, body.value
  end

  def test_parser_hex_literal
    source = <<~MLC
      fn test() -> i32 = 0xFF
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.first
    body = func.body

    assert_instance_of MLC::Source::AST::IntLit, body
    assert_equal 255, body.value
  end

  def test_parser_octal_literal
    source = <<~MLC
      fn test() -> i32 = 0o777
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.first
    body = func.body

    assert_instance_of MLC::Source::AST::IntLit, body
    assert_equal 511, body.value
  end

  def test_parser_decimal_with_underscore
    source = <<~MLC
      fn test() -> i32 = 1_000_000
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.first
    body = func.body

    assert_instance_of MLC::Source::AST::IntLit, body
    assert_equal 1000000, body.value
  end

  # ========== C++ Codegen ==========

  def test_cpp_codegen_binary_literal
    source = <<~MLC
      fn test() -> i32 = 0b1010
    MLC

    cpp = MLC.to_cpp(source)
    # Binary is converted to decimal value 10
    assert_includes cpp, "return 10"
  end

  def test_cpp_codegen_hex_literal
    source = <<~MLC
      fn test() -> i32 = 0xFF
    MLC

    cpp = MLC.to_cpp(source)
    # Hex is converted to decimal value 255
    assert_includes cpp, "return 255"
  end

  def test_cpp_codegen_octal_literal
    source = <<~MLC
      fn test() -> i32 = 0o777
    MLC

    cpp = MLC.to_cpp(source)
    # Octal is converted to decimal value 511
    assert_includes cpp, "return 511"
  end

  # ========== Bitwise operations with numeric literals ==========

  def test_bitwise_with_hex
    source = <<~MLC
      fn mask(value: i32) -> i32 = value & 0xFF
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "value & 255"
  end

  def test_bitwise_with_binary
    source = <<~MLC
      fn set_bits(value: i32) -> i32 = value | 0b1111
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "value | 15"
  end

  def test_shift_with_hex
    source = <<~MLC
      fn shift_mask(value: i32) -> i32 = (value >> 8) & 0xFF
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, ">> 8"
    assert_includes cpp, "& 255"
  end

  # ========== Edge Cases ==========

  def test_zero_values
    source = "0b0"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize
    int_tokens = tokens.select { |t| t.type == :INT_LITERAL }
    assert_equal 0, int_tokens.first.value

    source = "0o0"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize
    int_tokens = tokens.select { |t| t.type == :INT_LITERAL }
    assert_equal 0, int_tokens.first.value

    source = "0x0"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize
    int_tokens = tokens.select { |t| t.type == :INT_LITERAL }
    assert_equal 0, int_tokens.first.value
  end

  def test_multiple_underscores
    source = "1__000"  # Double underscore
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    int_tokens = tokens.select { |t| t.type == :INT_LITERAL }
    assert_equal 1, int_tokens.size
    assert_equal 1000, int_tokens.first.value
  end

  def test_leading_underscore_not_number
    # _123 is identifier, not number
    source = "_123"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    id_tokens = tokens.select { |t| t.type == :IDENTIFIER }
    assert_equal 1, id_tokens.size
    assert_equal "_123", id_tokens.first.value
  end
end
