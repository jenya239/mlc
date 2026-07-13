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
        # When include_default_values: append " = <expr>" for parameters with a lowered default.
        # Non-generic: forward decl uses true; the single definition uses false. Template functions
        # have no separate forward decl, so the definition must include defaults (true when generic).
        def build_function_parameters_cpp(func, include_default_values:)
          name = @context.sanitize_identifier(func.name)
          is_main = name == "main" && func.params.empty?
          if is_main
            return ["int argc", "char** argv"]
          end

          func.params.map do |param|
            cpp_type = if param.respond_to?(:template_type_name) && param.template_type_name
                         param.template_type_name
                       else
                         @context.map_type(param.type)
                       end
            if param.respond_to?(:mutable) && param.mutable && !param.type.is_a?(SemanticIR::MutRefType)
              cpp_type = "#{cpp_type}&"
            end
            base = "#{cpp_type} #{@context.sanitize_identifier(param.name)}"
            if include_default_values && param.default
              base + " = " + @context.lower_expression(param.default).to_source
            else
              base
            end
          end
        end

        def lower_function(func)
          # Reset variable declaration tracking for each new function scope
          @context.reset_declared_variables
          @context.init_var_type_map_from_params(func.params)

          return_type = @context.map_type(func.ret_type)
          name = @context.sanitize_identifier(func.name)
          # Single template definition is the only declaration — keep default values there
          include_defaults = (func.respond_to?(:generic?) ? func.generic? : func.type_params.any?)
          parameters = build_function_parameters_cpp(func, include_default_values: include_defaults)

          # Set flag for generic function context
          @in_generic_function = (func.respond_to?(:generic?) ? func.generic? : func.type_params.any?)
          @container.instance_variable_set(:@in_generic_function, @in_generic_function)

          # For main(), inject argc/argv and set_args preamble
          is_main = name == "main" && func.params.empty?
          if is_main
            parameters = ["int argc", "char** argv"]
          end

          # Lower function body using new architecture
          block_body = if func.body.nil? && func.respond_to?(:extern_c_name) && func.extern_c_name
                         # FFI binding: thin wrapper that calls the C symbol.
                         # Capture C address before defining the MLC wrapper (avoids recursion when names match).
                         param_names = func.params.map { |parameter| @context.sanitize_identifier(parameter.name) }
                         call_arguments = param_names.join(", ")
                         c_name = func.extern_c_name.to_s
                         return_mapped = @context.map_type(func.ret_type).to_s
                         param_types = func.params.map { |parameter| @context.map_type(parameter.type).to_s }
                         signature_params = param_types.join(", ")
                         binder_name = "mlc_ffi_bind_#{@context.sanitize_identifier(func.name)}"
                         binder_code = "static auto const #{binder_name} = "                                        "static_cast<#{return_mapped}(*)(#{signature_params})>(&::#{c_name});"
                         call_expression = if call_arguments.empty?
                                             "#{binder_name}()"
                                           else
                                             "#{binder_name}(#{call_arguments})"
                                           end
                         body_code = if return_mapped == "void" || return_mapped.empty?
                                       "#{call_expression};"
                                     else
                                       "return #{call_expression};"
                                     end
                         CppAst::Nodes::BlockStatement.new(
                           statements: [
                             @context.factory.raw_statement(code: binder_code),
                             @context.factory.raw_statement(code: body_code)
                           ],
                           statement_trailings: ["
", "
"],
                           lbrace_suffix: "
",
                           rbrace_prefix: ""
                         )
                       elsif func.body.nil?
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
                       elsif empty_unit_function_body?(func.body, func.ret_type)
                         CppAst::Nodes::BlockStatement.new(
                           statements: [],
                           statement_trailings: [],
                           lbrace_suffix: "\n",
                           rbrace_prefix: ""
                         )
                       else
                         @container.expected_return_type = func.ret_type
                         if @context.should_lower_as_statement?(func.body)
                           statement_body = @context.lower_statement(func.body)
                           CppAst::Nodes::BlockStatement.new(
                             statements: [statement_body],
                             statement_trailings: ["\n"],
                             lbrace_suffix: "\n",
                             rbrace_prefix: ""
                           )
                         else
                           body_expr = @context.lower_expression(func.body)
                           return_statement = CppAst::Nodes::ReturnStatement.new(expression: body_expr)
                           return_statement = @context.attach_line_directive(return_statement, func.body)
                           CppAst::Nodes::BlockStatement.new(
                             statements: [return_statement],
                             statement_trailings: [""],
                             lbrace_suffix: "",
                             rbrace_prefix: ""
                           )
                         end
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
          if (func.respond_to?(:generic?) ? func.generic? : func.type_params.any?)
            generate_template_function((func.respond_to?(:all_type_params) ? func.all_type_params : func.type_params), func_decl)
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
            CppAst::Nodes::IncludeDirective.new(path: "mlc/core/arith.hpp", system: false),
            CppAst::Nodes::IncludeDirective.new(path: "mlc/core/int_arith.hpp", system: false),
            CppAst::Nodes::IncludeDirective.new(path: "mlc/io/file.hpp", system: false),
            CppAst::Nodes::IncludeDirective.new(path: "mlc/net/tcp_bridge.hpp", system: false),
            CppAst::Nodes::IncludeDirective.new(path: "mlc/net/websocket_bridge.hpp", system: false),
            CppAst::Nodes::IncludeDirective.new(path: "mlc/db/postgres_bridge.hpp", system: false),
            CppAst::Nodes::IncludeDirective.new(path: "mlc/crypto/sodium_bridge.hpp", system: false),
            CppAst::Nodes::IncludeDirective.new(path: "mlc/env/env_abi.hpp", system: false),
            CppAst::Nodes::IncludeDirective.new(path: "mlc/core/profile.hpp", system: false),
            CppAst::Nodes::IncludeDirective.new(path: "mlc/core/match.hpp", system: false),
            CppAst::Nodes::IncludeDirective.new(path: "mlc/core/result.hpp", system: false),
            CppAst::Nodes::IncludeDirective.new(path: "mlc/json/json.hpp", system: false),
            CppAst::Nodes::IncludeDirective.new(path: "mlc/core/result_combinators.hpp", system: false),
            CppAst::Nodes::IncludeDirective.new(path: "mlc/core/optional_combinators.hpp", system: false)
          ]
          ffi_headers = module_node.items.grep(SemanticIR::Func).filter_map do |function|
            next unless function.respond_to?(:extern_header) && function.extern_header
            header_path = function.extern_header.to_s
            system_include = header_path.start_with?("<") && header_path.end_with?(">")
            cleaned = system_include ? header_path[1...-1] : header_path.delete_prefix('"').delete_suffix('"')
            CppAst::Nodes::IncludeDirective.new(path: cleaned, system: system_include)
          end.uniq { |directive| [directive.path, directive.system] }
          include_stmts.concat(ffi_headers)

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
              stmts = sum_type_body_stmts(td.name, td.type)
              derive_traits = Array(td.derive_traits)
              stmts + (derive_traits.empty? ? [] : generate_derive_methods(td.name, td.type, derive_traits))
            else
              result = lower(td)
              result.is_a?(CppAst::Nodes::Program) ? result.statements : [result]
            end
          end

          # Phase 3: forward declarations for all non-generic user functions.
          # Must come after all type definitions so parameter/return types are known.
          # Uses the same modifiers as the actual definition to avoid redeclaration mismatch.
          func_protos = module_node.items.grep(SemanticIR::Func).flat_map do |func|
            next [] if (func.respond_to?(:generic?) ? func.generic? : func.type_params.any?)
            next [] if func.body.nil? && !(func.respond_to?(:extern_c_name) && func.extern_c_name) # bare extern - no forward decl
            [function_forward_decl(func)]
          end

          # Phase 4: full function bodies (non-generic functions only; generics handled inline).
          func_bodies = module_node.items.grep(SemanticIR::Func).flat_map do |func|
            result = lower(func)
            result.is_a?(CppAst::Nodes::Program) ? result.statements : [result]
          end

          # Phase 0: trait concepts (C++20) or vtable structs
          trait_concepts = generate_trait_concept_declarations
          trait_extend_suffix = generate_trait_extend_concept_suffix
          trait_structs = generate_trait_structs

          # Phase 3.5: adapter functions for trait implementations
          trait_adapters = generate_trait_adapters

          all_stmts = include_stmts + trait_concepts + trait_extend_suffix + trait_structs + sum_preambles + type_defs + func_protos + trait_adapters + func_bodies
          CppAst::Nodes::Program.new(
            statements: all_stmts,
            statement_trailings: Array.new(all_stmts.size, "\n")
          )
        end

        # Generate a forward declaration (prototype) for a function using the same
        # modifiers (constexpr, noexcept) that the actual definition will have.
        def generate_trait_concept_declarations
          return [] unless @trait_registry

          lines = @trait_registry.all_traits.flat_map do |trait_info|
            TraitConceptCodegen.generate_trait_concept_header_lines(trait_info)
          end
          lines.map { |code| @context.factory.raw_statement(code: code) }
        end

        def generate_trait_extend_concept_suffix
          return [] unless @trait_registry

          type_registry = @context.type_registry
          header_lines = []
          implementation_lines = []
          @trait_registry.all_traits.each do |trait_info|
            next unless TraitConceptCodegen.concept_trait?(trait_info)

            @trait_registry.implementations_for_trait(trait_info.name).each do |implementation|
              header_lines.concat(
                TraitConceptCodegen.generate_extend_trait_suffix_header_lines(
                  trait_info, implementation, type_registry
                )
              )
              implementation_lines.concat(
                TraitConceptCodegen.generate_extern_extend_implementation_lines(trait_info, implementation)
              )
            end
          end
          (header_lines + implementation_lines).map { |code| @context.factory.raw_statement(code: code) }
        end

        def generate_trait_structs
          return [] unless @trait_registry
          @trait_registry.all_traits.filter_map do |trait_info|
            next if TraitConceptCodegen.concept_trait?(trait_info)

            uses_associated_types = trait_info.associated_types&.any?
            associated_type_names = trait_info.associated_types&.map { |entry| entry[:name].to_s }&.to_set
            trait_self_name = "TraitSelf"
            trait_self_keyword = uses_associated_types ? trait_self_name : nil
            fields = trait_info.trait_methods
                               .reject { |m| m[:name] == "self" }
                               .map do |m|
              param_types = TraitCppTypes.vtable_std_function_parameter_types(
                m,
                trait_self_name: trait_self_name,
                trait_self_keyword: trait_self_keyword,
                associated_type_names: associated_type_names
              )
              ret_syntax = m[:ret_type]
              ret_type = ret_syntax ? TraitCppTypes.ast_type_to_cpp(ret_syntax, trait_self: trait_self_keyword, associated_type_names: associated_type_names) : "void"
              "  std::function<#{ret_type}(#{param_types})> #{m[:name]};"
            end.join("\n")
            struct_body = if uses_associated_types
                            "template<typename #{trait_self_name}>\nstruct #{trait_info.name} {\n#{fields}\n};"
                          else
                            "struct #{trait_info.name} {\n#{fields}\n};"
                          end
            @context.factory.raw_statement(code: struct_body)
          end
        end

        def generate_trait_adapters
          return [] unless @trait_registry
          adapters = []
          @trait_registry.all_traits.each do |trait_info|
            trait_name = trait_info.name
            uses_associated_types = trait_info.associated_types&.any?
            next if Array(trait_info.type_params).map(&:to_s).any?
            next if TraitConceptCodegen.concept_trait?(trait_info)

            @trait_registry.implementations_for_trait(trait_name).each do |impl|
              type_name = impl.type_name
              cpp_type = @context.type_registry&.cpp_name(type_name) || type_name
              trait_cpp_name = uses_associated_types ? "#{trait_name}<#{cpp_type}>" : trait_name
              field_inits = trait_info.trait_methods
                                      .reject { |m| m[:name] == "self" }
                                      .map do |trait_method_entry|
                method_name = trait_method_entry[:name]
                impl_method = impl.impl_methods[method_name]
                implementation_parameters = Array(impl_method.params)
                first_trait_parameter = Array(trait_method_entry[:params]).first
                first_trait_type = first_trait_parameter&.respond_to?(:type) ? first_trait_parameter.type : first_trait_parameter&.[](:type)
                trait_has_self_receiver = first_trait_type.is_a?(MLC::Source::AST::Type) && first_trait_type.name == "Self"
                implementation_parameters = implementation_parameters.drop(1) if trait_has_self_receiver
                param_decls = implementation_parameters.each_with_index.map do |parameter, index|
                  TraitCppTypes.ast_type_to_cpp(parameter.type) + " _p#{index}"
                end.join(", ")
                param_names = implementation_parameters.each_with_index.map { |_, index| "_p#{index}" }.join(", ")
                ret_cpp = TraitCppTypes.ast_type_to_cpp(impl_method.ret_type)
                statement_prefix = ret_cpp == "void" ? "" : "return "
                if trait_has_self_receiver && uses_associated_types
                  receiver_declaration = "#{cpp_type} receiver"
                  extra_declarations = implementation_parameters.each_with_index.map do |parameter, index|
                    "#{TraitCppTypes.ast_type_to_cpp(parameter.type)} _p#{index}"
                  end
                  lambda_parameter_list = ([receiver_declaration] + extra_declarations).join(", ")
                  call_arguments = (["receiver"] + implementation_parameters.each_with_index.map { |_, index| "_p#{index}" }).join(", ")
                  "  .#{method_name} = [](#{lambda_parameter_list}) noexcept { #{statement_prefix}#{type_name}_#{method_name}(#{call_arguments}); }"
                elsif trait_has_self_receiver
                  capture_call_arguments = (["self"] + implementation_parameters.each_with_index.map { |_, index| "_p#{index}" }).join(", ")
                  "  .#{method_name} = [self](#{param_decls}) noexcept { #{statement_prefix}#{type_name}_#{method_name}(#{capture_call_arguments}); }"
                else
                  "  .#{method_name} = [](#{param_decls}) noexcept { #{statement_prefix}#{type_name}_#{method_name}(#{param_names}); }"
                end
              end.join(",\n")
              adapter_code = "inline #{trait_cpp_name} #{type_name}_as_#{trait_name}(#{cpp_type} self) noexcept {\n  return #{trait_cpp_name}{\n#{field_inits}\n  };\n}"
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
                         build_function_parameters_cpp(func, include_default_values: true)
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
            if @context.should_lower_as_statement?(block_expr.result)
              statements << @context.lower_statement(block_expr.result)
            else
              result_expr = @context.lower_expression(block_expr.result)
              result_statement = if emit_return
                                   CppAst::Nodes::ReturnStatement.new(expression: result_expr)
                                 else
                                   CppAst::Nodes::ExpressionStatement.new(expression: result_expr)
                                 end
              statements << @context.attach_line_directive(result_statement, block_expr.result)
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

          derive_traits = Array(type_decl.derive_traits)
          unless derive_traits.empty?
            derive_stmts = generate_derive_methods(type_decl.name, type_decl.type, derive_traits)
            unless derive_stmts.empty?
              base_stmts = result.is_a?(CppAst::Nodes::Program) ? result.statements : [result]
              all_stmts = base_stmts + derive_stmts
              result = CppAst::Nodes::Program.new(
                statements: all_stmts,
                statement_trailings: Array.new(all_stmts.size, "\n")
              )
            end
          end

          if type_decl.type_params.any?
            if result.is_a?(CppAst::Nodes::Program) && type_decl.type.is_a?(SemanticIR::SumType)
              # Per-variant templates already applied inside lower_sum_type_internal.
              result
            elsif result.is_a?(CppAst::Nodes::Program)
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

        # Generate C++ free functions for derived traits on a type.
        # Returns array of raw_statement nodes.
        def generate_derive_methods(name, type, traits)
          stmts = []
          traits.each do |trait|
            case trait
            when "Display"
              stmts << generate_derive_display(name, type)
            when "Eq"
              stmts << generate_derive_eq(name, type)
            when "Ord"
              stmts << generate_derive_ord(name, type)
            when "Hash"
              stmts << generate_derive_hash(name, type)
            when "Json"
              stmts << generate_derive_json(name, type)
            end
          end
          stmts.compact
        end

        def derive_field_to_string(field_name, field_type)
          case field_type
          when SemanticIR::Type
            if field_type.kind == :prim && field_type.name == "string"
              "self.#{field_name}"
            else
              "mlc::to_string(self.#{field_name})"
            end
          else
            "mlc::to_string(self.#{field_name})"
          end
        end

        def generate_derive_display(name, type)
          body = case type
                 when SemanticIR::RecordType
                   parts = type.fields.map do |field|
                     fn = field[:name]
                     ft = field[:type]
                     "mlc::String(\", #{fn}: \") + #{derive_field_to_string(fn, ft)}"
                   end
                   if parts.empty?
                     "mlc::String(\"#{name} {}\")"
                   else
                     first_field = type.fields.first[:name]
                     first_str = derive_field_to_string(first_field, type.fields.first[:type])
                     rest = type.fields.drop(1).map do |field|
                       fn = field[:name]
                       "mlc::String(\", #{fn}: \") + #{derive_field_to_string(fn, field[:type])}"
                     end
                     all_parts = ["mlc::String(\"#{name} { #{first_field}: \") + #{first_str}"] + rest + ["mlc::String(\" }\")"]
                     all_parts.join(" + ")
                   end
                 when SemanticIR::SumType
                   variant_cases = type.variants.map do |variant|
                     vname = variant[:name]
                     fields = variant[:fields]
                     if fields.empty?
                       "if (std::holds_alternative<#{vname}>(self._)) return mlc::String(\"#{vname}\");"
                     elsif fields.length == 1 && !fields.first[:name]
                       "if (std::holds_alternative<#{vname}>(self._)) return mlc::String(\"#{vname}(\") + mlc::to_string(std::get<#{vname}>(self._)._0) + mlc::String(\")\");"
                     else
                       field_strs = fields.map { |f| "mlc::to_string(std::get<#{vname}>(self._).#{f[:name]})" }.join(" + mlc::String(\", \") + ")
                       "if (std::holds_alternative<#{vname}>(self._)) return mlc::String(\"#{vname}(\") + #{field_strs} + mlc::String(\")\");"
                     end
                   end
                   variant_cases.join("\n  ") + "\n  return mlc::String(\"\");"
                 else
                   return nil
                 end

          code = "mlc::String #{name}_to_string(const #{name}& self) noexcept {\n  #{body}\n}"
          @context.factory.raw_statement(code: code)
        end

        def generate_derive_eq(name, type)
          body = case type
                 when SemanticIR::RecordType
                   if type.fields.empty?
                     "return true;"
                   else
                     conditions = type.fields.map { |f| "a.#{f[:name]} == b.#{f[:name]}" }.join(" && ")
                     "return #{conditions};"
                   end
                 when SemanticIR::SumType
                   "return a._ == b._;"
                 else
                   return nil
                 end

          code = "bool operator==(const #{name}& a, const #{name}& b) noexcept { #{body} }"
          @context.factory.raw_statement(code: code)
        end

        def generate_derive_ord(name, type)
          body = case type
                 when SemanticIR::RecordType
                   if type.fields.empty?
                     "return false;"
                   else
                     conditions = type.fields.map { |f| "a.#{f[:name]} < b.#{f[:name]}" }.join(" || (a.#{type.fields.first[:name]} == b.#{type.fields.first[:name]} && ")
                     field_parts = type.fields.each_with_index.map do |f, i|
                       if i == 0
                         "a.#{f[:name]} < b.#{f[:name]}"
                       else
                         prev_eq = type.fields[0...i].map { |pf| "a.#{pf[:name]} == b.#{pf[:name]}" }.join(" && ")
                         "(#{prev_eq} && a.#{f[:name]} < b.#{f[:name]})"
                       end
                     end
                     "return #{field_parts.join(" || ")};"
                   end
                 when SemanticIR::SumType
                   "return a._ < b._;"
                 else
                   return nil
                 end

          code = "bool operator<(const #{name}& a, const #{name}& b) noexcept { #{body} }"
          @context.factory.raw_statement(code: code)
        end


        def generate_derive_json(name, type)
  case type
  when SemanticIR::RecordType
    to_json_body = derive_json_to_json_body(type)
    from_json_body = derive_json_from_json_body(name, type)
  when SemanticIR::SumType
    to_json_body = derive_json_sum_to_json_body(name, type)
    from_json_body = derive_json_sum_from_json_body(name, type)
  else
    return nil
  end

  code = <<~CPP.strip
    mlc::json::JsonValue #{name}_to_json(const #{name}& self) noexcept {
      #{to_json_body}
    }
    mlc::result::Result<#{name}, mlc::json::JsonError> #{name}_from_json(const mlc::json::JsonValue& value) noexcept {
      #{from_json_body}
    }
  CPP
  @context.factory.raw_statement(code: code)
