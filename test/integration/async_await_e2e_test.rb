# frozen_string_literal: true

require_relative "../test_helper"
require "open3"
require "tmpdir"

class AsyncAwaitE2ETest < Minitest::Test
  tag :slow  # Mark as slow test (requires C++ compilation)

  CLI = File.expand_path("../../bin/mlc", __dir__)

  # Helper to run MLC program and check result
  def run_mlc(source_code, expected_exit: nil, &block)
    Dir.mktmpdir do |dir|
      source = File.join(dir, "test.mlc")
      File.write(source, source_code)
      stdout, stderr, status = Open3.capture3(CLI, source)

      yield stdout, stderr, status if block_given?
      assert_equal expected_exit, status.exitstatus if expected_exit
    end
  end

  # Helper to check that code compiles (generates C++ without errors)
  def assert_compiles(source_code, msg = nil)
    Dir.mktmpdir do |dir|
      source = File.join(dir, "test.mlc")
      File.write(source, source_code)
      stdout, stderr, status = Open3.capture3(CLI, "--emit-cpp", source)

      refute_includes stderr, "error:", msg || "Compilation failed: #{stderr}"
      assert status.success?, msg || "MLC compilation failed"
    end
  end

  # Test 1: Simple async function declaration
  def test_async_function_declaration
    assert_compiles(<<~MLC)
      async fn compute_async() -> i32 = 42

      fn main() -> i32 = 0
    MLC
  end

  # Test 2: Async function with await
  def test_async_function_with_await
    assert_compiles(<<~MLC)
      async fn get_value() -> i32 = 10

      async fn compute() -> i32 = do
        let x = await get_value()
        x + 5
      end

      fn main() -> i32 = 0
    MLC
  end

  # Test 3: Chained await expressions
  def test_chained_await
    assert_compiles(<<~MLC)
      async fn step1() -> i32 = 1
      async fn step2(x: i32) -> i32 = x + 2
      async fn step3(x: i32) -> i32 = x + 3

      async fn pipeline() -> i32 = do
        let a = await step1()
        let b = await step2(a)
        await step3(b)
      end

      fn main() -> i32 = 0
    MLC
  end

  # Test 4: Await with arithmetic
  def test_await_with_arithmetic
    assert_compiles(<<~MLC)
      async fn get_number() -> i32 = 10

      async fn calculate() -> i32 = do
        let x = await get_number() + 5
        let y = await get_number() * 2
        x + y
      end

      fn main() -> i32 = 0
    MLC
  end

  # Test 5: Export async function
  def test_export_async_function
    assert_compiles(<<~MLC)
      export async fn public_async() -> i32 = 100

      fn main() -> i32 = 0
    MLC
  end

  # Test 6: Async function with parameters
  def test_async_function_with_params
    assert_compiles(<<~MLC)
      async fn add_async(a: i32, b: i32) -> i32 = a + b

      async fn test() -> i32 = do
        let result = await add_async(10, 20)
        result
      end

      fn main() -> i32 = 0
    MLC
  end

  # Test 7: Async function returning unit
  def test_async_function_unit_return
    assert_compiles(<<~MLC)
      async fn do_work() -> unit = do
        let _ = 42
      end

      fn main() -> i32 = 0
    MLC
  end

  # Test 8: Multiple async functions
  def test_multiple_async_functions
    assert_compiles(<<~MLC)
      async fn fetch_a() -> i32 = 1
      async fn fetch_b() -> i32 = 2
      async fn fetch_c() -> i32 = 3

      async fn combine() -> i32 = do
        let a = await fetch_a()
        let b = await fetch_b()
        let c = await fetch_c()
        a + b + c
      end

      fn main() -> i32 = 0
    MLC
  end

  # Test 9: Async with conditional
  def test_async_with_conditional
    assert_compiles(<<~MLC)
      async fn get_value(flag: bool) -> i32 =
        if flag then 100 else 200

      async fn process() -> i32 = do
        let x = await get_value(true)
        let y = await get_value(false)
        x + y
      end

      fn main() -> i32 = 0
    MLC
  end
end
