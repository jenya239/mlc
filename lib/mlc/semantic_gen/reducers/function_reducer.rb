# frozen_string_literal: true

module MLC
  module SemanticGen
    module Reducers
      # FunctionReducer lowers AST::FuncDecl nodes into SemanticIR::Func
      # using SemanticGen visitors and shared services.
      class FunctionReducer
        def initialize(services:, engine:, function_registration_service:, rule_engine:, effect_analyzer: nil)
          @services = services
          @engine = engine
          @function_registration_service = function_registration_service
          @rule_engine = rule_engine
          @effect_analyzer = effect_analyzer

          @function_registry = services.function_registry
          @type_checker = services.type_checker
          @type_builder = services.type_builder
          @scope_context = services.scope_context
          @var_type_registry = services.var_type_registry
        end

        def register_signature(func_decl)
          return @function_registry.fetch(func_decl.name) if @function_registry.registered?(func_decl.name)

          type_params = @type_checker.normalize_type_params(func_decl.type_params)
          signature = nil

          @type_builder.with_type_params(type_params) do
            param_types = func_decl.params.map { |param| build_type_annotation(param.type) }
            ret_type = build_type_annotation(func_decl.ret_type)
            signature = @function_registration_service.register_function_signature(
              func_decl,
              param_types,
              ret_type,
              type_params
            )
          end

          signature
        end

        def reduce(func_decl, signature: nil)
          @scope_context.with_current_node(func_decl) do
            type_params = @type_checker.normalize_type_params(func_decl.type_params)
            @scope_context.with_type_params(type_params) do
              signature ||= register_signature(func_decl)
              params = build_params(func_decl, signature.param_types)
              result_func = if func_decl.external
                              build_external_func(func_decl, params, signature, type_params)
                            else
                              build_function(func_decl, params, signature, type_params)
                            end
              result_func = apply_function_rules(result_func)
              update_registry_metadata(func_decl, result_func)
              result_func
            end
          end
        end

        private

        def build_params(func_decl, param_types)
          if param_types.length != func_decl.params.length
            @type_checker.type_error(
              "Function '#{func_decl.name}' expects #{param_types.length} parameter(s), got #{func_decl.params.length}",
              node: func_decl
            )
          end

          func_decl.params.each_with_index.map do |param, index|
            type = param_types[index]
            MLC::SemanticIR::Param.new(name: param.name, type: type, origin: param.origin)
          end
        end

        def build_external_func(func_decl, params, signature, type_params)
          MLC::SemanticIR::Func.new(
            name: func_decl.name,
            params: params,
            ret_type: signature.ret_type,
            body: nil,
            effects: [],
            type_params: type_params,
            external: true,
            exported: func_decl.exported,
            origin: func_decl.origin
          )
        end

      def build_function(func_decl, params, signature, type_params)
        saved_types = @var_type_registry.snapshot
        params.each { |param| @var_type_registry.set(param.name, param.type) }

        body_ir = nil
        @scope_context.with_function_return(signature.ret_type) do
          body_ir = @engine.run_expression(func_decl.body)
          body_ir = wrap_statement_like_expression(body_ir)
        end

        ensure_return_type!(func_decl, body_ir, signature.ret_type)

        MLC::SemanticIR::Func.new(
            name: func_decl.name,
            params: params,
            ret_type: signature.ret_type,
            body: body_ir,
            effects: [],
            type_params: type_params,
            external: func_decl.external,
            exported: func_decl.exported,
            origin: func_decl.origin
          )
        ensure
          @var_type_registry.restore(saved_types)
        end

        def ensure_return_type!(func_decl, body_ir, expected_type)
          actual_type = body_ir&.type
          return if expected_type.nil? || actual_type.nil?

          @type_checker.ensure_compatible_type(
            actual_type,
            expected_type,
            "function '#{func_decl.name}' result",
            node: func_decl
          )
        end

      def apply_function_rules(func_ir)
          return func_ir unless @rule_engine

          result = @rule_engine.apply(
            :core_ir_function,
            func_ir,
            context: {
              effect_analyzer: @effect_analyzer,
              type_registry: @services.type_registry
            }
          )

          normalize_rule_result(func_ir, result)
        end

        def normalize_rule_result(original, result)
          case result
          when nil
            original
          when MLC::SemanticIR::Func
            result
          when Array
            result.find { |node| node.is_a?(MLC::SemanticIR::Func) && node.name == original.name } || original
          else
            original
          end
        end

        def update_registry_metadata(func_decl, func_ir)
          return unless @function_registry.registered?(func_decl.name)

          @function_registry.update(
            func_decl.name,
            effects: func_ir.effects,
            external: func_decl.external,
            exported: func_decl.exported
          )
        end

        def build_type_annotation(type_ast)
          return MLC::SemanticIR::Builder.unit_type unless type_ast

          @type_builder.transform(type_ast)
        end

        def wrap_statement_like_expression(body)
          return body if body.is_a?(MLC::SemanticIR::BlockExpr)

          if unit_literal?(body)
            return block_expr_from_statements([], body.origin)
          end

          if statement_like_if?(body)
            if_stmt = build_if_statement(body)
            return block_expr_from_statements([if_stmt], body.origin)
          end

          body
        end

        def unit_literal?(expr)
          expr.is_a?(MLC::SemanticIR::UnitLiteral)
        end

        def statement_like_if?(expr)
          return false unless expr.is_a?(MLC::SemanticIR::IfExpr)

          type = expr.type
          name = @type_checker.normalized_type_name(@type_checker.type_name(type))
          %w[unit void].include?(name) || type.is_a?(MLC::SemanticIR::UnitType)
        end

        def build_if_statement(expr_ir)
          then_block = block_from_expression(expr_ir.then_branch)
          else_block = expr_ir.else_branch ? block_from_expression(expr_ir.else_branch) : nil

          @services.ir_builder.if_stmt(
            condition: expr_ir.condition,
            then_body: then_block,
            else_body: else_block,
            origin: expr_ir.origin
          )
        end

        def block_from_expression(expr_ir)
          case expr_ir
          when MLC::SemanticIR::BlockExpr
            statements = expr_ir.statements.dup
            if expr_ir.result && !expr_ir.result.is_a?(MLC::SemanticIR::UnitLiteral)
              statements << @services.ir_builder.expr_statement(expression: expr_ir.result, origin: expr_ir.result.origin)
            end
            @services.ir_builder.block(statements: statements, origin: expr_ir.origin)
          when MLC::SemanticIR::UnitLiteral
            @services.ir_builder.block(statements: [], origin: expr_ir.origin)
          else
            stmt = @services.ir_builder.expr_statement(expression: expr_ir, origin: expr_ir.origin)
            @services.ir_builder.block(statements: [stmt], origin: expr_ir.origin)
          end
        end

        def block_expr_from_statements(statements, origin)
          unit = @services.ir_builder.unit_literal(origin: origin)
          @services.ir_builder.block_expr(
            statements: statements,
            result: unit,
            type: unit.type,
            origin: origin
          )
        end
      end
    end
  end
end
