# frozen_string_literal: true

require_relative "../test_helper"

class TryOperatorTest < Minitest::Test
  # ========== Lexer Tests ==========

  def test_lexer_tokenizes_question_operator
    lexer = MLC::Source::Parser::Lexer.new("x?")
    tokens = lexer.tokenize

    assert_equal 3, tokens.size
    assert_equal :IDENTIFIER, tokens[0].type
    assert_equal "x", tokens[0].value
    assert_equal :QUESTION, tokens[1].type
    assert_equal "?", tokens[1].value
    assert_equal :EOF, tokens[2].type
  end

  def test_lexer_distinguishes_question_from_safe_nav
    lexer = MLC::Source::Parser::Lexer.new("x?.y")
    tokens = lexer.tokenize

    assert_equal 4, tokens.size
    assert_equal :IDENTIFIER, tokens[0].type
    assert_equal :SAFE_NAV, tokens[1].type
    assert_equal "?.", tokens[1].value
    assert_equal :IDENTIFIER, tokens[2].type
    assert_equal "y", tokens[2].value
  end

  def test_lexer_tokenizes_chained_question
    lexer = MLC::Source::Parser::Lexer.new("a?.b?")
    tokens = lexer.tokenize

    assert_equal 5, tokens.size
    assert_equal :IDENTIFIER, tokens[0].type
    assert_equal :SAFE_NAV, tokens[1].type
    assert_equal :IDENTIFIER, tokens[2].type
    assert_equal :QUESTION, tokens[3].type
    assert_equal :EOF, tokens[4].type
  end

  # ========== Parser Tests ==========

  def test_parser_parses_try_expr
    code = "fn test() -> i32 = x?"
    ast = parse_program(code)

    body = ast.declarations.first.body
    assert_instance_of MLC::Source::AST::TryExpr, body
    assert_instance_of MLC::Source::AST::VarRef, body.operand
    assert_equal "x", body.operand.name
  end

  def test_parser_parses_try_after_call
    code = "fn test() -> i32 = get_value()?"
    ast = parse_program(code)

    body = ast.declarations.first.body
    assert_instance_of MLC::Source::AST::TryExpr, body
    assert_instance_of MLC::Source::AST::Call, body.operand
  end

  def test_parser_parses_try_after_member_access
    code = "fn test() -> i32 = x.value?"
    ast = parse_program(code)

    body = ast.declarations.first.body
    assert_instance_of MLC::Source::AST::TryExpr, body
    assert_instance_of MLC::Source::AST::MemberAccess, body.operand
    assert_equal "value", body.operand.member
  end

  def test_parser_parses_chained_try_and_safe_nav
    # a?.b? means (a?.b)?
    code = "fn test() -> i32 = a?.b?"
    ast = parse_program(code)

    body = ast.declarations.first.body
    assert_instance_of MLC::Source::AST::TryExpr, body

    inner = body.operand
    assert_instance_of MLC::Source::AST::SafeMemberAccess, inner
    assert_equal "b", inner.member
  end

  def test_parser_parses_try_after_index
    code = "fn test() -> i32 = arr[0]?"
    ast = parse_program(code)

    body = ast.declarations.first.body
    assert_instance_of MLC::Source::AST::TryExpr, body
    assert_instance_of MLC::Source::AST::IndexAccess, body.operand
  end

  def test_parser_parses_multiple_try
    # a?? means (a?)?
    code = "fn test() -> i32 = x??"
    ast = parse_program(code)

    body = ast.declarations.first.body
    assert_instance_of MLC::Source::AST::TryExpr, body

    inner = body.operand
    assert_instance_of MLC::Source::AST::TryExpr, inner
    assert_instance_of MLC::Source::AST::VarRef, inner.operand
  end

  private

  def parse_program(code)
    parser = MLC::Source::Parser::Parser.new(code)
    parser.parse
  end
end
