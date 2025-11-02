# frozen_string_literal: true

module MLC
  class IRGen
    # ExpressionTransformer
    # Expression transformation orchestration and helper methods
    # Phase 19-B: Extracted from IRGen
    #
    # Responsibilities:
    # - Main expression transformation entry point
    # - Apply expression transformation rules with context
    # - Transform block expressions with scope management
    # - Module member function lookup
    # - Pipe operator desugaring
    # - Module import alias resolution
    #
    # Dependencies (from IRGen):
    # - @rule_engine: For applying expression rules
    # - @type_registry: For type lookups
    # - @function_registry: For function lookups
    # - @type_checker_service: For type checking
    # - @type_inference_service: For type inference (including infer_call_type)
    # - @record_builder_service: For record construction
    # - @generic_call_resolver_service: For generic call resolution
    # - @scope_context_service: For scope context
    # - @var_type_registry: For variable type tracking
    # - @current_import_aliases: For import alias resolution
    # - with_current_node: From TransformationContext
    # - transform_block: From StatementTransformer
    # - transform_statements: From StatementTransformer
    module ExpressionTransformer
      # Transform AST expression to HighIR expression
      # Main entry point for expression transformation
      # Phase 23-A: Delegates to separate ExpressionVisitor class
      def transform_expression(expr)
        # Delegate to ExpressionVisitor instance (Phase 23-A)
        # Visitor handles tree traversal, rules handle semantic processing
        @expression_visitor.visit(expr)
      end

      # Apply expression transformation rules with full context
      # Builds context hash with all necessary dependencies for rules
      def apply_expression_rules(expr)
        context = {
          transformer: self,
          type_registry: @type_registry,
          function_registry: @function_registry,
          rule_engine: @rule_engine,
          # expression_transformer removed - rules call transformer directly
          # predicates removed - rules call transformer directly
          # context_manager removed - state management, not a service
          type_checker: @type_checker_service,
          type_inference: @type_inference_service,
          record_builder: @record_builder_service,
          scope_context: @scope_context_service
        }
        @rule_engine.apply(:core_ir_expression, expr, context: context)
      end

      # Transform block expression with scope management
      # Block expressions reuse the statement pipeline so that we handle the full
      # range of statements (return, break, nested blocks, etc.) consistently.
      # We preserve scope during transformation to allow the trailing expression
      # to see variables defined in the statement portion, and restore the outer
      # scope afterwards to avoid leaking bindings.
      def transform_block_expr(block_expr)
        saved_var_types = @var_type_registry.snapshot

        statements_ir = transform_statements(block_expr.statements)
        result_ir = transform_expression(block_expr.result_expr)
        block_type = result_ir&.type || HighIR::Builder.unit_type

        HighIR::Builder.block_expr(statements_ir, result_ir, block_type)
      ensure
        @var_type_registry.restore(saved_var_types)
      end

      # Lookup module member function entry
      # Resolves import aliases and fetches function entry from registry
      def module_member_function_entry(object_node, member_name)
        return unless object_node.is_a?(AST::VarRef)

        # Resolve import alias and lookup member in FunctionRegistry
        resolved = resolve_module_alias(object_node.name)
        @function_registry.fetch_entry_for_member(resolved, member_name)
      end

      # Transform pipe operator: left |> right
      # Desugars pipe into function call with left as first argument
      # Phase 19-B: Fixed infer_call_type delegation to @type_inference_service
      def transform_pipe(pipe_expr)
        # Desugar pipe operator: left |> right
        # If right is a function call, insert left as first argument
        # Otherwise, treat right as function name and create call with left

        left = transform_expression(pipe_expr.left)

        if pipe_expr.right.is_a?(AST::Call)
          # right is f(args) => transform to f(left, args)
          callee = transform_expression(pipe_expr.right.callee)
          args = pipe_expr.right.args.map { |arg| transform_expression(arg) }

          # Insert left as first argument
          all_args = [left] + args

          # Infer return type - Phase 19-B: Fixed delegation to service
          type = @type_inference_service.infer_call_type(callee, all_args)

          HighIR::Builder.call(callee, all_args, type)
        else
          # right is just a function name => f(left)
          callee = transform_expression(pipe_expr.right)
          # Phase 19-B: Fixed delegation to service
          type = @type_inference_service.infer_call_type(callee, [left])

          HighIR::Builder.call(callee, [left], type)
        end
      end

      # Resolve module import alias
      # Looks up alias in current import context, returns original identifier if not found
      def resolve_module_alias(identifier)
        return identifier unless identifier.is_a?(String) && !identifier.empty?
        (@current_import_aliases && @current_import_aliases[identifier]) || identifier
      end
    end
  end
end
