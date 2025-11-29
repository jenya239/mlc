# frozen_string_literal: true

require_relative "../test_helper"

class SelfAccessTest < Minitest::Test
  def test_lexer_tokenizes_self_access
    lexer = MLC::Source::Parser::Lexer.new("@field @x @value123")
    tokens = lexer.tokenize

    # Filter out EOF
    tokens = tokens.reject { |t| t.type == :EOF }

    assert_equal 3, tokens.size
    assert_equal :SELF_ACCESS, tokens[0].type
    assert_equal "field", tokens[0].value

    assert_equal :SELF_ACCESS, tokens[1].type
    assert_equal "x", tokens[1].value

    assert_equal :SELF_ACCESS, tokens[2].type
    assert_equal "value123", tokens[2].value
  end

  def test_parser_parses_self_access_as_member_access
    code = "fn test() -> i32 = @field"
    ast = MLC.parse(code)
    func = ast.declarations.first
    body = func.body

    # @field should be parsed as MemberAccess(self, "field")
    assert_instance_of MLC::Source::AST::MemberAccess, body
    assert_instance_of MLC::Source::AST::VarRef, body.object
    assert_equal "self", body.object.name
    assert_equal "field", body.member
  end

  def test_parser_parses_self_access_in_expression
    code = "fn test() -> i32 = @x + @y"
    ast = MLC.parse(code)
    func = ast.declarations.first
    body = func.body

    assert_instance_of MLC::Source::AST::BinaryOp, body
    assert_equal "+", body.op

    # Left: @x -> self.x
    assert_instance_of MLC::Source::AST::MemberAccess, body.left
    assert_equal "self", body.left.object.name
    assert_equal "x", body.left.member

    # Right: @y -> self.y
    assert_instance_of MLC::Source::AST::MemberAccess, body.right
    assert_equal "self", body.right.object.name
    assert_equal "y", body.right.member
  end

  def test_parser_parses_self_access_with_method_call
    code = "fn test() -> i32 = @values.len()"
    ast = MLC.parse(code)
    func = ast.declarations.first
    body = func.body

    # @values.len() -> Call(MemberAccess(MemberAccess(self, "values"), "len"), [])
    assert_instance_of MLC::Source::AST::Call, body

    callee = body.callee
    assert_instance_of MLC::Source::AST::MemberAccess, callee
    assert_equal "len", callee.member

    # The object is @values -> self.values
    inner = callee.object
    assert_instance_of MLC::Source::AST::MemberAccess, inner
    assert_equal "self", inner.object.name
    assert_equal "values", inner.member
  end

  def test_parser_parses_self_access_in_record_literal
    code = <<~MLC
      type Point = { x: i32, y: i32 }
      fn test() -> Point = Point { x: @x, y: @y }
    MLC
    ast = MLC.parse(code)
    func = ast.declarations[1]
    body = func.body

    assert_instance_of MLC::Source::AST::RecordLit, body
    assert_equal "Point", body.type_name

    # x field: @x -> self.x
    x_value = body.fields["x"]
    assert_instance_of MLC::Source::AST::MemberAccess, x_value
    assert_equal "self", x_value.object.name
    assert_equal "x", x_value.member

    # y field: @y -> self.y
    y_value = body.fields["y"]
    assert_instance_of MLC::Source::AST::MemberAccess, y_value
    assert_equal "self", y_value.object.name
    assert_equal "y", y_value.member
  end

  def test_bare_at_is_skipped
    # Bare @ without identifier should be silently skipped
    lexer = MLC::Source::Parser::Lexer.new("@ 123")
    tokens = lexer.tokenize

    # Filter out EOF
    tokens = tokens.reject { |t| t.type == :EOF }

    # Should only have the int literal
    assert_equal 1, tokens.size
    assert_equal :INT_LITERAL, tokens[0].type
  end
end
