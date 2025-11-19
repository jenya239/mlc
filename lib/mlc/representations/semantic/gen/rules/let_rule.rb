# frozen_string_literal: true


module MLC
  module Representations
    module Semantic
      module Gen
        module Rules
          # LetRule - handles let bindings by producing a block expression
          class LetRule < BaseRule
            def matches?(node, context)
              services(context).ast_type_checker.let?(node)
        end

            def produce(node, context)
              svc = services(context)
              visitor = context.fetch(:expression_visitor)
              var_registry = svc.var_type_registry

              saved = var_registry.snapshot

              value_ir = visitor.visit(node.value)
              ensure_type!(svc, value_ir.type, "let binding '#{node.name}'", node)
              var_registry.set(node.name, value_ir.type)

              body_ir = visitor.visit(node.body)
              ensure_type!(svc, body_ir.type, "let body for '#{node.name}'", node)

              decl_stmt = svc.ir_builder.variable_decl_stmt(
                name: node.name,
                type: value_ir.type,
                value: value_ir,
                mutable: node.mutable,
                origin: node
              )

              svc.ir_builder.block_expr(
                statements: [decl_stmt],
                result: body_ir,
                type: body_ir.type,
                origin: node
              )
            ensure
              var_registry.restore(saved) if saved
        end

            private

            def ensure_type!(services, type, context, node)
              services.type_checker.ensure_type!(type, "#{context} has unknown type", node: node)
        end
          end
        end
          end
        end
      end
    end