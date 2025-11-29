# frozen_string_literal: true

require "minitest/autorun"
require_relative "../../lib/mlc/common/index"
require_relative "../../lib/mlc/representations/semantic/gen/services/scope/var_type_registry"

class SafetyModelTest < Minitest::Test
  def setup
    @parser = MLC::Source::Parser::Parser
  end

  # ===========================================
  # Reference Type Parsing Tests
  # ===========================================

  def test_parse_immutable_reference_type
    source = "fn foo(x: Ref<i32>) -> i32 = 0"
    ast = @parser.new(source).parse

    func = ast.declarations.first
    param_type = func.params.first.type

    assert_instance_of MLC::Source::AST::GenericType, param_type
    assert_equal "Ref", param_type.base_type.name
    assert_equal 1, param_type.type_params.length
    assert_equal "i32", param_type.type_params.first.name
  end

  def test_parse_mutable_reference_type
    source = "fn foo(x: RefMut<i32>) -> i32 = 0"
    ast = @parser.new(source).parse

    func = ast.declarations.first
    param_type = func.params.first.type

    assert_instance_of MLC::Source::AST::GenericType, param_type
    assert_equal "RefMut", param_type.base_type.name
    assert_equal 1, param_type.type_params.length
    assert_equal "i32", param_type.type_params.first.name
  end

  def test_parse_reference_to_array
    source = "fn foo(arr: Ref<i32[]>) -> i32 = 0"
    ast = @parser.new(source).parse

    func = ast.declarations.first
    param_type = func.params.first.type

    assert_instance_of MLC::Source::AST::GenericType, param_type
    assert_equal "Ref", param_type.base_type.name
    assert_instance_of MLC::Source::AST::ArrayType, param_type.type_params.first
  end

  def test_parse_mutable_reference_to_custom_type
    source = "fn foo(buf: RefMut<Buffer>) -> i32 = 0"
    ast = @parser.new(source).parse

    func = ast.declarations.first
    param_type = func.params.first.type

    assert_instance_of MLC::Source::AST::GenericType, param_type
    assert_equal "RefMut", param_type.base_type.name
    assert_equal "Buffer", param_type.type_params.first.name
  end

  # ===========================================
  # Unsafe Block Parsing Tests
  # ===========================================

  def test_parse_unsafe_block_simple
    source = <<~MLC
      fn foo() -> i32 = unsafe {
        42
      }
    MLC
    ast = @parser.new(source).parse

    func = ast.declarations.first
    body = func.body

    assert_instance_of MLC::Source::AST::UnsafeBlock, body
    assert_instance_of MLC::Source::AST::BlockExpr, body.body
  end

  def test_parse_unsafe_block_with_statements
    source = <<~MLC
      fn foo() -> i32 = unsafe {
        let x = 10
        let y = 20
        x + y
      }
    MLC
    ast = @parser.new(source).parse

    func = ast.declarations.first
    body = func.body

    assert_instance_of MLC::Source::AST::UnsafeBlock, body
    assert_equal 2, body.body.statements.length
  end

  def test_parse_unsafe_block_with_reference_type
    source = <<~MLC
      fn foo(data: Ref<i32>) -> i32 = unsafe {
        data
      }
    MLC
    ast = @parser.new(source).parse

    func = ast.declarations.first
    param_type = func.params.first.type

    assert_instance_of MLC::Source::AST::GenericType, param_type
    assert_equal "Ref", param_type.base_type.name
    assert_instance_of MLC::Source::AST::UnsafeBlock, func.body
  end

  # ===========================================
  # Smart Pointer Type Parsing Tests
  # ===========================================

  def test_parse_shared_type
    source = "fn foo(data: Shared<i32>) -> i32 = 0"
    ast = @parser.new(source).parse

    func = ast.declarations.first
    param_type = func.params.first.type

    assert_instance_of MLC::Source::AST::GenericType, param_type
    assert_equal "Shared", param_type.base_type.name
    assert_equal 1, param_type.type_params.length
  end

  def test_parse_weak_type
    source = "fn foo(data: Weak<Node>) -> bool = true"
    ast = @parser.new(source).parse

    func = ast.declarations.first
    param_type = func.params.first.type

    assert_instance_of MLC::Source::AST::GenericType, param_type
    assert_equal "Weak", param_type.base_type.name
  end

  def test_parse_nested_smart_pointer
    source = "fn foo(data: Shared<Weak<Node>>) -> bool = true"
    ast = @parser.new(source).parse

    func = ast.declarations.first
    param_type = func.params.first.type

    assert_instance_of MLC::Source::AST::GenericType, param_type
    assert_equal "Shared", param_type.base_type.name

    inner_type = param_type.type_params.first
    assert_instance_of MLC::Source::AST::GenericType, inner_type
    assert_equal "Weak", inner_type.base_type.name
  end

  # ===========================================
  # Type Mapper Tests
  # ===========================================

  def test_type_mapper_shared_to_shared_ptr
    type = MLC::SemanticIR::GenericType.new(
      base_type: MLC::SemanticIR::Type.new(kind: :prim, name: "Shared"),
      type_args: [MLC::SemanticIR::Type.new(kind: :prim, name: "i32")]
    )

    type_map = { "i32" => "int32_t" }
    result = MLC::Backends::Cpp::Services::Utils::TypeMapper.map_type(
      type, type_map: type_map
    )

    assert_equal "std::shared_ptr<int32_t>", result
  end

  def test_type_mapper_weak_to_weak_ptr
    type = MLC::SemanticIR::GenericType.new(
      base_type: MLC::SemanticIR::Type.new(kind: :prim, name: "Weak"),
      type_args: [MLC::SemanticIR::Type.new(kind: :prim, name: "Node")]
    )

    type_map = {}
    result = MLC::Backends::Cpp::Services::Utils::TypeMapper.map_type(
      type, type_map: type_map
    )

    assert_equal "std::weak_ptr<Node>", result
  end

  def test_type_mapper_owned_to_unique_ptr
    type = MLC::SemanticIR::GenericType.new(
      base_type: MLC::SemanticIR::Type.new(kind: :prim, name: "Owned"),
      type_args: [MLC::SemanticIR::Type.new(kind: :prim, name: "Buffer")]
    )

    type_map = {}
    result = MLC::Backends::Cpp::Services::Utils::TypeMapper.map_type(
      type, type_map: type_map
    )

    assert_equal "std::unique_ptr<Buffer>", result
  end

  def test_type_mapper_ref_type
    inner = MLC::SemanticIR::Type.new(kind: :prim, name: "i32")
    type = MLC::SemanticIR::RefType.new(inner_type: inner)

    type_map = { "i32" => "int32_t" }
    result = MLC::Backends::Cpp::Services::Utils::TypeMapper.map_type(
      type, type_map: type_map
    )

    assert_equal "const int32_t&", result
  end

  def test_type_mapper_mut_ref_type
    inner = MLC::SemanticIR::Type.new(kind: :prim, name: "i32")
    type = MLC::SemanticIR::MutRefType.new(inner_type: inner)

    type_map = { "i32" => "int32_t" }
    result = MLC::Backends::Cpp::Services::Utils::TypeMapper.map_type(
      type, type_map: type_map
    )

    assert_equal "int32_t&", result
  end

  # ===========================================
  # SemanticIR Type Tests
  # ===========================================

  def test_semantic_ir_ref_type
    inner = MLC::SemanticIR::Type.new(kind: :prim, name: "i32")
    ref_type = MLC::SemanticIR::RefType.new(inner_type: inner)

    assert_equal :ref, ref_type.kind
    assert ref_type.ref?
    assert_equal inner, ref_type.inner_type
  end

  def test_semantic_ir_mut_ref_type
    inner = MLC::SemanticIR::Type.new(kind: :prim, name: "i32")
    mut_ref_type = MLC::SemanticIR::MutRefType.new(inner_type: inner)

    assert_equal :mut_ref, mut_ref_type.kind
    assert mut_ref_type.mut_ref?
    assert_equal inner, mut_ref_type.inner_type
  end

  def test_semantic_ir_unsafe_block_expr
    i32_type = MLC::SemanticIR::Type.new(kind: :prim, name: "i32")
    literal = MLC::SemanticIR::LiteralExpr.new(
      value: 42,
      type: i32_type
    )

    body = MLC::SemanticIR::BlockExpr.new(
      statements: [],
      result: literal,
      type: i32_type
    )

    unsafe_block = MLC::SemanticIR::UnsafeBlockExpr.new(
      body: body,
      type: i32_type
    )

    assert_equal :unsafe_block, unsafe_block.kind
    assert_equal body, unsafe_block.body
  end

  # ===========================================
  # Keyword Tests
  # ===========================================

  def test_unsafe_is_keyword
    source = "fn unsafe_func() -> i32 = 0"
    # 'unsafe' should be a keyword, not an identifier
    # But 'unsafe_func' should be a valid identifier
    ast = @parser.new(source).parse

    func = ast.declarations.first
    assert_equal "unsafe_func", func.name
  end

  def test_ref_not_keyword
    source = "fn ref_count() -> i32 = 0"
    # 'ref' is no longer a keyword, 'ref_count' and even 'ref' are valid identifiers
    ast = @parser.new(source).parse

    func = ast.declarations.first
    assert_equal "ref_count", func.name
  end

  # ===========================================
  # End-to-End Compilation Tests
  # ===========================================

  def test_e2e_unsafe_block_compiles
    source = <<~MLCORA
      fn compute() -> i32 = unsafe {
        let x = 10
        let y = 20
        x + y
      }
    MLCORA

    # This should compile without errors
    cpp = MLC.compile(source).to_source
    assert_includes cpp, "compute"
    # Unsafe blocks with statements use IIFE pattern
    assert_includes cpp, "x + y"
  end

  def test_e2e_shared_pointer_type
    source = <<~MLCORA
      fn get_shared(data: Shared<i32>) -> i32 = 0
    MLCORA

    cpp = MLC.compile(source).to_source
    assert_includes cpp, "std::shared_ptr<int>"
  end

  def test_e2e_weak_pointer_type
    source = <<~MLCORA
      fn get_weak(data: Weak<i32>) -> bool = true
    MLCORA

    cpp = MLC.compile(source).to_source
    assert_includes cpp, "std::weak_ptr<int>"
  end

  def test_e2e_owned_pointer_type
    source = <<~MLCORA
      fn get_owned(data: Owned<i32>) -> i32 = 0
    MLCORA

    cpp = MLC.compile(source).to_source
    assert_includes cpp, "std::unique_ptr<int>"
  end

  # ===========================================
  # Move Semantics Tests
  # ===========================================

  def test_var_type_registry_move_tracking
    registry = MLC::Representations::Semantic::Gen::Services::VarTypeRegistry.new

    # Create Owned<i32> type for testing
    i32_type = MLC::SemanticIR::Type.new(kind: :prim, name: "i32")
    owned_type = MLC::SemanticIR::GenericType.new(
      base_type: MLC::SemanticIR::Type.new(kind: :prim, name: "Owned"),
      type_args: [i32_type]
    )

    registry.set("buf", owned_type)

    # Initially not moved
    refute registry.moved?("buf")

    # Mark as moved
    registry.mark_moved("buf")
    assert registry.moved?("buf")

    # Reset moved state
    registry.reset_moved("buf")
    refute registry.moved?("buf")
  end

  def test_has_move_semantics_for_owned_type
    registry_class = MLC::Representations::Semantic::Gen::Services::VarTypeRegistry

    # Owned<T> has move semantics
    owned_type = MLC::SemanticIR::GenericType.new(
      base_type: MLC::SemanticIR::Type.new(kind: :prim, name: "Owned"),
      type_args: [MLC::SemanticIR::Type.new(kind: :prim, name: "i32")]
    )
    assert registry_class.has_move_semantics?(owned_type)

    # Shared<T> does NOT have move semantics
    shared_type = MLC::SemanticIR::GenericType.new(
      base_type: MLC::SemanticIR::Type.new(kind: :prim, name: "Shared"),
      type_args: [MLC::SemanticIR::Type.new(kind: :prim, name: "i32")]
    )
    refute registry_class.has_move_semantics?(shared_type)

    # Plain types do NOT have move semantics
    plain_type = MLC::SemanticIR::Type.new(kind: :prim, name: "i32")
    refute registry_class.has_move_semantics?(plain_type)
  end

  def test_snapshot_preserves_moved_state
    registry = MLC::Representations::Semantic::Gen::Services::VarTypeRegistry.new
    i32_type = MLC::SemanticIR::Type.new(kind: :prim, name: "i32")

    registry.set("x", i32_type)
    registry.mark_moved("x")

    snapshot = registry.snapshot
    assert snapshot.moved_vars.include?("x")

    # Restore should preserve moved state
    registry.reset_moved("x")
    refute registry.moved?("x")

    registry.restore(snapshot)
    assert registry.moved?("x")
  end
end
