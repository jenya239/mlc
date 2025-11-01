# frozen_string_literal: true

require_relative "ast/nodes"
require_relative "high_ir/nodes"
require_relative "high_ir/builder"
require_relative "event_bus"
require_relative "stdlib_resolver"
require_relative "stdlib_signature_registry"
require_relative "function_registry"
require_relative "type_registry"
require_relative "irgen/base_transformer"
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
require_relative "irgen/function_transformer"
# services/expression_transformer.rb deleted - pure delegation wrapper
# services/predicate_service.rb deleted - trivial helpers
# services/context_manager.rb deleted - state management
require_relative "services/var_type_registry"
require_relative "services/type_checker"
require_relative "services/type_inference_service"
require_relative "services/record_builder_service"
require_relative "services/generic_call_resolver_service"
require_relative "services/purity_analyzer"

module MLC
  class IRGen
      # Include all transformation modules
      include BaseTransformer
      # TypeInference module deleted - methods moved to TypeInferenceService
      # ExpressionTransformer methods now directly in IRGen class (see below)
      include StatementTransformer
      include FunctionTransformer

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
        @rule_engine = rule_engine || build_default_rule_engine
        @scope_context_service = Services::ScopeContextService.new
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
        @lambda_param_type_stack = @scope_context_service.lambda_param_stack
        @function_return_type_stack = @scope_context_service.function_return_stack
        @current_node = nil
        @current_import_aliases = nil
        @current_module_name = nil
        @current_module_namespace = nil

        # Initialize services for rules
        # @expression_transformer_service deleted - rules call transformer directly
        # @predicate_service deleted - rules call transformer directly (unit_branch_ast?)
        # @context_manager_service deleted - state management, not business logic
        @type_checker_service = Services::TypeChecker.new(
          function_registry: @function_registry,
          event_bus: @event_bus,
          current_node_proc: -> { @current_node }
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

        ensure_required_rules!
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

      def build_default_rule_engine
        engine = Rules::RuleEngine.new
        engine.register(:core_ir_type_decl, Rules::IRGen::SumConstructorRule.new)
        engine.register(:core_ir_match_expr, Rules::IRGen::MatchRule.new)
        engine.register(:core_ir_function, Rules::IRGen::FunctionEffectRule.new)
        engine.register(:core_ir_stdlib_import, Rules::IRGen::StdlibImportRule.new)
        engine.register(:core_ir_expression, Rules::IRGen::Expression::LiteralRule.new)
        engine.register(:core_ir_expression, Rules::IRGen::Expression::VarRefRule.new)
        engine.register(:core_ir_expression, Rules::IRGen::Expression::MemberRule.new)
        engine.register(:core_ir_expression, Rules::IRGen::Expression::CallRule.new)
        engine.register(:core_ir_expression, Rules::IRGen::Expression::UnaryRule.new)
        engine.register(:core_ir_expression, Rules::IRGen::Expression::PipeRule.new)
        engine.register(:core_ir_expression, Rules::IRGen::Expression::BinaryRule.new)
        engine.register(:core_ir_expression, Rules::IRGen::Expression::LetRule.new)
        engine.register(:core_ir_expression, Rules::IRGen::Expression::RecordLiteralRule.new)
        engine.register(:core_ir_expression, Rules::IRGen::Expression::IfRule.new)
        engine.register(:core_ir_expression, Rules::IRGen::Expression::ArrayLiteralRule.new)
        engine.register(:core_ir_expression, Rules::IRGen::Expression::DoRule.new)
        engine.register(:core_ir_expression, Rules::IRGen::Expression::BlockRule.new)
        engine.register(:core_ir_expression, Rules::IRGen::Expression::MatchRule.new)
        engine.register(:core_ir_expression, Rules::IRGen::Expression::LambdaRule.new)
        engine.register(:core_ir_expression, Rules::IRGen::Expression::IndexAccessRule.new)
        engine.register(:core_ir_expression, Rules::IRGen::Expression::ForLoopRule.new)
        engine.register(:core_ir_expression, Rules::IRGen::Expression::WhileLoopRule.new)
        engine.register(:core_ir_expression, Rules::IRGen::Expression::ListComprehensionRule.new)
        engine.register(:core_ir_statement, Rules::IRGen::Statement::ExprStmtRule.new)
        engine.register(:core_ir_statement, Rules::IRGen::Statement::VariableDeclRule.new)
        engine.register(:core_ir_statement, Rules::IRGen::Statement::AssignmentRule.new)
        engine.register(:core_ir_statement, Rules::IRGen::Statement::ForRule.new)
        engine.register(:core_ir_statement, Rules::IRGen::Statement::IfRule.new)
        engine.register(:core_ir_statement, Rules::IRGen::Statement::WhileRule.new)
        engine.register(:core_ir_statement, Rules::IRGen::Statement::ReturnRule.new)
        engine.register(:core_ir_statement, Rules::IRGen::Statement::BreakRule.new)
        engine.register(:core_ir_statement, Rules::IRGen::Statement::ContinueRule.new)
        engine.register(:core_ir_statement, Rules::IRGen::Statement::BlockRule.new)
        engine
      end

      def ensure_required_rules!
        ensure_rule_registered(:core_ir_type_decl, Rules::IRGen::SumConstructorRule)
        ensure_rule_registered(:core_ir_match_expr, Rules::IRGen::MatchRule)
        ensure_rule_registered(:core_ir_function, Rules::IRGen::FunctionEffectRule)
        ensure_rule_registered(:core_ir_stdlib_import, Rules::IRGen::StdlibImportRule)
        ensure_rule_registered(:core_ir_expression, Rules::IRGen::Expression::LiteralRule)
        ensure_rule_registered(:core_ir_expression, Rules::IRGen::Expression::VarRefRule)
        ensure_rule_registered(:core_ir_expression, Rules::IRGen::Expression::MemberRule)
        ensure_rule_registered(:core_ir_expression, Rules::IRGen::Expression::CallRule)
        ensure_rule_registered(:core_ir_expression, Rules::IRGen::Expression::UnaryRule)
        ensure_rule_registered(:core_ir_expression, Rules::IRGen::Expression::PipeRule)
        ensure_rule_registered(:core_ir_expression, Rules::IRGen::Expression::BinaryRule)
        ensure_rule_registered(:core_ir_expression, Rules::IRGen::Expression::LetRule)
        ensure_rule_registered(:core_ir_expression, Rules::IRGen::Expression::RecordLiteralRule)
        ensure_rule_registered(:core_ir_expression, Rules::IRGen::Expression::IfRule)
        ensure_rule_registered(:core_ir_expression, Rules::IRGen::Expression::ArrayLiteralRule)
        ensure_rule_registered(:core_ir_expression, Rules::IRGen::Expression::DoRule)
        ensure_rule_registered(:core_ir_expression, Rules::IRGen::Expression::BlockRule)
        ensure_rule_registered(:core_ir_expression, Rules::IRGen::Expression::MatchRule)
        ensure_rule_registered(:core_ir_expression, Rules::IRGen::Expression::LambdaRule)
        ensure_rule_registered(:core_ir_expression, Rules::IRGen::Expression::IndexAccessRule)
        ensure_rule_registered(:core_ir_expression, Rules::IRGen::Expression::ForLoopRule)
        ensure_rule_registered(:core_ir_expression, Rules::IRGen::Expression::WhileLoopRule)
        ensure_rule_registered(:core_ir_expression, Rules::IRGen::Expression::ListComprehensionRule)
        ensure_rule_registered(:core_ir_statement, Rules::IRGen::Statement::ExprStmtRule)
        ensure_rule_registered(:core_ir_statement, Rules::IRGen::Statement::VariableDeclRule)
        ensure_rule_registered(:core_ir_statement, Rules::IRGen::Statement::AssignmentRule)
        ensure_rule_registered(:core_ir_statement, Rules::IRGen::Statement::ForRule)
        ensure_rule_registered(:core_ir_statement, Rules::IRGen::Statement::IfRule)
        ensure_rule_registered(:core_ir_statement, Rules::IRGen::Statement::WhileRule)
        ensure_rule_registered(:core_ir_statement, Rules::IRGen::Statement::ReturnRule)
        ensure_rule_registered(:core_ir_statement, Rules::IRGen::Statement::BreakRule)
        ensure_rule_registered(:core_ir_statement, Rules::IRGen::Statement::ContinueRule)
        ensure_rule_registered(:core_ir_statement, Rules::IRGen::Statement::BlockRule)
      end

      def ensure_rule_registered(stage, rule_class)
        rules = @rule_engine.registry[stage.to_sym]
        return if rules.any? { |rule| rule.is_a?(rule_class) || rule == rule_class || rule.class == rule_class }

        @rule_engine.register(stage, rule_class.new)
      end

      def with_type_params(params, &block)
        @scope_context_service.with_type_params(params, &block)
      end

      def current_type_params
        @scope_context_service.current_type_params
      end

      def with_function_return(type, &block)
        @scope_context_service.with_function_return(type, &block)
      end

      def current_function_return
        @scope_context_service.current_function_return
      end

      def with_lambda_param_types(types, &block)
        @scope_context_service.with_lambda_param_types(types, &block)
      end

      def current_lambda_param_types
        @scope_context_service.current_lambda_param_types
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

      def bind_pattern_variables(pattern, scrutinee_type)
        case pattern[:kind]
        when :constructor
          info = constructor_info_for(pattern[:name], scrutinee_type)
          field_types = info ? info.param_types : []
          bindings = []

          Array(pattern[:fields]).each_with_index do |field_name, idx|
            next if field_name.nil? || field_name == "_"
            field_type = field_types[idx] || HighIR::Builder.primitive_type("auto")
            @var_type_registry.set(field_name, field_type)
            bindings << field_name
          end

          pattern[:bindings] = bindings unless bindings.empty?
        when :var
          name = pattern[:name]
          @var_type_registry.set(name, scrutinee_type) if name && name != "_"
        when :regex
          Array(pattern[:bindings]).each do |binding|
            next if binding.nil? || binding == "_"
            @var_type_registry.set(binding, HighIR::Builder.primitive_type("string"))
          end
        end
      end

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
        module_member_entry(object_node.name, member_name)
      end







      def unify_type(pattern, actual, type_map, context:)
        return false unless pattern && actual

        case pattern
        when HighIR::TypeVariable
          existing = type_map[pattern.name]
          if existing
            type_equivalent?(existing, actual)
          else
            type_map[pattern.name] = actual
            true
          end
        when HighIR::GenericType
          return false unless actual.is_a?(HighIR::GenericType)
          pattern_base = type_name(pattern.base_type)
          actual_base = type_name(actual.base_type)
          return false unless pattern_base == actual_base

          pattern.type_args.zip(actual.type_args).all? do |pattern_arg, actual_arg|
            unify_type(pattern_arg, actual_arg, type_map, context: context)
          end
        when HighIR::ArrayType
          return false unless actual.is_a?(HighIR::ArrayType)
          unify_type(pattern.element_type, actual.element_type, type_map, context: context)
        else
          type_equivalent?(pattern, actual)
        end
      end

      def type_equivalent?(left, right)
        return false if left.nil? || right.nil?
        return true if left.equal?(right)

        if left.is_a?(HighIR::TypeVariable) && right.is_a?(HighIR::TypeVariable)
          return left.name == right.name
        end

        if left.is_a?(HighIR::GenericType) && right.is_a?(HighIR::GenericType)
          left_base = type_name(left.base_type)
          right_base = type_name(right.base_type)
          return false unless left_base == right_base
          return false unless left.type_args.length == right.type_args.length

          return left.type_args.zip(right.type_args).all? { |l_arg, r_arg| type_equivalent?(l_arg, r_arg) }
        end

        if left.is_a?(HighIR::ArrayType) && right.is_a?(HighIR::ArrayType)
          return type_equivalent?(left.element_type, right.element_type)
        end

        type_name(left) == type_name(right)
      end

      def transform_match_expr(match_expr)
        scrutinee_ir = transform_expression(match_expr.scrutinee)

        if match_stmt_applicable?(match_expr)
          return transform_match_expr_to_statement(match_expr, scrutinee_ir)
        end

        transform_match_expr_core(match_expr, scrutinee_ir)
      end

      def transform_match_expr_core(match_expr, scrutinee_ir = nil)
        scrutinee_ir ||= transform_expression(match_expr.scrutinee)

        result = @rule_engine.apply(
          :core_ir_match_expr,
          match_expr,
          context: {
            scrutinee: scrutinee_ir,
            match_analyzer: @match_analyzer,
            transform_arm: method(:transform_match_arm)
          }
        )

        unless result.is_a?(HighIR::MatchExpr)
          raise "Match rule must produce HighIR::MatchExpr, got #{result.class}"
        end

        result
      end

      def transform_match_expr_to_statement(match_expr, scrutinee_ir)
        arms = match_expr.arms.map do |arm|
          transform_match_arm_statement(scrutinee_ir.type, arm)
        end

        match_stmt = HighIR::Builder.match_stmt(scrutinee_ir, arms, origin: match_expr.origin)
        unit_literal = HighIR::Builder.unit_literal(origin: match_expr.origin)
        HighIR::Builder.block_expr([match_stmt], unit_literal, unit_literal.type, origin: match_expr.origin)
      end

      def transform_match_arm(scrutinee_type, arm)
        saved_var_types = @var_type_registry.snapshot
        pattern = transform_pattern(arm[:pattern])
        bind_pattern_variables(pattern, scrutinee_type)
        guard = arm[:guard] ? transform_expression(arm[:guard]) : nil
        body = transform_expression(arm[:body])
        {pattern: pattern, guard: guard, body: body}
      ensure
        @var_type_registry.restore(saved_var_types)
      end

      def transform_match_arm_statement(scrutinee_type, arm)
        saved_var_types = @var_type_registry.snapshot
        pattern = transform_pattern(arm[:pattern])
        bind_pattern_variables(pattern, scrutinee_type)
        guard_ir = arm[:guard] ? transform_expression(arm[:guard]) : nil
        body_ir = transform_statement_block(arm[:body])
        {pattern: pattern, guard: guard_ir, body: body_ir}
      ensure
        @var_type_registry.restore(saved_var_types)
      end

      def transform_pattern(pattern)
        case pattern.kind
        when :wildcard
          {kind: :wildcard}
        when :literal
          {kind: :literal, value: pattern.data[:value]}
        when :constructor
          {kind: :constructor, name: pattern.data[:name], fields: pattern.data[:fields]}
        when :var
          {kind: :var, name: pattern.data[:name]}
        when :regex
          {
            kind: :regex,
            pattern: pattern.data[:pattern],
            flags: pattern.data[:flags],
            bindings: pattern.data[:bindings] || []
          }
        else
          raise "Unknown pattern kind: #{pattern.kind}"
        end
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

      def unit_branch_ast?(node)
        node.is_a?(AST::BlockExpr) && node.result_expr.is_a?(AST::UnitLit)
      end

      def match_stmt_applicable?(match_expr)
        match_expr.arms.all? do |arm|
          unit_branch_ast?(arm[:body]) &&
            arm[:guard].nil? &&
            match_stmt_safe_pattern?(arm[:pattern])
        end
      end

      def match_stmt_safe_pattern?(pattern)
        kind = pattern.kind
        return false if kind == :regex
        %i[constructor wildcard var].include?(kind)
      end
    end
end
