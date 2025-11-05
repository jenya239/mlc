# frozen_string_literal: true

module MLC
  module IRGenV2
    module Services
      # ModuleResolver - Pure service for module resolution
      # Responsibility: ONLY state about import aliases and module names
      class ModuleResolver
        def initialize(import_aliases = {})
          @import_aliases = import_aliases
        end

        # Resolve import alias to canonical module name
        # Pure function: name -> module_name
        def resolve_alias(name)
          @import_aliases[name] || name
        end

        # Add import alias
        def add_alias(alias_name, module_name)
          @import_aliases[alias_name] = module_name
        end

        # Resolve module member function entry
        # Delegates to FunctionRegistry (separation of concerns)
        def resolve_member_function(object_ast, member_name, function_registry, ast_type_checker)
          return nil unless ast_type_checker.var_ref?(object_ast)

          # Resolve import alias
          module_name = resolve_alias(object_ast.name)

          # Lookup in function registry (another service)
          function_registry.fetch_entry_for_member(module_name, member_name)
        end
      end
    end
  end
end
