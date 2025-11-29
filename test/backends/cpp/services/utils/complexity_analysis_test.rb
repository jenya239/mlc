# frozen_string_literal: true

require_relative "../../../../test_helper"

class ComplexityAnalysisTest < Minitest::Test
  def setup
    @analysis = MLC::Backends::Cpp::Services::Utils::ComplexityAnalysis
    @int_type = MLC::SemanticIR::Builder.primitive_type("i32")
    @bool_type = MLC::SemanticIR::Builder.primitive_type("bool")
  end

  # === statement_has_control_flow? ===

  def test_if_stmt_has_control_flow
    stmt = MLC::SemanticIR::IfStmt.new(
      condition: MLC::SemanticIR::Builder.var_expr("x", @bool_type),
      then_body: [],
      else_body: nil
    )
    assert @analysis.statement_has_control_flow?(stmt)
  end

  def test_for_stmt_has_control_flow
    array_type = MLC::SemanticIR::Builder.array_type(@int_type)
    stmt = MLC::SemanticIR::ForStmt.new(
      var_name: "i",
      var_type: @int_type,
      iterable: MLC::SemanticIR::Builder.var_expr("items", array_type),
      body: []
    )
    assert @analysis.statement_has_control_flow?(stmt)
  end

  def test_variable_decl_without_value_no_control_flow
    stmt = MLC::SemanticIR::VariableDeclStmt.new(
      name: "x",
      value: nil,
      type: @int_type,
      mutable: false
    )
    refute @analysis.statement_has_control_flow?(stmt)
  end

  def test_variable_decl_with_simple_value_no_control_flow
    stmt = MLC::SemanticIR::VariableDeclStmt.new(
      name: "x",
      value: MLC::SemanticIR::Builder.literal(42, @int_type),
      type: @int_type,
      mutable: false
    )
    refute @analysis.statement_has_control_flow?(stmt)
  end

  def test_expr_statement_no_control_flow
    stmt = MLC::SemanticIR::ExprStatement.new(
      expression: MLC::SemanticIR::Builder.var_expr("x", @int_type)
    )
    refute @analysis.statement_has_control_flow?(stmt)
  end

  # === expression_has_control_flow? ===

  def test_if_expr_has_control_flow
    expr = MLC::SemanticIR::IfExpr.new(
      condition: MLC::SemanticIR::Builder.var_expr("x", @bool_type),
      then_branch: MLC::SemanticIR::Builder.literal(1, @int_type),
      else_branch: MLC::SemanticIR::Builder.literal(2, @int_type),
      type: @int_type
    )
    assert @analysis.expression_has_control_flow?(expr)
  end

  def test_block_expr_has_control_flow
    expr = MLC::SemanticIR::BlockExpr.new(
      statements: [],
      result: MLC::SemanticIR::Builder.literal(1, @int_type),
      type: @int_type
    )
    assert @analysis.expression_has_control_flow?(expr)
  end

  def test_literal_no_control_flow
    expr = MLC::SemanticIR::Builder.literal(42, @int_type)
    refute @analysis.expression_has_control_flow?(expr)
  end

  def test_var_expr_no_control_flow
    expr = MLC::SemanticIR::Builder.var_expr("x", @int_type)
    refute @analysis.expression_has_control_flow?(expr)
  end

  def test_binary_expr_no_control_flow
    left = MLC::SemanticIR::Builder.literal(1, @int_type)
    right = MLC::SemanticIR::Builder.literal(2, @int_type)
    expr = MLC::SemanticIR::BinaryExpr.new(op: "+", left: left, right: right, type: @int_type)
    refute @analysis.expression_has_control_flow?(expr)
  end

  def test_call_with_simple_args_no_control_flow
    arg = MLC::SemanticIR::Builder.var_expr("x", @int_type)
    callee = MLC::SemanticIR::Builder.var_expr("foo", @int_type)
    expr = MLC::SemanticIR::CallExpr.new(callee: callee, args: [arg], type: @int_type)
    refute @analysis.expression_has_control_flow?(expr)
  end

  # === simple_statement? ===

  def test_assignment_is_simple
    stmt = MLC::SemanticIR::AssignmentStmt.new(
      target: MLC::SemanticIR::Builder.var_expr("x", @int_type),
      value: MLC::SemanticIR::Builder.literal(1, @int_type)
    )
    assert @analysis.simple_statement?(stmt)
  end

  def test_variable_decl_without_value_is_simple
    stmt = MLC::SemanticIR::VariableDeclStmt.new(
      name: "x",
      value: nil,
      type: @int_type,
      mutable: false
    )
    assert @analysis.simple_statement?(stmt)
  end

  # === all_statements_simple? ===

  def test_empty_statements_all_simple
    assert @analysis.all_statements_simple?([])
  end

  def test_single_simple_statement_all_simple
    stmt = MLC::SemanticIR::AssignmentStmt.new(
      target: MLC::SemanticIR::Builder.var_expr("x", @int_type),
      value: MLC::SemanticIR::Builder.literal(1, @int_type)
    )
    assert @analysis.all_statements_simple?([stmt])
  end

  # === has_nested_blocks? ===

  def test_no_nested_blocks
    stmt = MLC::SemanticIR::VariableDeclStmt.new(
      name: "x",
      value: MLC::SemanticIR::Builder.literal(1, @int_type),
      type: @int_type,
      mutable: false
    )
    refute @analysis.has_nested_blocks?([stmt])
  end

  def test_has_nested_block
    block = MLC::SemanticIR::BlockExpr.new(
      statements: [],
      result: MLC::SemanticIR::Builder.literal(1, @int_type),
      type: @int_type
    )
    stmt = MLC::SemanticIR::VariableDeclStmt.new(
      name: "x",
      value: block,
      type: @int_type,
      mutable: false
    )
    assert @analysis.has_nested_blocks?([stmt])
  end

  # === simple_branch? ===

  def test_literal_is_simple_branch
    expr = MLC::SemanticIR::Builder.literal(42, @int_type)
    assert @analysis.simple_branch?(expr)
  end

  def test_var_expr_is_simple_branch
    expr = MLC::SemanticIR::Builder.var_expr("x", @int_type)
    assert @analysis.simple_branch?(expr)
  end

  def test_nil_is_not_simple_branch
    refute @analysis.simple_branch?(nil)
  end

  def test_binary_with_simple_operands_is_simple_branch
    left = MLC::SemanticIR::Builder.literal(1, @int_type)
    right = MLC::SemanticIR::Builder.var_expr("x", @int_type)
    expr = MLC::SemanticIR::BinaryExpr.new(op: "+", left: left, right: right, type: @int_type)
    assert @analysis.simple_branch?(expr)
  end

  # === branches_have_nested_control_flow? ===

  def test_empty_branches_no_nested_control_flow
    refute @analysis.branches_have_nested_control_flow?([])
  end

  def test_simple_branches_no_nested_control_flow
    branches = [
      MLC::SemanticIR::Builder.literal(1, @int_type),
      MLC::SemanticIR::Builder.var_expr("x", @int_type)
    ]
    refute @analysis.branches_have_nested_control_flow?(branches)
  end

  def test_if_expr_in_branches_has_nested_control_flow
    if_expr = MLC::SemanticIR::IfExpr.new(
      condition: MLC::SemanticIR::Builder.var_expr("x", @bool_type),
      then_branch: MLC::SemanticIR::Builder.literal(1, @int_type),
      else_branch: MLC::SemanticIR::Builder.literal(2, @int_type),
      type: @int_type
    )
    assert @analysis.branches_have_nested_control_flow?([if_expr])
  end

  # === Pattern helpers ===

  def test_constructor_pattern
    assert @analysis.constructor_pattern?({ kind: :constructor })
    assert @analysis.constructor_pattern?({ kind: :wildcard })
    assert @analysis.constructor_pattern?({ kind: :var })
  end

  def test_non_constructor_pattern
    refute @analysis.constructor_pattern?({ kind: :regex })
    refute @analysis.constructor_pattern?({ kind: :literal })
  end

  def test_regex_pattern
    assert @analysis.regex_pattern?({ kind: :regex })
    refute @analysis.regex_pattern?({ kind: :constructor })
  end
end
