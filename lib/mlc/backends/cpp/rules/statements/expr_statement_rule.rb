# frozen_string_literal: true

require_relative "../statement_rule"

module MLC
  module Backends
    module Cpp
      module Rules
        module Statements
          # Rule for lowering SemanticIR::ExprStatement to C++ expression statements
          class ExprStatementRule < StatementRule
            def applies?(node)
              context.checker.expr_stmt?(node)
            end

            def apply(node)
              if context.should_lower_as_statement?(node.expression)
                # Expression with unit type should be lowered as statement
                # Currently only IfExpr with unit type
                if context.checker.if_expr?(node.expression)
                  # Delegate to IfStmt handling - lower as statement
                  context.lower_statement(node.expression)
                else
                  raise "Unknown statement-like expression: #{node.expression.class}"
                end
              else
                # Regular expression statement
                expr = context.lower_expression(node.expression)
                context.factory.expression_statement(expression: expr)
              end
            end
          end
        end
      end
    end
  end
end
