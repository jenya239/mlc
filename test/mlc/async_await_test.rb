# frozen_string_literal: true

require_relative "../test_helper"

class AsyncAwaitTest < Minitest::Test
  # ========== Lexer Tests ==========

  def test_lexer_tokenizes_async_keyword
    lexer = MLC::Source::Parser::Lexer.new("async fn")
    tokens = lexer.tokenize

    assert_equal 3, tokens.size
    assert_equal :ASYNC, tokens[0].type
    assert_equal "async", tokens[0].value
    assert_equal :FN, tokens[1].type
    assert_equal :EOF, tokens[2].type
  end

  def test_lexer_tokenizes_await_keyword
    lexer = MLC::Source::Parser::Lexer.new("await x")
    tokens = lexer.tokenize

    assert_equal 3, tokens.size
    assert_equal :AWAIT, tokens[0].type
    assert_equal "await", tokens[0].value
    assert_equal :IDENTIFIER, tokens[1].type
    assert_equal "x", tokens[1].value
    assert_equal :EOF, tokens[2].type
  end

  # ========== Parser Tests (async fn) ==========

  def test_parser_parses_async_function
    code = "async fn fetch_data() -> i32 = 42"
    ast = parse_program(code)

    func = ast.declarations.first
    assert_instance_of MLC::Source::AST::FuncDecl, func
    assert_equal "fetch_data", func.name
    assert func.is_async, "Function should be marked as async"
  end

  def test_parser_parses_export_async_function
    code = "export async fn fetch() -> i32 = 42"
    ast = parse_program(code)

    func = ast.declarations.first
    assert_instance_of MLC::Source::AST::FuncDecl, func
    assert func.is_async, "Function should be marked as async"
    assert func.exported, "Function should be exported"
  end

  def test_parser_parses_non_async_function_has_is_async_false
    code = "fn regular() -> i32 = 42"
    ast = parse_program(code)

    func = ast.declarations.first
    assert_instance_of MLC::Source::AST::FuncDecl, func
    refute func.is_async, "Regular function should not be async"
  end

  # ========== Parser Tests (await expr) ==========

  def test_parser_parses_await_expr
    code = "async fn test() -> i32 = await future"
    ast = parse_program(code)

    body = ast.declarations.first.body
    assert_instance_of MLC::Source::AST::AwaitExpr, body
    assert_instance_of MLC::Source::AST::VarRef, body.operand
    assert_equal "future", body.operand.name
  end

  def test_parser_parses_await_call
    code = "async fn test() -> i32 = await fetch_data()"
    ast = parse_program(code)

    body = ast.declarations.first.body
    assert_instance_of MLC::Source::AST::AwaitExpr, body
    assert_instance_of MLC::Source::AST::Call, body.operand
    # Call has callee (VarRef) instead of name
    assert_equal "fetch_data", body.operand.callee.name
  end

  def test_parser_parses_chained_await
    # await await x means await (await x)
    code = "async fn test() -> i32 = await await future"
    ast = parse_program(code)

    body = ast.declarations.first.body
    assert_instance_of MLC::Source::AST::AwaitExpr, body

    inner = body.operand
    assert_instance_of MLC::Source::AST::AwaitExpr, inner
    assert_instance_of MLC::Source::AST::VarRef, inner.operand
  end

  def test_parser_parses_await_with_arithmetic
    # await x + 1 means (await x) + 1
    code = "async fn test() -> i32 = await x + 1"
    ast = parse_program(code)

    body = ast.declarations.first.body
    assert_instance_of MLC::Source::AST::BinaryOp, body
    assert_equal "+", body.op

    left = body.left
    assert_instance_of MLC::Source::AST::AwaitExpr, left
  end

  private

  def parse_program(code)
    parser = MLC::Source::Parser::Parser.new(code)
    parser.parse
  end
end
