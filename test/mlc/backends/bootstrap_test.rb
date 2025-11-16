# frozen_string_literal: true

require "test_helper"
require_relative "../../../lib/mlc/backends/cpp/bootstrap"
require_relative "../../../lib/mlc/registries/type_registry"
require_relative "../../../lib/mlc/registries/function_registry"

module MLC
  module Backends
    module Cpp
      class BootstrapTest < Minitest::Test
        def setup
          @type_registry = MLC::Registries::TypeRegistry.new
          @function_registry = MLC::Registries::FunctionRegistry.new
        end

        def test_create_backend_returns_orchestrator_with_all_components
          backend = Bootstrap.create_backend(
            type_registry: @type_registry,
            function_registry: @function_registry
          )

          assert_instance_of Hash, backend
          assert backend.key?(:container)
          assert backend.key?(:context)
          assert backend.key?(:rule_engine)

          assert_instance_of Container, backend[:container]
          assert_instance_of Context, backend[:context]
          assert_instance_of Services::RuleEngine, backend[:rule_engine]
        end

        def test_rule_engine_has_expression_rules_registered
          backend = Bootstrap.create_backend(
            type_registry: @type_registry,
            function_registry: @function_registry
          )

          rule_engine = backend[:rule_engine]

          # Check that expression rules are registered (at least 15)
          assert rule_engine.instance_variable_get(:@expression_rules).length >= 15,
                 "Expected at least 15 expression rules registered"
        end

        def test_rule_engine_has_statement_rules_registered
          backend = Bootstrap.create_backend(
            type_registry: @type_registry,
            function_registry: @function_registry
          )

          rule_engine = backend[:rule_engine]

          # Check that statement rules are registered (at least 11)
          assert rule_engine.instance_variable_get(:@statement_rules).length >= 11,
                 "Expected at least 11 statement rules registered"
        end

        def test_container_has_rule_engine_attached
          backend = Bootstrap.create_backend(
            type_registry: @type_registry,
            function_registry: @function_registry
          )

          container = backend[:container]

          refute_nil container.rule_engine, "Container should have rule_engine attached"
          assert_same backend[:rule_engine], container.rule_engine
        end

        def test_context_provides_access_to_factory
          backend = Bootstrap.create_backend(
            type_registry: @type_registry,
            function_registry: @function_registry
          )

          context = backend[:context]

          assert_respond_to context, :factory
          assert_instance_of Services::Builders::CppAstFactory, context.factory
        end

        def test_context_provides_access_to_checker
          backend = Bootstrap.create_backend(
            type_registry: @type_registry,
            function_registry: @function_registry
          )

          context = backend[:context]

          assert_respond_to context, :checker
          assert_instance_of MLC::Services::SemanticIRTypeChecker, context.checker
        end

        def test_with_custom_runtime_policy
          custom_policy = MLC::Backends::Cpp::RuntimePolicy.new

          backend = Bootstrap.create_backend(
            type_registry: @type_registry,
            function_registry: @function_registry,
            runtime_policy: custom_policy
          )

          container = backend[:container]
          assert_same custom_policy, container.runtime_policy
        end
      end
    end
  end
end
