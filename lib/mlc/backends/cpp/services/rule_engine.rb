# frozen_string_literal: true

module MLC
  module Backends
    module Cpp
      module Services
        # Rule engine for dispatching expression and statement lowering
        # Implements Chain of Responsibility pattern for rules
        class RuleEngine
          def initialize(context)
            @context = context
            @expression_rules = []
            @statement_rules = []
          end

          # Register an expression rule
          def register_expression_rule(rule)
            @expression_rules << rule
          end

          # Register a statement rule
          def register_statement_rule(rule)
            @statement_rules << rule
          end

          # Lower a SemanticIR expression to CppAst node
          # Iterates through registered rules until one applies
          def lower_expression(node)
            rule = @expression_rules.find { |r| r.applies?(node) }

            raise "No rule found for expression: #{node.class}" unless rule

            rule.apply(node)
          end

          # Lower a SemanticIR statement to CppAst node
          # Iterates through registered rules until one applies
          def lower_statement(node)
            rule = @statement_rules.find { |r| r.applies?(node) }

            raise "No rule found for statement: #{node.class}" unless rule

            rule.apply(node)
          end
        end
      end
    end
  end
end
