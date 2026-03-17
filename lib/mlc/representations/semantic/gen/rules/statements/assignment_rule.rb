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
                expr_visitor = context.fetch(:expression_visitor)
                value_ir = context[:value_ir] || expr_visitor.visit(node.value)
                target = node.target

                # Index assignment: arr[i] = val -> arr.set(i, val)
                if svc.ast_type_checker.index_access?(target)
                  return produce_index_assignment(node, context, svc, expr_visitor, target, value_ir)
                end

                # Field assignment: obj.field = val -> obj.field = val
                if svc.ast_type_checker.member_access?(target)
                  return produce_field_assignment(node, context, svc, expr_visitor, target, value_ir)
                end

                svc.type_checker.type_error('assignment target must be a variable', node: node) unless svc.ast_type_checker.var_ref?(target)

                target_name = target.name
                existing_type = svc.var_type_registry.get(target_name)
                svc.type_checker.type_error("assignment to undefined variable '#{target_name}'", node: node) unless existing_type
                unless svc.var_type_registry.mutable?(target_name)
                  svc.type_checker.type_error("cannot rebind 'const #{target_name}'", node: node)
                end

                svc.type_checker.ensure_compatible_type(
                  value_ir.type,
                  existing_type,
                  "assignment to '#{target_name}'",
                  node: node
                )

                mark_source_as_moved(svc, node.value, value_ir)
                svc.var_type_registry.reset_moved(target_name)
                svc.var_type_registry.set(target_name, existing_type, mutable: true)

                target_ir = svc.ir_builder.var(name: target_name, type: existing_type, origin: node)
                svc.ir_builder.assignment_stmt(target: target_ir, value: value_ir, origin: node)
              end

              private

              def produce_index_assignment(node, _context, svc, expr_visitor, target, value_ir)
                obj_ir = expr_visitor.visit(target.object)
                idx_ir = expr_visitor.visit(target.index)
                unit_type = MLC::SemanticIR::UnitType.new(origin: node)
                set_fn_type = SemanticIR::Builder.function_type([idx_ir.type, value_ir.type], unit_type)
                member_ir = svc.ir_builder.member(
                  object: obj_ir,
                  member: "set",
                  type: set_fn_type,
                  origin: node
                )
                call_ir = svc.ir_builder.call(
                  func: member_ir,
                  args: [idx_ir, value_ir],
                  type: unit_type,
                  origin: node
                )
                svc.ir_builder.expr_statement(expression: call_ir, origin: node)
              end

              def produce_field_assignment(node, _context, svc, _expr_visitor, target, _value_ir)
                svc.type_checker.type_error(
                  "field assignment 'obj.#{target.member} = ...' is not yet supported",
                  node: node
                )
              end

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
