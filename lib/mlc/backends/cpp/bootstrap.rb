# frozen_string_literal: true

require_relative "container"
require_relative "context"
require_relative "services/rule_engine"

# All rules autoloaded by Zeitwerk on-demand via string-based resolution

module MLC
  module Backends
    module Cpp
      # Bootstrap module for initializing the new backend architecture
      module Bootstrap
        # Initialize complete backend with all rules registered
        def self.create_backend(type_registry:, function_registry:, runtime_policy: nil, stdlib_scanner: nil, event_bus: nil)
          # Create container
          container = Container.new(
            type_registry: type_registry,
            function_registry: function_registry,
            runtime_policy: runtime_policy,
            stdlib_scanner: stdlib_scanner,
            event_bus: event_bus
          )

          # Create context
          context = Context.new(container)

          # Create and configure rule engine
          rule_engine = Services::RuleEngine.new(context)

          # Register all expression rules (order matters - more specific first)
          register_expression_rules(rule_engine, context)

          # Register all statement rules (order matters - more specific first)
          register_statement_rules(rule_engine, context)

          # Attach rule engine to container
          container.rule_engine = rule_engine

          # Return orchestrator object
          { container: container, context: context, rule_engine: rule_engine }
        end

        # Register all expression rules in priority order
        def self.register_expression_rules(rule_engine, context)
          # Literals and references (highest priority)
          rule_engine.register_expression_rule(Object.const_get("MLC::Backends::Cpp::Rules::Expressions::VarRefRule").new(context))
          rule_engine.register_expression_rule(Object.const_get("MLC::Backends::Cpp::Rules::Expressions::LiteralRule").new(context))
          rule_engine.register_expression_rule(Object.const_get("MLC::Backends::Cpp::Rules::Expressions::RegexRule").new(context))

          # Simple operations
          rule_engine.register_expression_rule(Object.const_get("MLC::Backends::Cpp::Rules::Expressions::UnaryRule").new(context))
          rule_engine.register_expression_rule(Object.const_get("MLC::Backends::Cpp::Rules::Expressions::BinaryRule").new(context))
          rule_engine.register_expression_rule(Object.const_get("MLC::Backends::Cpp::Rules::Expressions::IndexRule").new(context))
          rule_engine.register_expression_rule(Object.const_get("MLC::Backends::Cpp::Rules::Expressions::MemberRule").new(context))

          # Construction
          rule_engine.register_expression_rule(Object.const_get("MLC::Backends::Cpp::Rules::Expressions::ArrayLiteralRule").new(context))
          rule_engine.register_expression_rule(Object.const_get("MLC::Backends::Cpp::Rules::Expressions::RecordRule").new(context))
          rule_engine.register_expression_rule(Object.const_get("MLC::Backends::Cpp::Rules::Expressions::LambdaRule").new(context))

          # Control flow
          rule_engine.register_expression_rule(Object.const_get("MLC::Backends::Cpp::Rules::Expressions::IfRule").new(context))
          rule_engine.register_expression_rule(Object.const_get("MLC::Backends::Cpp::Rules::Expressions::BlockRule").new(context))
          rule_engine.register_expression_rule(Object.const_get("MLC::Backends::Cpp::Rules::Expressions::MatchRule").new(context))

          # Complex operations
          rule_engine.register_expression_rule(Object.const_get("MLC::Backends::Cpp::Rules::Expressions::CallRule").new(context))
          rule_engine.register_expression_rule(Object.const_get("MLC::Backends::Cpp::Rules::Expressions::ListCompRule").new(context))
        end

        # Register all statement rules in priority order
        def self.register_statement_rules(rule_engine, context)
          # Control flow statements
          rule_engine.register_statement_rule(Object.const_get("MLC::Backends::Cpp::Rules::Statements::BreakRule").new(context))
          rule_engine.register_statement_rule(Object.const_get("MLC::Backends::Cpp::Rules::Statements::ContinueRule").new(context))
          rule_engine.register_statement_rule(Object.const_get("MLC::Backends::Cpp::Rules::Statements::ReturnRule").new(context))

          # Simple statements
          rule_engine.register_statement_rule(Object.const_get("MLC::Backends::Cpp::Rules::Statements::ExprStatementRule").new(context))
          rule_engine.register_statement_rule(Object.const_get("MLC::Backends::Cpp::Rules::Statements::AssignmentRule").new(context))
          rule_engine.register_statement_rule(Object.const_get("MLC::Backends::Cpp::Rules::Statements::VariableDeclRule").new(context))

          # Compound statements
          rule_engine.register_statement_rule(Object.const_get("MLC::Backends::Cpp::Rules::Statements::IfRule").new(context))
          rule_engine.register_statement_rule(Object.const_get("MLC::Backends::Cpp::Rules::Statements::WhileRule").new(context))
          rule_engine.register_statement_rule(Object.const_get("MLC::Backends::Cpp::Rules::Statements::ForRule").new(context))
          rule_engine.register_statement_rule(Object.const_get("MLC::Backends::Cpp::Rules::Statements::MatchRule").new(context))
          rule_engine.register_statement_rule(Object.const_get("MLC::Backends::Cpp::Rules::Statements::BlockRule").new(context))
        end
      end
    end
  end
end
