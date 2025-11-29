# frozen_string_literal: true

module MLC
  module Source
  module Parser
    # Pattern parsing module
    # Handles parsing of pattern matching patterns (match expressions, destructuring)
    module PatternParser
      # Parse a pattern (entry point)
      # Patterns can be: wildcard, regex, literal, variable, constructor, tuple, array, or or-pattern
      def parse_pattern
        case current.type
        when :UNDERSCORE, :OPERATOR
          parse_wildcard_pattern
        when :REGEX
          parse_regex_pattern
        when :INT_LITERAL
          parse_int_literal_pattern
        when :FLOAT_LITERAL
          parse_float_literal_pattern
        when :STRING_LITERAL
          parse_string_literal_pattern
        when :TRUE, :FALSE
          parse_bool_literal_pattern
        when :LBRACKET
          parse_array_pattern
        when :LPAREN
          parse_tuple_pattern
        when :IDENTIFIER
          parse_identifier_pattern
        else
          raise "Unexpected token in pattern: #{current}"
        end
      end

      # Parse or-pattern: pattern1 | pattern2 | ...
      # Returns single pattern if no `|`, or OrPattern with alternatives
      def parse_or_pattern
        first_pattern = parse_pattern

        # Check for or-patterns: | pattern | pattern ...
        # But NOT | pattern => (that's a new match arm)
        alternatives = [first_pattern]

        while current.type == :OPERATOR && current.value == "|" && !or_pattern_ends_here?
          consume(:OPERATOR)  # consume |
          alternatives << parse_pattern
        end

        if alternatives.length == 1
          first_pattern
        else
          # Create or-pattern with origin from first pattern
          origin = first_pattern.respond_to?(:origin) ? first_pattern.origin : nil
          MLC::Source::AST::Pattern.new(
            kind: :or,
            data: { alternatives: alternatives },
            origin: origin
          )
        end
      end

      # Check if current position marks end of or-pattern (next is => or if)
      def or_pattern_ends_here?
        # Look ahead to see if | is followed by => or if (new arm) vs pattern (or-pattern)
        return true if peek&.type == :FAT_ARROW
        return true if peek&.type == :IF
        false
      end

      # Check if token is start of a pattern
      def pattern_start?(token)
        return false unless token
        case token.type
        when :UNDERSCORE, :REGEX, :INT_LITERAL, :FLOAT_LITERAL, :STRING_LITERAL, :TRUE, :FALSE, :IDENTIFIER, :LBRACKET, :LPAREN
          true
        when :OPERATOR
          token.value == "_"
        else
          false
        end
      end

      private

      # Parse wildcard pattern: _
      def parse_wildcard_pattern
        if current.type == :UNDERSCORE || (current.type == :OPERATOR && current.value == "_")
          token = current
          consume(current.type)
          return with_origin(token) { MLC::Source::AST::Pattern.new(kind: :wildcard, data: {}) }
        end
        raise "Unexpected operator in pattern: #{current.value}"
      end

      # Parse regex pattern: r"..." or r"..." as [x, y, z]
      def parse_regex_pattern
        regex_token = consume(:REGEX)
        regex_data = regex_token.value
        bindings = parse_regex_bindings if current.type == :AS

        with_origin(regex_token) do
          MLC::Source::AST::Pattern.new(
            kind: :regex,
            data: {
              pattern: regex_data[:pattern],
              flags: regex_data[:flags],
              bindings: bindings
            }
          )
        end
      end

      # Parse regex bindings: as [x, y, _]
      def parse_regex_bindings
        consume(:AS)
        return [] unless current.type == :LBRACKET

        consume(:LBRACKET)
        bindings = []

        while current.type != :RBRACKET
          bindings << parse_regex_binding
          break unless current.type == :COMMA
          consume(:COMMA)
        end

        consume(:RBRACKET)
        bindings
      end

      # Parse single regex binding: identifier or _
      def parse_regex_binding
        if current.type == :IDENTIFIER
          consume(:IDENTIFIER).value
        elsif current.type == :UNDERSCORE || (current.type == :OPERATOR && current.value == "_")
          consume(current.type)
          "_"
        else
          raise "Expected identifier or _ in regex bindings"
        end
      end

      # Parse integer literal pattern: 42
      def parse_int_literal_pattern
        token = consume(:INT_LITERAL)
        value = token.value.to_i
        with_origin(token) { MLC::Source::AST::Pattern.new(kind: :literal, data: {value: value}) }
      end

      # Parse float literal pattern: 3.14
      def parse_float_literal_pattern
        token = consume(:FLOAT_LITERAL)
        value = token.value.to_f
        with_origin(token) { MLC::Source::AST::Pattern.new(kind: :literal, data: {value: value}) }
      end

      # Parse string literal pattern: "hello"
      def parse_string_literal_pattern
        token = consume(:STRING_LITERAL)
        value = token.value
        with_origin(token) { MLC::Source::AST::Pattern.new(kind: :literal, data: {value: value}) }
      end

      # Parse boolean literal pattern: true | false
      def parse_bool_literal_pattern
        token = current
        consume(token.type)  # consume TRUE or FALSE
        value = (token.type == :TRUE)
        with_origin(token) { MLC::Source::AST::Pattern.new(kind: :literal, data: {value: value}) }
      end

      # Parse tuple pattern: (), (x,), (x, y), (x, y, z)
      # Similar to tuple literal syntax:
      #   () - empty tuple pattern
      #   (x,) - single-element tuple pattern (trailing comma required)
      #   (x, y) - two-element tuple pattern
      # Examples: (x, y), (_, z), (Some(a), b)
      def parse_tuple_pattern
        lparen_token = consume(:LPAREN)

        # Empty tuple pattern: ()
        if current.type == :RPAREN
          consume(:RPAREN)
          return with_origin(lparen_token) do
            MLC::Source::AST::Pattern.new(kind: :tuple, data: {elements: []})
          end
        end

        # Parse first element
        first_pattern = parse_pattern
        elements = [first_pattern]

        # Check for comma (required for tuple)
        if current.type == :COMMA
          consume(:COMMA)

          # Check if this is single-element tuple: (x,)
          if current.type == :RPAREN
            consume(:RPAREN)
            return with_origin(lparen_token) do
              MLC::Source::AST::Pattern.new(kind: :tuple, data: {elements: elements})
            end
          end

          # Parse remaining elements
          while current.type != :RPAREN
            elements << parse_pattern
            break unless current.type == :COMMA
            consume(:COMMA)
            break if current.type == :RPAREN  # Allow trailing comma
          end
        else
          # No comma - this is a grouped pattern, not a tuple
          # Just return the inner pattern
          consume(:RPAREN)
          return first_pattern
        end

        consume(:RPAREN)
        with_origin(lparen_token) do
          MLC::Source::AST::Pattern.new(kind: :tuple, data: {elements: elements})
        end
      end

      # Parse array pattern: [], [x], [x, y], [x, y, z]
      # Handles empty arrays and arrays with pattern elements
      # Examples: [], [_], [x, y], [1, 2, 3], [Some(x), None]
      def parse_array_pattern
        lbracket_token = consume(:LBRACKET)

        # Empty array pattern: []
        if current.type == :RBRACKET
          consume(:RBRACKET)
          return with_origin(lbracket_token) do
            MLC::Source::AST::Pattern.new(kind: :array, data: {elements: []})
          end
        end

        # Parse element patterns
        elements = []
        while current.type != :RBRACKET
          elements << parse_pattern
          break unless current.type == :COMMA
          consume(:COMMA)
        end

        consume(:RBRACKET)
        with_origin(lbracket_token) do
          MLC::Source::AST::Pattern.new(kind: :array, data: {elements: elements})
        end
      end

      # Parse identifier pattern: can be variable, constructor, or constructor with fields
      # Examples: x, Some(x), Point{x, y}
      def parse_identifier_pattern
        constructor_token = consume(:IDENTIFIER)
        constructor = constructor_token.value

        case current.type
        when :LPAREN
          parse_constructor_pattern_with_parens(constructor_token, constructor)
        when :LBRACE
          parse_constructor_pattern_with_braces(constructor_token, constructor)
        else
          parse_simple_identifier_pattern(constructor_token, constructor)
        end
      end

      # Parse constructor pattern with parentheses: Some(x)
      def parse_constructor_pattern_with_parens(constructor_token, constructor)
        consume(:LPAREN)
        fields = parse_pattern_fields(:RPAREN)
        consume(:RPAREN)

        with_origin(constructor_token) do
          MLC::Source::AST::Pattern.new(
            kind: :constructor,
            data: {name: constructor, fields: fields}
          )
        end
      end

      # Parse constructor pattern with braces: Point{x, y}
      def parse_constructor_pattern_with_braces(constructor_token, constructor)
        consume(:LBRACE)
        bindings = parse_pattern_bindings
        consume(:RBRACE)

        with_origin(constructor_token) do
          MLC::Source::AST::Pattern.new(
            kind: :constructor,
            data: {name: constructor, fields: bindings}
          )
        end
      end

      # Parse pattern fields (for constructor patterns)
      # Returns array of Pattern objects for nested patterns, or strings for simple bindings
      # Examples:
      #   Some(x) -> fields: ["x"] (simple binding)
      #   Ok(Some(v)) -> fields: [Pattern(kind: :constructor, name: "Some", fields: ["v"])]
      def parse_pattern_fields(end_token)
        fields = []

        while current.type != end_token
          # Check for nested constructor pattern (uppercase identifier followed by parentheses)
          if current.type == :IDENTIFIER && peek_type == :LPAREN && current.value[0] == current.value[0].upcase
            # Nested constructor pattern - recursively parse
            fields << parse_pattern
          elsif current.type == :IDENTIFIER
            # Simple variable binding
            fields << consume(:IDENTIFIER).value
          elsif current.type == :UNDERSCORE || (current.type == :OPERATOR && current.value == "_")
            consume(current.type)
            fields << "_"
          else
            raise "Expected identifier or _ in pattern, got #{current.type}"
          end

          break unless current.type == :COMMA
          consume(:COMMA)
        end

        fields
      end

      # Helper to peek at the next token type
      def peek_type
        peek&.type
      end

      # Parse pattern bindings (for record-style patterns)
      def parse_pattern_bindings
        bindings = []

        while current.type != :RBRACE
          bindings << consume(:IDENTIFIER).value
          break unless current.type == :COMMA
          consume(:COMMA)
        end

        bindings
      end

      # Parse simple identifier: determines if it's wildcard, constructor, variable, or boolean literal
      def parse_simple_identifier_pattern(constructor_token, constructor)
        with_origin(constructor_token) do
          if constructor == "_"
            MLC::Source::AST::Pattern.new(kind: :wildcard, data: {})
          elsif constructor == "true"
            # Boolean literal: true
            MLC::Source::AST::Pattern.new(kind: :literal, data: {value: true})
          elsif constructor == "false"
            # Boolean literal: false
            MLC::Source::AST::Pattern.new(kind: :literal, data: {value: false})
          elsif constructor[0] == constructor[0].upcase
            # Uppercase = constructor with no fields
            MLC::Source::AST::Pattern.new(
              kind: :constructor,
              data: {name: constructor, fields: []}
            )
          else
            # Lowercase = variable binding
            MLC::Source::AST::Pattern.new(kind: :var, data: {name: constructor})
          end
        end
      end
    end
  end
  end
end
