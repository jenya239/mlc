# frozen_string_literal: true

require_relative 'base_rule'

module MLC
  module Representations
    module Semantic
      module Gen
        module Rules
          class MatchRule < BaseRule
            def matches?(node, context)
              services(context).ast_type_checker.match_expr?(node)
        end

            def produce(node, context)
              svc = services(context)
              svc.match_service.build(
                node,
                expression_visitor: context.fetch(:expression_visitor),
                statement_visitor: context[:statement_visitor]
              )
        end
          end
        end
          end
        end
      end
    end