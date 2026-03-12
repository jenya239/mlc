# frozen_string_literal: true

require_relative "../test_helper"
require "open3"
require "tmpdir"

class HashMapE2ETest < Minitest::Test
  tag :slow

  CLI = File.expand_path("../../bin/mlc", __dir__)

  def run_mlc(source_code)
    Dir.mktmpdir do |dir|
      source = File.join(dir, "test.mlc")
      File.write(source, source_code)
      stdout, stderr, status = Open3.capture3(CLI, source)
      yield stdout, stderr, status if block_given?
    end
  end

  def test_map_set_get
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = do
        let m: Map<string, i32> = Map.new()
        m.set("a", 10)
        m.set("b", 20)
        m.get("a") + m.get("b")
      end
    MLC
      assert_equal 30, status.exitstatus
    end
  end

  def test_map_has
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = do
        let m: Map<string, i32> = Map.new()
        m.set("x", 42)
        if m.has("x") then 1 else 0
      end
    MLC
      assert_equal 1, status.exitstatus
    end
  end

  def test_map_size
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = do
        let m: Map<string, i32> = Map.new()
        m.set("a", 1)
        m.set("b", 2)
        m.set("c", 3)
        m.size()
      end
    MLC
      assert_equal 3, status.exitstatus
    end
  end

  def test_map_overwrite
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = do
        let m: Map<string, i32> = Map.new()
        m.set("a", 10)
        m.set("a", 99)
        m.get("a")
      end
    MLC
      assert_equal 99, status.exitstatus
    end
  end

  def test_map_i32_keys
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = do
        let m: Map<i32, i32> = Map.new()
        m.set(1, 10)
        m.set(2, 20)
        m.set(3, 30)
        m.get(1) + m.get(2) + m.get(3)
      end
    MLC
      assert_equal 60, status.exitstatus
    end
  end

  def test_map_keys_length
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = do
        let m: Map<string, i32> = Map.new()
        m.set("a", 1)
        m.set("b", 2)
        m.keys().length()
      end
    MLC
      assert_equal 2, status.exitstatus
    end
  end

  def test_map_remove
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = do
        let m: Map<string, i32> = Map.new()
        m.set("a", 1)
        m.set("b", 2)
        m.remove("a")
        m.size()
      end
    MLC
      assert_equal 1, status.exitstatus
    end
  end

  def test_map_cow_copy_independent
    run_mlc(<<~MLC) do |_stdout, _stderr, status|
      fn main() -> i32 = do
        let a: Map<string, i32> = Map.new()
        a.set("x", 10)
        let b = a
        a.set("y", 20)
        a.size() * 10 + b.size()
      end
    MLC
      assert_equal 21, status.exitstatus  # a.size()=2, b.size()=1 -> 2*10+1=21
    end
  end
end
