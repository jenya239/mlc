# frozen_string_literal: true

require_relative "../expression_rule"
require_relative "../../for_loop_variable"

module MLC
  module Backends
    module Cpp
      module Rules
        module Expressions
          # Rule for lowering SemanticIR::ListCompExpr to C++ loops in IIFE
          # Example: [x * 2 for x in nums if x > 0] ->
          # [&]() { std::vector<int> result; for(auto x : nums) { if(!(x > 0)) continue; result.push_back(x * 2); } return result; }()
          class ListCompRule < ExpressionRule
            def applies?(node)
              context.checker.list_comp_expr?(node)
            end

            def apply(node)
              # Map element type to C++
              element_cpp_type = context.map_type(node.element_type)
              vector_type = "std::vector<#{element_cpp_type}>"

              # Create result vector declaration
              result_decl = context.factory.variable_declaration(
                type: vector_type,
                declarators: ["result"],
                declarator_separators: [],
                type_suffix: " "
              )

              lambda_statements = [result_decl]

              # Build loop body: filters + push_back
              body_statements = build_filter_statements(node)
              body_statements << build_push_back_statement(node)

              # Wrap in block
              body_block = context.factory.block_statement(
                statements: body_statements,
                statement_trailings: Array.new(body_statements.length, "\n"),
                lbrace_suffix: "\n",
                rbrace_prefix: ""
              )

              # Build nested range-for loops (reversed to get proper nesting)
              current_body = body_block
              outer_range_stmt = nil

              node.generators.reverse_each do |generator|
                var_type_str = context.map_type(generator[:var_type])
                variable = ForLoopVariable.new(var_type_str, generator[:var_name])
                container_expr = context.lower_expression(generator[:iterable])

                range_stmt = context.factory.range_for_statement(
                  variable: variable,
                  container: container_expr,
                  body: current_body
                )

                outer_range_stmt = range_stmt
                current_body = context.factory.block_statement(
                  statements: [range_stmt],
                  statement_trailings: ["\n"],
                  lbrace_suffix: "\n",
                  rbrace_prefix: ""
                )
              end

              # Add loops or body to lambda statements
              if outer_range_stmt
                lambda_statements << outer_range_stmt
              else
                lambda_statements.concat(body_statements)
              end

              # Add return statement
              return_expr = context.factory.identifier(name: "result")
              lambda_statements << context.factory.return_statement(expression: return_expr)

              # Generate IIFE lambda body
              body_str = lambda_statements.map(&:to_source).join("\n")

              lambda_expr = context.factory.lambda(
                capture: "&",
                parameters: "",
                specifiers: "",
                body: body_str,
                capture_suffix: "",
                params_suffix: ""
              )

              # Return immediately invoked lambda
              context.factory.function_call(
                callee: lambda_expr,
                arguments: [],
                argument_separators: []
              )
            end

            private

            # Build filter statements as if-continue guards
            def build_filter_statements(node)
              node.filters.map do |filter_expr|
                # Lower filter expression
                condition_expr = context.lower_expression(filter_expr)

                # Negate: if (!(condition)) continue;
                parenthesized = context.factory.parenthesized(expression: condition_expr)
                negated = context.factory.unary_expression(
                  operator: "!",
                  operand: parenthesized,
                  prefix: true,
                  operator_suffix: ""
                )

                # Build continue block
                continue_stmt = context.factory.continue_statement
                continue_block = context.factory.block_statement(
                  statements: [continue_stmt],
                  statement_trailings: ["\n"],
                  lbrace_suffix: "\n",
                  rbrace_prefix: ""
                )

                # Build if statement
                context.factory.if_statement(
                  condition: negated,
                  then_statement: continue_block,
                  else_statement: nil,
                  if_suffix: " ",
                  condition_lparen_suffix: "",
                  condition_rparen_suffix: "",
                  else_prefix: "",
                  else_suffix: ""
                )
              end
            end

            # Build result.push_back(output_expr) statement
            def build_push_back_statement(node)
              result_id = context.factory.identifier(name: "result")
              push_back_member = context.factory.member_access(
                object: result_id,
                operator: ".",
                member: context.factory.identifier(name: "push_back")
              )

              output_arg = context.lower_expression(node.output_expr)
              push_call = context.factory.function_call(
                callee: push_back_member,
                arguments: [output_arg],
                argument_separators: []
              )

              context.factory.expression_statement(expression: push_call)
            end
          end
        end
      end
    end
  end
end
