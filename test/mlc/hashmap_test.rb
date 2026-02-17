# frozen_string_literal: true

require "minitest/autorun"
require_relative "../../lib/mlc/common/index"

class HashMapTest < Minitest::Test
  def test_map_new_codegen
    source = <<~MLC
      fn main() -> i32 = do
        let mut m: Map<string, i32> = Map.new()
        m.set("x", 42)
        m.get("x")
      end
    MLC
    cpp = MLC.to_cpp(source)
    assert_includes cpp, "mlc::HashMap<mlc::String, int>"
    assert_includes cpp, 'm.set(mlc::String("x"), 42)'
    assert_includes cpp, 'm.get(mlc::String("x"))'
  end

  def test_map_has_codegen
    source = <<~MLC
      fn main() -> bool = do
        let mut m: Map<string, i32> = Map.new()
        m.set("a", 1)
        m.has("a")
      end
    MLC
    cpp = MLC.to_cpp(source)
    assert_includes cpp, "m.has("
  end

  def test_map_size_codegen
    source = <<~MLC
      fn main() -> i32 = do
        let mut m: Map<string, i32> = Map.new()
        m.set("a", 1)
        m.size()
      end
    MLC
    cpp = MLC.to_cpp(source)
    assert_includes cpp, "m.size()"
  end

  def test_map_keys_codegen
    source = <<~MLC
      fn main() -> i32 = do
        let mut m: Map<string, i32> = Map.new()
        m.set("a", 1)
        m.keys().length()
      end
    MLC
    cpp = MLC.to_cpp(source)
    assert_includes cpp, "m.keys()"
  end

  def test_map_set_on_immutable_errors
    source = <<~MLC
      fn main() -> i32 = do
        let m: Map<string, i32> = Map.new()
        m.set("a", 1)
        0
      end
    MLC
    error = assert_raises(MLC::CompileError) { MLC.to_cpp(source) }
    assert_match(/Cannot call .set\(\) on immutable binding 'm'/, error.message)
  end

  def test_map_remove_on_immutable_errors
    source = <<~MLC
      fn main() -> i32 = do
        let m: Map<string, i32> = Map.new()
        m.remove("a")
        0
      end
    MLC
    error = assert_raises(MLC::CompileError) { MLC.to_cpp(source) }
    assert_match(/Cannot call .remove\(\) on immutable binding 'm'/, error.message)
  end

  def test_map_immutable_ops_on_let
    source = <<~MLC
      fn main() -> i32 = do
        let m: Map<string, i32> = Map.new()
        m.size()
      end
    MLC
    cpp = MLC.to_cpp(source)
    assert_includes cpp, "m.size()"
  end

  def test_hashmap_alias
    source = <<~MLC
      fn main() -> i32 = do
        let mut m: HashMap<string, i32> = HashMap.new()
        m.set("x", 99)
        m.get("x")
      end
    MLC
    cpp = MLC.to_cpp(source)
    assert_includes cpp, "mlc::HashMap<mlc::String, int>"
  end

  def test_map_i32_keys
    source = <<~MLC
      fn main() -> i32 = do
        let mut m: Map<i32, i32> = Map.new()
        m.set(1, 100)
        m.set(2, 200)
        m.get(1)
      end
    MLC
    cpp = MLC.to_cpp(source)
    assert_includes cpp, "mlc::HashMap<int, int>"
  end
end
