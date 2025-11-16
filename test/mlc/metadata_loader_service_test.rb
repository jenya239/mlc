# frozen_string_literal: true

require_relative "../test_helper"
require_relative "../../lib/mlc/common/stdlib/metadata_loader"

module MLC
  module Compiler
    class MetadataLoaderServiceTest < Minitest::Test
      def setup
        @function_registry = Registries::FunctionRegistry.new
        @type_registry = Registries::TypeRegistry.new
        @loader = MetadataLoaderService.new(
          function_registry: @function_registry,
          type_registry: @type_registry
        )
        @tmpdir = Dir.mktmpdir("mlc_metadata_test")
      end

      def teardown
        FileUtils.rm_rf(@tmpdir) if @tmpdir && File.exist?(@tmpdir)
      end

      def test_load_returns_false_if_file_does_not_exist
        result = @loader.load("/nonexistent/path.mlcmeta")
        assert_equal false, result
      end

      def test_load_simple_function_metadata
        metadata = {
          "module_name" => "Math",
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

        metadata_path = write_metadata(metadata)
        result = @loader.load(metadata_path)

        assert_equal true, result

        # Check function was registered
        entry = @function_registry.fetch_entry("add")
        refute_nil entry
        assert_equal "add", entry.name
        assert_equal "Math", entry.module_name
        assert_equal "math", entry.namespace
        assert entry.exported?

        # Check signature
        sig = entry.info
        assert_equal "add", sig.name
        assert_equal 2, sig.param_types.length
        assert_equal "i32", sig.param_types[0].name
        assert_equal "i32", sig.param_types[1].name
        assert_equal "i32", sig.ret_type.name
      end

      def test_load_record_type_metadata
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

        metadata_path = write_metadata(metadata)
        result = @loader.load(metadata_path)

        assert_equal true, result

        # Check type was registered
        type_info = @type_registry.lookup("Point")
        refute_nil type_info
        assert_equal "Point", type_info.name
        assert_equal :record, type_info.kind
        assert_equal "Geometry", type_info.module_name
        assert_equal "geometry", type_info.namespace
        assert type_info.exported

        # Check fields
        assert_equal 2, type_info.fields.length
        assert_equal "x", type_info.fields[0][:name]
        assert_equal "f32", type_info.fields[0][:type].name
        assert_equal "y", type_info.fields[1][:name]
        assert_equal "f32", type_info.fields[1][:type].name
      end

      def test_load_sum_type_metadata
        metadata = {
          "module_name" => "Core",
          "exports" => {
            "functions" => [],
            "types" => [],
            "sum_types" => [
              {
                "name" => "Result",
                "kind" => "sum",
                "type_params" => [],
                "variants" => ["Ok", "Err"],
                "is_exported" => true
              }
            ]
          }
        }

        metadata_path = write_metadata(metadata)
        result = @loader.load(metadata_path)

        assert_equal true, result

        # Check sum type was registered
        type_info = @type_registry.lookup("Result")
        refute_nil type_info
        assert_equal "Result", type_info.name
        assert_equal :sum, type_info.kind
        assert_equal "Core", type_info.module_name
        assert type_info.exported

        # Check variants
        assert_equal 2, type_info.variants.length
        assert_equal "Ok", type_info.variants[0][:name]
        assert_equal "Err", type_info.variants[1][:name]
      end

      def test_parse_type_primitives
        i32_type = @loader.send(:parse_type, "i32")
        assert_equal "i32", i32_type.name
        assert_equal :prim, i32_type.kind

        f32_type = @loader.send(:parse_type, "f32")
        assert_equal "f32", f32_type.name
        assert_equal :prim, f32_type.kind

        bool_type = @loader.send(:parse_type, "bool")
        assert_equal "bool", bool_type.name
        assert_equal :prim, bool_type.kind
      end

      def test_parse_type_array
        array_type = @loader.send(:parse_type, "i32[]")
        assert array_type.is_a?(SemanticIR::ArrayType)
        assert_equal "i32", array_type.element_type.name
      end

      def test_module_name_to_namespace
        assert_equal "math", @loader.send(:module_name_to_namespace, "Math")
        assert_equal "math::vector", @loader.send(:module_name_to_namespace, "Math::Vector")
        assert_equal "geometry", @loader.send(:module_name_to_namespace, "Geometry")
      end

      private

      def write_metadata(metadata_hash)
        path = File.join(@tmpdir, "test.mlcmeta")
        File.write(path, JSON.pretty_generate(metadata_hash))
        path
      end
    end
  end
end
