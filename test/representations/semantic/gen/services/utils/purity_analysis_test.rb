# frozen_string_literal: true

require_relative "../../../../../test_helper"

class PurityAnalysisTest < Minitest::Test
  def setup
    @purity = MLC::Representations::Semantic::Gen::Services::Utils::PurityAnalysis
    @int_type = MLC::SemanticIR::Builder.primitive_type("i32")
    @string_type = MLC::SemanticIR::Builder.primitive_type("string")
  end

  # === pure_expression? - literals and vars ===

  def test_literal_is_pure
    expr = MLC::SemanticIR::Builder.literal(42, @int_type)
    assert @purity.pure_expression?(expr)
  end

  def test_var_is_pure
    expr = MLC::SemanticIR::Builder.var_expr("x", @int_type)
    assert @purity.pure_expression?(expr)
  end

  # === pure_expression? - binary ===

  def test_binary_with_pure_operands_is_pure
    left = MLC::SemanticIR::Builder.literal(1, @int_type)
    right = MLC::SemanticIR::Builder.literal(2, @int_type)
    expr = MLC::SemanticIR::BinaryExpr.new(op: "+", left: left, right: right, type: @int_type)
    assert @purity.pure_expression?(expr)
  end

  # === pure_expression? - unary ===

  def test_unary_with_pure_operand_is_pure
    operand = MLC::SemanticIR::Builder.literal(1, @int_type)
    expr = MLC::SemanticIR::UnaryExpr.new(op: "-", operand: operand, type: @int_type)
    assert @purity.pure_expression?(expr)
  end

  # === non_literal_type? ===

  def test_string_is_non_literal
    assert @purity.non_literal_type?(@string_type)
  end

  def test_int_is_literal
    refute @purity.non_literal_type?(@int_type)
  end

  def test_array_type_is_non_literal
    array_type = MLC::SemanticIR::Builder.primitive_type("Array")
    assert @purity.non_literal_type?(array_type)
  end

  def test_vec_type_is_non_literal
    vec_type = MLC::SemanticIR::Builder.primitive_type("Vec")
    assert @purity.non_literal_type?(vec_type)
  end

  def test_nil_type_is_literal
    refute @purity.non_literal_type?(nil)
  end

  # === pure_call? ===

  def test_call_to_println_is_not_pure
    callee = MLC::SemanticIR::Builder.var_expr("println", @int_type)
    arg = MLC::SemanticIR::Builder.literal("hello", @string_type)
    expr = MLC::SemanticIR::CallExpr.new(callee: callee, args: [arg], type: @int_type)
    refute @purity.pure_call?(expr)
  end

  def test_call_to_print_is_not_pure
    callee = MLC::SemanticIR::Builder.var_expr("print", @int_type)
    arg = MLC::SemanticIR::Builder.literal("hello", @string_type)
    expr = MLC::SemanticIR::CallExpr.new(callee: callee, args: [arg], type: @int_type)
    refute @purity.pure_call?(expr)
  end

  def test_call_to_to_string_is_not_pure
    callee = MLC::SemanticIR::Builder.var_expr("to_string", @string_type)
    arg = MLC::SemanticIR::Builder.literal(42, @int_type)
    expr = MLC::SemanticIR::CallExpr.new(callee: callee, args: [arg], type: @string_type)
    refute @purity.pure_call?(expr)
  end

  def test_call_returning_string_is_not_pure
    callee = MLC::SemanticIR::Builder.var_expr("get_name", @string_type)
    expr = MLC::SemanticIR::CallExpr.new(callee: callee, args: [], type: @string_type)
    refute @purity.pure_call?(expr)
  end

  def test_pure_call_with_pure_args_is_pure
    callee = MLC::SemanticIR::Builder.var_expr("add", @int_type)
    arg1 = MLC::SemanticIR::Builder.literal(1, @int_type)
    arg2 = MLC::SemanticIR::Builder.literal(2, @int_type)
    expr = MLC::SemanticIR::CallExpr.new(callee: callee, args: [arg1, arg2], type: @int_type)
    assert @purity.pure_call?(expr)
  end

  # === pure_statement? ===

  def test_immutable_var_decl_is_pure
    stmt = MLC::SemanticIR::VariableDeclStmt.new(
      name: "x",
      value: MLC::SemanticIR::Builder.literal(42, @int_type),
      type: @int_type,
      mutable: false
    )
    assert @purity.pure_statement?(stmt)
  end

  def test_mutable_var_decl_is_not_pure
    stmt = MLC::SemanticIR::VariableDeclStmt.new(
      name: "x",
      value: MLC::SemanticIR::Builder.literal(42, @int_type),
      type: @int_type,
      mutable: true
    )
    refute @purity.pure_statement?(stmt)
  end

  def test_expr_statement_with_pure_expr_is_pure
    stmt = MLC::SemanticIR::ExprStatement.new(
      expression: MLC::SemanticIR::Builder.literal(42, @int_type)
    )
    assert @purity.pure_statement?(stmt)
  end

  # === pure_block? ===

  def test_empty_block_is_pure
    block = MLC::SemanticIR::BlockExpr.new(
      statements: [],
      result: nil,
      type: @int_type
    )
    assert @purity.pure_block?(block)
  end

  def test_block_with_pure_statements_is_pure
    stmt = MLC::SemanticIR::VariableDeclStmt.new(
      name: "x",
      value: MLC::SemanticIR::Builder.literal(42, @int_type),
      type: @int_type,
      mutable: false
    )
    result = MLC::SemanticIR::Builder.var_expr("x", @int_type)
    block = MLC::SemanticIR::BlockExpr.new(
      statements: [stmt],
      result: result,
      type: @int_type
    )
    assert @purity.pure_block?(block)
  end

  def test_block_with_mutable_var_is_not_pure
    stmt = MLC::SemanticIR::VariableDeclStmt.new(
      name: "x",
      value: MLC::SemanticIR::Builder.literal(42, @int_type),
      type: @int_type,
      mutable: true
    )
    block = MLC::SemanticIR::BlockExpr.new(
      statements: [stmt],
      result: MLC::SemanticIR::Builder.var_expr("x", @int_type),
      type: @int_type
    )
    refute @purity.pure_block?(block)
  end
end
