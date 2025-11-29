# frozen_string_literal: true

require_relative "../test_helper"
require_relative "../../lib/mlc/common/index"

class MLCExtendedIntTypesTest < Minitest::Test
  # ========== Lexer Tests ==========

  def test_lexer_i8_keyword
    source = "i8"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    keyword = tokens.find { |t| t.type == :I8 }
    refute_nil keyword
    assert_equal "i8", keyword.value
  end

  def test_lexer_i16_keyword
    source = "i16"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    keyword = tokens.find { |t| t.type == :I16 }
    refute_nil keyword
    assert_equal "i16", keyword.value
  end

  def test_lexer_u8_keyword
    source = "u8"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    keyword = tokens.find { |t| t.type == :U8 }
    refute_nil keyword
    assert_equal "u8", keyword.value
  end

  def test_lexer_u16_keyword
    source = "u16"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    keyword = tokens.find { |t| t.type == :U16 }
    refute_nil keyword
    assert_equal "u16", keyword.value
  end

  def test_lexer_u32_keyword
    source = "u32"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    keyword = tokens.find { |t| t.type == :U32 }
    refute_nil keyword
    assert_equal "u32", keyword.value
  end

  def test_lexer_u64_keyword
    source = "u64"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    keyword = tokens.find { |t| t.type == :U64 }
    refute_nil keyword
    assert_equal "u64", keyword.value
  end

  def test_lexer_i64_keyword
    source = "i64"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    keyword = tokens.find { |t| t.type == :I64 }
    refute_nil keyword
    assert_equal "i64", keyword.value
  end

  def test_lexer_f64_keyword
    source = "f64"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    keyword = tokens.find { |t| t.type == :F64 }
    refute_nil keyword
    assert_equal "f64", keyword.value
  end

  # ========== Parser Tests ==========

  def test_parser_i8_param_type
    source = <<~MLC
      fn test(x: i8) -> i8 = x
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.first
    param_type = func.params.first.type

    assert_instance_of MLC::Source::AST::PrimType, param_type
    assert_equal "i8", param_type.name
  end

  def test_parser_u16_return_type
    source = <<~MLC
      fn test(x: u16) -> u16 = x
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.first
    ret_type = func.ret_type

    assert_instance_of MLC::Source::AST::PrimType, ret_type
    assert_equal "u16", ret_type.name
  end

  def test_parser_mixed_int_types
    source = <<~MLC
      fn convert(a: i8, b: u16, c: i32, d: u64) -> i64 = 0
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.first
    param_types = func.params.map { |p| p.type.name }

    assert_equal %w[i8 u16 i32 u64], param_types
    assert_equal "i64", func.ret_type.name
  end

  # ========== C++ Codegen Tests ==========

  def test_cpp_codegen_i8
    source = <<~MLC
      fn test(x: i8) -> i8 = x
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "int8_t"
  end

  def test_cpp_codegen_u8
    source = <<~MLC
      fn test(x: u8) -> u8 = x
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "uint8_t"
  end

  def test_cpp_codegen_i16
    source = <<~MLC
      fn test(x: i16) -> i16 = x
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "int16_t"
  end

  def test_cpp_codegen_u16
    source = <<~MLC
      fn test(x: u16) -> u16 = x
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "uint16_t"
  end

  def test_cpp_codegen_u32
    source = <<~MLC
      fn test(x: u32) -> u32 = x
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "uint32_t"
  end

  def test_cpp_codegen_i64
    source = <<~MLC
      fn test(x: i64) -> i64 = x
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "int64_t"
  end

  def test_cpp_codegen_u64
    source = <<~MLC
      fn test(x: u64) -> u64 = x
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "uint64_t"
  end

  def test_cpp_codegen_f64
    source = <<~MLC
      fn test(x: f64) -> f64 = x
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "double"
  end

  # ========== Type System Tests ==========

  def test_predicates_numeric_includes_all_int_types
    predicates = MLC::Common::Typing::Predicates

    %w[i8 i16 i32 i64 u8 u16 u32 u64 f32 f64].each do |type_name|
      assert_includes predicates::NUMERIC_PRIMITIVES, type_name,
        "#{type_name} should be in NUMERIC_PRIMITIVES"
    end
  end

  def test_predicates_integer_includes_all_int_types
    predicates = MLC::Common::Typing::Predicates

    %w[i8 i16 i32 i64 u8 u16 u32 u64].each do |type_name|
      assert_includes predicates::INTEGER_PRIMITIVES, type_name,
        "#{type_name} should be in INTEGER_PRIMITIVES"
    end
  end

  def test_predicates_integer_excludes_floats
    predicates = MLC::Common::Typing::Predicates

    %w[f32 f64].each do |type_name|
      refute_includes predicates::INTEGER_PRIMITIVES, type_name,
        "#{type_name} should NOT be in INTEGER_PRIMITIVES"
    end
  end

  def test_predicates_signed_integers
    predicates = MLC::Common::Typing::Predicates

    assert_equal %w[i8 i16 i32 i64], predicates::SIGNED_INTEGERS
  end

  def test_predicates_unsigned_integers
    predicates = MLC::Common::Typing::Predicates

    assert_equal %w[u8 u16 u32 u64], predicates::UNSIGNED_INTEGERS
  end

  # ========== Bitwise Operations with Extended Types ==========

  def test_bitwise_and_u8
    # u8 bitwise AND with u8 variable (both types match)
    source = <<~MLC
      fn mask_byte(x: u8, mask: u8) -> u8 = x & mask
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "uint8_t"
    assert_includes cpp, "x & mask"
  end

  def test_bitwise_shift_u16
    # u16 shift with u16 (both types match)
    source = <<~MLC
      fn shift_word(x: u16, bits: u16) -> u16 = x << bits
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "uint16_t"
    assert_includes cpp, "x << bits"
  end
end
