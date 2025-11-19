# frozen_string_literal: true


module MLC
  module Representations
    module Semantic
      module Gen
        module Rules
          # FunctionEffectRule analyzes SemanticIR functions and annotates effects.
          class FunctionEffectRule < BaseRule
            def matches?(node, _context)
              node.is_a?(MLC::SemanticIR::Func) && !node.external
        end

            def produce(func, context)
              analyzer = context[:effect_analyzer]
              return func unless analyzer

              effects = analyzer.analyze(func.body, return_type: func.ret_type)
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
                origin: func.origin
              )
        end
          end
        end
          end
        end
      end
    end