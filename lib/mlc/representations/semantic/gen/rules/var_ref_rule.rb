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

              check_use_after_move!(svc, name, node)

              # Implicit self: if name not in scope but self is, and self's type has a field `name`
              if !svc.var_type_registry.has?(name) && svc.var_type_registry.has?("self")
                self_type = svc.var_type_registry.get("self")
                if self_type.is_a?(SemanticIR::RecordType)
                  field = self_type.fields.find { |f| f[:name] == name }
                  if field
                    self_ir = svc.ir_builder.var(name: "self", type: self_type, origin: node)
                    return svc.ir_builder.member(object: self_ir, member: name, type: field[:type], origin: node)
                  end
                end
              end

              type = svc.type_inference_service.infer_variable_type(name)
              svc.ir_builder.var(name: name, type: type, origin: node)
            end

            private

            def check_use_after_move!(svc, name, _node)
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
