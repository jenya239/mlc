# frozen_string_literal: true

require_relative "../test_helper"
require_relative "../../lib/mlc/common/index"

class MLCBitwiseOpsTest < Minitest::Test
  # Lexer tests - verify tokens are recognized

  def test_lexer_bitwise_and
    source = "1 & 2"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    ops = tokens.select { |t| t.type == :OPERATOR && t.value == "&" }
    assert_equal 1, ops.size
  end

  def test_lexer_bitwise_or
    source = "1 | 2"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    ops = tokens.select { |t| t.type == :OPERATOR && t.value == "|" }
    assert_equal 1, ops.size
  end

  def test_lexer_bitwise_xor
    source = "1 ^ 2"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    ops = tokens.select { |t| t.type == :OPERATOR && t.value == "^" }
    assert_equal 1, ops.size
  end

  def test_lexer_bitwise_not
    source = "~1"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    ops = tokens.select { |t| t.type == :OPERATOR && t.value == "~" }
    assert_equal 1, ops.size
  end

  def test_lexer_left_shift
    source = "1 << 2"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    ops = tokens.select { |t| t.type == :OPERATOR && t.value == "<<" }
    assert_equal 1, ops.size
  end

  def test_lexer_right_shift
    source = "1 >> 2"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    ops = tokens.select { |t| t.type == :OPERATOR && t.value == ">>" }
    assert_equal 1, ops.size
  end

  # Parser tests - verify AST structure

  def test_parser_bitwise_and
    source = <<~MLC
      fn test(a: i32, b: i32) -> i32 = a & b
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.first
    body = func.body

    assert_instance_of MLC::Source::AST::BinaryOp, body
    assert_equal "&", body.op
  end

  def test_parser_bitwise_or
    source = <<~MLC
      fn test(a: i32, b: i32) -> i32 = a | b
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.first
    body = func.body

    assert_instance_of MLC::Source::AST::BinaryOp, body
    assert_equal "|", body.op
  end

  def test_parser_bitwise_xor
    source = <<~MLC
      fn test(a: i32, b: i32) -> i32 = a ^ b
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.first
    body = func.body

    assert_instance_of MLC::Source::AST::BinaryOp, body
    assert_equal "^", body.op
  end

  def test_parser_bitwise_not
    source = <<~MLC
      fn test(a: i32) -> i32 = ~a
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.first
    body = func.body

    assert_instance_of MLC::Source::AST::UnaryOp, body
    assert_equal "~", body.op
  end

  def test_parser_left_shift
    source = <<~MLC
      fn test(a: i32, b: i32) -> i32 = a << b
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.first
    body = func.body

    assert_instance_of MLC::Source::AST::BinaryOp, body
    assert_equal "<<", body.op
  end

  def test_parser_right_shift
    source = <<~MLC
      fn test(a: i32, b: i32) -> i32 = a >> b
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.first
    body = func.body

    assert_instance_of MLC::Source::AST::BinaryOp, body
    assert_equal ">>", body.op
  end

  # Operator precedence tests

  def test_bitwise_or_lower_than_xor
    # a | b ^ c should parse as a | (b ^ c)
    source = <<~MLC
      fn test(a: i32, b: i32, c: i32) -> i32 = a | b ^ c
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.first
    body = func.body

    assert_instance_of MLC::Source::AST::BinaryOp, body
    assert_equal "|", body.op
    assert_instance_of MLC::Source::AST::BinaryOp, body.right
    assert_equal "^", body.right.op
  end

  def test_bitwise_xor_lower_than_and
    # a ^ b & c should parse as a ^ (b & c)
    source = <<~MLC
      fn test(a: i32, b: i32, c: i32) -> i32 = a ^ b & c
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.first
    body = func.body

    assert_instance_of MLC::Source::AST::BinaryOp, body
    assert_equal "^", body.op
    assert_instance_of MLC::Source::AST::BinaryOp, body.right
    assert_equal "&", body.right.op
  end

  def test_bitwise_and_lower_than_equality
    # a & b == c should parse as a & (b == c)
    source = <<~MLC
      fn test(a: i32, b: i32, c: i32) -> bool = a & b == c
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.first
    body = func.body

    assert_instance_of MLC::Source::AST::BinaryOp, body
    assert_equal "&", body.op
    assert_instance_of MLC::Source::AST::BinaryOp, body.right
    assert_equal "==", body.right.op
  end

  def test_shift_higher_than_comparison
    # a < b << c should parse as a < (b << c)
    source = <<~MLC
      fn test(a: i32, b: i32, c: i32) -> bool = a < b << c
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.first
    body = func.body

    assert_instance_of MLC::Source::AST::BinaryOp, body
    assert_equal "<", body.op
    assert_instance_of MLC::Source::AST::BinaryOp, body.right
    assert_equal "<<", body.right.op
  end

  def test_shift_lower_than_addition
    # a + b << c should parse as (a + b) << c
    source = <<~MLC
      fn test(a: i32, b: i32, c: i32) -> i32 = a + b << c
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.first
    body = func.body

    assert_instance_of MLC::Source::AST::BinaryOp, body
    assert_equal "<<", body.op
    assert_instance_of MLC::Source::AST::BinaryOp, body.left
    assert_equal "+", body.left.op
  end

  def test_logical_or_lower_than_bitwise_or
    # a || b | c should parse as a || (b | c)
    source = <<~MLC
      fn test(a: bool, b: i32, c: i32) -> bool = a || b | c
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.first
    body = func.body

    assert_instance_of MLC::Source::AST::BinaryOp, body
    assert_equal "||", body.op
    assert_instance_of MLC::Source::AST::BinaryOp, body.right
    assert_equal "|", body.right.op
  end

  # C++ codegen tests

  def test_cpp_codegen_bitwise_and
    source = <<~MLC
      fn test(a: i32, b: i32) -> i32 = a & b
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "a & b"
  end

  def test_cpp_codegen_bitwise_or
    source = <<~MLC
      fn test(a: i32, b: i32) -> i32 = a | b
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "a | b"
  end

  def test_cpp_codegen_bitwise_xor
    source = <<~MLC
      fn test(a: i32, b: i32) -> i32 = a ^ b
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "a ^ b"
  end

  def test_cpp_codegen_bitwise_not
    source = <<~MLC
      fn test(a: i32) -> i32 = ~a
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "~a"
  end

  def test_cpp_codegen_left_shift
    source = <<~MLC
      fn test(a: i32, b: i32) -> i32 = a << b
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "a << b"
  end

  def test_cpp_codegen_right_shift
    source = <<~MLC
      fn test(a: i32, b: i32) -> i32 = a >> b
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "a >> b"
  end

  # Complex expression tests

  def test_complex_bitwise_expression
    source = <<~MLC
      fn set_bit(value: i32, bit: i32) -> i32 = value | (1 << bit)
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "value | "
    assert_includes cpp, "1 << bit"
  end

  def test_clear_bit_expression
    # Note: ~1 << bit is equivalent to ~(1) << bit in C++ precedence
    # For actual clear_bit, you need: value & (~(1 << bit))
    # But this test verifies that basic bitwise NOT and shift work
    source = <<~MLC
      fn invert_shift(bit: i32) -> i32 = ~1 << bit
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "~1 << bit"
  end

  def test_toggle_bit_expression
    source = <<~MLC
      fn toggle_bit(value: i32, bit: i32) -> i32 = value ^ (1 << bit)
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "value ^ "
    assert_includes cpp, "1 << bit"
  end
end
