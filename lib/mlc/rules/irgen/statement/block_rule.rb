# frozen_string_literal: true

require_relative "../../base_rule"

module MLC
  module Rules
    module IRGen
      module Statement
        # BlockRule: Transform AST block statements to HighIR
        # Contains FULL logic (no delegation to transformer)
        # Flattens block statements into statement list
        class BlockRule < BaseRule
          def applies?(node, _context = {})
            node.is_a?(MLC::AST::Block)
          end

          def apply(node, context = {})
            expression_visitor = context.fetch(:expression_visitor)

            # Transform block without requiring value (statement context)
            # Phase 25-C: Migrated from transformer.send() to visitor
            nested = expression_visitor.visit_block(node, require_value: false)

            # Return flattened statements
            nested.statements
          end
        end
      end
    end
  end
end
