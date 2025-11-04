# frozen_string_literal: true

require_relative "ast/nodes"
require_relative "high_ir/nodes"
require_relative "high_ir/builder"
require_relative "event_bus"
require_relative "services/stdlib_resolver"
require_relative "services/stdlib_signature_registry"
require_relative "function_registry"
require_relative "type_registry"
# Note: base_transformer.rb deleted - methods now in IRGen class (private section at bottom)
require_relative "services/scope_context_service"
require_relative "pass_manager"
require_relative "rules/rule_engine"
# Note: expression_transformer.rb deleted - methods now in IRGen class directly
require_relative "rules/irgen/sum_constructor_rule"
require_relative "rules/irgen/match_rule"
require_relative "rules/irgen/function_effect_rule"
require_relative "rules/irgen/stdlib_import_rule"
require_relative "rules/irgen/expression/literal_rule"
require_relative "rules/irgen/expression/var_ref_rule"
require_relative "rules/irgen/expression/member_rule"
require_relative "rules/irgen/expression/call_rule"
require_relative "rules/irgen/expression/unary_rule"
require_relative "rules/irgen/expression/binary_rule"
require_relative "rules/irgen/expression/pipe_rule"
require_relative "rules/irgen/expression/let_rule"
require_relative "rules/irgen/expression/record_literal_rule"
require_relative "rules/irgen/expression/if_rule"
require_relative "rules/irgen/expression/array_literal_rule"
require_relative "rules/irgen/expression/do_rule"
require_relative "rules/irgen/expression/block_rule"
require_relative "rules/irgen/expression/match_rule"
require_relative "rules/irgen/expression/lambda_rule"
require_relative "rules/irgen/expression/index_access_rule"
require_relative "rules/irgen/expression/for_loop_rule"
require_relative "rules/irgen/expression/while_loop_rule"
require_relative "rules/irgen/expression/list_comprehension_rule"
require_relative "rules/irgen/statement/expr_stmt_rule"
require_relative "rules/irgen/statement/variable_decl_rule"
require_relative "rules/irgen/statement/assignment_rule"
require_relative "rules/irgen/statement/for_rule"
require_relative "rules/irgen/statement/if_rule"
require_relative "rules/irgen/statement/while_rule"
require_relative "rules/irgen/statement/return_rule"
require_relative "rules/irgen/statement/break_rule"
require_relative "rules/irgen/statement/continue_rule"
require_relative "rules/irgen/statement/block_rule"
require_relative "type_system/type_constraint_solver"
require_relative "type_system/generic_call_resolver"
require_relative "type_system/match_analyzer"
require_relative "type_system/effect_analyzer"
# irgen/type_inference.rb deleted - methods moved to TypeInferenceService
# Phase 25-A: ExpressionTransformer and StatementTransformer deleted - methods moved to IRGen private section
require_relative "irgen/pattern_matching_transformer"
# Phase 23-A: Visitor Pattern for AST traversal
require_relative "visitors/expression_visitor"
require_relative "visitors/statement_visitor"
# Phase 18-F: FunctionTransformer split into 4 focused modules
require_relative "irgen/type_transformer"
require_relative "irgen/function_declaration_transformer"
require_relative "irgen/stdlib_import_transformer"
require_relative "irgen/program_transformer"
# services/expression_transformer.rb deleted - pure delegation wrapper
# services/predicate_service.rb deleted - trivial helpers
# services/context_manager.rb deleted - state management
require_relative "services/var_type_registry"
require_relative "services/type_checker"
require_relative "services/type_inference_service"
require_relative "services/record_builder_service"
require_relative "services/purity_analyzer"
require_relative "services/module_context_service"
require_relative "services/type_resolution_service"
require_relative "services/function_registration_service"
require_relative "services/sum_type_constructor_service"
require_relative "services/type_unification_service"
require_relative "services/function_lookup_service"
require_relative "services/metadata_loader_service"
require_relative "services/rule_engine_builder"
require_relative "services/irgen_container"

