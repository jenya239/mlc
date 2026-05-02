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

  def test_derive_hash_record_generates_std_hash_specialization
    cpp = MLC.to_cpp(<<~MLC)
      type Cell = { tag: i32, flag: bool, label: string } derive { Hash }
      fn main() -> i32 = 0
    MLC
    assert_includes cpp, "namespace std"
    assert_includes cpp, "struct hash<Cell>"
    assert_includes cpp, "std::hash<int>{}(self.tag)"
    assert_includes cpp, "std::hash<bool>{}(self.flag)"
    assert_includes cpp, "std::hash<mlc::String>{}(self.label)"
    assert_includes cpp, "0x9e3779b97f4a7c15ULL"
  end

  def test_derive_hash_empty_record_constant_seed
    cpp = MLC.to_cpp(<<~MLC)
      type Empty = { } derive { Hash }
      fn main() -> i32 = 0
    MLC
    assert_includes cpp, "struct hash<Empty>"
    assert_includes cpp, "1469598103934665603ULL"
  end

  def test_derive_hash_sum_type_units_and_tuple_payload
    cpp = MLC.to_cpp(<<~MLC)
      type Shape = Circle(i32) | Empty derive { Hash }
      fn main() -> i32 = 0
    MLC
    assert_includes cpp, "struct hash<Shape>"
    assert_includes cpp, "std::holds_alternative<Circle>"
    assert_includes cpp, "std::holds_alternative<Empty>"
    assert_includes cpp, "std::get<Circle>(self._).field0"
  end

  def test_derive_hash_as_hash_map_key_emits_hash_map_type
    cpp = MLC.to_cpp(<<~MLC)
      type PointKey = { x: i32, y: i32 } derive { Eq, Hash }
      fn main() -> i32 = do
        let table: Map<PointKey, i32> = Map.new()
        table.set(PointKey { x: 1, y: 2 }, 1)
        0
      end
    MLC
    assert_includes cpp, "mlc::HashMap<PointKey"
  end

  def test_derive_unknown_trait_raises
    source = <<~MLC
      type Bad = { x: i32 } derive { NotATrait }
      fn main() -> i32 = 0
    MLC
    error = assert_raises(MLC::CompileError) { MLC.to_cpp(source) }
    assert_includes error.message, "unknown derive trait \"NotATrait\""
  end

  def test_derive_duplicate_trait_raises
    source = <<~MLC
      type Bad = { x: i32 } derive { Eq, Eq }
      fn main() -> i32 = 0
    MLC
    error = assert_raises(MLC::CompileError) { MLC.to_cpp(source) }
    assert_includes error.message, "duplicate derive trait \"Eq\""
  end

  def test_derive_hash_wrong_case_trait_name_raises
    source = <<~MLC
      type Bad = { x: i32 } derive { hash }
      fn main() -> i32 = 0
    MLC
    error = assert_raises(MLC::CompileError) { MLC.to_cpp(source) }
    assert_includes error.message, "unknown derive trait \"hash\""
  end

  def test_derive_hash_generic_type_raises
    source = <<~MLC
      type Box<T> = { value: T } derive { Hash }
      fn main() -> i32 = 0
    MLC
    error = assert_raises(MLC::CompileError) { MLC.to_cpp(source) }
    assert_includes error.message, "derive Hash is not supported for generic types in this version"
  end

  def test_derive_hash_nested_named_field_raises
    source = <<~MLC
      type Inner = { x: i32 }
      type Outer = { inner: Inner } derive { Hash }
      fn main() -> i32 = 0
    MLC
    error = assert_raises(MLC::CompileError) { MLC.to_cpp(source) }
    assert_includes error.message, "derive Hash: unsupported field type for \"inner\""
  end
end
