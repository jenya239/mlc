# frozen_string_literal: true

module MLC
  module Services
    # RuleEngineBuilder - Build and configure RuleEngine with default IRGen rules
    # Phase 19-A: Extracted from IRGen
    #
    # Responsibilities:
    # - Build default rule engine with all IRGen transformation rules
    # - Ensure required rules are registered
    # - Validate rule engine configuration
    #
    # Dependencies: None (pure builder)
    class RuleEngineBuilder
      # Build default rule engine with all IRGen transformation rules
      # Returns fully configured RuleEngine instance
      #
      # @return [Rules::RuleEngine]
      def self.build_default
        engine = MLC::Rules::RuleEngine.new

        # Type declaration rules
        engine.register(:core_ir_type_decl, MLC::Rules::IRGen::SumConstructorRule.new)

        # Top-level transformation rules
        engine.register(:core_ir_match_expr, MLC::Rules::IRGen::MatchRule.new)
        engine.register(:core_ir_function, MLC::Rules::IRGen::FunctionEffectRule.new)
        engine.register(:core_ir_stdlib_import, MLC::Rules::IRGen::StdlibImportRule.new)

        # Expression transformation rules
        engine.register(:core_ir_expression, MLC::Rules::IRGen::Expression::LiteralRule.new)
        engine.register(:core_ir_expression, MLC::Rules::IRGen::Expression::VarRefRule.new)
        engine.register(:core_ir_expression, MLC::Rules::IRGen::Expression::MemberRule.new)
        engine.register(:core_ir_expression, MLC::Rules::IRGen::Expression::CallRule.new)
        engine.register(:core_ir_expression, MLC::Rules::IRGen::Expression::UnaryRule.new)
        engine.register(:core_ir_expression, MLC::Rules::IRGen::Expression::PipeRule.new)
        engine.register(:core_ir_expression, MLC::Rules::IRGen::Expression::BinaryRule.new)
        engine.register(:core_ir_expression, MLC::Rules::IRGen::Expression::LetRule.new)
        engine.register(:core_ir_expression, MLC::Rules::IRGen::Expression::RecordLiteralRule.new)
        engine.register(:core_ir_expression, MLC::Rules::IRGen::Expression::IfRule.new)
        engine.register(:core_ir_expression, MLC::Rules::IRGen::Expression::ArrayLiteralRule.new)
        engine.register(:core_ir_expression, MLC::Rules::IRGen::Expression::DoRule.new)
        engine.register(:core_ir_expression, MLC::Rules::IRGen::Expression::BlockRule.new)
        engine.register(:core_ir_expression, MLC::Rules::IRGen::Expression::MatchRule.new)
        engine.register(:core_ir_expression, MLC::Rules::IRGen::Expression::LambdaRule.new)
        engine.register(:core_ir_expression, MLC::Rules::IRGen::Expression::IndexAccessRule.new)
        engine.register(:core_ir_expression, MLC::Rules::IRGen::Expression::ForLoopRule.new)
        engine.register(:core_ir_expression, MLC::Rules::IRGen::Expression::WhileLoopRule.new)
        engine.register(:core_ir_expression, MLC::Rules::IRGen::Expression::ListComprehensionRule.new)

        # Statement transformation rules
        engine.register(:core_ir_statement, MLC::Rules::IRGen::Statement::ExprStmtRule.new)
        engine.register(:core_ir_statement, MLC::Rules::IRGen::Statement::VariableDeclRule.new)
        engine.register(:core_ir_statement, MLC::Rules::IRGen::Statement::AssignmentRule.new)
        engine.register(:core_ir_statement, MLC::Rules::IRGen::Statement::ForRule.new)
        engine.register(:core_ir_statement, MLC::Rules::IRGen::Statement::IfRule.new)
        engine.register(:core_ir_statement, MLC::Rules::IRGen::Statement::WhileRule.new)
        engine.register(:core_ir_statement, MLC::Rules::IRGen::Statement::ReturnRule.new)
        engine.register(:core_ir_statement, MLC::Rules::IRGen::Statement::BreakRule.new)
        engine.register(:core_ir_statement, MLC::Rules::IRGen::Statement::ContinueRule.new)
        engine.register(:core_ir_statement, MLC::Rules::IRGen::Statement::BlockRule.new)

        engine
      end

      # Ensure all required rules are registered in the engine
      # Registers missing rules automatically
      #
      # @param engine [Rules::RuleEngine] The rule engine to validate
      def self.ensure_required_rules!(engine)
        # Type declaration rules
        ensure_rule_registered(engine, :core_ir_type_decl, MLC::Rules::IRGen::SumConstructorRule)

        # Top-level transformation rules
        ensure_rule_registered(engine, :core_ir_match_expr, MLC::Rules::IRGen::MatchRule)
        ensure_rule_registered(engine, :core_ir_function, MLC::Rules::IRGen::FunctionEffectRule)
        ensure_rule_registered(engine, :core_ir_stdlib_import, MLC::Rules::IRGen::StdlibImportRule)

        # Expression transformation rules
        ensure_rule_registered(engine, :core_ir_expression, MLC::Rules::IRGen::Expression::LiteralRule)
        ensure_rule_registered(engine, :core_ir_expression, MLC::Rules::IRGen::Expression::VarRefRule)
        ensure_rule_registered(engine, :core_ir_expression, MLC::Rules::IRGen::Expression::MemberRule)
        ensure_rule_registered(engine, :core_ir_expression, MLC::Rules::IRGen::Expression::CallRule)
        ensure_rule_registered(engine, :core_ir_expression, MLC::Rules::IRGen::Expression::UnaryRule)
        ensure_rule_registered(engine, :core_ir_expression, MLC::Rules::IRGen::Expression::PipeRule)
        ensure_rule_registered(engine, :core_ir_expression, MLC::Rules::IRGen::Expression::BinaryRule)
        ensure_rule_registered(engine, :core_ir_expression, MLC::Rules::IRGen::Expression::LetRule)
        ensure_rule_registered(engine, :core_ir_expression, MLC::Rules::IRGen::Expression::RecordLiteralRule)
        ensure_rule_registered(engine, :core_ir_expression, MLC::Rules::IRGen::Expression::IfRule)
        ensure_rule_registered(engine, :core_ir_expression, MLC::Rules::IRGen::Expression::ArrayLiteralRule)
        ensure_rule_registered(engine, :core_ir_expression, MLC::Rules::IRGen::Expression::DoRule)
        ensure_rule_registered(engine, :core_ir_expression, MLC::Rules::IRGen::Expression::BlockRule)
        ensure_rule_registered(engine, :core_ir_expression, MLC::Rules::IRGen::Expression::MatchRule)
        ensure_rule_registered(engine, :core_ir_expression, MLC::Rules::IRGen::Expression::LambdaRule)
        ensure_rule_registered(engine, :core_ir_expression, MLC::Rules::IRGen::Expression::IndexAccessRule)
        ensure_rule_registered(engine, :core_ir_expression, MLC::Rules::IRGen::Expression::ForLoopRule)
        ensure_rule_registered(engine, :core_ir_expression, MLC::Rules::IRGen::Expression::WhileLoopRule)
        ensure_rule_registered(engine, :core_ir_expression, MLC::Rules::IRGen::Expression::ListComprehensionRule)

        # Statement transformation rules
        ensure_rule_registered(engine, :core_ir_statement, MLC::Rules::IRGen::Statement::ExprStmtRule)
        ensure_rule_registered(engine, :core_ir_statement, MLC::Rules::IRGen::Statement::VariableDeclRule)
        ensure_rule_registered(engine, :core_ir_statement, MLC::Rules::IRGen::Statement::AssignmentRule)
        ensure_rule_registered(engine, :core_ir_statement, MLC::Rules::IRGen::Statement::ForRule)
        ensure_rule_registered(engine, :core_ir_statement, MLC::Rules::IRGen::Statement::IfRule)
        ensure_rule_registered(engine, :core_ir_statement, MLC::Rules::IRGen::Statement::WhileRule)
        ensure_rule_registered(engine, :core_ir_statement, MLC::Rules::IRGen::Statement::ReturnRule)
        ensure_rule_registered(engine, :core_ir_statement, MLC::Rules::IRGen::Statement::BreakRule)
        ensure_rule_registered(engine, :core_ir_statement, MLC::Rules::IRGen::Statement::ContinueRule)
        ensure_rule_registered(engine, :core_ir_statement, MLC::Rules::IRGen::Statement::BlockRule)
      end

      # Ensure a specific rule is registered for a stage
      # Registers the rule if not already present
      #
      # @param engine [Rules::RuleEngine] The rule engine
      # @param stage [Symbol] The transformation stage
      # @param rule_class [Class] The rule class to ensure
      def self.ensure_rule_registered(engine, stage, rule_class)
        rules = engine.registry[stage.to_sym]
        return if rules.any? { |rule| rule.is_a?(rule_class) || rule == rule_class || rule.class == rule_class }

        engine.register(stage, rule_class.new)
      end

      private_class_method :ensure_rule_registered
    end
  end
end
