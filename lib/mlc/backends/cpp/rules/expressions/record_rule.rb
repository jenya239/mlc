# frozen_string_literal: true

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

              type_base_name = node.type_name.to_s.split("<").first
              type_entry = context.type_registry.lookup(type_base_name)
              record_fields_list = type_entry&.record? ? Array(type_entry.fields) : []
              has_field_default_in_declaration = record_fields_list.any? { |field_row| field_row[:default] }
              known_record_shape = type_entry&.record? && !record_fields_list.empty?

              literal_covers_all_declared_fields =
                known_record_shape && node.fields.size == record_fields_list.size

              use_designated_initializers =
                !node.type_name.to_s.start_with?("__anon_record") &&
                known_record_shape &&
                (has_field_default_in_declaration || !literal_covers_all_declared_fields)

              field_arguments =
                if use_designated_initializers
                  node.fields.map do |field_name, value|
                    lowered_value = lower_expression(value)
                    field_cpp_name = context.sanitize_identifier(field_name.to_s)
                    context.factory.raw_expression(code: ".#{field_cpp_name} = #{lowered_value.to_source}")
                  end
                else
                  node.fields.values.map { |value| lower_expression(value) }
                end

              context.factory.brace_initializer(
                type: type_str,
                arguments: field_arguments,
                argument_separators: field_arguments.size > 1 ? Array.new(field_arguments.size - 1, ", ") : []
              )
            end
          end
        end
      end
    end
  end
end
