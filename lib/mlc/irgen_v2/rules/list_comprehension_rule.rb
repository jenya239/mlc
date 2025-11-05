# frozen_string_literal: true

require_relative 'base_rule'

module MLC
  module IRGenV2
    module Rules
      # ListComprehensionRule - lowers list comprehension expressions via service
      class ListComprehensionRule < BaseRule
        def matches?(node, context)
          services(context).ast_type_checker.list_comprehension?(node)
        end

        def produce(node, context)
          svc = services(context)
          svc.list_comprehension_service.build(
            node,
            expression_visitor: context.fetch(:expression_visitor)
          )
        end
      end
    end
  end
end
