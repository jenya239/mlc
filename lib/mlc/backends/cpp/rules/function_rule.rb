# frozen_string_literal: true


module MLC
  module Backends
    module Cpp
      module Rules
        # Rule for applying function-level transformations
        # Handles constexpr/noexcept modifiers based on SemanticIR::Func effects
        #
        # Unlike expression/statement rules, this operates on already-lowered CppAst
        # nodes and decorates them with additional modifiers.
        class FunctionRule < BaseRule
          def applies?(node)
            node.is_a?(CppAst::Nodes::FunctionDeclaration)
          end

          # Apply function effects (constexpr, noexcept) to CppAst function declaration
          # @param node [CppAst::Nodes::FunctionDeclaration] The C++ function declaration
          # @param semantic_func [SemanticIR::Func] The semantic function (for effects)
          # @param event_bus [Infrastructure::EventBus] Optional event bus for notifications
          # @return [CppAst::Nodes::FunctionDeclaration] Modified function declaration
          def apply(node, semantic_func:, event_bus: nil)
            effects = Array(semantic_func.effects)

            if effects.include?(:constexpr)
              node.prefix_modifiers = merge_prefix_modifier(node.prefix_modifiers, "constexpr")
            end

            if effects.include?(:noexcept)
              node.modifiers_text = merge_suffix_modifier(node.modifiers_text, "noexcept")
            end

            event_bus&.publish(
              :cpp_function_rule_applied,
              name: semantic_func.name,
              effects: effects
            )

            node
          end

          private

          # Merge a prefix modifier (like 'constexpr') into existing prefix modifiers
          # @param existing [String] Existing prefix modifiers
          # @param modifier [String] Modifier to add
          # @return [String] Merged prefix modifiers with trailing space
          def merge_prefix_modifier(existing, modifier)
            tokens = existing.to_s.split(/\s+/).reject(&:empty?)
            tokens.unshift(modifier) unless tokens.include?(modifier)
            tokens.empty? ? "" : "#{tokens.join(' ')} "
          end

          # Merge a suffix modifier (like 'noexcept') into existing suffix modifiers
          # @param existing [String] Existing suffix modifiers
          # @param modifier [String] Modifier to add
          # @return [String] Merged suffix modifiers
          def merge_suffix_modifier(existing, modifier)
            text = existing.to_s
            return text if text.include?(modifier)

            suffix = modifier.start_with?(" ") ? modifier : " #{modifier}"
            text + suffix
          end
        end
      end
    end
  end
end
