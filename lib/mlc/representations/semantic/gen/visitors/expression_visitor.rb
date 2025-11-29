# frozen_string_literal: true

module MLC
  module Representations
    module Semantic
      module Gen
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
              return nil unless node

              return build_block_from_statements(node, extra_context) if node.is_a?(MLC::Source::AST::Block)
              return desugar_string_interpolation(node, extra_context) if svc.string_interpolation?(node)

              return apply_rules(node, extra_context) if svc.literal_kind(node)
              return apply_rules(node, extra_context) if svc.var_ref?(node)
              return apply_rules(node, extra_context) if svc.member_access?(node)
              return apply_rules(node, extra_context) if svc.safe_member_access?(node)
              return apply_rules(node, extra_context) if svc.safe_call?(node)
              return apply_rules(node, extra_context) if svc.lambda?(node)
              return apply_rules(node, extra_context) if svc.list_comprehension?(node)
              return apply_rules(node, extra_context) if svc.index_access?(node)
              if svc.slice_access?(node)
                object_ir = visit(node.object)
                start_ir = node.start_index ? visit(node.start_index) : nil
                end_ir = node.end_index ? visit(node.end_index) : nil
                return apply_rules(node, extra_context.merge(object_ir: object_ir, start_ir: start_ir, end_ir: end_ir))
              end
              return apply_rules(node, extra_context) if svc.let?(node)
              return apply_rules(node, extra_context) if svc.match_expr?(node)
              return apply_rules(node, extra_context) if svc.block_expr?(node)
              return apply_rules(node, extra_context) if svc.unsafe_block?(node)
              return apply_rules(node, extra_context) if svc.do_expr?(node)

              if svc.range_expr?(node)
                start_ir = visit(node.start)
                end_ir = visit(node.end_expr)
                # Range type is Range<element_type>
                element_type = start_ir.type
                range_type = @services.ir_builder.generic_type(
                  base_type: @services.ir_builder.prim_type(name: "Range"),
                  type_args: [element_type]
                )
                return @services.ir_builder.range_expr(
                  start_expr: start_ir,
                  end_expr: end_ir,
                  inclusive: node.inclusive,
                  type: range_type,
                  origin: node
                )
              end

              if svc.for_loop?(node)
                iterable_ir = visit(node.iterable)
                return apply_rules(node, extra_context.merge(iterable_ir: iterable_ir))
              end

              if svc.while_loop_expr?(node)
                condition_ir = visit(node.condition)
                return apply_rules(node, extra_context.merge(condition_ir: condition_ir))
              end

              if svc.if_expr?(node)
                condition_ir = visit(node.condition)
                then_ir = visit(node.then_branch)
                else_ir = node.else_branch ? visit(node.else_branch) : nil
                return apply_rules(node, extra_context.merge(condition_ir: condition_ir, then_ir: then_ir, else_ir: else_ir))
              end

              if svc.record_literal?(node)
                fields_ir = build_record_fields(node)
                return apply_rules(node, extra_context.merge(fields_ir: fields_ir))
              end

              if svc.array_literal?(node)
                elements_ir = node.elements.map { |element| visit(element) }
                return apply_rules(node, extra_context.merge(elements_ir: elements_ir))
              end

              if svc.tuple_literal?(node)
                elements_ir = node.elements.map { |element| visit(element) }
                return apply_rules(node, extra_context.merge(elements_ir: elements_ir))
              end

              return apply_rules(node, extra_context) if svc.symbol_literal?(node)
              return desugar_pipe(node, extra_context) if pipe_op?(node)

              if svc.call?(node)
                if static_method_call?(node)
                  args_ir = node.args.map { |arg| visit(arg) }
                  return apply_rules(node, extra_context.merge(args_ir: args_ir, skip_callee_visit: true))
                end
                callee_ir = visit(node.callee)
                args_ir = node.args.map { |arg| visit(arg) }
                return apply_rules(node, extra_context.merge(callee_ir: callee_ir, args_ir: args_ir))
              end

              if svc.unary_op?(node)
                operand_ir = visit(node.operand)
                return apply_rules(node, extra_context.merge(operand_ir: operand_ir))
              end

              if svc.await_expr?(node)
                operand_ir = visit(node.operand)
                return apply_rules(node, extra_context.merge(operand_ir: operand_ir))
              end

              if svc.binary_op?(node)
                left_ir = visit(node.left)
                right_ir = visit(node.right)
                return apply_rules(node, extra_context.merge(left_ir: left_ir, right_ir: right_ir))
              end

              raise NotImplementedError, "ExpressionVisitor cannot handle #{node.class}"
            end

            private

            def pipe_op?(node)
              node.respond_to?(:op) && node.op == '|>'
            end

            # Check if this is a static method call: Type.method(args)
            # Pattern: Call where callee is MemberAccess where object is VarRef with capitalized name
            # IMPORTANT: If the name is a known variable in scope, it's NOT a static call
            def static_method_call?(node)
              svc = @services.ast_type_checker
              return false unless svc.call?(node)

              callee = node.callee
              return false unless svc.member_access?(callee)
              return false unless svc.var_ref?(callee.object)

              type_name = callee.object.name
              method_name = callee.member

              # If it's a variable in scope, it's an instance method call, not static
              return false if @services.var_type_registry.has?(type_name)

              return false unless type_name[0] =~ /[A-Z]/

              @services.trait_registry.resolve_static_method(type_name, method_name) ||
                @services.function_registry.fetch("#{type_name}_#{method_name}")
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

            # Desugar string interpolation into binary + operations
            # "Hello, {name}!" => "Hello, " + name + "!"
            def desugar_string_interpolation(node, extra_context)
              factory = @services.ast_factory
              ast_parts = node.parts.map do |part|
                if part[:type] == :text
                  factory.string_literal(value: part[:value], origin: node.origin)
                else
                  part[:value]
                end
              end

              return visit(ast_parts.first, extra_context) if ast_parts.length == 1

              result = ast_parts.first
              ast_parts[1..].each do |part|
                result = factory.binary(
                  op: '+',
                  left: result,
                  right: part,
                  origin: node.origin
                )
              end

              visit(result, extra_context)
            end

            def build_record_fields(node)
              type_name = node.type_name.to_s
              explicit_fields = node.fields.each_with_object({}) do |(name, expr), acc|
                field_name = name.to_s
                expected_type = @services.type_registry.resolve_member(type_name, field_name)
                acc[field_name] = visit(expr, expected_type: expected_type)
              end

              return explicit_fields if node.spreads.empty?

              expanded_fields = {}
              target_type_info = @services.type_registry.lookup(type_name)
              return explicit_fields unless target_type_info&.record?

              expected_field_names = Array(target_type_info.fields).map { |f| f[:name].to_s }
              explicit_field_names = explicit_fields.keys

              node.spreads.each do |spread_info|
                spread_expr = spread_info[:expr]
                spread_ir = visit(spread_expr)
                spread_type = spread_ir.type
                spread_type_name = extract_spread_type_name(spread_type)
                spread_type_info = @services.type_registry.lookup(spread_type_name) if spread_type_name
                next unless spread_type_info&.record?

                spread_field_names = Array(spread_type_info.fields).map { |f| f[:name].to_s }

                expected_field_names.each do |field_name|
                  next if explicit_field_names.include?(field_name)
                  next if expanded_fields.key?(field_name)
                  next unless spread_field_names.include?(field_name)

                  field_info = target_type_info.fields.find { |f| f[:name].to_s == field_name }
                  field_type = field_info ? field_info[:type] : nil

                  expanded_fields[field_name] = @services.ir_builder.member(
                    object: spread_ir,
                    member: field_name,
                    type: field_type,
                    origin: spread_expr.respond_to?(:origin) ? spread_expr.origin : nil
                  )
                end
              end

              expanded_fields.merge(explicit_fields)
            end

            def extract_spread_type_name(type)
              return nil unless type

              case type
              when MLC::SemanticIR::RecordType
                type.name&.to_s
              when Hash
                type[:name]&.to_s
              when String
                type.split('<').first
              else
                type.respond_to?(:name) ? type.name&.to_s : nil
              end
            end

            def build_block_from_statements(node, extra_context)
              unless @statement_visitor
                raise NotImplementedError, "ExpressionVisitor cannot handle #{node.class} without statement visitor"
              end

              statements = Array(node.stmts).dup
              result_expr = nil
              if statements.last.is_a?(MLC::Source::AST::ExprStmt)
                result_expr = statements.pop.expr
              end

              statements_ir = @statement_visitor.visit_statements(statements)
              result_ir = result_expr ? visit(result_expr, extra_context) : @services.ir_builder.unit_literal(origin: node)

              @services.ir_builder.block_expr(
                statements: statements_ir,
                result: result_ir,
                type: result_ir.type,
                origin: node
              )
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
  end
end
