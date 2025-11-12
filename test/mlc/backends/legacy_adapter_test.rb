# frozen_string_literal: true

require "test_helper"
require_relative "../../../lib/mlc/backends/cpp/legacy_adapter"
require_relative "../../../lib/mlc/core/type_registry"
require_relative "../../../lib/mlc/core/function_registry"

module MLC
  module Backends
    module Cpp
      class LegacyAdapterTest < Minitest::Test
        def setup
          @type_registry = MLC::Core::TypeRegistry.new
          @function_registry = MLC::Core::FunctionRegistry.new
        end

        def test_legacy_adapter_initializes_successfully
          adapter = LegacyAdapter.new(
            type_registry: @type_registry,
            function_registry: @function_registry
          )

          assert_instance_of LegacyAdapter, adapter
          refute_nil adapter.legacy_backend
          refute_nil adapter.container
          refute_nil adapter.context
        end

        def test_legacy_adapter_exposes_backend_attributes
          adapter = LegacyAdapter.new(
            type_registry: @type_registry,
            function_registry: @function_registry
          )

          # Should expose same attributes as Backend::CodeGen
          assert_respond_to adapter, :type_registry
          assert_respond_to adapter, :function_registry
          assert_respond_to adapter, :rule_engine
          assert_respond_to adapter, :event_bus
          assert_respond_to adapter, :runtime_policy
          assert_respond_to adapter, :type_map

          assert_equal @type_registry, adapter.type_registry
          assert_equal @function_registry, adapter.function_registry
        end

        def test_legacy_adapter_has_lower_method
          adapter = LegacyAdapter.new(
            type_registry: @type_registry,
            function_registry: @function_registry
          )

          assert_respond_to adapter, :lower
        end

        def test_legacy_adapter_delegates_to_backend_codegen
          adapter = LegacyAdapter.new(
            type_registry: @type_registry,
            function_registry: @function_registry
          )

          # Verify delegation structure
          assert_instance_of MLC::Backend::CodeGen, adapter.legacy_backend
          assert_same adapter.legacy_backend.type_registry, @type_registry
        end

        def test_legacy_adapter_initializes_new_architecture
          adapter = LegacyAdapter.new(
            type_registry: @type_registry,
            function_registry: @function_registry
          )

          # Verify new architecture components are initialized
          assert_instance_of Container, adapter.container
          assert_instance_of Context, adapter.context

          # Context should have access to container services
          assert_respond_to adapter.context, :factory
          assert_respond_to adapter.context, :checker
          assert_respond_to adapter.context, :lower_expression
          assert_respond_to adapter.context, :lower_statement
        end

        def test_application_uses_legacy_adapter_when_env_set
          # Setup environment variable
          original_env = ENV["MLC_CPP_BACKEND"]
          ENV["MLC_CPP_BACKEND"] = "v2"

          begin
            app = Application.new
            lowering = app.build_cpp_lowering(
              type_registry: @type_registry,
              function_registry: @function_registry
            )

            assert_instance_of LegacyAdapter, lowering
          ensure
            # Restore original environment
            if original_env
              ENV["MLC_CPP_BACKEND"] = original_env
            else
              ENV.delete("MLC_CPP_BACKEND")
            end
          end
        end

        def test_application_uses_legacy_codegen_by_default
          # Ensure env variable is not set
          original_env = ENV["MLC_CPP_BACKEND"]
          ENV.delete("MLC_CPP_BACKEND")

          begin
            app = Application.new
            lowering = app.build_cpp_lowering(
              type_registry: @type_registry,
              function_registry: @function_registry
            )

            assert_instance_of Backend::CodeGen, lowering
            refute_instance_of LegacyAdapter, lowering
          ensure
            # Restore original environment
            ENV["MLC_CPP_BACKEND"] = original_env if original_env
          end
        end
      end
    end
  end
end
