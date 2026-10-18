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
              constructed = nullary_variant_value_type(svc, name, type)
              type = constructed if constructed
              svc.ir_builder.var(name: name, type: type, origin: node)
            end

            private

            def nullary_variant_value_type(svc, name, type)
              return nil unless type.is_a?(SemanticIR::FunctionType)
              return nil unless Array(type.params).empty?

              info = svc.function_registry.fetch(name)
              return nil if info.nil? || Array(info.type_params).any?

              return_type = type.ret_type
              sum_type = if return_type.is_a?(SemanticIR::SumType)
                           return_type
                         elsif return_type.is_a?(SemanticIR::GenericType) &&
                               return_type.base_type.is_a?(SemanticIR::SumType)
                           return_type.base_type
                         end
              return nil unless sum_type

              variant = Array(sum_type.variants).find { |entry| entry[:name] == name }
              return nil unless variant && Array(variant[:fields]).empty?

              return_type
            end

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
