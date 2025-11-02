# frozen_string_literal: true

require_relative "../../base_rule"

module MLC
  module Rules
    module IRGen
      module Statement
        # IfRule: Transform AST if statements to HighIR
        # Contains FULL logic (no delegation to transformer)
        class IfRule < BaseRule
          def applies?(node, _context = {})
            node.is_a?(MLC::AST::IfStmt)
          end

          def apply(node, context = {})
            transformer = context.fetch(:transformer)
            type_checker = context.fetch(:type_checker)

            # Phase 23-C: Support both visitor path (with cond_ir/then_ir/else_ir) and legacy path
            condition_ir = if context.key?(:cond_ir)
                             # New path: Visitor provides pre-transformed IR
                             context.fetch(:cond_ir)
                           else
                             # Legacy path: Transform inline for backward compatibility
                             transformer.send(:transform_expression, node.condition)
                           end

            # Validate boolean condition type
            type_checker.ensure_boolean(condition_ir.type, "if condition", node: node.condition)

            # Transform branches: support both BlockExpr (legacy) and array of statements (visitor)
            then_ir = if context.key?(:then_ir)
                        # New path: Visitor provides array of statements
                        body_ir = context.fetch(:then_ir)
                        # Wrap in BlockExpr if it's an array
                        body_ir.is_a?(Array) ? wrap_in_block(body_ir) : body_ir
                      else
                        # Legacy path: Transform to BlockExpr
                        transformer.send(:transform_statement_block, node.then_branch)
                      end

            else_ir = if context.key?(:else_ir)
                        # New path: Visitor provides array of statements or nil
                        body_ir = context.fetch(:else_ir)
                        body_ir.nil? ? nil : (body_ir.is_a?(Array) ? wrap_in_block(body_ir) : body_ir)
                      else
                        # Legacy path: Transform to BlockExpr or nil
                        node.else_branch ? transformer.send(:transform_statement_block, node.else_branch) : nil
                      end

            # Build if statement
            [MLC::HighIR::Builder.if_stmt(condition_ir, then_ir, else_ir)]
          end

          private

          # Wrap array of statements in BlockExpr for IfStmt compatibility
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
