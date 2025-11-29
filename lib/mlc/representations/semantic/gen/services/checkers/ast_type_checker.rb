# frozen_string_literal: true

module MLC
  module Representations
    module Semantic
      module Gen
        module Services
          # ASTTypeChecker - Pure service for AST node type checking
          # Responsibility: Encapsulate all type checks to avoid explicit class mentions
          # Pattern: Replace node.is_a?(MLC::Source::AST::VarRef) with ast_checker.var_ref?(node)
          class ASTTypeChecker
            # Check if node is a variable reference
            def var_ref?(node)
              node.is_a?(MLC::Source::AST::VarRef)
            end

            # Check if node is a member access
            def member_access?(node)
              node.is_a?(MLC::Source::AST::MemberAccess)
            end

            # Check if node is a safe member access (optional chaining)
            def safe_member_access?(node)
              node.is_a?(MLC::Source::AST::SafeMemberAccess)
            end

            # Check if node is a safe call (optional chaining)
            def safe_call?(node)
              node.is_a?(MLC::Source::AST::SafeCall)
            end

            # Check if node is a try expression (error propagation)
            def try_expr?(node)
              node.is_a?(MLC::Source::AST::TryExpr)
            end

            # Check if node is a function call
            def call?(node)
              node.is_a?(MLC::Source::AST::Call)
            end

            # Check if node is a literal
            def literal?(node)
              node.is_a?(MLC::Source::AST::Literal)
            end

            def int_literal?(node)
              node.is_a?(MLC::Source::AST::IntLit)
            end

            def float_literal?(node)
              node.is_a?(MLC::Source::AST::FloatLit)
            end

            def string_literal?(node)
              node.is_a?(MLC::Source::AST::StringLit)
            end

            def string_interpolation?(node)
              node.is_a?(MLC::Source::AST::StringInterpolation)
            end

            def regex_literal?(node)
              node.is_a?(MLC::Source::AST::RegexLit)
            end

            def unit_literal?(node)
              node.is_a?(MLC::Source::AST::UnitLit)
            end

            def literal_kind(node)
              return :int if int_literal?(node)
              return :float if float_literal?(node)
              return :string if string_literal?(node)
              return :regex if regex_literal?(node)
              return :unit if unit_literal?(node)
              nil
            end

            def let?(node)
              node.is_a?(MLC::Source::AST::Let)
            end

            def if_expr?(node)
              node.is_a?(MLC::Source::AST::IfExpr)
            end

            def match_expr?(node)
              node.is_a?(MLC::Source::AST::MatchExpr)
            end

            # Check if node is a binary operation
            def binary_op?(node)
              node.is_a?(MLC::Source::AST::BinaryOp)
            end

            # Check if node is a unary operation
            def unary_op?(node)
              node.is_a?(MLC::Source::AST::UnaryOp)
            end

            def record_literal?(node)
              node.is_a?(MLC::Source::AST::RecordLit)
            end

            def array_literal?(node)
              node.is_a?(MLC::Source::AST::ArrayLiteral)
            end

            def tuple_literal?(node)
              node.is_a?(MLC::Source::AST::TupleLit)
            end

            def symbol_literal?(node)
              node.is_a?(MLC::Source::AST::SymbolLit)
            end

            def block_expr?(node)
              node.is_a?(MLC::Source::AST::BlockExpr)
            end

            def do_expr?(node)
              node.is_a?(MLC::Source::AST::DoExpr)
            end

            def for_loop?(node)
              node.is_a?(MLC::Source::AST::ForLoop)
            end

            def while_loop_expr?(node)
              node.is_a?(MLC::Source::AST::WhileLoop)
            end

            def index_access?(node)
              node.is_a?(MLC::Source::AST::IndexAccess)
            end

            def slice_access?(node)
              node.is_a?(MLC::Source::AST::SliceAccess)
            end

            def tuple_access?(node)
              node.is_a?(MLC::Source::AST::TupleAccess)
            end

            def lambda?(node)
              node.is_a?(MLC::Source::AST::Lambda)
            end

            def list_comprehension?(node)
              node.is_a?(MLC::Source::AST::ListComprehension)
            end

            def unsafe_block?(node)
              node.is_a?(MLC::Source::AST::UnsafeBlock)
            end

            def block_statement?(node)
              node.is_a?(MLC::Source::AST::Block)
            end

            def expr_stmt?(node)
              node.is_a?(MLC::Source::AST::ExprStmt)
            end

            def variable_decl_statement?(node)
              node.is_a?(MLC::Source::AST::VariableDecl)
            end

            def return_statement?(node)
              node.is_a?(MLC::Source::AST::Return)
            end

            def assignment_statement?(node)
              node.is_a?(MLC::Source::AST::Assignment)
            end

            def break_statement?(node)
              node.is_a?(MLC::Source::AST::Break)
            end

            def continue_statement?(node)
              node.is_a?(MLC::Source::AST::Continue)
            end

            # Check if node is an if statement
            def if_statement?(node)
              node.is_a?(MLC::Source::AST::IfStmt)
            end

            # Check if node is a while statement
            def while_statement?(node)
              node.is_a?(MLC::Source::AST::WhileStmt)
            end

            # Check if node is a for statement
            def for_statement?(node)
              node.is_a?(MLC::Source::AST::ForLoop)
            end

            # Check if node is a variable declaration
            def var_decl?(node)
              node.is_a?(MLC::Source::AST::VarDecl)
            end

            # Check if node is a function definition
            def function_def?(node)
              node.is_a?(MLC::Source::AST::FunctionDef)
            end

            # Check if node is a class definition
            def class_def?(node)
              node.is_a?(MLC::Source::AST::ClassDef)
            end

            # Check if node is an await expression
            def await_expr?(node)
              node.is_a?(MLC::Source::AST::AwaitExpr)
            end

            # Check if node is a range expression
            def range_expr?(node)
              node.is_a?(MLC::Source::AST::RangeExpr)
            end
          end
        end
      end
    end
  end
end
