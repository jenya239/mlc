# frozen_string_literal: true

module MLC
  module Representations
    module Semantic
      module Gen
        module Services
          module Utils
            # Pure functions for expression and statement purity analysis
            # No side effects, no dependencies, no state
            #
            # Used for:
            # - Determining if expressions are "pure" (no side effects)
            # - C++20 constexpr compatibility checking
            # - Non-literal type detection (String, collections)
            module PurityAnalysis
              # IO functions that are never pure
              IO_FUNCTION_PATTERN = /^(println|print|read|write|open|close)/.freeze

              # Functions returning non-literal types
              NON_LITERAL_FUNCTION_PATTERN = /^(to_string|format|String)/.freeze

              # Non-literal type names
              NON_LITERAL_TYPES = %w[string String].freeze
              NON_LITERAL_TYPE_PATTERN = /^(Array|Vec|HashMap|HashSet)$/.freeze

              module_function

              # Check if an expression is pure (no side effects)
              #
              # @param expr [SemanticIR::Expression] The expression to check
              # @return [Boolean] Whether the expression is pure
              def pure_expression?(expr)
                case expr
                when MLC::SemanticIR::LiteralExpr, MLC::SemanticIR::VarExpr
                  true
                when MLC::SemanticIR::BinaryExpr
                  pure_expression?(expr.left) && pure_expression?(expr.right)
                when MLC::SemanticIR::UnaryExpr
                  pure_expression?(expr.operand)
                when MLC::SemanticIR::CallExpr
                  pure_call?(expr)
                when MLC::SemanticIR::MemberExpr
                  pure_expression?(expr.object)
                when MLC::SemanticIR::RecordExpr
                  expr.fields.values.all? { |field| pure_expression?(field) }
                when MLC::SemanticIR::BlockExpr
                  pure_block?(expr)
                else
                  false
                end
              end

              # Check if a function call is pure
              #
              # @param call_expr [SemanticIR::CallExpr] The call expression to check
              # @return [Boolean] Whether the call is pure
              def pure_call?(call_expr)
                # Check if function name indicates non-pure operation
                if call_expr.callee.is_a?(MLC::SemanticIR::VarExpr)
                  func_name = call_expr.callee.name
                  return false if func_name =~ IO_FUNCTION_PATTERN
                  return false if func_name =~ NON_LITERAL_FUNCTION_PATTERN
                end

                # Check if return type is non-literal
                return false if non_literal_type?(call_expr.type)

                # Recursively check arguments
                call_expr.args.all? { |arg| pure_expression?(arg) }
              end

              # Check if a type is non-literal (cannot be constexpr in C++20)
              #
              # @param type [SemanticIR::Type] The type to check
              # @return [Boolean] Whether the type is non-literal
              def non_literal_type?(type)
                return false if type.nil?
                return false unless type.respond_to?(:name)

                NON_LITERAL_TYPES.include?(type.name) ||
                  type.name =~ NON_LITERAL_TYPE_PATTERN
              end

              # Check if a block expression is pure
              #
              # @param block_expr [SemanticIR::BlockExpr] The block to check
              # @return [Boolean] Whether the block is pure
              def pure_block?(block_expr)
                statements_pure = block_expr.statements.all? { |stmt| pure_statement?(stmt) }
                result_pure = block_expr.result.nil? || pure_expression?(block_expr.result)
                statements_pure && result_pure
              end

              # Check if a statement is pure
              #
              # @param stmt [SemanticIR::Statement] The statement to check
              # @return [Boolean] Whether the statement is pure
              def pure_statement?(stmt)
                case stmt
                when MLC::SemanticIR::VariableDeclStmt
                  !stmt.mutable && pure_expression?(stmt.value)
                when MLC::SemanticIR::ExprStatement
                  pure_expression?(stmt.expression)
                when MLC::SemanticIR::Block
                  stmt.stmts.all? { |inner| pure_statement?(inner) }
                when MLC::SemanticIR::MatchStmt
                  pure_expression?(stmt.scrutinee) &&
                    stmt.arms.all? do |arm|
                      (arm[:guard].nil? || pure_expression?(arm[:guard])) &&
                        pure_block?(arm[:body])
                    end
                else
                  false
                end
              end
            end
          end
        end
      end
    end
  end
end
