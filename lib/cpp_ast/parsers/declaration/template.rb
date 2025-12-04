# frozen_string_literal: true

module CppAst
  module Parsers
    module TemplateParser
      def parse_template_declaration(leading_trivia)
        template_suffix = current_token.trailing_trivia
        expect(:keyword_template)

        less_suffix = current_token.trailing_trivia
        expect(:less)

        params = "".dup
        depth = 1

        loop do
          break if at_end?

          token_text = current_token.lexeme + current_token.trailing_trivia
          consume_token = true
          case current_token.kind
          when :less
            depth += 1
          when :greater
            depth -= 1
            if depth.zero?
              consume_token = false
              break
            end
          end

          params << token_text if consume_token
          advance_raw if consume_token
        end

        params_suffix = current_token.trailing_trivia
        expect(:greater)

        inner_leading = current_leading_trivia
        declaration, trailing = parse_statement(inner_leading)

        stmt = Nodes::TemplateDeclaration.new(
          leading_trivia: leading_trivia,
          template_params: params,
          declaration: declaration,
          template_suffix: template_suffix,
          less_suffix: less_suffix,
          params_suffix: params_suffix
        )

        [stmt, trailing]
      end
    end
  end
end