end

def derive_json_sum_access(name)
  if @cyclic_sum_types&.include?(name)
    "self._"
  else
    "self"
  end
end

def derive_json_sum_to_json_body(name, type)
  access = derive_json_sum_access(name)
  lines = []
  type.variants.each do |variant|
    vname = variant[:name]
    fields = Array(variant[:fields])
    lines << "if (std::holds_alternative<#{vname}>(#{access})) {"
    if fields.empty?
      lines << "  return mlc::json::json_string(mlc::String(\"#{vname}\"));"
    elsif fields.length == 1
      field = fields.first
      field_access = "std::get<#{vname}>(#{access}).#{field[:name] || "field0"}"
      encoded = derive_json_encode_field(field_access, field[:type])
      lines << "  mlc::json::JsonValue object = mlc::json::json_object();"
      lines << "  object = mlc::json::json_set(object, mlc::String(\"tag\"), mlc::json::json_string(mlc::String(\"#{vname}\")));"
      lines << "  object = mlc::json::json_set(object, mlc::String(\"value\"), #{encoded});"
      lines << "  return object;"
    else
      lines << "  mlc::json::JsonValue object = mlc::json::json_object();"
      lines << "  object = mlc::json::json_set(object, mlc::String(\"tag\"), mlc::json::json_string(mlc::String(\"#{vname}\")));"
      lines << "  std::vector<mlc::json::JsonValue> fields;"
      fields.each do |field|
        field_access = "std::get<#{vname}>(#{access}).#{field[:name] || "field0"}"
        encoded = derive_json_encode_field(field_access, field[:type])
        lines << "  fields.push_back(#{encoded});"
      end
      lines << "  object = mlc::json::json_set(object, mlc::String(\"fields\"), mlc::json::json_array(fields));"
      lines << "  return object;"
    end
    lines << "}"
  end
  lines << "return mlc::json::json_null();"
  lines.join("\n  ")
