# frozen_string_literal: true

require "minitest/autorun"
require_relative "../../lib/mlc/common/index"

class ArrayMutationTest < Minitest::Test
  # =========================================================================
  # push
  # =========================================================================

  def test_push_on_mut_array
    source = <<~MLC
      fn main() -> i32 = do
        let mut arr = [1, 2, 3]
        arr.push(4)
        arr.length()
      end
    MLC
    cpp = MLC.to_cpp(source)
    assert_includes cpp, "arr.push_back(4)"
  end

  def test_push_on_immutable_array_errors
    source = <<~MLC
      fn main() -> i32 = do
        let arr = [1, 2, 3]
        arr.push(4)
        arr.length()
      end
    MLC
    error = assert_raises(MLC::CompileError) { MLC.to_cpp(source) }
    assert_match(/Cannot call .push\(\) on immutable binding 'arr'/, error.message)
  end

  def test_push_multiple_values
    source = <<~MLC
      fn main() -> i32 = do
        let mut arr = [1]
        arr.push(2)
        arr.push(3)
        arr.push(4)
        arr.length()
      end
    MLC
    cpp = MLC.to_cpp(source)
    assert_includes cpp, "arr.push_back(2)"
    assert_includes cpp, "arr.push_back(3)"
    assert_includes cpp, "arr.push_back(4)"
  end

  def test_push_string_array
    source = <<~MLC
      fn main() -> i32 = do
        let mut names = ["alice", "bob"]
        names.push("charlie")
        names.length()
      end
    MLC
    cpp = MLC.to_cpp(source)
    assert_includes cpp, "names.push_back"
  end

  # =========================================================================
  # pop
  # =========================================================================

  def test_pop_on_mut_array
    source = <<~MLC
      fn main() -> i32 = do
        let mut arr = [1, 2, 3]
        arr.pop()
        arr.length()
      end
    MLC
    cpp = MLC.to_cpp(source)
    assert_includes cpp, "arr.pop_back()"
  end

  def test_pop_on_immutable_array_errors
    source = <<~MLC
      fn main() -> i32 = do
        let arr = [1, 2, 3]
        arr.pop()
        arr.length()
      end
    MLC
    error = assert_raises(MLC::CompileError) { MLC.to_cpp(source) }
    assert_match(/Cannot call .pop\(\) on immutable binding 'arr'/, error.message)
  end

  # =========================================================================
  # set
  # =========================================================================

  def test_set_on_mut_array
    source = <<~MLC
      fn main() -> i32 = do
        let mut arr = [10, 20, 30]
        arr.set(0, 99)
        arr[0]
      end
    MLC
    cpp = MLC.to_cpp(source)
    assert_includes cpp, "arr.set(0, 99)"
  end

  def test_set_on_immutable_array_errors
    source = <<~MLC
      fn main() -> i32 = do
        let arr = [10, 20, 30]
        arr.set(0, 99)
        arr[0]
      end
    MLC
    error = assert_raises(MLC::CompileError) { MLC.to_cpp(source) }
    assert_match(/Cannot call .set\(\) on immutable binding 'arr'/, error.message)
  end

  # =========================================================================
  # get
  # =========================================================================

  def test_get_on_array
    source = <<~MLC
      fn main() -> i32 = do
        let arr = [10, 20, 30]
        arr.get(1)
      end
    MLC
    cpp = MLC.to_cpp(source)
    assert_includes cpp, "arr[1]"
  end

  # =========================================================================
  # Immutable operations still work on let
  # =========================================================================

  def test_immutable_ops_on_let
    source = <<~MLC
      fn main() -> i32 = do
        let arr = [1, 2, 3]
        arr.length()
      end
    MLC
    cpp = MLC.to_cpp(source)
    assert_includes cpp, "arr.size()"
  end

  def test_immutable_contains_on_let
    source = <<~MLC
      fn main() -> bool = do
        let arr = [1, 2, 3]
        arr.contains(2)
      end
    MLC
    cpp = MLC.to_cpp(source)
    assert_includes cpp, "contains"
  end

  # =========================================================================
  # Function parameter mutability
  # =========================================================================

  def test_push_on_function_param_errors
    source = <<~MLC
      fn add_item(arr: i32[]) -> i32 = do
        arr.push(42)
        arr.length()
      end

      fn main() -> i32 = add_item([1, 2])
    MLC
    error = assert_raises(MLC::CompileError) { MLC.to_cpp(source) }
    assert_match(/Cannot call .push\(\) on immutable binding 'arr'/, error.message)
  end

  # =========================================================================
  # Combination: push + length
  # =========================================================================

  def test_push_then_length
    source = <<~MLC
      fn main() -> i32 = do
        let mut items = [10, 20]
        items.push(30)
        items.push(40)
        items.length()
      end
    MLC
    cpp = MLC.to_cpp(source)
    assert_includes cpp, "items.push_back(30)"
    assert_includes cpp, "items.push_back(40)"
    assert_includes cpp, "items.size()"
  end
end
