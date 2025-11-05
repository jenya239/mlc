# frozen_string_literal: true

require_relative '../../rules/base_rule'

module MLC
  module IRGenV2
    module Rules
      module Statements
        class BreakRule < BaseRule
          def matches?(node, context)
            services(context).ast_type_checker.break_statement?(node)
          end

          def produce(node, context)
            services(context).ir_builder.break_node(origin: node)
          end
        end
      end
    end
  end
end
