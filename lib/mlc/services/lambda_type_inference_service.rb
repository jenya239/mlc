# frozen_string_literal: true

module MLC
  module Services
    # LambdaTypeInferenceService - Type inference for lambda expressions
    # Phase 18-C: Extracted from IRGen
    #
    # Responsibilities:
    # - Infer lambda parameter types from higher-order function context (map, filter, fold)
    # - Extract return types from function types
    #
    # Dependencies: None (pure type inspection)
    class LambdaTypeInferenceService
      # Infer expected parameter types for a lambda based on HOF context
      # Used when type-checking lambdas passed to higher-order functions
      #
      # @param object_ir [HighIR::Expression] The object the method is called on
      # @param member_name [String] The method name (e.g., "map", "filter", "fold")
      # @param transformed_args [Array<HighIR::Expression>] Transformed arguments
      # @param index [Integer] Index of the lambda argument
      # @return [Array<HighIR::Type>] Expected parameter types for the lambda
      #
      # Examples:
      #   [1, 2, 3].map(fn(x) x + 1)     # x has type inferred from array element type
      #   [1, 2, 3].filter(fn(x) x > 0)  # x has type inferred from array element type
      #   [1, 2, 3].fold(0, fn(acc, x) acc + x)  # acc and x inferred from types
      def expected_lambda_param_types(object_ir, member_name, transformed_args, index)
        return [] unless object_ir && member_name

        object_type = object_ir.type
        return [] unless object_type

        case member_name
        when "map"
          if index.zero? && object_type.is_a?(HighIR::ArrayType)
            [object_type.element_type]
          else
            []
          end
        when "filter"
          if index.zero? && object_type.is_a?(HighIR::ArrayType)
            [object_type.element_type]
          else
            []
          end
        when "fold"
          if index == 1 && object_type.is_a?(HighIR::ArrayType)
            accumulator_type = transformed_args.first&.type
            element_type = object_type.element_type
            accumulator_type ? [accumulator_type, element_type] : []
          else
            []
          end
        else
          []
        end
      end

      # Extract return type from a function type
      # Handles both explicit function types and inferred types
      #
      # @param arg [HighIR::Expression] The argument with a function type
      # @return [HighIR::Type, nil] The return type, or nil if not a function
      def lambda_return_type(arg)
        return nil unless arg

        if arg.respond_to?(:function_type) && arg.function_type
          arg.function_type.ret_type
        elsif arg.respond_to?(:type) && arg.type.is_a?(HighIR::FunctionType)
          arg.type.ret_type
        else
          nil
        end
      end
    end
  end
end
