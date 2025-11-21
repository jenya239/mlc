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

        def parse
          parse_program
        end
      end
    end
  end
end
