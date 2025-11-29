# frozen_string_literal: true

require_relative "../test_helper"

# Tests for let destructuring: let (a, b) = expr and let { x, y } = expr
class DestructuringTest < Minitest::Test
  # ============================================================================
  # Parser Tests - Tuple Destructuring
  # ============================================================================

  def test_parse_tuple_destructuring_two_elements
    code = <<~MLC
      fn test() -> i32 = do
        let (a, b) = (1, 2)
        a + b
      end
    MLC

    ast = parse_mlc(code)
    func = ast.declarations.first
    decl = func.body.statements.first

    assert_instance_of MLC::Source::AST::DestructuringDecl, decl
    assert_equal :tuple, decl.pattern.kind
    assert_equal 2, decl.pattern.data[:elements].size
  end

  def test_parse_tuple_destructuring_three_elements
    code = <<~MLC
      fn test() -> i32 = do
        let (x, y, z) = get_triple()
        x
      end
    MLC

    ast = parse_mlc(code)
    func = ast.declarations.first
    decl = func.body.statements.first

    assert_instance_of MLC::Source::AST::DestructuringDecl, decl
    assert_equal :tuple, decl.pattern.kind
    assert_equal 3, decl.pattern.data[:elements].size
  end

  def test_parse_tuple_destructuring_with_mutable
    code = <<~MLC
      fn test() -> i32 = do
        let mut (a, b) = (1, 2)
        a = 10
        a
      end
    MLC

    ast = parse_mlc(code)
    func = ast.declarations.first
    decl = func.body.statements.first

    assert_instance_of MLC::Source::AST::DestructuringDecl, decl
    assert decl.mutable
  end

  def test_parse_tuple_destructuring_with_wildcard
    code = <<~MLC
      fn test() -> i32 = do
        let (a, _) = (1, 2)
        a
      end
    MLC

    ast = parse_mlc(code)
    func = ast.declarations.first
    decl = func.body.statements.first

    assert_instance_of MLC::Source::AST::DestructuringDecl, decl
    elements = decl.pattern.data[:elements]
    assert_equal :var, elements[0].kind
    assert_equal :wildcard, elements[1].kind
  end

  # ============================================================================
  # Parser Tests - Record Destructuring
  # ============================================================================

  def test_parse_record_destructuring_two_fields
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
    assert_equal :record, decl.pattern.kind
    assert_equal %w[x y], decl.pattern.data[:bindings]
  end

  def test_parse_record_destructuring_three_fields
    code = <<~MLC
      fn test() -> i32 = do
        let { name, age, email } = user
        age
      end
    MLC

    ast = parse_mlc(code)
    func = ast.declarations.first
    decl = func.body.statements.first

    assert_instance_of MLC::Source::AST::DestructuringDecl, decl
    assert_equal :record, decl.pattern.kind
    assert_equal %w[name age email], decl.pattern.data[:bindings]
  end

  def test_parse_record_destructuring_with_mutable
    code = <<~MLC
      fn test() -> i32 = do
        let mut { x, y } = point
        x = 10
        x
      end
    MLC

    ast = parse_mlc(code)
    func = ast.declarations.first
    decl = func.body.statements.first

    assert_instance_of MLC::Source::AST::DestructuringDecl, decl
    assert decl.mutable
  end

  # ============================================================================
  # Regular let still works
  # ============================================================================

  def test_regular_let_still_works
    code = <<~MLC
      fn test() -> i32 = do
        let x = 42
        x
      end
    MLC

    ast = parse_mlc(code)
    func = ast.declarations.first
    decl = func.body.statements.first

    assert_instance_of MLC::Source::AST::VariableDecl, decl
    assert_equal "x", decl.name
  end

  def test_regular_let_with_type_annotation
    code = <<~MLC
      fn test() -> i32 = do
        let x: i32 = 42
        x
      end
    MLC

    ast = parse_mlc(code)
    func = ast.declarations.first
    decl = func.body.statements.first

    assert_instance_of MLC::Source::AST::VariableDecl, decl
    assert_equal "x", decl.name
    assert_equal "i32", decl.type.name
  end

  private

  def parse_mlc(code)
    MLC.parse(code)
  end
end
