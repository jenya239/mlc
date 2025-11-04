# frozen_string_literal: true

module MLC
  module Visitors
    # StatementVisitor
    # Visitor Pattern implementation for AST→IR statement transformation
    # Phase 23-C: Completes visitor pattern migration
    #
    # Responsibilities:
    # - Tree traversal for statement nodes (AST → IR)
    # - Recursive visitation of child nodes
    # - Delegates semantic processing to rules
    #
    # Architecture:
    # - Visitor handles TRAVERSAL (what to visit)
    # - Rules handle SEMANTICS (type inference, IR construction)
    # - No circular dependencies (visitor → rules, not rules → visitor)
    class StatementVisitor
      def initialize(rule_engine:, transformer:, expression_visitor:, services:)
        @rule_engine = rule_engine
        @transformer = transformer  # IRGen instance for backward compatibility
        @expression_visitor = expression_visitor  # For expression transformation

        # Unpack services
        @type_registry = services[:type_registry]
        @function_registry = services[:function_registry]
        @type_checker_service = services[:type_checker]
        @type_inference_service = services[:type_inference]
        @scope_context_service = services[:scope_context]
        @var_type_registry = services[:var_type_registry]
      end

      # Main entry point: Visit AST statement node and return IR statement
      # Dispatches to specific visit_* method based on node type
      def visit(stmt)
        @transformer.send(:with_current_node, stmt) do
          # Dispatch to specific visitor method
          case stmt
          when AST::VariableDecl
            visit_variable_decl(stmt)
          when AST::Assignment
            visit_assignment(stmt)
          when AST::Return
            visit_return(stmt)
          when AST::Break
            visit_break(stmt)
          when AST::Continue
            visit_continue(stmt)
          when AST::ExprStmt
            visit_expr_stmt(stmt)
          when AST::WhileLoop
            visit_while_statement(stmt)
          when AST::ForLoop
            visit_for_statement(stmt)
          when AST::IfStmt
            visit_if_statement(stmt)
          else
            # No visitor method for this node type - compiler bug
            raise CompileError, "No visitor method for statement #{stmt.class}. " \
                                "This indicates a missing visitor method in StatementVisitor."
          end
        end
      end

      # Visit sequence of statements
      # Returns array of IR statements
      # Rules can return arrays, so we need to flatten
      def visit_statements(statements)
        statements.each_with_object([]) do |stmt, acc|
          result = visit(stmt)
          acc.concat(Array(result))
        end
      end

      private

      # Build context hash for statement rules
      # Contains all services needed by rules
      # Phase 25-B: Includes visitors for direct access (eliminates transformer.send())
      def statement_rule_context
        {
          transformer: @transformer,  # Backward compatibility
          expression_visitor: @expression_visitor,  # Phase 25-B: Direct visitor access
          statement_visitor: self,  # Phase 25-B: Direct visitor access
          type_registry: @type_registry,
          function_registry: @function_registry,
          rule_engine: @rule_engine,
          type_checker: @type_checker_service,
          type_inference: @type_inference_service,
          scope_context: @scope_context_service,
          var_type_registry: @var_type_registry
        }
      end

      # Visit variable declaration: let x = value
      # Recursively visit value expression, then apply rule
      def visit_variable_decl(node)
        value_ir = @expression_visitor.visit(node.value)

        context = statement_rule_context.merge(
          value_ir: value_ir
        )
        @rule_engine.apply(:core_ir_statement, node, context: context)
      end

      # Visit assignment: x = value
      # Recursively visit value expression, then apply rule
      def visit_assignment(node)
        value_ir = @expression_visitor.visit(node.value)

        context = statement_rule_context.merge(
          value_ir: value_ir
        )
        @rule_engine.apply(:core_ir_statement, node, context: context)
      end

      # Visit return statement: return expr
      # Recursively visit expression if present, then apply rule
      def visit_return(node)
        value_ir = node.expr ? @expression_visitor.visit(node.expr) : nil

        context = statement_rule_context.merge(
          value_ir: value_ir
        )
        @rule_engine.apply(:core_ir_statement, node, context: context)
      end

      # Visit break statement
      # No child nodes to visit
      def visit_break(node)
        @rule_engine.apply(:core_ir_statement, node, context: statement_rule_context)
      end

      # Visit continue statement
      # No child nodes to visit
      def visit_continue(node)
        @rule_engine.apply(:core_ir_statement, node, context: statement_rule_context)
      end

      # Visit expression statement: expr;
      # Recursively visit expression, then apply rule
      def visit_expr_stmt(node)
        expr_ir = @expression_visitor.visit(node.expr)

        context = statement_rule_context.merge(
          expr_ir: expr_ir
        )
        @rule_engine.apply(:core_ir_statement, node, context: context)
      end

      # Visit while loop statement: while cond { body }
      # Recursively visit condition and body statements
      def visit_while_statement(node)
        cond_ir = @expression_visitor.visit(node.condition)
        body_ir = visit_statements(node.body)

        context = statement_rule_context.merge(
          cond_ir: cond_ir,
          body_ir: body_ir
        )
        @rule_engine.apply(:core_ir_statement, node, context: context)
      end

      # Visit for loop statement: for x in iter { body }
      # Recursively visit iterable and body statements
      def visit_for_statement(node)
        iter_ir = @expression_visitor.visit(node.iterable)
        body_ir = visit_statements(node.body)

        context = statement_rule_context.merge(
          iter_ir: iter_ir,
          body_ir: body_ir
        )
        @rule_engine.apply(:core_ir_statement, node, context: context)
      end

      # Visit if statement: if cond { then_branch } else { else_branch }
      # Recursively visit condition and branch statements
      def visit_if_statement(node)
        cond_ir = @expression_visitor.visit(node.condition)
        then_ir = visit_statements(node.then_branch)
        else_ir = node.else_branch ? visit_statements(node.else_branch) : nil

        context = statement_rule_context.merge(
          cond_ir: cond_ir,
          then_ir: then_ir,
          else_ir: else_ir
        )
        @rule_engine.apply(:core_ir_statement, node, context: context)
      end
    end
  end
end
