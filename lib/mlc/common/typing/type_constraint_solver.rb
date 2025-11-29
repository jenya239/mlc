# frozen_string_literal: true

module MLC
  module Common
    module Typing
      class TypeConstraintSolver
      Instantiation = Struct.new(:type_map, :param_types, :ret_type)

      def initialize(infer_type_arguments:, substitute_type:, ensure_compatible_type:, type_error:, assign_expression_type: nil)
        @infer_type_arguments = infer_type_arguments
        @substitute_type = substitute_type
        @ensure_compatible_type = ensure_compatible_type
        @type_error = type_error
        @assign_expression_type = assign_expression_type || ->(_expr, _type) {}
      end

      # Solve type constraints for a generic function call
      # @param expected_ret_type [SemanticIR::Type, nil] Expected return type for bidirectional inference
      def solve(info, args, name:, expected_ret_type: nil)
        type_params = info.type_params || []
        arg_types = args.map(&:type)
        type_map = @infer_type_arguments.call(type_params, info.param_types, arg_types)

        # Bidirectional type inference: if type_map is incomplete and we have expected return type,
        # try to infer remaining type variables from expected_ret_type
        if expected_ret_type && incomplete_type_map?(type_map, type_params)
          unify_with_expected(info.ret_type, expected_ret_type, type_map)
        end

        param_types = info.param_types.map { |pt| @substitute_type.call(pt, type_map) }

        if param_types.length != args.length
          @type_error.call("Function '#{name}' expects #{param_types.length} argument(s), got #{args.length}")
        end

        param_types.each_with_index do |type, index|
          @ensure_compatible_type.call(
            args[index].type,
            type,
            "argument #{index + 1} of '#{name}'"
          )
          @assign_expression_type.call(args[index], type)
        end

        ret_type = @substitute_type.call(info.ret_type, type_map)

        Instantiation.new(type_map, param_types, ret_type)
      end

      private

      # Check if type_map is missing any type parameters
      def incomplete_type_map?(type_map, type_params)
        type_params.any? { |tp| !type_map.key?(tp.name) || type_map[tp.name].is_a?(MLC::SemanticIR::TypeVariable) }
      end

      # Unify function return type with expected type to infer missing type variables
      def unify_with_expected(pattern_type, expected_type, type_map)
        case pattern_type
        when MLC::SemanticIR::TypeVariable
          # Type variable in return type - bind to expected type
          type_map[pattern_type.name] = expected_type unless type_map.key?(pattern_type.name)
        when MLC::SemanticIR::GenericType
          # Generic type - recursively unify type arguments
          if expected_type.is_a?(MLC::SemanticIR::GenericType)
            pattern_type.type_args&.each_with_index do |pattern_arg, index|
              expected_arg = expected_type.type_args&.[](index)
              unify_with_expected(pattern_arg, expected_arg, type_map) if expected_arg
            end
          end
        when MLC::SemanticIR::ArrayType
          if expected_type.is_a?(MLC::SemanticIR::ArrayType)
            unify_with_expected(pattern_type.element_type, expected_type.element_type, type_map)
          end
        end
      end
      end
    end
  end
end
