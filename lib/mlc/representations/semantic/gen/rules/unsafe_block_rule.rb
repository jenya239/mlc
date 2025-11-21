# frozen_string_literal: true

module MLC
  module Representations
    module Semantic
      module Gen
        module Rules
          # UnsafeBlockRule - transforms AST UnsafeBlock to SemanticIR UnsafeBlockExpr
          # Unsafe blocks enable the use of reference types (&T, &mut T)
          class UnsafeBlockRule < BaseRule
            def matches?(node, context)
              services(context).ast_type_checker.unsafe_block?(node)
            end

            def produce(node, context)
              svc = services(context)
              expression_visitor = context.fetch(:expression_visitor)

              # Transform body within unsafe context
              body_ir = svc.scope_context.with_unsafe_context do
                expression_visitor.visit(node.body)
              end

              body_type = body_ir&.type || svc.ir_builder.prim_type(name: "unit", origin: node)

              svc.ir_builder.unsafe_block_expr(
                body: body_ir,
                type: body_type,
                origin: node
              )
            end
          end
        end
      end
    end
  end
end
