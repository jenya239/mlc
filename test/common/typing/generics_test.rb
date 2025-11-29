# frozen_string_literal: true

require_relative "../../test_helper"

class GenericsTest < Minitest::Test
  def setup
    @generics = MLC::Common::Typing::Generics
  end

  # === Empty type params ===

  def test_nil_type_params_returns_empty
    int_type = MLC::SemanticIR::Builder.primitive_type("i32")
    base = MLC::SemanticIR::Builder.primitive_type("Option")
    scrutinee = MLC::SemanticIR::Builder.generic_type(base, [int_type])

    result = @generics.infer_substitutions(nil, base, scrutinee)
    assert_empty result
  end

  def test_empty_type_params_returns_empty
    int_type = MLC::SemanticIR::Builder.primitive_type("i32")
    base = MLC::SemanticIR::Builder.primitive_type("Option")
    scrutinee = MLC::SemanticIR::Builder.generic_type(base, [int_type])

    result = @generics.infer_substitutions([], base, scrutinee)
    assert_empty result
  end

  # === Non-generic scrutinee ===

  def test_non_generic_scrutinee_returns_empty
    t_var = MLC::SemanticIR::TypeVariable.new(name: "T")
    ret_type = MLC::SemanticIR::Builder.primitive_type("Option")
    scrutinee = MLC::SemanticIR::Builder.primitive_type("i32")

    result = @generics.infer_substitutions([t_var], ret_type, scrutinee)
    assert_empty result
  end

  # === Base type mismatch ===

  def test_base_type_mismatch_returns_empty
    t_var = MLC::SemanticIR::TypeVariable.new(name: "T")
    ret_type = MLC::SemanticIR::Builder.primitive_type("Option")

    int_type = MLC::SemanticIR::Builder.primitive_type("i32")
    result_base = MLC::SemanticIR::Builder.primitive_type("Result")
    scrutinee = MLC::SemanticIR::Builder.generic_type(result_base, [int_type])

    result = @generics.infer_substitutions([t_var], ret_type, scrutinee)
    assert_empty result
  end

  # === Single type parameter ===

  def test_single_type_param_inferred
    t_var = MLC::SemanticIR::TypeVariable.new(name: "T")
    ret_type = MLC::SemanticIR::Builder.primitive_type("Option")

    int_type = MLC::SemanticIR::Builder.primitive_type("i32")
    option_base = MLC::SemanticIR::Builder.primitive_type("Option")
    scrutinee = MLC::SemanticIR::Builder.generic_type(option_base, [int_type])

    result = @generics.infer_substitutions([t_var], ret_type, scrutinee)
    assert_equal 1, result.size
    assert_equal int_type, result["T"]
  end

  # === Multiple type parameters ===

  def test_multiple_type_params_inferred
    t_var = MLC::SemanticIR::TypeVariable.new(name: "T")
    e_var = MLC::SemanticIR::TypeVariable.new(name: "E")
    ret_type = MLC::SemanticIR::Builder.primitive_type("Result")

    int_type = MLC::SemanticIR::Builder.primitive_type("i32")
    str_type = MLC::SemanticIR::Builder.primitive_type("String")
    result_base = MLC::SemanticIR::Builder.primitive_type("Result")
    scrutinee = MLC::SemanticIR::Builder.generic_type(result_base, [int_type, str_type])

    result = @generics.infer_substitutions([t_var, e_var], ret_type, scrutinee)
    assert_equal 2, result.size
    assert_equal int_type, result["T"]
    assert_equal str_type, result["E"]
  end

  # === Fewer actual args than type params ===

  def test_partial_args_inferred
    t_var = MLC::SemanticIR::TypeVariable.new(name: "T")
    e_var = MLC::SemanticIR::TypeVariable.new(name: "E")
    ret_type = MLC::SemanticIR::Builder.primitive_type("Result")

    int_type = MLC::SemanticIR::Builder.primitive_type("i32")
    result_base = MLC::SemanticIR::Builder.primitive_type("Result")
    scrutinee = MLC::SemanticIR::Builder.generic_type(result_base, [int_type])

    result = @generics.infer_substitutions([t_var, e_var], ret_type, scrutinee)
    assert_equal 1, result.size
    assert_equal int_type, result["T"]
    refute result.key?("E")
  end
end
