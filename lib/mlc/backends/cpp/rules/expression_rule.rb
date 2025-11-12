# frozen_string_literal: true

require_relative "base_rule"

module MLC
  module Backends
    module Cpp
      module Rules
        # Base class for expression rules
        # Generates C++ expressions from MidIR::Expression nodes
        class ExpressionRule < BaseRule
          # Returns C++ expression string
          def apply(expr_node)
            raise NotImplementedError, "#{self.class}#apply must be implemented"
          end
        end
      end
    end
  end
end
