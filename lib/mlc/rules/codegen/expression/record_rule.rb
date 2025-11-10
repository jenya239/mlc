# frozen_string_literal: true

require_relative "../../base_rule"
require_relative "../../../backend/codegen/helpers"

module MLC
  module Rules
    module CodeGen
      module Expression
        # Rule for lowering SemanticIR record expressions to C++ struct initialization
        # Example: Point { x: 10, y: 20 } -> Point{10, 20}
        class RecordRule < BaseRule
          include MLC::Backend::CodeGenHelpers

          def applies?(node, _context = {})
            node.is_a?(MLC::SemanticIR::RecordExpr)
          end

          def apply(node, context = {})
            lowerer = context[:lowerer]
            type_map = context[:type_map] || {}
            type_registry = context[:type_registry]
            in_generic_function = context[:in_generic_function] || false

            # Check if type contains unresolved type variables
            # For nested RecordExpr inside generic functions, use full type (TypeVariables are in scope)
            # For top-level RecordExpr outside generic functions, use CTAD or auto
            type_str = if contains_type_variables?(node.type)
              if in_generic_function
                # Inside generic function: TypeVariables are in scope, use full type
                map_type(node.type, type_map: type_map, type_registry: type_registry)
              else
                # Outside generic function: TypeVariables not in scope, use CTAD
                extract_base_type_name(node.type, type_map: type_map, type_registry: type_registry)
              end
            else
              # Fully resolved type - use explicit template arguments
              map_type(node.type, type_map: type_map, type_registry: type_registry)
            end

            # Recursively lower each field value
            args = node.fields.values.map { |value| lowerer.send(:lower_expression, value) }

            # Generate brace initializer: Point{x_val, y_val}
            CppAst::Nodes::BraceInitializerExpression.new(
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
