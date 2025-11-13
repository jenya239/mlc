# frozen_string_literal: true

module MLC
  module Rules
    # Simple rule engine for semantic generation
    # Pattern: Chain of Responsibility
    class RuleEngine
      def initialize
        @rules = Hash.new { |hash, key| hash[key] = [] }
      end

      def register(stage, rule)
        @rules[stage] ||= []
        @rules[stage] << rule
      end

      def apply(stage, node, context: {})
        rules = @rules[stage] || []
        rules.each do |rule|
          if rule.respond_to?(:applies?)
            # New interface (BaseRule pattern)
            if rule.applies?(node, context)
              return rule.apply(node, context)
            end
          elsif rule.respond_to?(:matches?)
            # Direct interface
            if rule.matches?(node, context)
              return rule.produce(node, context)
            end
          end
        end

        # No rule matched
        nil
      end

      def rules_for(stage)
        @rules[stage] || []
      end

      # Expose internal registry for introspection (used by Engine#register_rule)
      def registry
        @rules
      end
    end
  end
end
