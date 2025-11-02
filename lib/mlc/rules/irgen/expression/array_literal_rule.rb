# frozen_string_literal: true

require_relative "../../base_rule"

module MLC
  module Rules
    module IRGen
      module Expression
        # ArrayLiteralRule: Transform AST array literals to HighIR array expressions
        # Phase 23-B: Visitor provides pre-transformed IR nodes via context
        # Rule focuses on semantics (type checking + IR construction)
        class ArrayLiteralRule < BaseRule
          def applies?(node, _context = {})
            node.is_a?(MLC::AST::ArrayLiteral)
          end

          def apply(node, context = {})
            type_checker = context.fetch(:type_checker)

            # Visitor already transformed elements - get from context
            elements_ir = context.fetch(:elements_ir)

            # Infer element type from first element (or default to i32)
            element_type = if elements_ir.any?
                             elements_ir.first.type
                           else
                             MLC::HighIR::Builder.primitive_type("i32")
                           end

            # Validate type compatibility for all elements
            elements_ir.each_with_index do |elem, index|
              next if index.zero?
              type_checker.ensure_compatible(elem.type, element_type, "array element #{index}")
            end

            # Create array type and build array literal expression
            array_type = MLC::HighIR::ArrayType.new(element_type: element_type)

            MLC::HighIR::ArrayLiteralExpr.new(
              elements: elements_ir,
              type: array_type
            )
          end
        end
      end
    end
  end
end
