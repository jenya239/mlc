# frozen_string_literal: true

require "test_helper"

module MLC
  class ArrayHofTest < Minitest::Test
    def test_array_map
      source = <<~MLC
        fn test_map() -> i32[] = do
          let arr = [1, 2, 3, 4, 5]
          let doubled = arr.map(x => x * 2)
          doubled
        end
      MLC

      cpp = MLC.to_cpp(source)
      assert_includes cpp, ".map("
      assert_includes cpp, "test_map"
    end

    def test_array_filter
      source = <<~MLC
        fn test_filter() -> i32[] = do
          let arr = [1, 2, 3, 4, 5]
          let evens = arr.filter(x => x % 2 == 0)
          evens
        end
      MLC

      cpp = MLC.to_cpp(source)
      assert_includes cpp, ".filter("
      assert_includes cpp, "test_filter"
    end

    def test_array_fold
      source = <<~MLC
        fn test_fold() -> i32 = do
          let arr = [1, 2, 3, 4, 5]
          let sum = arr.fold(0, (acc, x) => acc + x)
          sum
        end
      MLC

      cpp = MLC.to_cpp(source)
      assert_includes cpp, ".fold("
      assert_includes cpp, "test_fold"
    end

    def test_map_filter_composition
      source = <<~MLC
        fn test_composition() -> i32[] = do
          let arr = [1, 2, 3, 4, 5, 6]
          let result = arr.map(x => x * 2).filter(y => y > 5)
          result
        end
      MLC

      cpp = MLC.to_cpp(source)
      assert_includes cpp, ".map("
      assert_includes cpp, ".filter("
      assert_includes cpp, "test_composition"
    end

    def test_map_complex_lambda
      source = <<~MLC
        fn test_complex() -> i32[] = do
          let arr = [1, 2, 3]
          let result = arr.map(x => if x > 1 then x * 10 else x)
          result
        end
      MLC

      cpp = MLC.to_cpp(source)
      assert_includes cpp, ".map("
      assert_includes cpp, "test_complex"
    end

    SHARED_NODE_TYPE = <<~MLC
      type Node = { name: string, value: i32 }
    MLC

    def test_flat_map_shared_lambda_param_type
      source = SHARED_NODE_TYPE + <<~MLC
        fn test_fn(nodes: [Shared<Node>]) -> [string] =
          nodes.flat_map(node => [node.name])
      MLC

      cpp = MLC.to_cpp(source)
      assert_includes cpp, "flat_map"
      assert_match(/\[\]\(std::shared_ptr<\w+> node\)/, cpp)
      refute_match(/\[\]\(int32_t node\)/, cpp)
    end

    def test_any_shared_lambda_param_type
      source = SHARED_NODE_TYPE + <<~MLC
        fn test_fn(nodes: [Shared<Node>]) -> bool =
          nodes.any(node => node.value > 0)
      MLC

      cpp = MLC.to_cpp(source)
      assert_includes cpp, ".any("
      assert_match(/\[\]\(std::shared_ptr<\w+> node\)/, cpp)
      refute_match(/\[\]\(int32_t node\)/, cpp)
    end

    def test_all_shared_lambda_param_type
      source = SHARED_NODE_TYPE + <<~MLC
        fn test_fn(nodes: [Shared<Node>]) -> bool =
          nodes.all(node => node.value > 0)
      MLC

      cpp = MLC.to_cpp(source)
      assert_includes cpp, ".all("
      assert_match(/\[\]\(std::shared_ptr<\w+> node\)/, cpp)
      refute_match(/\[\]\(int32_t node\)/, cpp)
    end

    def test_none_shared_lambda_param_type
      source = SHARED_NODE_TYPE + <<~MLC
        fn test_fn(nodes: [Shared<Node>]) -> bool =
          nodes.none(node => node.value > 0)
      MLC

      cpp = MLC.to_cpp(source)
      assert_includes cpp, ".none("
      assert_match(/\[\]\(std::shared_ptr<\w+> node\)/, cpp)
      refute_match(/\[\]\(int32_t node\)/, cpp)
    end

    def test_find_shared_lambda_param_type
      source = SHARED_NODE_TYPE + <<~MLC
        fn test_fn(nodes: [Shared<Node>]) -> Shared<Node> =
          nodes.find(node => node.value > 0)
      MLC

      cpp = MLC.to_cpp(source)
      assert_includes cpp, "find"
      assert_match(/\[\]\(std::shared_ptr<\w+> node\)/, cpp)
      refute_match(/\[\]\(int32_t node\)/, cpp)
    end

    def test_sort_by_shared_lambda_param_type
      source = SHARED_NODE_TYPE + <<~MLC
        fn test_fn(nodes: [Shared<Node>]) -> [Shared<Node>] =
          nodes.sort_by(node => node.value)
      MLC

      cpp = MLC.to_cpp(source)
      assert_includes cpp, "sort_by"
      assert_match(/\[\]\(std::shared_ptr<\w+> node\)/, cpp)
      refute_match(/\[\]\(int32_t node\)/, cpp)
    end

    def test_enumerate_length_chain
      source = <<~MLC
        fn test_fn(a: [i32]) -> i32 = a.enumerate().length()
      MLC

      cpp = MLC.to_cpp(source)
      assert_includes cpp, "enumerate"
      assert_includes cpp, "size()"
    end

    def test_zip_length_chain
      source = <<~MLC
        fn test_fn(a: [i32], b: [string]) -> i32 = a.zip(b).length()
      MLC

      cpp = MLC.to_cpp(source)
      assert_includes cpp, "zip"
      assert_includes cpp, "size()"
    end

    def test_filter_shared_lambda_param_type
      source = SHARED_NODE_TYPE + <<~MLC
        fn test_fn(nodes: [Shared<Node>]) -> [Shared<Node>] =
          nodes.filter(node => node.value > 0)
      MLC

      cpp = MLC.to_cpp(source)
      assert_includes cpp, ".filter("
      assert_match(/\[\]\(std::shared_ptr<\w+> node\)/, cpp)
      refute_match(/\[\]\(int32_t node\)/, cpp)
    end
  end
end
