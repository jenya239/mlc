# frozen_string_literal: true

require_relative "../statement_rule"

module MLC
  module Backends
    module Cpp
      module Rules
        module Statements
          # Rule for lowering SemanticIR::AssignmentStmt to C++ assignments
          class AssignmentRule < StatementRule
            def applies?(node)
              context.checker.assignment?(node)
            end

            def apply(node)
              # Lower left and right sides
              left_expr = context.lower_expression(node.target)
              right_expr = context.lower_expression(node.value)

              # Create assignment expression
              assignment = CppAst::Nodes::AssignmentExpression.new(
                left: left_expr,
                operator: "=",
                right: right_expr
              )

              # Wrap in expression statement
              context.factory.expression_statement(expression: assignment)
            end
          end
        end
      end
    end
  end
end
