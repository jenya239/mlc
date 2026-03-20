# frozen_string_literal: true

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
              analyzer = MLC::Backends::Cpp::BlockComplexityAnalyzer.new(node)
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
                if node.result && !unit_literal?(node.result)
                  lower_expression(node.result)
                end
              else
                # Fallback to IIFE (conservative)
                lower_as_iife(node)
              end
            end

            private

            # IIFE strategy: [&]() -> ReturnType { ... return val; }()
            def lower_as_iife(block_expr)
              snapshot = context.snapshot_declared_variables
              var_snapshot = context.snapshot_var_type_map
              statements = lower_statements(block_expr, emit_return: true)
              context.restore_declared_variables(snapshot)
              context.restore_var_type_map(var_snapshot)
              body_lines = statements.map { |stmt| "  #{stmt.to_source}" }
              lambda_body = "\n#{body_lines.join("\n")}\n"

              # Add explicit return type so `return {}` resolves correctly and IIFE returns correct type.
              ret_type = nil
              type_to_map = (block_expr.result && !unit_literal?(block_expr.result)) ? block_expr.result.type : nil
              type_to_map ||= block_expr.type
              if type_to_map && !unit_type?(type_to_map)
                mapped = context.map_type(type_to_map)
                skip = mapped.nil? || mapped.empty? || mapped == "auto" || mapped.include?("function<") || mapped == "void"
                ret_type = mapped unless skip
              end

              lambda_expr = context.factory.lambda(
                capture: "&",
                parameters: "",
                specifiers: "",
                body: lambda_body,
                capture_suffix: "",
                params_suffix: "",
                return_type: ret_type
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
              if block_expr.result && !unit_literal?(block_expr.result)
                if context.should_lower_as_statement?(block_expr.result)
                  statements << lower_statement(MLC::SemanticIR::ExprStatement.new(expression: block_expr.result, origin: block_expr.result.origin))
                else
                  result_expr = lower_expression(block_expr.result)
                  statements << context.factory.expression_statement(expression: result_expr)
                end
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
              statements = block_expr.statements.flat_map do |stmt|
                result = lower_statement(stmt)
                result.is_a?(CppAst::Nodes::Program) ? result.statements : [result]
              end

              # Skip unit literals - they represent void/no value
              if block_expr.result && !unit_literal?(block_expr.result)
                if context.should_lower_as_statement?(block_expr.result)
                  statements << lower_statement(MLC::SemanticIR::ExprStatement.new(expression: block_expr.result, origin: block_expr.result.origin))
                else
                  result_expr = lower_expression(block_expr.result)
                  statements << if emit_return
                                  context.factory.return_statement(expression: result_expr)
                                else
                                  context.factory.expression_statement(expression: result_expr)
                                end
                end
              end

              statements
            end

            # Check if expression is a unit literal (void)
            def unit_literal?(expr)
              return true if expr.is_a?(MLC::SemanticIR::UnitLiteral)
              expr.is_a?(MLC::SemanticIR::LiteralExpr) && expr.value == :unit
            end

            def unit_type?(type)
              type.is_a?(MLC::SemanticIR::UnitType) ||
                (type.respond_to?(:name) && %w[unit void].include?(type.name.to_s))
            end
          end
        end
      end
    end
  end
end
