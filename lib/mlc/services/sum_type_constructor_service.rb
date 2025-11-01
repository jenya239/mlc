# frozen_string_literal: true

module MLC
  module Services
    # SumTypeConstructorService - Sum type constructor registration
    # Phase 17-G: Extracted from IRGen FunctionTransformer
    #
    # Responsibilities:
    # - Register constructors for sum type variants
    # - Generate constructor signatures with proper return types
    # - Handle generic type parameters in constructors
    #
    # Dependencies (injected):
    # - sum_type_constructors: Hash (storage for registered constructors)
    # - type_decl_table: Hash (type declarations lookup)
    # - type_checker: TypeChecker (for type parameter normalization)
    class SumTypeConstructorService
      def initialize(sum_type_constructors:, type_decl_table:, type_checker:)
        @sum_type_constructors = sum_type_constructors
        @type_decl_table = type_decl_table
        @type_checker = type_checker
      end

      # Register constructors for sum type variants
      # Creates FunctionInfo for each variant constructor
      #
      # @param sum_type_name [String] Name of the sum type
      # @param sum_type [HighIR::SumType] The sum type definition
      def register_sum_type_constructors(sum_type_name, sum_type)
        return unless sum_type.respond_to?(:variants)

        type_decl = @type_decl_table[sum_type_name]
        type_params = type_decl ? @type_checker.normalize_type_params(type_decl.type_params) : []

        # Build type variables for generic types
        type_param_vars = type_params.map do |tp|
          HighIR::Builder.type_variable(tp.name, constraint: tp.constraint)
        end

        # Constructor return type: either generic (if type params exist) or plain sum type
        generic_ret_type = if type_param_vars.any?
          HighIR::Builder.generic_type(sum_type, type_param_vars)
        else
          sum_type
        end

        # Register each variant as a constructor function
        sum_type.variants.each do |variant|
          field_types = (variant[:fields] || []).map { |field| field[:type] }
          @sum_type_constructors[variant[:name]] = MLC::IRGen::FunctionInfo.new(
            variant[:name],
            field_types,
            generic_ret_type,
            type_params
          )
        end
      end
    end
  end
end
