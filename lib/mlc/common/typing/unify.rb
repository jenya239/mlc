# frozen_string_literal: true

module MLC
  module Common
    module Typing
      # Pure functions for type unification
      # No side effects, no dependencies, no state
      module Unify
        # Result of unification attempt
        # @attr success [Boolean] whether unification succeeded
        # @attr substitutions [Hash<String, SemanticIR::Type>] accumulated type variable bindings
        UnifyResult = Struct.new(:success, :substitutions, keyword_init: true) do
          def success?
            success
          end

          def failure?
            !success
          end
        end

        module_function

        # Create a successful unification result
        def success(substitutions = {})
          UnifyResult.new(success: true, substitutions: substitutions)
        end

        # Create a failed unification result
        def failure
          UnifyResult.new(success: false, substitutions: {})
        end

        # Unify a type pattern with an actual type
        # Returns UnifyResult with success status and discovered substitutions
        #
        # @param pattern [SemanticIR::Type] The pattern type (may contain type variables)
        # @param actual [SemanticIR::Type] The actual type to unify against
        # @param existing [Hash] Existing substitutions to respect
        # @return [UnifyResult] Result containing success status and new substitutions
        def unify(pattern, actual, existing = {})
          return failure unless pattern && actual

          case pattern
          when SemanticIR::TypeVariable
            unify_type_variable(pattern, actual, existing)

          when SemanticIR::GenericType
            unify_generic_type(pattern, actual, existing)

          when SemanticIR::ArrayType
            unify_array_type(pattern, actual, existing)

          else
            # For other types, check equivalence
            if Equivalence.equivalent?(pattern, actual)
              success(existing)
            else
              failure
            end
          end
        end

        # Unify type variable with actual type
        def unify_type_variable(type_var, actual, existing)
          bound = existing[type_var.name]
          if bound
            # Type variable already bound - check consistency
            if Equivalence.equivalent?(bound, actual)
              success(existing)
            else
              failure
            end
          else
            # Bind type variable to actual type
            success(existing.merge(type_var.name => actual))
          end
        end

        # Unify generic types
        def unify_generic_type(pattern, actual, existing)
          return failure unless actual.is_a?(SemanticIR::GenericType)

          # Check base types match
          pattern_base = pattern.base_type&.name
          actual_base = actual.base_type&.name
          return failure unless pattern_base == actual_base

          # Check arity matches
          return failure unless pattern.type_args.length == actual.type_args.length

          # Unify each type argument, threading substitutions through
          current_subst = existing
          pattern.type_args.zip(actual.type_args).each do |pattern_arg, actual_arg|
            result = unify(pattern_arg, actual_arg, current_subst)
            return failure if result.failure?
            current_subst = result.substitutions
          end

          success(current_subst)
        end

        # Unify array types
        def unify_array_type(pattern, actual, existing)
          return failure unless actual.is_a?(SemanticIR::ArrayType)

          unify(pattern.element_type, actual.element_type, existing)
        end

        private_class_method :unify_type_variable, :unify_generic_type, :unify_array_type
      end
    end
  end
end
