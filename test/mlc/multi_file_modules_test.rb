# frozen_string_literal: true

require_relative "../test_helper"
require_relative "../../lib/mlc"
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
    skip "Requires Phase 24-B (MetadataLoader) - imports not yet supported"
  end

  def test_compile_nested_directories
    skip "Requires Phase 24-B (MetadataLoader) - imports not yet supported"
  end

  def test_compile_parent_directory_import
    skip "Requires Phase 24-B (MetadataLoader) - imports not yet supported"
  end

  def test_compile_with_sum_types_across_files
    skip "Requires Phase 24-B (MetadataLoader) - imports not yet supported"
  end

  def test_multiple_imports_from_same_file
    skip "Requires Phase 24-B (MetadataLoader) - imports not yet supported"
  end

  def test_wildcard_import
    skip "Requires Phase 24-B (MetadataLoader) - imports not yet supported"
  end

  def test_write_generated_files_to_disk
    # Create a simple MLC file
    aurora_file = File.join(@tmpdir, "test.mlc")
    File.write(aurora_file, <<~MLCORA)
      export fn hello() -> i32 = 42
    MLCORA

    # Compile it
    result = compile_aurora_file(aurora_file)

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

  def compile_aurora_file(path)
    source = File.read(path)
    MLC.to_hpp_cpp(source)
  end
end
