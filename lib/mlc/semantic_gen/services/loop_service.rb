# frozen_string_literal: true

module MLC
  module SemanticGen
    module Services
      # LoopService - helpers for for/while loop handling
      class LoopService
        def initialize(ir_builder:, type_checker:, ast_factory:, ast_type_checker:, scope_context:, variable_types:)
          @ir_builder = ir_builder
          @type_checker = type_checker
          @ast_factory = ast_factory
          @ast_type_checker = ast_type_checker
          @scope_context = scope_context
          @var_type_registry = variable_types
        end

        def infer_iterable_element(iterable_ir, node: nil)
          type = iterable_ir.type
          if type.respond_to?(:element_type) && type.element_type
            ensure_type!(type.element_type, 'iterable', node)
            type.element_type
          else
            @type_checker.type_error('cannot iterate over value without element type', node: node)
          end
        end

        def normalize_loop_body(body_ast, statement_visitor)
          checker = @ast_type_checker

          statements =
            if checker.block_statement?(body_ast)
              body_ast.stmts
            elsif checker.block_expr?(body_ast)
              stmts = body_ast.statements.dup
              if body_ast.result_expr
                result = body_ast.result_expr
                stmts << @ast_factory.expr_stmt(expr: result, origin: result.origin)
              end
              stmts
            else
              [@ast_factory.expr_stmt(expr: body_ast, origin: body_ast.origin)]
            end

          statement_visitor.visit_statements(statements)
        end

        def with_loop_scope
          @scope_context.with_loop_scope do
            yield
          end
        end

        def inside_loop?
          @scope_context.inside_loop?
        end

        def save_variable(name)
          @var_type_registry.snapshot[name]
        end

        def with_loop_variable(name, type)
          saved = @var_type_registry.get(name)
          @var_type_registry.set(name, type)
          yield
        ensure
          if saved
            @var_type_registry.set(name, saved)
          else
            @var_type_registry.delete(name)
          end
        end

        private

        def ensure_type!(type, context, node)
          @type_checker.ensure_type!(type, "#{context} has no type", node: node)
        end
      end
    end
  end
end
