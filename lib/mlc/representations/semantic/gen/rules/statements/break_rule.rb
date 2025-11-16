# frozen_string_literal: true

require_relative '../base_rule'

module MLC
  module Representations
    module Semantic
      module Gen
        module Rules
          module Statements
            class BreakRule < BaseRule
              def matches?(node, context)
                services(context).ast_type_checker.break_statement?(node)
          end

              def produce(node, context)
                svc = services(context)
                loops = svc.loop_service
                unless loops.inside_loop?
                  svc.type_checker.type_error("'break' used outside of loop", node: node)
            end

                svc.ir_builder.break_node(origin: node)
          end
            end
          end
            end
          end
        end
      end
    end