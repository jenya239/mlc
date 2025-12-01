# frozen_string_literal: true

require "test_helper"
require "mlc/representations/semantic/gen/services/traits/trait_registry"
require "mlc/representations/semantic/gen/services/operator_trait_mapper"
require "mlc/representations/semantic/nodes/nodes"

class OperatorTraitMapperTest < Minitest::Test
  def setup
    @trait_registry = MLC::Representations::Semantic::Gen::Services::TraitRegistry.new
    @mapper = MLC::Representations::Semantic::Gen::Services::OperatorTraitMapper.new(@trait_registry)
  end

  # ==========================================================================
  # OPERATOR_TRAITS mapping tests
  # ==========================================================================

  def test_operator_trait_mapping_add
    assert_equal ["Add", "add"], MLC::Representations::Semantic::Gen::Services::OperatorTraitMapper::OPERATOR_TRAITS["+"]
  end

  def test_operator_trait_mapping_sub
    assert_equal ["Sub", "sub"], MLC::Representations::Semantic::Gen::Services::OperatorTraitMapper::OPERATOR_TRAITS["-"]
  end

  def test_operator_trait_mapping_mul
    assert_equal ["Mul", "mul"], MLC::Representations::Semantic::Gen::Services::OperatorTraitMapper::OPERATOR_TRAITS["*"]
  end

  def test_operator_trait_mapping_div
    assert_equal ["Div", "div"], MLC::Representations::Semantic::Gen::Services::OperatorTraitMapper::OPERATOR_TRAITS["/"]
  end

  def test_operator_trait_mapping_rem
    assert_equal ["Rem", "rem"], MLC::Representations::Semantic::Gen::Services::OperatorTraitMapper::OPERATOR_TRAITS["%"]
  end

  def test_operator_trait_mapping_eq
    assert_equal ["Eq", "eq"], MLC::Representations::Semantic::Gen::Services::OperatorTraitMapper::OPERATOR_TRAITS["=="]
  end

  def test_operator_trait_mapping_ne
    assert_equal ["Eq", "ne"], MLC::Representations::Semantic::Gen::Services::OperatorTraitMapper::OPERATOR_TRAITS["!="]
  end

  def test_operator_trait_mapping_lt
    assert_equal ["Ord", "lt"], MLC::Representations::Semantic::Gen::Services::OperatorTraitMapper::OPERATOR_TRAITS["<"]
  end

  def test_operator_trait_mapping_gt
    assert_equal ["Ord", "gt"], MLC::Representations::Semantic::Gen::Services::OperatorTraitMapper::OPERATOR_TRAITS[">"]
  end

  def test_operator_trait_mapping_le
    assert_equal ["Ord", "le"], MLC::Representations::Semantic::Gen::Services::OperatorTraitMapper::OPERATOR_TRAITS["<="]
  end

  def test_operator_trait_mapping_ge
    assert_equal ["Ord", "ge"], MLC::Representations::Semantic::Gen::Services::OperatorTraitMapper::OPERATOR_TRAITS[">="]
  end

  def test_operator_trait_mapping_bitand
    assert_equal ["BitAnd", "bitand"], MLC::Representations::Semantic::Gen::Services::OperatorTraitMapper::OPERATOR_TRAITS["&"]
  end

  def test_operator_trait_mapping_bitor
    assert_equal ["BitOr", "bitor"], MLC::Representations::Semantic::Gen::Services::OperatorTraitMapper::OPERATOR_TRAITS["|"]
  end

  def test_operator_trait_mapping_bitxor
    assert_equal ["BitXor", "bitxor"], MLC::Representations::Semantic::Gen::Services::OperatorTraitMapper::OPERATOR_TRAITS["^"]
  end

  def test_operator_trait_mapping_shl
    assert_equal ["Shl", "shl"], MLC::Representations::Semantic::Gen::Services::OperatorTraitMapper::OPERATOR_TRAITS["<<"]
  end

  def test_operator_trait_mapping_shr
    assert_equal ["Shr", "shr"], MLC::Representations::Semantic::Gen::Services::OperatorTraitMapper::OPERATOR_TRAITS[">>"]
  end

  # ==========================================================================
  # trait_for and method_for tests
  # ==========================================================================

  def test_trait_for_add
    assert_equal "Add", @mapper.trait_for("+")
  end

  def test_method_for_add
    assert_equal "add", @mapper.method_for("+")
  end

  def test_trait_for_unknown_operator
    assert_nil @mapper.trait_for("+++")
  end

  def test_method_for_unknown_operator
    assert_nil @mapper.method_for("+++")
  end

  # ==========================================================================
  # overloadable? tests
  # ==========================================================================

  def test_not_overloadable_for_unknown_operator
    type = make_type("Vec2")
    refute @mapper.overloadable?("+++", type)
  end

  def test_not_overloadable_for_primitive_i32
    type = make_type("i32")
    refute @mapper.overloadable?("+", type)
  end

  def test_not_overloadable_for_primitive_f64
    type = make_type("f64")
    refute @mapper.overloadable?("*", type)
  end

  def test_not_overloadable_for_primitive_bool
    type = make_type("bool")
    refute @mapper.overloadable?("==", type)
  end

  def test_not_overloadable_without_trait_impl
    type = make_type("Vec2")
    # No trait registered
    refute @mapper.overloadable?("+", type)
  end

  def test_overloadable_with_trait_impl
    # Register Add trait
    @trait_registry.register_trait(
      name: "Add",
      type_params: [],
      methods: [{ name: "add", params: [], ret_type: nil }]
    )

    # Register implementation for Vec2
    @trait_registry.register_implementation(
      type_name: "Vec2",
      trait_name: "Add",
      methods: { "add" => make_method_info("add") }
    )

    type = make_type("Vec2")
    assert @mapper.overloadable?("+", type)
  end

  def test_overloadable_with_direct_method
    # Register method directly without trait
    @trait_registry.register_implementation(
      type_name: "Vec2",
      trait_name: nil,
      methods: { "add" => make_method_info("add") }
    )

    type = make_type("Vec2")
    assert @mapper.overloadable?("+", type)
  end

  # ==========================================================================
  # resolve tests
  # ==========================================================================

  def test_resolve_returns_nil_for_unknown_operator
    type = make_type("Vec2")
    assert_nil @mapper.resolve("+++", type)
  end

  def test_resolve_returns_nil_for_no_implementation
    type = make_type("Vec2")
    assert_nil @mapper.resolve("+", type)
  end

  def test_resolve_with_trait_impl
    @trait_registry.register_trait(
      name: "Add",
      type_params: [],
      methods: [{ name: "add", params: [], ret_type: nil }]
    )

    @trait_registry.register_implementation(
      type_name: "Vec2",
      trait_name: "Add",
      methods: { "add" => make_method_info("add") }
    )

    type = make_type("Vec2")
    result = @mapper.resolve("+", type)

    refute_nil result
    assert_equal "Add", result[:trait_name]
    assert_equal "add", result[:method_name]
    assert_equal "Vec2_add", result[:mangled_name]
  end

  def test_resolve_with_direct_method
    @trait_registry.register_implementation(
      type_name: "Vec2",
      trait_name: nil,
      methods: { "add" => make_method_info("add") }
    )

    type = make_type("Vec2")
    result = @mapper.resolve("+", type)

    refute_nil result
    assert_nil result[:trait_name]
    assert_equal "add", result[:method_name]
    assert_equal "Vec2_add", result[:mangled_name]
  end

  def test_resolve_mul_operator
    @trait_registry.register_trait(
      name: "Mul",
      type_params: [],
      methods: [{ name: "mul", params: [], ret_type: nil }]
    )

    @trait_registry.register_implementation(
      type_name: "Matrix",
      trait_name: "Mul",
      methods: { "mul" => make_method_info("mul") }
    )

    type = make_type("Matrix")
    result = @mapper.resolve("*", type)

    assert_equal "Mul", result[:trait_name]
    assert_equal "mul", result[:method_name]
    assert_equal "Matrix_mul", result[:mangled_name]
  end

  private

  def make_type(name)
    MLC::SemanticIR::Type.new(kind: :struct, name: name, origin: nil)
  end

  def make_method_info(name)
    MLC::Representations::Semantic::Gen::Services::TraitRegistry::MethodInfo.new(
      name: name,
      params: [],
      ret_type: nil,
      body: nil,
      is_static: true
    )
  end
end
