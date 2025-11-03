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
            transformer = context.fetch(:transformer)
            type_checker = context.fetch(:type_checker)
            type_inference = context.fetch(:type_inference)
            scope_context = context.fetch(:scope_context)

            # Phase 23-C: Support both visitor path (with value_ir) and legacy path (without)
            value_ir = if context.key?(:value_ir)
                         # New path: Visitor provides pre-transformed IR
                         context.fetch(:value_ir)
                       else
                         # Legacy path: Transform inline for backward compatibility
                         node.expr ? transformer.send(:transform_expression, node.expr) : nil
                       end

            # Validate: return must be inside function
            expected = scope_context.current_function_return
            unless expected
              type_checker.type_error("return statement outside of function")
            end

            # Validate return type compatibility
            if type_inference.void_type?(expected)
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
