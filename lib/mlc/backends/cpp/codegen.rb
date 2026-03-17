# frozen_string_literal: true

# rubocop:disable Metrics/ParameterLists

# All classes autoloaded by Zeitwerk on-demand:
# - MLC::Backends::Cpp::Bootstrap (line 25)
# - MLC::Backends::Cpp::Rules::FunctionRule (line 106)

require_relative "../../../cpp_ast/index" # CppAst not using Zeitwerk, manual require needed

module MLC
  module Backends
    module Cpp
      # Codegen - C++ Code Generator (v2 Architecture)
      #
      # Main entry point for lowering SemanticIR to C++ AST.
      # Uses modular architecture with Container, Context, and Rules pattern.
      #
      # Architecture:
      # - Bootstrap: Initializes Container with all services
      # - Container: Dependency injection for type_registry, function_registry, etc.
      # - Context: High-level API for lowering operations
      # - Rules: Chain of Responsibility for expression/statement lowering
      class Codegen
        attr_reader :container, :context

        def initialize(type_registry:, function_registry: nil, trait_registry: nil, stdlib_scanner: nil, _rule_engine: nil, event_bus: nil, runtime_policy: nil)
          # Initialize new architecture
          backend = MLC::Backends::Cpp::Bootstrap.create_backend(
            type_registry: type_registry,
            function_registry: function_registry,
            runtime_policy: runtime_policy,
            stdlib_scanner: stdlib_scanner,
            event_bus: event_bus
          )
          @container = backend[:container]
          @context = backend[:context]
          @trait_registry = trait_registry
          @container.trait_registry = trait_registry

          @in_generic_function = false
          @cyclic_sum_types = Set.new
        end

        def cyclic_sum_types=(val)
          @cyclic_sum_types = val
          @container.cyclic_sum_types = val
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
          # Reset variable declaration tracking for each new function scope
          @context.reset_declared_variables

          return_type = @context.map_type(func.ret_type)
          name = @context.sanitize_identifier(func.name)
          parameters = func.params.map do |param|
            "#{@context.map_type(param.type)} #{@context.sanitize_identifier(param.name)}"
          end

          # Set flag for generic function context
          @in_generic_function = func.type_params.any?
          @container.instance_variable_set(:@in_generic_function, @in_generic_function)

          # For main(), inject argc/argv and set_args preamble
          is_main = name == "main" && func.params.empty?
          if is_main
            parameters = ["int argc", "char** argv"]
          end

          # Lower function body using new architecture
          block_body = if func.body.nil?
                         # External/opaque functions have no body
                         nil
                       elsif func.body.is_a?(SemanticIR::BlockExpr)
                         @container.expected_return_type = func.ret_type
                         stmts = lower_block_expr_statements(func.body, emit_return: true)
                         if is_main
                           set_args = @context.factory.raw_statement(
                             code: "mlc::io::set_args(std::vector<mlc::String>(argv + 1, argv + argc));"
                           )
                           stmts = [set_args] + stmts
                         end
                         CppAst::Nodes::BlockStatement.new(
                           statements: stmts,
                           statement_trailings: Array.new(stmts.length, "\n"),
                           lbrace_suffix: "\n",
                           rbrace_prefix: ""
                         )
                       else
                         @container.expected_return_type = func.ret_type
                         body_expr = @context.lower_expression(func.body)
                         CppAst::Nodes::BlockStatement.new(
                           statements: [CppAst::Nodes::ReturnStatement.new(expression: body_expr)],
                           statement_trailings: [""],
                           lbrace_suffix: "",
                           rbrace_prefix: ""
                         )
                       end

          # Reset flags after processing function body
          @in_generic_function = false
          @container.instance_variable_set(:@in_generic_function, false)
          @container.expected_return_type = nil

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
          function_rule = MLC::Backends::Cpp::Rules::FunctionRule.new(@context)
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

          # Standard includes for runtime support
          include_stmts = [
            CppAst::Nodes::IncludeDirective.new(path: "mlc/core/string.hpp", system: false),
            CppAst::Nodes::IncludeDirective.new(path: "mlc/core/collections.hpp", system: false),
            CppAst::Nodes::IncludeDirective.new(path: "mlc/core/hashmap.hpp", system: false),
            CppAst::Nodes::IncludeDirective.new(path: "mlc/io/io.hpp", system: false),
            CppAst::Nodes::IncludeDirective.new(path: "mlc/io/file.hpp", system: false),
            CppAst::Nodes::IncludeDirective.new(path: "mlc/core/match.hpp", system: false),
            CppAst::Nodes::IncludeDirective.new(path: "mlc/core/result.hpp", system: false)
          ]

          # Phase 1: emit forward declarations + using aliases for ALL sum types.
          # Record types (like MatchArm, FieldVal) may reference sum types (Expr, Stmt)
          # that are declared later. Emitting all sum type preambles first ensures
          # every sum type name is known before any struct body references it.
          sum_preambles = module_node.items.grep(SemanticIR::TypeDecl).flat_map do |td|
            next [] unless td.type.is_a?(SemanticIR::SumType)
            next [] if td.type_params.any? # skip generic sum types (handled inline)
            sum_type_preamble_stmts(td.name, td.type)
          end

          # Phase 2: full type definitions (sum type variant structs + record types).
          # Functions are excluded here; they come after forward declarations in Phase 4.
          type_defs = module_node.items.grep(SemanticIR::TypeDecl).flat_map do |td|
            if td.type.is_a?(SemanticIR::SumType) && td.type_params.empty?
              sum_type_body_stmts(td.name, td.type)
            else
              result = lower(td)
              result.is_a?(CppAst::Nodes::Program) ? result.statements : [result]
            end
          end

          # Phase 3: forward declarations for all non-generic user functions.
          # Must come after all type definitions so parameter/return types are known.
          # Uses the same modifiers as the actual definition to avoid redeclaration mismatch.
          func_protos = module_node.items.grep(SemanticIR::Func).flat_map do |func|
            next [] if func.type_params.any?
            next [] if func.body.nil? # extern - no forward decl needed
            [function_forward_decl(func)]
          end

          # Phase 4: full function bodies (non-generic functions only; generics handled inline).
          func_bodies = module_node.items.grep(SemanticIR::Func).flat_map do |func|
            result = lower(func)
            result.is_a?(CppAst::Nodes::Program) ? result.statements : [result]
          end

          # Phase 0: trait vtable structs (before type defs, so param types can reference them)
          trait_structs = generate_trait_structs

          # Phase 3.5: adapter functions for trait implementations
          trait_adapters = generate_trait_adapters

          all_stmts = include_stmts + trait_structs + sum_preambles + type_defs + func_protos + trait_adapters + func_bodies
          CppAst::Nodes::Program.new(
            statements: all_stmts,
            statement_trailings: Array.new(all_stmts.size, "\n")
          )
        end

        # Generate a forward declaration (prototype) for a function using the same
        # modifiers (constexpr, noexcept) that the actual definition will have.
        def generate_trait_structs
          return [] unless @trait_registry
          @trait_registry.all_traits.map do |trait_info|
            fields = trait_info.trait_methods
                               .reject { |m| m[:name] == "self" }
                               .map do |m|
              params = Array(m[:params]).reject { |p| p.respond_to?(:name) ? p.name == "self" : p[:name] == "self" }
              param_types = params.map do |p|
                t = p.respond_to?(:type) ? p.type : p[:type]
                t ? @context.map_type(t) : "auto"
              end.join(", ")
              ret_type = m[:ret_type] ? @context.map_type(m[:ret_type]) : "void"
              "  std::function<#{ret_type}(#{param_types})> #{m[:name]};"
            end.join("\n")
            @context.factory.raw_statement(code: "struct #{trait_info.name} {\n#{fields}\n};")
          end
        end

        def generate_trait_adapters
          return [] unless @trait_registry
          adapters = []
          @trait_registry.all_traits.each do |trait_info|
            trait_name = trait_info.name
            @trait_registry.implementations_for_trait(trait_name).each do |impl|
              type_name = impl.type_name
              cpp_type = @context.type_registry&.cpp_name(type_name) || type_name
              field_inits = trait_info.trait_methods
                                      .reject { |m| m[:name] == "self" }
                                      .map do |m|
                method_name = m[:name]
                params = Array(m[:params]).reject { |p| p.respond_to?(:name) ? p.name == "self" : p[:name] == "self" }
                param_decls = params.each_with_index.map do |p, i|
                  t = p.respond_to?(:type) ? p.type : p[:type]
                  "#{t ? @context.map_type(t) : "auto"} _p#{i}"
                end.join(", ")
                param_names = params.each_with_index.map { |_, i| "_p#{i}" }.join(", ")
                sep = param_names.empty? ? "" : ", "
                "  .#{method_name} = [self](#{param_decls}) noexcept { return #{type_name}_#{method_name}(self#{sep}#{param_names}); }"
              end.join(",\n")
              adapter_code = "inline #{trait_name} #{type_name}_as_#{trait_name}(#{cpp_type} self) noexcept {\n  return #{trait_name}{\n#{field_inits}\n  };\n}"
              adapters << @context.factory.raw_statement(code: adapter_code)
            end
          end
          adapters
        end

        def function_forward_decl(func)
          return_type = @context.map_type(func.ret_type)
          name = @context.sanitize_identifier(func.name)
          is_main = name == "main" && func.params.empty?
          parameters = if is_main
                         ["int argc", "char** argv"]
                       else
                         func.params.map do |p|
                           "#{@context.map_type(p.type)} #{@context.sanitize_identifier(p.name)}"
                         end
                       end
          proto = CppAst::Nodes::FunctionDeclaration.new(
            return_type: return_type,
            name: name,
            parameters: parameters,
            body: nil,
            return_type_suffix: " ",
            lparen_suffix: "",
            rparen_suffix: "",
            param_separators: parameters.size > 1 ? Array.new(parameters.size - 1, ", ") : [],
            modifiers_text: "",
            prefix_modifiers: ""
          )
          function_rule = MLC::Backends::Cpp::Rules::FunctionRule.new(@context)
          function_rule.apply(proto, semantic_func: func, event_bus: @container.event_bus)
        end

        # Forward declarations + using alias for a sum type (no full struct bodies).
        def sum_type_preamble_stmts(name, sum_type)
          fwd_decls = sum_type.variants.map do |v|
            @context.factory.raw_statement(code: "struct #{v[:name]};")
          end
          variant_names = sum_type.variants.map { |v| v[:name] }.join(", ")
          using_decl = CppAst::Nodes::UsingDeclaration.new(
            kind: :alias,
            name: name,
            alias_target: "std::variant<#{variant_names}>",
            using_suffix: " ",
            equals_prefix: " ",
            equals_suffix: " "
          )
          fwd_decls + [using_decl]
        end

        # Full struct definitions for sum type variants only (no fwd decls or using).
        def sum_type_body_stmts(name, sum_type)
          sum_type.variants.map do |variant|
            if variant[:fields].empty?
              CppAst::Nodes::StructDeclaration.new(
                name: variant[:name], members: [], member_trailings: [],
                struct_suffix: " ", name_suffix: " ",
                lbrace_suffix: "", rbrace_suffix: "", base_classes_text: ""
              )
            else
              members = variant[:fields].map do |field|
                CppAst::Nodes::VariableDeclaration.new(
                  type: @context.map_type(field[:type]),
                  declarators: [field[:name]],
                  declarator_separators: [],
                  type_suffix: " ",
                  prefix_modifiers: ""
                )
              end
              CppAst::Nodes::StructDeclaration.new(
                name: variant[:name], members: members,
                member_trailings: Array.new(members.size, ""),
                struct_suffix: " ", name_suffix: " ",
                lbrace_suffix: "", rbrace_suffix: "", base_classes_text: ""
              )
            end
          end
        end

        # Helper: Lower block expression statements with optional return
        def lower_block_expr_statements(block_expr, emit_return: true)
          statements = block_expr.statements.flat_map { |stmt| Array(@context.lower_statement(stmt)) }

          # Skip unit literals - they represent void/no value
          if block_expr.result && !block_expr.result.is_a?(SemanticIR::UnitLiteral)
            result_expr = @context.lower_expression(block_expr.result)
            statements << if emit_return
                            CppAst::Nodes::ReturnStatement.new(expression: result_expr)
                          else
                            CppAst::Nodes::ExpressionStatement.new(expression: result_expr)
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

        # Type declaration lowering
        def lower_type_decl_internal(type_decl)
          result = case type_decl.type
                   when SemanticIR::RecordType
                     lower_record_type_internal(type_decl.name, type_decl.type)
                   when SemanticIR::SumType
                     lower_sum_type_internal(type_decl.name, type_decl.type, type_decl.type_params)
                   when SemanticIR::OpaqueType
                     # Opaque types: no C++ emission
                     CppAst::Nodes::Program.new(statements: [], statement_trailings: [])
                   else
                     # Type alias: using Name = CppType;
                     lower_type_alias_internal(type_decl.name, type_decl.type)
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

        def lower_type_alias_internal(name, aliased_type)
          cpp_type = @context.map_type(aliased_type)
          @context.factory.raw_statement(code: "using #{name} = #{cpp_type};")
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

        def sum_type_recursive?(name, sum_type)
          sum_type.variants.any? do |v|
            v[:fields].any? { |f| field_type_references?(f[:type], name) }
          end
        end

        def field_type_references?(type, name)
          case type
          when SemanticIR::GenericType
            type.type_args.any? { |a| field_type_references?(a, name) }
          when SemanticIR::ArrayType
            field_type_references?(type.element_type, name)
          when SemanticIR::Type
            type.name == name
          else
            false
          end
        end

        def cyclic_sum_type_stmts(name, sum_type)
          variant_structs = sum_type_body_stmts(name, sum_type)
          wrapper = lower_sum_type_as_wrapper_struct(name, sum_type)
          stmts = variant_structs + [wrapper]
          CppAst::Nodes::Program.new(
            statements: stmts,
            statement_trailings: Array.new(stmts.size, "\n")
          )
        end

        # Output sum type as wrapper struct (for forward decl in modular headers with cycles)
        def lower_sum_type_as_wrapper_struct(name, sum_type)
          variant_names = sum_type.variants.map { |v| v[:name] }.join(", ")
          variant_type = "std::variant<#{variant_names}>"
          member = CppAst::Nodes::VariableDeclaration.new(
            type: variant_type,
            declarators: ["_"],
            declarator_separators: [],
            type_suffix: " ",
            prefix_modifiers: ""
          )
          CppAst::Nodes::StructDeclaration.new(
            name: name,
            members: [member],
            member_trailings: [""],
            struct_suffix: " ",
            name_suffix: " ",
            lbrace_suffix: "",
            rbrace_suffix: "",
            base_classes_text: ""
          )
        end

        def lower_sum_type_internal(name, sum_type, type_params = [])
          if @cyclic_sum_types&.include?(name)
            return cyclic_sum_type_stmts(name, sum_type)
          end

          recursive = sum_type_recursive?(name, sum_type)

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
                                 type_params_str = type_params.map(&:name).join(", ")
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

          if recursive
            # For recursive types: forward-declare structs, emit using alias,
            # then full struct definitions so that std::shared_ptr<SumType> compiles.
            fwd_decls = sum_type.variants.map do |v|
              @context.factory.raw_statement(code: "struct #{v[:name]};")
            end
            all_statements = fwd_decls + [using_decl] + variant_structs
          else
            all_statements = variant_structs + [using_decl]
          end

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
# rubocop:enable Metrics/ParameterLists
