# frozen_string_literal: true

require_relative "../../../../../registries/function_signature"

module MLC
  module Services
    # TypeUnificationService - Type unification and substitution for generic type inference
    # Phase 18-B: Extracted during the legacy IR refactor
    #
    # Responsibilities:
    # - Unify type patterns with actual types (for generic type inference)
    # - Check type equivalence (structural equality)
    # - Apply type variable substitutions recursively
    # - Compute generic type substitutions from scrutinee types
    # - Resolve constructor info with generic substitutions
    #
    # Dependencies:
    # - type_checker: TypeChecker (for type_name helper)
    # - sum_type_constructors: Hash (for constructor lookup)
    class TypeUnificationService
      def initialize(type_checker:, sum_type_constructors:)
        @type_checker = type_checker
        @sum_type_constructors = sum_type_constructors
      end

      # Unify a type pattern with an actual type, collecting type variable substitutions
      # Returns true if unification succeeds, false otherwise
      # Mutates type_map to record discovered substitutions
      #
      # @param pattern [SemanticIR::Type] The pattern type (may contain type variables)
      # @param actual [SemanticIR::Type] The actual type to unify against
      # @param type_map [Hash] Mutable map from type variable names to concrete types
      # @param context [Hash] Context for error reporting (currently unused)
      # @return [Boolean] Whether unification succeeded
      def unify_type(pattern, actual, type_map, context:)
        return false unless pattern && actual

        case pattern
        when SemanticIR::TypeVariable
          existing = type_map[pattern.name]
          if existing
            type_equivalent?(existing, actual)
          else
            type_map[pattern.name] = actual
            true
          end
        when SemanticIR::GenericType
          return false unless actual.is_a?(SemanticIR::GenericType)
          pattern_base = type_name(pattern.base_type)
          actual_base = type_name(actual.base_type)
          return false unless pattern_base == actual_base

          pattern.type_args.zip(actual.type_args).all? do |pattern_arg, actual_arg|
            unify_type(pattern_arg, actual_arg, type_map, context: context)
          end
        when SemanticIR::ArrayType
          return false unless actual.is_a?(SemanticIR::ArrayType)
          unify_type(pattern.element_type, actual.element_type, type_map, context: context)
        else
          type_equivalent?(pattern, actual)
        end
      end

      # Check structural type equivalence (deep equality check)
      # Handles type variables, generic types, array types, and named types
      #
      # @param left [SemanticIR::Type] First type
      # @param right [SemanticIR::Type] Second type
      # @return [Boolean] Whether the types are equivalent
      def type_equivalent?(left, right)
        return false if left.nil? || right.nil?
        return true if left.equal?(right)

        if left.is_a?(SemanticIR::TypeVariable) && right.is_a?(SemanticIR::TypeVariable)
          return left.name == right.name
        end

        if left.is_a?(SemanticIR::GenericType) && right.is_a?(SemanticIR::GenericType)
          left_base = type_name(left.base_type)
          right_base = type_name(right.base_type)
          return false unless left_base == right_base
          return false unless left.type_args.length == right.type_args.length

          return left.type_args.zip(right.type_args).all? { |l_arg, r_arg| type_equivalent?(l_arg, r_arg) }
        end

        if left.is_a?(SemanticIR::ArrayType) && right.is_a?(SemanticIR::ArrayType)
          return type_equivalent?(left.element_type, right.element_type)
        end

        type_name(left) == type_name(right)
      end

      # Apply type variable substitutions recursively to a type
      # Replaces type variables with their concrete types from the substitutions map
      #
      # @param type [SemanticIR::Type] The type to apply substitutions to
      # @param substitutions [Hash<String, SemanticIR::Type>] Map from type variable names to concrete types
      # @return [SemanticIR::Type] The type with substitutions applied
      def apply_type_substitutions(type, substitutions)
        case type
        when SemanticIR::TypeVariable
          substitutions[type.name] || type
        when SemanticIR::GenericType
          base = apply_type_substitutions(type.base_type, substitutions)
          args = type.type_args.map { |arg| apply_type_substitutions(arg, substitutions) }
          SemanticIR::Builder.generic_type(base, args)
        when SemanticIR::ArrayType
          SemanticIR::Builder.array_type(apply_type_substitutions(type.element_type, substitutions))
        when SemanticIR::FunctionType
          params = type.params.map { |param| {name: param[:name], type: apply_type_substitutions(param[:type], substitutions)} }
          ret_type = apply_type_substitutions(type.ret_type, substitutions)
          SemanticIR::Builder.function_type(params, ret_type)
        when SemanticIR::RecordType
          fields = type.fields.map { |field| {name: field[:name], type: apply_type_substitutions(field[:type], substitutions)} }
          SemanticIR::Builder.record_type(type.name, fields)
        when SemanticIR::SumType
          variants = type.variants.map do |variant|
            fields = Array(variant[:fields]).map do |field|
              {name: field[:name], type: apply_type_substitutions(field[:type], substitutions)}
            end
            {name: variant[:name], fields: fields}
          end
          SemanticIR::Builder.sum_type(type.name, variants)
        else
          type
        end
      end

      # Compute generic type variable substitutions from scrutinee type
      # Used in pattern matching to specialize generic constructor types
      #
      # @param info [FunctionSignature] The constructor function info with type parameters
      # @param scrutinee_type [SemanticIR::Type] The concrete type being matched on
      # @return [Hash<String, SemanticIR::Type>] Map from type parameter names to concrete types
      def generic_substitutions(info, scrutinee_type)
        return {} unless info.type_params&.any?
        return {} unless scrutinee_type.is_a?(SemanticIR::GenericType)

        base_match = @type_checker.type_name(info.ret_type) == @type_checker.type_name(scrutinee_type.base_type)
        return {} unless base_match

        substitutions = {}
        info.type_params.each_with_index do |param, idx|
          actual_arg = scrutinee_type.type_args[idx]
          substitutions[param.name] = actual_arg if actual_arg
        end
        substitutions
      end

      # Get constructor info with generic type substitutions applied
      # Returns specialized constructor info for a concrete scrutinee type
      #
      # @param name [String] The constructor name
      # @param scrutinee_type [SemanticIR::Type] The concrete type being matched on
      # @return [FunctionSignature, nil] Specialized constructor info, or nil if not found
      def constructor_info_for(name, scrutinee_type)
        info = @sum_type_constructors[name]
        return unless info

        substitutions = generic_substitutions(info, scrutinee_type)
        return info if substitutions.empty?

        substituted_params = info.param_types.map do |type|
          apply_type_substitutions(type, substitutions)
        end
        substituted_ret_type = apply_type_substitutions(info.ret_type, substitutions)

        MLC::Registries::FunctionSignature.new(info.name, substituted_params, substituted_ret_type, info.type_params)
      end

      private

      # Helper to get type name (delegates to type_checker)
      def type_name(type)
        @type_checker.type_name(type)
      end
    end
  end
end
