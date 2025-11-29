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
            def initialize(services:, type_reducer:, function_reducer:, module_import_pass:, type_resolution_service:)
              @services = services
              @type_reducer = type_reducer
              @function_reducer = function_reducer
              @module_import_pass = module_import_pass
              @type_resolution_service = type_resolution_service
              @pass_manager = build_pass_manager
        end

            def reduce(program)
              context = {
                program: program,
                imports: [],
                type_items: [],
                func_items: [],
                module_name: module_name_for(program),
                import_aliases: {}
              }

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
        end

            private

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
                  trait_registry.register_trait(
                    name: decl.name,
                    type_params: decl.type_params.map(&:name),
                    methods: methods
                  )

                when MLC::Source::AST::ExtendDecl
                  # Register trait implementation
                  type_name = extract_type_name(decl.target_type)
                  # Extract type params from extend target type (e.g., T from extend Shared<T>)
                  extend_type_params = extract_extend_type_params(decl.target_type)
                  methods = {}
                  decl.methods.each do |func|
                    methods[func.name] = Services::TraitRegistry::MethodInfo.new(
                      name: func.name,
                      params: func.params,
                      ret_type: func.ret_type,
                      body: func.body,
                      is_static: true  # Associated functions are static
                    )

                    # Also register as regular function with mangled name for C++ backend
                    # Type_method -> allows Type.method() to resolve to Type_method()
                    mangled_name = "#{type_name}_#{func.name}"
                    # Combine extend type params with function's own type params
                    combined_type_params = extend_type_params + Array(func.type_params)
                    synthetic_func = MLC::Source::AST::FuncDecl.new(
                      name: mangled_name,
                      params: func.params,
                      ret_type: func.ret_type,
                      body: func.body,
                      type_params: combined_type_params,
                      exported: decl.exported,
                      external: func.external,  # Preserve extern flag
                      origin: func.origin
                    )
                    @function_reducer.register_signature(synthetic_func)
                  end

                  trait_registry.register_implementation(
                    type_name: type_name,
                    trait_name: decl.trait_name,
                    methods: methods
                  )

                  # Validate completeness if implementing a trait
                  if decl.trait_name
                    validate_trait_implementation(trait_registry, type_name, decl.trait_name, methods, decl)
                  end
                end
              end
            end

            def validate_trait_implementation(trait_registry, type_name, trait_name, impl_methods, decl)
              trait_info = trait_registry.get_trait(trait_name)
              return unless trait_info  # Trait not found - will be caught elsewhere

              missing_methods = []
              trait_info.methods.each do |trait_method|
                method_name = trait_method[:name]
                # Skip if method has default implementation
                next if trait_method[:body]
                # Check if implemented
                unless impl_methods.key?(method_name)
                  missing_methods << method_name
                end
              end

              unless missing_methods.empty?
                origin = decl.origin
                location = origin ? " at #{origin.label}" : ""
                raise MLC::CompileError,
                      "Incomplete trait implementation: #{type_name} does not implement " \
                      "required method(s) #{missing_methods.join(', ')} from trait #{trait_name}#{location}"
              end
            end

            def extract_type_name(type)
              case type
              when MLC::Source::AST::PrimType
                type.name
              when MLC::Source::AST::GenericType
                type.name
              else
                type.name
              end
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
                @function_reducer.register_signature(decl)
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
                  context[:func_items] << @function_reducer.reduce(decl)
                when MLC::Source::AST::ExtendDecl
                  # Lower methods from extend block as functions with mangled names
                  type_name = extract_type_name(decl.target_type)
                  extend_type_params = extract_extend_type_params(decl.target_type)
                  decl.methods.each do |func|
                    # Skip extern functions - they are declarations only, no code generation
                    next if func.external

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
                    context[:func_items] << @function_reducer.reduce(synthetic_func)
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