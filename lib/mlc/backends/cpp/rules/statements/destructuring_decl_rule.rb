# frozen_string_literal: true

module MLC
  module Backends
    module Cpp
      module Rules
        module Statements
          # Rule for lowering SemanticIR::DestructuringDeclStmt to C++ structured bindings
          #
          # Tuple destructuring: let (a, b) = expr
          # Generates: auto [a, b] = expr;  (C++17 structured bindings)
          #
          # Record destructuring: let { x, y } = point
          # Generates:
          #   auto __tmp = point;
          #   auto x = __tmp.x;
          #   auto y = __tmp.y;
          class DestructuringDeclRule < StatementRule
            def applies?(node)
              node.is_a?(MLC::SemanticIR::DestructuringDeclStmt)
            end

            def apply(node)
              value_expr = context.lower_expression(node.value)
              bindings = node.bindings

              # Check if this is tuple destructuring (accessors are TupleAccessExpr)
              # or record destructuring (accessors are MemberExpr)
              if tuple_destructuring?(bindings)
                generate_tuple_destructuring(bindings, value_expr, node)
              else
                generate_record_destructuring(bindings, value_expr, node)
              end
            end

            private

            def tuple_destructuring?(bindings)
              return false if bindings.empty?
              bindings.first[:accessor].is_a?(MLC::SemanticIR::TupleAccessExpr)
            end

            # Generate C++17 structured bindings: auto [a, b] = expr;
            def generate_tuple_destructuring(bindings, value_expr, node)
              binding_names = bindings.map { |b| context.sanitize_identifier(b[:name]) }

              # Track all variables as declared
              bindings.each { |b| context.declare_variable(b[:name]) }

              # Generate: auto [a, b, c] = value;
              binding_list = "[#{binding_names.join(', ')}]"
              declarator = "#{binding_list} = #{value_expr.to_source}"

              context.factory.variable_declaration(
                type: "auto",
                declarators: [declarator],
                declarator_separators: [],
                type_suffix: " ",
                prefix_modifiers: ""
              )
            end

            # Generate individual variable declarations for record fields
            # Since C++ doesn't have nice syntax for named field extraction,
            # we generate separate declarations
            def generate_record_destructuring(bindings, value_expr, _node)
              statements = []

              # First, store the value in a temp variable to avoid re-evaluation
              temp_name = context.generate_temp_name
              temp_decl = context.factory.variable_declaration(
                type: "auto",
                declarators: ["#{temp_name} = #{value_expr.to_source}"],
                declarator_separators: [],
                type_suffix: " ",
                prefix_modifiers: ""
              )
              statements << temp_decl

              # Then extract each field
              bindings.each do |binding|
                identifier = context.sanitize_identifier(binding[:name])
                field_name = binding[:name]  # Same as variable name for { x, y } pattern

                context.declare_variable(binding[:name])

                field_decl = context.factory.variable_declaration(
                  type: "auto",
                  declarators: ["#{identifier} = #{temp_name}.#{field_name}"],
                  declarator_separators: [],
                  type_suffix: " ",
                  prefix_modifiers: ""
                )
                statements << field_decl
              end

              # Return a compound of statements
              context.factory.compound_statement(statements: statements)
            end
          end
        end
      end
    end
  end
end
