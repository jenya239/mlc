# frozen_string_literal: true

module MLC
  module Representations
    module Semantic
      module Gen
        module Services
          module Features
            # Rewrites Call / SafeCall with direct VarRef("_") arguments into Lambda (E6).
            module PartialApplicationDesugar
              class << self
                def rewrite(node, factory:, serial:)
                  Rewriter.new(factory: factory, serial: serial).rewrite_expression(node)
                end
              end

              # rubocop:disable Metrics/ClassLength
              class Rewriter
                A = MLC::Source::AST

                def initialize(factory:, serial:)
                  @factory = factory
                  @serial = serial
                end

                def rewrite_expression(node)
                  return nil unless node

                  case node
                  when A::Block
                    A::Block.new(stmts: node.stmts.map { |statement| rewrite_statement(statement) }, origin: node.origin)
                  when A::Call
                    after_children = A::Call.new(
                      callee: rewrite_expression(node.callee),
                      args: node.args.map { |argument| rewrite_argument(argument) },
                      origin: node.origin
                    )
                    wrap_call_if_partial(after_children)
                  when A::SafeCall
                    after_children = A::SafeCall.new(
                      object: rewrite_expression(node.object),
                      method_name: node.method_name,
                      args: node.args.map { |argument| rewrite_argument(argument) },
                      origin: node.origin
                    )
                    wrap_safe_call_if_partial(after_children)
                  when A::BinaryOp
                    A::BinaryOp.new(
                      op: node.op,
                      left: rewrite_expression(node.left),
                      right: rewrite_expression(node.right),
                      origin: node.origin
                    )
                  when A::UnaryOp
                    A::UnaryOp.new(
                      op: node.op,
                      operand: rewrite_expression(node.operand),
                      origin: node.origin
                    )
                  when A::MemberAccess
                    A::MemberAccess.new(
                      object: rewrite_expression(node.object),
                      member: node.member,
                      origin: node.origin
                    )
                  when A::SafeMemberAccess
                    A::SafeMemberAccess.new(
                      object: rewrite_expression(node.object),
                      member: node.member,
                      origin: node.origin
                    )
                  when A::IndexAccess
                    A::IndexAccess.new(
                      object: rewrite_expression(node.object),
                      index: rewrite_expression(node.index),
                      origin: node.origin
                    )
                  when A::SliceAccess
                    A::SliceAccess.new(
                      object: rewrite_expression(node.object),
                      start_index: node.start_index ? rewrite_expression(node.start_index) : nil,
                      end_index: node.end_index ? rewrite_expression(node.end_index) : nil,
                      origin: node.origin
                    )
                  when A::IfExpr
                    A::IfExpr.new(
                      condition: rewrite_expression(node.condition),
                      then_branch: rewrite_expression(node.then_branch),
                      else_branch: node.else_branch ? rewrite_expression(node.else_branch) : nil,
                      origin: node.origin
                    )
                  when A::Let
                    A::Let.new(
                      name: node.name,
                      value: rewrite_expression(node.value),
                      body: rewrite_expression(node.body),
                      mutable: node.mutable,
                      constant: node.constant,
                      type: node.type,
                      origin: node.origin
                    )
                  when A::DoExpr
                    A::DoExpr.new(
                      body: node.body.map { |element| rewrite_expression(element) },
                      origin: node.origin
                    )
                  when A::BlockExpr
                    A::BlockExpr.new(
                      statements: node.statements.map { |statement| rewrite_statement(statement) },
                      result_expr: rewrite_expression(node.result_expr),
                      origin: node.origin
                    )
                  when A::UnsafeBlock
                    A::UnsafeBlock.new(
                      body: rewrite_expression(node.body),
                      origin: node.origin
                    )
                  when A::MatchExpr
                    A::MatchExpr.new(
                      scrutinee: rewrite_expression(node.scrutinee),
                      arms: node.arms.map { |arm| rewrite_match_arm(arm) },
                      origin: node.origin
                    )
                  when A::ArrayLiteral
                    A::ArrayLiteral.new(
                      elements: node.elements.map { |element| rewrite_expression(element) },
                      origin: node.origin
                    )
                  when A::TupleLit
                    A::TupleLit.new(
                      elements: node.elements.map { |element| rewrite_expression(element) },
                      origin: node.origin
                    )
                  when A::StringInterpolation
                    A::StringInterpolation.new(
                      parts: node.parts.map do |part|
                        part[:type] == :expr ? { type: :expr, value: rewrite_expression(part[:value]) } : part
                      end,
                      origin: node.origin
                    )
                  when A::RecordLit
                    A::RecordLit.new(
                      type_name: node.type_name,
                      fields: node.fields.transform_values { |value| rewrite_expression(value) },
                      spreads: node.spreads.map { |spread| { expr: rewrite_expression(spread[:expr]), position: spread[:position] } },
                      origin: node.origin
                    )
                  when A::SpreadExpr
                    A::SpreadExpr.new(
                      operand: rewrite_expression(node.operand),
                      origin: node.origin
                    )
                  when A::ForLoop
                    A::ForLoop.new(
                      var_name: node.var_name,
                      iterable: rewrite_expression(node.iterable),
                      body: rewrite_expression(node.body),
                      origin: node.origin
                    )
                  when A::WhileLoop
                    A::WhileLoop.new(
                      condition: rewrite_expression(node.condition),
                      body: rewrite_expression(node.body),
                      origin: node.origin
                    )
                  when A::WithExpr
                    A::WithExpr.new(
                      resource: rewrite_expression(node.resource),
                      binder: node.binder,
                      body: rewrite_expression(node.body),
                      origin: node.origin
                    )
                  when A::RangeExpr
                    A::RangeExpr.new(
                      start: rewrite_expression(node.start),
                      end_expr: rewrite_expression(node.end_expr),
                      inclusive: node.inclusive,
                      origin: node.origin
                    )
                  when A::ListComprehension
                    A::ListComprehension.new(
                      output_expr: rewrite_expression(node.output_expr),
                      generators: node.generators.map { |generator| rewrite_generator(generator) },
                      filters: node.filters.map { |filter| rewrite_expression(filter) },
                      origin: node.origin
                    )
                  when A::Lambda
                    A::Lambda.new(
                      params: node.params,
                      body: rewrite_expression(node.body),
                      return_type: node.return_type,
                      origin: node.origin
                    )
                  when A::TryExpr
                    A::TryExpr.new(
                      operand: rewrite_expression(node.operand),
                      origin: node.origin
                    )
                  when A::AwaitExpr
                    A::AwaitExpr.new(
                      operand: rewrite_expression(node.operand),
                      origin: node.origin
                    )
                  when A::TupleAccess
                    A::TupleAccess.new(
                      tuple: rewrite_expression(node.tuple),
                      index: node.index,
                      origin: node.origin
                    )
                  else
                    node
                  end
                end

                private

                def rewrite_argument(argument)
                  if argument.is_a?(A::NamedArg)
                    A::NamedArg.new(
                      label: argument.label,
                      value: rewrite_expression(argument.value),
                      origin: argument.origin
                    )
                  else
                    rewrite_expression(argument)
                  end
                end

                def rewrite_generator(generator)
                  A::Generator.new(
                    var_name: generator.var_name,
                    iterable: rewrite_expression(generator.iterable),
                    origin: generator.origin
                  )
                end

                def rewrite_match_arm(arm)
                  {
                    pattern: arm[:pattern],
                    guard: arm[:guard] ? rewrite_expression(arm[:guard]) : nil,
                    body: rewrite_expression(arm[:body])
                  }
                end

                def rewrite_statement(statement)
                  case statement
                  when A::VariableDecl
                    A::VariableDecl.new(
                      name: statement.name,
                      value: rewrite_expression(statement.value),
                      mutable: statement.mutable,
                      constant: statement.constant,
                      type: statement.type,
                      origin: statement.origin
                    )
                  when A::DestructuringDecl
                    A::DestructuringDecl.new(
                      pattern: statement.pattern,
                      value: rewrite_expression(statement.value),
                      mutable: statement.mutable,
                      else_body: statement.else_body ? rewrite_expression(statement.else_body) : nil,
                      origin: statement.origin
                    )
                  when A::Assignment
                    A::Assignment.new(
                      target: statement.target,
                      value: rewrite_expression(statement.value),
                      origin: statement.origin
                    )
                  when A::ExprStmt
                    A::ExprStmt.new(
                      expr: rewrite_expression(statement.expr),
                      origin: statement.origin
                    )
                  when A::Return
                    A::Return.new(
                      expr: statement.expr ? rewrite_expression(statement.expr) : nil,
                      origin: statement.origin
                    )
                  when A::IfStmt
                    A::IfStmt.new(
                      condition: rewrite_expression(statement.condition),
                      then_branch: rewrite_branch_block(statement.then_branch),
                      else_branch: statement.else_branch ? rewrite_branch_block(statement.else_branch) : nil,
                      origin: statement.origin
                    )
                  when A::WhileStmt
                    A::WhileStmt.new(
                      condition: rewrite_expression(statement.condition),
                      body: rewrite_branch_block(statement.body),
                      origin: statement.origin
                    )
                  else
                    statement
                  end
                end

                def rewrite_branch_block(branch)
                  case branch
                  when A::Block
                    A::Block.new(stmts: branch.stmts.map { |statement| rewrite_statement(statement) }, origin: branch.origin)
                  else
                    branch
                  end
                end

                def underscore_hole?(expression)
                  expression.is_a?(A::VarRef) && expression.name == '_'
                end

                def argument_expression(argument)
                  argument.is_a?(A::NamedArg) ? argument.value : argument
                end

                def next_parameter_name
                  index = @serial[0]
                  @serial[0] = index + 1
                  "partial_application_parameter_#{index}"
                end

                def replace_holes_in_arguments(arguments, parameter_names, fallback_origin)
                  index = 0
                  arguments.map do |argument|
                    if argument.is_a?(A::NamedArg)
                      if underscore_hole?(argument.value)
                        name = parameter_names[index]
                        index += 1
                        A::NamedArg.new(
                          label: argument.label,
                          value: @factory.var_ref(name: name, origin: fallback_origin),
                          origin: argument.origin
                        )
                      else
                        argument
                      end
                    elsif underscore_hole?(argument)
                      name = parameter_names[index]
                      index += 1
                      @factory.var_ref(name: name, origin: fallback_origin)
                    else
                      argument
                    end
                  end
                end

                def hole_count_in_arguments(arguments)
                  arguments.count { |argument| underscore_hole?(argument_expression(argument)) }
                end

                def wrap_call_if_partial(call_node)
                  count = hole_count_in_arguments(call_node.args)
                  return call_node if count.zero?

                  names = Array.new(count) { next_parameter_name }
                  new_args = replace_holes_in_arguments(call_node.args, names, call_node.origin)
                  inner = @factory.call(callee: call_node.callee, args: new_args, origin: call_node.origin)
                  lambda_params = names.map { |parameter_name| @factory.lambda_param(name: parameter_name, origin: call_node.origin) }
                  @factory.lambda(params: lambda_params, body: inner, origin: call_node.origin)
                end

                def wrap_safe_call_if_partial(safe_call_node)
                  count = hole_count_in_arguments(safe_call_node.args)
                  return safe_call_node if count.zero?

                  names = Array.new(count) { next_parameter_name }
                  new_args = replace_holes_in_arguments(safe_call_node.args, names, safe_call_node.origin)
                  inner = A::SafeCall.new(
                    object: safe_call_node.object,
                    method_name: safe_call_node.method_name,
                    args: new_args,
                    origin: safe_call_node.origin
                  )
                  lambda_params = names.map { |parameter_name| @factory.lambda_param(name: parameter_name, origin: safe_call_node.origin) }
                  @factory.lambda(params: lambda_params, body: inner, origin: safe_call_node.origin)
                end
              end
              # rubocop:enable Metrics/ClassLength
            end
          end
        end
      end
    end
  end
end
