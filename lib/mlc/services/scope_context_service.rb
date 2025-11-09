# frozen_string_literal: true

module MLC
  module Services
    # TransformationContext manages all scoped state during AST → SemanticIR transformation
    # Phase 18-A: Renamed from ScopeContextService, added current_node and var_type_registry
    #
    # Responsibilities:
    # - Track type parameters (for generic functions)
    # - Track function return types (for validating return statements)
    # - Track lambda parameter types (for type inference in lambdas)
    # - Track current AST node (for error reporting)
    # - Provide access to variable type registry
    #
    # Dependencies:
    # - var_type_registry: VarTypeRegistry (injected, for tracking variable types)
    class TransformationContext
      attr_reader :type_param_stack, :lambda_param_stack, :function_return_stack
      attr_reader :var_type_registry, :loop_depth
      attr_accessor :current_node

      def initialize(var_type_registry:)
        @type_param_stack = []
        @lambda_param_stack = []
        @function_return_stack = []
        @var_type_registry = var_type_registry
        @current_node = nil
        @loop_depth = 0
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

      # Current AST node (for error reporting with source location)
      def with_current_node(node)
        previous = @current_node
        @current_node = node if node
        yield
      ensure
        @current_node = previous
      end

      def with_loop_scope
        @loop_depth += 1
        yield
      ensure
        @loop_depth -= 1
      end

      def inside_loop?
        @loop_depth.positive?
      end
    end

    # Backward compatibility alias
    ScopeContextService = TransformationContext
  end
end
