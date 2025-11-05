# frozen_string_literal: true

require_relative 'base_rule'

module MLC
  module IRGenV2
    module Rules
      # LambdaRule - lowers lambda expressions through LambdaService
      class LambdaRule < BaseRule
        def matches?(node, context)
          services(context).ast_type_checker.lambda?(node)
        end

        def produce(node, context)
          svc = services(context)
          svc.lambda_service.build(
            node,
            expression_visitor: context.fetch(:expression_visitor)
          )
        end
      end
    end
  end
end
