# frozen_string_literal: true

require_relative "../../base_rule"

module MLC
  module Rules
    module IRGen
      module Statement
        # ForRule: Transform AST for-loop statements to HighIR
        # Contains FULL logic (no delegation to transformer)
        # Manages loop variable scoping and loop depth for break/continue
        class ForRule < BaseRule
          def applies?(node, _context = {})
            node.is_a?(MLC::AST::ForLoop)
          end

          def apply(node, context = {})
            transformer = context.fetch(:transformer)
            type_inference = context.fetch(:type_inference)
            var_type_registry = context.fetch(:var_type_registry)

            # Phase 23-C: Support both visitor path (with iter_ir) and legacy path
            iterable_ir = if context.key?(:iter_ir)
                            # New path: Visitor provides pre-transformed IR
                            context.fetch(:iter_ir)
                          else
                            # Legacy path: Transform inline for backward compatibility
                            transformer.send(:transform_expression, node.iterable)
                          end

            # Save loop variable if it exists (for scope restoration)
            saved = var_type_registry.get(node.var_name)

            # Infer element type from iterable
            element_type = type_inference.infer_iterable_type(iterable_ir)

            # Add loop variable to scope
            var_type_registry.set(node.var_name, element_type)

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

            # Build for statement
            [MLC::HighIR::Builder.for_stmt(node.var_name, element_type, iterable_ir, body_ir)]
          ensure
            # Restore previous loop variable binding
            if saved
              var_type_registry.set(node.var_name, saved)
            else
              var_type_registry.delete(node.var_name)
            end
          end

          private

          # Wrap array of statements in BlockExpr for ForStmt compatibility
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
