# frozen_string_literal: true

require_relative "../../test_helper"

class SubstitutionTest < Minitest::Test
  def setup
    @subst = MLC::Common::Typing::Substitution
  end

  # === Empty substitutions ===

  def test_empty_substitutions_returns_same_type
    int_type = MLC::SemanticIR::Builder.primitive_type("i32")
    result = @subst.apply(int_type, {})
    assert_equal int_type, result
  end

  # === Type variables ===

  def test_substitutes_type_variable
    type_var = MLC::SemanticIR::TypeVariable.new(name: "T")
    int_type = MLC::SemanticIR::Builder.primitive_type("i32")
    substitutions = { "T" => int_type }

    result = @subst.apply(type_var, substitutions)
    assert_equal int_type, result
  end

  def test_unmatched_type_variable_unchanged
    type_var = MLC::SemanticIR::TypeVariable.new(name: "T")
    int_type = MLC::SemanticIR::Builder.primitive_type("i32")
    substitutions = { "U" => int_type }

    result = @subst.apply(type_var, substitutions)
    assert_equal type_var, result
  end

  # === Primitive types ===

  def test_primitive_type_unchanged
    int_type = MLC::SemanticIR::Builder.primitive_type("i32")
    substitutions = { "T" => MLC::SemanticIR::Builder.primitive_type("f32") }

    result = @subst.apply(int_type, substitutions)
    assert_equal int_type, result
  end

  # === Array types ===

  def test_substitutes_array_element_type
    type_var = MLC::SemanticIR::TypeVariable.new(name: "T")
    arr_type = MLC::SemanticIR::Builder.array_type(type_var)
    int_type = MLC::SemanticIR::Builder.primitive_type("i32")
    substitutions = { "T" => int_type }

    result = @subst.apply(arr_type, substitutions)
    assert_instance_of MLC::SemanticIR::ArrayType, result
    assert_equal "i32", result.element_type.name
  end

  # === Generic types ===

  def test_substitutes_generic_type_args
    type_var = MLC::SemanticIR::TypeVariable.new(name: "T")
    base = MLC::SemanticIR::Builder.primitive_type("Option")
    gen_type = MLC::SemanticIR::Builder.generic_type(base, [type_var])
    int_type = MLC::SemanticIR::Builder.primitive_type("i32")
    substitutions = { "T" => int_type }

    result = @subst.apply(gen_type, substitutions)
    assert_instance_of MLC::SemanticIR::GenericType, result
    assert_equal 1, result.type_args.length
    assert_equal "i32", result.type_args.first.name
  end

  def test_substitutes_multiple_generic_args
    t_var = MLC::SemanticIR::TypeVariable.new(name: "T")
    e_var = MLC::SemanticIR::TypeVariable.new(name: "E")
    base = MLC::SemanticIR::Builder.primitive_type("Result")
    gen_type = MLC::SemanticIR::Builder.generic_type(base, [t_var, e_var])

    int_type = MLC::SemanticIR::Builder.primitive_type("i32")
    str_type = MLC::SemanticIR::Builder.primitive_type("String")
    substitutions = { "T" => int_type, "E" => str_type }

    result = @subst.apply(gen_type, substitutions)
    assert_equal 2, result.type_args.length
    assert_equal "i32", result.type_args[0].name
    assert_equal "String", result.type_args[1].name
  end

  # === Function types ===

  def test_substitutes_function_param_types
    t_var = MLC::SemanticIR::TypeVariable.new(name: "T")
    params = [{ name: "x", type: t_var }]
    ret_type = MLC::SemanticIR::Builder.primitive_type("void")
    func_type = MLC::SemanticIR::Builder.function_type(params, ret_type)

    int_type = MLC::SemanticIR::Builder.primitive_type("i32")
    substitutions = { "T" => int_type }

    result = @subst.apply(func_type, substitutions)
    assert_instance_of MLC::SemanticIR::FunctionType, result
    assert_equal "i32", result.params.first[:type].name
  end

  def test_substitutes_function_return_type
    t_var = MLC::SemanticIR::TypeVariable.new(name: "T")
    params = []
    func_type = MLC::SemanticIR::Builder.function_type(params, t_var)

    int_type = MLC::SemanticIR::Builder.primitive_type("i32")
    substitutions = { "T" => int_type }

    result = @subst.apply(func_type, substitutions)
    assert_equal "i32", result.ret_type.name
  end

  # === Nested types ===

  def test_substitutes_nested_generic_types
    t_var = MLC::SemanticIR::TypeVariable.new(name: "T")
    option_base = MLC::SemanticIR::Builder.primitive_type("Option")
    inner = MLC::SemanticIR::Builder.generic_type(option_base, [t_var])

    result_base = MLC::SemanticIR::Builder.primitive_type("Result")
    outer = MLC::SemanticIR::Builder.generic_type(result_base, [inner])

    int_type = MLC::SemanticIR::Builder.primitive_type("i32")
    substitutions = { "T" => int_type }

    result = @subst.apply(outer, substitutions)
    assert_instance_of MLC::SemanticIR::GenericType, result
    inner_result = result.type_args.first
    assert_instance_of MLC::SemanticIR::GenericType, inner_result
    assert_equal "i32", inner_result.type_args.first.name
  end

  def test_substitutes_array_of_generic
    t_var = MLC::SemanticIR::TypeVariable.new(name: "T")
    option_base = MLC::SemanticIR::Builder.primitive_type("Option")
    gen_type = MLC::SemanticIR::Builder.generic_type(option_base, [t_var])
    arr_type = MLC::SemanticIR::Builder.array_type(gen_type)

    int_type = MLC::SemanticIR::Builder.primitive_type("i32")
    substitutions = { "T" => int_type }

    result = @subst.apply(arr_type, substitutions)
    assert_instance_of MLC::SemanticIR::ArrayType, result
    inner = result.element_type
    assert_instance_of MLC::SemanticIR::GenericType, inner
    assert_equal "i32", inner.type_args.first.name
  end
end
