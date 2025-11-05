# frozen_string_literal: true

module MLC
  module IRGenV2
    module Services
      # ASTTypeChecker - Pure service for AST node type checking
      # Responsibility: Encapsulate all type checks to avoid explicit class mentions
      # Pattern: Replace node.is_a?(MLC::AST::VarRef) with ast_checker.var_ref?(node)
      class ASTTypeChecker
        # Check if node is a variable reference
        def var_ref?(node)
          node.is_a?(MLC::AST::VarRef)
        end

        # Check if node is a member access
        def member_access?(node)
          node.is_a?(MLC::AST::MemberAccess)
        end

        # Check if node is a function call
        def call?(node)
          node.is_a?(MLC::AST::Call)
        end

        # Check if node is a literal
        def literal?(node)
          node.is_a?(MLC::AST::Literal)
        end

        def int_literal?(node)
          node.is_a?(MLC::AST::IntLit)
        end

        def float_literal?(node)
          node.is_a?(MLC::AST::FloatLit)
        end

        def string_literal?(node)
          node.is_a?(MLC::AST::StringLit)
        end

        def regex_literal?(node)
          node.is_a?(MLC::AST::RegexLit)
        end

        def unit_literal?(node)
          node.is_a?(MLC::AST::UnitLit)
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
          node.is_a?(MLC::AST::Let)
        end

        def if_expr?(node)
          node.is_a?(MLC::AST::IfExpr)
        end

        # Check if node is a binary operation
        def binary_op?(node)
          node.is_a?(MLC::AST::BinaryOp)
        end

        # Check if node is a unary operation
        def unary_op?(node)
          node.is_a?(MLC::AST::UnaryOp)
        end

        def record_literal?(node)
          node.is_a?(MLC::AST::RecordLit)
        end

        def array_literal?(node)
          node.is_a?(MLC::AST::ArrayLiteral)
        end

        def block_expr?(node)
          node.is_a?(MLC::AST::BlockExpr)
        end

        def do_expr?(node)
          node.is_a?(MLC::AST::DoExpr)
        end

        def expr_stmt?(node)
          node.is_a?(MLC::AST::ExprStmt)
        end

        def variable_decl_statement?(node)
          node.is_a?(MLC::AST::VariableDecl)
        end

        def return_statement?(node)
          node.is_a?(MLC::AST::Return)
        end

        def assignment_statement?(node)
          node.is_a?(MLC::AST::Assignment)
        end

        def break_statement?(node)
          node.is_a?(MLC::AST::Break)
        end

        def continue_statement?(node)
          node.is_a?(MLC::AST::Continue)
        end

        # Check if node is an if statement
        def if_statement?(node)
          node.is_a?(MLC::AST::If)
        end

        # Check if node is a while statement
        def while_statement?(node)
          node.is_a?(MLC::AST::While)
        end

        # Check if node is a for statement
        def for_statement?(node)
          node.is_a?(MLC::AST::For)
        end

        # Check if node is a return statement
        def return_statement?(node)
          node.is_a?(MLC::AST::Return)
        end

        # Check if node is a break statement
        def break_statement?(node)
          node.is_a?(MLC::AST::Break)
        end

        # Check if node is a continue statement
        def continue_statement?(node)
          node.is_a?(MLC::AST::Continue)
        end

        # Check if node is a variable declaration
        def var_decl?(node)
          node.is_a?(MLC::AST::VarDecl)
        end

        # Check if node is a function definition
        def function_def?(node)
          node.is_a?(MLC::AST::FunctionDef)
        end

        # Check if node is a class definition
        def class_def?(node)
          node.is_a?(MLC::AST::ClassDef)
        end
      end
    end
  end
end
