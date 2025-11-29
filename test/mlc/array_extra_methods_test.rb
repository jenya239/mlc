# frozen_string_literal: true

require_relative "../test_helper"

class ArrayExtraMethodsTest < Minitest::Test
  # Test arr.first() method
  def test_array_first
    source = <<~MLC
      fn get_first(arr: i32[]) -> i32 = arr.first()
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, ".front()"
    assert_includes cpp, "get_first"
  end

  # Test arr.last() method
  def test_array_last
    source = <<~MLC
      fn get_last(arr: i32[]) -> i32 = arr.last()
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, ".back()"
    assert_includes cpp, "get_last"
  end

  # Test arr.reverse() method
  def test_array_reverse
    source = <<~MLC
      fn reversed(arr: i32[]) -> i32[] = arr.reverse()
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "mlc::collections::reverse"
    assert_includes cpp, "reversed"
  end

  # Test arr.is_empty() method
  def test_array_is_empty
    source = <<~MLC
      fn check_empty(arr: i32[]) -> bool = arr.is_empty()
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, ".empty()"
    assert_includes cpp, "check_empty"
  end

  # Test chained methods: first after filter
  def test_first_after_filter
    source = <<~MLC
      fn first_even(arr: i32[]) -> i32 = arr.filter(x => x % 2 == 0).first()
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "mlc::collections::filter"
    assert_includes cpp, ".front()"
  end

  # Test chained methods: last after reverse
  def test_last_after_reverse
    source = <<~MLC
      fn original_first(arr: i32[]) -> i32 = arr.reverse().last()
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "mlc::collections::reverse"
    assert_includes cpp, ".back()"
  end
end
