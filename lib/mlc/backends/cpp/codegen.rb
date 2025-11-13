# frozen_string_literal: true

require_relative "bootstrap"
require_relative "rules/function_rule"

module MLC
  module Backends
    module Cpp
      # CodeGen - C++ Code Generator (v2 Architecture)
      #
      # Main entry point for lowering SemanticIR to C++ AST.
      # Uses modular architecture with Container, Context, and Rules pattern.
      #
      # Architecture:
      # - Bootstrap: Initializes Container with all services
      # - Container: Dependency injection for type_registry, function_registry, etc.
      # - Context: High-level API for lowering operations
      # - Rules: Chain of Responsibility for expression/statement lowering
      #
      # Migration History:
      # - Phase 1-3 (complete): Strangler Fig migration from legacy Backend::CodeGen
      # - All legacy code removed, v2 architecture is production-ready
      class CodeGen
        attr_reader :container, :context

        def initialize(type_registry:, function_registry: nil, stdlib_scanner: nil, rule_engine: nil, event_bus: nil, runtime_policy: nil)
          # Initialize new architecture
          backend = Bootstrap.create_backend(
            type_registry: type_registry,
            function_registry: function_registry,
            runtime_policy: runtime_policy,
            stdlib_scanner: stdlib_scanner,
            event_bus: event_bus
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
            lower_type_decl_internal(core_ir)
          else
            raise "Unknown SemanticIR node: #{core_ir.class}"
          end
        end

        # Lower function using new architecture for expression/statement lowering
        # Public for test compatibility
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
          block_body = if func.body.nil?
                         # External/opaque functions have no body
                         nil
                       elsif func.body.is_a?(SemanticIR::BlockExpr)
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

          # Apply function-level rules (constexpr, noexcept modifiers)
          function_rule = Rules::FunctionRule.new(@context)
          func_decl = function_rule.apply(
            func_decl,
            semantic_func: func,
            event_bus: @container.event_bus
          )

          @container.event_bus&.publish(
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

        private

        # Lower module using new architecture for expression/statement lowering
        def lower_module(module_node)
          # Track user-defined functions for qualified name resolution
          @container.user_functions = module_node.items
                                                  .grep(SemanticIR::Func)
                                                  .map(&:name)
                                                  .to_set

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

        # Type declaration lowering (migrated from Backend::CodeGen::TypeLowerer)
        def lower_type_decl_internal(type_decl)
          result = case type_decl.type
                   when SemanticIR::RecordType
                     lower_record_type_internal(type_decl.name, type_decl.type)
                   when SemanticIR::SumType
                     lower_sum_type_internal(type_decl.name, type_decl.type, type_decl.type_params)
                   else
                     # For primitive types, generate empty program (no direct C++ emission)
                     CppAst::Nodes::Program.new(statements: [], statement_trailings: [])
                   end

          if type_decl.type_params.any?
            if result.is_a?(CppAst::Nodes::Program)
              wrap_statements_with_template(type_decl.type_params, result)
            else
              template_params_str, params_suffix = @context.build_template_signature(type_decl.type_params)
              CppAst::Nodes::TemplateDeclaration.new(
                template_params: template_params_str,
                declaration: result,
                template_suffix: "",
                less_suffix: "",
                params_suffix: params_suffix
              )
            end
          else
            result
          end
        end

        def lower_record_type_internal(name, record_type)
          # Generate struct declaration
          members = record_type.fields.map do |field|
            field_type = @context.map_type(field[:type])
            CppAst::Nodes::VariableDeclaration.new(
              type: field_type,
              declarators: [@context.sanitize_identifier(field[:name])],
              declarator_separators: [],
              type_suffix: " ",
              prefix_modifiers: ""
            )
          end

          CppAst::Nodes::StructDeclaration.new(
            name: name,
            members: members,
            member_trailings: Array.new(members.size, ""),
            struct_suffix: " ",
            name_suffix: " ",
            lbrace_suffix: "",
            rbrace_suffix: "",
            base_classes_text: ""
          )
        end

        def lower_sum_type_internal(name, sum_type, type_params = [])
          # Generate structs for each variant
          variant_structs = sum_type.variants.map do |variant|
            if variant[:fields].empty?
              # Empty variant - generate empty struct
              CppAst::Nodes::StructDeclaration.new(
                name: variant[:name],
                members: [],
                member_trailings: [],
                struct_suffix: " ",
                name_suffix: " ",
                lbrace_suffix: "",
                rbrace_suffix: "",
                base_classes_text: ""
              )
            else
              # Variant with fields
              members = variant[:fields].map do |field|
                field_type = @context.map_type(field[:type])
                CppAst::Nodes::VariableDeclaration.new(
                  type: field_type,
                  declarators: [field[:name]],
                  declarator_separators: [],
                  type_suffix: " ",
                  prefix_modifiers: ""
                )
              end

              CppAst::Nodes::StructDeclaration.new(
                name: variant[:name],
                members: members,
                member_trailings: Array.new(members.size, ""),
                struct_suffix: " ",
                name_suffix: " ",
                lbrace_suffix: "",
                rbrace_suffix: "",
                base_classes_text: ""
              )
            end
          end

          # Generate using declaration for std::variant
          variant_type_names = if type_params.any?
                                 type_params_str = type_params.map { |tp| tp.name }.join(", ")
                                 sum_type.variants.map { |v| "#{v[:name]}<#{type_params_str}>" }.join(", ")
                               else
                                 sum_type.variants.map { |v| v[:name] }.join(", ")
                               end
          using_decl = CppAst::Nodes::UsingDeclaration.new(
            kind: :alias,
            name: name,
            alias_target: "std::variant<#{variant_type_names}>",
            using_suffix: " ",
            equals_prefix: " ",
            equals_suffix: " "
          )

          # Return program with all structs + using declaration
          all_statements = variant_structs + [using_decl]
          CppAst::Nodes::Program.new(
            statements: all_statements,
            statement_trailings: Array.new(all_statements.size, "")
          )
        end

        def wrap_statements_with_template(type_params, program)
          # Wrap each statement (struct declarations, using) with template
          template_params_str, params_suffix = @context.build_template_signature(type_params)

          wrapped_statements = program.statements.map do |stmt|
            CppAst::Nodes::TemplateDeclaration.new(
              template_params: template_params_str,
              declaration: stmt,
              template_suffix: "",
              less_suffix: "",
              params_suffix: params_suffix
            )
          end

          CppAst::Nodes::Program.new(
            statements: wrapped_statements,
            statement_trailings: Array.new(wrapped_statements.size, "")
          )
        end

        public

        # Expose container attributes for compatibility
        def type_registry
          @container.type_registry
        end

        def function_registry
          @container.function_registry
        end

        def event_bus
          @container.event_bus
        end

        def runtime_policy
          @container.runtime_policy
        end

        def type_map
          @container.type_map
        end

        # Expose map_type for compatibility with tests
        def map_type(type)
          @context.map_type(type)
        end

        # Expose sanitize_identifier for compatibility with header_generator
        def sanitize_identifier(name)
          @context.sanitize_identifier(name)
        end

        # rule_engine no longer exposed - v2 architecture handles rules internally
      end
    end
  end
end
