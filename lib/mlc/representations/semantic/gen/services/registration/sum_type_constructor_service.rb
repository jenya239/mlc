# frozen_string_literal: true

require_relative "../../../common/registry/function_signature"

module MLC
  module Services
    # SumTypeConstructorService - Sum type constructor registration
    # Phase 17-G: Extracted from the legacy FunctionTransformer
    #
    # Responsibilities:
    # - Register constructors for sum type variants
    # - Generate constructor signatures with proper return types
    # - Handle generic type parameters in constructors
    #
    # Dependencies (injected):
    # - sum_type_constructors: Hash (storage for registered constructors)
    # - function_registry: FunctionRegistry (for registering constructors as functions)
    # - type_decl_table: Hash (type declarations lookup)
    # - type_checker: TypeChecker (for type parameter normalization)
    class SumTypeConstructorService
      def initialize(sum_type_constructors:, function_registry:, type_decl_table:, type_checker:)
        @sum_type_constructors = sum_type_constructors
        @function_registry = function_registry
        @type_decl_table = type_decl_table
        @type_checker = type_checker
      end

      # Register constructors for sum type variants
      # Creates FunctionSignature for each variant constructor
      #
      # @param sum_type_name [String] Name of the sum type
      # @param sum_type [SemanticIR::SumType] The sum type definition
      def register_sum_type_constructors(sum_type_name, sum_type)
        return unless sum_type.respond_to?(:variants)

        type_decl = @type_decl_table[sum_type_name]
        type_params = type_decl ? @type_checker.normalize_type_params(type_decl.type_params) : []

        # Build type variables for generic types
        type_param_vars = type_params.map do |tp|
          SemanticIR::Builder.type_variable(tp.name, constraint: tp.constraint)
        end

        # Constructor return type: either generic (if type params exist) or plain sum type
        generic_ret_type = if type_param_vars.any?
          SemanticIR::Builder.generic_type(sum_type, type_param_vars)
        else
          sum_type
        end

        # Register each variant as a constructor function
        sum_type.variants.each do |variant|
          field_types = (variant[:fields] || []).map { |field| field[:type] }
          constructor_info = MLC::Core::FunctionSignature.new(
            variant[:name],
            field_types,
            generic_ret_type,
            type_params
          )

          # Register in both storages
          @sum_type_constructors[variant[:name]] = constructor_info
          @function_registry.register(variant[:name], constructor_info, {
            exported: true,
            external: true  # Constructors are treated as external/builtin
          })
        end
      end
    end
  end
end
