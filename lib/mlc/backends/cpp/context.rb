# frozen_string_literal: true

require_relative "services/utils/string_helpers"
require_relative "services/utils/type_mapper"
require_relative "services/policies/lowering_policy"
require_relative "services/builders/template_builder"
require_relative "services/helpers/function_helper"

module MLC
  module Backends
    module Cpp
      # Public API for code generation rules
      # Replaces direct lowerer.send(:private_method) calls
      class Context
        def initialize(container)
          @container = container
        end

        def type_registry
          @container.type_registry
        end

        def function_registry
          @container.function_registry
        end

        # Generic function context flag
        def in_generic_function?
          @container.in_generic_function
        end

        # Runtime policy for strategy selection
        def runtime_policy
          @container.runtime_policy
        end

        # Factory for CppAst::Nodes construction
        def factory
          @container.cpp_ast_factory
        end

        # Type checker for SemanticIR nodes
        def checker
          @container.semantic_ir_checker
        end

        # Type mapping methods - delegating to TypeMapper
        def map_type(type)
          Services::Utils::TypeMapper.map_type(
            type,
            type_map: @container.type_map,
            type_registry: @container.type_registry
          )
        end

        def type_requires_auto?(type, type_str: nil)
          Services::Utils::TypeMapper.type_requires_auto?(
            type,
            type_map: @container.type_map,
            type_registry: @container.type_registry,
            type_str: type_str
          )
        end

        def contains_type_variables?(type)
          Services::Utils::TypeMapper.contains_type_variables?(type)
        end

        def extract_base_type_name(type)
          Services::Utils::TypeMapper.extract_base_type_name(
            type,
            type_map: @container.type_map,
            type_registry: @container.type_registry
          )
        end

        # String formatting methods - delegating to StringHelpers
        def escape_string(str)
          Services::Utils::StringHelpers.escape_cpp_string(str)
        end

        def cpp_string_literal(str)
          Services::Utils::StringHelpers.cpp_string_literal(str)
        end

        def sanitize_identifier(name)
          Services::Utils::StringHelpers.sanitize_identifier(name)
        end

        def cpp_keyword?(name)
          Services::Utils::StringHelpers.cpp_keyword?(name)
        end

        # Template methods - delegating to TemplateBuilder
        def generate_lambda_type(params, return_type)
          # TODO: Delegate to template builder service
          "std::function<#{return_type}(#{params.join(', ')})>"
        end

        def build_template_signature(type_params)
          Services::Builders::TemplateBuilder.build_template_signature(type_params)
        end

        # Policy methods - delegating to LoweringPolicy
        def should_lower_as_statement?(expr_or_type)
          Services::Policies::LoweringPolicy.should_lower_as_statement?(expr_or_type)
        end

        # Function helper methods - delegating to FunctionHelper
        def qualified_function_name(name)
          Services::Helpers::FunctionHelper.qualified_function_name(
            name,
            @container.function_registry
          )
        end

        # Recursively lower an expression node via RuleEngine
        def lower_expression(node)
          raise "RuleEngine not initialized" unless @container.rule_engine

          @container.rule_engine.lower_expression(node)
        end

        # Recursively lower a statement node via RuleEngine
        def lower_statement(node)
          raise "RuleEngine not initialized" unless @container.rule_engine

          @container.rule_engine.lower_statement(node)
        end
      end
    end
  end
end
