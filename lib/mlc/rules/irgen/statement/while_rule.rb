# frozen_string_literal: true

require_relative "../../base_rule"

module MLC
  module Rules
    module IRGen
      module Statement
        # WhileRule: Transform AST while statements to HighIR
        # Contains FULL logic (no delegation to transformer)
        # Manages loop depth for break/continue validation
        class WhileRule < BaseRule
          def applies?(node, _context = {})
            node.is_a?(MLC::AST::WhileStmt)
          end

          def apply(node, context = {})
            transformer = context.fetch(:transformer)
            type_checker = context.fetch(:type_checker)

            # Phase 23-C: Support both visitor path (with cond_ir/body_ir) and legacy path
            condition_ir = if context.key?(:cond_ir)
                             # New path: Visitor provides pre-transformed IR
                             context.fetch(:cond_ir)
                           else
                             # Legacy path: Transform inline for backward compatibility
                             transformer.send(:transform_expression, node.condition)
                           end

            # Validate boolean condition type
            type_checker.ensure_boolean(condition_ir.type, "while condition", node: node.condition)

            # Transform body: support both BlockExpr (legacy) and array of statements (visitor)
            body_ir = if context.key?(:body_ir)
                        # New path: Visitor provides array of statements
                        body_stmts = context.fetch(:body_ir)
                        # Wrap in BlockExpr if it's an array
                        body_stmts.is_a?(Array) ? wrap_in_block(body_stmts) : body_stmts
                      else
                        # Legacy path: Transform to BlockExpr within loop scope
                        transformer.send(:within_loop_scope) do
                          transformer.send(:transform_statement_block, node.body, preserve_scope: true)
                        end
                      end

            # Build while statement
            [MLC::HighIR::Builder.while_stmt(condition_ir, body_ir)]
          end

          private

          # Wrap array of statements in BlockExpr for WhileStmt compatibility
          def wrap_in_block(statements)
            MLC::HighIR::Builder.block_expr(
              statements,
              nil,
              MLC::HighIR::Builder.primitive_type("void")
            )
          end
        end
      end
    end
  end
end
