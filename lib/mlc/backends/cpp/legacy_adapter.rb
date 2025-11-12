# frozen_string_literal: true

require_relative "../../backend/codegen"
require_relative "bootstrap"

module MLC
  module Backends
    module Cpp
      # LegacyAdapter - Strangler Fig Pattern adapter
      #
      # Purpose: Provides drop-in replacement for Backend::CodeGen while:
      # 1. Using new architecture (Bootstrap, Container, Context, Rules) for lowering
      # 2. Delegating high-level operations (lower_module, lower_function) to legacy
      # 3. Enabling gradual migration of individual components
      #
      # Strategy:
      # - Phase 1 (complete): Delegate all lowering to Backend::CodeGen
      # - Phase 2 (current): Replace expression/statement lowering with new rules
      # - Phase 3 (future): Replace function/module lowering with new implementation
      class LegacyAdapter
        attr_reader :legacy_backend, :container, :context

        def initialize(type_registry:, function_registry: nil, stdlib_scanner: nil, rule_engine: nil, event_bus: nil, runtime_policy: nil)
          # Initialize legacy backend with all original parameters
          @legacy_backend = Backend::CodeGen.new(
            type_registry: type_registry,
            function_registry: function_registry,
            stdlib_scanner: stdlib_scanner,
            rule_engine: rule_engine,
            event_bus: event_bus,
            runtime_policy: runtime_policy
          )

          # Initialize new architecture
          backend = Bootstrap.create_backend(
            type_registry: type_registry,
            function_registry: function_registry,
            runtime_policy: runtime_policy
          )
          @container = backend[:container]
          @context = backend[:context]

          # Track state for generic function context (needed for lowering)
          @in_generic_function = false
        end

        # Main entry point - Phase 2: use new architecture for expression/statement lowering
        def lower(core_ir)
          case core_ir
          when SemanticIR::Module
            lower_module(core_ir)
          when SemanticIR::Func
            lower_function(core_ir)
          when SemanticIR::TypeDecl
            # Delegate type declarations to legacy (Phase 3 concern)
            @legacy_backend.lower(core_ir)
          else
            raise "Unknown SemanticIR node: #{core_ir.class}"
          end
        end

        private

        # Lower module using new architecture for expression/statement lowering
        def lower_module(module_node)
          include_stmt = CppAst::Nodes::IncludeDirective.new(
            path: "mlc_match.hpp",
            system: false
          )

          items = module_node.items.flat_map do |item|
            result = lower(item)
            # If result is a Program (from sum types), extract its statements
            result.is_a?(CppAst::Nodes::Program) ? result.statements : [result]
          end

          statements = [include_stmt] + items
          trailings = ["\n"] + Array.new(items.size, "\n")
          CppAst::Nodes::Program.new(statements: statements, statement_trailings: trailings)
        end

        # Lower function using new architecture for expression/statement lowering
        def lower_function(func)
          return_type = @context.map_type(func.ret_type)
          name = @context.sanitize_identifier(func.name)
          parameters = func.params.map do |param|
            "#{@context.map_type(param.type)} #{@context.sanitize_identifier(param.name)}"
          end

          # Set flag for generic function context
          @in_generic_function = func.type_params.any?
          @container.instance_variable_set(:@in_generic_function, @in_generic_function)

          # Lower function body using new architecture
          block_body = if func.body.is_a?(SemanticIR::BlockExpr)
                         stmts = lower_block_expr_statements(func.body, emit_return: true)
                         CppAst::Nodes::BlockStatement.new(
                           statements: stmts,
                           statement_trailings: Array.new(stmts.length, "\n"),
                           lbrace_suffix: "\n",
                           rbrace_prefix: ""
                         )
                       else
                         body_expr = @context.lower_expression(func.body)
                         CppAst::Nodes::BlockStatement.new(
                           statements: [CppAst::Nodes::ReturnStatement.new(expression: body_expr)],
                           statement_trailings: [""],
                           lbrace_suffix: "",
                           rbrace_prefix: ""
                         )
                       end

          # Reset flag after processing function body
          @in_generic_function = false
          @container.instance_variable_set(:@in_generic_function, false)

          func_decl = CppAst::Nodes::FunctionDeclaration.new(
            return_type: return_type,
            name: name,
            parameters: parameters,
            body: block_body,
            return_type_suffix: " ",
            lparen_suffix: "",
            rparen_suffix: "",
            param_separators: parameters.size > 1 ? Array.new(parameters.size - 1, ", ") : [],
            modifiers_text: "",
            prefix_modifiers: ""
          )

          # Apply legacy function-level rules (for effects, etc.)
          func_decl = @legacy_backend.rule_engine.apply(
            :cpp_function_declaration,
            func_decl,
            context: {
              core_func: func,
              event_bus: @legacy_backend.event_bus
            }
          )

          @legacy_backend.event_bus&.publish(
            :cpp_function_lowered,
            name: func.name,
            effects: Array(func.effects)
          )

          # If function has type parameters, wrap with template declaration
          if func.type_params.any?
            generate_template_function(func.type_params, func_decl)
          else
            func_decl
          end
        end

        # Helper: Lower block expression statements with optional return
        def lower_block_expr_statements(block_expr, emit_return: true)
          statements = block_expr.statements.map { |stmt| @context.lower_statement(stmt) }

          if block_expr.result
            # Skip unit literals - they represent void/no value
            unless block_expr.result.is_a?(SemanticIR::UnitLiteral)
              result_expr = @context.lower_expression(block_expr.result)
              if emit_return
                statements << CppAst::Nodes::ReturnStatement.new(expression: result_expr)
              else
                statements << CppAst::Nodes::ExpressionStatement.new(expression: result_expr)
              end
            end
          end

          statements
        end

        # Helper: Generate template function declaration
        def generate_template_function(type_params, func_decl)
          template_params_str, params_suffix = @context.build_template_signature(type_params)

          CppAst::Nodes::TemplateDeclaration.new(
            template_params: template_params_str,
            declaration: func_decl,
            template_suffix: "",
            less_suffix: "",
            params_suffix: params_suffix
          )
        end

        public

        # Expose legacy backend attributes for compatibility
        def type_registry
          @legacy_backend.type_registry
        end

        def function_registry
          @legacy_backend.function_registry
        end

        def rule_engine
          @legacy_backend.rule_engine
        end

        def event_bus
          @legacy_backend.event_bus
        end

        def runtime_policy
          @legacy_backend.runtime_policy
        end

        def type_map
          @legacy_backend.type_map
        end
      end
    end
  end
end
