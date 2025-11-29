# frozen_string_literal: true

require_relative "../test_helper"

class ArrayAdditionalMethodsTest < Minitest::Test
  # Test arr.take(n) method
  def test_array_take
    source = <<~MLC
      fn get_first_three(arr: i32[]) -> i32[] = arr.take(3)
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "mlc::collections::take"
    assert_includes cpp, "get_first_three"
  end

  # Test arr.drop(n) method
  def test_array_drop
    source = <<~MLC
      fn skip_first_two(arr: i32[]) -> i32[] = arr.drop(2)
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "mlc::collections::drop"
    assert_includes cpp, "skip_first_two"
  end

  # Test arr.contains(elem) method
  def test_array_contains
    source = <<~MLC
      fn has_five(arr: i32[]) -> bool = arr.contains(5)
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "mlc::collections::contains"
    assert_includes cpp, "has_five"
  end

  # Test arr.join(separator) method
  def test_array_join
    source = <<~MLC
      fn to_csv(arr: str[]) -> str = arr.join(", ")
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "mlc::collections::join"
    assert_includes cpp, "to_csv"
  end

  # Test arr.sum() method
  def test_array_sum
    source = <<~MLC
      fn total(arr: i32[]) -> i32 = arr.sum()
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "mlc::collections::fold"
    assert_includes cpp, "total"
  end

  # Test chained methods: take after filter
  def test_take_after_filter
    source = <<~MLC
      fn first_three_positive(arr: i32[]) -> i32[] = arr.filter(x => x > 0).take(3)
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "mlc::collections::filter"
    assert_includes cpp, "mlc::collections::take"
  end

  # Test chained methods: sum after map
  def test_sum_after_map
    source = <<~MLC
      fn sum_doubled(arr: i32[]) -> i32 = arr.map(x => x * 2).sum()
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "mlc::collections::map"
    assert_includes cpp, "mlc::collections::fold"
  end
end
