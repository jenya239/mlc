# frozen_string_literal: true

require_relative "../test_helper"

class ArrayExtraMethodsTest < Minitest::Test
  def test_array_first
    source = <<~MLC
      fn get_first(arr: i32[]) -> i32 = arr.first()
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, ".front()"
    assert_includes cpp, "get_first"
  end

  def test_array_last
    source = <<~MLC
      fn get_last(arr: i32[]) -> i32 = arr.last()
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, ".back()"
    assert_includes cpp, "get_last"
  end

  def test_array_reverse
    source = <<~MLC
      fn reversed(arr: i32[]) -> i32[] = arr.reverse()
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, ".reverse()"
    assert_includes cpp, "reversed"
  end

  def test_array_is_empty
    source = <<~MLC
      fn check_empty(arr: i32[]) -> bool = arr.is_empty()
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, ".empty()"
    assert_includes cpp, "check_empty"
  end

  def test_first_after_filter
    source = <<~MLC
      fn first_even(arr: i32[]) -> i32 = arr.filter(x => x % 2 == 0).first()
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, ".filter("
    assert_includes cpp, ".front()"
  end

  def test_last_after_reverse
    source = <<~MLC
      fn original_first(arr: i32[]) -> i32 = arr.reverse().last()
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, ".reverse()"
    assert_includes cpp, ".back()"
  end
end
