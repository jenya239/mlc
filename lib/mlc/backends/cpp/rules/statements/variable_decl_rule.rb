# frozen_string_literal: true


module MLC
  module Backends
    module Cpp
      module Rules
        module Statements
          # Rule for lowering SemanticIR::VariableDeclStmt to C++ variable declarations
          # Handles variable rebinding by generating assignment for subsequent let bindings
          class VariableDeclRule < StatementRule
            def applies?(node)
              context.checker.var_decl?(node)
            end

            def apply(node)
              identifier = context.sanitize_identifier(node.name)
              init_expr = context.lower_expression(node.value)

              # Check if this is a rebinding (variable already declared)
              if context.variable_declared?(node.name)
                # Generate assignment instead of declaration
                return context.factory.expression_statement(
                  expression: context.factory.binary_expression(
                    left: context.factory.identifier(name: identifier),
                    operator: "=",
                    right: init_expr
                  )
                )
              end

              # First declaration - track it
              context.declare_variable(node.name)

              # Map type to C++
              type_str = context.map_type(node.type)
              use_auto = context.type_requires_auto?(node.type, type_str: type_str)

              # Build declarator
              decl_type = use_auto ? "auto" : type_str
              declarator = "#{identifier} = #{init_expr.to_source}"

              # Don't add const for pointer types (they end with *)
              # Also don't add const if variable might be rebound (we don't know at this point)
              is_pointer = type_str.end_with?("*")
              prefix = (node.mutable || is_pointer) ? "" : ""  # Remove const - variables may be rebound

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
