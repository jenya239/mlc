# frozen_string_literal: true

require_relative "../statement_rule"

module MLC
  module Backends
    module Cpp
      module Rules
        module Statements
          # Rule for lowering SemanticIR::VariableDeclStmt to C++ variable declarations
          class VariableDeclRule < StatementRule
            def applies?(node)
              context.checker.var_decl?(node)
            end

            def apply(node)
              # Map type to C++
              type_str = context.map_type(node.type)
              use_auto = context.type_requires_auto?(node.type, type_str: type_str)

              # Lower initializer expression
              init_expr = context.lower_expression(node.value)

              # Build declarator
              decl_type = use_auto ? "auto" : type_str
              identifier = context.sanitize_identifier(node.name)
              declarator = "#{identifier} = #{init_expr.to_source}"

              # Don't add const for pointer types (they end with *)
              is_pointer = type_str.end_with?("*")
              prefix = (node.mutable || is_pointer) ? "" : "const "

              context.factory.variable_declaration(
                type: decl_type,
                declarators: [declarator],
                declarator_separators: [],
                type_suffix: " ",
                prefix_modifiers: prefix
              )
            end
          end
        end
      end
    end
  end
end
