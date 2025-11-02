# frozen_string_literal: true

require_relative "../../base_rule"

module MLC
  module Rules
    module IRGen
      module Expression
        # VarRefRule: Transform AST variable references to HighIR variables
        # Phase 23-B: Already clean - no child transformations needed
        # Rule focuses on semantics (type inference)
        class VarRefRule < BaseRule
          def applies?(node, _context = {})
            node.is_a?(MLC::AST::VarRef)
          end

          def apply(node, context = {})
            type_inference = context.fetch(:type_inference)

            # Infer variable type from type inference service
            type = type_inference.infer_variable_type(node.name)

            # Build HighIR variable reference
            MLC::HighIR::Builder.var(node.name, type)
          end
        end
      end
    end
  end
end
