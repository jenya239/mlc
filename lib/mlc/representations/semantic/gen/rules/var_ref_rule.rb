# frozen_string_literal: true


module MLC
  module Representations
    module Semantic
      module Gen
        module Rules
          # VarRefRule - Handles variable and function references
          class VarRefRule < BaseRule
            def matches?(node, context)
              services(context).ast_type_checker.var_ref?(node)
            end

            def produce(node, context)
              svc = services(context)
              name = node.name

              # Check for use-after-move
              check_use_after_move!(svc, name, node)

              type = svc.type_inference_service.infer_variable_type(name)
              svc.ir_builder.var(name: name, type: type, origin: node)
            end

            private

            def check_use_after_move!(svc, name, node)
              return unless svc.var_type_registry.moved?(name)

              type = svc.var_type_registry.get(name)
              return unless Services::VarTypeRegistry.has_move_semantics?(type)

              raise MLC::CompileError, "use of moved value '#{name}' - ownership was previously transferred"
            end
          end
        end
          end
        end
      end
    end