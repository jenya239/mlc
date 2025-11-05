# frozen_string_literal: true

require_relative '../../rules/base_rule'

module MLC
  module IRGenV2
    module Rules
      module Statements
        class ContinueRule < BaseRule
          def matches?(node, context)
            services(context).ast_type_checker.continue_statement?(node)
          end

          def produce(node, context)
            services(context).ir_builder.continue_node(origin: node)
          end
        end
      end
    end
  end
end
