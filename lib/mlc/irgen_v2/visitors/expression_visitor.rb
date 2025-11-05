# frozen_string_literal: true

module MLC
  module IRGenV2
    module Visitors
      # ExpressionVisitor - Traverses AST expressions and invokes declarative rules
      class ExpressionVisitor
        def initialize(engine:)
          @engine = engine
          @rule_engine = engine.rule_engine
          @services = engine.services
          @statement_visitor = nil
        end

        attr_writer :statement_visitor

        def visit(node, extra_context = {})
          svc = @services.ast_type_checker

          return apply_rules(node, extra_context) if svc.literal_kind(node)
          return apply_rules(node, extra_context) if svc.var_ref?(node)
          return apply_rules(node, extra_context) if svc.member_access?(node)
          return apply_rules(node, extra_context) if svc.index_access?(node)
          return apply_rules(node, extra_context) if svc.let?(node)
          return apply_rules(node, extra_context) if svc.match_expr?(node)
          return apply_rules(node, extra_context) if svc.block_expr?(node)
          return apply_rules(node, extra_context) if svc.do_expr?(node)

          if svc.for_loop?(node)
            iterable_ir = visit(node.iterable)
            context = extra_context.merge(iterable_ir: iterable_ir)
            return apply_rules(node, context)
          end

          if svc.while_loop_expr?(node)
            condition_ir = visit(node.condition)
            context = extra_context.merge(condition_ir: condition_ir)
            return apply_rules(node, context)
          end

          if svc.if_expr?(node)
            condition_ir = visit(node.condition)
            then_ir = visit(node.then_branch)
            else_ir = node.else_branch ? visit(node.else_branch) : nil
            context = extra_context.merge(
              condition_ir: condition_ir,
              then_ir: then_ir,
              else_ir: else_ir
            )
            return apply_rules(node, context)
          end

          if svc.record_literal?(node)
            fields_ir = build_record_fields(node)
            context = extra_context.merge(fields_ir: fields_ir)
            return apply_rules(node, context)
          end

          if svc.array_literal?(node)
            elements_ir = node.elements.map { |element| visit(element) }
            context = extra_context.merge(elements_ir: elements_ir)
            return apply_rules(node, context)
          end

          if pipe_op?(node)
            return desugar_pipe(node, extra_context)
          end

          if svc.call?(node)
            callee_ir = visit(node.callee)
            args_ir = node.args.map { |arg| visit(arg) }
            context = extra_context.merge(callee_ir: callee_ir, args_ir: args_ir)
            return apply_rules(node, context)
          end

          if svc.unary_op?(node)
            operand_ir = visit(node.operand)
            context = extra_context.merge(operand_ir: operand_ir)
            return apply_rules(node, context)
          end

          if svc.binary_op?(node)
            left_ir = visit(node.left)
            right_ir = visit(node.right)
            context = extra_context.merge(left_ir: left_ir, right_ir: right_ir)
            return apply_rules(node, context)
          end

          raise NotImplementedError, "ExpressionVisitor cannot handle #{node.class}"
        end

        private

        def pipe_op?(node)
          node.respond_to?(:op) && node.op == '|>'
        end

        def desugar_pipe(node, extra_context)
          factory = @services.ast_factory
          checker = @services.ast_type_checker

          left_expr = node.left
          right_expr = node.right

          if checker.call?(right_expr)
            new_args = [left_expr] + right_expr.args
            new_call = factory.call(callee: right_expr.callee, args: new_args, origin: node.origin)
            return visit(new_call, extra_context)
          end

          new_call = factory.call(callee: right_expr, args: [left_expr], origin: node.origin)
          visit(new_call, extra_context)
        end

        def build_record_fields(node)
          visitor = self
          node.fields.each_with_object({}) do |(name, expr), acc|
            acc[name.to_s] = visitor.visit(expr)
          end
        end

        def apply_rules(node, extra_context)
          context = base_context.merge(extra_context)
          result = @rule_engine.apply(:expression, node, context: context)
          return result unless result.equal?(node)

          raise NotImplementedError, "No rule produced IR for #{node.class}"
        end

        def base_context
          {
            services: @services,
            expression_visitor: self,
            statement_visitor: @statement_visitor,
            engine: @engine
          }
        end
      end
    end
  end
end
