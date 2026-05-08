# frozen_string_literal: true

require "test_helper"

module MLC
  # Tests that extend-generated methods on exported types are declared in the header.
  class ExtendHeaderTest < Minitest::Test
    def test_extend_method_on_exported_type_appears_in_header
      source = <<~MLC
        export type Point = Point { x: i32, y: i32 }

        extend Point {
          fn sum(self: Point) -> i32 = self.x + self.y
        }

        export fn make_point(x: i32, y: i32) -> Point = Point { x: x, y: y }
      MLC

      result = MLC.to_hpp_cpp(source)
      assert_includes result[:header], "Point_sum",
        "extend method on exported type must be declared in header"
    end

    def test_extend_method_on_non_exported_type_stays_in_impl
      source = <<~MLC
        type Internal = Internal { value: i32 }

        extend Internal {
          fn get(self: Internal) -> i32 = self.value
        }

        export fn make_internal(value: i32) -> Internal = Internal { value: value }
      MLC

      result = MLC.to_hpp_cpp(source)
      refute_includes result[:header], "Internal_get",
        "extend method on non-exported type must NOT be declared in header"
      assert_includes result[:implementation], "Internal_get",
        "extend method must still be defined in implementation"
    end

    def test_extend_method_callable_cross_module
      # Simulate: module A defines type + extend, module B imports and uses instance method.
      # Since to_hpp_cpp compiles single-module, we verify the declaration is in the header
      # (which is what enables cross-module calls).
      source = <<~MLC
        export type Counter = Counter { count: i32 }

        extend Counter {
          fn increment(self: Counter) -> Counter = Counter { count: self.count + 1 }
          fn value(self: Counter) -> i32 = self.count
        }

        export fn new_counter() -> Counter = Counter { count: 0 }
      MLC

      result = MLC.to_hpp_cpp(source)
      assert_includes result[:header], "Counter_increment",
        "Counter_increment must be in header for cross-module use"
      assert_includes result[:header], "Counter_value",
        "Counter_value must be in header for cross-module use"
    end
  end
end
