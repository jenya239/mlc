# frozen_string_literal: true

module MLC
  module Common
    module Typing
      # Pure functions for generic type parameter inference
      # No side effects, no dependencies, no state
      module Generics
        module_function

        # Infer type variable substitutions from a generic scrutinee type
        # Used in pattern matching to specialize generic constructor types
        #
        # @param type_params [Array<TypeVariable>] The type parameters to infer
        # @param ret_type [SemanticIR::Type] The return type of the constructor
        # @param scrutinee_type [SemanticIR::Type] The concrete type being matched on
        # @return [Hash<String, SemanticIR::Type>] Map from type parameter names to concrete types
        def infer_substitutions(type_params, ret_type, scrutinee_type)
          return {} unless type_params&.any?
          return {} unless scrutinee_type.is_a?(SemanticIR::GenericType)

          # Check if base types match
          ret_base = ret_type&.name
          scrutinee_base = scrutinee_type.base_type&.name
          return {} unless ret_base == scrutinee_base

          # Map type parameters to actual type arguments by position
          substitutions = {}
          type_params.each_with_index do |param, idx|
            actual_arg = scrutinee_type.type_args[idx]
            substitutions[param.name] = actual_arg if actual_arg
          end
          substitutions
        end
      end
    end
  end
end
