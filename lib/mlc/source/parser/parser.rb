# frozen_string_literal: true

# Zeitwerk autoloads parser modules and Lexer via const_missing
# Only AST nodes require explicit loading (used throughout parser modules)
require_relative "../ast/nodes"

module MLC
  module Source
    module Parser
      class Parser
        # Include all parser modules
        include BaseParser
        include ExpressionParser
        include PatternParser
        include TypeParser
        include StatementParser
        include DeclarationParser

        def initialize(source, filename: nil)
          initialize_parser(source, filename: filename)
        end

        # Alternative constructor from pre-tokenized tokens
        def self.new_from_tokens(tokens)
          parser = allocate
          parser.instance_variable_set(:@tokens, tokens)
          parser.instance_variable_set(:@pos, 0)
          parser.instance_variable_set(:@last_token, nil)
          parser
        end

        def parse
          parse_program
        end

        # Parse a single expression (for string interpolation sub-parsing)
        def parse_single_expression
          parse_expression
        end
      end
    end
  end
end
