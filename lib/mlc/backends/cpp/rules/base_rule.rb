# frozen_string_literal: true

module MLC
  module Backends
    module Cpp
      module Rules
        # Base class for all code generation rules
        # Replaces direct lowerer inheritance with composition
        class BaseRule
          attr_reader :context

          def initialize(context)
            @context = context
          end

          # Abstract method - must be implemented by subclasses
          # Check if this rule applies to the given node
          def applies?(node)
            raise NotImplementedError, "#{self.class}#applies? must be implemented"
          end

          # Abstract method - must be implemented by subclasses
          # Apply transformation to the node
          def apply(node)
            raise NotImplementedError, "#{self.class}#apply must be implemented"
          end

          protected

          # Recursively lower an expression node
          def lower_expression(node)
            context.lower_expression(node)
          end

          # Recursively lower a statement node
          def lower_statement(node)
            context.lower_statement(node)
          end


          # Convenience methods delegating to context
          def type_registry
            context.type_registry
          end

          def function_registry
            context.function_registry
          end

          def map_type(type)
            context.map_type(type)
          end

          def escape_string(str)
            context.escape_string(str)
          end

          def sanitize_identifier(name)
            context.sanitize_identifier(name)
          end
        end
      end
    end
  end
end
