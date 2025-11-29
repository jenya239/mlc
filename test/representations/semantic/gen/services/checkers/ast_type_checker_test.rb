# frozen_string_literal: true

require_relative "../../../../../test_helper"

# Explicitly require the service
require_relative "../../../../../../lib/mlc/representations/semantic/gen/services/checkers/ast_type_checker"

class ASTTypeCheckerTest < Minitest::Test
  def setup
    @checker = MLC::Representations::Semantic::Gen::Services::ASTTypeChecker.new
  end

  # ========== Literal predicates ==========

  def test_int_literal
    node = MLC::Source::AST::IntLit.new(value: 42)

    assert @checker.int_literal?(node)
    refute @checker.float_literal?(node)
    refute @checker.string_literal?(node)
  end

  def test_float_literal
    node = MLC::Source::AST::FloatLit.new(value: 3.14)

    assert @checker.float_literal?(node)
    refute @checker.int_literal?(node)
  end

  def test_string_literal
    node = MLC::Source::AST::StringLit.new(value: "hello")

    assert @checker.string_literal?(node)
    refute @checker.int_literal?(node)
  end

  def test_regex_literal
    node = MLC::Source::AST::RegexLit.new(pattern: "\\d+", flags: "")

    assert @checker.regex_literal?(node)
    refute @checker.string_literal?(node)
  end

  def test_unit_literal
    node = MLC::Source::AST::UnitLit.new

    assert @checker.unit_literal?(node)
    refute @checker.int_literal?(node)
  end

  # ========== literal_kind ==========

  def test_literal_kind_int
    node = MLC::Source::AST::IntLit.new(value: 42)

    assert_equal :int, @checker.literal_kind(node)
  end

  def test_literal_kind_float
    node = MLC::Source::AST::FloatLit.new(value: 3.14)

    assert_equal :float, @checker.literal_kind(node)
  end

  def test_literal_kind_string
    node = MLC::Source::AST::StringLit.new(value: "hello")

    assert_equal :string, @checker.literal_kind(node)
  end

  def test_literal_kind_regex
    node = MLC::Source::AST::RegexLit.new(pattern: "\\d+", flags: "")

    assert_equal :regex, @checker.literal_kind(node)
  end

  def test_literal_kind_unit
    node = MLC::Source::AST::UnitLit.new

    assert_equal :unit, @checker.literal_kind(node)
  end

  def test_literal_kind_nil_for_non_literal
    node = MLC::Source::AST::VarRef.new(name: "x")

    assert_nil @checker.literal_kind(node)
  end

  # ========== Expression predicates ==========

  def test_var_ref
    node = MLC::Source::AST::VarRef.new(name: "x")

    assert @checker.var_ref?(node)
    refute @checker.call?(node)
  end

  def test_member_access
    obj = MLC::Source::AST::VarRef.new(name: "point")
    node = MLC::Source::AST::MemberAccess.new(object: obj, member: "x")

    assert @checker.member_access?(node)
    refute @checker.var_ref?(node)
  end

  def test_call
    callee = MLC::Source::AST::VarRef.new(name: "foo")
    node = MLC::Source::AST::Call.new(callee: callee, args: [])

    assert @checker.call?(node)
    refute @checker.var_ref?(node)
  end

  def test_binary_op
    left = MLC::Source::AST::IntLit.new(value: 1)
    right = MLC::Source::AST::IntLit.new(value: 2)
    node = MLC::Source::AST::BinaryOp.new(op: "+", left: left, right: right)

    assert @checker.binary_op?(node)
    refute @checker.unary_op?(node)
  end

  def test_unary_op
    operand = MLC::Source::AST::IntLit.new(value: 5)
    node = MLC::Source::AST::UnaryOp.new(op: "-", operand: operand)

    assert @checker.unary_op?(node)
    refute @checker.binary_op?(node)
  end

  def test_if_expr
    cond = MLC::Source::AST::VarRef.new(name: "x")
    then_branch = MLC::Source::AST::IntLit.new(value: 1)
    else_branch = MLC::Source::AST::IntLit.new(value: 2)
    node = MLC::Source::AST::IfExpr.new(condition: cond, then_branch: then_branch, else_branch: else_branch)

    assert @checker.if_expr?(node)
    refute @checker.match_expr?(node)
  end

  def test_match_expr
    scrutinee = MLC::Source::AST::VarRef.new(name: "x")
    node = MLC::Source::AST::MatchExpr.new(scrutinee: scrutinee, arms: [])

    assert @checker.match_expr?(node)
    refute @checker.if_expr?(node)
  end

  def test_let
    value = MLC::Source::AST::IntLit.new(value: 42)
    body = MLC::Source::AST::VarRef.new(name: "x")
    node = MLC::Source::AST::Let.new(name: "x", value: value, body: body, mutable: false)

    assert @checker.let?(node)
  end

  # ========== Complex expression predicates ==========

  def test_record_literal
    node = MLC::Source::AST::RecordLit.new(type_name: "Point", fields: [])

    assert @checker.record_literal?(node)
    refute @checker.array_literal?(node)
  end

  def test_array_literal
    node = MLC::Source::AST::ArrayLiteral.new(elements: [])

    assert @checker.array_literal?(node)
    refute @checker.tuple_literal?(node)
  end

  def test_tuple_literal
    node = MLC::Source::AST::TupleLit.new(elements: [])

    assert @checker.tuple_literal?(node)
    refute @checker.array_literal?(node)
  end

  def test_symbol_literal
    node = MLC::Source::AST::SymbolLit.new(name: "foo")

    assert @checker.symbol_literal?(node)
  end

  def test_block_expr
    node = MLC::Source::AST::BlockExpr.new(statements: [], result_expr: nil)

    assert @checker.block_expr?(node)
    refute @checker.do_expr?(node)
  end

  def test_lambda
    body = MLC::Source::AST::IntLit.new(value: 42)
    node = MLC::Source::AST::Lambda.new(params: [], body: body)

    assert @checker.lambda?(node)
  end

  def test_index_access
    obj = MLC::Source::AST::VarRef.new(name: "arr")
    index = MLC::Source::AST::IntLit.new(value: 0)
    node = MLC::Source::AST::IndexAccess.new(object: obj, index: index)

    assert @checker.index_access?(node)
    refute @checker.slice_access?(node)
  end

  def test_tuple_access
    obj = MLC::Source::AST::VarRef.new(name: "tuple")
    node = MLC::Source::AST::TupleAccess.new(tuple: obj, index: 0)

    assert @checker.tuple_access?(node)
  end

  def test_for_loop
    iterable = MLC::Source::AST::VarRef.new(name: "arr")
    body = MLC::Source::AST::IntLit.new(value: 1)
    node = MLC::Source::AST::ForLoop.new(var_name: "x", iterable: iterable, body: body)

    assert @checker.for_loop?(node)
    assert @checker.for_statement?(node)  # Same method, same class
  end

  # ========== Statement predicates ==========

  def test_block_statement
    node = MLC::Source::AST::Block.new(stmts: [])

    assert @checker.block_statement?(node)
    refute @checker.block_expr?(node)
  end

  def test_expr_stmt
    expr = MLC::Source::AST::IntLit.new(value: 42)
    node = MLC::Source::AST::ExprStmt.new(expr: expr)

    assert @checker.expr_stmt?(node)
    refute @checker.return_statement?(node)
  end

  def test_variable_decl_statement
    value = MLC::Source::AST::IntLit.new(value: 42)
    node = MLC::Source::AST::VariableDecl.new(name: "x", value: value, mutable: false)

    assert @checker.variable_decl_statement?(node)
    refute @checker.assignment_statement?(node)
  end

  def test_return_statement
    value = MLC::Source::AST::IntLit.new(value: 42)
    node = MLC::Source::AST::Return.new(expr: value)

    assert @checker.return_statement?(node)
    refute @checker.break_statement?(node)
  end

  def test_assignment_statement
    target = MLC::Source::AST::VarRef.new(name: "x")
    value = MLC::Source::AST::IntLit.new(value: 42)
    node = MLC::Source::AST::Assignment.new(target: target, value: value)

    assert @checker.assignment_statement?(node)
    refute @checker.variable_decl_statement?(node)
  end

  def test_break_statement
    node = MLC::Source::AST::Break.new

    assert @checker.break_statement?(node)
    refute @checker.continue_statement?(node)
  end

  def test_continue_statement
    node = MLC::Source::AST::Continue.new

    assert @checker.continue_statement?(node)
    refute @checker.break_statement?(node)
  end

  def test_if_statement
    cond = MLC::Source::AST::VarRef.new(name: "x")
    then_block = MLC::Source::AST::Block.new(stmts: [])
    node = MLC::Source::AST::IfStmt.new(condition: cond, then_branch: then_block, else_branch: nil)

    assert @checker.if_statement?(node)
    refute @checker.while_statement?(node)
  end

  def test_while_statement
    cond = MLC::Source::AST::VarRef.new(name: "x")
    body = MLC::Source::AST::Block.new(stmts: [])
    node = MLC::Source::AST::WhileStmt.new(condition: cond, body: body)

    assert @checker.while_statement?(node)
    refute @checker.for_statement?(node)
  end

  # ========== Non-matching types ==========

  def test_plain_object_matches_nothing
    obj = Object.new

    refute @checker.var_ref?(obj)
    refute @checker.call?(obj)
    refute @checker.int_literal?(obj)
    refute @checker.block_statement?(obj)
  end
end
