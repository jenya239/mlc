# frozen_string_literal: true

module MLC
  module Source
    module Transforms
      # E3: desugar record-shaped parameter `{ x, y }: T` into one synthetic parameter
      # plus a leading record destructuring on the function body.
      module ParameterDestructuringExpand
        class << self
          def expand(func_decl)
            return func_decl if func_decl.params.none?(&:destructure_pattern)

            if func_decl.external
              raise MLC::CompileError.new(
                "destructuring parameters are not supported on extern functions",
                origin: func_decl.origin
              )
            end

            prelude = []
            new_params = func_decl.params.map do |parameter|
              if parameter.destructure_pattern
                prelude << build_destructuring_statement(parameter)
                duplicate_plain_parameter(parameter, mutable_override: false)
              else
                duplicate_plain_parameter(parameter)
              end
            end

            new_body = wrap_function_body(func_decl.body, prelude)

            MLC::Source::AST::FuncDecl.new(
              name: func_decl.name,
              params: new_params,
              ret_type: func_decl.ret_type,
              body: new_body,
              type_params: func_decl.type_params,
              where_clause: func_decl.where_clause,
              exported: func_decl.exported,
              external: func_decl.external,
              is_async: func_decl.is_async,
              origin: func_decl.origin
            )
          end

          private

          def build_destructuring_statement(parameter)
            MLC::Source::AST::DestructuringDecl.new(
              pattern: parameter.destructure_pattern,
              value: MLC::Source::AST::VarRef.new(name: parameter.name, origin: parameter.origin),
              mutable: parameter.mutable,
              else_body: nil,
              origin: parameter.origin
            )
          end

          def duplicate_plain_parameter(parameter, mutable_override: nil)
            chosen_mutable = mutable_override.nil? ? parameter.mutable : mutable_override
            MLC::Source::AST::Param.new(
              name: parameter.name,
              type: parameter.type,
              mutable: chosen_mutable,
              default: parameter.default,
              destructure_pattern: nil,
              origin: parameter.origin
            )
          end

          def wrap_function_body(body, prelude_statements)
            raise MLC::CompileError.new("function body required", origin: nil) if body.nil?

            case body
            when MLC::Source::AST::BlockExpr
              MLC::Source::AST::BlockExpr.new(
                statements: prelude_statements + body.statements,
                result_expr: body.result_expr,
                origin: body.origin
              )
            else
              MLC::Source::AST::BlockExpr.new(
                statements: prelude_statements,
                result_expr: body,
                origin: body.origin
              )
            end
          end
        end
      end
    end
  end
end
