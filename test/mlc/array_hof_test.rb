# frozen_string_literal: true

require "test_helper"

module MLC
  class ArrayHofTest < Minitest::Test
    # Test array.map() method
    def test_array_map
      source = <<~MLC
        fn test_map() -> i32[] = do
          let arr = [1, 2, 3, 4, 5]
          let doubled = arr.map(x => x * 2)
          doubled
        end
      MLC

      cpp = MLC.to_cpp(source)
      assert_includes cpp, "mlc::collections::map"
      assert_includes cpp, "test_map"
    end

    # Test array.filter() method
    def test_array_filter
      source = <<~MLC
        fn test_filter() -> i32[] = do
          let arr = [1, 2, 3, 4, 5]
          let evens = arr.filter(x => x % 2 == 0)
          evens
        end
      MLC

      cpp = MLC.to_cpp(source)
      assert_includes cpp, "mlc::collections::filter"
      assert_includes cpp, "test_filter"
    end

    # Test array.fold() method
    def test_array_fold
      source = <<~MLC
        fn test_fold() -> i32 = do
          let arr = [1, 2, 3, 4, 5]
          let sum = arr.fold(0, (acc, x) => acc + x)
          sum
        end
      MLC

      cpp = MLC.to_cpp(source)
      assert_includes cpp, "mlc::collections::fold"
      assert_includes cpp, "test_fold"
    end

    # Test composition: map then filter
    def test_map_filter_composition
      source = <<~MLC
        fn test_composition() -> i32[] = do
          let arr = [1, 2, 3, 4, 5, 6]
          let result = arr.map(x => x * 2).filter(y => y > 5)
          result
        end
      MLC

      cpp = MLC.to_cpp(source)
      assert_includes cpp, "mlc::collections::map"
      assert_includes cpp, "mlc::collections::filter"
      assert_includes cpp, "test_composition"
    end

    # Test complex lambda in map
    def test_map_complex_lambda
      source = <<~MLC
        fn test_complex() -> i32[] = do
          let arr = [1, 2, 3]
          let result = arr.map(x => if x > 1 then x * 10 else x)
          result
        end
      MLC

      cpp = MLC.to_cpp(source)
      assert_includes cpp, "mlc::collections::map"
      assert_includes cpp, "test_complex"
    end
  end
end
