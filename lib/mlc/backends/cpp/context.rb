# frozen_string_literal: true

module MLC
  module Backends
    module Cpp
      # Public API for code generation rules
      # Replaces direct lowerer.send(:private_method) calls
      class Context
        def initialize(container)
          @container = container
        end

        def type_registry
          @container.type_registry
        end

        def function_registry
          @container.function_registry
        end

        # Type mapping methods (will replace lowerer.send(:type_map, ...))
        def map_type(type)
          # TODO: Delegate to type mapper service
          type.to_s
        end

        def map_function_type(func_type)
          # TODO: Delegate to type mapper service
          func_type.to_s
        end

        # String formatting methods (will replace lowerer.send(:escape_string, ...))
        def escape_string(str)
          # TODO: Delegate to string helpers service
          str.inspect
        end

        def sanitize_identifier(name)
          # TODO: Delegate to string helpers service
          name.to_s
        end

        # Template methods (will replace lowerer.send(:generate_lambda_type, ...))
        def generate_lambda_type(params, return_type)
          # TODO: Delegate to template builder service
          "std::function<#{return_type}(#{params.join(', ')})>"
        end
      end
    end
  end
end
