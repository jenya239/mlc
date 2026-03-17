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
                type_registry: svc.type_registry,
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

            def determine_type(type_checker:, ir_builder:, type_registry:, then_ir:, else_ir:, node:)
              if else_ir
                then_type = then_ir.type
                else_type = else_ir.type

                # When branches have different types, check if they share a parent sum type
                parent = common_sum_parent(then_type, else_type, type_registry)
                return parent if parent

                type_checker.ensure_compatible_type(
                  else_type,
                  then_type,
                  'if expression branches',
                  node: node
                )
                then_type
              else
                MLC::SemanticIR::UnitType.new(origin: node)
              end
            end

            # Returns parent SumType if both types are record variants of the same sum type
            def common_sum_parent(then_type, else_type, type_registry)
              return nil unless type_registry
              return nil if then_type.class == else_type.class && type_name(then_type) == type_name(else_type)

              then_name = type_name(then_type)
              else_name = type_name(else_type)
              return nil unless then_name && else_name

              parent_info = type_registry.find_type_with_variant(then_name)
              return nil unless parent_info
              return nil unless parent_info.variant?(else_name)

              parent_info.core_ir_type
            end

            def type_name(type)
              type.respond_to?(:name) ? type.name : nil
            end
          end
        end
      end
    end
  end
end
