# frozen_string_literal: true

require_relative "../../base_rule"

module MLC
  module Rules
    module IRGen
      module Expression
        # UnaryRule: Transform AST unary operations to HighIR unary expressions
        # Phase 23-B: Visitor provides pre-transformed IR nodes via context
        # Rule focuses on semantics (type inference + IR construction)
        class UnaryRule < BaseRule
          def applies?(node, _context = {})
            node.is_a?(MLC::AST::UnaryOp)
          end

          def apply(node, context = {})
            type_inference = context.fetch(:type_inference)

            # Visitor already transformed operand - get from context
            operand_ir = context.fetch(:operand_ir)

            # Infer unary operation result type
            type = type_inference.infer_unary_type(node.op, operand_ir.type)

            # Build HighIR unary expression
            MLC::HighIR::Builder.unary(node.op, operand_ir, type)
          end
        end
      end
    end
  end
end
