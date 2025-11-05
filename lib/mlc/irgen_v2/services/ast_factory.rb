# frozen_string_literal: true

module MLC
  module IRGenV2
    module Services
      # ASTFactory - Centralizes AST node construction for tests and rules
      class ASTFactory
        def int_literal(value:, origin: nil)
          MLC::AST::IntLit.new(value: value, origin: origin)
        end

        def float_literal(value:, origin: nil)
          MLC::AST::FloatLit.new(value: value, origin: origin)
        end

        def string_literal(value:, origin: nil)
          MLC::AST::StringLit.new(value: value, origin: origin)
        end

        def regex_literal(pattern:, flags: "", origin: nil)
          MLC::AST::RegexLit.new(pattern: pattern, flags: flags, origin: origin)
        end

        def unit_literal(origin: nil)
          MLC::AST::UnitLit.new(origin: origin)
        end

        def var_ref(name:, origin: nil)
          MLC::AST::VarRef.new(name: name, origin: origin)
        end

        def member_access(object:, member:, origin: nil)
          MLC::AST::MemberAccess.new(object: object, member: member, origin: origin)
        end

        def index_access(object:, index:, origin: nil)
          MLC::AST::IndexAccess.new(object: object, index: index, origin: origin)
        end

        def call(callee:, args:, origin: nil)
          MLC::AST::Call.new(callee: callee, args: args, origin: origin)
        end

        def unary(op:, operand:, origin: nil)
          MLC::AST::UnaryOp.new(op: op, operand: operand, origin: origin)
        end

        def binary(op:, left:, right:, origin: nil)
          MLC::AST::BinaryOp.new(op: op, left: left, right: right, origin: origin)
        end

        def lambda(params:, body:, origin: nil)
          MLC::AST::Lambda.new(params: params, body: body, origin: origin)
        end

        def lambda_param(name:, type: nil, origin: nil)
          MLC::AST::LambdaParam.new(name: name, type: type, origin: origin)
        end

        def let(name:, value:, body:, mutable: false, type: nil, origin: nil)
          MLC::AST::Let.new(name: name, value: value, body: body, mutable: mutable, type: type, origin: origin)
        end

        def record_literal(type_name:, fields:, origin: nil)
          MLC::AST::RecordLit.new(type_name: type_name, fields: fields, origin: origin)
        end

        def array_literal(elements:, origin: nil)
          MLC::AST::ArrayLiteral.new(elements: elements, origin: origin)
        end

        def generator(var_name:, iterable:, origin: nil)
          MLC::AST::Generator.new(var_name: var_name, iterable: iterable, origin: origin)
        end

        def list_comprehension(output_expr:, generators:, filters: [], origin: nil)
          MLC::AST::ListComprehension.new(output_expr: output_expr, generators: generators, filters: filters, origin: origin)
        end

        def block_expr(statements:, result_expr:, origin: nil)
          MLC::AST::BlockExpr.new(statements: statements, result_expr: result_expr, origin: origin)
        end

        def expr_stmt(expr:, origin: nil)
          MLC::AST::ExprStmt.new(expr: expr, origin: origin)
        end

        def variable_decl(name:, value:, mutable: false, type: nil, origin: nil)
          MLC::AST::VariableDecl.new(name: name, value: value, mutable: mutable, type: type, origin: origin)
        end

        def return_stmt(expr: nil, origin: nil)
          MLC::AST::Return.new(expr: expr, origin: origin)
        end

        def do_expr(body:, origin: nil)
          MLC::AST::DoExpr.new(body: body, origin: origin)
        end

        def assignment(target:, value:, origin: nil)
          MLC::AST::Assignment.new(target: target, value: value, origin: origin)
        end

        def break_stmt(origin: nil)
          MLC::AST::Break.new(origin: origin)
        end

        def continue_stmt(origin: nil)
          MLC::AST::Continue.new(origin: origin)
        end

        def for_loop(var_name:, iterable:, body:, origin: nil)
          MLC::AST::ForLoop.new(var_name: var_name, iterable: iterable, body: body, origin: origin)
        end

        def while_loop(condition:, body:, origin: nil)
          MLC::AST::WhileLoop.new(condition: condition, body: body, origin: origin)
        end

        def match_expr(scrutinee:, arms:, origin: nil)
          MLC::AST::MatchExpr.new(scrutinee: scrutinee, arms: arms, origin: origin)
        end

        def match_arm(pattern:, body:, guard: nil)
          { pattern: pattern, body: body, guard: guard }
        end

        def pattern_wildcard(origin: nil)
          MLC::AST::Pattern.new(kind: :wildcard, data: {}, origin: origin)
        end

        def pattern_literal(value:, origin: nil)
          MLC::AST::Pattern.new(kind: :literal, data: { value: value }, origin: origin)
        end

        def pattern_var(name:, origin: nil)
          MLC::AST::Pattern.new(kind: :var, data: { name: name }, origin: origin)
        end

        def pattern_constructor(name:, fields: [], origin: nil)
          MLC::AST::Pattern.new(
            kind: :constructor,
            data: { name: name, fields: Array(fields) },
            origin: origin
          )
        end

        def pattern_regex(pattern:, flags: "", bindings: [], origin: nil)
          MLC::AST::Pattern.new(
            kind: :regex,
            data: {
              pattern: pattern,
              flags: flags,
              bindings: Array(bindings)
            },
            origin: origin
          )
        end
      end
    end
  end
end
