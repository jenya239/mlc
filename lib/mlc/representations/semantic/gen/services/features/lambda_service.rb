# frozen_string_literal: true

module MLC
  module Representations
    module Semantic
      module Gen
        module Services
          # LambdaService - builds SemanticIR lambda expressions with scoped parameters
          class LambdaService
            DEFAULT_PARAM_TYPE = 'i32'

            def initialize(ir_builder:, type_checker:, var_type_registry:, scope_context:, type_builder:, capture_analyzer:)
              @ir_builder = ir_builder
              @type_checker = type_checker
              @var_type_registry = var_type_registry
              @scope_context = scope_context
              @type_builder = type_builder
              @capture_analyzer = capture_analyzer
            end

            def build(node, expression_visitor:)
              snapshot = @var_type_registry.snapshot

              # Analyze captures BEFORE processing lambda body
              # At this point, var_type_registry contains outer scope variables
              captures = @capture_analyzer.analyze(node)

              params_ir = build_params(node, node.params)

              body_ir = expression_visitor.visit(node.body)
              @type_checker.ensure_type!(body_ir.type, 'lambda body has unknown type', node: node.body)

              function_type = @ir_builder.function_type(
                params: params_ir.map { |param| { name: param.name, type: param.type } },
                return_type: body_ir.type,
                origin: node
              )

              @ir_builder.lambda_expr(
                params: params_ir,
                body: body_ir,
                function_type: function_type,
                captures: captures,
                origin: node
              )
            ensure
              @var_type_registry.restore(snapshot)
            end

            private

            def build_params(node, params)
              expected = Array(@scope_context.current_lambda_param_types)

              params.each_with_index.map do |param_ast, index|
                name = param_ast.respond_to?(:name) ? param_ast.name : param_ast
                type = resolve_param_type(param_ast, expected[index], node: node)
                @var_type_registry.set(name, type)
                @ir_builder.param(name: name, type: type, origin: param_ast)
              end
            end

            def resolve_param_type(param_ast, expected_type, node:)
              if param_ast.respond_to?(:type) && param_ast.type
                return @type_builder.transform(param_ast.type)
              end

              expected_type || default_param_type(node)
            end

            def default_param_type(node)
              @ir_builder.prim_type(name: DEFAULT_PARAM_TYPE, origin: node)
            end
          end
        end
      end
    end
  end
end
