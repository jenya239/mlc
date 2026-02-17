# frozen_string_literal: true

require "minitest/autorun"
require_relative "../../lib/mlc/common/index"

class DestructuringTest < Minitest::Test
  def test_tuple_destructuring
    source = <<~MLC
      fn main() -> i32 = do
        let (a, b) = (10, 20)
        a + b
      end
    MLC
    cpp = MLC.to_cpp(source)
    assert_includes cpp, "auto [a, b] = std::make_tuple(10, 20)"
  end

  def test_tuple_destructuring_three_elements
    source = <<~MLC
      fn main() -> i32 = do
        let (x, y, z) = (1, 2, 3)
        x + y + z
      end
    MLC
    cpp = MLC.to_cpp(source)
    assert_includes cpp, "auto [x, y, z] = std::make_tuple(1, 2, 3)"
  end

  def test_record_destructuring
    source = <<~MLC
      type Point = { x: i32, y: i32 }
      fn main() -> i32 = do
        let p = Point { x: 10, y: 20 }
        let { x, y } = p
        x + y
      end
    MLC
    cpp = MLC.to_cpp(source)
    assert_includes cpp, "auto __tmp_0 = p"
    assert_includes cpp, "auto x = __tmp_0.x"
    assert_includes cpp, "auto y = __tmp_0.y"
  end

end
