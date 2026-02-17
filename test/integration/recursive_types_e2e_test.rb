# frozen_string_literal: true

require_relative "../test_helper"
require "open3"
require "tmpdir"

class RecursiveTypesE2ETest < Minitest::Test
  tag :slow

  CLI = File.expand_path("../../bin/mlc", __dir__)

  def run_mlc(source_code)
    Dir.mktmpdir do |dir|
      source = File.join(dir, "test.mlc")
      File.write(source, source_code)
      stdout, stderr, status = Open3.capture3(CLI, source)
      refute_includes stderr, "error:", "Compilation failed: #{stderr}"
      yield stdout, stderr, status if block_given?
    end
  end

  def test_recursive_linked_list
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      type Node = { value: i32, next: Shared<Node> }

      extend Shared<T> {
        extern fn new(value: T) -> Shared<T>
        extern fn null() -> Shared<T>
      }

      fn sum_list(node: Shared<Node>) -> i32 = node.value

      fn main() -> i32 = do
        let n = Shared.new(Node { value: 42, next: Shared.null() })
        sum_list(n)
      end
    MLC
      assert_equal 42, status.exitstatus
    end
  end

  def test_recursive_tree
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      type Tree = { value: i32, left: Shared<Tree>, right: Shared<Tree> }

      extend Shared<T> {
        extern fn new(value: T) -> Shared<T>
        extern fn null() -> Shared<T>
      }

      fn get_value(tree: Shared<Tree>) -> i32 = tree.value

      fn main() -> i32 = do
        let leaf = Shared.new(Tree { value: 100, left: Shared.null(), right: Shared.null() })
        get_value(leaf)
      end
    MLC
      assert_equal 100, status.exitstatus
    end
  end
end
