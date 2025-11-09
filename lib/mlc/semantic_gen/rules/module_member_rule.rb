# frozen_string_literal: true

require_relative 'base_rule'

module MLC
  module SemanticGen
    module Rules
      # ModuleMemberRule - Rewrite module member access to variable reference
      # Example: Math.sqrt -> var("Math::sqrt", func_type)
      #
      # This is a DECLARATIVE rule:
      # - applies? describes WHEN to apply
      # - produce describes WHAT to build
      # - Services provide all state
      # - NO explicit class mentions (use ast_type_checker)
      # - NO explicit constructors (use ir_builder)
      class ModuleMemberRule < BaseRule
        def matches?(node, context)
          svc = services(context)
          return false unless svc.ast_type_checker.member_access?(node)

          object_ast = node.object
          member_name = node.member

          !resolve_entry(object_ast, member_name, svc).nil?
        end

        def produce(node, context)
          svc = services(context)
          # Resolve module member function entry
          entry = resolve_entry(node.object, node.member, svc)

          # Get canonical name and build type using factory
          canonical_name = entry.canonical_name || entry.name
          func_type = svc.ir_builder.func_type(
            name: canonical_name,
            origin: node
          )

          # Build variable reference using factory
          svc.ir_builder.var(
            name: canonical_name,
            type: func_type,
            origin: node
          )
        end

        private

        def resolve_entry(object_ast, member_name, services)
          services.module_resolver.resolve_member_function(
            object_ast,
            member_name,
            services.function_registry,
            services.ast_type_checker
          )
        end
      end
    end
  end
end
