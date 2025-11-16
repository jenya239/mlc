# frozen_string_literal: true

require_relative '../../../../common/analysis/pass_manager'

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