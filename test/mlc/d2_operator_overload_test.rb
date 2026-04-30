# frozen_string_literal: true

require_relative "../test_helper"
require_relative "../../lib/mlc/common/index"

class MLCD2OperatorOverloadTest < Minitest::Test
  def test_add_desugars_to_type_method
    src = <<~MLC
      type Vec2 = { x: i32, y: i32 }
      extend Vec2 : Add<Vec2> {
        type Output = Vec2
        fn add(self: Vec2, rhs: Vec2) -> Vec2 = Vec2 { x: self.x + rhs.x, y: self.y + rhs.y }
      }
      fn test(a: Vec2, b: Vec2) -> Vec2 = a + b
      fn main() -> i32 = 0
    MLC
    cpp = MLC.to_cpp(src)
    assert_includes cpp, "Vec2_add(a, b)"
  end

  def test_output_type_used_as_return_type
    src = <<~MLC
      type Vec2 = { x: i32, y: i32 }
      extend Vec2 : Add<Vec2> {
        type Output = Vec2
        fn add(self: Vec2, rhs: Vec2) -> Vec2 = Vec2 { x: self.x + rhs.x, y: self.y + rhs.y }
      }
      fn test(a: Vec2, b: Vec2) -> Vec2 = a + b
      fn main() -> i32 = 0
    MLC
    cpp = MLC.to_cpp(src)
    assert_includes cpp, "Vec2 test(Vec2 a, Vec2 b)"
  end

  def test_struct_has_using_output
    src = <<~MLC
      type Vec2 = { x: i32, y: i32 }
      extend Vec2 : Add<Vec2> {
        type Output = Vec2
        fn add(self: Vec2, rhs: Vec2) -> Vec2 = Vec2 { x: self.x + rhs.x, y: self.y + rhs.y }
      }
      fn main() -> i32 = 0
    MLC
    cpp = MLC.to_cpp(src)
    assert_includes cpp, "using Output = Vec2"
  end

  def test_primitive_plus_unaffected
    src = <<~MLC
      fn test(a: i32, b: i32) -> i32 = a + b
      fn main() -> i32 = 0
    MLC
    cpp = MLC.to_cpp(src)
    assert_includes cpp, "a + b"
    refute_includes cpp, "_add("
  end
end
