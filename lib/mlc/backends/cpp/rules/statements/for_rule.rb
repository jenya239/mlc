# frozen_string_literal: true

module MLC
  module Backends
    module Cpp
      module Rules
        module Statements
          # Rule for lowering SemanticIR::ForStmt to C++ range-for loops
          class ForRule < StatementRule
            def applies?(node)
              context.checker.for_stmt?(node)
            end

            def apply(node)
              container = context.lower_expression(node.iterable)
              if cyclic_sum_array?(node.iterable)
                element_temporary = context.generate_temp_name
                binding = context.factory.raw_statement(
                  code: "const auto& #{context.sanitize_identifier(node.var_name)} = *#{element_temporary};\n"
                )
                return context.factory.range_for_statement(
                  variable: ForLoopVariable.new("auto", element_temporary),
                  container: container,
                  body: lower_for_body(node.body, binding)
                )
              end

              var_type_str = context.map_type(node.var_type)
              variable = ForLoopVariable.new(
                var_type_str,
                context.sanitize_identifier(node.var_name)
              )
              context.factory.range_for_statement(
                variable: variable,
                container: container,
                body: lower_for_body(node.body)
              )
            end

            private

            def cyclic_sum_array?(iterable)
              element_type = iterable&.type
              return false unless element_type.is_a?(MLC::SemanticIR::ArrayType)

              context.named_cyclic_sum_type?(element_type.element_type)
            end

            def lower_for_body(body_ir, prefix_statement = nil)
              statements = if context.checker.block_expr?(body_ir)
                body_ir.statements.map { |stmt| context.lower_statement(stmt) }
              else
                [context.lower_statement(body_ir)]
              end
              statements.unshift(prefix_statement) if prefix_statement

              context.factory.block_statement(
                statements: statements,
                statement_trailings: Array.new(statements.length, "\n"),
                lbrace_suffix: "\n",
                rbrace_prefix: ""
              )
            end
          end
        end
      end
    end
  end
end
