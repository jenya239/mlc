# frozen_string_literal: true

require_relative 'base_rule'

module MLC
  module SemanticGen
    module Rules
      # VarRefRule - Handles variable and function references
      class VarRefRule < BaseRule
        def matches?(node, context)
          services(context).ast_type_checker.var_ref?(node)
        end

        def produce(node, context)
          svc = services(context)
          name = node.name
          type = svc.identifier_type_resolver.type_for(name)
          svc.ir_builder.var(name: name, type: type, origin: node)
        end
      end
    end
  end
end
