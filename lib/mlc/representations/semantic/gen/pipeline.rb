# frozen_string_literal: true

require_relative '../../../registries/function_registry'
require_relative '../../../registries/type_registry'
require_relative 'services/registration/function_registration_service'
require_relative 'services/utils/type_resolution_service'
require_relative 'services/utils/purity_analyzer'
require_relative '../../../common/typing/type_constraint_solver'
require_relative '../../../common/typing/generic_call_resolver'
require_relative '../../../common/typing/match_analyzer'
require_relative '../../../common/typing/effect_analyzer'
require_relative 'rules/function_effect_rule'
require_relative 'reducers/function_reducer'
require_relative 'reducers/type_reducer'
require_relative 'reducers/program_reducer'
require_relative 'passes/module_import_pass'

module MLC
  module Representations
    module Semantic
      module Gen
        # Pipeline composes services, reducers, and passes to build SemanticIR
        class Pipeline
          attr_reader :type_registry, :function_registry, :services

          def initialize(function_registry: nil, type_registry: nil, rule_engine: nil)
            @type_registry = type_registry || MLC::Registries::TypeRegistry.new
            @function_registry = function_registry || MLC::Registries::FunctionRegistry.new
            @services = MLC::Representations::Semantic::Gen::Services::Container.new(
              function_registry: @function_registry,
              type_registry: @type_registry
            )

            @function_registration_service = MLC::Representations::Semantic::Gen::Services::FunctionRegistrationService.new(
              function_registry: @function_registry,
              module_context_service: @services.module_context_service
            )

            @type_resolution_service = MLC::Representations::Semantic::Gen::Services::TypeResolutionService.new(
              function_registry: @function_registry,
              type_registry: @type_registry,
              type_checker: @services.type_checker
            )

            @purity_analyzer = MLC::Representations::Semantic::Gen::Services::PurityAnalyzer.new
            @effect_analyzer = MLC::Common::Typing::EffectAnalyzer.new(
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
            return if rules.any? { |rule| rule.is_a?(MLC::Representations::Semantic::Gen::Rules::FunctionEffectRule) }

            engine.register(:core_ir_function, MLC::Representations::Semantic::Gen::Rules::FunctionEffectRule.new)
          end
        end # class Pipeline
      end   # module Gen
    end     # module Semantic
  end       # module Representations
end         # module MLC
