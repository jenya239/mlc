# frozen_string_literal: true

require_relative "../test_helper"
require_relative "../../lib/mlc/common/index"

class MLCD1AssocTypesTest < Minitest::Test
  def test_struct_with_assoc_type_binding_contains_using
    src = <<~MLC
      trait Iterator {
        type Item
        fn next(self: Iterator) -> i32
      }
      type Box<T> = { val: T }
      extend Box<T>: Iterator {
        type Item = T
        fn next(self: Box<T>) -> i32 = 0
      }
      fn main() -> i32 = 0
    MLC

    cpp = MLC.to_cpp(src)
    assert_includes cpp, "using Item ="
  end

  def test_generic_fn_assoc_type_ref_generates_typename
    src = <<~MLC
      trait Iterator {
        type Item
        fn next(self: Iterator) -> i32
      }
      type Box<T> = { val: T }
      extend Box<T>: Iterator {
        type Item = T
        fn next(self: Box<T>) -> i32 = 0
      }
      fn first<I: Iterator>(x: I) -> I.Item = x
      fn main() -> i32 = 0
    MLC

    cpp = MLC.to_cpp(src)
    assert_includes cpp, "typename I::Item"
  end
end
