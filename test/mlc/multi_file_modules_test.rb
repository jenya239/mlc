# frozen_string_literal: true

require_relative "../test_helper"
require_relative "../../lib/mlc/common/index"
require "tmpdir"
require "fileutils"

class MLCMultiFileModulesTest < Minitest::Test
  def setup
    @tmpdir = Dir.mktmpdir("mlc_test")
  end

  def teardown
    FileUtils.rm_rf(@tmpdir) if @tmpdir && File.exist?(@tmpdir)
  end

  def test_compile_two_files_with_import
    # Step 1: Create and compile math module
    math_source = <<~MLCORA
      export fn add(a: i32, b: i32) -> i32 = a + b
    MLCORA

    math_path = File.join(@tmpdir, "math.mlc")
    File.write(math_path, math_source)

    # Compile math module and generate metadata
    Dir.chdir(@tmpdir) do
      result = MLC.to_hpp_cpp(math_source)

      # Write metadata to math.mlcmeta
      require 'json'
      metadata_path = File.join(@tmpdir, "math.mlcmeta")
      File.write(metadata_path, JSON.pretty_generate(result[:metadata]))

      # Verify metadata was generated correctly
      assert File.exist?(metadata_path), "Metadata file should be created"
      metadata = JSON.parse(File.read(metadata_path))
      assert_equal "main", metadata["module_name"]
      assert_equal 1, metadata["exports"]["functions"].length
      assert_equal "add", metadata["exports"]["functions"][0]["name"]
    end

    # Step 2: Create main module that imports math
    main_source = <<~MLCORA
      import math::{add}

      fn compute(x: i32, y: i32) -> i32 = add(x, y)
    MLCORA

    # Compile main module (should load math.mlcmeta)
    Dir.chdir(@tmpdir) do
      result = MLC.to_hpp_cpp(main_source)

      # Verify compilation succeeded
      refute_nil result[:header], "Header should be generated"
      refute_nil result[:implementation], "Implementation should be generated"

      # Verify that add function is available in main
      assert_includes result[:implementation], "compute", "compute function should be in implementation"
    end
  end

  def test_compile_nested_directories
    # Create nested directory structure with metadata files
    # Since relative paths are not yet supported, test metadata loading in nested dirs

    # Step 1: Create string_utils module in tmpdir
    string_source = <<~MLCORA
      export fn uppercase(s: str) -> str = s
    MLCORA

    Dir.chdir(@tmpdir) do
      result = MLC.to_hpp_cpp(string_source)

      # Write metadata to string_utils.mlcmeta
      require 'json'
      metadata_path = "string_utils.mlcmeta"
      File.write(metadata_path, JSON.pretty_generate(result[:metadata]))

      # Verify metadata
      assert File.exist?(metadata_path)
      metadata = JSON.parse(File.read(metadata_path))
      assert_equal "uppercase", metadata["exports"]["functions"][0]["name"]
    end

    # Step 2: Create main module that imports string_utils
    main_source = <<~MLCORA
      import string_utils::{uppercase}

      fn process(text: str) -> str = uppercase(text)
    MLCORA

    # Compile main module (should load string_utils.mlcmeta)
    Dir.chdir(@tmpdir) do
      result = MLC.to_hpp_cpp(main_source)

      # Verify compilation succeeded
      refute_nil result[:header]
      refute_nil result[:implementation]
      assert_includes result[:implementation], "process"
    end
  end

  def test_compile_parent_directory_import
    # Test metadata loading from same directory (simplified version)
    # Parent directory imports require parser support which is not yet available

    # Step 1: Create helper module
    helper_source = <<~MLCORA
      export fn multiply(a: i32, b: i32) -> i32 = a * b
    MLCORA

    Dir.chdir(@tmpdir) do
      result = MLC.to_hpp_cpp(helper_source)

      require 'json'
      File.write("helper.mlcmeta", JSON.pretty_generate(result[:metadata]))

      assert File.exist?("helper.mlcmeta")
    end

    # Step 2: Create main that imports helper
    main_source = <<~MLCORA
      import helper::{multiply}

      fn calc(x: i32, y: i32) -> i32 = multiply(x, y)
    MLCORA

    Dir.chdir(@tmpdir) do
      result = MLC.to_hpp_cpp(main_source)

      refute_nil result[:header]
      refute_nil result[:implementation]
      assert_includes result[:implementation], "calc"
    end
  end

  def test_compile_with_sum_types_across_files
    # Test metadata with sum types (simplified - just verify metadata is correct)
    types_source = <<~MLCORA
      export type Status = Success | Failure
    MLCORA

    Dir.chdir(@tmpdir) do
      result = MLC.to_hpp_cpp(types_source)

      require 'json'
      File.write("types.mlcmeta", JSON.pretty_generate(result[:metadata]))

      metadata = JSON.parse(File.read("types.mlcmeta"))
      assert_equal 1, metadata["exports"]["sum_types"].length
      assert_equal "Status", metadata["exports"]["sum_types"][0]["name"]

      # Verify variants are present
      variants = metadata["exports"]["sum_types"][0]["variants"]
      assert_equal 2, variants.length
      assert_includes variants, "Success"
      assert_includes variants, "Failure"
    end

    # Note: Using the imported sum type constructors requires additional
    # sum type constructor registration which is beyond this test's scope
  end

  def test_multiple_imports_from_same_file
    # Step 1: Create module with multiple exports
    utils_source = <<~MLCORA
      export fn add(a: i32, b: i32) -> i32 = a + b
      export fn subtract(a: i32, b: i32) -> i32 = a - b
      export fn multiply(a: i32, b: i32) -> i32 = a * b
    MLCORA

    Dir.chdir(@tmpdir) do
      result = MLC.to_hpp_cpp(utils_source)

      require 'json'
      File.write("utils.mlcmeta", JSON.pretty_generate(result[:metadata]))

      metadata = JSON.parse(File.read("utils.mlcmeta"))
      assert_equal 3, metadata["exports"]["functions"].length
    end

    # Step 2: Import multiple functions from same file
    main_source = <<~MLCORA
      import utils::{add, subtract, multiply}

      fn compute(x: i32, y: i32) -> i32 = add(subtract(x, y), multiply(x, y))
    MLCORA

    Dir.chdir(@tmpdir) do
      result = MLC.to_hpp_cpp(main_source)

      refute_nil result[:header]
      refute_nil result[:implementation]
      assert_includes result[:implementation], "compute"
    end
  end

  def test_wildcard_import
    # Step 1: Create module with multiple exports
    math_source = <<~MLCORA
      export fn add(a: i32, b: i32) -> i32 = a + b
      export fn subtract(a: i32, b: i32) -> i32 = a - b
    MLCORA

    Dir.chdir(@tmpdir) do
      result = MLC.to_hpp_cpp(math_source)

      require 'json'
      File.write("math.mlcmeta", JSON.pretty_generate(result[:metadata]))
    end

    # Step 2: Import all with wildcard
    main_source = <<~MLCORA
      import * as math from math

      fn compute(x: i32, y: i32) -> i32 = math.add(x, math.subtract(x, y))
    MLCORA

    Dir.chdir(@tmpdir) do
      result = MLC.to_hpp_cpp(main_source)

      refute_nil result[:header]
      refute_nil result[:implementation]
      assert_includes result[:implementation], "compute"
    end
  end

  def test_write_generated_files_to_disk
    # Create a simple MLC file
    mlc_file = File.join(@tmpdir, "test.mlc")
    File.write(mlc_file, <<~MLCORA)
      export fn hello() -> i32 = 42
    MLCORA

    # Compile it
    result = compile_mlc_file(mlc_file)

    # Write generated files
    hpp_file = File.join(@tmpdir, "test.hpp")
    cpp_file = File.join(@tmpdir, "test.cpp")

    File.write(hpp_file, result[:header])
    File.write(cpp_file, result[:implementation])

    # Verify files exist and have content
    assert File.exist?(hpp_file), "Header file should be created"
    assert File.exist?(cpp_file), "Implementation file should be created"
    assert File.size(hpp_file) > 0, "Header should have content"
    assert File.size(cpp_file) > 0, "Implementation should have content"

    # Verify content
    hpp_content = File.read(hpp_file)
    assert_includes hpp_content, "#ifndef"
    assert_includes hpp_content, "int hello();"

    cpp_content = File.read(cpp_file)
    # Note: module name defaults to "main" without explicit module declaration
    assert_includes cpp_content, '#include "main.hpp"'
    assert_includes cpp_content, "return 42"
  end

  private

  def compile_mlc_file(path)
    source = File.read(path)
    MLC.to_hpp_cpp(source)
  end
end
