# frozen_string_literal: true

require_relative "../test_helper"

# Tests for spread operator and shorthand record syntax
class SpreadOperatorTest < Minitest::Test
  # ============================================================================
  # Lexer Tests - SPREAD token
  # ============================================================================

  def test_lexer_tokenizes_spread
    tokens = tokenize("...")
    assert_equal :SPREAD, tokens.first.type
    assert_equal "...", tokens.first.value
  end

  def test_lexer_spread_before_range
    # Spread (...) should be tokenized before range (..)
    tokens = tokenize("...a")
    assert_equal :SPREAD, tokens.first.type
    assert_equal :IDENTIFIER, tokens[1].type
  end

  def test_lexer_range_still_works
    tokens = tokenize("1..10")
    assert_equal :INT_LITERAL, tokens[0].type
    assert_equal :RANGE, tokens[1].type
    assert_equal :INT_LITERAL, tokens[2].type
  end

  # ============================================================================
  # Parser Tests - Record Literals with Spread
  # ============================================================================

  def test_parse_record_with_spread
    code = <<~MLC
      fn test() -> i32 = do
        let base = { x: 1, y: 2 }
        let extended = { ...base, z: 3 }
        0
      end
    MLC

    ast = parse_mlc(code)
    func = ast.declarations.first
    decl = func.body.statements[1]  # let extended = ...

    record_lit = decl.value
    assert_instance_of MLC::Source::AST::RecordLit, record_lit
    assert_equal 1, record_lit.spreads.size
    assert_equal 0, record_lit.spreads.first[:position]
    assert record_lit.fields.key?("z")
  end

  def test_parse_spread_in_middle
    code = <<~MLC
      fn test() -> i32 = do
        let r = { a: 1, ...base, b: 2 }
        0
      end
    MLC

    ast = parse_mlc(code)
    func = ast.declarations.first
    decl = func.body.statements.first

    record_lit = decl.value
    assert_instance_of MLC::Source::AST::RecordLit, record_lit
    assert_equal 1, record_lit.spreads.size
    assert_equal 1, record_lit.spreads.first[:position]  # After 'a: 1'
    assert record_lit.fields.key?("a")
    assert record_lit.fields.key?("b")
  end

  def test_parse_multiple_spreads
    code = <<~MLC
      fn test() -> i32 = do
        let r = { ...a, ...b, x: 1 }
        0
      end
    MLC

    ast = parse_mlc(code)
    func = ast.declarations.first
    decl = func.body.statements.first

    record_lit = decl.value
    assert_instance_of MLC::Source::AST::RecordLit, record_lit
    assert_equal 2, record_lit.spreads.size
    assert_equal 0, record_lit.spreads[0][:position]
    assert_equal 1, record_lit.spreads[1][:position]
  end

  def test_parse_spread_only
    code = <<~MLC
      fn test() -> i32 = do
        let r = { ...base }
        0
      end
    MLC

    ast = parse_mlc(code)
    func = ast.declarations.first
    decl = func.body.statements.first

    record_lit = decl.value
    assert_instance_of MLC::Source::AST::RecordLit, record_lit
    assert_equal 1, record_lit.spreads.size
    assert_empty record_lit.fields
  end

  # ============================================================================
  # Parser Tests - Shorthand Record Syntax
  # ============================================================================

  def test_parse_shorthand_syntax
    code = <<~MLC
      fn test() -> i32 = do
        let x = 1
        let y = 2
        let point = { x, y }
        0
      end
    MLC

    ast = parse_mlc(code)
    func = ast.declarations.first
    decl = func.body.statements[2]  # let point = { x, y }

    record_lit = decl.value
    assert_instance_of MLC::Source::AST::RecordLit, record_lit

    # Shorthand { x, y } should create { x: x, y: y }
    assert record_lit.fields.key?("x")
    assert record_lit.fields.key?("y")

    # The values should be VarRef nodes
    assert_instance_of MLC::Source::AST::VarRef, record_lit.fields["x"]
    assert_equal "x", record_lit.fields["x"].name

    assert_instance_of MLC::Source::AST::VarRef, record_lit.fields["y"]
    assert_equal "y", record_lit.fields["y"].name
  end

  def test_parse_mixed_shorthand_and_full
    code = <<~MLC
      fn test() -> i32 = do
        let x = 1
        let point = { x, y: 2, z: x + 1 }
        0
      end
    MLC

    ast = parse_mlc(code)
    func = ast.declarations.first
    decl = func.body.statements[1]  # let point = ...

    record_lit = decl.value
    assert_instance_of MLC::Source::AST::RecordLit, record_lit

    # x is shorthand
    assert_instance_of MLC::Source::AST::VarRef, record_lit.fields["x"]
    assert_equal "x", record_lit.fields["x"].name

    # y: 2 is full syntax
    assert_instance_of MLC::Source::AST::IntLit, record_lit.fields["y"]

    # z: x + 1 is full syntax with expression
    assert_instance_of MLC::Source::AST::BinaryOp, record_lit.fields["z"]
  end

  def test_parse_shorthand_with_spread
    code = <<~MLC
      fn test() -> i32 = do
        let x = 1
        let r = { ...base, x }
        0
      end
    MLC

    ast = parse_mlc(code)
    func = ast.declarations.first
    decl = func.body.statements[1]

    record_lit = decl.value
    assert_instance_of MLC::Source::AST::RecordLit, record_lit
    assert_equal 1, record_lit.spreads.size
    assert record_lit.fields.key?("x")
    assert_instance_of MLC::Source::AST::VarRef, record_lit.fields["x"]
  end

  # ============================================================================
  # Named Record Literals
  # ============================================================================

  def test_parse_named_record_with_spread
    code = <<~MLC
      type Point = { x: i32, y: i32, z: i32 }

      fn test() -> i32 = do
        let base = Point { x: 1, y: 2, z: 0 }
        let extended = Point { ...base, z: 3 }
        0
      end
    MLC

    ast = parse_mlc(code)
    func = ast.declarations[1]  # Second declaration is the function
    decl = func.body.statements[1]  # let extended = ...

    record_lit = decl.value
    assert_instance_of MLC::Source::AST::RecordLit, record_lit
    assert_equal "Point", record_lit.type_name
    assert_equal 1, record_lit.spreads.size
    assert record_lit.fields.key?("z")
  end

  # ============================================================================
  # Parser Tests - Rest Pattern in Destructuring
  # ============================================================================

  def test_parse_rest_pattern_basic
    code = <<~MLC
      fn test() -> i32 = do
        let { x, ...rest } = obj
        x
      end
    MLC

    ast = parse_mlc(code)
    func = ast.declarations.first
    decl = func.body.statements.first

    assert_instance_of MLC::Source::AST::DestructuringDecl, decl
    pattern = decl.pattern
    assert_equal :record, pattern.kind
    assert_equal %w[x], pattern.data[:bindings]
    assert_equal "rest", pattern.data[:rest]
  end

  def test_parse_rest_pattern_multiple_fields
    code = <<~MLC
      fn test() -> i32 = do
        let { a, b, c, ...others } = obj
        a
      end
    MLC

    ast = parse_mlc(code)
    func = ast.declarations.first
    decl = func.body.statements.first

    assert_instance_of MLC::Source::AST::DestructuringDecl, decl
    pattern = decl.pattern
    assert_equal %w[a b c], pattern.data[:bindings]
    assert_equal "others", pattern.data[:rest]
  end

  def test_parse_rest_only
    code = <<~MLC
      fn test() -> i32 = do
        let { ...all } = obj
        0
      end
    MLC

    ast = parse_mlc(code)
    func = ast.declarations.first
    decl = func.body.statements.first

    assert_instance_of MLC::Source::AST::DestructuringDecl, decl
    pattern = decl.pattern
    assert_empty pattern.data[:bindings]
    assert_equal "all", pattern.data[:rest]
  end

  def test_parse_destructuring_without_rest
    code = <<~MLC
      fn test() -> i32 = do
        let { x, y } = point
        x + y
      end
    MLC

    ast = parse_mlc(code)
    func = ast.declarations.first
    decl = func.body.statements.first

    assert_instance_of MLC::Source::AST::DestructuringDecl, decl
    pattern = decl.pattern
    assert_equal %w[x y], pattern.data[:bindings]
    assert_nil pattern.data[:rest]
  end

  private

  def tokenize(code)
    lexer = MLC::Source::Parser::Lexer.new(code)
    lexer.tokenize
  end

  def parse_mlc(code)
    MLC.parse(code)
  end
end
