# frozen_string_literal: true

require_relative "../expression_rule"

module MLC
  module Backends
    module Cpp
      module Rules
        module Expressions
          # Rule for lowering SemanticIR::RecordExpr to C++ struct initialization
          # Handles: Point { x: 10, y: 20 } -> Point{10, 20}
          class RecordRule < ExpressionRule
            def applies?(node)
              context.checker.record_expr?(node)
            end

            def apply(node)
              # Check if type contains unresolved type variables
              # For nested RecordExpr inside generic functions, use full type (TypeVariables are in scope)
              # For top-level RecordExpr outside generic functions, use CTAD or auto
              type_str = if context.contains_type_variables?(node.type)
                           if context.in_generic_function?
                             # Inside generic function: TypeVariables are in scope, use full type
                             context.map_type(node.type)
                           else
                             # Outside generic function: TypeVariables not in scope, use CTAD
                             context.extract_base_type_name(node.type)
                           end
                         else
                           # Fully resolved type - use explicit template arguments
                           context.map_type(node.type)
                         end

              # Recursively lower each field value
              args = node.fields.values.map { |value| lower_expression(value) }

              # Generate brace initializer: Point{x_val, y_val}
              context.factory.brace_initializer(
                type: type_str,
                arguments: args,
                argument_separators: args.size > 1 ? Array.new(args.size - 1, ", ") : []
              )
            end
          end
        end
      end
    end
  end
end
