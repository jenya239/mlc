# frozen_string_literal: true

require_relative '../test_helper'
require 'open3'
require 'tmpdir'

class SmartPointersE2ETest < Minitest::Test
  tag :slow  # Requires C++ compilation

  CLI = File.expand_path("../../bin/mlc", __dir__)

  # Helper to run MLC program and check result
  def run_mlc(source_code, &block)
    Dir.mktmpdir do |dir|
      source = File.join(dir, "test.mlc")
      File.write(source, source_code)
      stdout, stderr, status = Open3.capture3(CLI, source)

      refute_includes stderr, "error:", "Compilation failed: #{stderr}"
      yield stdout, stderr, status if block_given?
    end
  end

  # ===========================================================================
  # Shared<T> Tests
  # ===========================================================================

  # Test 1: Basic Shared.new
  def test_shared_new_basic
    run_mlc(<<~MLC) do |stdout, stderr, status|
      type Node = { value: i32 }

      extend Shared<T> {
        extern fn new(value: T) -> Shared<T>
      }

      fn main() -> i32 = do
        let node = Shared.new(Node { value: 42 })
        node.value
      end
    MLC
      assert_equal 42, status.exitstatus
    end
  end

  # Test 2: Shared.null returns null pointer
  def test_shared_null
    run_mlc(<<~MLC) do |stdout, stderr, status|
      type Node = { value: i32 }

      extend Shared<T> {
        extern fn new(value: T) -> Shared<T>
        extern fn null() -> Shared<T>
      }

      fn main() -> i32 = do
        let _null_ptr: Shared<Node> = Shared.null()
        // null_ptr would be nullptr in C++
        // Just verify compilation works
        let valid = Shared.new(Node { value: 100 })
        valid.value
      end
    MLC
      assert_equal 100, status.exitstatus
    end
  end

  # Test 3: Shared pointer passed as function argument
  def test_shared_as_argument
    run_mlc(<<~MLC) do |stdout, stderr, status|
      type Node = { value: i32 }

      extend Shared<T> {
        extern fn new(value: T) -> Shared<T>
      }

      fn get_value(n: Shared<Node>) -> i32 = n.value

      fn main() -> i32 = do
        let node = Shared.new(Node { value: 77 })
        get_value(node)
      end
    MLC
      assert_equal 77, status.exitstatus
    end
  end

  # Test 4: Multiple shared references to same object
  def test_shared_multiple_references
    run_mlc(<<~MLC) do |stdout, stderr, status|
      type Node = { value: i32 }

      extend Shared<T> {
        extern fn new(value: T) -> Shared<T>
      }

      fn main() -> i32 = do
        let node = Shared.new(Node { value: 10 })
        let alias1 = node
        let alias2 = node
        // All three should point to same object
        node.value + alias1.value + alias2.value
      end
    MLC
      assert_equal 30, status.exitstatus
    end
  end

  # Test 5: Shared with generic type containing i32
  def test_shared_with_different_types
    run_mlc(<<~MLC) do |stdout, stderr, status|
      type IntBox = { value: i32 }
      type Pair = { first: i32, second: i32 }

      extend Shared<T> {
        extern fn new(value: T) -> Shared<T>
      }

      fn main() -> i32 = do
        let box = Shared.new(IntBox { value: 25 })
        let pair = Shared.new(Pair { first: 10, second: 15 })
        box.value + pair.first + pair.second
      end
    MLC
      assert_equal 50, status.exitstatus
    end
  end

  # ===========================================================================
  # Weak<T> Tests
  # ===========================================================================

  # Test 6: Shared.downgrade to Weak
  def test_shared_downgrade_to_weak
    run_mlc(<<~MLC) do |stdout, stderr, status|
      type Node = { value: i32 }

      extend Shared<T> {
        extern fn new(value: T) -> Shared<T>
        extern fn downgrade(shared: Shared<T>) -> Weak<T>
      }

      extend Weak<T> {
        extern fn is_valid(weak: Weak<T>) -> bool
      }

      fn main() -> i32 = do
        let shared = Shared.new(Node { value: 42 })
        let weak = Shared.downgrade(shared)
        // While shared exists, weak should be valid
        if Weak.is_valid(weak) then 1 else 0
      end
    MLC
      assert_equal 1, status.exitstatus
    end
  end

  # Test 7: Weak.null
  def test_weak_null
    run_mlc(<<~MLC) do |stdout, stderr, status|
      type Node = { value: i32 }

      extend Weak<T> {
        extern fn null() -> Weak<T>
        extern fn is_valid(weak: Weak<T>) -> bool
      }

      fn main() -> i32 = do
        let null_weak: Weak<Node> = Weak.null()
        // null weak should not be valid
        if Weak.is_valid(null_weak) then 1 else 0
      end
    MLC
      assert_equal 0, status.exitstatus
    end
  end

  # Test 8: Weak.lock returns Option
  def test_weak_lock_some
    run_mlc(<<~MLC) do |stdout, stderr, status|
      type Node = { value: i32 }

      extend Shared<T> {
        extern fn new(value: T) -> Shared<T>
        extern fn downgrade(shared: Shared<T>) -> Weak<T>
      }

      extend Weak<T> {
        extern fn lock(weak: Weak<T>) -> Option<Shared<T>>
      }

      fn main() -> i32 = do
        let shared = Shared.new(Node { value: 88 })
        let weak = Shared.downgrade(shared)
        match Weak.lock(weak) {
          Some(locked) => locked.value,
          None => 0
        }
      end
    MLC
      assert_equal 88, status.exitstatus
    end
  end

  # Test 9: Weak.lock on null returns None
  def test_weak_lock_none
    run_mlc(<<~MLC) do |stdout, stderr, status|
      type Node = { value: i32 }

      extend Weak<T> {
        extern fn null() -> Weak<T>
        extern fn lock(weak: Weak<T>) -> Option<Shared<T>>
      }

      fn main() -> i32 = do
        let null_weak: Weak<Node> = Weak.null()
        match Weak.lock(null_weak) {
          Some(_) => 1,
          None => 0
        }
      end
    MLC
      assert_equal 0, status.exitstatus
    end
  end

  # ===========================================================================
  # Owned<T> Tests
  # ===========================================================================

  # Test 10: Basic Owned.new
  def test_owned_new_basic
    run_mlc(<<~MLC) do |stdout, stderr, status|
      type Buffer = { size: i32 }

      extend Owned<T> {
        extern fn new(value: T) -> Owned<T>
      }

      fn main() -> i32 = do
        let buf = Owned.new(Buffer { size: 1024 })
        buf.size
      end
    MLC
      assert_equal 0, status.exitstatus  # 1024 mod 256 = 0
    end
  end

  # Test 11: Owned.null
  def test_owned_null
    run_mlc(<<~MLC) do |stdout, stderr, status|
      type Buffer = { size: i32 }

      extend Owned<T> {
        extern fn new(value: T) -> Owned<T>
        extern fn null() -> Owned<T>
      }

      fn main() -> i32 = do
        let _null_buf: Owned<Buffer> = Owned.null()
        let valid = Owned.new(Buffer { size: 50 })
        valid.size
      end
    MLC
      assert_equal 50, status.exitstatus
    end
  end

  # Test 12: Owned passed to function (move semantics)
  def test_owned_move_to_function
    run_mlc(<<~MLC) do |stdout, stderr, status|
      type Data = { value: i32 }

      extend Owned<T> {
        extern fn new(value: T) -> Owned<T>
      }

      fn consume(d: Owned<Data>) -> i32 = d.value

      fn main() -> i32 = do
        let data = Owned.new(Data { value: 65 })
        consume(data)
        // After this, data is moved and cannot be used
      end
    MLC
      assert_equal 65, status.exitstatus
    end
  end

  # ===========================================================================
  # Nested Smart Pointers
  # ===========================================================================

  # Test 13: Shared containing Shared (nested)
  def test_nested_shared_pointers
    run_mlc(<<~MLC) do |stdout, stderr, status|
      type Inner = { value: i32 }
      type Outer = { inner: Shared<Inner> }

      extend Shared<T> {
        extern fn new(value: T) -> Shared<T>
      }

      fn main() -> i32 = do
        let inner = Shared.new(Inner { value: 33 })
        let outer = Shared.new(Outer { inner: inner })
        outer.inner.value
      end
    MLC
      assert_equal 33, status.exitstatus
    end
  end

  # Test 14: Array of Shared pointers
  def test_array_of_shared_pointers
    run_mlc(<<~MLC) do |stdout, stderr, status|
      type Node = { value: i32 }

      extend Shared<T> {
        extern fn new(value: T) -> Shared<T>
      }

      fn main() -> i32 = do
        let n1 = Shared.new(Node { value: 10 })
        let n2 = Shared.new(Node { value: 20 })
        let n3 = Shared.new(Node { value: 30 })
        let nodes: Shared<Node>[] = [n1, n2, n3]
        nodes[0].value + nodes[1].value + nodes[2].value
      end
    MLC
      assert_equal 60, status.exitstatus
    end
  end

  # ===========================================================================
  # Complex Patterns
  # ===========================================================================

  # Test 15: Tree-like structure with Shared and Weak (avoiding cycles)
  # Bidirectional type inference: Weak.null() infers T from record field context
  def test_tree_structure_with_weak_parent
    run_mlc(<<~MLC) do |stdout, stderr, status|
      type TreeNode = {
        value: i32,
        parent: Weak<TreeNode>
      }

      extend Shared<T> {
        extern fn new(value: T) -> Shared<T>
        extern fn downgrade(shared: Shared<T>) -> Weak<T>
      }

      extend Weak<T> {
        extern fn null() -> Weak<T>
        extern fn lock(weak: Weak<T>) -> Option<Shared<T>>
      }

      fn main() -> i32 = do
        let root = Shared.new(TreeNode { value: 100, parent: Weak.null() })
        let child = Shared.new(TreeNode { value: 50, parent: Shared.downgrade(root) })

        // Check child's parent
        match Weak.lock(child.parent) {
          Some(p) => p.value + child.value,
          None => 0
        }
      end
    MLC
      assert_equal 150, status.exitstatus
    end
  end

  # Test 16: Factory function returning Shared
  def test_factory_function_returning_shared
    run_mlc(<<~MLC) do |stdout, stderr, status|
      type Point = { x: i32, y: i32 }

      extend Shared<T> {
        extern fn new(value: T) -> Shared<T>
      }

      fn make_point(x: i32, y: i32) -> Shared<Point> =
        Shared.new(Point { x: x, y: y })

      fn main() -> i32 = do
        let p = make_point(15, 25)
        p.x + p.y
      end
    MLC
      assert_equal 40, status.exitstatus
    end
  end

  # Test 17: Generic function with Shared parameter
  def test_generic_function_with_shared
    run_mlc(<<~MLC) do |stdout, stderr, status|
      type Box<T> = { value: T }

      extend Shared<T> {
        extern fn new(value: T) -> Shared<T>
      }

      fn unwrap_shared<T>(s: Shared<Box<T>>) -> T = s.value

      fn main() -> i32 = do
        // Explicit type annotation needed for generic record literal
        let boxed: Shared<Box<i32>> = Shared.new(Box { value: 77 })
        unwrap_shared(boxed)
      end
    MLC
      assert_equal 77, status.exitstatus
    end
  end

  # Test 18: Chain of Shared dereferences
  def test_chain_of_shared_dereferences
    run_mlc(<<~MLC) do |stdout, stderr, status|
      type A = { value: i32 }
      type B = { a: Shared<A> }
      type C = { b: Shared<B> }

      extend Shared<T> {
        extern fn new(value: T) -> Shared<T>
      }

      fn main() -> i32 = do
        let a = Shared.new(A { value: 99 })
        let b = Shared.new(B { a: a })
        let c = Shared.new(C { b: b })
        c.b.a.value
      end
    MLC
      assert_equal 99, status.exitstatus
    end
  end

  # Test 19: Conditional with Shared types
  def test_conditional_with_shared
    run_mlc(<<~MLC) do |stdout, stderr, status|
      type Node = { value: i32 }

      extend Shared<T> {
        extern fn new(value: T) -> Shared<T>
      }

      fn main() -> i32 = do
        let n1 = Shared.new(Node { value: 10 })
        let n2 = Shared.new(Node { value: 20 })
        let flag = true
        let selected = if flag then n1 else n2
        selected.value
      end
    MLC
      assert_equal 10, status.exitstatus
    end
  end

  # Test 20: Multiple smart pointer types in same program
  def test_all_smart_pointer_types_together
    run_mlc(<<~MLC) do |stdout, stderr, status|
      type Data = { value: i32 }

      extend Shared<T> {
        extern fn new(value: T) -> Shared<T>
        extern fn downgrade(shared: Shared<T>) -> Weak<T>
      }

      extend Weak<T> {
        extern fn is_valid(weak: Weak<T>) -> bool
      }

      extend Owned<T> {
        extern fn new(value: T) -> Owned<T>
      }

      fn main() -> i32 = do
        // Create shared
        let shared = Shared.new(Data { value: 10 })

        // Downgrade to weak
        let weak = Shared.downgrade(shared)
        let weak_valid = if Weak.is_valid(weak) then 1 else 0

        // Create owned
        let owned = Owned.new(Data { value: 20 })

        shared.value + owned.value + weak_valid
      end
    MLC
      assert_equal 31, status.exitstatus
    end
  end
end
