# frozen_string_literal: true

module MLC
  module Representations
    module Semantic
      module Gen
        module Rules
          # IfRule - builds SemanticIR if expressions
          class IfRule < BaseRule
            def matches?(node, context)
              services(context).ast_type_checker.if_expr?(node)
            end

            def produce(node, context)
              svc = services(context)
              visitor = context.fetch(:expression_visitor)

              condition_ir = context[:condition_ir] || visitor.visit(node.condition)
              svc.type_checker.ensure_boolean_type(condition_ir.type, 'if condition', node: node.condition)

              then_ir = context[:then_ir] || visitor.visit(node.then_branch)
              else_ir = if context.key?(:else_ir)
                          context[:else_ir]
                        elsif node.else_branch
                          visitor.visit(node.else_branch)
                        end

              # Auto-call zero-arity constructors so branch types match
              then_ir = maybe_autocall(then_ir, else_ir&.type, svc.ir_builder, node)
              else_ir = maybe_autocall(else_ir, then_ir.type, svc.ir_builder, node) if else_ir

              result_type = determine_type(
                type_checker: svc.type_checker,
                ir_builder: svc.ir_builder,
                then_ir: then_ir,
                else_ir: else_ir,
                node: node
              )

              svc.ir_builder.if_expr(
                cond: condition_ir,
                then_branch: then_ir,
                else_branch: else_ir,
                type: result_type,
                origin: node
              )
            end

            private

            # If ir has type FunctionType([]) and expected_type is not a function type,
            # wrap it in a zero-arg call to normalize to the value type.
            def maybe_autocall(ir, expected_type, ir_builder, node)
              return ir unless ir
              return ir unless ir.type.is_a?(SemanticIR::FunctionType)
              return ir if expected_type.nil? || expected_type.is_a?(SemanticIR::FunctionType)

              params = ir.type.params || []
              return ir unless params.empty?

              ret_type = ir.type.ret_type
              ir_builder.call(func: ir, args: [], type: ret_type, origin: node)
            end

            def determine_type(type_checker:, ir_builder:, then_ir:, else_ir:, node:)
              if else_ir
                type_checker.ensure_compatible_type(
                  else_ir.type,
                  then_ir.type,
                  'if expression branches',
                  node: node
                )
                then_ir.type
              else
                MLC::SemanticIR::UnitType.new(origin: node)
              end
            end
          end
        end
      end
    end
  end
end
