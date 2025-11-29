# frozen_string_literal: true

module MLC
  module Common
    module Typing
      # Pure functions for type equivalence checking
      # No side effects, no dependencies, no state
      module Equivalence
        module_function

        # Check structural type equivalence (deep equality check)
        # Handles type variables, generic types, array types, and named types
        #
        # @param left [SemanticIR::Type] First type
        # @param right [SemanticIR::Type] Second type
        # @return [Boolean] Whether the types are equivalent
        def equivalent?(left, right)
          return false if left.nil? || right.nil?
          return true if left.equal?(right)

          # Type variables: compare by name
          if left.is_a?(SemanticIR::TypeVariable) && right.is_a?(SemanticIR::TypeVariable)
            return left.name == right.name
          end

          # Generic types: compare base and all type args recursively
          if left.is_a?(SemanticIR::GenericType) && right.is_a?(SemanticIR::GenericType)
            return false unless left.base_type&.name == right.base_type&.name
            return false unless left.type_args.length == right.type_args.length

            return left.type_args.zip(right.type_args).all? { |l_arg, r_arg| equivalent?(l_arg, r_arg) }
          end

          # Array types: compare element types recursively
          if left.is_a?(SemanticIR::ArrayType) && right.is_a?(SemanticIR::ArrayType)
            return equivalent?(left.element_type, right.element_type)
          end

          # Named types: compare by name
          left&.name == right&.name
        end
      end
    end
  end
end
