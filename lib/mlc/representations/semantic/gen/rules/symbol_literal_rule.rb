# frozen_string_literal: true

module MLC
  module Representations
    module Semantic
      module Gen
        module Rules
          # SymbolLiteralRule - builds SemanticIR symbol expressions
          class SymbolLiteralRule < BaseRule
            def matches?(node, context)
              services(context).ast_type_checker.symbol_literal?(node)
            end

            def produce(node, context)
              svc = services(context)
              symbol_type = svc.ir_builder.symbol_type

              svc.ir_builder.symbol_expr(
                name: node.name,
                type: symbol_type,
                origin: node.origin
              )
            end
          end
        end
      end
    end
  end
end
