# frozen_string_literal: true

require "test_helper"

class TraitsTest < Minitest::Test
  # ==========================================================================
  # Parsing Tests
  # ==========================================================================

  def test_parse_simple_trait
    code = <<~MLC
      trait Show {
        fn show(x: i32) -> str
      }
    MLC

    ast = parse(code)
    assert_equal 1, ast.declarations.size

    trait = ast.declarations.first
    assert_instance_of MLC::Source::AST::TraitDecl, trait
    assert_equal "Show", trait.name
    assert_empty trait.type_params
    assert_equal 1, trait.methods.size

    method = trait.methods.first
    assert_instance_of MLC::Source::AST::TraitMethod, method
    assert_equal "show", method.name
    assert_equal 1, method.params.size
    assert_nil method.body
  end

  def test_parse_generic_trait
    code = <<~MLC
      trait Into<T> {
        fn into(self: Self) -> T
      }
    MLC

    ast = parse(code)
    trait = ast.declarations.first

    assert_equal "Into", trait.name
    assert_equal 1, trait.type_params.size
    assert_equal "T", trait.type_params.first.name
  end

  def test_parse_trait_with_default_impl
    code = <<~MLC
      trait Default {
        fn default() -> i32 = 0
      }
    MLC

    ast = parse(code)
    trait = ast.declarations.first
    method = trait.methods.first

    assert_equal "default", method.name
    refute_nil method.body
    assert_instance_of MLC::Source::AST::IntLit, method.body
    assert_equal 0, method.body.value
  end

  def test_parse_trait_multiple_methods
    code = <<~MLC
      trait Eq {
        fn eq(a: i32, b: i32) -> bool
        fn ne(a: i32, b: i32) -> bool = !eq(a, b)
      }
    MLC

    ast = parse(code)
    trait = ast.declarations.first

    assert_equal 2, trait.methods.size
    assert_equal "eq", trait.methods[0].name
    assert_equal "ne", trait.methods[1].name
    assert_nil trait.methods[0].body
    refute_nil trait.methods[1].body
  end

  def test_parse_extend_with_trait
    code = <<~MLC
      extend i32 : Show {
        fn show(x: i32) -> str = "number"
      }
    MLC

    ast = parse(code)
    extend_decl = ast.declarations.first

    assert_instance_of MLC::Source::AST::ExtendDecl, extend_decl
    assert_equal "i32", extend_decl.target_type.name
    assert_equal "Show", extend_decl.trait_name
    assert_equal 1, extend_decl.methods.size
  end

  def test_parse_extend_without_trait
    code = <<~MLC
      extend Vec2 {
        fn length(v: Vec2) -> f32 = sqrt(v.x * v.x + v.y * v.y)
      }
    MLC

    ast = parse(code)
    extend_decl = ast.declarations.first

    assert_instance_of MLC::Source::AST::ExtendDecl, extend_decl
    assert_equal "Vec2", extend_decl.target_type.name
    assert_nil extend_decl.trait_name
    assert_equal 1, extend_decl.methods.size
  end

  def test_parse_extend_generic_type
    code = <<~MLC
      extend Option<T> {
        fn unwrap_or(opt: Option<T>, default: T) -> T = match opt
          | Some(v) => v
          | None => default
      }
    MLC

    ast = parse(code)
    extend_decl = ast.declarations.first

    assert_instance_of MLC::Source::AST::ExtendDecl, extend_decl
    assert_equal "Option", extend_decl.target_type.name
    assert_equal 1, extend_decl.target_type.type_params.size
  end

  def test_parse_exported_trait
    code = <<~MLC
      export trait Printable {
        fn print(x: i32) -> void
      }
    MLC

    ast = parse(code)
    trait = ast.declarations.first

    assert trait.exported
  end

  def test_parse_exported_extend
    code = <<~MLC
      export extend i32 : Show {
        fn show(x: i32) -> str = "int"
      }
    MLC

    ast = parse(code)
    extend_decl = ast.declarations.first

    assert extend_decl.exported
  end

  # ==========================================================================
  # SemanticIR + C++ Codegen Tests
  # ==========================================================================

  def test_extend_generates_mangled_function
    code = <<~MLC
      type Vec2 = { x: f32, y: f32 }

      extend Vec2 {
        fn zero() -> Vec2 = { x: 0.0, y: 0.0 }
      }

      fn main() -> Vec2 = Vec2.zero()
    MLC

    cpp = compile_to_cpp(code)

    # Extend method should be generated as Vec2_zero
    assert_match(/Vec2_zero/, cpp)
    # main should call Vec2_zero()
    assert_match(/Vec2_zero\(\)/, cpp)
  end

  def test_extend_with_params
    code = <<~MLC
      type Point = { x: i32, y: i32 }

      extend Point {
        fn new(x: i32, y: i32) -> Point = { x: x, y: y }
        fn add(a: Point, b: Point) -> Point = { x: a.x + b.x, y: a.y + b.y }
      }

      fn test() -> Point = Point.add(Point.new(1, 2), Point.new(3, 4))
    MLC

    cpp = compile_to_cpp(code)

    # Both methods should be generated
    assert_match(/Point_new/, cpp)
    assert_match(/Point_add/, cpp)
    # Nested call in test()
    assert_match(/Point_add\(Point_new\(1, 2\), Point_new\(3, 4\)\)/, cpp)
  end

  def test_trait_registration
    code = <<~MLC
      trait Show {
        fn show(x: i32) -> str
      }

      extend i32 : Show {
        fn show(x: i32) -> str = "int"
      }
    MLC

    # Should compile without errors - trait is registered and impl matches
    cpp = compile_to_cpp(code)
    assert_match(/i32_show/, cpp)
  end

  def test_generic_extend
    code = <<~MLC
      type Box<T> = { value: T }

      extend Box<T> {
        fn wrap(v: T) -> Box<T> = { value: v }
        fn unwrap(b: Box<T>) -> T = b.value
      }

      fn test() -> i32 = Box.unwrap(Box.wrap(42))
    MLC

    cpp = compile_to_cpp(code)

    # Generic methods should be templated
    assert_match(/Box_wrap/, cpp)
    assert_match(/Box_unwrap/, cpp)
  end

  private

  def parse(code)
    MLC::Source::Parser::Parser.new(code).parse
  end

  def compile_to_cpp(code)
    MLC.to_cpp(code)
  end
end
