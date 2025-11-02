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
# Phase 19-B: ExpressionTransformer extracted into module
require_relative "irgen/expression_transformer"
require_relative "irgen/statement_transformer"
require_relative "irgen/pattern_matching_transformer"
# Phase 23-A: Visitor Pattern for AST traversal
require_relative "visitors/expression_visitor"
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
      include ExpressionTransformer
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




      # Error context management (migrated from BaseTransformer)
      def with_current_node(node, &block)
        @context.with_current_node(node, &block)
      end
    end
end
