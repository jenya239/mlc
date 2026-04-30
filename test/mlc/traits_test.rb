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
  # Where Clause Tests
  # ==========================================================================

  def test_parse_function_with_single_where_bound
    code = <<~MLC
      fn print_value<T>(x: T) -> void where T: Show = show(x)
    MLC

    ast = parse(code)
    func = ast.declarations.first

    assert_instance_of MLC::Source::AST::FuncDecl, func
    assert_equal "print_value", func.name
    refute_nil func.where_clause
    assert_instance_of MLC::Source::AST::WhereClause, func.where_clause
    assert_equal 1, func.where_clause.bounds.size

    bound = func.where_clause.bounds.first
    assert_instance_of MLC::Source::AST::WhereBound, bound
    assert_equal "T", bound.type_param
    assert_equal ["Show"], bound.traits
  end

  def test_parse_function_with_multiple_traits
    code = <<~MLC
      fn process<T>(x: T) -> T where T: Show + Clone + Eq = x
    MLC

    ast = parse(code)
    func = ast.declarations.first

    refute_nil func.where_clause
    bound = func.where_clause.bounds.first

    assert_equal "T", bound.type_param
    assert_equal ["Show", "Clone", "Eq"], bound.traits
  end

  def test_parse_function_with_multiple_bounds
    code = <<~MLC
      fn combine<T, U>(x: T, y: U) -> T where T: Show, U: Clone = x
    MLC

    ast = parse(code)
    func = ast.declarations.first

    refute_nil func.where_clause
    assert_equal 2, func.where_clause.bounds.size

    assert_equal "T", func.where_clause.bounds[0].type_param
    assert_equal ["Show"], func.where_clause.bounds[0].traits

    assert_equal "U", func.where_clause.bounds[1].type_param
    assert_equal ["Clone"], func.where_clause.bounds[1].traits
  end

  def test_parse_function_with_complex_where_clause
    code = <<~MLC
      fn transform<A, B, C>(a: A, b: B) -> C where A: Into + Clone, B: From, C: Default + Eq = default()
    MLC

    ast = parse(code)
    func = ast.declarations.first

    refute_nil func.where_clause
    assert_equal 3, func.where_clause.bounds.size

    assert_equal "A", func.where_clause.bounds[0].type_param
    assert_equal ["Into", "Clone"], func.where_clause.bounds[0].traits

    assert_equal "B", func.where_clause.bounds[1].type_param
    assert_equal ["From"], func.where_clause.bounds[1].traits

    assert_equal "C", func.where_clause.bounds[2].type_param
    assert_equal ["Default", "Eq"], func.where_clause.bounds[2].traits
  end

  def test_parse_function_without_where_clause
    code = <<~MLC
      fn identity<T>(x: T) -> T = x
    MLC

    ast = parse(code)
    func = ast.declarations.first

    assert_nil func.where_clause
  end

  # ==========================================================================
  # Associated Types Tests
  # ==========================================================================

  def test_parse_trait_with_simple_associated_type
    code = <<~MLC
      trait Iterator {
        type Item
        fn next(self: Self) -> Item
      }
    MLC

    ast = parse(code)
    trait = ast.declarations.first

    assert_instance_of MLC::Source::AST::TraitDecl, trait
    assert_equal "Iterator", trait.name
    assert_equal 1, trait.associated_types.size
    assert_equal 1, trait.methods.size

    assoc_type = trait.associated_types.first
    assert_instance_of MLC::Source::AST::AssociatedType, assoc_type
    assert_equal "Item", assoc_type.name
    assert_empty assoc_type.bounds
    assert_nil assoc_type.default_type
  end

  def test_parse_trait_with_bounded_associated_type
    code = <<~MLC
      trait Container {
        type Item: Clone + Debug
        fn get(self: Self) -> Item
      }
    MLC

    ast = parse(code)
    trait = ast.declarations.first

    assoc_type = trait.associated_types.first
    assert_equal "Item", assoc_type.name
    assert_equal ["Clone", "Debug"], assoc_type.bounds
    assert_nil assoc_type.default_type
  end

  def test_parse_trait_with_default_associated_type
    code = <<~MLC
      trait IntoIterator {
        type Item = i32
        fn into_iter(self: Self) -> Iterator
      }
    MLC

    ast = parse(code)
    trait = ast.declarations.first

    assoc_type = trait.associated_types.first
    assert_equal "Item", assoc_type.name
    assert_empty assoc_type.bounds
    refute_nil assoc_type.default_type
    assert_equal "i32", assoc_type.default_type.name
  end

  def test_parse_trait_with_multiple_associated_types
    code = <<~MLC
      trait Mapping {
        type Key
        type Value
        fn get(self: Self, key: Key) -> Value
      }
    MLC

    ast = parse(code)
    trait = ast.declarations.first

    assert_equal 2, trait.associated_types.size
    assert_equal "Key", trait.associated_types[0].name
    assert_equal "Value", trait.associated_types[1].name
  end

  def test_parse_extend_with_associated_type_binding
    code = <<~MLC
      trait Iterator {
        type Item
        fn next(self: Self) -> Item
      }

      extend Vec<T> : Iterator {
        type Item = T
        fn next(self: Self) -> T = self.pop()
      }
    MLC

    ast = parse(code)
    extend_decl = ast.declarations[1]

    assert_instance_of MLC::Source::AST::ExtendDecl, extend_decl
    assert_equal 1, extend_decl.associated_type_bindings.size
    assert_equal 1, extend_decl.methods.size

    binding = extend_decl.associated_type_bindings.first
    assert_instance_of MLC::Source::AST::AssociatedTypeBinding, binding
    assert_equal "Item", binding.name
    assert_equal "T", binding.type.name
  end

  def test_parse_extend_with_multiple_associated_type_bindings
    code = <<~MLC
      trait Mapping {
        type Key
        type Value
      }

      extend HashMap<K, V> : Mapping {
        type Key = K
        type Value = V
      }
    MLC

    ast = parse(code)
    extend_decl = ast.declarations[1]

    assert_equal 2, extend_decl.associated_type_bindings.size
    assert_equal "Key", extend_decl.associated_type_bindings[0].name
    assert_equal "K", extend_decl.associated_type_bindings[0].type.name
    assert_equal "Value", extend_decl.associated_type_bindings[1].name
    assert_equal "V", extend_decl.associated_type_bindings[1].type.name
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

  def test_trait_as_parameter_desugars_to_template_requires
    code = <<~MLC
      trait Display {
        fn to_string(self: Self) -> string
      }

      fn consume(_value: Display) -> unit = ()

      fn main() -> i32 = 0
    MLC

    cpp = compile_to_cpp(code)
    assert_match(/requires/, cpp)
    assert_match(/__trait_param_0/, cpp)
  end

  def test_trait_as_parameter_two_occurrences_two_type_parameters
    code = <<~MLC
      trait Display {
        fn to_string(self: Self) -> string
      }

      fn pair(_first: Display, _second: Display) -> unit = ()

      fn main() -> i32 = 0
    MLC

    cpp = compile_to_cpp(code)
    assert_match(/__trait_param_0/, cpp)
    assert_match(/__trait_param_1/, cpp)
  end

  def test_where_clause_trait_bounds_in_requires
    code = <<~MLC
      trait Display {
        fn to_string(self: Self) -> string
      }
      trait Same {
        fn same(self: Self, other: Self) -> bool
      }

      fn f<T>(_value: T) -> unit where T: Display + Same = ()

      fn main() -> i32 = 0
    MLC

    cpp = compile_to_cpp(code)
    assert_match(/requires/, cpp)
    assert_match(/Display/, cpp)
    assert_match(/Same/, cpp)
  end

  def test_where_clause_unknown_type_parameter_raises
    code = <<~MLC
      trait Display {
        fn to_string(self: Self) -> string
      }

      fn f<T>(_value: T) -> unit where U: Display = ()

      fn main() -> i32 = 0
    MLC

    error = assert_raises(MLC::CompileError) { compile_to_cpp(code) }
    assert_match(/unknown type parameter/, error.message)
  end

  def test_trait_and_type_same_name_conflict_raises
    code = <<~MLC
      trait Ambiguous {
        fn method(self: Self) -> unit
      }

      type Ambiguous = VariantOne | VariantTwo

      fn main() -> i32 = 0
    MLC

    error = assert_raises(MLC::CompileError) { compile_to_cpp(code) }
    assert_match(/both a type and a trait/, error.message)
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
