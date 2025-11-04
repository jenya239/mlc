# frozen_string_literal: true

require_relative "../../base_rule"

module MLC
  module Rules
    module IRGen
      module Statement
        # AssignmentRule: Transform AST assignment statements to HighIR
        # Contains FULL logic (no delegation to transformer)
        # Validates target variable exists and value type is compatible
        class AssignmentRule < BaseRule
          def applies?(node, _context = {})
            node.is_a?(MLC::AST::Assignment)
          end

          def apply(node, context = {})
            transformer = context.fetch(:transformer)
            expression_visitor = context.fetch(:expression_visitor)
            statement_visitor = context.fetch(:statement_visitor)
            type_checker = context.fetch(:type_checker)
            var_type_registry = context.fetch(:var_type_registry)

            # Validate: target must be variable reference
            unless node.target.is_a?(MLC::AST::VarRef)
              type_checker.type_error("Assignment target must be a variable", node: node)
            end

            target_name = node.target.name

            # Validate: variable must exist
            existing_type = var_type_registry.get(target_name)
            unless existing_type
              type_checker.type_error("Assignment to undefined variable '#{target_name}'", node: node)
            end

            # Phase 23-C: Support both visitor path (with value_ir) and legacy path (without)
            value_ir = if context.key?(:value_ir)
                         # New path: Visitor provides pre-transformed IR
                         context.fetch(:value_ir)
                       else
                         # Legacy path: Transform inline for backward compatibility
                         expression_visitor.visit(node.value)
                       end

            # Validate: value type must be compatible with variable type
            type_checker.ensure_compatible(value_ir.type, existing_type, "assignment to '#{target_name}'")

            # Update variable type in scope (may refine type)
            var_type_registry.set(target_name, existing_type)

            # Build assignment statement
            target_ir = MLC::HighIR::Builder.var(target_name, existing_type)
            [MLC::HighIR::Builder.assignment_stmt(target_ir, value_ir)]
          end
        end
      end
    end
  end
end
