# frozen_string_literal: true

module MLC
  module Representations
    module Semantic
      module Gen
        module Rules
          # BaseRule - Declarative rule pattern for SemanticGen
          # Rules operate on AST nodes using data provided through context services
          class BaseRule
            def applies?(node, context)
              matches?(node, context)
        end

            def apply(node, context)
              produce(node, context)
        end

            private

            def matches?(_node, _context)
              raise NotImplementedError, "#{self.class} must implement matches?"
        end

            def produce(_node, _context)
              raise NotImplementedError, "#{self.class} must implement produce"
        end

            def services(context)
              context.fetch(:services)
        end
          end
        end
          end
        end
      end
    end