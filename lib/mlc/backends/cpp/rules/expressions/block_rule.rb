# frozen_string_literal: true

require_relative "../expression_rule"
require_relative "../../block_complexity_analyzer"

module MLC
  module Backends
    module Cpp
      module Rules
        module Expressions
          # Rule for lowering SemanticIR::BlockExpr to C++ blocks
          # Supports multiple strategies: IIFE, GCC extensions, inline
          class BlockRule < ExpressionRule
            def applies?(node)
              context.checker.block_expr?(node)
            end

            def apply(node)
              # Analyze block complexity to choose lowering strategy
              analyzer = MLC::Backend::BlockComplexityAnalyzer.new(node)
              strategy = context.runtime_policy.strategy_for_block(analyzer)

              case strategy
              when :iife
                lower_as_iife(node)
              when :scope_tmp
                lower_as_scope_tmp(node)
              when :gcc_expr
                lower_as_gcc_expr(node)
              when :inline
                # For trivial blocks, just return the result expression
                node.result ? lower_expression(node.result) : nil
              else
                # Fallback to IIFE (conservative)
                lower_as_iife(node)
              end
            end

            private

            # IIFE strategy: [&]() { ... return val; }()
            def lower_as_iife(block_expr)
              statements = lower_statements(block_expr, emit_return: true)
              body_lines = statements.map { |stmt| "  #{stmt.to_source}" }
              lambda_body = "\n#{body_lines.join("\n")}\n"

              lambda_expr = context.factory.lambda(
                capture: "&",
                parameters: "",
                specifiers: "",
                body: lambda_body,
                capture_suffix: "",
                params_suffix: ""
              )

              context.factory.function_call(
                callee: lambda_expr,
                arguments: [],
                argument_separators: []
              )
            end

            # Scope + tmp strategy: delegates to GCC or IIFE based on policy
            def lower_as_scope_tmp(block_expr)
              # For expression context, we need GCC extension or IIFE
              # Prefer GCC extension if enabled
              if context.runtime_policy.use_gcc_extensions
                lower_as_gcc_expr(block_expr)
              else
                # For standard C++, use IIFE (compiler will optimize)
                lower_as_iife(block_expr)
              end
            end

            # GCC expression statement: ({ ... })
            def lower_as_gcc_expr(block_expr)
              # GCC/Clang extension: ({ statement; ... value; })
              # This creates a compound statement that returns a value
              # Example: ({ int x = 1; int y = 2; x + y; })

              statements = []

              # Lower all statements
              block_expr.statements.each do |stmt|
                statements << lower_statement(stmt)
              end

              # Add result expression as final statement (no return needed in GCC expr)
              if block_expr.result && !is_unit_literal?(block_expr.result)
                result_expr = lower_expression(block_expr.result)
                statements << context.factory.expression_statement(expression: result_expr)
              end

              # Generate compound expression: ({ ... })
              # Format with proper indentation
              body_lines = statements.map { |stmt| "  #{stmt.to_source}" }
              compound_body = "\n#{body_lines.join("\n")}\n"

              # Return as raw code wrapped in parentheses
              context.factory.raw_expression(code: "(#{compound_body})")
            end

            # Helper to lower block statements with optional return
            def lower_statements(block_expr, emit_return: true)
              statements = block_expr.statements.map { |stmt|
                lower_statement(stmt)
              }

              if block_expr.result
                # Skip unit literals - they represent void/no value
                unless is_unit_literal?(block_expr.result)
                  result_expr = lower_expression(block_expr.result)
                  if emit_return
                    statements << context.factory.return_statement(expression: result_expr)
                  else
                    statements << context.factory.expression_statement(expression: result_expr)
                  end
                end
              end

              statements
            end

            # Check if expression is a unit literal (void)
            def is_unit_literal?(expr)
              # UnitLiteral check - need to verify SemanticIR has this type
              expr.is_a?(MLC::SemanticIR::LiteralExpr) && expr.value == :unit
            end
          end
        end
      end
    end
  end
end
