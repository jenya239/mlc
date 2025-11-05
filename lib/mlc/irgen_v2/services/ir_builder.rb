# frozen_string_literal: true

module MLC
  module IRGenV2
    module Services
      # IRBuilder - Factory for HighIR node construction
      # Responsibility: Encapsulate all HighIR node construction
      # Pattern: Replace MLC::HighIR::Var.new(...) with ir_builder.var(...)
      class IRBuilder
        # Build a variable reference node
        def var(name:, type:, origin: nil)
          MLC::HighIR::VarExpr.new(
            origin: origin,
            name: name,
            type: type
          )
        end

        # Build a type node
        def type(kind:, name:, origin: nil)
          MLC::HighIR::Type.new(
            origin: origin,
            kind: kind,
            name: name
          )
        end

        # Build a function type (by name only)
        def func_type(name:, origin: nil)
          type(kind: :func, name: name, origin: origin)
        end

        # Build a function type from parameter and return types
        def function_type(params:, return_type:, origin: nil)
          MLC::HighIR::Builder.function_type(params, return_type, origin: origin)
        end

        # Build a primitive type
        def prim_type(name:, origin: nil)
          type(kind: :prim, name: name, origin: origin)
        end

        # Build a struct type
        def struct_type(name:, origin: nil)
          type(kind: :struct, name: name, origin: origin)
        end

        # Build a record type
        def record_type(name:, fields:, origin: nil)
          MLC::HighIR::Builder.record_type(name, fields, origin: origin)
        end

        # Build an array type
        def array_type(element_type:, origin: nil)
          MLC::HighIR::Builder.array_type(element_type, origin: origin)
        end

        # Build a literal node
        def literal(value:, type:, origin: nil)
          MLC::HighIR::LiteralExpr.new(
            origin: origin,
            value: value,
            type: type
          )
        end

        # Build a binary operation node
        def binary_op(op:, left:, right:, type:, origin: nil)
          MLC::HighIR::BinaryExpr.new(
            origin: origin,
            op: op,
            left: left,
            right: right,
            type: type
          )
        end

        # Build a unary operation node
        def unary_op(op:, operand:, type:, origin: nil)
          MLC::HighIR::UnaryExpr.new(
            origin: origin,
            op: op,
            operand: operand,
            type: type
          )
        end

        # Build a call node
        def call(func:, args:, type:, origin: nil)
          MLC::HighIR::CallExpr.new(
            origin: origin,
            callee: func,
            args: args,
            type: type
          )
        end

        # Build a member access node
        def member(object:, member:, type:, origin: nil)
          MLC::HighIR::Builder.member(object, member, type, origin: origin)
        end

        # Build an if node
        def if_node(cond:, then_branch:, else_branch:, type:, origin: nil)
          MLC::HighIR::If.new(
            origin: origin,
            cond: cond,
            then_branch: then_branch,
            else_branch: else_branch,
            type: type
          )
        end

        def if_expr(cond:, then_branch:, else_branch:, type:, origin: nil)
          MLC::HighIR::Builder.if_expr(cond, then_branch, else_branch, type, origin: origin)
        end

        # Build a while node
        def while_node(cond:, body:, origin: nil)
          MLC::HighIR::While.new(
            origin: origin,
            cond: cond,
            body: body
          )
        end

        def while_stmt(condition:, body:, origin: nil)
          MLC::HighIR::Builder.while_stmt(condition, body, origin: origin)
        end

        # Build a for node
        def for_node(var:, range:, body:, origin: nil)
          MLC::HighIR::For.new(
            origin: origin,
            var: var,
            range: range,
            body: body
          )
        end

        def for_stmt(var_name:, var_type:, iterable:, body:, origin: nil)
          MLC::HighIR::Builder.for_stmt(var_name, var_type, iterable, body, origin: origin)
        end

        # Build a return node
        def return_node(value:, origin: nil)
          MLC::HighIR::Return.new(
            origin: origin,
            value: value
          )
        end

        # Build a break node
        def break_node(origin: nil)
          MLC::HighIR::Break.new(origin: origin)
        end

        # Build a continue node
        def continue_node(origin: nil)
          MLC::HighIR::Continue.new(origin: origin)
        end

        # Build a block node
        def block(statements:, origin: nil)
          MLC::HighIR::Block.new(
            stmts: statements,
            origin: origin
          )
        end

        def block_expr(statements:, result:, type:, origin: nil)
          MLC::HighIR::Builder.block_expr(statements, result, type, origin: origin)
        end

        def variable_decl_stmt(name:, type:, value:, mutable: false, origin: nil)
          MLC::HighIR::Builder.variable_decl_stmt(name, type, value, mutable: mutable, origin: origin)
        end

        def expr_statement(expression:, origin: nil)
          MLC::HighIR::Builder.expr_statement(expression, origin: origin)
        end

        def assignment_stmt(target:, value:, origin: nil)
          MLC::HighIR::Builder.assignment_stmt(target, value, origin: origin)
        end

        # Build a variable declaration node
        def var_decl(name:, value:, type:, origin: nil)
          MLC::HighIR::VarDecl.new(
            origin: origin,
            name: name,
            value: value,
            type: type
          )
        end

        # Build a function definition node
        def function_def(name:, params:, body:, return_type:, origin: nil)
          MLC::HighIR::FunctionDef.new(
            origin: origin,
            name: name,
            params: params,
            body: body,
            return_type: return_type
          )
        end

        def regex(pattern:, flags:, type:, origin: nil)
          MLC::HighIR::RegexExpr.new(
            origin: origin,
            pattern: pattern,
            flags: flags,
            type: type
          )
        end

        def record_expr(type_name:, fields:, type:, origin: nil)
          MLC::HighIR::Builder.record(type_name, fields, type, origin: origin)
        end

        def array_literal(elements:, type:, origin: nil)
          MLC::HighIR::ArrayLiteralExpr.new(elements: elements, type: type, origin: origin)
        end

        def match_expr(scrutinee:, arms:, type:, origin: nil)
          MLC::HighIR::Builder.match_expr(scrutinee, arms, type, origin: origin)
        end

        def match_stmt(scrutinee:, arms:, origin: nil)
          MLC::HighIR::Builder.match_stmt(scrutinee, arms, origin: origin)
        end

        # Build a unit value
        def unit_literal(origin: nil)
          MLC::HighIR::UnitLiteral.new(origin: origin)
        end
      end
    end
  end
end
