# frozen_string_literal: true

require "test_helper"

# TDD tests for new block syntax (RFC_BLOCK_SYNTAX.md)
# These tests define the expected behavior BEFORE implementation
class BlockSyntaxTest < Minitest::Test
  def setup
    @parser = MLC::Source::Parser::Parser
  end

  # ===========================================
  # If/Unless with end
  # ===========================================

  def test_if_inline_requires_then
    source = "fn foo(x: i32) -> i32 = if x > 0 then x else -x end"
    ast = @parser.new(source).parse

    func = ast.declarations.first
    assert_instance_of MLC::Source::AST::IfExpr, func.body
  end

  def test_if_multiline_then_optional
    source = <<~MLC
      fn foo(x: i32) -> i32 =
        if x > 0
          x
        else
          -x
        end
    MLC
    ast = @parser.new(source).parse

    func = ast.declarations.first
    assert_instance_of MLC::Source::AST::IfExpr, func.body
  end

  def test_if_multiline_with_then
    source = <<~MLC
      fn foo(x: i32) -> i32 =
        if x > 0 then
          x
        else
          -x
        end
    MLC
    ast = @parser.new(source).parse

    func = ast.declarations.first
    assert_instance_of MLC::Source::AST::IfExpr, func.body
  end

  def test_unless_inline
    source = "fn foo(x: i32) -> i32 = unless x == 0 then 1 else 0 end"
    ast = @parser.new(source).parse

    func = ast.declarations.first
    # Unless is syntactic sugar for if with negated condition
    assert_instance_of MLC::Source::AST::IfExpr, func.body
  end

  def test_unless_multiline
    source = <<~MLC
      fn foo(x: i32) -> i32 =
        unless x == 0
          1
        else
          0
        end
    MLC
    ast = @parser.new(source).parse

    func = ast.declarations.first
    assert_instance_of MLC::Source::AST::IfExpr, func.body
  end

  # ===========================================
  # Postfix if/unless
  # ===========================================

  def test_postfix_if
    source = "fn foo(x: i32) -> i32 = x * 2 if x > 0"
    ast = @parser.new(source).parse

    func = ast.declarations.first
    assert_instance_of MLC::Source::AST::IfExpr, func.body
    # else branch should be unit/nil
  end

  def test_postfix_unless
    source = "fn foo(x: bool) -> i32 = 0 unless x"
    ast = @parser.new(source).parse

    func = ast.declarations.first
    assert_instance_of MLC::Source::AST::IfExpr, func.body
  end

  # ===========================================
  # Unsafe with end
  # ===========================================

  def test_unsafe_with_end
    source = <<~MLC
      fn foo(x: i32) -> i32 = unsafe
        let y = x + 1
        y
      end
    MLC
    ast = @parser.new(source).parse

    func = ast.declarations.first
    assert_instance_of MLC::Source::AST::UnsafeBlock, func.body
  end

  def test_unsafe_inline
    source = "fn foo(x: i32) -> i32 = unsafe x + 1 end"
    ast = @parser.new(source).parse

    func = ast.declarations.first
    assert_instance_of MLC::Source::AST::UnsafeBlock, func.body
  end

  # ===========================================
  # Match with pipe and end
  # ===========================================

  def test_match_with_pipe_syntax
    source = <<~MLC
      fn foo(x: Option<i32>) -> i32 =
        match x
        | Some(v) => v
        | None => 0
        end
    MLC
    ast = @parser.new(source).parse

    func = ast.declarations.first
    assert_instance_of MLC::Source::AST::MatchExpr, func.body
    assert_equal 2, func.body.arms.length
  end

  def test_match_inline
    source = "fn foo(x: bool) -> i32 = match x | true => 1 | false => 0 end"
    ast = @parser.new(source).parse

    func = ast.declarations.first
    assert_instance_of MLC::Source::AST::MatchExpr, func.body
  end

  # ===========================================
  # Do block (already works, verify still works)
  # ===========================================

  def test_do_block_still_works
    source = <<~MLC
      fn foo() -> i32 = do
        let x = 10
        let y = 20
        x + y
      end
    MLC
    ast = @parser.new(source).parse

    func = ast.declarations.first
    assert_instance_of MLC::Source::AST::BlockExpr, func.body
  end

  # ===========================================
  # Record literals still use { }
  # ===========================================

  def test_record_literal_still_uses_braces
    source = "fn foo() -> Point = { x: 1, y: 2 }"
    ast = @parser.new(source).parse

    func = ast.declarations.first
    assert_instance_of MLC::Source::AST::RecordLit, func.body
  end

  # ===========================================
  # Lambda syntax
  # ===========================================

  def test_lambda_single_expression
    source = "fn foo() -> fn(i32) -> i32 = (x) => x * 2"
    ast = @parser.new(source).parse

    func = ast.declarations.first
    assert_instance_of MLC::Source::AST::Lambda, func.body
  end

  def test_lambda_with_do_block
    source = <<~MLC
      fn foo() -> fn(i32) -> i32 = (x) => do
        let y = x * 2
        y + 1
      end
    MLC
    ast = @parser.new(source).parse

    func = ast.declarations.first
    assert_instance_of MLC::Source::AST::Lambda, func.body
    assert_instance_of MLC::Source::AST::BlockExpr, func.body.body
  end

  # ===========================================
  # While/For (already use do...end)
  # ===========================================

  def test_while_loop_syntax
    source = <<~MLC
      fn foo() -> i32 = do
        let mut i = 0
        while i < 10 do
          i = i + 1
        end
        i
      end
    MLC
    ast = @parser.new(source).parse

    func = ast.declarations.first
    assert_instance_of MLC::Source::AST::BlockExpr, func.body
  end

  def test_for_loop_syntax
    source = <<~MLC
      fn sum(arr: i32[]) -> i32 = do
        let mut total = 0
        for x in arr do
          total = total + x
        end
        total
      end
    MLC
    ast = @parser.new(source).parse

    func = ast.declarations.first
    assert_instance_of MLC::Source::AST::BlockExpr, func.body
  end
end
