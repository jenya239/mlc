# frozen_string_literal: true

require_relative "../../test_helper"

class ErrorTypeTest < Minitest::Test
  def test_error_type_creation
    error_type = MLC::SemanticIR::ErrorType.new(error_message: "Test error")

    assert_equal :error, error_type.kind
    assert_equal "<error>", error_type.name
    assert_equal "Test error", error_type.error_message
  end

  def test_error_type_error_predicate
    error_type = MLC::SemanticIR::ErrorType.new

    assert error_type.error?
  end

  def test_error_type_compatible_with_everything
    error_type = MLC::SemanticIR::ErrorType.new

    # ErrorType should be compatible with any type to prevent cascading errors
    assert error_type.compatible_with?(MLC::SemanticIR::Builder.primitive_type("i32"))
    assert error_type.compatible_with?(MLC::SemanticIR::Builder.primitive_type("string"))
    assert error_type.compatible_with?(MLC::SemanticIR::UnitType.new)
    assert error_type.compatible_with?(nil)
  end

  def test_error_type_with_origin
    origin = MLC::SourceOrigin.new(file: "test.mlc", line: 10, column: 5)
    error_type = MLC::SemanticIR::ErrorType.new(
      error_message: "Undefined variable",
      origin: origin
    )

    assert_equal origin, error_type.origin
  end

  def test_error_type_is_not_primitive
    error_type = MLC::SemanticIR::ErrorType.new

    refute error_type.primitive?
  end
end
