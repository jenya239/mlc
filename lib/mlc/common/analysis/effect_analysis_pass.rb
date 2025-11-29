# frozen_string_literal: true


module MLC
  module Common
    module Analysis
      # EffectAnalysisPass - analyzes SemanticIR functions for effects (constexpr, noexcept)
      # Walks SemanticIR module and annotates functions based on body/return type.
      # Stores results in context[:function_effects] = { function_name => [:constexpr, :noexcept] }
      class EffectAnalysisPass < BasePass
        def initialize(effect_analyzer:, name: "effect_analysis")
          super(name: name)
          @effect_analyzer = effect_analyzer
        end

        def required_keys
          [:core_ir]
        end

        def produced_keys
          [:function_effects]
        end

        def run(context)
          core_ir = context[:core_ir]
          return unless core_ir

          function_effects = {}

          core_ir.items.each do |item|
            next unless item.is_a?(SemanticIR::Func)

            effects = @effect_analyzer.analyze(item.body, return_type: item.ret_type)
            function_effects[item.name] = effects
          end

          context[:function_effects] = function_effects
        end
      end
    end
  end
end
