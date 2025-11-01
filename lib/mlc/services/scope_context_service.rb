# frozen_string_literal: true

module MLC
  module Services
    # ScopeContextService manages scoped state during AST → HighIR transformation
    # Tracks type parameters, function return types, and lambda parameter types
    # using stacks to support nested scopes (generic functions, lambdas, etc.)
    class ScopeContextService
      attr_reader :type_param_stack, :lambda_param_stack, :function_return_stack

      def initialize
        @type_param_stack = []
        @lambda_param_stack = []
        @function_return_stack = []
      end

      # Type parameters (e.g., <T> in fn foo<T>())
      def with_type_params(params)
        @type_param_stack.push(params || [])
        yield
      ensure
        @type_param_stack.pop
      end

      def current_type_params
        @type_param_stack.last || []
      end

      # Function return types (for validating return statements)
      def with_function_return(type)
        @function_return_stack.push(type)
        yield
      ensure
        @function_return_stack.pop
      end

      def current_function_return
        @function_return_stack.last
      end

      # Lambda parameter types (for type inference in lambdas)
      def with_lambda_param_types(types)
        @lambda_param_stack.push(Array(types))
        yield
      ensure
        @lambda_param_stack.pop
      end

      def current_lambda_param_types
        @lambda_param_stack.last || []
      end
    end
  end
end
