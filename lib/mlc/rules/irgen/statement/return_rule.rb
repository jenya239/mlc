# frozen_string_literal: true

require_relative "../../base_rule"

module MLC
  module Rules
    module IRGen
      module Statement
        # ReturnRule: Transform AST return statements to HighIR
        # Phase 23-C: Visitor provides pre-transformed IR nodes via context
        # Rule focuses on semantics (return type validation)
        class ReturnRule < BaseRule
          def applies?(node, _context = {})
            node.is_a?(MLC::AST::Return)
          end

          def apply(node, context = {})
            type_checker = context.fetch(:type_checker)
            scope_context = context.fetch(:scope_context)

            # Validate: return must be inside function
            expected = scope_context.current_function_return
            unless expected
              type_checker.type_error("return statement outside of function")
            end

            # Visitor already transformed return expression - get from context
            # Can be nil for void returns (return without value)
            value_ir = context.fetch(:value_ir, nil)

            # Validate return type compatibility
            if type_checker.void_type?(expected)
              # Void function: no return value allowed
              if value_ir
                type_checker.type_error("return value not allowed in void function", node: node)
              end
            else
              # Non-void function: return value required
              unless value_ir
                expected_name = type_checker.describe_type(expected)
                type_checker.type_error("return statement requires a value of type #{expected_name}", node: node)
              end
              # Check type compatibility
              type_checker.ensure_compatible(value_ir.type, expected, "return statement", node: node)
            end

            # Build return statement (wrap in array for statement rule convention)
            [MLC::HighIR::Builder.return_stmt(value_ir)]
          end
        end
      end
    end
  end
end
