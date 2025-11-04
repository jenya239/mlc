# frozen_string_literal: true

require_relative "../../base_rule"

module MLC
  module Rules
    module IRGen
      module Expression
        # WhileLoopRule: Transform AST while-loop expressions to HighIR
        # Contains FULL logic (no delegation to transformer)
        # Wraps while-loop statement in block expression with unit result
        class WhileLoopRule < BaseRule
          def applies?(node, _context = {})
            node.is_a?(MLC::AST::WhileLoop)
          end

          def apply(node, context = {})
            transformer = context.fetch(:transformer)
            expression_visitor = context.fetch(:expression_visitor)

            # Transform as statement-level while-loop
            loop_stmt = transformer.send(:transform_while_statement, node.condition, node.body)

            # Wrap in block expression with unit result (while-loops return unit)
            unit_result = MLC::HighIR::Builder.unit_literal(origin: node.origin)
            MLC::HighIR::Builder.block_expr(
              [loop_stmt],
              unit_result,
              unit_result.type
            )
          end
        end
      end
    end
  end
end
