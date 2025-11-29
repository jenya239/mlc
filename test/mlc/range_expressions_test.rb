# frozen_string_literal: true

require_relative "../test_helper"
require_relative "../../lib/mlc/common/index"

class MLCRangeExpressionsTest < Minitest::Test
  # ========== Lexer Tests ==========

  def test_lexer_range_token
    source = "1..10"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    range_tokens = tokens.select { |t| t.type == :RANGE }
    assert_equal 1, range_tokens.size
    assert_equal "..", range_tokens.first.value
  end

  def test_lexer_spread_token
    source = "1...10"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    spread_tokens = tokens.select { |t| t.type == :SPREAD }
    assert_equal 1, spread_tokens.size
    assert_equal "...", spread_tokens.first.value
  end

  def test_lexer_range_with_identifiers
    source = "start..end_val"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    idents = tokens.select { |t| t.type == :IDENTIFIER }
    assert_equal 2, idents.size
    assert_equal "start", idents[0].value
    assert_equal "end_val", idents[1].value

    range_tokens = tokens.select { |t| t.type == :RANGE }
    assert_equal 1, range_tokens.size
  end

  # ========== Parser Tests ==========

  def test_parser_inclusive_range
    source = <<~MLC
      fn test() -> i32 = 1..10
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.first
    body = func.body

    assert_instance_of MLC::Source::AST::RangeExpr, body
    assert_equal 1, body.start.value
    assert_equal 10, body.end_expr.value
    assert_equal true, body.inclusive
  end

  def test_parser_exclusive_range
    source = <<~MLC
      fn test() -> i32 = 1...10
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.first
    body = func.body

    assert_instance_of MLC::Source::AST::RangeExpr, body
    assert_equal 1, body.start.value
    assert_equal 10, body.end_expr.value
    assert_equal false, body.inclusive
  end

  def test_parser_range_with_variables
    source = <<~MLC
      fn test(a: i32, b: i32) -> i32 = a..b
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.first
    body = func.body

    assert_instance_of MLC::Source::AST::RangeExpr, body
    assert_instance_of MLC::Source::AST::VarRef, body.start
    assert_instance_of MLC::Source::AST::VarRef, body.end_expr
    assert_equal "a", body.start.name
    assert_equal "b", body.end_expr.name
  end

  def test_parser_range_with_expressions
    source = <<~MLC
      fn test(n: i32) -> i32 = 0..n+1
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.first
    body = func.body

    assert_instance_of MLC::Source::AST::RangeExpr, body
    assert_equal 0, body.start.value
    # n+1 should be parsed as end_expr since + has higher precedence
    assert_instance_of MLC::Source::AST::BinaryOp, body.end_expr
  end

  def test_parser_range_precedence_lower_than_arithmetic
    # Range should bind looser than arithmetic
    source = <<~MLC
      fn test() -> i32 = 1+2..3+4
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.first
    body = func.body

    # Should parse as (1+2)..(3+4), not 1+(2..3)+4
    assert_instance_of MLC::Source::AST::RangeExpr, body
    assert_instance_of MLC::Source::AST::BinaryOp, body.start
    assert_instance_of MLC::Source::AST::BinaryOp, body.end_expr
  end

  def test_parser_range_precedence_lower_than_shift
    source = <<~MLC
      fn test() -> i32 = 1<<2..3<<4
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.first
    body = func.body

    # Should parse as (1<<2)..(3<<4)
    assert_instance_of MLC::Source::AST::RangeExpr, body
    assert_instance_of MLC::Source::AST::BinaryOp, body.start
    assert_equal "<<", body.start.op
    assert_instance_of MLC::Source::AST::BinaryOp, body.end_expr
    assert_equal "<<", body.end_expr.op
  end

  # ========== For Loop Integration Tests ==========

  def test_parser_for_loop_with_range
    source = <<~MLC
      fn test() -> i32 = do
        let mut sum = 0;
        for i in 1..10 do
          sum = sum + i
        end
        sum
      end
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.first
    # Find for loop in block
    block = func.body
    assert_instance_of MLC::Source::AST::BlockExpr, block
  end

  # ========== Origin/Location Tests ==========

  def test_range_has_origin
    source = <<~MLC
      fn test() -> i32 = 1..10
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.first
    body = func.body

    refute_nil body.origin, "RangeExpr should have origin"
    assert_respond_to body.origin, :line
  end

  # ========== Edge Cases ==========

  def test_parser_range_zero_to_n
    source = <<~MLC
      fn test(n: i32) -> i32 = 0..n
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.first
    body = func.body

    assert_instance_of MLC::Source::AST::RangeExpr, body
    assert_equal 0, body.start.value
    assert_equal "n", body.end_expr.name
  end

  def test_parser_negative_range
    source = <<~MLC
      fn test() -> i32 = -5..5
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.first
    body = func.body

    assert_instance_of MLC::Source::AST::RangeExpr, body
    # -5 should be parsed as UnaryOp(-, 5)
    assert_instance_of MLC::Source::AST::UnaryOp, body.start
    assert_equal "-", body.start.op
  end

  def test_parser_range_in_parentheses
    source = <<~MLC
      fn test() -> i32 = (1..10)
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.first
    body = func.body

    # Parentheses should unwrap, leaving RangeExpr
    assert_instance_of MLC::Source::AST::RangeExpr, body
  end
end