module MLC
  class IRGen
      # Include transformation modules
      # BaseTransformer deleted - methods moved to IRGen private section (see bottom of file)
      # TypeInference module deleted - methods moved to TypeInferenceService
      # Phase 19-B: ExpressionTransformer extracted into module
      # Phase 18-F: FunctionTransformer split into 4 focused modules
      # Phase 23-A: Visitor Pattern (separate class, not mixin)
      # Phase 25-A: ExpressionTransformer and StatementTransformer removed - using visitors only
      include PatternMatchingTransformer
      include TypeTransformer
      include FunctionDeclarationTransformer
      include StdlibImportTransformer
      include ProgramTransformer

      FunctionInfo = Struct.new(:name, :param_types, :ret_type, :type_params) do
        def initialize(name, param_types, ret_type, type_params = [])
          super(name, param_types, ret_type, type_params)
        end
      end
      NUMERIC_PRIMITIVES = %w[i32 f32 i64 f64 u32 u64].freeze
      IO_RETURN_TYPES = {
        "print" => "i32",
        "println" => "i32",
        "eprint" => "i32",
        "eprintln" => "i32",
        "read_line" => "string",
        "input" => "string",
        "args" => :array_of_string,
        "to_string" => "string",
        "format" => "string"
      }.freeze
      BUILTIN_CONSTRAINTS = {
        "Numeric" => %w[i32 f32 i64 f64 u32 u64]
      }.freeze

      # Expose type_registry for sharing with CppLowering
      attr_reader :type_registry, :function_registry, :rule_engine, :generic_call_resolver, :match_analyzer, :effect_analyzer, :stdlib_registry, :event_bus

      def initialize(rule_engine: nil, generic_call_resolver: nil, type_constraint_solver: nil, match_analyzer: nil, effect_analyzer: nil, event_bus: nil)
        # Phase 20: Use IRGenContainer for service initialization
        # Eliminates 109 lines of initialization boilerplate
        container = Services::IRGenContainer.build(
          rule_engine: rule_engine,
          generic_call_resolver: generic_call_resolver,
          type_constraint_solver: type_constraint_solver,
          match_analyzer: match_analyzer,
          effect_analyzer: effect_analyzer,
          event_bus: event_bus,
          transformer: self
        )

        # Unpack container components into instance variables
        @type_registry = container.type_registry
        @function_registry = container.function_registry
        @rule_engine = container.rule_engine
        @type_constraint_solver = container.type_constraint_solver
        @generic_call_resolver = container.generic_call_resolver
        @match_analyzer = container.match_analyzer
        @effect_analyzer = container.effect_analyzer
        @stdlib_resolver = container.stdlib_resolver
        @stdlib_registry = container.stdlib_registry
        @event_bus = container.event_bus
        @type_decl_table = container.type_decl_table
        @sum_type_constructors = container.sum_type_constructors
        @var_type_registry = container.var_type_registry
        @temp_counter = container.temp_counter
        @loop_depth = container.loop_depth
        @current_import_aliases = container.current_import_aliases
        @context = container.context
        @scope_context_service = container.scope_context_service  # Phase 20: Fixed initialization
        @module_context_service = container.module_context_service
        @type_checker_service = container.type_checker_service
        @type_inference_service = container.type_inference_service
        @record_builder_service = container.record_builder_service
        @purity_analyzer = container.purity_analyzer
        @type_resolution_service = container.type_resolution_service
        @function_registration_service = container.function_registration_service
        @sum_type_constructor_service = container.sum_type_constructor_service
        @type_unification_service = container.type_unification_service
        @function_lookup_service = container.function_lookup_service
        @metadata_loader_service = container.metadata_loader_service

        # Maintain backward compatibility for stacks
        @lambda_param_type_stack = @context.lambda_param_stack
        @function_return_type_stack = @context.function_return_stack

        # Phase 23-A: Instantiate separate ExpressionVisitor class
        @expression_visitor = Visitors::ExpressionVisitor.new(
          rule_engine: @rule_engine,
          transformer: self,
          services: {
            type_registry: @type_registry,
            function_registry: @function_registry,
            type_checker: @type_checker_service,
            type_inference: @type_inference_service,
            record_builder: @record_builder_service,
            scope_context: @scope_context_service,
            var_type_registry: @var_type_registry
          }
        )

        # Phase 23-C: Instantiate separate StatementVisitor class
        @statement_visitor = Visitors::StatementVisitor.new(
          rule_engine: @rule_engine,
          transformer: self,
          expression_visitor: @expression_visitor,
          services: {
            type_registry: @type_registry,
            function_registry: @function_registry,
            type_checker: @type_checker_service,
            type_inference: @type_inference_service,
            scope_context: @scope_context_service,
            var_type_registry: @var_type_registry
          }
        )

        # Phase 25-B: Connect statement_visitor back to expression_visitor (breaks circular dependency)
        @expression_visitor.statement_visitor = @statement_visitor
      end

      def transform(ast)
        case ast
        when AST::Program
          transform_program(ast)
        when AST::FuncDecl
          transform_function(ast)
        when AST::TypeDecl
          transform_type_decl(ast)
        else
          raise "Unknown AST node: #{ast.class}"
        end
      end

      private

      # Phase 19-A: Rule engine setup methods moved to RuleEngineBuilder service

      # Phase 18-A: Delegate to TransformationContext
      def with_type_params(params, &block)
        @context.with_type_params(params, &block)
      end

      def current_type_params
        @context.current_type_params
      end

      def with_function_return(type, &block)
        @context.with_function_return(type, &block)
      end

      def current_function_return
        @context.current_function_return
      end

      def with_lambda_param_types(types, &block)
        @context.with_lambda_param_types(types, &block)
      end

      def current_lambda_param_types
        @context.current_lambda_param_types
      end

      # Delegate to TypeInferenceService
      def expected_lambda_param_types(object_ir, member_name, transformed_args, index)
        @type_inference_service.expected_lambda_param_types(object_ir, member_name, transformed_args, index)
      end

      # Delegate to TypeInferenceService
      def lambda_return_type(arg)
        @type_inference_service.lambda_return_type(arg)
      end

      # =========================================================================
      # Phase 25-A: ExpressionTransformer and StatementTransformer methods
      # Copied from mixin modules to eliminate `include` dependency
      # Methods remain unchanged - only moved from module to class
      # =========================================================================

      # --- ExpressionTransformer methods ---

      # Apply expression transformation rules with full context
      # Builds context hash with all necessary dependencies for rules
      def apply_expression_rules(expr)
        context = {
          transformer: self,
          expression_visitor: @expression_visitor,
          statement_visitor: @statement_visitor,
          type_registry: @type_registry,
          function_registry: @function_registry,
          rule_engine: @rule_engine,
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

          # Infer return type
          type = @type_inference_service.infer_call_type(callee, all_args)

          HighIR::Builder.call(callee, all_args, type)
        else
          # right is just a function name => f(left)
          callee = transform_expression(pipe_expr.right)
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

      # --- StatementTransformer methods ---

      def apply_statement_rules(stmt)
        context = {
          transformer: self,
          expression_visitor: @expression_visitor,
          statement_visitor: @statement_visitor,
          type_registry: @type_registry,
          function_registry: @function_registry,
          rule_engine: @rule_engine,
          type_checker: @type_checker_service,
          type_inference: @type_inference_service,
          record_builder: @record_builder_service,
          scope_context: @scope_context_service,
          var_type_registry: @var_type_registry
        }
        result = @rule_engine.apply(:core_ir_statement, stmt, context: context)
        return nil if result.equal?(stmt)
        result.is_a?(Array) ? result : [result]
      end

      def transform_block(block, require_value: true, preserve_scope: false)
        with_current_node(block) do
          saved_var_types = @var_type_registry.snapshot unless preserve_scope
          if block.stmts.empty?
            if require_value
              @type_checker_service.type_error("Block must end with an expression")
            else
              return HighIR::Builder.block_expr(
                [],
                nil,
                HighIR::Builder.primitive_type("void")
              )
            end
          end

          statements = block.stmts.dup
          tail = require_value ? statements.pop : nil

          statement_nodes = transform_statements(statements)
          result_ir = nil

          if require_value && tail
            case tail
            when AST::ExprStmt
              result_ir = transform_expression(tail.expr)
            when AST::Return
              statement_nodes << transform_return_statement(tail)
            else
              statement_nodes.concat(transform_statements([tail]))
            end
          end

          block_type = result_ir ? result_ir.type : HighIR::Builder.primitive_type("void")
          HighIR::Builder.block_expr(statement_nodes, result_ir, block_type)
        ensure
          @var_type_registry.restore(saved_var_types) if defined?(saved_var_types) && !preserve_scope
        end
      end

      def transform_expr_statement(expr_stmt)
        expr = expr_stmt.expr
        case expr
        when AST::ForLoop
          [transform_for_statement(expr)]
        when AST::IfExpr
          [transform_if_statement(expr.condition, expr.then_branch, expr.else_branch)]
        when AST::WhileLoop
          [transform_while_statement(expr.condition, expr.body)]
        when AST::Block
          transform_block(expr, require_value: false).statements
        else
          ir = transform_expression(expr)
          if expr.is_a?(AST::IfExpr) && ir.is_a?(HighIR::IfExpr)
            HighIR::Builder.if_stmt(ir.condition, transform_statement_block(expr.then_branch), expr.else_branch ? transform_statement_block(expr.else_branch) : nil)
            []
          else
            [HighIR::Builder.expr_statement(ir)]
          end
        end
      end

      def transform_for_statement(stmt)
        iterable_ir = transform_expression(stmt.iterable)
        saved = @var_type_registry.get(stmt.var_name)
        element_type = @type_inference_service.infer_iterable_type(iterable_ir)
        @var_type_registry.set(stmt.var_name, element_type)
        body_ir = within_loop_scope { transform_statement_block(stmt.body, preserve_scope: true) }

        HighIR::Builder.for_stmt(stmt.var_name, element_type, iterable_ir, body_ir)
      ensure
        if saved
          @var_type_registry.set(stmt.var_name, saved)
        else
          @var_type_registry.delete(stmt.var_name)
        end
      end

      def transform_if_statement(condition_node, then_node, else_node)
        condition_ir = transform_expression(condition_node)
        @type_checker_service.ensure_boolean_type(condition_ir.type, "if condition", node: condition_node)
        then_ir = transform_statement_block(then_node)
        else_ir = else_node ? transform_statement_block(else_node) : nil
        HighIR::Builder.if_stmt(condition_ir, then_ir, else_ir)
      end

      def transform_return_statement(stmt)
        expected = current_function_return
        @type_checker_service.type_error("return statement outside of function") unless expected

        expr_ir = stmt.expr ? transform_expression(stmt.expr) : nil

        if @type_inference_service.void_type?(expected)
          @type_checker_service.type_error("return value not allowed in void function", node: stmt) if expr_ir
        else
          unless expr_ir
            expected_name = @type_checker_service.describe_type(expected)
            @type_checker_service.type_error("return statement requires a value of type #{expected_name}", node: stmt)
          end
          @type_checker_service.ensure_compatible_type(expr_ir.type, expected, "return statement", node: stmt)
        end

        HighIR::Builder.return_stmt(expr_ir)
      end

      def transform_statement_block(node, preserve_scope: false)
        block_ast =
          case node
          when AST::Block
            node
          when AST::BlockExpr
            stmts = node.statements.dup
            if node.result_expr && !node.result_expr.is_a?(AST::UnitLit)
              stmts << AST::ExprStmt.new(expr: node.result_expr)
            end
            AST::Block.new(stmts: stmts)
          when AST::Stmt
            AST::Block.new(stmts: [node])
          else
            AST::Block.new(stmts: [AST::ExprStmt.new(expr: node)])
          end

        transform_block(block_ast, require_value: false, preserve_scope: preserve_scope)
      end

      def transform_statements(statements)
        statements.each_with_object([]) do |stmt, acc|
          with_current_node(stmt) do
            if (rule_result = apply_statement_rules(stmt))
              acc.concat(rule_result)
              next
            end

            case stmt
            when AST::Block
              nested = transform_block(stmt, require_value: false)
              acc.concat(nested.statements)
            else
              @type_checker_service.type_error("Unsupported statement: #{stmt.class}", node: stmt)
            end
          end
        end
      end

      def transform_while_statement(condition_node, body_node)
        condition_ir = transform_expression(condition_node)
        @type_checker_service.ensure_boolean_type(condition_ir.type, "while condition", node: condition_node)
        body_ir = within_loop_scope { transform_statement_block(body_node, preserve_scope: true) }
        HighIR::Builder.while_stmt(condition_ir, body_ir)
      end

      def transform_variable_decl_statement(stmt)
        value_ir = transform_expression(stmt.value)

        var_type = if stmt.type
                     explicit_type = transform_type(stmt.type)
                     if value_ir.is_a?(HighIR::RecordExpr) && value_ir.type_name == "record"
                       actual_type_name = @type_checker_service.type_name(explicit_type)
                       value_ir = HighIR::Builder.record(actual_type_name, value_ir.fields, explicit_type)
                     else
                       @type_checker_service.ensure_compatible_type(value_ir.type, explicit_type, "variable '#{stmt.name}' initialization")
                     end
                     explicit_type
                   else
                     value_ir.type
                   end

        @var_type_registry.set(stmt.name, var_type)

        [HighIR::Builder.variable_decl_stmt(
          stmt.name,
          var_type,
          value_ir,
          mutable: stmt.mutable
        )]
      end

      def transform_assignment_statement(stmt)
        unless stmt.target.is_a?(AST::VarRef)
          @type_checker_service.type_error("Assignment target must be a variable", node: stmt)
        end

        target_name = stmt.target.name
        existing_type = @var_type_registry.get(target_name)
        @type_checker_service.type_error("Assignment to undefined variable '#{target_name}'", node: stmt) unless existing_type

        value_ir = transform_expression(stmt.value)
        @type_checker_service.ensure_compatible_type(value_ir.type, existing_type, "assignment to '#{target_name}'")
        @var_type_registry.set(target_name, existing_type)
        target_ir = HighIR::Builder.var(target_name, existing_type)
        [HighIR::Builder.assignment_stmt(target_ir, value_ir)]
      end

      def transform_break_statement(stmt)
        @type_checker_service.type_error("'break' used outside of loop", node: stmt) if @loop_depth.to_i <= 0
        [HighIR::Builder.break_stmt]
      end

      def transform_continue_statement(stmt)
        @type_checker_service.type_error("'continue' used outside of loop", node: stmt) if @loop_depth.to_i <= 0
        [HighIR::Builder.continue_stmt]
      end

      def transform_block_statement(stmt)
        nested = transform_block(stmt, require_value: false)
        nested.statements
      end

      def within_loop_scope
        @loop_depth ||= 0
        @loop_depth += 1
        yield
      ensure
        @loop_depth -= 1
      end

      # =========================================================================
      # End of Phase 25-A transformer methods
      # =========================================================================

      # Error context management (migrated from BaseTransformer)
      def with_current_node(node, &block)
        @context.with_current_node(node, &block)
      end
    end
end
