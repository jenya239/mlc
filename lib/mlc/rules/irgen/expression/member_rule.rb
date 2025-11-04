# frozen_string_literal: true

require_relative "../../base_rule"

module MLC
  module Rules
    module IRGen
      module Expression
        # MemberRule: Transform AST member access to HighIR member expressions
        # Contains FULL logic (no delegation to transformer)
        class MemberRule < BaseRule
          def applies?(node, _context = {})
            node.is_a?(MLC::AST::MemberAccess)
          end

          def apply(node, context = {})
            transformer = context.fetch(:transformer)
            expression_visitor = context.fetch(:expression_visitor)
            type_inference = context.fetch(:type_inference)

            # Check if this is a module member function (e.g., Math.sqrt)
            entry = transformer.send(:module_member_function_entry, node.object, node.member) rescue nil
            if entry
              # Module function - create variable reference with canonical name
              canonical_name = entry.name
              type = type_inference.function_placeholder_type(canonical_name)
              return MLC::HighIR::Builder.var(canonical_name, type)
            end

            # Regular member access - transform object and infer member type
            object = expression_visitor.visit(node.object)
            type = type_inference.infer_member_type(object.type, node.member)

            # Build HighIR member access expression
            MLC::HighIR::Builder.member(object, node.member, type)
          end
        end
      end
    end
  end
end
