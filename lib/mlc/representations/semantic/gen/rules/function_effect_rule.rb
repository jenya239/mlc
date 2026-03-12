# frozen_string_literal: true

module MLC
  module Representations
    module Semantic
      module Gen
        module Rules
          # FunctionEffectRule analyzes SemanticIR functions and annotates effects.
          # Skips :constexpr when params or ret_type are non-literal (C++20).
          class FunctionEffectRule < BaseRule
            def matches?(node, _context)
              node.is_a?(MLC::SemanticIR::Func) && !node.external
            end

            def produce(func, context)
              analyzer = context[:effect_analyzer]
              return func unless analyzer

              effects = analyzer.analyze(func.body, return_type: func.ret_type)
              effects = drop_constexpr_if_non_literal(effects, func)
              return func if effects == func.effects

              MLC::SemanticIR::Func.new(
                name: func.name,
                params: func.params,
                ret_type: func.ret_type,
                body: func.body,
                effects: effects,
                type_params: func.type_params,
                external: func.external,
                exported: func.exported,
                is_async: func.is_async,
                origin: func.origin
              )
            end

            private

            def drop_constexpr_if_non_literal(effects, func)
              return effects unless Array(effects).include?(:constexpr)

              has_non_literal = func.params.any? { |p| non_literal?(p.type) } || non_literal?(func.ret_type)
              return effects unless has_non_literal

              Array(effects).reject { |e| e == :constexpr }
            end

            def non_literal?(type)
              MLC::Representations::Semantic::Gen::Services::Utils::PurityAnalysis.non_literal_type?(type)
            end
          end
        end
      end
    end
  end
end
