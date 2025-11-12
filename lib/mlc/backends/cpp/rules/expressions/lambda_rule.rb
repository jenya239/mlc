# frozen_string_literal: true

require_relative "../expression_rule"

module MLC
  module Backends
    module Cpp
      module Rules
        module Expressions
          # Rule for lowering SemanticIR::LambdaExpr to C++ lambda expressions
          # Handles: [captures](params) { return body; }
          class LambdaRule < ExpressionRule
            def applies?(node)
              context.checker.lambda_expr?(node)
            end

            def apply(node)
              # Build capture clause (without brackets)
              capture = if node.captures.empty?
                          ""
                        else
                          # Build capture list with proper modes (&ref or value)
                          captures = node.captures.map do |cap|
                            sanitized = context.sanitize_identifier(cap[:name])
                            case cap[:mode]
                            when :ref
                              "&#{sanitized}"
                            when :value
                              sanitized
                            else
                              sanitized
                            end
                          end
                          captures.join(', ')
                        end

              # Build parameter list with types
              params_str = node.params.map do |param|
                param_type = context.map_type(param.type)
                "#{param_type} #{context.sanitize_identifier(param.name)}"
              end.join(", ")

              # Recursively lower body
              body_expr = lower_expression(node.body)

              # Build body string with return statement
              body_str = "return #{body_expr.to_source};"

              # Create C++ lambda expression
              context.factory.lambda(
                capture: capture,
                parameters: params_str,
                specifiers: "",
                body: body_str,
                capture_suffix: "",
                params_suffix: " "
              )
            end
          end
        end
      end
    end
  end
end