end

def derive_json_sum_from_json_body(name, type)
  lines = []
  lines << "if (value.is_string()) {"
  lines << "  mlc::String tag = *value.as_string();"
  type.variants.each do |variant|
    next unless Array(variant[:fields]).empty?

    vname = variant[:name]
    lines << "  if (tag == mlc::String(\"#{vname}\")) {"
    lines << "    return mlc::result::Ok<#{name}>(#{vname}{});"
    lines << "  }"
  end
  lines << "  return mlc::result::Err<mlc::json::JsonError>(mlc::json::json_type_mismatch(mlc::String(\"tag\"), mlc::String(\"known unit variant\")));"
  lines << "}"
  lines << "if (!value.is_object()) {"
  lines << "  return mlc::result::Err<mlc::json::JsonError>(mlc::json::json_type_mismatch(mlc::String(\"\"), mlc::String(\"object or string\")));"
  lines << "}"
  lines << "auto __opt_tag = mlc::json::json_get(value, mlc::String(\"tag\"));"
  lines << "if (!__opt_tag.has_value() || !__opt_tag->is_string()) {"
  lines << "  return mlc::result::Err<mlc::json::JsonError>(mlc::json::json_missing_field(mlc::String(\"tag\")));"
  lines << "}"
  lines << "mlc::String tag = *__opt_tag->as_string();"
  type.variants.each do |variant|
    vname = variant[:name]
    fields = Array(variant[:fields])
    lines << "if (tag == mlc::String(\"#{vname}\")) {"
    if fields.empty?
      lines << "  return mlc::result::Ok<#{name}>(#{vname}{});"
    elsif fields.length == 1
      field = fields.first
      field_name = field[:name] || "field0"
      lines << "  auto __opt_value = mlc::json::json_get(value, mlc::String(\"value\"));"
      lines << "  if (!__opt_value.has_value()) {"
      lines << "    return mlc::result::Err<mlc::json::JsonError>(mlc::json::json_missing_field(mlc::String(\"value\")));"
      lines << "  }"
      lines.concat(derive_json_extract_required("__opt_value.value()", "#{vname}_payload", field[:type], "  "))
      lines << "  return mlc::result::Ok<#{name}>(#{vname}{.#{field_name} = __decoded_#{vname}_payload});"
    else
      lines << "  auto __opt_fields = mlc::json::json_get(value, mlc::String(\"fields\"));"
      lines << "  if (!__opt_fields.has_value() || !__opt_fields->is_array()) {"
      lines << "    return mlc::result::Err<mlc::json::JsonError>(mlc::json::json_type_mismatch(mlc::String(\"fields\"), mlc::String(\"array\")));"
      lines << "  }"
      lines << "  auto __arr_fields = *__opt_fields->as_array();"
      lines << "  if (__arr_fields.size() != #{fields.length}) {"
      lines << "    return mlc::result::Err<mlc::json::JsonError>(mlc::json::json_type_mismatch(mlc::String(\"fields\"), mlc::String(\"array length #{fields.length}\")));"
      lines << "  }"
      fields.each_with_index do |field, index|
        field_name = field[:name] || "field#{index}"
        lines.concat(derive_json_extract_required("__arr_fields[#{index}]", "#{vname}_#{field_name}", field[:type], "  "))
      end
      inits = fields.each_with_index.map do |field, index|
        field_name = field[:name] || "field#{index}"
        ".#{field_name} = __decoded_#{vname}_#{field_name}"
      end.join(", ")
      lines << "  return mlc::result::Ok<#{name}>(#{vname}{#{inits}});"
    end
    lines << "}"
  end
  lines << "return mlc::result::Err<mlc::json::JsonError>(mlc::json::json_type_mismatch(mlc::String(\"tag\"), mlc::String(\"known variant\")));"
  lines.join("\n  ")
