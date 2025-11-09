# frozen_string_literal: true

require_relative '../core/function_registry'
require_relative '../core/type_registry'
require_relative '../services/function_registration_service'
require_relative '../services/type_resolution_service'
require_relative '../services/purity_analyzer'
require_relative '../type_system/type_constraint_solver'
require_relative '../type_system/generic_call_resolver'
require_relative '../type_system/match_analyzer'
require_relative '../type_system/effect_analyzer'
require_relative 'rules/function_effect_rule'

module MLC
  module SemanticGen
    # Pipeline composes services, reducers, and passes to build SemanticIR
    class Pipeline
      attr_reader :type_registry, :function_registry, :services

      def initialize(function_registry: nil, type_registry: nil, rule_engine: nil)
        @type_registry = type_registry || MLC::Core::TypeRegistry.new
        @function_registry = function_registry || MLC::Core::FunctionRegistry.new
        @services = Services::Container.new(
          function_registry: @function_registry,
          type_registry: @type_registry
        )

        @function_registration_service = MLC::Services::FunctionRegistrationService.new(
          function_registry: @function_registry,
          module_context_service: @services.module_context_service
        )

        @type_resolution_service = MLC::Services::TypeResolutionService.new(
          function_registry: @function_registry,
          type_registry: @type_registry,
          type_checker: @services.type_checker
        )

        @purity_analyzer = MLC::Services::PurityAnalyzer.new
        @effect_analyzer = MLC::TypeSystem::EffectAnalyzer.new(
          pure_expression: @purity_analyzer.method(:is_pure_expression),
          non_literal_type: @purity_analyzer.method(:non_literal_type?)
        )

        @engine = Engine.new(
          function_registry: @function_registry,
          type_registry: @type_registry,
          services: @services,
          rule_engine: rule_engine
        )

        @rule_engine = @engine.rule_engine
        ensure_function_rule(@rule_engine)

        @function_reducer = Reducers::FunctionReducer.new(
          services: @services,
          engine: @engine,
          function_registration_service: @function_registration_service,
          rule_engine: @rule_engine,
          effect_analyzer: @effect_analyzer
        )

        @type_reducer = Reducers::TypeReducer.new(
          services: @services,
          rule_engine: @rule_engine
        )

        @module_import_pass = Passes::ModuleImportPass.new(
          import_service: @services.import_service,
          function_registry: @function_registry,
          type_registry: @type_registry
        )

        @program_reducer = Reducers::ProgramReducer.new(
          services: @services,
          type_reducer: @type_reducer,
          function_reducer: @function_reducer,
          module_import_pass: @module_import_pass,
          type_resolution_service: @type_resolution_service
        )
      end

      def transform(ast)
        @program_reducer.reduce(ast)
      end

      private

      def ensure_function_rule(engine)
        rules = engine.registry[:core_ir_function]
        return if rules.any? { |rule| rule.is_a?(MLC::SemanticGen::Rules::FunctionEffectRule) }

        engine.register(:core_ir_function, MLC::SemanticGen::Rules::FunctionEffectRule.new)
      end
    end
  end
end
