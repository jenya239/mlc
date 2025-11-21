# frozen_string_literal: true


module MLC
  module Representations
    module Semantic
      module Gen
        module Rules
          module Statements
            class VariableDeclRule < BaseRule
              def matches?(node, context)
                services(context).ast_type_checker.variable_decl_statement?(node)
          end

              def produce(node, context)
                svc = services(context)
                value_ir = context[:value_ir] || context.fetch(:expression_visitor).visit(node.value)

                type_checker = svc.type_checker
                type_checker.ensure_type!(value_ir.type, "initializer for '#{node.name}'", node: node)

                final_type = value_ir.type

                if node.type
                  expected_type = svc.type_builder.transform(node.type)
                  type_checker.ensure_compatible_type(
                    value_ir.type,
                    expected_type,
                    "initializer for '#{node.name}'",
                    node: node
                  )
                  type_checker.assign_expression_type(value_ir, expected_type)
                  final_type = expected_type
                end

                # Track move semantics - if RHS is a variable with Owned<T> type, mark it as moved
                mark_source_as_moved(svc, node.value, value_ir)

                svc.var_type_registry.set(node.name, final_type, initializer: value_ir)

                svc.ir_builder.variable_decl_stmt(
                  name: node.name,
                  type: final_type,
                  value: value_ir,
                  mutable: node.mutable,
                  origin: node
                )
              end

              private

              def mark_source_as_moved(svc, ast_node, value_ir)
                return unless svc.ast_type_checker.var_ref?(ast_node)
                return unless Services::VarTypeRegistry.has_move_semantics?(value_ir.type)

                source_name = ast_node.name
                svc.var_type_registry.mark_moved(source_name)
              end
            end
          end
            end
          end
        end
      end
    end