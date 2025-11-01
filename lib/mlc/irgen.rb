# frozen_string_literal: true

require_relative "ast/nodes"
require_relative "high_ir/nodes"
require_relative "high_ir/builder"
require_relative "event_bus"
require_relative "stdlib_resolver"
require_relative "stdlib_signature_registry"
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
# expression_transformer.rb deleted - methods integrated into IRGen class
require_relative "irgen/statement_transformer"
require_relative "irgen/pattern_matching_transformer"
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
require_relative "services/generic_call_resolver_service"
require_relative "services/purity_analyzer"
require_relative "services/module_context_service"
require_relative "services/type_resolution_service"
require_relative "services/function_registration_service"
require_relative "services/sum_type_constructor_service"
require_relative "services/type_unification_service"
require_relative "services/lambda_type_inference_service"
require_relative "services/function_lookup_service"
require_relative "services/rule_engine_builder"

module MLC
  class IRGen
      # Include transformation modules
      # BaseTransformer deleted - methods moved to IRGen private section (see bottom of file)
      # TypeInference module deleted - methods moved to TypeInferenceService
      # ExpressionTransformer methods now directly in IRGen class (see below)
      # Phase 18-F: FunctionTransformer split into 4 focused modules
      include StatementTransformer
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
        # NEW: Unified type registry
        @type_registry = TypeRegistry.new
        # Phase 19-A: Use RuleEngineBuilder service
        @rule_engine = rule_engine || Services::RuleEngineBuilder.build_default
        @type_constraint_solver = type_constraint_solver
        @generic_call_resolver = generic_call_resolver
        @match_analyzer = match_analyzer
        @effect_analyzer = effect_analyzer
        @stdlib_resolver = StdlibResolver.new
        @stdlib_registry = StdlibSignatureRegistry.new(scanner: @stdlib_resolver.scanner)
        @event_bus = event_bus || MLC::EventBus.new

        @function_registry = FunctionRegistry.new
        @type_decl_table = {}
        @sum_type_constructors = {}
        @var_type_registry = Services::VarTypeRegistry.new  # Track variable types for let bindings
        @temp_counter = 0
        @loop_depth = 0
        @current_import_aliases = nil

        # Phase 18-A: TransformationContext centralizes transformation state
        @context = Services::TransformationContext.new(var_type_registry: @var_type_registry)
        @lambda_param_type_stack = @context.lambda_param_stack
        @function_return_type_stack = @context.function_return_stack

        # Initialize module context service
        @module_context_service = Services::ModuleContextService.new(
          function_registry: @function_registry
        )

        # Initialize services for rules
        # @expression_transformer_service deleted - rules call transformer directly
        # @predicate_service deleted - rules call transformer directly (unit_branch_ast?)
        # @context_manager_service deleted - state management, not business logic
        @type_checker_service = Services::TypeChecker.new(
          function_registry: @function_registry,
          type_decl_table: @type_decl_table,
          event_bus: @event_bus,
          current_node_proc: -> { @context.current_node }
        )
        @generic_call_resolver_service = Services::GenericCallResolverService.new(self)
        @type_inference_service = Services::TypeInferenceService.new(
          var_type_registry: @var_type_registry,
          type_registry: @type_registry,
          function_registry: @function_registry,
          type_decl_table: @type_decl_table,
          generic_call_resolver: @generic_call_resolver_service,
          type_checker: @type_checker_service,
          transformer: self,
          scope_context: @scope_context_service
        )
        @record_builder_service = Services::RecordBuilderService.new(self)
        @purity_analyzer = Services::PurityAnalyzer.new
        @type_resolution_service = Services::TypeResolutionService.new(
          function_registry: @function_registry,
          type_registry: @type_registry,
          type_checker: @type_checker_service
        )
        @function_registration_service = Services::FunctionRegistrationService.new(
          function_registry: @function_registry,
          module_context_service: @module_context_service
        )
        @sum_type_constructor_service = Services::SumTypeConstructorService.new(
          sum_type_constructors: @sum_type_constructors,
          type_decl_table: @type_decl_table,
          type_checker: @type_checker_service
        )
        # Phase 18-B: Type unification service for generic type inference
        @type_unification_service = Services::TypeUnificationService.new(
          type_checker: @type_checker_service,
          sum_type_constructors: @sum_type_constructors
        )
        # Phase 18-C: Lambda type inference service (no dependencies)
        @lambda_type_inference_service = Services::LambdaTypeInferenceService.new
        # Phase 18-E: Function lookup service for unified function metadata
        @function_lookup_service = Services::FunctionLookupService.new(
          function_registry: @function_registry,
          sum_type_constructors: @sum_type_constructors,
          type_checker: @type_checker_service,
          type_inference: @type_inference_service
        )

        # Initialize type system components (after services)
        @type_constraint_solver ||= TypeSystem::TypeConstraintSolver.new(
          infer_type_arguments: @type_inference_service.method(:infer_type_arguments),
          substitute_type: @type_inference_service.method(:substitute_type),
          ensure_compatible_type: @type_checker_service.method(:ensure_compatible_type),
          type_error: ->(message) { @type_checker_service.type_error(message) }
        )

        @generic_call_resolver ||= TypeSystem::GenericCallResolver.new(
          constraint_solver: @type_constraint_solver
        )

        @match_analyzer ||= TypeSystem::MatchAnalyzer.new(
          ensure_compatible_type: @type_checker_service.method(:ensure_compatible_type),
          type_registry: @type_registry,
          check_exhaustiveness: true
        )

        @effect_analyzer ||= TypeSystem::EffectAnalyzer.new(
          pure_expression: @purity_analyzer.method(:is_pure_expression),
          non_literal_type: @purity_analyzer.method(:non_literal_type?)
        )

        # Phase 19-A: Use RuleEngineBuilder service
        Services::RuleEngineBuilder.ensure_required_rules!(@rule_engine)
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

      # Expression transformation methods (migrated from ExpressionTransformer module)
      # These methods handle AST → HighIR transformation for expressions

      def transform_expression(expr)
        with_current_node(expr) do
          # Special case: AST::Block is a statement block (extends Stmt, not Expr)
          # but can appear in expression context (e.g., function bodies).
          # Convert it to a block expression using statement transformer.
          if expr.is_a?(AST::Block)
            return transform_block(expr, require_value: true)
          end

          # Apply transformation rules
          result = apply_expression_rules(expr)

          # If no rule applied, this is a compiler bug - all AST expression types
          # should be covered by rules in lib/mlc/rules/irgen/expression/
          if result.equal?(expr)
            raise CompileError, "No transformation rule applied for #{expr.class}. " \
                                "This indicates a missing rule in the IRGen rule engine."
          end

          result
        end
      end

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
          generic_call_resolver: @generic_call_resolver_service,
          scope_context: @scope_context_service
        }
        @rule_engine.apply(:core_ir_expression, expr, context: context)
      end


      # Phase 18-C: Delegate to LambdaTypeInferenceService
      def expected_lambda_param_types(object_ir, member_name, transformed_args, index)
        @lambda_type_inference_service.expected_lambda_param_types(object_ir, member_name, transformed_args, index)
      end

      # Phase 18-C: Delegate to LambdaTypeInferenceService
      def lambda_return_type(arg)
        @lambda_type_inference_service.lambda_return_type(arg)
      end

      def transform_block_expr(block_expr)
        # Block expressions reuse the statement pipeline so that we handle the full
        # range of statements (return, break, nested blocks, etc.) consistently.
        # We preserve scope during transformation to allow the trailing expression
        # to see variables defined in the statement portion, and restore the outer
        # scope afterwards to avoid leaking bindings.
        saved_var_types = @var_type_registry.snapshot

        statements_ir = transform_statements(block_expr.statements)
        result_ir = transform_expression(block_expr.result_expr)
        block_type = result_ir&.type || HighIR::Builder.unit_type

        HighIR::Builder.block_expr(statements_ir, result_ir, block_type)
      ensure
        @var_type_registry.restore(saved_var_types)
      end

      def module_member_function_entry(object_node, member_name)
        return unless object_node.is_a?(AST::VarRef)

        # Resolve import alias and lookup member in FunctionRegistry
        resolved = resolve_module_alias(object_node.name)
        @function_registry.fetch_entry_for_member(resolved, member_name)
      end







      # Phase 18-B: Delegate to TypeUnificationService
      def unify_type(pattern, actual, type_map, context:)
        @type_unification_service.unify_type(pattern, actual, type_map, context: context)
      end

      # Phase 18-B: Delegate to TypeUnificationService
      def type_equivalent?(left, right)
        @type_unification_service.type_equivalent?(left, right)
      end







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
          type = infer_call_type(callee, all_args)

          HighIR::Builder.call(callee, all_args, type)
        else
          # right is just a function name => f(left)
          callee = transform_expression(pipe_expr.right)
          type = infer_call_type(callee, [left])

          HighIR::Builder.call(callee, [left], type)
        end
      end


      # Resolve module import alias
      def resolve_module_alias(identifier)
        return identifier unless identifier.is_a?(String) && !identifier.empty?
        (@current_import_aliases && @current_import_aliases[identifier]) || identifier
      end

      # Get constructor info with generic type substitutions
      # Phase 18-B: Delegate to TypeUnificationService
      def constructor_info_for(name, scrutinee_type)
        @type_unification_service.constructor_info_for(name, scrutinee_type)
      end

      # Phase 18-B: Delegate to TypeUnificationService
      def generic_substitutions(info, scrutinee_type)
        @type_unification_service.generic_substitutions(info, scrutinee_type)
      end

      # Phase 18-B: Delegate to TypeUnificationService
      def apply_type_substitutions(type, substitutions)
        @type_unification_service.apply_type_substitutions(type, substitutions)
      end

      # Error context management (migrated from BaseTransformer)
      def with_current_node(node, &block)
        @context.with_current_node(node, &block)
      end

      # Phase 18-E: Delegate to FunctionLookupService
      def builtin_function_info(name)
        @function_lookup_service.builtin_function_info(name)
      end

      # Phase 18-E: Delegate to FunctionLookupService
      def lookup_function_info(name)
        @function_lookup_service.lookup_function_info(name)
      end

      # Phase 18-E: Delegate to FunctionLookupService
      def function_placeholder_type(name)
        @function_lookup_service.function_placeholder_type(name)
      end
    end
end
