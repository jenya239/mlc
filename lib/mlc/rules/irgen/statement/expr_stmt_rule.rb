# frozen_string_literal: true

require_relative "../../base_rule"

module MLC
  module Rules
    module IRGen
      module Statement
        # ExprStmtRule: Transform AST expression statements to HighIR
        # Contains FULL logic (no delegation to transformer)
        # Handles special cases: loops, if-expressions, blocks
        class ExprStmtRule < BaseRule
          def applies?(node, _context = {})
            node.is_a?(MLC::AST::ExprStmt)
          end

          def apply(node, context = {})
            transformer = context.fetch(:transformer)
            expression_visitor = context.fetch(:expression_visitor)
            statement_visitor = context.fetch(:statement_visitor)
            expr = node.expr

            # Phase 23-C: Support both visitor path (with expr_ir) and legacy path
            if context.key?(:expr_ir)
              # New path: Visitor provides pre-transformed expression IR
              expr_ir = context.fetch(:expr_ir)

              # Check if expr_ir is already a statement type (ForStmt, WhileStmt, IfStmt)
              # In that case, return it directly
              if expr_ir.is_a?(MLC::HighIR::ForStmt) ||
                 expr_ir.is_a?(MLC::HighIR::WhileStmt) ||
                 expr_ir.is_a?(MLC::HighIR::IfStmt)
                [expr_ir]
              elsif expr_ir.is_a?(MLC::HighIR::BlockExpr)
                # Block expression as statement - flatten statements
                expr_ir.statements
              else
                # General expression - wrap in expression statement
                [MLC::HighIR::Builder.expr_statement(expr_ir)]
              end
            else
              # Legacy path: Transform inline for backward compatibility
              case expr
              when MLC::AST::ForLoop
                # For-loop as statement
                [transformer.send(:transform_for_statement, expr)]

              when MLC::AST::IfExpr
                # If-expression as statement
                [transformer.send(:transform_if_statement, expr.condition, expr.then_branch, expr.else_branch)]

              when MLC::AST::WhileLoop
                # While-loop as statement
                [transformer.send(:transform_while_statement, expr.condition, expr.body)]

              when MLC::AST::Block
                # Block expression as statement (flatten statements)
                # Phase 25-C: Migrated from transformer.send() to visitor
                expression_visitor.visit_block(expr, require_value: false).statements

              else
                # General expression as statement
                ir = expression_visitor.visit(expr)

                # Special case: if-expression with unit type - convert to if-statement
                if expr.is_a?(MLC::AST::IfExpr) && ir.is_a?(MLC::HighIR::IfExpr)
                  MLC::HighIR::Builder.if_stmt(
                    ir.condition,
                    transformer.send(:transform_statement_block, expr.then_branch),
                    expr.else_branch ? transformer.send(:transform_statement_block, expr.else_branch) : nil
                  )
                  []
                else
                  # Wrap expression in expression statement
                  [MLC::HighIR::Builder.expr_statement(ir)]
                end
              end
            end
          end
        end
      end
    end
  end
end
