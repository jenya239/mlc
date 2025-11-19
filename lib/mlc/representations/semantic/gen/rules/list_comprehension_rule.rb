# frozen_string_literal: true


module MLC
  module Representations
    module Semantic
      module Gen
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
      end
    end