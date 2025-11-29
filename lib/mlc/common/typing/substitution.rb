# frozen_string_literal: true

module MLC
  module Common
    module Typing
      # Pure functions for type variable substitution
      # No side effects, no dependencies, no state
      module Substitution
        module_function

        # Apply type variable substitutions recursively to a type
        # Replaces type variables with their concrete types from the substitutions map
        #
        # @param type [SemanticIR::Type] The type to apply substitutions to
        # @param substitutions [Hash<String, SemanticIR::Type>] Map from type variable names to concrete types
        # @return [SemanticIR::Type] New type with substitutions applied
        def apply(type, substitutions)
          return type if substitutions.empty?

          case type
          when SemanticIR::TypeVariable
            substitutions[type.name] || type

          when SemanticIR::GenericType
            base = apply(type.base_type, substitutions)
            args = type.type_args.map { |arg| apply(arg, substitutions) }
            SemanticIR::Builder.generic_type(base, args)

          when SemanticIR::ArrayType
            SemanticIR::Builder.array_type(apply(type.element_type, substitutions))

          when SemanticIR::FunctionType
            params = type.params.map do |param|
              { name: param[:name], type: apply(param[:type], substitutions) }
            end
            ret_type = apply(type.ret_type, substitutions)
            SemanticIR::Builder.function_type(params, ret_type)

          when SemanticIR::RecordType
            fields = type.fields.map do |field|
              { name: field[:name], type: apply(field[:type], substitutions) }
            end
            SemanticIR::Builder.record_type(type.name, fields)

          when SemanticIR::SumType
            variants = type.variants.map do |variant|
              fields = Array(variant[:fields]).map do |field|
                { name: field[:name], type: apply(field[:type], substitutions) }
              end
              { name: variant[:name], fields: fields }
            end
            SemanticIR::Builder.sum_type(type.name, variants)

          else
            type
          end
        end
      end
    end
  end
end
