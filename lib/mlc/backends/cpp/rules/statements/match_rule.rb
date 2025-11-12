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
              # Lower subject expression
              subject_expr = context.lower_expression(node.subject)

              # Lower match arms
              arms = node.arms.map { |arm| lower_match_arm(arm) }

              CppAst::Nodes::MatchExpression.new(
                subject: subject_expr,
                arms: arms,
                trailing_newline: true
              )
            end

            private

            def lower_match_arm(arm)
              if context.checker.wildcard_pattern?(arm.pattern)
                # Wildcard pattern (_)
                body = context.lower_statement(arm.body)
                CppAst::Nodes::WildcardMatchArmStatement.new(body: body)
              else
                # Constructor pattern
                pattern_type = context.lower_type(arm.pattern.type)
                body = lower_match_arm_body(arm)

                CppAst::Nodes::MatchArmStatement.new(
                  pattern_type: pattern_type,
                  body: body
                )
              end
            end

            def lower_match_arm_body(arm)
              case arm.pattern
              when MLC::SemanticIR::ConstructorPattern
                # Lower body statements
                body_stmts = if context.checker.block_expr?(arm.body)
                  arm.body.statements.map { |stmt| context.lower_statement(stmt) }
                else
                  [context.lower_statement(arm.body)]
                end

                # Add structured binding if pattern has variable bindings
                if arm.pattern.fields && !arm.pattern.fields.empty?
                  body_stmts = add_structured_binding(arm.pattern, body_stmts)
                end

                context.factory.block_statement(
                  statements: body_stmts,
                  statement_trailings: Array.new(body_stmts.length, "\n"),
                  lbrace_suffix: "\n",
                  rbrace_prefix: ""
                )
              when MLC::SemanticIR::VarPattern
                # Variable pattern - bind the whole value
                var_name = context.sanitize_identifier(arm.pattern.name)

                # Lower body statements
                body_stmts = if context.checker.block_expr?(arm.body)
                  arm.body.statements.map { |stmt| context.lower_statement(stmt) }
                else
                  [context.lower_statement(arm.body)]
                end

                # Prepend variable binding
                binding_decl = CppAst::Nodes::RawStatement.new(
                  code: "auto& #{var_name} = arg;"
                )

                context.factory.block_statement(
                  statements: [binding_decl] + body_stmts,
                  statement_trailings: Array.new(body_stmts.length + 1, "\n"),
                  lbrace_suffix: "\n",
                  rbrace_prefix: ""
                )
              else
                raise "Unsupported pattern type: #{arm.pattern.class}"
              end
            end

            def add_structured_binding(pattern, body_stmts)
              # Extract field names from constructor pattern
              field_names = pattern.fields.map { |f| context.sanitize_identifier(f.name) }
              binding_list = "[#{field_names.join(', ')}]"

              # Create structured binding declaration
              binding_decl = CppAst::Nodes::RawStatement.new(
                code: "auto #{binding_list} = arg;"
              )

              [binding_decl] + body_stmts
            end
          end
        end
      end
    end
  end
end
