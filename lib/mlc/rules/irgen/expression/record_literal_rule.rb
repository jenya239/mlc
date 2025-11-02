# frozen_string_literal: true

require_relative "../../base_rule"

module MLC
  module Rules
    module IRGen
      module Expression
        # RecordLiteralRule: Transform AST record literals to HighIR record expressions
        # Phase 23-B: Visitor provides pre-transformed IR nodes via context
        # Rule focuses on semantics (record type construction)
        class RecordLiteralRule < BaseRule
          def applies?(node, _context = {})
            node.is_a?(MLC::AST::RecordLit)
          end

          def apply(node, context = {})
            record_builder = context.fetch(:record_builder)

            # Visitor already transformed field values - get from context
            fields_ir = context.fetch(:fields_ir)

            # Determine record type based on type_name
            if node.type_name == "record"
              # Anonymous record - infer from context/registry or build anonymous
              record_builder.infer_from_context(fields_ir) ||
                record_builder.infer_from_registry(fields_ir) ||
                record_builder.build_anonymous(fields_ir)
            else
              # Named record - build with explicit type name
              record_builder.build_named(node.type_name, fields_ir)
            end
          end
        end
      end
    end
  end
end