end


        def derive_json_to_json_body(type)
          lines = ["mlc::json::JsonValue object = mlc::json::json_object();"]
          type.fields.each do |field|
            field_name = field[:name]
            encoded = derive_json_encode_field("self.#{field_name}", field[:type])
            lines << "object = mlc::json::json_set(object, mlc::String(\"#{field_name}\"), #{encoded});"
          end
          lines << "return object;"
          lines.join("\n  ")
        end

        def derive_json_from_json_body(name, type)
          lines = []
          lines << "if (!value.is_object()) {"
          lines << "  return mlc::result::Err<mlc::json::JsonError>(mlc::json::json_type_mismatch(mlc::String(\"\"), mlc::String(\"object\")));"
          lines << "}"
          type.fields.each do |field|
            field_name = field[:name]
            lines.concat(derive_json_decode_field(field_name, field[:type]))
          end
          field_inits = type.fields.map { |field| ".#{field[:name]} = #{field[:name]}" }.join(", ")
          lines << "return mlc::result::Ok<#{name}>(#{name}{#{field_inits}});"
          lines.join("\n  ")
        end

                def derive_json_encode_field(access, field_type)
          case field_type
          when SemanticIR::ArrayType
            element = field_type.element_type
            encoded_item = derive_json_encode_field("item", element)
            "[&]() { std::vector<mlc::json::JsonValue> items; items.reserve(#{access}.size()); for (const auto& item : #{access}) { items.push_back(#{encoded_item}); } return mlc::json::json_array(items); }()"
          when SemanticIR::GenericType
            base = field_type.base_type.respond_to?(:name) ? field_type.base_type.name : nil
            if base == "Option"
              inner = field_type.type_args.first
              encoded_inner = derive_json_encode_field("(*#{access})", inner)
              "(#{access}.has_value() ? #{encoded_inner} : mlc::json::json_null())"
            else
              "mlc::json::json_null()"
            end
          when SemanticIR::Type
            case field_type.name
            when "string", "str"
              "mlc::json::json_string(#{access})"
            when "bool"
              "mlc::json::json_bool(#{access})"
            when "i8", "i16", "i32", "i64", "u8", "u16", "u32", "u64", "usize", "f32", "f64"
              "mlc::json::json_number(static_cast<double>(#{access}))"
            else
              "mlc::json::json_string(mlc::to_string(#{access}))"
            end
          else
            "mlc::json::json_null()"
          end
        end


        def derive_json_decode_field(field_name, field_type)
          lines = []
          if field_type.is_a?(SemanticIR::GenericType)
            base = field_type.base_type.respond_to?(:name) ? field_type.base_type.name : nil
            if base == "Option"
              inner = field_type.type_args.first
              cpp_inner = derive_json_cpp_type(inner)
              lines << "std::optional<#{cpp_inner}> #{field_name} = std::nullopt;"
              lines << "{"
              lines << "  auto __opt_#{field_name} = mlc::json::json_get(value, mlc::String(\"#{field_name}\"));"
              lines << "  if (__opt_#{field_name}.has_value() && !__opt_#{field_name}->is_null()) {"
              derive_json_extract_required("__opt_#{field_name}.value()", field_name, inner, "    ").each do |line|
                lines << "  #{line}"
              end
              lines << "    #{field_name} = __decoded_#{field_name};"
              lines << "  }"
              lines << "}"
              return lines
            end
          end

          cpp_type = derive_json_cpp_type(field_type)
          lines << "#{cpp_type} #{field_name};"
          lines << "{"
          lines << "  auto __opt_#{field_name} = mlc::json::json_get(value, mlc::String(\"#{field_name}\"));"
          lines << "  if (!__opt_#{field_name}.has_value()) {"
          lines << "    return mlc::result::Err<mlc::json::JsonError>(mlc::json::json_missing_field(mlc::String(\"#{field_name}\")));"
          lines << "  }"
          lines.concat(derive_json_extract_required("__opt_#{field_name}.value()", field_name, field_type, "  "))
          lines << "  #{field_name} = __decoded_#{field_name};"
          lines << "}"
          lines
        end

                def derive_json_extract_required(value_expr, field_name, field_type, indent)
          lines = []
          case field_type
          when SemanticIR::ArrayType
            element = field_type.element_type
            cpp_elem = derive_json_cpp_type(element)
            lines << "#{indent}if (!#{value_expr}.is_array()) {"
            lines << "#{indent}  return mlc::result::Err<mlc::json::JsonError>(mlc::json::json_type_mismatch(mlc::String(\"#{field_name}\"), mlc::String(\"array\")));"
            lines << "#{indent}}"
            lines << "#{indent}mlc::Array<#{cpp_elem}> __decoded_#{field_name};"
            lines << "#{indent}{"
            lines << "#{indent}  auto __arr_#{field_name} = *#{value_expr}.as_array();"
            lines << "#{indent}  for (const auto& __item_#{field_name} : __arr_#{field_name}) {"
            lines.concat(derive_json_extract_required("__item_#{field_name}", "#{field_name}_item", element, "#{indent}    "))
            lines << "#{indent}    __decoded_#{field_name}.push_back(__decoded_#{field_name}_item);"
            lines << "#{indent}  }"
            lines << "#{indent}}"
          when SemanticIR::Type
            case field_type.name
            when "string", "str"
              lines << "#{indent}if (!#{value_expr}.is_string()) {"
              lines << "#{indent}  return mlc::result::Err<mlc::json::JsonError>(mlc::json::json_type_mismatch(mlc::String(\"#{field_name}\"), mlc::String(\"string\")));"
              lines << "#{indent}}"
              lines << "#{indent}mlc::String __decoded_#{field_name} = *#{value_expr}.as_string();"
            when "bool"
              lines << "#{indent}if (!#{value_expr}.is_bool()) {"
              lines << "#{indent}  return mlc::result::Err<mlc::json::JsonError>(mlc::json::json_type_mismatch(mlc::String(\"#{field_name}\"), mlc::String(\"bool\")));"
              lines << "#{indent}}"
              lines << "#{indent}bool __decoded_#{field_name} = *#{value_expr}.as_bool();"
            when "i8", "i16", "i32", "i64", "u8", "u16", "u32", "u64", "usize", "f32", "f64"
              cpp_type = derive_json_cpp_type(field_type)
              lines << "#{indent}if (!#{value_expr}.is_number()) {"
              lines << "#{indent}  return mlc::result::Err<mlc::json::JsonError>(mlc::json::json_type_mismatch(mlc::String(\"#{field_name}\"), mlc::String(\"number\")));"
              lines << "#{indent}}"
              lines << "#{indent}#{cpp_type} __decoded_#{field_name} = static_cast<#{cpp_type}>(*#{value_expr}.as_number());"
            else
              lines << "#{indent}return mlc::result::Err<mlc::json::JsonError>(mlc::json::json_type_mismatch(mlc::String(\"#{field_name}\"), mlc::String(\"unsupported\")));"
              lines << "#{indent}#{derive_json_cpp_type(field_type)} __decoded_#{field_name}{};"
            end
          else
            lines << "#{indent}return mlc::result::Err<mlc::json::JsonError>(mlc::json::json_type_mismatch(mlc::String(\"#{field_name}\"), mlc::String(\"unsupported\")));"
            lines << "#{indent}#{derive_json_cpp_type(field_type)} __decoded_#{field_name}{};"
          end
          lines
        end


                def derive_json_cpp_type(field_type)
          case field_type
          when SemanticIR::ArrayType
            "mlc::Array<#{derive_json_cpp_type(field_type.element_type)}>"
          when SemanticIR::GenericType
            base = field_type.base_type.respond_to?(:name) ? field_type.base_type.name : "auto"
            if base == "Option"
              "std::optional<#{derive_json_cpp_type(field_type.type_args.first)}>"
            else
              "#{base}<#{field_type.type_args.map { |argument| derive_json_cpp_type(argument) }.join(', ')}>"
            end
          when SemanticIR::Type
            {
              "string" => "mlc::String",
              "str" => "mlc::String",
              "bool" => "bool",
              "i8" => "int8_t",
              "i16" => "int16_t",
              "i32" => "int",
              "i64" => "int64_t",
              "u8" => "uint8_t",
              "u16" => "uint16_t",
              "u32" => "uint32_t",
              "u64" => "uint64_t",
              "usize" => "size_t",
              "f32" => "float",
              "f64" => "double"
            }.fetch(field_type.name, field_type.name)
          else
            "auto"
          end
        end


        HASH_COMBINE_SUFFIX = " + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);"

        def generate_derive_hash(name, type)
          body = case type
                 when SemanticIR::RecordType
                   derive_hash_record_body(type.fields)
                 when SemanticIR::SumType
                   derive_hash_sum_body(type.variants)
                 else
                   return nil
                 end

          code = <<~CPP.strip
            namespace std {
            template<>
            struct hash<#{name}> {
              size_t operator()(const #{name}& self) const noexcept {
                #{body}
              }
            };
            }
          CPP
          @context.factory.raw_statement(code: code)
        end

        def derive_hash_prim_std_type(semantic_type)
          return nil unless semantic_type.is_a?(SemanticIR::Type) && semantic_type.kind == :prim

          case semantic_type.name
          when "string"
            "mlc::String"
          when "i32"
            "int"
          when "bool"
            "bool"
          else
            nil
          end
        end

        def derive_hash_combine_line(cpp_type, access_expr)
          "h ^= std::hash<#{cpp_type}>{}(#{access_expr})#{HASH_COMBINE_SUFFIX}"
        end

        def derive_hash_record_body(fields)
          if fields.empty?
            "return static_cast<size_t>(1469598103934665603ULL);"
          else
            lines = ["size_t h = 1469598103934665603ULL;"]
            fields.each do |field|
              cpp_t = derive_hash_prim_std_type(field[:type])
              lines << "#{derive_hash_combine_line(cpp_t, "self.#{field[:name]}")}"
            end
            lines << "return h;"
            lines.join("\n    ")
          end
        end

        def derive_hash_sum_body(variants)
          lines = ["size_t h = 1469598103934665603ULL;"]
          variants.each_with_index do |variant, idx|
            vname = variant[:name]
            fields = Array(variant[:fields])
            branch_inner =
              if fields.empty?
                derive_hash_combine_line("size_t", idx.to_s)
              elsif fields.length == 1 && fields.first[:name].nil?
                ft = fields.first[:type]
                cpp_t = derive_hash_prim_std_type(ft)
                idx_line = derive_hash_combine_line("size_t", idx.to_s)
                payload_line = derive_hash_combine_line(cpp_t, "std::get<#{vname}>(self._).field0")
                "#{idx_line}\n      #{payload_line}"
              else
                idx_line = derive_hash_combine_line("size_t", idx.to_s)
                payload_lines = fields.map do |field|
                  cpp_t = derive_hash_prim_std_type(field[:type])
                  derive_hash_combine_line(cpp_t, "std::get<#{vname}>(self._).#{field[:name]}")
                end
                ([idx_line] + payload_lines).join("\n      ")
              end
            lines << "if (std::holds_alternative<#{vname}>(self._)) {\n      #{branch_inner}\n      return h;\n    }"
          end
          lines << "return h;"
          lines.join("\n    ")
        end

        def lower_type_alias_internal(name, aliased_type)
          cpp_type = @context.map_type(aliased_type)
          @context.factory.raw_statement(code: "using #{name} = #{cpp_type};")
        end

        def lower_record_type_internal(name, record_type)
          # Generate struct declaration
          members = record_type.fields.map do |field|
            field_type = @context.map_type(field[:type])
            declarator_name = @context.sanitize_identifier(field[:name])
            declarator_text =
              if field[:default]
                "#{declarator_name} = #{@context.lower_expression(field[:default]).to_source}"
              else
                declarator_name
              end
            CppAst::Nodes::VariableDeclaration.new(
              type: field_type,
              declarators: [declarator_text],
              declarator_separators: [],
              type_suffix: " ",
              prefix_modifiers: ""
            )
          end

          # Inject associated type using declarations from trait implementations
          if @trait_registry
            @trait_registry.implementations_for(name).each do |impl|
              next if impl.associated_type_bindings.nil? || impl.associated_type_bindings.empty?
              impl.associated_type_bindings.each do |assoc_name, ast_type|
                cpp_type = TraitCppTypes.ast_type_to_cpp(ast_type)
                next unless cpp_type
                members << @context.factory.raw_statement(code: "using #{assoc_name} = #{cpp_type};")
              end
            end
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

        def ast_type_to_cpp_string(ast_type)
          case ast_type
          when MLC::Source::AST::GenericType
            params = ast_type.type_params.map { |p| ast_type_to_cpp_string(p) }.join(", ")
            "#{ast_type.name}<#{params}>"
          when MLC::Source::AST::AssocTypeRef
            "typename #{ast_type.param_name}::#{ast_type.assoc_name}"
          when MLC::Source::AST::Type
            ast_type.name
          else
            nil
          end
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

          # Generate using declaration for std::variant.
          # For generic sum types, use per-variant type params in the instantiation.
          # Phantom params (not in any variant's fields) are included in every variant.
          all_param_names = type_params.any? ? sum_type_param_names_ordered(sum_type) : []
          phantom_param_names = type_params.any? ? type_params.map(&:name) - all_param_names : []
          variant_type_names = if type_params.any?
                                 sum_type.variants.map do |v|
                                   used = variant_param_names_used(v)
                                   used_params = all_param_names.select { |n| used.include?(n) } + phantom_param_names
                                   used_params.any? ? "#{v[:name]}<#{used_params.join(", ")}>" : v[:name]
                                 end.join(", ")
                               else
                                 sum_type.variants.map { |v| v[:name] }.join(", ")
                               end
          using_decl_raw = CppAst::Nodes::UsingDeclaration.new(
            kind: :alias,
            name: name,
            alias_target: "std::variant<#{variant_type_names}>",
            using_suffix: " ",
            equals_prefix: " ",
            equals_suffix: " "
          )

          # Wrap using alias with full template, variant structs with per-variant templates.
          if type_params.any?
            full_tpl_str, full_suffix = @context.build_template_signature(type_params)
            using_decl = CppAst::Nodes::TemplateDeclaration.new(
              template_params: full_tpl_str, declaration: using_decl_raw,
              template_suffix: "", less_suffix: "", params_suffix: full_suffix
            )
            variant_structs = variant_structs.each_with_index.map do |struct, idx|
              v = sum_type.variants[idx]
              used = variant_param_names_used(v)
              used_names = all_param_names.select { |n| used.include?(n) } + phantom_param_names
              used_tps = type_params.select { |tp| used_names.include?(tp.name) }
              if used_tps.any?
                tpl_str, suffix = @context.build_template_signature(used_tps)
                CppAst::Nodes::TemplateDeclaration.new(
                  template_params: tpl_str, declaration: struct,
                  template_suffix: "", less_suffix: "", params_suffix: suffix
                )
              else
                struct
              end
            end
          else
            using_decl = using_decl_raw
          end

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

        # Collect TypeVariable names from all variants in declaration order.
        def sum_type_param_names_ordered(sum_type)
          seen = []
          sum_type.variants.each do |v|
            collect_type_var_names_from_fields(v[:fields] || [], seen)
          end
          seen
        end

        # Collect TypeVariable names used in a specific variant's fields.
        def variant_param_names_used(variant)
          seen = []
          collect_type_var_names_from_fields(variant[:fields] || [], seen)
          seen
        end

        def collect_type_var_names_from_fields(fields, seen)
          Array(fields).each do |f|
            collect_type_var_names_from_type(f[:type], seen) if f[:type]
          end
        end

        def collect_type_var_names_from_type(type, seen)
          if type.is_a?(MLC::SemanticIR::TypeVariable)
            seen << type.name unless seen.include?(type.name)
          elsif type.respond_to?(:type_args)
            type.type_args.each { |a| collect_type_var_names_from_type(a, seen) }
          end
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

        def empty_unit_function_body?(body, return_type)
          return true if body.is_a?(SemanticIR::UnitLiteral)
          return false unless body.is_a?(SemanticIR::TupleExpr) && body.elements.empty?

          return_type.is_a?(SemanticIR::UnitType) ||
            (return_type.respond_to?(:name) && return_type.name.to_s == "unit")
        end

        # rule_engine no longer exposed - v2 architecture handles rules internally
      end
    end
  end
end
# rubocop:enable Metrics/ParameterLists
