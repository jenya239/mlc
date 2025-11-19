# frozen_string_literal: true


module MLC
  module Representations
    module Semantic
      module Gen
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
      end
    end