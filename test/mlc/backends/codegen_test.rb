# frozen_string_literal: true

require "test_helper"
require_relative "../../../lib/mlc/backends/cpp/codegen"
require_relative "../../../lib/mlc/common/registry/type_registry"
require_relative "../../../lib/mlc/common/registry/function_registry"

module MLC
  module Backends
    module Cpp
      class CodeGenTest < Minitest::Test
        def setup
          @type_registry = MLC::Core::TypeRegistry.new
          @function_registry = MLC::Core::FunctionRegistry.new
        end

        def test_codegen_initializes_successfully
          codegen = CodeGen.new(
            type_registry: @type_registry,
            function_registry: @function_registry
          )

          assert_instance_of CodeGen, codegen
          refute_nil codegen.container
          refute_nil codegen.context
        end

        def test_codegen_exposes_backend_attributes
          codegen = CodeGen.new(
            type_registry: @type_registry,
            function_registry: @function_registry
          )

          # Should expose expected attributes
          assert_respond_to codegen, :type_registry
          assert_respond_to codegen, :function_registry
          assert_respond_to codegen, :event_bus
          assert_respond_to codegen, :runtime_policy
          assert_respond_to codegen, :type_map

          assert_equal @type_registry, codegen.type_registry
          assert_equal @function_registry, codegen.function_registry
        end

        def test_codegen_has_lower_method
          codegen = CodeGen.new(
            type_registry: @type_registry,
            function_registry: @function_registry
          )

          assert_respond_to codegen, :lower
        end

        def test_codegen_initializes_v2_architecture
          codegen = CodeGen.new(
            type_registry: @type_registry,
            function_registry: @function_registry
          )

          # Verify v2 architecture components are initialized
          assert_instance_of Container, codegen.container
          assert_instance_of Context, codegen.context

          # Context should have access to container services
          assert_respond_to codegen.context, :factory
          assert_respond_to codegen.context, :checker
          assert_respond_to codegen.context, :lower_expression
          assert_respond_to codegen.context, :lower_statement
        end
      end
    end
  end
end
