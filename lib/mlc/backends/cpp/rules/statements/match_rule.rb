# frozen_string_literal: true

require_relative "../statement_rule"

module MLC
  module Backends
    module Cpp
      module Rules
        module Statements
          # Rule for lowering SemanticIR::MatchStmt to C++ std::visit
          class MatchRule < StatementRule
            def applies?(node)
              context.checker.match_stmt?(node)
            end

            def apply(node)
              # Lower scrutinee expression
              scrutinee = context.lower_expression(node.scrutinee)

              # Lower match arms
              arms = node.arms.map { |arm| lower_match_arm(arm) }

              CppAst::Nodes::MatchExpression.new(
                value: scrutinee,
                arms: arms,
                arm_separators: Array.new([arms.size - 1, 0].max, ",\n")
              )
            end

            private

            def lower_match_arm(arm)
              pattern = arm[:pattern]

              if pattern[:kind] == :wildcard
                # Wildcard pattern (_)
                body = lower_statement_block(arm[:body])
                CppAst::Nodes::WildcardMatchArmStatement.new(
                  var_name: "_unused",
                  body: body
                )
              else
                # Constructor pattern
                lower_constructor_arm(arm)
              end
            end

            def lower_constructor_arm(arm)
              pattern = arm[:pattern]
              case_name = pattern[:name]
              var_name = context.sanitize_identifier(case_name.downcase)
              bindings = Array(pattern[:bindings] || pattern[:fields]).compact.reject { |b| b == "_" }
              sanitized_bindings = bindings.map { |name| context.sanitize_identifier(name) }

              body_block = lower_statement_block(arm[:body])

              # Add structured binding if there are bindings
              block_with_binding = if sanitized_bindings.any?
                add_structured_binding(body_block, sanitized_bindings, var_name)
              else
                body_block
              end

              CppAst::Nodes::MatchArmStatement.new(
                case_name: case_name,
                var_name: var_name,
                body: block_with_binding
              )
            end

            def lower_statement_block(body_ir)
              if context.checker.block_expr?(body_ir)
                statements = body_ir.statements.map { |stmt| context.lower_statement(stmt) }
                context.factory.block_statement(
                  statements: statements,
                  statement_trailings: Array.new(statements.length, "\n"),
                  lbrace_suffix: "\n",
                  rbrace_prefix: ""
                )
              else
                stmt = context.lower_statement(body_ir)
                context.factory.block_statement(
                  statements: [stmt],
                  statement_trailings: ["\n"],
                  lbrace_suffix: "\n",
                  rbrace_prefix: ""
                )
              end
            end

            def add_structured_binding(body_block, bindings, var_name)
              # Create structured binding declaration: auto [field1, field2, ...] = var_name;
              binding_list = bindings.join(", ")
              binding_stmt = CppAst::Nodes::RawStatement.new(
                code: "auto [#{binding_list}] = #{var_name};"
              )

              # Prepend binding to body statements
              new_statements = [binding_stmt] + body_block.statements
              new_trailings = ["\n"] + body_block.statement_trailings

              context.factory.block_statement(
                statements: new_statements,
                statement_trailings: new_trailings,
                lbrace_suffix: body_block.lbrace_suffix,
                rbrace_prefix: body_block.rbrace_prefix
              )
            end
          end
        end
      end
    end
  end
end
