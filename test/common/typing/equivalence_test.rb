# frozen_string_literal: true

require_relative "../../test_helper"

class EquivalenceTest < Minitest::Test
  def setup
    @equiv = MLC::Common::Typing::Equivalence
  end

  # === Nil handling ===

  def test_nil_left_returns_false
    int_type = MLC::SemanticIR::Builder.primitive_type("i32")
    refute @equiv.equivalent?(nil, int_type)
  end

  def test_nil_right_returns_false
    int_type = MLC::SemanticIR::Builder.primitive_type("i32")
    refute @equiv.equivalent?(int_type, nil)
  end

  def test_both_nil_returns_false
    refute @equiv.equivalent?(nil, nil)
  end

  # === Identity ===

  def test_same_object_returns_true
    int_type = MLC::SemanticIR::Builder.primitive_type("i32")
    assert @equiv.equivalent?(int_type, int_type)
  end

  # === Primitive types ===

  def test_same_primitive_types_are_equivalent
    int1 = MLC::SemanticIR::Builder.primitive_type("i32")
    int2 = MLC::SemanticIR::Builder.primitive_type("i32")
    assert @equiv.equivalent?(int1, int2)
  end

  def test_different_primitive_types_are_not_equivalent
    int_type = MLC::SemanticIR::Builder.primitive_type("i32")
    float_type = MLC::SemanticIR::Builder.primitive_type("f32")
    refute @equiv.equivalent?(int_type, float_type)
  end

  # === Type variables ===

  def test_same_type_variables_are_equivalent
    t1 = MLC::SemanticIR::TypeVariable.new(name: "T")
    t2 = MLC::SemanticIR::TypeVariable.new(name: "T")
    assert @equiv.equivalent?(t1, t2)
  end

  def test_different_type_variables_are_not_equivalent
    t = MLC::SemanticIR::TypeVariable.new(name: "T")
    u = MLC::SemanticIR::TypeVariable.new(name: "U")
    refute @equiv.equivalent?(t, u)
  end

  # === Array types ===

  def test_same_array_types_are_equivalent
    arr1 = MLC::SemanticIR::Builder.array_type(MLC::SemanticIR::Builder.primitive_type("i32"))
    arr2 = MLC::SemanticIR::Builder.array_type(MLC::SemanticIR::Builder.primitive_type("i32"))
    assert @equiv.equivalent?(arr1, arr2)
  end

  def test_different_array_types_are_not_equivalent
    int_arr = MLC::SemanticIR::Builder.array_type(MLC::SemanticIR::Builder.primitive_type("i32"))
    float_arr = MLC::SemanticIR::Builder.array_type(MLC::SemanticIR::Builder.primitive_type("f32"))
    refute @equiv.equivalent?(int_arr, float_arr)
  end

  def test_array_and_primitive_are_not_equivalent
    int_arr = MLC::SemanticIR::Builder.array_type(MLC::SemanticIR::Builder.primitive_type("i32"))
    int_type = MLC::SemanticIR::Builder.primitive_type("i32")
    refute @equiv.equivalent?(int_arr, int_type)
  end

  # === Generic types ===

  def test_same_generic_types_are_equivalent
    int_type = MLC::SemanticIR::Builder.primitive_type("i32")
    base1 = MLC::SemanticIR::Builder.primitive_type("Option")
    base2 = MLC::SemanticIR::Builder.primitive_type("Option")
    gen1 = MLC::SemanticIR::Builder.generic_type(base1, [int_type])
    gen2 = MLC::SemanticIR::Builder.generic_type(base2, [int_type])
    assert @equiv.equivalent?(gen1, gen2)
  end

  def test_different_generic_base_types_are_not_equivalent
    int_type = MLC::SemanticIR::Builder.primitive_type("i32")
    option_base = MLC::SemanticIR::Builder.primitive_type("Option")
    result_base = MLC::SemanticIR::Builder.primitive_type("Result")
    option = MLC::SemanticIR::Builder.generic_type(option_base, [int_type])
    result = MLC::SemanticIR::Builder.generic_type(result_base, [int_type])
    refute @equiv.equivalent?(option, result)
  end

  def test_different_generic_args_are_not_equivalent
    int_type = MLC::SemanticIR::Builder.primitive_type("i32")
    str_type = MLC::SemanticIR::Builder.primitive_type("String")
    base = MLC::SemanticIR::Builder.primitive_type("Option")
    option_int = MLC::SemanticIR::Builder.generic_type(base, [int_type])
    option_str = MLC::SemanticIR::Builder.generic_type(base, [str_type])
    refute @equiv.equivalent?(option_int, option_str)
  end

  def test_different_arity_generic_types_are_not_equivalent
    int_type = MLC::SemanticIR::Builder.primitive_type("i32")
    str_type = MLC::SemanticIR::Builder.primitive_type("String")
    base = MLC::SemanticIR::Builder.primitive_type("Result")
    result1 = MLC::SemanticIR::Builder.generic_type(base, [int_type])
    result2 = MLC::SemanticIR::Builder.generic_type(base, [int_type, str_type])
    refute @equiv.equivalent?(result1, result2)
  end

  # === Nested generic types ===

  def test_nested_generic_types_are_equivalent
    int_type = MLC::SemanticIR::Builder.primitive_type("i32")
    option_base = MLC::SemanticIR::Builder.primitive_type("Option")
    inner = MLC::SemanticIR::Builder.generic_type(option_base, [int_type])

    result_base = MLC::SemanticIR::Builder.primitive_type("Result")
    outer1 = MLC::SemanticIR::Builder.generic_type(result_base, [inner])
    outer2 = MLC::SemanticIR::Builder.generic_type(result_base, [inner])
    assert @equiv.equivalent?(outer1, outer2)
  end
end
