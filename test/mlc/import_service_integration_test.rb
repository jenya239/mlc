# frozen_string_literal: true

require_relative "../test_helper"
require "tmpdir"
require "fileutils"
require "json"

module MLC
  class ImportServiceIntegrationTest < Minitest::Test
    def setup
      @tmpdir = Dir.mktmpdir("mlc_import_test")
      @original_dir = Dir.pwd
      Dir.chdir(@tmpdir)
    end

    def teardown
      Dir.chdir(@original_dir) if @original_dir
      FileUtils.rm_rf(@tmpdir) if @tmpdir && File.exist?(@tmpdir)
    end

    def test_imports_user_module_with_metadata
      # Create metadata file for imported module
      metadata = {
        "module_name" => "MyMath",
        "exports" => {
          "functions" => [
            {
              "name" => "add",
              "params" => [
                { "name" => "a", "type" => "i32" },
                { "name" => "b", "type" => "i32" }
              ],
              "return_type" => "i32",
              "is_exported" => true
            }
          ],
          "types" => [],
          "sum_types" => []
        }
      }

      File.write("./MyMath.mlcmeta", JSON.pretty_generate(metadata))

      # Import the module in MLC source
      source = <<~MLCORA
        import MyMath::{add}

        fn compute(x: i32, y: i32) -> i32 = add(x, y)
      MLCORA

      # Parse and transform
      ast = MLC.parse(source)
      transformer = SemanticGen::Pipeline.new
      core_ir = transformer.transform(ast)

      # Verify function was loaded from metadata
      entry = transformer.function_registry.fetch_entry("add")
      refute_nil entry, "Function 'add' should be registered from metadata"
      assert_equal "add", entry.name
      assert_equal "MyMath", entry.module_name
      assert_equal "mymath", entry.namespace
      assert entry.exported?

      # Verify SemanticIR contains the compute function
      compute_func = core_ir.items.find { |item| item.is_a?(SemanticIR::Func) && item.name == "compute" }
      refute_nil compute_func
    end

    def test_imports_user_module_with_record_type
      # Create metadata with a record type
      metadata = {
        "module_name" => "Geometry",
        "exports" => {
          "functions" => [],
          "types" => [
            {
              "name" => "Point",
              "kind" => "record",
              "fields" => [
                { "name" => "x", "type" => "f32" },
                { "name" => "y", "type" => "f32" }
              ],
              "is_exported" => true
            }
          ],
          "sum_types" => []
        }
      }

      File.write("./Geometry.mlcmeta", JSON.pretty_generate(metadata))

      # Import the type
      source = <<~MLCORA
        import Geometry::{Point}

        fn origin() -> Point = Point { x: 0.0, y: 0.0 }
      MLCORA

      ast = MLC.parse(source)
      transformer = SemanticGen::Pipeline.new
      core_ir = transformer.transform(ast)

      # Verify type was loaded from metadata
      type_info = transformer.type_registry.lookup("Point")
      refute_nil type_info, "Type 'Point' should be registered from metadata"
      assert_equal "Point", type_info.name
      assert_equal :record, type_info.kind
      assert_equal "Geometry", type_info.module_name
      assert type_info.exported
    end

    def test_skips_missing_metadata_file_gracefully
      # Import non-existent module (no metadata file)
      source = <<~MLCORA
        import NonExistent::{foo}

        fn bar() -> i32 = 42
      MLCORA

      ast = MLC.parse(source)
      transformer = SemanticGen::Pipeline.new

      # Should not crash, just skip metadata loading
      core_ir = transformer.transform(ast)
      refute_nil core_ir

      # Verify that attempting to use the function will fail later
      # (not part of this test, but import should not crash)
    end

    def test_metadata_loading_with_lowercase_filename
      # Create metadata with lowercase filename
      metadata = {
        "module_name" => "MyModule",
        "exports" => {
          "functions" => [
            {
              "name" => "test",
              "params" => [],
              "return_type" => "i32",
              "is_exported" => true
            }
          ],
          "types" => [],
          "sum_types" => []
        }
      }

      # Write with lowercase filename
      File.write("./mymodule.mlcmeta", JSON.pretty_generate(metadata))

      source = <<~MLCORA
        import MyModule::{test}

        fn main() -> i32 = test()
      MLCORA

      ast = MLC.parse(source)
      transformer = SemanticGen::Pipeline.new
      core_ir = transformer.transform(ast)

      # Verify function was loaded (case-insensitive fallback)
      entry = transformer.function_registry.fetch_entry("test")
      refute_nil entry, "Function 'test' should be registered from lowercase metadata file"
    end
  end
end
