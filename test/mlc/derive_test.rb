# frozen_string_literal: true

require_relative "../test_helper"
require_relative "../../lib/mlc/common/index"

class MLCDeriveTest < Minitest::Test
  def test_derive_display_record_generates_to_string
    cpp = MLC.to_cpp(<<~MLC)
      type Point = { x: i32, y: i32 } derive { Display }
      fn main() -> string = Point { x: 1, y: 2 }.to_string()
    MLC
    assert_includes cpp, "Point_to_string"
    assert_includes cpp, "Point { x: "
  end

  def test_derive_eq_record_generates_operator_eq
    cpp = MLC.to_cpp(<<~MLC)
      type Point = { x: i32, y: i32 } derive { Eq }
      fn eq(a: Point, b: Point) -> bool = a == b
      fn main() -> bool = eq(Point { x: 1, y: 1 }, Point { x: 1, y: 1 })
    MLC
    assert_includes cpp, "operator=="
    assert_includes cpp, "a.x == b.x"
  end

  def test_derive_display_and_eq_together
    cpp = MLC.to_cpp(<<~MLC)
      type Color = { r: i32, g: i32, b: i32 } derive { Display, Eq }
      fn main() -> string = Color { r: 255, g: 0, b: 0 }.to_string()
    MLC
    assert_includes cpp, "Color_to_string"
    assert_includes cpp, "operator=="
  end

  def test_derive_display_string_field
    cpp = MLC.to_cpp(<<~MLC)
      type Named = { name: string, age: i32 } derive { Display }
      fn main() -> string = Named { name: "Alice", age: 30 }.to_string()
    MLC
    assert_includes cpp, "Named_to_string"
    assert_includes cpp, "self.name"
    assert_includes cpp, "mlc::to_string(self.age)"
  end

  def test_derive_display_sum_type
    cpp = MLC.to_cpp(<<~MLC)
      type Shape = Circle(i32) | Square(i32) derive { Display }
      fn main() -> string = Circle(5).to_string()
    MLC
    assert_includes cpp, "Shape_to_string"
    assert_includes cpp, "Circle"
  end

  def test_derive_eq_sum_type
    cpp = MLC.to_cpp(<<~MLC)
      type Dir = North | South | East | West derive { Eq }
      fn same(a: Dir, b: Dir) -> bool = a == b
      fn main() -> bool = same(North, North)
    MLC
    assert_includes cpp, "operator=="
  end

  def test_no_derive_no_extra_methods
    cpp = MLC.to_cpp(<<~MLC)
      type Point = { x: i32, y: i32 }
      fn main() -> i32 = Point { x: 1, y: 2 }.x
    MLC
    refute_includes cpp, "Point_to_string"
  end

  def test_derive_ord_record
    cpp = MLC.to_cpp(<<~MLC)
      type Pair = { a: i32, b: i32 } derive { Ord }
      fn main() -> i32 = 0
    MLC
    assert_includes cpp, "operator<"
  end

  def test_derive_display_empty_record
    cpp = MLC.to_cpp(<<~MLC)
      type Empty = { } derive { Display }
      fn show(e: Empty) -> string = e.to_string()
      fn main() -> i32 = 0
    MLC
    assert_includes cpp, "Empty_to_string"
    assert_includes cpp, "\"Empty {}\""
  end
end
