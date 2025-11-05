# frozen_string_literal: true

require_relative 'base_rule'

module MLC
  module IRGenV2
    module Rules
      # LiteralRule - Declarative literal handling using services
      class LiteralRule < BaseRule
        def matches?(node, context)
          svc = services(context)
          svc.literal_processor.supported?(node, svc.ast_type_checker)
        end

        def produce(node, context)
          svc = services(context)
          svc.literal_processor.build(node, svc.ast_type_checker)
        end
      end
    end
  end
end
