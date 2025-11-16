# frozen_string_literal: true

require_relative "../../test_helper"

module MLC
  module Backends
    module Cpp
      class SmokeTest < Minitest::Test
        def test_module_loads
          assert_equal "2.0.0", MLC::Backends::Cpp::VERSION
        end

        def test_container_has_registries
          type_registry = MLC::Registries::TypeRegistry.new
          function_registry = MLC::Registries::FunctionRegistry.new

          container = Container.new(
            type_registry: type_registry,
            function_registry: function_registry
          )

          assert_same type_registry, container.type_registry
          assert_same function_registry, container.function_registry
        end

        def test_context_provides_public_api
          type_registry = MLC::Registries::TypeRegistry.new
          function_registry = MLC::Registries::FunctionRegistry.new

          container = Container.new(
            type_registry: type_registry,
            function_registry: function_registry
          )

          context = Context.new(container)

          assert_respond_to context, :map_type
          assert_respond_to context, :escape_string
          assert_respond_to context, :sanitize_identifier
          assert_same type_registry, context.type_registry
          assert_same function_registry, context.function_registry
        end

        def test_context_map_type
          container = Container.new(
            type_registry: MLC::Registries::TypeRegistry.new,
            function_registry: MLC::Registries::FunctionRegistry.new
          )
          context = Context.new(container)

          # Test primitive type mapping
          type = SemanticIR::Type.new(name: "i32", kind: :primitive)
          result = context.map_type(type)
          assert_equal "int", result
        end

        def test_context_string_helpers
          container = Container.new(
            type_registry: MLC::Registries::TypeRegistry.new,
            function_registry: MLC::Registries::FunctionRegistry.new
          )
          context = Context.new(container)

          # Test escape_string (no quotes)
          result = context.escape_string("hello")
          assert_equal "hello", result

          # Test cpp_string_literal (with quotes)
          result = context.cpp_string_literal("hello")
          assert_equal '"hello"', result
        end

        def test_context_generate_lambda_type_placeholder
          container = Container.new(
            type_registry: MLC::Registries::TypeRegistry.new,
            function_registry: MLC::Registries::FunctionRegistry.new
          )
          context = Context.new(container)

          result = context.generate_lambda_type(["int", "float"], "bool")
          assert_equal "std::function<bool(int, float)>", result
        end
      end
    end
  end
end
