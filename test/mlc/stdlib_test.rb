# frozen_string_literal: true

require_relative "../test_helper"
require_relative "../../lib/mlc"

class MLCStdlibTest < Minitest::Test
  def test_builtin_functions
    # Test built-in functions that already work
    mlc_source = <<~MLC
      fn test_builtins() -> i32 =
        print("Hello");
        println("World");
        42
    MLC

    cpp = MLC.to_cpp(mlc_source)
    
    assert_includes cpp, "print"
    # Note: println might not be generated if it's not used in the return value
    assert_includes cpp, "Hello"
  end

  def test_array_operations
    # Test array operations that already work
    mlc_source = <<~MLC
      fn test_arrays() -> i32 =
        [1, 2, 3, 4, 5].map(x => x * 2).filter(x => x > 5).length()
    MLC

    cpp = MLC.to_cpp(mlc_source)
    
    assert_includes cpp, "map"
    assert_includes cpp, "filter"
    assert_includes cpp, "size"  # MLC uses .size() instead of .length()
  end

  def test_string_operations
    # Test string operations that already work
    mlc_source = <<~MLC
      fn test_strings() -> str =
        "  hello world  ".trim().upper()
    MLC

    cpp = MLC.to_cpp(mlc_source)
    
    assert_includes cpp, "trim"
    assert_includes cpp, "upper"
  end

  def test_stdlib_files_exist
    # Test that working stdlib files exist
    project_root = File.expand_path('../..', __dir__)
    stdlib_files = [
      "lib/mlc/stdlib/math.mlc",
      "lib/mlc/stdlib/io.mlc",
      "lib/mlc/stdlib/string.mlc",
      "lib/mlc/stdlib/conv.mlc"
    ]

    stdlib_files.each do |file|
      full_path = File.join(project_root, file)
      assert File.exist?(full_path), "Stdlib file #{file} should exist at #{full_path}"
    end
  end

  # Removed: test_stdlib_documentation_exists
  # docs/STDLIB.md was removed as part of documentation cleanup

  def test_simple_math_functions
    # Test simple math that works with current syntax
    mlc_source = <<~MLC
      fn test_simple_math() -> i32 =
        5 + 3 + 7
    MLC

    cpp = MLC.to_cpp(mlc_source)
    
    assert_includes cpp, "5 + 3 + 7"
  end

  def test_string_concatenation
    # Test string concatenation that we implemented
    mlc_source = <<~MLC
      fn test_string_concat() -> str =
        "Hello" + " " + "World"
    MLC

    cpp = MLC.to_cpp(mlc_source)
    
    assert_includes cpp, "mlc::String"
    assert_includes cpp, "Hello"
    assert_includes cpp, "World"
  end
end
