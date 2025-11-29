# frozen_string_literal: true

require_relative "../../../../test_helper"

# Explicitly require the services
require_relative "../../../../../lib/mlc/representations/semantic/gen/services/purity_analyzer"
require_relative "../../../../../lib/mlc/representations/semantic/gen/services/utils/purity_analysis"

class PurityAnalyzerTest < Minitest::Test
  def setup
    @analyzer = MLC::Representations::Semantic::Gen::Services::PurityAnalyzer.new
  end

  # ========== is_pure_expression ==========

  def test_literal_is_pure
    expr = MLC::SemanticIR::LiteralExpr.new(value: 42, type: int_type)

    assert @analyzer.is_pure_expression(expr)
  end

  def test_var_expr_is_pure
    expr = MLC::SemanticIR::VarExpr.new(name: "x", type: int_type)

    assert @analyzer.is_pure_expression(expr)
  end

  def test_binary_expr_is_pure_when_operands_pure
    left = MLC::SemanticIR::LiteralExpr.new(value: 1, type: int_type)
    right = MLC::SemanticIR::LiteralExpr.new(value: 2, type: int_type)
    expr = MLC::SemanticIR::BinaryExpr.new(op: "+", left: left, right: right, type: int_type)

    assert @analyzer.is_pure_expression(expr)
  end

  def test_unary_expr_is_pure_when_operand_pure
    operand = MLC::SemanticIR::LiteralExpr.new(value: 5, type: int_type)
    expr = MLC::SemanticIR::UnaryExpr.new(op: "-", operand: operand, type: int_type)

    assert @analyzer.is_pure_expression(expr)
  end

  def test_member_expr_is_pure_when_object_pure
    object = MLC::SemanticIR::VarExpr.new(name: "point", type: int_type)
    expr = MLC::SemanticIR::MemberExpr.new(object: object, member: "x", type: int_type)

    assert @analyzer.is_pure_expression(expr)
  end

  # ========== is_pure_call? ==========

  def test_pure_function_call
    callee = MLC::SemanticIR::VarExpr.new(name: "add", type: int_type)
    arg = MLC::SemanticIR::LiteralExpr.new(value: 1, type: int_type)
    call = MLC::SemanticIR::CallExpr.new(callee: callee, args: [arg], type: int_type)

    assert @analyzer.is_pure_call?(call)
  end

  def test_print_call_is_not_pure
    callee = MLC::SemanticIR::VarExpr.new(name: "println", type: int_type)
    arg = MLC::SemanticIR::LiteralExpr.new(value: "hello", type: string_type)
    call = MLC::SemanticIR::CallExpr.new(callee: callee, args: [arg], type: int_type)

    refute @analyzer.is_pure_call?(call)
  end

  def test_read_call_is_not_pure
    callee = MLC::SemanticIR::VarExpr.new(name: "read_line", type: string_type)
    call = MLC::SemanticIR::CallExpr.new(callee: callee, args: [], type: string_type)

    refute @analyzer.is_pure_call?(call)
  end

  def test_to_string_call_is_not_pure
    callee = MLC::SemanticIR::VarExpr.new(name: "to_string", type: string_type)
    arg = MLC::SemanticIR::LiteralExpr.new(value: 42, type: int_type)
    call = MLC::SemanticIR::CallExpr.new(callee: callee, args: [arg], type: string_type)

    refute @analyzer.is_pure_call?(call)
  end

  # ========== non_literal_type? ==========

  def test_string_is_non_literal_type
    assert @analyzer.non_literal_type?(string_type)
  end

  def test_int_is_literal_type
    refute @analyzer.non_literal_type?(int_type)
  end

  def test_nil_type_is_literal
    refute @analyzer.non_literal_type?(nil)
  end

  # ========== pure_block_expr? ==========

  def test_empty_block_is_pure
    block = MLC::SemanticIR::BlockExpr.new(statements: [], result: nil, type: void_type)

    assert @analyzer.pure_block_expr?(block)
  end

  def test_block_with_pure_statements_is_pure
    stmt = MLC::SemanticIR::VariableDeclStmt.new(
      name: "x",
      type: int_type,
      value: MLC::SemanticIR::LiteralExpr.new(value: 1, type: int_type),
      mutable: false
    )
    result = MLC::SemanticIR::VarExpr.new(name: "x", type: int_type)
    block = MLC::SemanticIR::BlockExpr.new(statements: [stmt], result: result, type: int_type)

    assert @analyzer.pure_block_expr?(block)
  end

  def test_block_with_mutable_decl_is_not_pure
    stmt = MLC::SemanticIR::VariableDeclStmt.new(
      name: "x",
      type: int_type,
      value: MLC::SemanticIR::LiteralExpr.new(value: 1, type: int_type),
      mutable: true
    )
    block = MLC::SemanticIR::BlockExpr.new(statements: [stmt], result: nil, type: void_type)

    refute @analyzer.pure_block_expr?(block)
  end

  # ========== pure_statement? ==========

  def test_immutable_var_decl_with_pure_value_is_pure
    stmt = MLC::SemanticIR::VariableDeclStmt.new(
      name: "x",
      type: int_type,
      value: MLC::SemanticIR::LiteralExpr.new(value: 42, type: int_type),
      mutable: false
    )

    assert @analyzer.pure_statement?(stmt)
  end

  def test_mutable_var_decl_is_not_pure
    stmt = MLC::SemanticIR::VariableDeclStmt.new(
      name: "x",
      type: int_type,
      value: MLC::SemanticIR::LiteralExpr.new(value: 42, type: int_type),
      mutable: true
    )

    refute @analyzer.pure_statement?(stmt)
  end

  def test_expr_statement_with_pure_expr_is_pure
    expr = MLC::SemanticIR::LiteralExpr.new(value: 42, type: int_type)
    stmt = MLC::SemanticIR::ExprStatement.new(expression: expr)

    assert @analyzer.pure_statement?(stmt)
  end

  private

  def int_type
    MLC::SemanticIR::Builder.primitive_type("i32")
  end

  def string_type
    MLC::SemanticIR::Builder.primitive_type("string")
  end

  def void_type
    MLC::SemanticIR::Builder.primitive_type("void")
  end
