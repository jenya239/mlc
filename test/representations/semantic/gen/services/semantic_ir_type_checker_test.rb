# frozen_string_literal: true

require_relative "../../../../test_helper"

# Explicitly require the service
require_relative "../../../../../lib/mlc/representations/semantic/gen/services/semantic_ir_type_checker"

class SemanticIRTypeCheckerTest < Minitest::Test
  def setup
    @checker = MLC::Representations::Semantic::Gen::Services::SemanticIRTypeChecker.new
  end

  # ========== Expression node predicates ==========

  def test_literal_expr
    expr = MLC::SemanticIR::LiteralExpr.new(value: 42, type: int_type)

    assert @checker.literal_expr?(expr)
    refute @checker.var_expr?(expr)
    refute @checker.binary_expr?(expr)
  end

  def test_var_expr
    expr = MLC::SemanticIR::VarExpr.new(name: "x", type: int_type)

    assert @checker.var_expr?(expr)
    refute @checker.literal_expr?(expr)
  end

  def test_binary_expr
    left = MLC::SemanticIR::LiteralExpr.new(value: 1, type: int_type)
    right = MLC::SemanticIR::LiteralExpr.new(value: 2, type: int_type)
    expr = MLC::SemanticIR::BinaryExpr.new(op: "+", left: left, right: right, type: int_type)

    assert @checker.binary_expr?(expr)
    refute @checker.unary_expr?(expr)
  end

  def test_unary_expr
    operand = MLC::SemanticIR::LiteralExpr.new(value: 5, type: int_type)
    expr = MLC::SemanticIR::UnaryExpr.new(op: "-", operand: operand, type: int_type)

    assert @checker.unary_expr?(expr)
    refute @checker.binary_expr?(expr)
  end

  def test_call_expr
    callee = MLC::SemanticIR::VarExpr.new(name: "foo", type: int_type)
    expr = MLC::SemanticIR::CallExpr.new(callee: callee, args: [], type: int_type)

    assert @checker.call_expr?(expr)
    refute @checker.var_expr?(expr)
  end

  def test_member_expr
    obj = MLC::SemanticIR::VarExpr.new(name: "point", type: int_type)
    expr = MLC::SemanticIR::MemberExpr.new(object: obj, member: "x", type: int_type)

    assert @checker.member_expr?(expr)
    refute @checker.index_expr?(expr)
  end

  def test_index_expr
    array = MLC::SemanticIR::VarExpr.new(name: "arr", type: int_type)
    index = MLC::SemanticIR::LiteralExpr.new(value: 0, type: int_type)
    expr = MLC::SemanticIR::IndexExpr.new(object: array, index: index, type: int_type)

    assert @checker.index_expr?(expr)
    refute @checker.slice_expr?(expr)
  end

  def test_slice_expr
    array = MLC::SemanticIR::VarExpr.new(name: "arr", type: int_type)
    start_idx = MLC::SemanticIR::LiteralExpr.new(value: 0, type: int_type)
    end_idx = MLC::SemanticIR::LiteralExpr.new(value: 5, type: int_type)
    expr = MLC::SemanticIR::SliceExpr.new(object: array, start_index: start_idx, end_index: end_idx, type: int_type)

    assert @checker.slice_expr?(expr)
    refute @checker.index_expr?(expr)
  end

  def test_array_literal_expr
    element = MLC::SemanticIR::LiteralExpr.new(value: 1, type: int_type)
    array_type = MLC::SemanticIR::ArrayType.new(element_type: int_type)
    expr = MLC::SemanticIR::ArrayLiteralExpr.new(elements: [element], type: array_type)

    assert @checker.array_literal_expr?(expr)
    refute @checker.tuple_expr?(expr)
  end

  def test_tuple_expr
    elem1 = MLC::SemanticIR::LiteralExpr.new(value: 1, type: int_type)
    elem2 = MLC::SemanticIR::LiteralExpr.new(value: "a", type: string_type)
    tuple_type = MLC::SemanticIR::TupleType.new(element_types: [int_type, string_type])
    expr = MLC::SemanticIR::TupleExpr.new(elements: [elem1, elem2], type: tuple_type)

    assert @checker.tuple_expr?(expr)
    refute @checker.array_literal_expr?(expr)
  end

  def test_symbol_expr
    expr = MLC::SemanticIR::SymbolExpr.new(name: "foo", type: int_type)

    assert @checker.symbol_expr?(expr)
  end

  def test_record_expr
    expr = MLC::SemanticIR::RecordExpr.new(type_name: "Point", fields: {}, type: int_type)

    assert @checker.record_expr?(expr)
  end

  def test_if_expr
    cond = MLC::SemanticIR::LiteralExpr.new(value: true, type: bool_type)
    then_expr = MLC::SemanticIR::LiteralExpr.new(value: 1, type: int_type)
    else_expr = MLC::SemanticIR::LiteralExpr.new(value: 2, type: int_type)
    expr = MLC::SemanticIR::IfExpr.new(condition: cond, then_branch: then_expr, else_branch: else_expr, type: int_type)

    assert @checker.if_expr?(expr)
    refute @checker.match_expr?(expr)
  end

  def test_match_expr
    scrutinee = MLC::SemanticIR::VarExpr.new(name: "x", type: int_type)
    expr = MLC::SemanticIR::MatchExpr.new(scrutinee: scrutinee, arms: [], type: int_type)

    assert @checker.match_expr?(expr)
    refute @checker.if_expr?(expr)
  end

  def test_lambda_expr
    body = MLC::SemanticIR::LiteralExpr.new(value: 42, type: int_type)
    func_type = MLC::SemanticIR::FunctionType.new(params: [], ret_type: int_type)
    expr = MLC::SemanticIR::LambdaExpr.new(captures: [], params: [], body: body, function_type: func_type)

    assert @checker.lambda_expr?(expr)
  end

  def test_block_expr
    expr = MLC::SemanticIR::BlockExpr.new(statements: [], result: nil, type: void_type)

    assert @checker.block_expr?(expr)
  end

  def test_unit_literal
    expr = MLC::SemanticIR::UnitLiteral.new

    assert @checker.unit_literal?(expr)
  end

  # ========== Statement node predicates ==========

  def test_block
    stmt = MLC::SemanticIR::Block.new(stmts: [])

    assert @checker.block?(stmt)
    refute @checker.return?(stmt)
  end

  def test_return
    value = MLC::SemanticIR::LiteralExpr.new(value: 42, type: int_type)
    stmt = MLC::SemanticIR::Return.new(expr: value)

    assert @checker.return?(stmt)
    refute @checker.block?(stmt)
  end

  def test_expr_stmt
    expr = MLC::SemanticIR::LiteralExpr.new(value: 42, type: int_type)
    stmt = MLC::SemanticIR::ExprStatement.new(expression: expr)

    assert @checker.expr_stmt?(stmt)
    refute @checker.var_decl?(stmt)
  end

  def test_var_decl
    value = MLC::SemanticIR::LiteralExpr.new(value: 42, type: int_type)
    stmt = MLC::SemanticIR::VariableDeclStmt.new(name: "x", type: int_type, value: value, mutable: false)

    assert @checker.var_decl?(stmt)
    refute @checker.assignment?(stmt)
  end

  def test_assignment
    target = MLC::SemanticIR::VarExpr.new(name: "x", type: int_type)
    value = MLC::SemanticIR::LiteralExpr.new(value: 42, type: int_type)
    stmt = MLC::SemanticIR::AssignmentStmt.new(target: target, value: value)

    assert @checker.assignment?(stmt)
    refute @checker.var_decl?(stmt)
  end

  def test_if_stmt
    cond = MLC::SemanticIR::LiteralExpr.new(value: true, type: bool_type)
    then_block = MLC::SemanticIR::Block.new(stmts: [])
    stmt = MLC::SemanticIR::IfStmt.new(condition: cond, then_body: then_block, else_body: nil)

    assert @checker.if_stmt?(stmt)
    refute @checker.while_stmt?(stmt)
  end

  def test_while_stmt
    cond = MLC::SemanticIR::LiteralExpr.new(value: true, type: bool_type)
    body = MLC::SemanticIR::Block.new(stmts: [])
    stmt = MLC::SemanticIR::WhileStmt.new(condition: cond, body: body)

    assert @checker.while_stmt?(stmt)
    refute @checker.for_stmt?(stmt)
  end

  def test_for_stmt
    iterable = MLC::SemanticIR::VarExpr.new(name: "arr", type: int_type)
    body = MLC::SemanticIR::Block.new(stmts: [])
    stmt = MLC::SemanticIR::ForStmt.new(var_name: "x", var_type: int_type, iterable: iterable, body: body)

    assert @checker.for_stmt?(stmt)
    refute @checker.while_stmt?(stmt)
  end

  def test_break_stmt
    stmt = MLC::SemanticIR::BreakStmt.new

    assert @checker.break_stmt?(stmt)
    refute @checker.continue_stmt?(stmt)
  end

  def test_continue_stmt
    stmt = MLC::SemanticIR::ContinueStmt.new

    assert @checker.continue_stmt?(stmt)
    refute @checker.break_stmt?(stmt)
  end

  def test_match_stmt
    scrutinee = MLC::SemanticIR::VarExpr.new(name: "x", type: int_type)
    stmt = MLC::SemanticIR::MatchStmt.new(scrutinee: scrutinee, arms: [])

    assert @checker.match_stmt?(stmt)
    refute @checker.if_stmt?(stmt)
  end

  # ========== Non-matching types ==========

  def test_plain_object_matches_nothing
    obj = Object.new

    refute @checker.literal_expr?(obj)
    refute @checker.var_expr?(obj)
    refute @checker.binary_expr?(obj)
    refute @checker.block?(obj)
    refute @checker.return?(obj)
  end

  private

  def int_type
    MLC::SemanticIR::Builder.primitive_type("i32")
  end

  def string_type
    MLC::SemanticIR::Builder.primitive_type("string")
  end

  def bool_type
    MLC::SemanticIR::Builder.primitive_type("bool")
  end

  def void_type
    MLC::SemanticIR::Builder.primitive_type("void")
  end
end
