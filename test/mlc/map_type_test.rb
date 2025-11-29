# frozen_string_literal: true

require_relative "../test_helper"

# Tests for Map<K, V> type and DynRecord functionality
class MapTypeTest < Minitest::Test

  # ============================================================================
  # C++ Type Mapping Tests
  # ============================================================================

  def test_map_type_to_cpp_unordered_map
    # Map<K, V> should map to std::unordered_map<K, V>
    type_map = MLC::Backends::Cpp::Services::Utils::TypeMapper::COLLECTION_TYPES
    assert_equal "std::unordered_map", type_map["Map"]
  end

  def test_map_type_mapper_generic
    # Map<str, i32> should produce std::unordered_map<std::string, int32_t>
    type_map = {
      "str" => "std::string",
      "i32" => "int32_t"
    }

    # Create a GenericType for Map<str, i32>
    base_type = MLC::SemanticIR::Type.new(kind: :primitive, name: "Map")
    key_type = MLC::SemanticIR::Type.new(kind: :primitive, name: "str")
    value_type = MLC::SemanticIR::Type.new(kind: :primitive, name: "i32")

    generic_type = MLC::SemanticIR::GenericType.new(
      base_type: base_type,
      type_args: [key_type, value_type]
    )

    result = MLC::Backends::Cpp::Services::Utils::TypeMapper.map_type(
      generic_type,
      type_map: type_map
    )

    assert_equal "std::unordered_map<std::string, int32_t>", result
  end

  def test_map_type_node
    # Test SemanticIR::MapType directly
    key_type = MLC::SemanticIR::Type.new(kind: :primitive, name: "str")
    value_type = MLC::SemanticIR::Type.new(kind: :primitive, name: "i32")

    map_type = MLC::SemanticIR::MapType.new(
      key_type: key_type,
      value_type: value_type
    )

    assert map_type.map?
    assert_equal key_type, map_type.key_type
    assert_equal value_type, map_type.value_type
  end

  def test_map_type_mapper_direct
    # Test mapping of SemanticIR::MapType
    type_map = {
      "str" => "std::string",
      "i32" => "int32_t"
    }

    key_type = MLC::SemanticIR::Type.new(kind: :primitive, name: "str")
    value_type = MLC::SemanticIR::Type.new(kind: :primitive, name: "i32")

    map_type = MLC::SemanticIR::MapType.new(
      key_type: key_type,
      value_type: value_type
    )

    result = MLC::Backends::Cpp::Services::Utils::TypeMapper.map_type(
      map_type,
      type_map: type_map
    )

    assert_equal "std::unordered_map<std::string, int32_t>", result
  end

  # ============================================================================
  # Parser Tests - Map<K, V> as GenericType
  # ============================================================================

  def test_parse_map_type_annotation
    code = <<~MLC
      fn get_map() -> Map<str, i32> = map_new()
    MLC

    ast = parse_mlc(code)
    func = ast.declarations.first

    assert_instance_of MLC::Source::AST::FuncDecl, func
    assert_instance_of MLC::Source::AST::GenericType, func.ret_type

    generic_type = func.ret_type
    assert_equal "Map", generic_type.base_type.name
    assert_equal 2, generic_type.type_params.size
    assert_equal "str", generic_type.type_params[0].name
    assert_equal "i32", generic_type.type_params[1].name
  end

  def test_parse_map_type_variable_decl
    code = <<~MLC
      fn test() -> i32 = do
        let m: Map<Symbol, Variant> = map_new()
        0
      end
    MLC

    ast = parse_mlc(code)
    func = ast.declarations.first
    var_decl = func.body.statements.first

    assert_instance_of MLC::Source::AST::VariableDecl, var_decl
    assert_instance_of MLC::Source::AST::GenericType, var_decl.type

    generic_type = var_decl.type
    assert_equal "Map", generic_type.base_type.name
    assert_equal 2, generic_type.type_params.size
  end

  # ============================================================================
  # contains_type_variables Tests for MapType
  # ============================================================================

  def test_contains_type_variables_map_no_variables
    type_map = {}
    key_type = MLC::SemanticIR::Type.new(kind: :primitive, name: "str")
    value_type = MLC::SemanticIR::Type.new(kind: :primitive, name: "i32")

    map_type = MLC::SemanticIR::MapType.new(
      key_type: key_type,
      value_type: value_type
    )

    result = MLC::Backends::Cpp::Services::Utils::TypeMapper.contains_type_variables?(map_type)
    refute result
  end

  def test_contains_type_variables_map_with_key_variable
    key_type = MLC::SemanticIR::TypeVariable.new(name: "K")
    value_type = MLC::SemanticIR::Type.new(kind: :primitive, name: "i32")

    map_type = MLC::SemanticIR::MapType.new(
      key_type: key_type,
      value_type: value_type
    )

    result = MLC::Backends::Cpp::Services::Utils::TypeMapper.contains_type_variables?(map_type)
    assert result
  end

  def test_contains_type_variables_map_with_value_variable
    key_type = MLC::SemanticIR::Type.new(kind: :primitive, name: "str")
    value_type = MLC::SemanticIR::TypeVariable.new(name: "V")

    map_type = MLC::SemanticIR::MapType.new(
      key_type: key_type,
      value_type: value_type
    )

    result = MLC::Backends::Cpp::Services::Utils::TypeMapper.contains_type_variables?(map_type)
    assert result
  end

  private

  def parse_mlc(code)
    MLC.parse(code)
  end
end
