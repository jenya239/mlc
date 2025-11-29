# frozen_string_literal: true

require_relative "../test_helper"

class SafeNavigationTest < Minitest::Test
  # ========== Lexer Tests ==========

  def test_lexer_tokenizes_safe_nav_operator
    lexer = MLC::Source::Parser::Lexer.new("obj?.field")
    tokens = lexer.tokenize

    assert_equal 4, tokens.size
    assert_equal :IDENTIFIER, tokens[0].type
    assert_equal "obj", tokens[0].value
    assert_equal :SAFE_NAV, tokens[1].type
    assert_equal "?.", tokens[1].value
    assert_equal :IDENTIFIER, tokens[2].type
    assert_equal "field", tokens[2].value
    assert_equal :EOF, tokens[3].type
  end

  def test_lexer_tokenizes_chained_safe_nav
    lexer = MLC::Source::Parser::Lexer.new("a?.b?.c")
    tokens = lexer.tokenize

    assert_equal 6, tokens.size
    assert_equal :IDENTIFIER, tokens[0].type
    assert_equal :SAFE_NAV, tokens[1].type
    assert_equal :IDENTIFIER, tokens[2].type
    assert_equal :SAFE_NAV, tokens[3].type
    assert_equal :IDENTIFIER, tokens[4].type
  end

  def test_lexer_tokenizes_safe_nav_method_call
    lexer = MLC::Source::Parser::Lexer.new("obj?.method()")
    tokens = lexer.tokenize

    assert_equal 6, tokens.size
    assert_equal :IDENTIFIER, tokens[0].type
    assert_equal :SAFE_NAV, tokens[1].type
    assert_equal :IDENTIFIER, tokens[2].type
    assert_equal :LPAREN, tokens[3].type
    assert_equal :RPAREN, tokens[4].type
  end

  def test_lexer_distinguishes_question_from_safe_nav
    lexer = MLC::Source::Parser::Lexer.new("x ? a : b")
    tokens = lexer.tokenize

    # ? alone (not ?.) should be handled differently
    # Currently it becomes OPERATOR
    assert_includes tokens.map(&:type), :IDENTIFIER
  end

  # ========== Parser Tests ==========

  def test_parser_parses_safe_member_access
    code = "fn test() -> i32 = x?.y"
    ast = parse_program(code)

    body = ast.declarations.first.body
    assert_instance_of MLC::Source::AST::SafeMemberAccess, body
    assert_equal "y", body.member
    assert_instance_of MLC::Source::AST::VarRef, body.object
    assert_equal "x", body.object.name
  end

  def test_parser_parses_safe_call
    code = "fn test() -> i32 = x?.foo()"
    ast = parse_program(code)

    body = ast.declarations.first.body
    assert_instance_of MLC::Source::AST::SafeCall, body
    assert_equal "foo", body.method_name
    assert_instance_of MLC::Source::AST::VarRef, body.object
    assert_equal "x", body.object.name
    assert_equal [], body.args
  end

  def test_parser_parses_safe_call_with_args
    code = "fn test() -> i32 = x?.bar(1, 2)"
    ast = parse_program(code)

    body = ast.declarations.first.body
    assert_instance_of MLC::Source::AST::SafeCall, body
    assert_equal "bar", body.method_name
    assert_equal 2, body.args.size
  end

  def test_parser_parses_chained_safe_nav
    code = "fn test() -> i32 = a?.b?.c"
    ast = parse_program(code)

    body = ast.declarations.first.body
    assert_instance_of MLC::Source::AST::SafeMemberAccess, body
    assert_equal "c", body.member

    inner = body.object
    assert_instance_of MLC::Source::AST::SafeMemberAccess, inner
    assert_equal "b", inner.member
  end

  def test_parser_parses_mixed_safe_and_regular_access
    code = "fn test() -> i32 = a.b?.c.d"
    ast = parse_program(code)

    body = ast.declarations.first.body
    # a.b?.c.d -> ((a.b)?.c).d

    assert_instance_of MLC::Source::AST::MemberAccess, body
    assert_equal "d", body.member

    middle = body.object
    assert_instance_of MLC::Source::AST::SafeMemberAccess, middle
    assert_equal "c", middle.member

    inner = middle.object
    assert_instance_of MLC::Source::AST::MemberAccess, inner
    assert_equal "b", inner.member
  end

  private

  def parse_program(code)
    parser = MLC::Source::Parser::Parser.new(code)
    parser.parse
  end
end
