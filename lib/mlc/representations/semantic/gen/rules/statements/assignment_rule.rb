# frozen_string_literal: true


module MLC
  module Representations
    module Semantic
      module Gen
        module Rules
          module Statements
            class AssignmentRule < BaseRule
              def matches?(node, context)
                services(context).ast_type_checker.assignment_statement?(node)
          end

              def produce(node, context)
                svc = services(context)
                value_ir = context[:value_ir] || context.fetch(:expression_visitor).visit(node.value)

                target = node.target
                unless svc.ast_type_checker.var_ref?(target)
                  svc.type_checker.type_error('assignment target must be a variable', node: node)
                end

                target_name = target.name
                existing_type = svc.var_type_registry.get(target_name)
                unless existing_type
                  svc.type_checker.type_error("assignment to undefined variable '#{target_name}'", node: node)
                end

                svc.type_checker.ensure_compatible_type(
                  value_ir.type,
                  existing_type,
                  "assignment to '#{target_name}'",
                  node: node
                )

                # Track move semantics - if RHS is a variable with Owned<T> type, mark it as moved
                mark_source_as_moved(svc, node.value, value_ir)

                # Reset moved state of target (mutable variable can be reassigned)
                svc.var_type_registry.reset_moved(target_name)

                svc.var_type_registry.set(target_name, existing_type)

                target_ir = svc.ir_builder.var(name: target_name, type: existing_type, origin: node)
                svc.ir_builder.assignment_stmt(target: target_ir, value: value_ir, origin: node)
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