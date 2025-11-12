# frozen_string_literal: true

require_relative "../expression_rule"

module MLC
  module Backends
    module Cpp
      module Rules
        module Expressions
          # Rule for lowering SemanticIR::VarExpr to C++ identifiers
          # Handles variable references and boolean literals (true/false)
          class VarRefRule < ExpressionRule
            def applies?(node)
              context.checker.var_expr?(node)
            end

            def apply(node)
              case node.name
              when "true"
                context.factory.boolean_literal(value: true)
              when "false"
                context.factory.boolean_literal(value: false)
              else
                context.factory.identifier(name: context.sanitize_identifier(node.name))
              end
            end
          end
        end
      end
    end
  end
end
