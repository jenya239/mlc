# frozen_string_literal: true

module MLC
  module Visitors
    # ExpressionVisitor
    # Visitor Pattern implementation for AST→IR transformation
    # Phase 23-A: Eliminates transformer.send() anti-pattern
    #
    # Responsibilities:
    # - Tree traversal for expression nodes (AST → IR)
    # - Recursive visitation of child nodes
    # - Delegates semantic processing to rules
    #
    # Architecture:
    # - Visitor handles TRAVERSAL (what to visit)
    # - Rules handle SEMANTICS (type inference, IR construction)
    # - No circular dependencies (visitor → rules, not rules → visitor)
    #
    # This is a SEPARATE CLASS (not a mixin) to allow:
    # - Better separation of concerns
    # - Easier testing in isolation
    # - Multi-pass compilation (different instances for different passes)
    # - Matches rustc/Swift architecture
    class ExpressionVisitor
      def initialize(rule_engine:, transformer:, services:, statement_visitor: nil)
        @rule_engine = rule_engine
        @transformer = transformer  # IRGen instance for with_current_node only
        @statement_visitor = statement_visitor  # Set later after both visitors created

        # Unpack services
        @type_registry = services[:type_registry]
        @function_registry = services[:function_registry]
        @type_checker_service = services[:type_checker]
        @type_inference_service = services[:type_inference]
        @record_builder_service = services[:record_builder]
        @scope_context_service = services[:scope_context]
        @var_type_registry = services[:var_type_registry]
      end

      # Setter to break circular dependency (called after StatementVisitor created)
      attr_writer :statement_visitor

      # Main entry point: Visit AST expression node and return IR expression
      # Dispatches to specific visit_* method based on node type
      def visit(expr)
        @transformer.send(:with_current_node, expr) do
          # Special case: AST::Block is a statement block but can appear in expression context
          if expr.is_a?(AST::Block)
            return visit_block(expr, require_value: true)
          end

          # Dispatch to specific visitor method
          case expr
          when AST::IntLit, AST::FloatLit, AST::StringLit, AST::RegexLit, AST::UnitLit
            visit_literal(expr)
          when AST::VarRef
            visit_var_ref(expr)
          when AST::MemberAccess
            visit_member(expr)
          when AST::Call
            visit_call(expr)
          when AST::UnaryOp
            visit_unary(expr)
          when AST::BinaryOp
            visit_binary(expr)
          when AST::Let
            visit_let(expr)
          when AST::RecordLit
            visit_record_literal(expr)
          when AST::IfExpr
            visit_if(expr)
          when AST::ArrayLiteral
            visit_array_literal(expr)
          when AST::DoExpr
            visit_do_block(expr)
          when AST::BlockExpr
            visit_block_expr(expr)
          when AST::MatchExpr
            visit_match(expr)
          when AST::Lambda
            visit_lambda(expr)
          when AST::IndexAccess
            visit_index_access(expr)
          when AST::ForLoop
            visit_for_loop(expr)
          when AST::WhileLoop
            visit_while_loop(expr)
          when AST::ListComprehension
            visit_list_comprehension(expr)
          else
            # No visitor method for this node type - compiler bug
            raise CompileError, "No visitor method for #{expr.class}. " \
                                "This indicates a missing visitor method in ExpressionVisitor."
          end
        end
      end

      private

      # Build context hash for expression rules
      # Contains all services needed by rules
      # Phase 23-A: Includes transformer for backward compatibility during migration
      # Phase 25-B: Includes visitors for direct access (eliminates transformer.send())
      def expression_rule_context
        {
          transformer: @transformer,  # Phase 23-A: Backward compatibility for transformer.send()
          expression_visitor: self,   # Phase 25-B: Direct visitor access
          statement_visitor: @statement_visitor,  # Phase 25-B: Direct visitor access
          type_registry: @type_registry,
          function_registry: @function_registry,
          rule_engine: @rule_engine,
          type_checker: @type_checker_service,
          type_inference: @type_inference_service,
          record_builder: @record_builder_service,
          scope_context: @scope_context_service
        }
      end

      # Visit literal expression (int, float, string, regex, unit)
      # No recursion needed - literals have no children
      def visit_literal(node)
        @rule_engine.apply(:core_ir_expression, node, context: expression_rule_context)
      end

      # Visit variable reference
      # No recursion needed - var refs have no children
      def visit_var_ref(node)
        @rule_engine.apply(:core_ir_expression, node, context: expression_rule_context)
      end

      # Visit member access: object.member
      # Let the rule handle object transformation (module member check first)
      # Phase 23-D: Don't pre-visit object - MemberRule checks for module members
      def visit_member(node)
        # MemberRule checks if this is module member access (e.g., Math.sqrt)
        # and only transforms object if it's NOT a module member
        @rule_engine.apply(:core_ir_expression, node, context: expression_rule_context)
      end

      # Visit function call: f(args...)
      # Recursively visit callee and arguments, then apply rule
      def visit_call(node)
        callee_ir = visit(node.callee)
        args_ir = node.args.map { |arg| visit(arg) }

        context = expression_rule_context.merge(
          callee_ir: callee_ir,
          args_ir: args_ir
        )
        @rule_engine.apply(:core_ir_expression, node, context: context)
      end

      # Visit unary operation: -x, !x
      # Recursively visit operand, then apply rule
      def visit_unary(node)
        operand_ir = visit(node.operand)

        context = expression_rule_context.merge(
          operand_ir: operand_ir
        )
        @rule_engine.apply(:core_ir_expression, node, context: context)
      end

      # Visit binary operation: x + y, x == y
      # Recursively visit left and right operands, then apply rule
      # Special case: pipe operator handled separately
      def visit_binary(node)
        # Pipe operator has special desugaring logic
        if node.op == "|>"
          return visit_pipe(node)
        end

        left_ir = visit(node.left)
        right_ir = visit(node.right)

        context = expression_rule_context.merge(
          left_ir: left_ir,
          right_ir: right_ir
        )
        @rule_engine.apply(:core_ir_expression, node, context: context)
      end

      # Visit pipe operator: x |> f
      # Desugars pipe into function call with left as first argument
      # Phase 23-B: Direct desugaring in visitor (AST-level transformation)
      def visit_pipe(node)
        left_ir = visit(node.left)

        # Desugar based on right-hand side structure
        if node.right.is_a?(AST::Call)
          # right is f(args) => desugar to f(left, args)
          callee_ir = visit(node.right.callee)
          args_ir = node.right.args.map { |arg| visit(arg) }

          # Insert left as first argument
          all_args_ir = [left_ir] + args_ir

          # Infer return type
          type = @type_inference_service.infer_call_type(callee_ir, all_args_ir)

          HighIR::Builder.call(callee_ir, all_args_ir, type)
        else
          # right is just a function name => f(left)
          callee_ir = visit(node.right)
          type = @type_inference_service.infer_call_type(callee_ir, [left_ir])

          HighIR::Builder.call(callee_ir, [left_ir], type)
        end
      end

      # Visit let expression: let x = value in body
      # Recursively visit value and body with scope management
      def visit_let(node)
        value_ir = visit(node.value)

        # Save scope before entering let body
        saved_var_types = @var_type_registry.snapshot

        # Type the binding
        @var_type_registry.set(node.name, value_ir.type)

        # Visit body in new scope
        body_ir = visit(node.body)

        # Restore scope
        @var_type_registry.restore(saved_var_types)

        context = expression_rule_context.merge(
          value_ir: value_ir,
          body_ir: body_ir
        )
        @rule_engine.apply(:core_ir_expression, node, context: context)
      end

      # Visit record literal: {x: 1, y: 2}
      # Recursively visit field values, then apply rule
      def visit_record_literal(node)
        # Transform fields hash: keys → string keys, values → IR values
        # node.fields is a Hash of {field_name => value_expr}
        fields_ir = node.fields.each_with_object({}) do |(name, value), hash|
          hash[name.to_s] = visit(value)
        end

        context = expression_rule_context.merge(
          fields_ir: fields_ir
        )
        @rule_engine.apply(:core_ir_expression, node, context: context)
      end

      # Visit if expression: if cond then branch else branch
      # Recursively visit condition and branches, then apply rule
      def visit_if(node)
        cond_ir = visit(node.condition)
        then_ir = visit(node.then_branch)
        else_ir = node.else_branch ? visit(node.else_branch) : nil

        context = expression_rule_context.merge(
          cond_ir: cond_ir,
          then_ir: then_ir,
          else_ir: else_ir
        )
        @rule_engine.apply(:core_ir_expression, node, context: context)
      end

      # Visit array literal: [1, 2, 3]
      # Recursively visit elements, then apply rule
      def visit_array_literal(node)
        elements_ir = node.elements.map { |elem| visit(elem) }

        context = expression_rule_context.merge(
          elements_ir: elements_ir
        )
        @rule_engine.apply(:core_ir_expression, node, context: context)
      end

      # Visit do block: do { statements... }
      # Recursively visit statements and result expression
      def visit_do_block(node)
        # Use statement visitor for statement sequence
        statements_ir = visit_statements(node.statements)
        result_ir = node.result_expr ? visit(node.result_expr) : nil

        context = expression_rule_context.merge(
          statements_ir: statements_ir,
          result_ir: result_ir
        )
        @rule_engine.apply(:core_ir_expression, node, context: context)
      end

      # Visit block expression: { statements...; result_expr }
      # Scope management: save/restore variable types
      def visit_block_expr(node)
        saved_var_types = @var_type_registry.snapshot

        statements_ir = visit_statements(node.statements)
        result_ir = visit(node.result_expr)
        block_type = result_ir&.type || HighIR::Builder.unit_type

        HighIR::Builder.block_expr(statements_ir, result_ir, block_type)
      ensure
        @var_type_registry.restore(saved_var_types)
      end

      # Visit match expression: match scrutinee { patterns... }
      # Recursively visit scrutinee, then apply rule
      # Pattern transformation delegated to rule
      def visit_match(node)
        scrutinee_ir = visit(node.scrutinee)

        context = expression_rule_context.merge(
          scrutinee_ir: scrutinee_ir
        )
        @rule_engine.apply(:core_ir_expression, node, context: context)
      end

      # Visit lambda expression: |params| body
      # Recursively visit body with lambda context
      def visit_lambda(node)
        # Lambda parameter types and return type may be inferred from context
        # Rule handles this logic
        context = expression_rule_context
        @rule_engine.apply(:core_ir_expression, node, context: context)
      end

      # Visit index access: array[index]
      # Recursively visit object and index expressions
      def visit_index_access(node)
        object_ir = visit(node.object)
        index_ir = visit(node.index)

        context = expression_rule_context.merge(
          object_ir: object_ir,
          index_ir: index_ir
        )
        @rule_engine.apply(:core_ir_expression, node, context: context)
      end

      # Visit for loop expression: for x in iter { body }
      # Recursively visit iterator and body
      def visit_for_loop(node)
        iter_ir = visit(node.iterable)

        context = expression_rule_context.merge(
          iter_ir: iter_ir
        )
        @rule_engine.apply(:core_ir_expression, node, context: context)
      end

      # Visit while loop expression: while cond { body }
      # Recursively visit condition and body
      def visit_while_loop(node)
        cond_ir = visit(node.condition)

        context = expression_rule_context.merge(
          cond_ir: cond_ir
        )
        @rule_engine.apply(:core_ir_expression, node, context: context)
      end

      # Visit list comprehension: [expr | x <- iter, cond]
      # Rule handles ALL transformations with proper scoping
      # Visitor does NOT pre-visit filters/output (they need generator vars in scope)
      def visit_list_comprehension(node)
        # Rule transforms everything with correct variable scoping
        # No pre-visit needed - rule is self-contained
        context = expression_rule_context
        @rule_engine.apply(:core_ir_expression, node, context: context)
      end

      # Visit statement block with scope management
      # Used by block expressions and do blocks
      # Phase 25-B: Delegates to StatementVisitor directly
      def visit_block(block_node, require_value: false)
        @transformer.send(:transform_block, block_node, require_value: require_value)
      end

      # Visit statement sequence
      # Phase 25-B: Delegates to StatementVisitor directly
      def visit_statements(statements)
        @statement_visitor.visit_statements(statements)
      end
    end
  end
end
