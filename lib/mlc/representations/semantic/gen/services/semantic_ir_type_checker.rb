# frozen_string_literal: true

module MLC
  module Representations
    module Semantic
      module Gen
        module Services
          # SemanticIRTypeChecker - Pure service for SemanticIR node type checking
          # Responsibility: Encapsulate all type checks to avoid explicit class mentions
          # Pattern: Replace node.is_a?(MLC::SemanticIR::VarExpr) with checker.var_expr?(node)
          # Location: Centralized at top level (lib/mlc/services/) for use across semantic_gen and backends
          class SemanticIRTypeChecker
            # Expression nodes
            def literal_expr?(node)
              node.is_a?(MLC::SemanticIR::LiteralExpr)
            end

            def regex_expr?(node)
              node.is_a?(MLC::SemanticIR::RegexExpr)
            end

            def var_expr?(node)
              node.is_a?(MLC::SemanticIR::VarExpr)
            end

            def binary_expr?(node)
              node.is_a?(MLC::SemanticIR::BinaryExpr)
            end

            def unary_expr?(node)
              node.is_a?(MLC::SemanticIR::UnaryExpr)
            end

            def call_expr?(node)
              node.is_a?(MLC::SemanticIR::CallExpr)
            end

            def member_expr?(node)
              node.is_a?(MLC::SemanticIR::MemberExpr)
            end

            def safe_member_expr?(node)
              node.is_a?(MLC::SemanticIR::SafeMemberExpr)
            end

            def safe_call_expr?(node)
              node.is_a?(MLC::SemanticIR::SafeCallExpr)
            end

            def try_expr?(node)
              node.is_a?(MLC::SemanticIR::TryExpr)
            end

            def index_expr?(node)
              node.is_a?(MLC::SemanticIR::IndexExpr)
            end

            def slice_expr?(node)
              node.is_a?(MLC::SemanticIR::SliceExpr)
            end

            def await_expr?(node)
              node.is_a?(MLC::SemanticIR::AwaitExpr)
            end

            def array_literal_expr?(node)
              node.is_a?(MLC::SemanticIR::ArrayLiteralExpr)
            end

            def tuple_expr?(node)
              node.is_a?(MLC::SemanticIR::TupleExpr)
            end

            def tuple_access_expr?(node)
              node.is_a?(MLC::SemanticIR::TupleAccessExpr)
            end

            def symbol_expr?(node)
              node.is_a?(MLC::SemanticIR::SymbolExpr)
            end

            def record_expr?(node)
              node.is_a?(MLC::SemanticIR::RecordExpr)
            end

            def if_expr?(node)
              node.is_a?(MLC::SemanticIR::IfExpr)
            end

            def match_expr?(node)
              node.is_a?(MLC::SemanticIR::MatchExpr)
            end

            def lambda_expr?(node)
              node.is_a?(MLC::SemanticIR::LambdaExpr)
            end

            def block_expr?(node)
              node.is_a?(MLC::SemanticIR::BlockExpr)
            end

            def unsafe_block_expr?(node)
              node.is_a?(MLC::SemanticIR::UnsafeBlockExpr)
            end

            def list_comp_expr?(node)
              node.is_a?(MLC::SemanticIR::ListCompExpr)
            end

            def unit_literal?(node)
              node.is_a?(MLC::SemanticIR::UnitLiteral)
            end

            # Statement nodes
            def block?(node)
              node.is_a?(MLC::SemanticIR::Block)
            end

            def return?(node)
              node.is_a?(MLC::SemanticIR::Return)
            end

            def expr_stmt?(node)
              node.is_a?(MLC::SemanticIR::ExprStatement)
            end

            def var_decl?(node)
              node.is_a?(MLC::SemanticIR::VariableDeclStmt)
            end

            def assignment?(node)
              node.is_a?(MLC::SemanticIR::AssignmentStmt)
            end

            def if_stmt?(node)
              node.is_a?(MLC::SemanticIR::IfStmt)
            end

            def while_stmt?(node)
              node.is_a?(MLC::SemanticIR::WhileStmt)
            end

            def for_stmt?(node)
              node.is_a?(MLC::SemanticIR::ForStmt)
            end

            def break_stmt?(node)
              node.is_a?(MLC::SemanticIR::BreakStmt)
            end

            def continue_stmt?(node)
              node.is_a?(MLC::SemanticIR::ContinueStmt)
            end

            def match_stmt?(node)
              node.is_a?(MLC::SemanticIR::MatchStmt)
            end
          end
        end
      end
    end
  end
end
