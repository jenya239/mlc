# frozen_string_literal: true

require_relative "../../base_rule"

module MLC
  module Rules
    module IRGen
      module Expression
        # BinaryRule: Transform AST binary operations to HighIR binary expressions
        # Phase 23-B: Visitor provides pre-transformed IR nodes via context
        # Rule focuses on semantics (type inference + IR construction)
        class BinaryRule < BaseRule
          def applies?(node, _context = {})
            node.is_a?(MLC::AST::BinaryOp) && node.op != "|>"
          end

          def apply(node, context = {})
            type_inference = context.fetch(:type_inference)

            # Visitor already transformed operands - get from context
            left_ir = context.fetch(:left_ir)
            right_ir = context.fetch(:right_ir)

            # Infer binary operation result type
            type = type_inference.infer_binary_type(node.op, left_ir.type, right_ir.type)

            # Build HighIR binary expression
            MLC::HighIR::Builder.binary(node.op, left_ir, right_ir, type)
          end
        end
      end
    end
  end
end
