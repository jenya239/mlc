# frozen_string_literal: true

require_relative "../utils/string_helpers"

module MLC
  module Backends
    module Cpp
      module Services
        module Helpers
          # Function name resolution and qualification
          module FunctionHelper
            module_function

            # Get qualified function name from registry
            # Returns nil if not found
            # Example: "std::vector" for stdlib function, "myFunc_" for keyword
            def qualified_function_name(name, function_registry)
              return nil unless function_registry

              entry = function_registry.fetch_entry(name)
              return nil unless entry

              base_name = Utils::StringHelpers.sanitize_identifier(entry.name)
              namespace = entry.namespace

              if namespace && !namespace.empty?
                "#{namespace}::#{base_name}"
              else
                base_name
              end
            end
          end
        end
      end
    end
  end
end
