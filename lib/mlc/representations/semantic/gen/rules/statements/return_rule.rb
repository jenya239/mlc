# frozen_string_literal: true


module MLC
  module Representations
    module Semantic
      module Gen
        module Rules
          module Statements
            class ReturnRule < BaseRule
              def matches?(node, context)
                services(context).ast_type_checker.return_statement?(node)
          end

              def produce(node, context)
                svc = services(context)
                scope = svc.scope_context
                expected_type = scope.current_function_return

                raise MLC::CompileError, 'return outside of function' unless expected_type

                value_ir = context[:value_ir]
                if expected_type && !unit_type?(expected_type, svc)
                  unless value_ir
                    svc.type_checker.type_error('return statement requires a value', node: node)
              end

                  svc.type_checker.ensure_type!(value_ir.type, 'return expression', node: node)
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
                name = services.type_checker.normalized_type_name(
                  services.type_checker.type_name(type)
                )
                return true if %w[unit void].include?(name)
                type.is_a?(MLC::SemanticIR::UnitType)
          end
            end
          end
            end
          end
        end
      end
    end