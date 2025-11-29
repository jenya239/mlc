# frozen_string_literal: true

require_relative "../../../../../test_helper"

# Explicitly require the builder
require_relative "../../../../../../lib/mlc/representations/semantic/gen/services/builders/ir_builder"

class IRBuilderServiceTest < Minitest::Test
  def setup
    @builder = MLC::Representations::Semantic::Gen::Services::IRBuilder.new
  end

  # ========== Variable and Type Nodes ==========

  def test_var_creates_var_expr
    type = MLC::SemanticIR::Builder.primitive_type("i32")

    result = @builder.var(name: "x", type: type)

    assert_instance_of MLC::SemanticIR::VarExpr, result
    assert_equal "x", result.name
    assert_equal type, result.type
  end

  def test_var_with_origin
    type = MLC::SemanticIR::Builder.primitive_type("i32")
    origin = { line: 1, column: 5 }

    result = @builder.var(name: "y", type: type, origin: origin)

    assert_equal origin, result.origin
  end

  def test_type_creates_type_node
    result = @builder.type(kind: :prim, name: "i32")

    assert_instance_of MLC::SemanticIR::Type, result
    assert_equal :prim, result.kind
    assert_equal "i32", result.name
  end

  def test_func_type_creates_function_kind_type
    result = @builder.func_type(name: "add")

    assert_instance_of MLC::SemanticIR::Type, result
    assert_equal :func, result.kind
    assert_equal "add", result.name
  end

  def test_function_type_creates_function_type_with_params
    param_type = MLC::SemanticIR::Builder.primitive_type("i32")
    ret_type = MLC::SemanticIR::Builder.primitive_type("bool")

    result = @builder.function_type(params: [param_type], return_type: ret_type)

    assert_instance_of MLC::SemanticIR::FunctionType, result
    assert_equal [param_type], result.params
    assert_equal ret_type, result.ret_type
  end

  def test_param_creates_param_node
    type = MLC::SemanticIR::Builder.primitive_type("i32")

    result = @builder.param(name: "x", type: type)

    assert_instance_of MLC::SemanticIR::Param, result
    assert_equal "x", result.name
    assert_equal type, result.type
  end

  def test_prim_type_creates_primitive_type
    result = @builder.prim_type(name: "f32")

    assert_instance_of MLC::SemanticIR::Type, result
    assert_equal :prim, result.kind
    assert_equal "f32", result.name
  end

  def test_opaque_type_creates_opaque_type
    result = @builder.opaque_type(name: "Window")

    assert_instance_of MLC::SemanticIR::OpaqueType, result
    assert_equal "Window", result.name
    assert result.opaque?
  end

  def test_type_variable_creates_type_variable
    result = @builder.type_variable(name: "T")

    assert_instance_of MLC::SemanticIR::TypeVariable, result
    assert_equal "T", result.name
    assert_nil result.constraint
  end

  def test_type_variable_with_constraint
    result = @builder.type_variable(name: "T", constraint: "Numeric")

    assert_instance_of MLC::SemanticIR::TypeVariable, result
    assert_equal "Numeric", result.constraint
  end

  def test_generic_type_creates_generic_type
    base = MLC::SemanticIR::Builder.primitive_type("Option")
    arg = MLC::SemanticIR::Builder.primitive_type("i32")

    result = @builder.generic_type(base_type: base, type_args: [arg])

    assert_instance_of MLC::SemanticIR::GenericType, result
    assert_equal base, result.base_type
    assert_equal [arg], result.type_args
  end

  def test_struct_type_creates_struct_kind_type
    result = @builder.struct_type(name: "Point")

    assert_instance_of MLC::SemanticIR::Type, result
    assert_equal :struct, result.kind
    assert_equal "Point", result.name
  end

  def test_record_type_creates_record_type
    fields = [{ name: "x", type: MLC::SemanticIR::Builder.primitive_type("i32") }]

    result = @builder.record_type(name: "Point", fields: fields)

    assert_instance_of MLC::SemanticIR::RecordType, result
    assert_equal "Point", result.name
    assert_equal fields, result.fields
  end

  def test_sum_type_creates_sum_type
    variants = [{ name: "Some", fields: [] }, { name: "None", fields: [] }]

    result = @builder.sum_type(name: "Option", variants: variants)

    assert_instance_of MLC::SemanticIR::SumType, result
    assert_equal "Option", result.name
    assert_equal variants, result.variants
  end

  def test_array_type_creates_array_type
    element = MLC::SemanticIR::Builder.primitive_type("i32")

    result = @builder.array_type(element_type: element)

    assert_instance_of MLC::SemanticIR::ArrayType, result
    assert_equal element, result.element_type
  end

  def test_ref_type_creates_ref_type
    inner = MLC::SemanticIR::Builder.primitive_type("i32")

    result = @builder.ref_type(inner_type: inner)

    assert_instance_of MLC::SemanticIR::RefType, result
    assert_equal inner, result.inner_type
    assert result.ref?
  end

  def test_mut_ref_type_creates_mut_ref_type
    inner = MLC::SemanticIR::Builder.primitive_type("i32")

    result = @builder.mut_ref_type(inner_type: inner)

    assert_instance_of MLC::SemanticIR::MutRefType, result
    assert_equal inner, result.inner_type
    assert result.mut_ref?
  end

  # ========== Expression Nodes ==========

  def test_literal_creates_literal_expr
    type = MLC::SemanticIR::Builder.primitive_type("i32")

    result = @builder.literal(value: 42, type: type)

    assert_instance_of MLC::SemanticIR::LiteralExpr, result
    assert_equal 42, result.value
    assert_equal type, result.type
  end

  def test_binary_op_creates_binary_expr
    type = MLC::SemanticIR::Builder.primitive_type("i32")
    left = @builder.literal(value: 1, type: type)
    right = @builder.literal(value: 2, type: type)

    result = @builder.binary_op(op: "+", left: left, right: right, type: type)

    assert_instance_of MLC::SemanticIR::BinaryExpr, result
    assert_equal "+", result.op
    assert_equal left, result.left
    assert_equal right, result.right
    assert_equal type, result.type
  end

  def test_unary_op_creates_unary_expr
    type = MLC::SemanticIR::Builder.primitive_type("i32")
    operand = @builder.literal(value: 5, type: type)

    result = @builder.unary_op(op: "-", operand: operand, type: type)

    assert_instance_of MLC::SemanticIR::UnaryExpr, result
    assert_equal "-", result.op
    assert_equal operand, result.operand
    assert_equal type, result.type
  end

  def test_call_creates_call_expr
    type = MLC::SemanticIR::Builder.primitive_type("i32")
    func = @builder.var(name: "add", type: @builder.func_type(name: "add"))
    args = [@builder.literal(value: 1, type: type)]

    result = @builder.call(func: func, args: args, type: type)

    assert_instance_of MLC::SemanticIR::CallExpr, result
    assert_equal func, result.callee
    assert_equal args, result.args
    assert_equal type, result.type
  end

  def test_member_creates_member_expr
    type = MLC::SemanticIR::Builder.primitive_type("i32")
    record_type = @builder.struct_type(name: "Point")
    object = @builder.var(name: "p", type: record_type)

    result = @builder.member(object: object, member: "x", type: type)

    assert_instance_of MLC::SemanticIR::MemberExpr, result
    assert_equal object, result.object
    assert_equal "x", result.member
    assert_equal type, result.type
  end

  def test_index_creates_index_expr
    element_type = MLC::SemanticIR::Builder.primitive_type("i32")
    array_type = @builder.array_type(element_type: element_type)
    arr = @builder.var(name: "arr", type: array_type)
    idx = @builder.literal(value: 0, type: element_type)

    result = @builder.index(object: arr, index: idx, type: element_type)

    assert_instance_of MLC::SemanticIR::IndexExpr, result
    assert_equal arr, result.object
    assert_equal idx, result.index
    assert_equal element_type, result.type
  end

  def test_slice_creates_slice_expr
    element_type = MLC::SemanticIR::Builder.primitive_type("i32")
    array_type = @builder.array_type(element_type: element_type)
    arr = @builder.var(name: "arr", type: array_type)
    start_idx = @builder.literal(value: 0, type: element_type)
    end_idx = @builder.literal(value: 5, type: element_type)

    result = @builder.slice(object: arr, start_index: start_idx, end_index: end_idx, type: array_type)

    assert_instance_of MLC::SemanticIR::SliceExpr, result
    assert_equal arr, result.object
    assert_equal start_idx, result.start_index
    assert_equal end_idx, result.end_index
  end

  def test_lambda_expr_creates_lambda_expr
    param_type = MLC::SemanticIR::Builder.primitive_type("i32")
    ret_type = MLC::SemanticIR::Builder.primitive_type("i32")
    func_type = @builder.function_type(params: [param_type], return_type: ret_type)
    param = @builder.param(name: "x", type: param_type)
    body = @builder.var(name: "x", type: param_type)

    result = @builder.lambda_expr(params: [param], body: body, function_type: func_type)

    assert_instance_of MLC::SemanticIR::LambdaExpr, result
    assert_equal [param], result.params
    assert_equal body, result.body
    assert_equal func_type, result.function_type
    assert_equal [], result.captures
  end

  def test_list_comprehension_creates_list_comp_expr
    element_type = MLC::SemanticIR::Builder.primitive_type("i32")
    array_type = @builder.array_type(element_type: element_type)
    output = @builder.var(name: "x", type: element_type)

    result = @builder.list_comprehension(
      element_type: element_type,
      generators: [],
      filters: [],
      output_expr: output,
      type: array_type
    )

    assert_instance_of MLC::SemanticIR::ListCompExpr, result
    assert_equal element_type, result.element_type
    assert_equal output, result.output_expr
    assert_equal array_type, result.type
  end

  # Note: if_node uses deprecated MLC::SemanticIR::If which doesn't exist
  # Use if_expr instead for IfExpr

  def test_if_expr_creates_if_expr
    type = MLC::SemanticIR::Builder.primitive_type("i32")
    bool_type = MLC::SemanticIR::Builder.primitive_type("bool")
    cond = @builder.literal(value: true, type: bool_type)
    then_branch = @builder.literal(value: 1, type: type)
    else_branch = @builder.literal(value: 2, type: type)

    result = @builder.if_expr(cond: cond, then_branch: then_branch, else_branch: else_branch, type: type)

    assert_instance_of MLC::SemanticIR::IfExpr, result
    assert_equal cond, result.condition
    assert_equal then_branch, result.then_branch
    assert_equal else_branch, result.else_branch
  end

  def test_if_stmt_creates_if_stmt
    bool_type = MLC::SemanticIR::Builder.primitive_type("bool")
    cond = @builder.literal(value: true, type: bool_type)
    then_body = @builder.block(statements: [])
    else_body = @builder.block(statements: [])

    result = @builder.if_stmt(condition: cond, then_body: then_body, else_body: else_body)

    assert_instance_of MLC::SemanticIR::IfStmt, result
    assert_equal cond, result.condition
    assert_equal then_body, result.then_body
    assert_equal else_body, result.else_body
  end

  # Note: while_node uses deprecated MLC::SemanticIR::While which doesn't exist
  # Use while_stmt instead for WhileStmt

  def test_while_stmt_creates_while_stmt
    bool_type = MLC::SemanticIR::Builder.primitive_type("bool")
    cond = @builder.literal(value: true, type: bool_type)
    body = @builder.block(statements: [])

    result = @builder.while_stmt(condition: cond, body: body)

    assert_instance_of MLC::SemanticIR::WhileStmt, result
    assert_equal cond, result.condition
    assert_equal body, result.body
  end

  # Note: for_node uses deprecated MLC::SemanticIR::For which doesn't exist
  # Use for_stmt instead for ForStmt

  def test_for_stmt_creates_for_stmt
    type = MLC::SemanticIR::Builder.primitive_type("i32")
    array_type = @builder.array_type(element_type: type)
    iterable = @builder.var(name: "arr", type: array_type)
    body = @builder.block(statements: [])

    result = @builder.for_stmt(var_name: "x", var_type: type, iterable: iterable, body: body)

    assert_instance_of MLC::SemanticIR::ForStmt, result
    assert_equal "x", result.var_name
    assert_equal type, result.var_type
    assert_equal iterable, result.iterable
  end

  # ========== Statement Nodes ==========

  def test_return_node_creates_return
    type = MLC::SemanticIR::Builder.primitive_type("i32")
    value = @builder.literal(value: 42, type: type)

    result = @builder.return_node(value: value)

    assert_instance_of MLC::SemanticIR::Return, result
    assert_equal value, result.expr
  end

  def test_break_node_creates_break_stmt
    result = @builder.break_node

    assert_instance_of MLC::SemanticIR::BreakStmt, result
  end

  def test_continue_node_creates_continue_stmt
    result = @builder.continue_node

    assert_instance_of MLC::SemanticIR::ContinueStmt, result
  end

  def test_block_creates_block
    type = MLC::SemanticIR::Builder.primitive_type("i32")
    stmt = @builder.return_node(value: @builder.literal(value: 1, type: type))

    result = @builder.block(statements: [stmt])

    assert_instance_of MLC::SemanticIR::Block, result
    assert_equal [stmt], result.stmts
  end

  def test_block_expr_creates_block_expr
    type = MLC::SemanticIR::Builder.primitive_type("i32")
    lit = @builder.literal(value: 42, type: type)

    result = @builder.block_expr(statements: [], result: lit, type: type)

    assert_instance_of MLC::SemanticIR::BlockExpr, result
    assert_equal [], result.statements
    assert_equal lit, result.result
    assert_equal type, result.type
  end

  def test_variable_decl_stmt_creates_variable_decl_stmt
    type = MLC::SemanticIR::Builder.primitive_type("i32")
    value = @builder.literal(value: 42, type: type)

    result = @builder.variable_decl_stmt(name: "x", type: type, value: value)

    assert_instance_of MLC::SemanticIR::VariableDeclStmt, result
    assert_equal "x", result.name
    assert_equal type, result.type
    assert_equal value, result.value
    refute result.mutable
  end

  def test_variable_decl_stmt_mutable
    type = MLC::SemanticIR::Builder.primitive_type("i32")
    value = @builder.literal(value: 42, type: type)

    result = @builder.variable_decl_stmt(name: "x", type: type, value: value, mutable: true)

    assert result.mutable
  end

  def test_expr_statement_creates_expr_statement
    type = MLC::SemanticIR::Builder.primitive_type("i32")
    expr = @builder.literal(value: 42, type: type)

    result = @builder.expr_statement(expression: expr)

    assert_instance_of MLC::SemanticIR::ExprStatement, result
    assert_equal expr, result.expression
  end

  def test_assignment_stmt_creates_assignment_stmt
    type = MLC::SemanticIR::Builder.primitive_type("i32")
    target = @builder.var(name: "x", type: type)
    value = @builder.literal(value: 42, type: type)

    result = @builder.assignment_stmt(target: target, value: value)

    assert_instance_of MLC::SemanticIR::AssignmentStmt, result
    assert_equal target, result.target
    assert_equal value, result.value
  end

  # Note: var_decl uses deprecated MLC::SemanticIR::VarDecl which doesn't exist
  # Use variable_decl_stmt instead for VariableDeclStmt

  # Note: function_def uses deprecated MLC::SemanticIR::FunctionDef which doesn't exist
  # Use MLC::SemanticIR::Func instead

  # ========== Miscellaneous Nodes ==========

  def test_regex_creates_regex_expr
    type = MLC::SemanticIR::Builder.primitive_type("Regex")

    result = @builder.regex(pattern: "\\d+", flags: "i", type: type)

    assert_instance_of MLC::SemanticIR::RegexExpr, result
    assert_equal "\\d+", result.pattern
    assert_equal "i", result.flags
    assert_equal type, result.type
  end

  def test_record_expr_creates_record_expr
    type = MLC::SemanticIR::Builder.record_type("Point", [])
    field_type = MLC::SemanticIR::Builder.primitive_type("i32")
    fields = { "x" => @builder.literal(value: 1, type: field_type) }

    result = @builder.record_expr(type_name: "Point", fields: fields, type: type)

    assert_instance_of MLC::SemanticIR::RecordExpr, result
    assert_equal "Point", result.type_name
    assert_equal fields, result.fields
  end

  def test_array_literal_creates_array_literal_expr
    element_type = MLC::SemanticIR::Builder.primitive_type("i32")
    array_type = @builder.array_type(element_type: element_type)
    elements = [
      @builder.literal(value: 1, type: element_type),
      @builder.literal(value: 2, type: element_type)
    ]

    result = @builder.array_literal(elements: elements, type: array_type)

    assert_instance_of MLC::SemanticIR::ArrayLiteralExpr, result
    assert_equal elements, result.elements
    assert_equal array_type, result.type
  end

  def test_tuple_expr_creates_tuple_expr
    type1 = MLC::SemanticIR::Builder.primitive_type("i32")
    type2 = MLC::SemanticIR::Builder.primitive_type("string")
    tuple_type = @builder.tuple_type(element_types: [type1, type2])
    elements = [
      @builder.literal(value: 1, type: type1),
      @builder.literal(value: "hello", type: type2)
    ]

    result = @builder.tuple_expr(elements: elements, type: tuple_type)

    assert_instance_of MLC::SemanticIR::TupleExpr, result
    assert_equal elements, result.elements
    assert_equal tuple_type, result.type
  end

  def test_tuple_type_creates_tuple_type
    type1 = MLC::SemanticIR::Builder.primitive_type("i32")
    type2 = MLC::SemanticIR::Builder.primitive_type("string")

    result = @builder.tuple_type(element_types: [type1, type2])

    assert_instance_of MLC::SemanticIR::TupleType, result
    assert_equal [type1, type2], result.element_types
    assert result.tuple?
  end

  def test_tuple_access_expr_creates_tuple_access_expr
    type1 = MLC::SemanticIR::Builder.primitive_type("i32")
    type2 = MLC::SemanticIR::Builder.primitive_type("string")
    tuple_type = @builder.tuple_type(element_types: [type1, type2])
    tuple = @builder.var(name: "t", type: tuple_type)

    result = @builder.tuple_access_expr(tuple: tuple, index: 0, type: type1)

    assert_instance_of MLC::SemanticIR::TupleAccessExpr, result
    assert_equal tuple, result.tuple
    assert_equal 0, result.index
    assert_equal type1, result.type
  end

  def test_symbol_expr_creates_symbol_expr
    type = @builder.symbol_type

    result = @builder.symbol_expr(name: "foo", type: type)

    assert_instance_of MLC::SemanticIR::SymbolExpr, result
    assert_equal "foo", result.name
    assert_equal type, result.type
  end

  def test_symbol_type_creates_symbol_type
    result = @builder.symbol_type

    assert_instance_of MLC::SemanticIR::SymbolType, result
    assert result.symbol?
  end

  def test_map_type_creates_map_type
    key_type = MLC::SemanticIR::Builder.primitive_type("string")
    value_type = MLC::SemanticIR::Builder.primitive_type("i32")

    result = @builder.map_type(key_type: key_type, value_type: value_type)

    assert_instance_of MLC::SemanticIR::MapType, result
    assert_equal key_type, result.key_type
    assert_equal value_type, result.value_type
    assert result.map?
  end

  def test_match_expr_creates_match_expr
    type = MLC::SemanticIR::Builder.primitive_type("i32")
    scrutinee = @builder.var(name: "x", type: type)
    arms = []

    result = @builder.match_expr(scrutinee: scrutinee, arms: arms, type: type)

    assert_instance_of MLC::SemanticIR::MatchExpr, result
    assert_equal scrutinee, result.scrutinee
    assert_equal arms, result.arms
    assert_equal type, result.type
  end

  def test_match_stmt_creates_match_stmt
    type = MLC::SemanticIR::Builder.primitive_type("i32")
    scrutinee = @builder.var(name: "x", type: type)
    arms = []

    result = @builder.match_stmt(scrutinee: scrutinee, arms: arms)

    assert_instance_of MLC::SemanticIR::MatchStmt, result
    assert_equal scrutinee, result.scrutinee
    assert_equal arms, result.arms
  end

  def test_unit_literal_creates_unit_literal
    result = @builder.unit_literal

    assert_instance_of MLC::SemanticIR::UnitLiteral, result
    assert_instance_of MLC::SemanticIR::UnitType, result.type
  end

  def test_unsafe_block_expr_creates_unsafe_block_expr
    type = MLC::SemanticIR::Builder.primitive_type("i32")
    lit = @builder.literal(value: 42, type: type)
    body = @builder.block_expr(statements: [], result: lit, type: type)

    result = @builder.unsafe_block_expr(body: body, type: type)

    assert_instance_of MLC::SemanticIR::UnsafeBlockExpr, result
    assert_equal body, result.body
    assert_equal type, result.type
  end
end
