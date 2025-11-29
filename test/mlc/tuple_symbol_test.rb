# frozen_string_literal: true

require_relative "../test_helper"

# Tests for tuple and symbol literals
class TupleSymbolTest < Minitest::Test
  # --- Lexer tests ---

  def test_lexer_tokenizes_symbol
    lexer = MLC::Source::Parser::Lexer.new(":foo :bar :my_symbol")
    tokens = lexer.tokenize.reject { |t| t.type == :EOF }

    assert_equal 3, tokens.size
    assert_equal :SYMBOL, tokens[0].type
    assert_equal "foo", tokens[0].value

    assert_equal :SYMBOL, tokens[1].type
    assert_equal "bar", tokens[1].value

    assert_equal :SYMBOL, tokens[2].type
    assert_equal "my_symbol", tokens[2].value
  end

  def test_lexer_colon_without_identifier_is_colon
    lexer = MLC::Source::Parser::Lexer.new("x: i32")
    tokens = lexer.tokenize.reject { |t| t.type == :EOF }

    assert_equal :IDENTIFIER, tokens[0].type
    assert_equal :COLON, tokens[1].type
    assert_equal :I32, tokens[2].type
  end

  # --- Parser tests for tuples ---

  def test_parse_grouped_expression
    code = "fn test() -> i32 = (42)"
    ast = MLC.parse(code)
    func = ast.declarations.first
    body = func.body

    # (42) is a grouped expression, not a tuple
    assert_instance_of MLC::Source::AST::IntLit, body
    assert_equal 42, body.value
  end

  def test_parse_single_element_tuple
    code = "fn test() -> i32 = (42,)"
    ast = MLC.parse(code)
    func = ast.declarations.first
    body = func.body

    # (42,) is a single-element tuple
    assert_instance_of MLC::Source::AST::TupleLit, body
    assert_equal 1, body.elements.size
    assert_instance_of MLC::Source::AST::IntLit, body.elements[0]
    assert_equal 42, body.elements[0].value
  end

  def test_parse_two_element_tuple
    code = "fn test() -> i32 = (1, 2)"
    ast = MLC.parse(code)
    func = ast.declarations.first
    body = func.body

    assert_instance_of MLC::Source::AST::TupleLit, body
    assert_equal 2, body.elements.size
    assert_equal 1, body.elements[0].value
    assert_equal 2, body.elements[1].value
  end

  def test_parse_three_element_tuple_with_trailing_comma
    code = "fn test() -> i32 = (1, 2, 3,)"
    ast = MLC.parse(code)
    func = ast.declarations.first
    body = func.body

    assert_instance_of MLC::Source::AST::TupleLit, body
    assert_equal 3, body.elements.size
  end

  def test_parse_empty_tuple
    code = "fn test() -> i32 = ()"
    ast = MLC.parse(code)
    func = ast.declarations.first
    body = func.body

    # () is an empty tuple (unit type)
    assert_instance_of MLC::Source::AST::TupleLit, body
    assert_equal 0, body.elements.size
  end

  def test_parse_nested_tuple
    code = "fn test() -> i32 = ((1, 2), 3)"
    ast = MLC.parse(code)
    func = ast.declarations.first
    body = func.body

    assert_instance_of MLC::Source::AST::TupleLit, body
    assert_equal 2, body.elements.size
    assert_instance_of MLC::Source::AST::TupleLit, body.elements[0]
    assert_instance_of MLC::Source::AST::IntLit, body.elements[1]
  end

  # --- Parser tests for symbols ---

  def test_parse_symbol_literal
    code = "fn test() -> i32 = :foo"
    ast = MLC.parse(code)
    func = ast.declarations.first
    body = func.body

    assert_instance_of MLC::Source::AST::SymbolLit, body
    assert_equal "foo", body.name
  end

  def test_parse_symbol_in_expression
    code = "fn test() -> i32 = if :active then 1 else 0"
    ast = MLC.parse(code)
    func = ast.declarations.first
    body = func.body

    assert_instance_of MLC::Source::AST::IfExpr, body
    # condition should be a symbol
    cond = body.condition
    assert_instance_of MLC::Source::AST::SymbolLit, cond
    assert_equal "active", cond.name
  end

  def test_parse_tuple_with_symbol
    code = "fn test() -> i32 = (:key, 42)"
    ast = MLC.parse(code)
    func = ast.declarations.first
    body = func.body

    assert_instance_of MLC::Source::AST::TupleLit, body
    assert_equal 2, body.elements.size
    assert_instance_of MLC::Source::AST::SymbolLit, body.elements[0]
    assert_equal "key", body.elements[0].name
    assert_instance_of MLC::Source::AST::IntLit, body.elements[1]
  end

  # --- Parser tests for tuple access ---

  def test_parse_tuple_access_first_element
    code = "fn test() -> i32 = t.0"
    ast = MLC.parse(code)
    func = ast.declarations.first
    body = func.body

    assert_instance_of MLC::Source::AST::TupleAccess, body
    assert_instance_of MLC::Source::AST::VarRef, body.tuple
    assert_equal "t", body.tuple.name
    assert_equal 0, body.index
  end

  def test_parse_tuple_access_second_element
    code = "fn test() -> i32 = pair.1"
    ast = MLC.parse(code)
    func = ast.declarations.first
    body = func.body

    assert_instance_of MLC::Source::AST::TupleAccess, body
    assert_equal "pair", body.tuple.name
    assert_equal 1, body.index
  end

  def test_parse_tuple_access_on_expression
    code = "fn test() -> i32 = (1, 2).0"
    ast = MLC.parse(code)
    func = ast.declarations.first
    body = func.body

    assert_instance_of MLC::Source::AST::TupleAccess, body
    assert_instance_of MLC::Source::AST::TupleLit, body.tuple
    assert_equal 0, body.index
  end

  def test_parse_chained_tuple_access
    code = "fn test() -> i32 = nested.0.1"
    ast = MLC.parse(code)
    func = ast.declarations.first
    body = func.body

    # nested.0.1 is (nested.0).1
    assert_instance_of MLC::Source::AST::TupleAccess, body
    assert_equal 1, body.index
    inner = body.tuple
    assert_instance_of MLC::Source::AST::TupleAccess, inner
    assert_equal 0, inner.index
    assert_equal "nested", inner.tuple.name
  end

  # --- Parser tests for tuple patterns ---

  def test_parse_tuple_pattern_two_elements
    code = <<~MLC
      fn test(t: (i32, i32)) -> i32 =
        match t
          | (x, y) => x + y
    MLC
    ast = MLC.parse(code)
    func = ast.declarations.first
    body = func.body

    assert_instance_of MLC::Source::AST::MatchExpr, body
    arm = body.arms.first
    pattern = arm[:pattern]
    assert_equal :tuple, pattern.kind
    assert_equal 2, pattern.data[:elements].size
    assert_equal :var, pattern.data[:elements][0].kind
    assert_equal "x", pattern.data[:elements][0].data[:name]
    assert_equal :var, pattern.data[:elements][1].kind
    assert_equal "y", pattern.data[:elements][1].data[:name]
  end

  def test_parse_tuple_pattern_with_wildcard
    code = <<~MLC
      fn test(t: (i32, i32)) -> i32 =
        match t
          | (x, _) => x
    MLC
    ast = MLC.parse(code)
    func = ast.declarations.first
    body = func.body

    arm = body.arms.first
    pattern = arm[:pattern]
    assert_equal :tuple, pattern.kind
    assert_equal 2, pattern.data[:elements].size
    assert_equal :var, pattern.data[:elements][0].kind
    assert_equal :wildcard, pattern.data[:elements][1].kind
  end

  def test_parse_tuple_pattern_single_element
    code = <<~MLC
      fn test(t: (i32,)) -> i32 =
        match t
          | (x,) => x
    MLC
    ast = MLC.parse(code)
    func = ast.declarations.first
    body = func.body

    arm = body.arms.first
    pattern = arm[:pattern]
    assert_equal :tuple, pattern.kind
    assert_equal 1, pattern.data[:elements].size
    assert_equal :var, pattern.data[:elements][0].kind
    assert_equal "x", pattern.data[:elements][0].data[:name]
  end

  def test_parse_grouped_pattern_not_tuple
    code = <<~MLC
      fn test(x: i32) -> i32 =
        match x
          | (y) => y
    MLC
    ast = MLC.parse(code)
    func = ast.declarations.first
    body = func.body

    arm = body.arms.first
    pattern = arm[:pattern]
    # (y) is a grouped pattern, not a tuple - should unwrap to just :var
    assert_equal :var, pattern.kind
    assert_equal "y", pattern.data[:name]
  end

  def test_parse_empty_tuple_pattern
    code = <<~MLC
      fn test(t: ()) -> i32 =
        match t
          | () => 42
    MLC
    ast = MLC.parse(code)
    func = ast.declarations.first
    body = func.body

    arm = body.arms.first
    pattern = arm[:pattern]
    assert_equal :tuple, pattern.kind
    assert_equal 0, pattern.data[:elements].size
  end

  def test_parse_nested_tuple_pattern
    code = <<~MLC
      fn test(t: ((i32, i32), i32)) -> i32 =
        match t
          | ((a, b), c) => a + b + c
    MLC
    ast = MLC.parse(code)
    func = ast.declarations.first
    body = func.body

    arm = body.arms.first
    pattern = arm[:pattern]
    assert_equal :tuple, pattern.kind
    assert_equal 2, pattern.data[:elements].size

    # First element is a nested tuple pattern
    inner_pattern = pattern.data[:elements][0]
    assert_equal :tuple, inner_pattern.kind
    assert_equal 2, inner_pattern.data[:elements].size
    assert_equal "a", inner_pattern.data[:elements][0].data[:name]
    assert_equal "b", inner_pattern.data[:elements][1].data[:name]

    # Second element is a variable
    assert_equal :var, pattern.data[:elements][1].kind
    assert_equal "c", pattern.data[:elements][1].data[:name]
  end

  # --- Integration tests: tuples + symbols + records + generics + lambdas ---

  def test_tuple_with_record_field
    code = <<~MLC
      type Point = { x: i32, y: i32 }
      fn test() -> i32 = do
        let pair = (Point { x: 1, y: 2 }, Point { x: 3, y: 4 })
        pair.0.x + pair.1.y
      end
    MLC
    ast = MLC.parse(code)
    func = ast.declarations.find { |d| d.is_a?(MLC::Source::AST::FuncDecl) }

    assert func, "Function 'test' should be parsed"
    body = func.body
    assert_instance_of MLC::Source::AST::BlockExpr, body
  end

  def test_generic_tuple_type_annotation
    code = <<~MLC
      type Pair<A, B> = { first: A, second: B }
      fn test() -> i32 = do
        let t: (i32, str) = (42, "hello")
        t.0
      end
    MLC
    ast = MLC.parse(code)
    func = ast.declarations.find { |d| d.is_a?(MLC::Source::AST::FuncDecl) }

    assert func
    body = func.body
    assert_instance_of MLC::Source::AST::BlockExpr, body

    # Find let statement with tuple type annotation
    let_stmt = body.statements.first
    assert_instance_of MLC::Source::AST::VariableDecl, let_stmt
    assert_instance_of MLC::Source::AST::TupleType, let_stmt.type
    assert_equal 2, let_stmt.type.types.size
  end

  def test_lambda_returning_tuple
    code = <<~MLC
      fn test() -> i32 = do
        let make_pair = (x: i32) => (x, x * 2)
        let pair = make_pair(5)
        pair.0 + pair.1
      end
    MLC
    ast = MLC.parse(code)
    func = ast.declarations.first
    body = func.body

    assert_instance_of MLC::Source::AST::BlockExpr, body

    # First let is lambda definition
    lambda_let = body.statements[0]
    assert_instance_of MLC::Source::AST::VariableDecl, lambda_let
    assert_instance_of MLC::Source::AST::Lambda, lambda_let.value
  end

  def test_tuple_in_sum_type_variant
    # Sum type variant with tuple type field
    code = <<~MLC
      type Result = Ok(i32, str) | Err(str)
      fn test(r: Result) -> i32 =
        match r
          | Ok(n, _) => n
          | Err(_) => 0
    MLC
    ast = MLC.parse(code)

    # Find type declaration
    type_decl = ast.declarations.find { |d| d.is_a?(MLC::Source::AST::TypeDecl) }
    assert type_decl
    assert_instance_of MLC::Source::AST::SumType, type_decl.type

    # First variant has two fields
    ok_variant = type_decl.type.variants.find { |v| v[:name] == "Ok" }
    assert ok_variant
    assert_equal 2, ok_variant[:fields].size
  end

  def test_symbol_in_tuple
    code = <<~MLC
      fn test() -> i32 = do
        let entry = (:key, 42)
        entry.1
      end
    MLC
    ast = MLC.parse(code)
    func = ast.declarations.first
    body = func.body
    let_stmt = body.statements.first
    tuple = let_stmt.value

    assert_instance_of MLC::Source::AST::TupleLit, tuple
    assert_equal 2, tuple.elements.size
    assert_instance_of MLC::Source::AST::SymbolLit, tuple.elements[0]
    assert_equal "key", tuple.elements[0].name
  end

  def test_array_of_tuples
    code = <<~MLC
      fn test() -> i32 = do
        let pairs = [(1, 2), (3, 4), (5, 6)]
        pairs[0].0 + pairs[1].1
      end
    MLC
    ast = MLC.parse(code)
    func = ast.declarations.first
    body = func.body
    let_stmt = body.statements.first
    array = let_stmt.value

    assert_instance_of MLC::Source::AST::ArrayLiteral, array
    assert_equal 3, array.elements.size
    array.elements.each do |elem|
      assert_instance_of MLC::Source::AST::TupleLit, elem
    end
  end

  def test_tuple_type_in_function_signature
    code = <<~MLC
      fn swap(t: (i32, i32)) -> (i32, i32) = (t.1, t.0)
    MLC
    ast = MLC.parse(code)
    func = ast.declarations.first

    # Check parameter type (Param is a node with type attribute)
    param = func.params.first
    assert_instance_of MLC::Source::AST::TupleType, param.type
    assert_equal 2, param.type.types.size

    # Check return type
    assert_instance_of MLC::Source::AST::TupleType, func.ret_type
    assert_equal 2, func.ret_type.types.size
  end

  def test_empty_tuple_as_unit_type
    code = <<~MLC
      fn do_nothing() -> () = ()
    MLC
    ast = MLC.parse(code)
    func = ast.declarations.first

    # Return type is empty tuple (unit)
    assert_instance_of MLC::Source::AST::TupleType, func.ret_type
    assert_equal 0, func.ret_type.types.size

    # Body is empty tuple literal
    assert_instance_of MLC::Source::AST::TupleLit, func.body
    assert_equal 0, func.body.elements.size
  end

  def test_match_with_tuple_pattern
    # Test basic tuple pattern (without guard, as tuple guard syntax may not be supported)
    code = <<~MLC
      fn test(t: (i32, i32)) -> i32 =
        match t
          | (x, y) => x + y
    MLC
    ast = MLC.parse(code)
    func = ast.declarations.first
    match_expr = func.body

    assert_instance_of MLC::Source::AST::MatchExpr, match_expr
    assert_equal 1, match_expr.arms.size

    first_arm = match_expr.arms.first
    assert_equal :tuple, first_arm[:pattern].kind
    assert_equal 2, first_arm[:pattern].data[:elements].size
  end
end
