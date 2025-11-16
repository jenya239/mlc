# frozen_string_literal: true

require_relative "../test_helper"
require_relative "../../lib/mlc/common/index"

class MLCBlockExprTest < Minitest::Test
  def test_block_expr_creation
    # Test creating BlockExpr with statements and result_expr
    stmt1 = MLC::Source::AST::VariableDecl.new(
      name: "x",
      value: MLC::Source::AST::IntLit.new(value: 1),
      mutable: false
    )

    stmt2 = MLC::Source::AST::VariableDecl.new(
      name: "y",
      value: MLC::Source::AST::IntLit.new(value: 2),
      mutable: false
    )

    result = MLC::Source::AST::BinaryOp.new(
      op: "+",
      left: MLC::Source::AST::VarRef.new(name: "x"),
      right: MLC::Source::AST::VarRef.new(name: "y")
    )

    block = MLC::Source::AST::BlockExpr.new(
      statements: [stmt1, stmt2],
      result_expr: result
    )

    assert_equal 2, block.statements.length
    assert_instance_of MLC::Source::AST::VariableDecl, block.statements[0]
    assert_instance_of MLC::Source::AST::VariableDecl, block.statements[1]
    assert_instance_of MLC::Source::AST::BinaryOp, block.result_expr
    assert_equal :block_expr, block.kind
  end

  def test_block_expr_with_mutable_variables
    # Test BlockExpr with mutable variable and assignment
    var_decl = MLC::Source::AST::VariableDecl.new(
      name: "x",
      value: MLC::Source::AST::IntLit.new(value: 0),
      mutable: true
    )

    assignment = MLC::Source::AST::Assignment.new(
      target: MLC::Source::AST::VarRef.new(name: "x"),
      value: MLC::Source::AST::BinaryOp.new(
        op: "+",
        left: MLC::Source::AST::VarRef.new(name: "x"),
        right: MLC::Source::AST::IntLit.new(value: 1)
      )
    )

    result = MLC::Source::AST::VarRef.new(name: "x")

    block = MLC::Source::AST::BlockExpr.new(
      statements: [var_decl, assignment],
      result_expr: result
    )

    assert_equal 2, block.statements.length
    assert_instance_of MLC::Source::AST::VariableDecl, block.statements[0]
    assert block.statements[0].mutable
    assert_instance_of MLC::Source::AST::Assignment, block.statements[1]
    assert_instance_of MLC::Source::AST::VarRef, block.result_expr
  end

  def test_block_expr_with_expr_statements
    # Test BlockExpr with expression statements
    call_expr = MLC::Source::AST::Call.new(
      callee: MLC::Source::AST::VarRef.new(name: "println"),
      args: [MLC::Source::AST::StringLit.new(value: "Hello")]
    )

    expr_stmt = MLC::Source::AST::ExprStmt.new(expr: call_expr)

    result = MLC::Source::AST::IntLit.new(value: 0)

    block = MLC::Source::AST::BlockExpr.new(
      statements: [expr_stmt],
      result_expr: result
    )

    assert_equal 1, block.statements.length
    assert_instance_of MLC::Source::AST::ExprStmt, block.statements[0]
    assert_instance_of MLC::Source::AST::IntLit, block.result_expr
  end

  def test_block_expr_empty_statements
    # Test BlockExpr with no statements, only result
    result = MLC::Source::AST::IntLit.new(value: 42)

    block = MLC::Source::AST::BlockExpr.new(
      statements: [],
      result_expr: result
    )

    assert_equal 0, block.statements.length
    assert_instance_of MLC::Source::AST::IntLit, block.result_expr
    assert_equal 42, block.result_expr.value
  end

  def test_block_expr_nested_blocks
    # Test nested BlockExpr
    inner_block = MLC::Source::AST::BlockExpr.new(
      statements: [
        MLC::Source::AST::VariableDecl.new(
          name: "y",
          value: MLC::Source::AST::IntLit.new(value: 1),
          mutable: false
        )
      ],
      result_expr: MLC::Source::AST::BinaryOp.new(
        op: "+",
        left: MLC::Source::AST::VarRef.new(name: "y"),
        right: MLC::Source::AST::IntLit.new(value: 1)
      )
    )

    outer_block = MLC::Source::AST::BlockExpr.new(
      statements: [
        MLC::Source::AST::VariableDecl.new(
          name: "x",
          value: inner_block,
          mutable: false
        )
      ],
      result_expr: MLC::Source::AST::BinaryOp.new(
        op: "*",
        left: MLC::Source::AST::VarRef.new(name: "x"),
        right: MLC::Source::AST::IntLit.new(value: 2)
      )
    )

    assert_equal 1, outer_block.statements.length
    assert_instance_of MLC::Source::AST::VariableDecl, outer_block.statements[0]
    assert_instance_of MLC::Source::AST::BlockExpr, outer_block.statements[0].value
    assert_instance_of MLC::Source::AST::BinaryOp, outer_block.result_expr
  end

  def test_block_expr_origin_tracking
    # Test that origin information is preserved
    origin = MLC::SourceOrigin.new(
      file: "test.mlc",
      line: 10,
      column: 5
    )

    block = MLC::Source::AST::BlockExpr.new(
      statements: [],
      result_expr: MLC::Source::AST::IntLit.new(value: 0),
      origin: origin
    )

    assert_equal origin, block.origin
    assert_equal "test.mlc", block.origin.file
    assert_equal 10, block.origin.line
    assert_equal 5, block.origin.column
  end
end
