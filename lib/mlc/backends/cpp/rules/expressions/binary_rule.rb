# frozen_string_literal: true

module MLC
  module Backends
    module Cpp
      module Rules
        module Expressions
          # Rule for lowering SemanticIR::BinaryExpr to C++ binary operators
          # Handles: +, -, *, /, %, ==, !=, <, >, <=, >=, &&, ||, &, |, ^, <<, >>, etc.
          class BinaryRule < ExpressionRule
            # C++ binary operator precedence (higher binds tighter). Matches the standard
            # C++ table; MLC's own recursive-descent parser (parse_logical_or ->
            # parse_logical_and -> parse_bitwise_or -> parse_bitwise_xor -> parse_bitwise_and
            # -> parse_equality -> parse_comparison -> parse_shift -> parse_addition ->
            # parse_multiplication) climbs through the exact same tiers, so a SemanticIR
            # BinaryExpr tree only ever nests a lower-precedence operator inside a
            # higher-precedence slot when the source had explicit parentheses.
            PRECEDENCE = {
              "*" => 12, "/" => 12, "%" => 12,
              "+" => 11, "-" => 11,
              "<<" => 10, ">>" => 10,
              "<" => 8, "<=" => 8, ">" => 8, ">=" => 8,
              "==" => 6, "!=" => 6,
              "&" => 5,
              "^" => 4,
              "|" => 3,
              "&&" => 2,
              "||" => 1
            }.freeze

            # Operators for which regrouping (a OP (b OP c)) vs ((a OP b) OP c) does not
            # change the result, so the right operand can drop parens when it repeats the
            # same operator as its parent. Every other same-precedence operator (-, /, %,
            # <<, >>, and the non-chaining comparison/equality operators) is grammatically
            # left-associative only: (a - b) - c != a - (b - c), so the right operand must
            # keep its parentheses whenever it shares the parent's precedence tier.
            FULLY_ASSOCIATIVE_OPERATORS = %w[+ * & | ^ && ||].freeze

            def applies?(node)
              context.checker.binary_expr?(node)
            end

            def apply(node)
              # Recursively lower left and right child expressions
              left = parenthesize_child_if_needed(lower_expression(node.left), node.left, node.op, side: :left)
              right = parenthesize_child_if_needed(lower_expression(node.right), node.right, node.op, side: :right)

              context.factory.binary_expression(
                left: left,
                operator: node.op,
                right: right,
                operator_prefix: " ",
                operator_suffix: " "
              )
            end

            private

            # Wrap a lowered child expression in parentheses when omitting them would
            # change what the emitted C++ parses back to (precedence- or
            # associativity-unsafe nesting). Grammatically left-associative parses (a
            # BinaryExpr child on the left with precedence >= the parent's, or on the
            # right with strictly higher precedence) reprint unambiguously without help.
            def parenthesize_child_if_needed(lowered_child, semantic_child, parent_operator, side:)
              return lowered_child unless semantic_child.is_a?(MLC::SemanticIR::BinaryExpr)

              child_precedence = PRECEDENCE[semantic_child.op]
              parent_precedence = PRECEDENCE[parent_operator]
              return lowered_child if child_precedence.nil? || parent_precedence.nil?

              needs_parens =
                if child_precedence != parent_precedence
                  child_precedence < parent_precedence
                else
                  side == :right &&
                    !(FULLY_ASSOCIATIVE_OPERATORS.include?(parent_operator) && semantic_child.op == parent_operator)
                end

              needs_parens ? context.factory.parenthesized(expression: lowered_child) : lowered_child
            end
          end
        end
      end
    end
  end
end
