# frozen_string_literal: true

module MLC
  module Backends
    module Cpp
      module Services
        module Utils
          # Pure functions for analyzing expression and statement complexity
          # No side effects, no dependencies on analyzer state
          module ComplexityAnalysis
            module_function

            # Check if a statement contains control flow constructs
            #
            # @param stmt [SemanticIR::Statement] The statement to check
            # @return [Boolean] Whether the statement has control flow
            def statement_has_control_flow?(stmt)
              case stmt
              when SemanticIR::IfStmt, SemanticIR::MatchStmt, SemanticIR::ForStmt, SemanticIR::WhileStmt
                true
              when SemanticIR::VariableDeclStmt
                stmt.value && expression_has_control_flow?(stmt.value)
              when SemanticIR::ExprStatement
                expression_has_control_flow?(stmt.expression)
              else
                false
              end
            end

            # Check if an expression contains control flow constructs
            #
            # @param expr [SemanticIR::Expression] The expression to check
            # @return [Boolean] Whether the expression has control flow
            def expression_has_control_flow?(expr)
              case expr
              when SemanticIR::IfExpr, SemanticIR::MatchExpr
                true
              when SemanticIR::BlockExpr
                true  # Nested block counts as control flow
              when SemanticIR::CallExpr
                expr.args.any? { |arg| expression_has_control_flow?(arg) }
              when SemanticIR::BinaryExpr
                expression_has_control_flow?(expr.left) || expression_has_control_flow?(expr.right)
              else
                false
              end
            end

            # Check if a statement is simple (no control flow)
            #
            # @param stmt [SemanticIR::Statement] The statement to check
            # @return [Boolean] Whether the statement is simple
            def simple_statement?(stmt)
              case stmt
              when SemanticIR::VariableDeclStmt
                !stmt.value || !expression_has_control_flow?(stmt.value)
              when SemanticIR::ExprStatement
                !expression_has_control_flow?(stmt.expression)
              when SemanticIR::AssignmentStmt
                true
              else
                false
              end
            end

            # Check if all statements in a collection are simple
            #
            # @param statements [Array<SemanticIR::Statement>] The statements to check
            # @return [Boolean] Whether all statements are simple
            def all_statements_simple?(statements)
              statements.all? { |stmt| simple_statement?(stmt) }
            end

            # Check if any statement contains a nested block expression
            #
            # @param statements [Array<SemanticIR::Statement>] The statements to check
            # @return [Boolean] Whether any statement has nested blocks
            def has_nested_blocks?(statements)
              statements.any? do |stmt|
                stmt.is_a?(SemanticIR::VariableDeclStmt) && stmt.value.is_a?(SemanticIR::BlockExpr)
              end
            end

            # Analyze control flow in a block expression
            #
            # @param block_expr [SemanticIR::BlockExpr] The block to analyze
            # @return [Boolean] Whether the block has control flow
            def block_has_control_flow?(block_expr)
              statements_have = block_expr.statements.any? { |stmt| statement_has_control_flow?(stmt) }
              result_has = block_expr.result ? expression_has_control_flow?(block_expr.result) : false
              statements_have || result_has
            end

            # Check if a branch expression is simple (for ternary candidate detection)
            #
            # @param branch [SemanticIR::Expression] The branch expression to check
            # @return [Boolean] Whether the branch is simple
            def simple_branch?(branch)
              return false unless branch

              case branch
              when SemanticIR::LiteralExpr, SemanticIR::VarExpr
                true
              when SemanticIR::BinaryExpr
                simple_branch?(branch.left) && simple_branch?(branch.right)
              when SemanticIR::CallExpr
                branch.args.all? { |arg| simple_branch?(arg) }
              else
                false
              end
            end

            # Check if branches contain nested control flow
            #
            # @param branches [Array<SemanticIR::Expression>] Branches to check
            # @return [Boolean] Whether any branch has nested control flow
            def branches_have_nested_control_flow?(branches)
              branches.compact.any? do |branch|
                branch.is_a?(SemanticIR::IfExpr) ||
                  branch.is_a?(SemanticIR::MatchExpr) ||
                  branch.is_a?(SemanticIR::BlockExpr)
              end
            end

            # Check if a pattern is a constructor-like pattern (for match analysis)
            #
            # @param pattern [Hash] The pattern to check
            # @return [Boolean] Whether the pattern is constructor-like
            def constructor_pattern?(pattern)
              [:constructor, :wildcard, :var].include?(pattern[:kind])
            end

            # Check if a pattern is a regex pattern
            #
            # @param pattern [Hash] The pattern to check
            # @return [Boolean] Whether the pattern is a regex
            def regex_pattern?(pattern)
              pattern[:kind] == :regex
            end
          end
        end
      end
    end
  end
end
