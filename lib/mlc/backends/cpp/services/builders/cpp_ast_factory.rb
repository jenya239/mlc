# frozen_string_literal: true

module MLC
  module Backends
    module Cpp
      module Services
        module Builders
          # CppAstFactory - Pure service for CppAst::Nodes construction
          # Responsibility: Encapsulate all CppAst::Nodes::*.new() calls
          # Pattern: Replace CppAst::Nodes::Identifier.new(name: x) with factory.identifier(name: x)
          class CppAstFactory
            # Identifier
            def identifier(name:)
              CppAst::Nodes::Identifier.new(name: name)
            end

            # Literals
            def number_literal(value:)
              CppAst::Nodes::NumberLiteral.new(value: value)
            end

            def string_literal(value:)
              CppAst::Nodes::StringLiteral.new(value: value)
            end

            def boolean_literal(value:)
              CppAst::Nodes::BooleanLiteral.new(value: value)
            end

            # Binary expression
            def binary_expression(left:, operator:, right:, operator_prefix: " ", operator_suffix: " ")
              CppAst::Nodes::BinaryExpression.new(
                left: left,
                operator: operator,
                right: right,
                operator_prefix: operator_prefix,
                operator_suffix: operator_suffix
              )
            end

            # Unary expression
            def unary_expression(operator:, operand:, operator_suffix: "", prefix: true)
              CppAst::Nodes::UnaryExpression.new(
                operator: operator,
                operand: operand,
                operator_suffix: operator_suffix,
                prefix: prefix
              )
            end

            # Function call
            def function_call(callee:, arguments:, argument_separators: [], lparen_suffix: "", rparen_prefix: "")
              CppAst::Nodes::FunctionCallExpression.new(
                callee: callee,
                arguments: arguments,
                argument_separators: argument_separators,
                lparen_suffix: lparen_suffix,
                rparen_prefix: rparen_prefix
              )
            end

            # Member access
            def member_access(object:, operator:, member:, operator_prefix: "", operator_suffix: "")
              CppAst::Nodes::MemberAccessExpression.new(
                object: object,
                operator: operator,
                member: member,
                operator_prefix: operator_prefix,
                operator_suffix: operator_suffix
              )
            end

            # Array subscript
            def array_subscript(array:, index:, lbracket_suffix: "", rbracket_prefix: "")
              CppAst::Nodes::ArraySubscriptExpression.new(
                array: array,
                index: index,
                lbracket_suffix: lbracket_suffix,
                rbracket_prefix: rbracket_prefix
              )
            end

            # Brace initializer (used for arrays and records)
            def brace_initializer(type:, arguments:, argument_separators: [],
                                  lbrace_prefix: "", lbrace_suffix: "", rbrace_prefix: "")
              CppAst::Nodes::BraceInitializerExpression.new(
                type: type,
                arguments: arguments,
                argument_separators: argument_separators,
                lbrace_prefix: lbrace_prefix,
                lbrace_suffix: lbrace_suffix,
                rbrace_prefix: rbrace_prefix
              )
            end

            # Ternary expression
            def ternary(condition:, true_expression:, false_expression:,
                        question_prefix: " ", question_suffix: " ",
                        colon_prefix: " ", colon_suffix: " ")
              CppAst::Nodes::TernaryExpression.new(
                condition: condition,
                true_expression: true_expression,
                false_expression: false_expression,
                question_prefix: question_prefix,
                question_suffix: question_suffix,
                colon_prefix: colon_prefix,
                colon_suffix: colon_suffix
              )
            end

            # Lambda expression
            def lambda(capture: "", parameters: "", specifiers: "", body: "",
                       capture_suffix: "", params_suffix: "")
              CppAst::Nodes::LambdaExpression.new(
                capture: capture,
                parameters: parameters,
                specifiers: specifiers,
                body: body,
                capture_suffix: capture_suffix,
                params_suffix: params_suffix
              )
            end

            # Parenthesized expression
            def parenthesized(expression:, open_paren_suffix: "", close_paren_prefix: "")
              CppAst::Nodes::ParenthesizedExpression.new(
                expression: expression,
                open_paren_suffix: open_paren_suffix,
                close_paren_prefix: close_paren_prefix
              )
            end

            # Raw expression (for GCC extensions like ({ ... }))
            def raw_expression(code:, leading_trivia: "")
              CppAst::Nodes::RawExpression.new(
                code: code,
                leading_trivia: leading_trivia
              )
            end

            # Statement nodes

            # Expression statement
            def expression_statement(expression:, leading_trivia: "")
              CppAst::Nodes::ExpressionStatement.new(
                expression: expression,
                leading_trivia: leading_trivia
              )
            end

            # Return statement
            def return_statement(expression: nil, keyword_suffix: " ", leading_trivia: "")
              CppAst::Nodes::ReturnStatement.new(
                expression: expression,
                keyword_suffix: keyword_suffix,
                leading_trivia: leading_trivia
              )
            end

            # Variable declaration
            def variable_declaration(type:, declarators:, declarator_separators: [], type_suffix: " ", prefix_modifiers: "", leading_trivia: "")
              CppAst::Nodes::VariableDeclaration.new(
                type: type,
                declarators: declarators,
                declarator_separators: declarator_separators,
                type_suffix: type_suffix,
                prefix_modifiers: prefix_modifiers,
                leading_trivia: leading_trivia
              )
            end

            # Block statement
            def block_statement(statements:, statement_trailings:, lbrace_suffix: "\n", rbrace_prefix: "", leading_trivia: "")
              CppAst::Nodes::BlockStatement.new(
                statements: statements,
                statement_trailings: statement_trailings,
                lbrace_suffix: lbrace_suffix,
                rbrace_prefix: rbrace_prefix,
                leading_trivia: leading_trivia
              )
            end

            # Range-for statement
            def range_for_statement(variable:, container:, body:, leading_trivia: "")
              CppAst::Nodes::RangeForStatement.new(
                variable: variable,
                container: container,
                body: body,
                leading_trivia: leading_trivia
              )
            end

            # If statement
            def if_statement(condition:, then_statement:, else_statement: nil, if_suffix: " ",
                            condition_lparen_suffix: "", condition_rparen_suffix: "",
                            else_prefix: " ", else_suffix: " ", leading_trivia: "")
              CppAst::Nodes::IfStatement.new(
                condition: condition,
                then_statement: then_statement,
                else_statement: else_statement,
                if_suffix: if_suffix,
                condition_lparen_suffix: condition_lparen_suffix,
                condition_rparen_suffix: condition_rparen_suffix,
                else_prefix: else_prefix,
                else_suffix: else_suffix,
                leading_trivia: leading_trivia
              )
            end

            # Continue statement
            def continue_statement(leading_trivia: "")
              CppAst::Nodes::ContinueStatement.new(leading_trivia: leading_trivia)
            end

            # Raw statement (for custom C++ code)
            def raw_statement(code:, leading_trivia: "")
              CppAst::Nodes::RawStatement.new(code: code, leading_trivia: leading_trivia)
            end

            # Break statement
            def break_statement(leading_trivia: "")
              CppAst::Nodes::BreakStatement.new(leading_trivia: leading_trivia)
            end
          end
        end
      end
    end
  end
end
