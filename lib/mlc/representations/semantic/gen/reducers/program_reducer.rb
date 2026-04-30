# frozen_string_literal: true

# All classes autoloaded by Zeitwerk on-demand:
# - MLC::Common::Analysis::PassManager (line 47)

module MLC
  module Representations
    module Semantic
      module Gen
        module Reducers
          # ProgramReducer orchestrates the SemanticGen pipeline using passes
          class ProgramReducer
            def initialize(services:, type_reducer:, function_reducer:, module_import_pass:, type_resolution_service:, module_name_override: nil)
              @services = services
              @type_reducer = type_reducer
              @function_reducer = function_reducer
              @module_import_pass = module_import_pass
              @type_resolution_service = type_resolution_service
              @module_name_override = module_name_override
              @pass_manager = build_pass_manager
            end

            def reduce(program, module_name: nil)
              context = {
                program: program,
                imports: [],
                type_items: [],
                func_items: [],
                module_name: module_name || @module_name_override || module_name_for(program),
                import_aliases: {}
              }

              @expanded_function_declaration_cache = {}
              Services::TraitParamExpandAst.validate_trait_and_type_name_conflicts(program, @services.type_checker)

              @services.module_context_service.with_current_module(context[:module_name]) do
                @services.module_context_service.with_import_aliases(context[:import_aliases]) do
                  @pass_manager.run(context)
                end
              end

              MLC::SemanticIR::Module.new(
                name: context[:module_name],
                items: context[:type_items] + context[:func_items],
                imports: context[:imports]
              )
            ensure
              @expanded_function_declaration_cache = nil
            end

            private

            def expanded_function_declaration(func_decl)
              cache = @expanded_function_declaration_cache ||= {}
              cache[func_decl.object_id] ||= Services::TraitParamExpandAst.expand_function_declaration(
                func_decl,
                trait_registry: @services.trait_registry,
                type_decl_table: @services.type_checker.type_decl_table
              )
            end

            def build_pass_manager
              MLC::Common::Analysis::PassManager.new.tap do |manager|
                manager.register(:collect_imports, method(:pass_collect_imports))
                manager.register(:preregister_types, method(:pass_preregister_types))
                manager.register(:preregister_traits, method(:pass_preregister_traits))
                manager.register(:preregister_functions, method(:pass_preregister_functions))
                manager.register(:register_import_aliases, method(:pass_register_import_aliases))
                manager.register(:lower_declarations, method(:pass_lower_declarations))
              end
            end

            def pass_collect_imports(context)
              program = context[:program]

              Array(program.imports).each do |import_decl|
                context[:imports] << MLC::SemanticIR::Import.new(
                  path: import_decl.path,
                  items: import_decl.items
                )
              end

              @module_import_pass.run(program)
            end

            def pass_preregister_types(context)
              program = context[:program]

              program.declarations.each do |decl|
                next unless decl.is_a?(MLC::Source::AST::TypeDecl)

                type_ir = @type_reducer.reduce(decl)
                context[:type_items] << type_ir
              end
            end

            def pass_preregister_traits(context)
              program = context[:program]
              trait_registry = @services.trait_registry

              program.declarations.each do |decl|
                case decl
                when MLC::Source::AST::TraitDecl
                  # Register trait definition
                  methods = decl.methods.map do |m|
                    {
                      name: m.name,
                      params: m.params,
                      ret_type: m.ret_type,
                      body: m.body,
                      is_static: m.is_static
                    }
                  end
                  # Extract associated types from trait
                  associated_types = Array(decl.associated_types).map do |at|
                    {
                      name: at.name,
                      bounds: at.bounds,
                      default_type: at.default_type
                    }
                  end
                  trait_registry.register_trait(
                    name: decl.name,
                    type_params: decl.type_params.map(&:name),
                    methods: methods,
                    associated_types: associated_types
                  )

                when MLC::Source::AST::ExtendDecl
                  # Register trait implementation
                  type_name = extract_type_name(decl.target_type)
                  # Extract type params from extend target type (e.g., T from extend Shared<T>)
                  extend_type_params = extract_extend_type_params(decl.target_type)

                  # Extract associated type bindings from extend FIRST
                  # (needed for resolving types like Item -> i32 in function signatures)
                  associated_type_bindings = {}
                  Array(decl.associated_type_bindings).each do |binding|
                    associated_type_bindings[binding.name] = binding.type
                  end

                  methods = {}
                  # Wrap method processing in associated type context for signature resolution
                  @services.scope_context.with_associated_type_bindings(associated_type_bindings) do
                    decl.methods.each do |func|
                      func = fill_self_type(func, decl.target_type)
                      methods[func.name] = Services::TraitRegistry::MethodInfo.new(
                        name: func.name,
                        params: func.params,
                        ret_type: func.ret_type,
                        body: func.body,
                        is_static: !self_receiver?(func)
                      )

                      mangled_name = "#{type_name}_#{func.name}"
                      combined_type_params = extend_type_params + Array(func.type_params)
                      synthetic_func = MLC::Source::AST::FuncDecl.new(
                        name: mangled_name,
                        params: func.params,
                        ret_type: func.ret_type,
                        body: func.body,
                        type_params: combined_type_params,
                        exported: decl.exported,
                        external: func.external,
                        origin: func.origin
                      )
                      @function_reducer.register_signature(synthetic_func)
                    end
                  end

                  trait_registry.register_implementation(
                    type_name: type_name,
                    trait_name: decl.trait_name,
                    methods: methods,
                    associated_type_bindings: associated_type_bindings
                  )

                  # Validate completeness if implementing a trait
                  validate_trait_implementation(trait_registry, type_name, decl.trait_name, methods, decl) if decl.trait_name
                end
              end
            end

            def validate_trait_implementation(trait_registry, type_name, trait_name, impl_methods, decl)
              trait_info = trait_registry.get_trait(trait_name)
              return unless trait_info # Trait not found - will be caught elsewhere

              missing_methods = []
              trait_info.trait_methods.each do |trait_method|
                method_name = trait_method[:name]
                # Skip if method has default implementation
                next if trait_method[:body]

                # Check if implemented
                missing_methods << method_name unless impl_methods.key?(method_name)
              end

              return if missing_methods.empty?

              origin = decl.origin
              location = origin ? " at #{origin.label}" : ""
              raise MLC::CompileError,
                    "Incomplete trait implementation: #{type_name} does not implement " \
                    "required method(s) #{missing_methods.join(', ')} from trait #{trait_name}#{location}"
            end

            def extract_type_name(type)
              type.name
            end

            def self_receiver?(func)
              func.params.any? && func.params.first.name == "self"
            end

            # If method has explicit `self` param with no type annotation, fill it in from the extend target type.
            def fill_self_type(func, target_type)
              return func unless self_receiver?(func)

              first = func.params.first
              return func if first.type # already has explicit type

              filled = MLC::Source::AST::Param.new(name: "self", type: target_type, mutable: first.mutable)
              MLC::Source::AST::FuncDecl.new(
                name: func.name,
                params: [filled] + func.params.drop(1),
                ret_type: func.ret_type,
                body: func.body,
                type_params: func.type_params,
                exported: func.exported,
                external: func.external,
                origin: func.origin
              )
            end

            # Extract type parameters from extend target type
            # e.g., extend Shared<T> -> [TypeParam(name: "T")]
            def extract_extend_type_params(type)
              case type
              when MLC::Source::AST::GenericType
                # type_params contains PrimType nodes representing type variables
                type.type_params.map do |tp|
                  case tp
                  when MLC::Source::AST::PrimType
                    # Convert to TypeParam AST node for function registration
                    MLC::Source::AST::TypeParam.new(name: tp.name, constraint: nil)
                  else
                    tp
                  end
                end
              else
                []
              end
            end

            def pass_preregister_functions(context)
              program = context[:program]

              program.declarations.each do |decl|
                next unless decl.is_a?(MLC::Source::AST::FuncDecl)

                @function_reducer.register_signature(expanded_function_declaration(decl))
              end
            end

            def pass_register_import_aliases(_context)
              # User module imports are handled eagerly in pass_collect_imports.
            end

            def pass_lower_declarations(context)
              program = context[:program]

              program.declarations.each do |decl|
                case decl
                when MLC::Source::AST::TypeDecl
                  @type_resolution_service&.refresh_function_signatures!(decl.name)
                when MLC::Source::AST::FuncDecl
                  context[:func_items] << @function_reducer.reduce(expanded_function_declaration(decl))
                when MLC::Source::AST::ExtendDecl
                  # Lower methods from extend block as functions with mangled names
                  type_name = extract_type_name(decl.target_type)
                  extend_type_params = extract_extend_type_params(decl.target_type)

                  # Build associated type bindings hash for resolving types like Item -> i32
                  assoc_bindings = {}
                  Array(decl.associated_type_bindings).each do |binding|
                    assoc_bindings[binding.name] = binding.type
                  end

                  # Wrap method processing in associated type context
                  @services.scope_context.with_associated_type_bindings(assoc_bindings) do
                    @services.scope_context.with_extend_type(type_name) do
                      decl.methods.each do |func|
                        next if func.external

                        func = fill_self_type(func, decl.target_type)
                        mangled_name = "#{type_name}_#{func.name}"
                        combined_type_params = extend_type_params + Array(func.type_params)
                        synthetic_func = MLC::Source::AST::FuncDecl.new(
                          name: mangled_name,
                          params: func.params,
                          ret_type: func.ret_type,
                          body: func.body,
                          type_params: combined_type_params,
                          exported: decl.exported,
                          origin: func.origin
                        )
                        context[:func_items] << @function_reducer.reduce(expanded_function_declaration(synthetic_func))
                      end
                    end
                  end
                when MLC::Source::AST::TraitDecl
                  # Traits don't generate code directly - they're contract definitions
                  # Methods with default implementations will be generated when used
                  nil
                end
              end
            end

            def module_name_for(program)
              program.module_decl ? program.module_decl.name : "main"
            end
          end
        end
      end
    end
  end
end
