# frozen_string_literal: true

require_relative "../../base_rule"

module MLC
  module Rules
    module IRGen
      module Expression
        # IndexAccessRule: Transform AST array indexing to HighIR index expressions
        # Phase 23-B: Visitor provides pre-transformed IR nodes via context
        # Rule focuses on semantics (type checking + IR construction)
        class IndexAccessRule < BaseRule
          def applies?(node, _context = {})
            node.is_a?(MLC::AST::IndexAccess)
          end

          def apply(node, context = {})
            type_checker = context.fetch(:type_checker)

            # Visitor already transformed object and index - get from context
            object_ir = context.fetch(:object_ir)
            index_ir = context.fetch(:index_ir)

            # Validate: object must be array type
            unless object_ir.type.is_a?(MLC::HighIR::ArrayType)
              type_checker.type_error("Indexing requires an array, got #{type_checker.describe_type(object_ir.type)}", node: node.object)
            end

            # Validate: index must be numeric
            type_checker.ensure_numeric_type(index_ir.type, "array index", node: node.index)

            # Build HighIR index access with array element type
            MLC::HighIR::IndexExpr.new(
              object: object_ir,
              index: index_ir,
              type: object_ir.type.element_type
            )
          end
        end
      end
    end
  end
end
