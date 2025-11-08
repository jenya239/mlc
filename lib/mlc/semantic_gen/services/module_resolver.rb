# frozen_string_literal: true

module MLC
  module SemanticGen
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

        def register_module_import(import_decl, function_registry)
          module_name = import_decl.path
          alias_name = import_decl.alias

          add_alias(alias_name, module_name) if alias_name && !alias_name.empty?

          prefixes = []
          prefixes << module_name if module_name && !module_name.empty?
          prefixes << alias_name if alias_name && !alias_name.empty?
          prefixes = prefixes.compact.uniq

          selected_items = import_decl.import_all ? nil : Array(import_decl.items)
          entries = function_registry.functions_in_module(module_name)

          entries.each do |entry|
            next if selected_items && !selected_items.include?(entry.info.name)

            prefixes.each do |prefix|
              alias_key = [prefix, entry.info.name].join('.')
              register_alias_for(function_registry, alias_key, entry.name)
            end

            if selected_items && !selected_items.empty?
              register_alias_for(function_registry, entry.info.name, entry.name)
            end
          end
        end

        def register_alias_for(function_registry, alias_name, canonical_name)
          return if alias_name.nil? || alias_name.empty?
          return if alias_name == canonical_name

          function_registry.register_alias(alias_name, canonical_name)
        rescue KeyError, ArgumentError
          nil
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
