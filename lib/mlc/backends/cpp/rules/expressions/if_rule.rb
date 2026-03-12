# frozen_string_literal: true

module MLC
  module Backends
    module Cpp
      module Rules
        module Expressions
          # Rule for lowering SemanticIR::IfExpr to C++ ternary operator
          # Handles: condition ? true_expr : false_expr
          # Note: Unit-type ifs should be lowered as statements, not expressions
          class IfRule < ExpressionRule
            def applies?(node)
              context.checker.if_expr?(node)
            end

            def apply(node)
              # Unit type if-expressions should be lowered as statements, not expressions
              if context.should_lower_as_statement?(node)
                raise "Internal error: Unit type if-expression should not be lowered as expression. " \
                      "Use lower_if_expr_as_statement instead."
              end

              # Recursively lower all parts
              condition = lower_expression(node.condition)
              then_branch = lower_expression(node.then_branch)

              # For value-producing expressions, use ternary operator
              # If no else branch, default to 0 (shouldn't happen in well-typed code)
              else_branch = if node.else_branch
                              lower_expression(node.else_branch)
                            else
                              context.factory.number_literal(value: "0")
                            end

              # When result type is a sum/generic type, wrap both branches to avoid C++ ternary
              # type mismatch (variant constructors are different struct types)
              if variant_result_type?(node.type) && node.else_branch
                sum_type_name = context.map_type(node.type)
                then_branch = wrap_in_type(then_branch, sum_type_name)
                else_branch = wrap_in_type(else_branch, sum_type_name)
              end

              # Resolve empty array {} in ternary branches using the other branch's type.
              if node.then_branch&.type && node.else_branch&.type
                then_type = context.map_type(node.then_branch.type)
                else_type = context.map_type(node.else_branch.type)
                if then_branch.to_source == "{}"
                  then_branch = context.factory.raw_expression(code: "#{else_type}{}")
                elsif else_branch.to_source == "{}"
                  else_branch = context.factory.raw_expression(code: "#{then_type}{}")
                end
              end

              context.factory.ternary(
                condition: condition,
                true_expression: then_branch,
                false_expression: else_branch,
                question_prefix: " ",
                colon_prefix: " ",
                question_suffix: " ",
                colon_suffix: " "
              )
            end

            private

            def variant_result_type?(type)
              return true if type.is_a?(MLC::SemanticIR::SumType)
              return false unless type.is_a?(MLC::SemanticIR::GenericType)

              base_name = type.base_type.respond_to?(:name) ? type.base_type.name : nil
              return false unless base_name

              type_info = context.type_registry&.lookup(base_name)
              type_info&.core_ir_type&.is_a?(MLC::SemanticIR::SumType)
            end

            def wrap_in_type(expr, type_name)
              context.factory.function_call(
                callee: context.factory.identifier(name: type_name),
                arguments: [expr]
              )
            end
          end
        end
      end
    end
  end
end