end

# Test PurityAnalysis module directly
class PurityAnalysisModuleTest < Minitest::Test
  PurityAnalysis = MLC::Representations::Semantic::Gen::Services::Utils::PurityAnalysis

  # ========== IO_FUNCTION_PATTERN ==========

  def test_io_function_pattern_matches_println
    assert "println" =~ PurityAnalysis::IO_FUNCTION_PATTERN
  end

  def test_io_function_pattern_matches_print
    assert "print" =~ PurityAnalysis::IO_FUNCTION_PATTERN
  end

  def test_io_function_pattern_matches_read
    assert "read_line" =~ PurityAnalysis::IO_FUNCTION_PATTERN
  end

  def test_io_function_pattern_does_not_match_add
    refute "add" =~ PurityAnalysis::IO_FUNCTION_PATTERN
  end

  # ========== NON_LITERAL_FUNCTION_PATTERN ==========

  def test_non_literal_function_pattern_matches_to_string
    assert "to_string" =~ PurityAnalysis::NON_LITERAL_FUNCTION_PATTERN
  end

  def test_non_literal_function_pattern_matches_format
    assert "format" =~ PurityAnalysis::NON_LITERAL_FUNCTION_PATTERN
  end

  def test_non_literal_function_pattern_does_not_match_sqrt
    refute "sqrt" =~ PurityAnalysis::NON_LITERAL_FUNCTION_PATTERN
  end

  # ========== NON_LITERAL_TYPES ==========

  def test_non_literal_types_includes_string
    assert_includes PurityAnalysis::NON_LITERAL_TYPES, "string"
    assert_includes PurityAnalysis::NON_LITERAL_TYPES, "String"
  end

  # ========== NON_LITERAL_TYPE_PATTERN ==========

  def test_non_literal_type_pattern_matches_array
    assert "Array" =~ PurityAnalysis::NON_LITERAL_TYPE_PATTERN
  end

  def test_non_literal_type_pattern_matches_vec
    assert "Vec" =~ PurityAnalysis::NON_LITERAL_TYPE_PATTERN
  end

  def test_non_literal_type_pattern_matches_hashmap
    assert "HashMap" =~ PurityAnalysis::NON_LITERAL_TYPE_PATTERN
  end

  def test_non_literal_type_pattern_does_not_match_int
    refute "i32" =~ PurityAnalysis::NON_LITERAL_TYPE_PATTERN
  end

  # ========== pure_expression? with RecordExpr ==========

  def test_record_expr_with_pure_fields_is_pure
    field_value = MLC::SemanticIR::LiteralExpr.new(value: 1, type: int_type)
    record = MLC::SemanticIR::RecordExpr.new(
      type_name: "Point",
      fields: { "x" => field_value },
      type: int_type
    )

    assert PurityAnalysis.pure_expression?(record)
  end

  # ========== pure_statement? with Block ==========

  def test_nested_block_with_pure_statements_is_pure
    inner_stmt = MLC::SemanticIR::ExprStatement.new(
      expression: MLC::SemanticIR::LiteralExpr.new(value: 1, type: int_type)
    )
    block = MLC::SemanticIR::Block.new(stmts: [inner_stmt])

    assert PurityAnalysis.pure_statement?(block)
  end

  # ========== Edge cases ==========

  def test_unknown_expression_type_is_not_pure
    # Use an object that doesn't match any known expression type
    unknown = Object.new

    refute PurityAnalysis.pure_expression?(unknown)
  end

  def test_unknown_statement_type_is_not_pure
    unknown = Object.new

    refute PurityAnalysis.pure_statement?(unknown)
  end

  private

  def int_type
    MLC::SemanticIR::Builder.primitive_type("i32")
  end
end
