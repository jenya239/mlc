# frozen_string_literal: true

require_relative "../../test_helper"

class PredicatesTest < Minitest::Test
  def setup
    @predicates = MLC::Common::Typing::Predicates
  end

  # === type_name ===

  def test_type_name_extracts_name
    type = MLC::SemanticIR::Builder.primitive_type("i32")
    assert_equal "i32", @predicates.type_name(type)
  end

  def test_type_name_nil_returns_nil
    assert_nil @predicates.type_name(nil)
  end

  # === normalized_type_name ===

  def test_normalized_type_name_str_to_string
    assert_equal "string", @predicates.normalized_type_name("str")
  end

  def test_normalized_type_name_preserves_other
    assert_equal "i32", @predicates.normalized_type_name("i32")
    assert_equal "bool", @predicates.normalized_type_name("bool")
  end

  def test_normalized_type_name_nil
    assert_nil @predicates.normalized_type_name(nil)
  end

  # === describe_type ===

  def test_describe_type_returns_normalized_name
    type = MLC::SemanticIR::Builder.primitive_type("str")
    assert_equal "string", @predicates.describe_type(type)
  end

  def test_describe_type_nil_returns_unknown
    assert_equal "unknown", @predicates.describe_type(nil)
  end

  # === numeric_type? ===

  def test_numeric_type_i32
    type = MLC::SemanticIR::Builder.primitive_type("i32")
    assert @predicates.numeric_type?(type)
  end

  def test_numeric_type_f32
    type = MLC::SemanticIR::Builder.primitive_type("f32")
    assert @predicates.numeric_type?(type)
  end

  def test_numeric_type_i64
    type = MLC::SemanticIR::Builder.primitive_type("i64")
    assert @predicates.numeric_type?(type)
  end

  def test_numeric_type_f64
    type = MLC::SemanticIR::Builder.primitive_type("f64")
    assert @predicates.numeric_type?(type)
  end

  def test_numeric_type_u32
    type = MLC::SemanticIR::Builder.primitive_type("u32")
    assert @predicates.numeric_type?(type)
  end

  def test_numeric_type_u64
    type = MLC::SemanticIR::Builder.primitive_type("u64")
    assert @predicates.numeric_type?(type)
  end

  def test_numeric_type_string_is_not_numeric
    type = MLC::SemanticIR::Builder.primitive_type("string")
    refute @predicates.numeric_type?(type)
  end

  def test_numeric_type_bool_is_not_numeric
    type = MLC::SemanticIR::Builder.primitive_type("bool")
    refute @predicates.numeric_type?(type)
  end

  def test_numeric_type_type_variable_is_numeric
    type_var = MLC::SemanticIR::TypeVariable.new(name: "T")
    assert @predicates.numeric_type?(type_var)
  end

  # === error_type? ===

  def test_error_type_true
    type = MLC::SemanticIR::ErrorType.new(error_message: "test")
    assert @predicates.error_type?(type)
  end

  def test_error_type_false_for_primitive
    type = MLC::SemanticIR::Builder.primitive_type("i32")
    refute @predicates.error_type?(type)
  end

  def test_error_type_false_for_nil
    refute @predicates.error_type?(nil)
  end

  # === generic_type_name? ===

  def test_generic_type_name_uppercase
    assert @predicates.generic_type_name?("T")
    assert @predicates.generic_type_name?("Type")
  end

  def test_generic_type_name_empty_string
    assert @predicates.generic_type_name?("")
  end

  def test_generic_type_name_lowercase_is_not_generic
    refute @predicates.generic_type_name?("i32")
    refute @predicates.generic_type_name?("string")
  end

  def test_generic_type_name_nil_is_not_generic
    refute @predicates.generic_type_name?(nil)
  end

  def test_generic_type_name_non_string_is_not_generic
    refute @predicates.generic_type_name?(123)
  end

  # === unit_like? ===

  def test_unit_like_unit_name
    type = MLC::SemanticIR::Builder.primitive_type("unit")
    assert @predicates.unit_like?("unit", type)
  end

  def test_unit_like_void_name
    type = MLC::SemanticIR::Builder.primitive_type("void")
    assert @predicates.unit_like?("void", type)
  end

  def test_unit_like_unit_type_instance
    type = MLC::SemanticIR::UnitType.new
    assert @predicates.unit_like?("other", type)
  end

  def test_unit_like_not_unit
    type = MLC::SemanticIR::Builder.primitive_type("i32")
    refute @predicates.unit_like?("i32", type)
  end
end
