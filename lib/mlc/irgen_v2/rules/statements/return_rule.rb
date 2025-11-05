# frozen_string_literal: true

require_relative '../base_rule'

module MLC
  module IRGenV2
    module Rules
      module Statements
        class ReturnRule < BaseRule
          def matches?(node, context)
            node.is_a?(MLC::AST::Return)
          end

          def produce(node, context)
            svc = services(context)
            scope = svc.scope_context
            expected_type = scope.current_function_return

            raise MLC::CompileError, 'return outside of function' unless expected_type

            value_ir = context[:value_ir]
            if expected_type && !unit_type?(expected_type, svc)
              svc.type_checker.ensure_type!(value_ir&.type, 'return expression', node: node)
              svc.type_checker.ensure_compatible_type(
                value_ir.type,
                expected_type,
                'return statement',
                node: node
              )
            elsif value_ir
              svc.type_checker.type_error('return value not allowed in void context', node: node)
            end

            svc.ir_builder.return_node(value: value_ir, origin: node)
          end

          private

          def unit_type?(type, services)
            services.type_checker.normalized_type_name(
              services.type_checker.type_name(type)
            ) == 'unit'
          end
        end
      end
    end
  end
end
