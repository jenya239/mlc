# frozen_string_literal: true

require_relative "../test_helper"

class ArrayAdditionalMethodsTest < Minitest::Test
  def test_array_take
    source = <<~MLC
      fn get_first_three(arr: i32[]) -> i32[] = arr.take(3)
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, ".take("
    assert_includes cpp, "get_first_three"
  end

  def test_array_drop
    source = <<~MLC
      fn skip_first_two(arr: i32[]) -> i32[] = arr.drop(2)
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, ".drop("
    assert_includes cpp, "skip_first_two"
  end

  def test_array_contains
    source = <<~MLC
      fn has_five(arr: i32[]) -> bool = arr.contains(5)
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, ".contains("
    assert_includes cpp, "has_five"
  end

  def test_array_join
    source = <<~MLC
      fn to_csv(arr: str[]) -> str = arr.join(", ")
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, ".join("
    assert_includes cpp, "to_csv"
  end

  def test_array_sum
    source = <<~MLC
      fn total(arr: i32[]) -> i32 = arr.sum()
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, ".fold("
    assert_includes cpp, "total"
  end

  def test_take_after_filter
    source = <<~MLC
      fn first_three_positive(arr: i32[]) -> i32[] = arr.filter(x => x > 0).take(3)
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, ".filter("
    assert_includes cpp, ".take("
  end

  def test_sum_after_map
    source = <<~MLC
      fn sum_doubled(arr: i32[]) -> i32 = arr.map(x => x * 2).sum()
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, ".map("
    assert_includes cpp, ".fold("
  end
end
