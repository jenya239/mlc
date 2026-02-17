# frozen_string_literal: true

require "test_helper"

class InstanceMethodDispatchTest < Minitest::Test
  def test_instance_method_on_record
    code = <<~MLC
      type Point = { x: i32, y: i32 }

      extend Point {
        fn translate(self: Point, dx: i32, dy: i32) -> Point =
          Point { x: self.x + dx, y: self.y + dy }
      }

      fn main() -> i32 = do
        let p = Point { x: 1, y: 2 }
        let p2 = p.translate(10, 20)
        p2.x
      end
    MLC

    cpp = compile_to_cpp(code)
    assert_includes cpp, "Point_translate(p, 10, 20)"
  end

  def test_instance_method_no_extra_args
    code = <<~MLC
      type Counter = { value: i32 }

      extend Counter {
        fn get(self: Counter) -> i32 = self.value
      }

      fn main() -> i32 = do
        let c = Counter { value: 42 }
        c.get()
      end
    MLC

    cpp = compile_to_cpp(code)
    assert_includes cpp, "Counter_get(c)"
  end

  def test_chained_instance_methods
    code = <<~MLC
      type Vec2 = { x: i32, y: i32 }

      extend Vec2 {
        fn add_vec(self: Vec2, other: Vec2) -> Vec2 =
          Vec2 { x: self.x + other.x, y: self.y + other.y }
      }

      fn main() -> i32 = do
        let a = Vec2 { x: 1, y: 2 }
        let b = Vec2 { x: 3, y: 4 }
        let c = Vec2 { x: 5, y: 6 }
        let result = a.add_vec(b).add_vec(c)
        result.x
      end
    MLC

    cpp = compile_to_cpp(code)
    assert_includes cpp, "Vec2_add_vec("
  end

  def test_static_and_instance_methods_coexist
    code = <<~MLC
      type Pair = { a: i32, b: i32 }

      extend Pair {
        fn zero() -> Pair = Pair { a: 0, b: 0 }
        fn sum(self: Pair) -> i32 = self.a + self.b
      }

      fn main() -> i32 = do
        let p = Pair.zero()
        p.sum()
      end
    MLC

    cpp = compile_to_cpp(code)
    assert_includes cpp, "Pair_zero()"
    assert_includes cpp, "Pair_sum(p)"
  end

  def test_instance_method_with_trait
    code = <<~MLC
      type MyInt = { value: i32 }

      trait Describable {
        fn describe(self: Self) -> i32
      }

      extend MyInt : Describable {
        fn describe(self: MyInt) -> i32 = self.value
      }

      fn main() -> i32 = do
        let x = MyInt { value: 99 }
        x.describe()
      end
    MLC

    cpp = compile_to_cpp(code)
    assert_includes cpp, "MyInt_describe(x)"
  end

  def test_instance_method_error_on_nonexistent
    code = <<~MLC
      type Foo = { x: i32 }

      fn main() -> i32 = do
        let f = Foo { x: 1 }
        f.nonexistent()
      end
    MLC

    err = assert_raises(MLC::CompileError) { compile_to_cpp(code) }
    assert_match(/Unknown field 'nonexistent'/, err.message)
  end

  private

  def compile_to_cpp(code)
    MLC.to_cpp(code)
  end
end
