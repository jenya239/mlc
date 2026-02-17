# frozen_string_literal: true

require_relative "../test_helper"
require "open3"
require "tmpdir"

class InstanceMethodE2ETest < Minitest::Test
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

  # =========================================================================
  # Basic instance method dispatch
  # =========================================================================

  def test_instance_method_returns_field
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      type Counter = { value: i32 }

      extend Counter {
        fn get(self: Counter) -> i32 = self.value
      }

      fn main() -> i32 = do
        let c = Counter { value: 42 }
        c.get()
      end
    MLC
      assert_equal 42, status.exitstatus
    end
  end

  def test_instance_method_with_args
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
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
      assert_equal 11, status.exitstatus
    end
  end

  def test_instance_method_chained
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      type Vec2 = { x: i32, y: i32 }

      extend Vec2 {
        fn add_vec(self: Vec2, other: Vec2) -> Vec2 =
          Vec2 { x: self.x + other.x, y: self.y + other.y }
      }

      fn main() -> i32 = do
        let a = Vec2 { x: 1, y: 0 }
        let b = Vec2 { x: 2, y: 0 }
        let c = Vec2 { x: 3, y: 0 }
        a.add_vec(b).add_vec(c).x
      end
    MLC
      assert_equal 6, status.exitstatus
    end
  end

  # =========================================================================
  # Static + instance coexist
  # =========================================================================

  def test_static_and_instance_on_same_type
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      type Pair = { a: i32, b: i32 }

      extend Pair {
        fn zero() -> Pair = Pair { a: 0, b: 0 }
        fn sum(self: Pair) -> i32 = self.a + self.b
      }

      fn main() -> i32 = do
        let p = Pair { a: 10, b: 20 }
        p.sum()
      end
    MLC
      assert_equal 30, status.exitstatus
    end
  end

  # =========================================================================
  # Instance method with trait
  # =========================================================================

  def test_instance_method_via_trait
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      type MyVal = { x: i32 }

      trait Gettable {
        fn get_val(self: Self) -> i32
      }

      extend MyVal : Gettable {
        fn get_val(self: MyVal) -> i32 = self.x
      }

      fn main() -> i32 = do
        let v = MyVal { x: 77 }
        v.get_val()
      end
    MLC
      assert_equal 77, status.exitstatus
    end
  end

  # =========================================================================
  # Instance method computing value
  # =========================================================================

  def test_instance_method_arithmetic
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      type Rect = { w: i32, h: i32 }

      extend Rect {
        fn area(self: Rect) -> i32 = self.w * self.h
        fn perimeter(self: Rect) -> i32 = (self.w + self.h) * 2
      }

      fn main() -> i32 = do
        let r = Rect { w: 3, h: 4 }
        r.area()
      end
    MLC
      assert_equal 12, status.exitstatus
    end
  end
end
