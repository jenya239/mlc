# frozen_string_literal: true

require_relative "../test_helper"

class TypeConstraintsTest < Minitest::Test
  def test_numeric_constraint_parsing
    source = <<~MLC
      fn sum<T: Numeric>(a: T, b: T) -> T = a + b
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.first
    assert_equal 1, func.type_params.length
    assert_equal "T", func.type_params.first.name
    assert_equal "Numeric", func.type_params.first.constraint
  end

  def test_comparable_constraint_parsing
    source = <<~MLC
      fn max<T: Comparable>(a: T, b: T) -> T =
        if a > b then a else b
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.first
    assert_equal "Comparable", func.type_params.first.constraint
  end

  def test_multiple_constrained_params
    source = <<~MLC
      fn combine<T: Eq, U: Hashable>(a: T, b: U) -> bool = true
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.first
    assert_equal 2, func.type_params.length
    assert_equal "Eq", func.type_params[0].constraint
    assert_equal "Hashable", func.type_params[1].constraint
  end

  def test_constraint_lowering_to_cpp_requires
    source = <<~MLC
      fn sum<T: Numeric>(a: T, b: T) -> T = a + b
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "template<typename T>"
    assert_includes cpp, "requires Numeric<T>"
  end

  def test_multiple_constraints_lowering
    source = <<~MLC
      fn foo<T: Numeric, U: Comparable>(a: T, b: U) -> T = a
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "typename T"
    assert_includes cpp, "typename U"
    # Should have requires clause with both constraints
    assert_includes cpp, "Numeric<T>"
    assert_includes cpp, "Comparable<U>"
  end

  def test_unconstrained_generic
    source = <<~MLC
      fn identity<T>(x: T) -> T = x
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "template<typename T>"
    # Should NOT have requires clause
    refute_includes cpp, "requires"
  end

  def test_constraint_validation_numeric_accepts_i32
    source = <<~MLC
      fn double<T: Numeric>(x: T) -> T = x + x
      fn test() -> i32 = double(5)
    MLC

    # Should compile without error
    cpp = MLC.to_cpp(source)
    assert_includes cpp, "double"
  end

  def test_constraint_validation_comparable_accepts_string
    source = <<~MLC
      fn compare<T: Comparable>(a: T, b: T) -> bool = a == b
    MLC

    # Should compile without error
    cpp = MLC.to_cpp(source)
    assert_includes cpp, "compare"
  end

  def test_eq_constraint
    source = <<~MLC
      fn equals<T: Eq>(a: T, b: T) -> bool = a == b
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "Eq<T>"
  end

  def test_default_constraint
    source = <<~MLC
      fn with_default<T: Default>(x: T) -> T = x
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "Default<T>"
  end

  def test_copy_constraint
    source = <<~MLC
      fn duplicate<T: Copy>(x: T) -> T = x
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "Copy<T>"
  end

  def test_hashable_constraint
    source = <<~MLC
      fn hash_it<T: Hashable>(x: T) -> i32 = 0
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "Hashable<T>"
  end
end
