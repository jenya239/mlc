# frozen_string_literal: true

require_relative "../../test_helper"

class UnifyTest < Minitest::Test
  def setup
    @unify = MLC::Common::Typing::Unify
  end

  # === Nil handling ===

  def test_nil_pattern_returns_failure
    int_type = MLC::SemanticIR::Builder.primitive_type("i32")
    result = @unify.unify(nil, int_type)
    assert result.failure?
  end

  def test_nil_actual_returns_failure
    int_type = MLC::SemanticIR::Builder.primitive_type("i32")
    result = @unify.unify(int_type, nil)
    assert result.failure?
  end

  # === Primitive types ===

  def test_same_primitives_succeed
    int1 = MLC::SemanticIR::Builder.primitive_type("i32")
    int2 = MLC::SemanticIR::Builder.primitive_type("i32")
    result = @unify.unify(int1, int2)
    assert result.success?
    assert_empty result.substitutions
  end

  def test_different_primitives_fail
    int_type = MLC::SemanticIR::Builder.primitive_type("i32")
    float_type = MLC::SemanticIR::Builder.primitive_type("f32")
    result = @unify.unify(int_type, float_type)
    assert result.failure?
  end

  # === Type variables ===

  def test_type_variable_binds_to_concrete
    type_var = MLC::SemanticIR::TypeVariable.new(name: "T")
    int_type = MLC::SemanticIR::Builder.primitive_type("i32")
    result = @unify.unify(type_var, int_type)
    assert result.success?
    assert_equal int_type, result.substitutions["T"]
  end

  def test_type_variable_consistent_binding_succeeds
    type_var = MLC::SemanticIR::TypeVariable.new(name: "T")
    int_type = MLC::SemanticIR::Builder.primitive_type("i32")
    existing = { "T" => int_type }
    result = @unify.unify(type_var, int_type, existing)
    assert result.success?
  end

  def test_type_variable_inconsistent_binding_fails
    type_var = MLC::SemanticIR::TypeVariable.new(name: "T")
    int_type = MLC::SemanticIR::Builder.primitive_type("i32")
    float_type = MLC::SemanticIR::Builder.primitive_type("f32")
    existing = { "T" => int_type }
    result = @unify.unify(type_var, float_type, existing)
    assert result.failure?
  end

  # === Array types ===

  def test_array_types_unify_element
    t_var = MLC::SemanticIR::TypeVariable.new(name: "T")
    pattern = MLC::SemanticIR::Builder.array_type(t_var)
    int_type = MLC::SemanticIR::Builder.primitive_type("i32")
    actual = MLC::SemanticIR::Builder.array_type(int_type)

    result = @unify.unify(pattern, actual)
    assert result.success?
    assert_equal int_type, result.substitutions["T"]
  end

  def test_array_with_primitive_fails
    pattern = MLC::SemanticIR::Builder.array_type(MLC::SemanticIR::Builder.primitive_type("i32"))
    actual = MLC::SemanticIR::Builder.primitive_type("i32")
    result = @unify.unify(pattern, actual)
    assert result.failure?
  end

  # === Generic types ===

  def test_generic_types_unify_args
    t_var = MLC::SemanticIR::TypeVariable.new(name: "T")
    option_base = MLC::SemanticIR::Builder.primitive_type("Option")
    pattern = MLC::SemanticIR::Builder.generic_type(option_base, [t_var])

    int_type = MLC::SemanticIR::Builder.primitive_type("i32")
    actual = MLC::SemanticIR::Builder.generic_type(option_base, [int_type])

    result = @unify.unify(pattern, actual)
    assert result.success?
    assert_equal int_type, result.substitutions["T"]
  end

  def test_generic_different_base_fails
    t_var = MLC::SemanticIR::TypeVariable.new(name: "T")
    option_base = MLC::SemanticIR::Builder.primitive_type("Option")
    result_base = MLC::SemanticIR::Builder.primitive_type("Result")
    pattern = MLC::SemanticIR::Builder.generic_type(option_base, [t_var])

    int_type = MLC::SemanticIR::Builder.primitive_type("i32")
    actual = MLC::SemanticIR::Builder.generic_type(result_base, [int_type])

    result = @unify.unify(pattern, actual)
    assert result.failure?
  end

  def test_generic_different_arity_fails
    t_var = MLC::SemanticIR::TypeVariable.new(name: "T")
    base = MLC::SemanticIR::Builder.primitive_type("Result")
    pattern = MLC::SemanticIR::Builder.generic_type(base, [t_var])

    int_type = MLC::SemanticIR::Builder.primitive_type("i32")
    str_type = MLC::SemanticIR::Builder.primitive_type("String")
    actual = MLC::SemanticIR::Builder.generic_type(base, [int_type, str_type])

    result = @unify.unify(pattern, actual)
    assert result.failure?
  end

  def test_generic_multiple_vars_unify
    t_var = MLC::SemanticIR::TypeVariable.new(name: "T")
    e_var = MLC::SemanticIR::TypeVariable.new(name: "E")
    base = MLC::SemanticIR::Builder.primitive_type("Result")
    pattern = MLC::SemanticIR::Builder.generic_type(base, [t_var, e_var])

    int_type = MLC::SemanticIR::Builder.primitive_type("i32")
    str_type = MLC::SemanticIR::Builder.primitive_type("String")
    actual = MLC::SemanticIR::Builder.generic_type(base, [int_type, str_type])

    result = @unify.unify(pattern, actual)
    assert result.success?
    assert_equal int_type, result.substitutions["T"]
    assert_equal str_type, result.substitutions["E"]
  end

  # === Preserves existing substitutions ===

  def test_preserves_existing_substitutions
    t_var = MLC::SemanticIR::TypeVariable.new(name: "T")
    int_type = MLC::SemanticIR::Builder.primitive_type("i32")
    str_type = MLC::SemanticIR::Builder.primitive_type("String")
    existing = { "U" => str_type }

    result = @unify.unify(t_var, int_type, existing)
    assert result.success?
    assert_equal int_type, result.substitutions["T"]
    assert_equal str_type, result.substitutions["U"]
  end

  # === Does not mutate input ===

  def test_does_not_mutate_existing
    t_var = MLC::SemanticIR::TypeVariable.new(name: "T")
    int_type = MLC::SemanticIR::Builder.primitive_type("i32")
    existing = { "U" => MLC::SemanticIR::Builder.primitive_type("String") }
    original_keys = existing.keys.dup

    @unify.unify(t_var, int_type, existing)

    assert_equal original_keys, existing.keys
    refute existing.key?("T")
  end
end
