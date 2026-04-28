# frozen_string_literal: true

# rubocop:disable Metrics/ParameterLists

module MLC
  module Source
    module Parser
      class Token
        attr_reader :type, :value, :line, :column, :file,
                    :end_line, :end_column, :text, :line_text

        def initialize(type:, value:, line: 1, column: 1, file: nil,
                       end_line: line, end_column: column,
                       text: nil, line_text: nil)
          @type = type
          @value = value
          @line = line
          @column = column
          @file = file
          @end_line = end_line
          @end_column = end_column
          @text = text
          @line_text = line_text
        end

        def to_s
          "#{type}(#{value})"
        end
      end

      class Lexer
        KEYWORDS = %w[
          fn type let mut const return break continue if then else unless while for in do end match
          i8 i16 i32 i64 u8 u16 u32 u64 f32 f64 bool void str module export import enum from as extern
          unsafe trait extend async await record where ref
        ].freeze

        OPERATORS = %w[
          + - * / % = == != < > <= >= && || ! &
          . , ; : ( ) { } [ ]
        ].freeze

        def initialize(source, filename: nil)
          @source = source
          @pos = 0
          @line = 1
          @column = 1
          @tokens = []
          @filename = filename
        end

        def tokenize
          while @pos < @source.length
            skip_whitespace
            skip_comments
            next if @pos >= @source.length

            char = @source[@pos]

            case char
            when /[a-zA-Z_]/
              tokenize_identifier_or_keyword
            when /[0-9]/
              tokenize_number
            when '"'
              tokenize_string
            when "'"
              tokenize_raw_string
            when '`'
              tokenize_backtick_template
            when '<'
              handle_left_angle
            when '/'
              # Distinguish between division operator and regex literal
              handle_slash
            when '%'
              handle_percent
            when /[=+\-*>!&|.?^~]/
              tokenize_operator
            when '('
              add_token(:LPAREN, char)
              advance
            when ')'
              add_token(:RPAREN, char)
              advance
            when '{'
              add_token(:LBRACE, char)
              advance
            when '}'
              add_token(:RBRACE, char)
              advance
            when '['
              add_token(:LBRACKET, char)
              advance
            when ']'
              add_token(:RBRACKET, char)
              advance
            when ','
              add_token(:COMMA, char)
              advance
            when ';'
              add_token(:SEMICOLON, char)
              advance
            when ':'
              handle_colon
            when '@'
              # Self-access: @field -> self.field
              tokenize_self_access
            else
              advance # Skip unknown characters
            end
          end

          add_token(:EOF, "")
          @tokens
        end

        private

        def handle_left_angle
          return tokenize_operator unless heredoc_start?

          tokenize_heredoc
        end

        def heredoc_start?
          return false unless @pos + 1 < @source.length && @source[@pos + 1] == '<'

          heredoc_lookahead = @pos + 2
          heredoc_lookahead += 1 if heredoc_lookahead < @source.length && @source[heredoc_lookahead] == '~'
          heredoc_lookahead < @source.length && @source[heredoc_lookahead] =~ /[A-Z]/
        end

        def handle_slash
          return tokenize_regex if regex_context?

          tokenize_operator
        end

        def handle_percent
          return tokenize_percent_literal if @pos + 1 < @source.length && %w[w i W I].include?(@source[@pos + 1])

          tokenize_operator
        end

        def handle_colon
          return emit_colon_and_advance if next_char?(':')

          return emit_symbol_or_colon if next_char_identifier?

          emit_colon_and_advance
        end

        def next_char?(expected)
          @pos + 1 < @source.length && @source[@pos + 1] == expected
        end

        def next_char_identifier?
          @pos + 1 < @source.length && @source[@pos + 1] =~ /[a-zA-Z_]/
        end

        def emit_colon_and_advance
          add_token(:COLON, ':')
          advance
        end

        def emit_symbol_or_colon
          last_type = @tokens.last&.type
          if last_type == :COLON
            emit_colon_and_advance
          elsif symbol_context?
            tokenize_symbol
          else
            emit_colon_and_advance
          end
        end

        def skip_whitespace
          while @pos < @source.length && @source[@pos] =~ /\s/
            if @source[@pos] == "\n"
              @line += 1
              @column = 1
            else
              @column += 1
            end
            @pos += 1
          end
        end

        def skip_comments
          loop do
            break if @pos >= @source.length

            break unless skip_single_comment || skip_slash_comment || skip_block_comment

            skip_whitespace
          end
        end

        def skip_single_comment
          return false unless @source[@pos] == '#'

          @pos += 1
          @column += 1
          advance_until_newline
          true
        end

        def skip_slash_comment
          return false unless @pos < @source.length - 1 && @source[@pos] == '/' && @source[@pos + 1] == '/'

          @pos += 2
          @column += 2
          advance_until_newline
          true
        end

        def skip_block_comment
          return false unless @pos < @source.length - 1 && @source[@pos] == '/' && @source[@pos + 1] == '*'

          @pos += 2
          @column += 2
          while @pos < @source.length - 1
            if @source[@pos] == '*' && @source[@pos + 1] == '/'
              @pos += 2
              @column += 2
              return true
            end

            advance_char
          end

          true
        end

        def advance_until_newline
          while @pos < @source.length && @source[@pos] != "\n"
            @pos += 1
            @column += 1
          end
        end

        def advance_char
          if @source[@pos] == "\n"
            @line += 1
            @column = 1
          else
            @column += 1
          end
          @pos += 1
        end

        def tokenize_identifier_or_keyword
          start_line = @line
          start_column = @column
          start = @pos
          @pos += 1 while @pos < @source.length && @source[@pos] =~ /[a-zA-Z0-9_]/

          value = @source[start...@pos]
          @column += value.length

          type = KEYWORDS.include?(value) ? value.upcase.to_sym : :IDENTIFIER
          add_token(type, value, line: start_line, column: start_column)
        end

        def tokenize_number
          start_line = @line
          start_column = @column
          start = @pos

          # Check for prefixed literals: 0b, 0o, 0x
          if @source[@pos] == '0' && @pos + 1 < @source.length
            prefix_char = @source[@pos + 1].downcase
            case prefix_char
            when 'b'
              return tokenize_binary_literal(start_line, start_column)
            when 'o'
              return tokenize_octal_literal(start_line, start_column)
            when 'x'
              return tokenize_hex_literal(start_line, start_column)
            end
          end

          # Decimal integer or float with optional underscore separators
          @pos += 1 while @pos < @source.length && @source[@pos] =~ /[0-9_]/

          # Check for decimal point (but not range operator ..)
          # Only consume '.' if followed by a digit, not another '.'
          if @pos < @source.length && @source[@pos] == '.' &&
             @pos + 1 < @source.length && @source[@pos + 1] =~ /[0-9]/
            @pos += 1
            @pos += 1 while @pos < @source.length && @source[@pos] =~ /[0-9_]/
            raw_value = @source[start...@pos]
            @column += raw_value.length
            # Remove underscores before conversion
            clean_value = raw_value.delete('_')
            add_token(:FLOAT_LITERAL, clean_value.to_f, line: start_line, column: start_column)
          else
            raw_value = @source[start...@pos]
            @column += raw_value.length
            # Remove underscores before conversion
            clean_value = raw_value.delete('_')
            add_token(:INT_LITERAL, clean_value.to_i, line: start_line, column: start_column)
          end
        end

        def tokenize_binary_literal(start_line, start_column)
          start = @pos
          @pos += 2 # Skip 0b

          @pos += 1 while @pos < @source.length && @source[@pos] =~ /[01_]/

          raw_value = @source[start...@pos]
          @column += raw_value.length

          # Extract binary digits (after 0b), remove underscores
          binary_part = raw_value[2..].delete('_')
          raise "Invalid binary literal: #{raw_value}" if binary_part.empty?

          int_value = binary_part.to_i(2)
          add_token(:INT_LITERAL, int_value, line: start_line, column: start_column)
        end

        def tokenize_octal_literal(start_line, start_column)
          start = @pos
          @pos += 2 # Skip 0o

          @pos += 1 while @pos < @source.length && @source[@pos] =~ /[0-7_]/

          raw_value = @source[start...@pos]
          @column += raw_value.length

          # Extract octal digits (after 0o), remove underscores
          octal_part = raw_value[2..].delete('_')
          raise "Invalid octal literal: #{raw_value}" if octal_part.empty?

          int_value = octal_part.to_i(8)
          add_token(:INT_LITERAL, int_value, line: start_line, column: start_column)
        end

        def tokenize_hex_literal(start_line, start_column)
          start = @pos
          @pos += 2 # Skip 0x

          @pos += 1 while @pos < @source.length && @source[@pos] =~ /[0-9a-fA-F_]/

          raw_value = @source[start...@pos]
          @column += raw_value.length

          # Extract hex digits (after 0x), remove underscores
          hex_part = raw_value[2..].delete('_')
          raise "Invalid hex literal: #{raw_value}" if hex_part.empty?

          int_value = hex_part.to_i(16)
          add_token(:INT_LITERAL, int_value, line: start_line, column: start_column)
        end

        def tokenize_string
          start_line = @line
          start_column = @column
          @pos += 1 # Skip opening quote
          @column += 1

          parts = [] # Array of {type: :text/:expr, value: ...}
          current_text = []
          has_interpolation = false

          while @pos < @source.length && @source[@pos] != '"'
            if @source[@pos] == '\\'
              consume_string_escape(current_text)
              next
            end

            if @source[@pos] == '$' && @pos + 1 < @source.length && @source[@pos + 1] == '{'
              @pos += 1
              @column += 1
              has_interpolation ||= handle_interpolation(parts, current_text)
              next
            end

            append_string_char(current_text)
          end

          # Add remaining text
          parts << { type: :text, value: current_text.join } if current_text.any?

          @pos += 1 # Skip closing quote
          @column += 1

          if has_interpolation
            add_token(:STRING_INTERP, parts, line: start_line, column: start_column)
          else
            # Simple string - extract value
            value = parts.empty? ? "" : parts.first[:value]
            add_token(:STRING_LITERAL, value, line: start_line, column: start_column)
          end
        end

        def tokenize_backtick_template
          start_line = @line
          start_column = @column
          @pos += 1
          @column += 1

          parts = []
          current_text = []
          has_interpolation = false

          while @pos < @source.length && @source[@pos] != '`'
            if @source[@pos] == '\\'
              @pos += 1
              @column += 1
              raise ParseError, "unterminated escape in template literal" if @pos >= @source.length

              escaped = @source[@pos]
              case escaped
              when '`'
                current_text << '`'
              when '$'
                if @pos + 1 < @source.length && @source[@pos + 1] == '{'
                  current_text << '$'
                  @pos += 1
                  @column += 1
                  current_text << '{'
                else
                  current_text << "\\"
                  current_text << escaped
                end
              when 'n' then current_text << "\n"
              when 't' then current_text << "\t"
              when 'r' then current_text << "\r"
              when '\\' then current_text << "\\"
              else
                current_text << "\\"
                current_text << escaped
              end
              @pos += 1
              @column += 1
              next
            end

            if @source[@pos] == '$' && @pos + 1 < @source.length && @source[@pos + 1] == '{'
              @pos += 1
              @column += 1
              has_interpolation = true if handle_interpolation(parts, current_text)
              next
            end

            if @source[@pos] == "\n"
              @line += 1
              @column = 1
            else
              @column += 1
            end
            current_text << @source[@pos]
            @pos += 1
          end

          raise ParseError, "unterminated template literal" if @pos >= @source.length

          parts << { type: :text, value: current_text.join } if current_text.any?
          @pos += 1
          @column += 1

          if has_interpolation
            add_token(:STRING_INTERP, parts, line: start_line, column: start_column)
          else
            value = parts.empty? ? "" : parts.first[:value]
            add_token(:STRING_LITERAL, value, line: start_line, column: start_column)
          end
        end

        def consume_string_escape(current_text)
          @pos += 1
          @column += 1
          raise "Unterminated escape sequence" if @pos >= @source.length

          escaped = @source[@pos]
          current_text << case escaped
                          when 'n' then "\n"
                          when 't' then "\t"
                          when 'r' then "\r"
                          when '\\' then "\\"
                          when '"' then '"'
                          when '0' then "\0"
                          when '$' then '$'
                          when '{' then '{'
                          when '}' then '}'
                          else
                            "\\#{escaped}"
                          end
          @pos += 1
          @column += 1
        end

        def handle_interpolation(parts, current_text)
          @pos += 1 # Skip {
          @column += 1

          expr_value = read_interpolation_expression
          if expr_value.nil?
            current_text << '{'
            current_text << '}'
            return false
          end

          parts << { type: :text, value: current_text.join } if current_text.any?
          current_text.clear
          parts << { type: :expr, value: expr_value }
          true
        end

        def read_interpolation_expression
          expr_chars = []
          brace_depth = 1
          while @pos < @source.length && brace_depth.positive?
            char = @source[@pos]
            case char
            when '{'
              brace_depth += 1
              expr_chars << char
            when '}'
              brace_depth -= 1
              expr_chars << char if brace_depth.positive?
            when '"'
              expr_chars << char
              skip_nested_string(expr_chars)
            else
              expr_chars << char
            end

            update_position_for_char
          end

          expr_value = expr_chars.join.strip
          return nil if expr_value.empty?

          expr_value
        end

        def skip_nested_string(expr_chars)
          @pos += 1
          @column += 1
          while @pos < @source.length && @source[@pos] != '"'
            expr_chars << @source[@pos]
            if @source[@pos] == '\\'
              @pos += 1
              @column += 1
              expr_chars << @source[@pos] if @pos < @source.length
            end
            if @source[@pos] == "\n"
              @line += 1
              @column = 1
            else
              @column += 1
            end
            @pos += 1
          end
          expr_chars << @source[@pos] if @pos < @source.length && @source[@pos] == '"'
        end

        def update_position_for_char
          if @source[@pos] == "\n"
            @line += 1
            @column = 1
          else
            @column += 1
          end
          @pos += 1
        end

        def append_string_char(current_text)
          if @source[@pos] == "\n"
            @line += 1
            @column = 1
          else
            @column += 1
          end
          current_text << @source[@pos]
          @pos += 1
        end

        def finalize_percent_word(words, current_word)
          return if current_word.empty?

          words << current_word.join
          current_word.clear
        end

        def advance_whitespace_char(char)
          if char == "\n"
            @line += 1
            @column = 1
          else
            @column += 1
          end
          @pos += 1
        end

        def consume_percent_escape(current_word, close_delim)
          @pos += 1
          @column += 1
          return unless @pos < @source.length

          escaped = @source[@pos]
          current_word << case escaped
                          when 'n' then "\n"
                          when 't' then "\t"
                          when 'r' then "\r"
                          when 's' then " "
                          when '\\' then "\\"
                          when close_delim then close_delim
                          else
                            escaped
                          end
          @pos += 1
          @column += 1
        end

        # Single-quoted strings: no interpolation, minimal escape sequences
        # Only \\ and \' are recognized escape sequences (like Ruby)
        def tokenize_raw_string
          start_line = @line
          start_column = @column
          @pos += 1 # Skip opening quote
          @column += 1

          value = []
          char_count = 0
          escaped_char = nil

          while @pos < @source.length && @source[@pos] != "'"
            if @source[@pos] == '\\'
              # Check for escape sequence
              @pos += 1
              @column += 1
              raise "Unterminated escape sequence in single-quoted string" if @pos >= @source.length

              case @source[@pos]
              when '\\'
                value << "\\"
                escaped_char = "\\" if char_count.zero?
              when "'"
                value << "'"
                escaped_char = "'" if char_count.zero?
              when 'n'
                value << "\n"
                escaped_char = "\n" if char_count.zero?
              when 't'
                value << "\t"
                escaped_char = "\t" if char_count.zero?
              when 'r'
                value << "\r"
                escaped_char = "\r" if char_count.zero?
              when '0'
                value << "\0"
                escaped_char = "\0" if char_count.zero?
              else
                # Unlike Ruby, we keep the backslash for unknown escapes
                value << '\\'
                value << @source[@pos]
                char_count += 1 # Count both backslash and char
              end
              char_count += 1
              @pos += 1
              @column += 1
            else
              if @source[@pos] == "\n"
                @line += 1
                @column = 1
              else
                @column += 1
              end
              value << @source[@pos]
              char_count += 1
              @pos += 1
            end
          end

          raise "Unterminated single-quoted string" if @pos >= @source.length

          @pos += 1 # Skip closing quote
          @column += 1

          # Single character or escape sequence -> CHAR_LITERAL (integer value)
          # Multiple characters -> STRING_LITERAL (raw string)
          if char_count == 1
            char_value = escaped_char || value.first
            add_token(:CHAR_LITERAL, char_value.ord, line: start_line, column: start_column)
          else
            add_token(:STRING_LITERAL, value.join, line: start_line, column: start_column)
          end
        end

        def tokenize_heredoc
          start_line = @line
          start_column = @column

          # Skip << or <<~
          @pos += 2
          @column += 2
          strip_indent = false

          # Check for ~ (indented heredoc)
          if @pos < @source.length && @source[@pos] == '~'
            strip_indent = true
            @pos += 1
            @column += 1
          end

          # Read delimiter (must be uppercase identifier)
          delimiter_start = @pos
          while @pos < @source.length && @source[@pos] =~ /[A-Z0-9_]/
            @pos += 1
            @column += 1
          end

          delimiter = @source[delimiter_start...@pos]
          raise "Heredoc delimiter must be an uppercase identifier" if delimiter.empty?

          # Skip to end of line
          while @pos < @source.length && @source[@pos] != "\n"
            @pos += 1
            @column += 1
          end

          if @pos < @source.length
            @pos += 1 # Skip newline
            @line += 1
            @column = 1
          end

          # Collect heredoc body lines
          body_lines = []

          while @pos < @source.length
            line_start = @pos

            # Read current line
            @pos += 1 while @pos < @source.length && @source[@pos] != "\n"

            line = @source[line_start...@pos]

            # Check if this line is the delimiter
            if line.strip == delimiter
              # Found end delimiter
              if @pos < @source.length
                @pos += 1 # Skip newline
                @line += 1
                @column = 1
              end
              break
            end

            body_lines << line

            next unless @pos < @source.length

            @pos += 1 # Skip newline
            @line += 1
            @column = 1
          end

          # Process body based on strip_indent flag
          value = if strip_indent
                    strip_common_indent(body_lines)
                  else
                    body_lines.join("\n")
                  end

          add_token(:STRING_LITERAL, value, line: start_line, column: start_column)
        end

        def strip_common_indent(lines)
          # Find minimum indentation (ignoring empty lines)
          non_empty_lines = lines.reject { |line| line.strip.empty? }
          return "" if non_empty_lines.empty?

          min_indent = non_empty_lines.map do |line|
            line.match(/^(\s*)/)[1].length
          end.min

          # Strip min_indent from all lines
          stripped_lines = lines.map do |line|
            if line.strip.empty?
              "" # Keep empty lines empty
            else
              line[min_indent..] || ""
            end
          end

          stripped_lines.join("\n")
        end

        def regex_context?
          # Regex can appear after: =, (, [, {, ,, return, :, =>, operators
          # Regex cannot appear after: ), ], identifiers, numbers, strings
          return true if @tokens.empty?

          last_token = @tokens.last
          return false if last_token.nil?

          # After these tokens, / starts a regex
          regex_after = [:EQUAL, :LPAREN, :LBRACKET, :LBRACE, :COMMA,
                         :RETURN, :COLON, :FAT_ARROW, :ARROW, :OPERATOR]

          regex_after.include?(last_token.type)
        end

        def symbol_context?
          # Symbol literals can appear after: =, (, [, {, ,, return, =>, operators, keywords
          # Symbol literals CANNOT appear after: IDENTIFIER (that would be ::path)
          # or ), ], numbers, strings (that would be type annotation)
          return true if @tokens.empty?

          last_token = @tokens.last
          return false if last_token.nil?

          # After IDENTIFIER, : starts a path separator or type annotation, not a symbol
          # After RPAREN/RBRACKET, : is type annotation, not symbol
          # After keywords (if, then, else, etc.), : starts a symbol
          symbol_after = [
            :EQUAL, :LPAREN, :LBRACKET, :LBRACE, :COMMA,
            :RETURN, :FAT_ARROW, :ARROW, :OPERATOR, :COLON,
            :SYMBOL, # :foo :bar is two symbols in a row
            # Keywords that can precede expressions
            :IF, :THEN, :ELSE, :UNLESS, :WHILE, :FOR, :IN, :DO, :LET, :MUT,
            :MATCH, :FN, :SEMICOLON, :PIPE
          ]

          symbol_after.include?(last_token.type)
        end

        def tokenize_regex
          # Regex format: /pattern/flags
          start_line = @line
          start_column = @column

          # Skip opening /
          @pos += 1
          @column += 1

          pattern = read_regex_pattern

          # Read flags (optional)
          flags = ""
          while @pos < @source.length && @source[@pos] =~ /[gimsuvy]/
            flags += @source[@pos]
            @pos += 1
            @column += 1
          end

          # Store both pattern and flags in the token value
          add_token(:REGEX, { pattern: pattern, flags: flags }, line: start_line, column: start_column)
        end

        def read_regex_pattern
          pattern = +""

          # Read pattern until closing /
          while @pos < @source.length
            char = @source[@pos]

            case char
            when '/'
              # Found closing /
              @pos += 1
              @column += 1
              break
            when '\\'
              pattern << escape_sequence
            when "\n"
              # Newline in regex pattern is an error, but we'll be permissive
              @line += 1
              @column = 1
              pattern << char
              @pos += 1
            else
              pattern << char
              @pos += 1
              @column += 1
            end
          end

          pattern
        end

        def escape_sequence
          escaped = @source[@pos, 2]
          @pos += 1
          @column += 1
          if @pos < @source.length
            @pos += 1
            @column += 1
          end
          escaped
        end

        # Tokenize :identifier -> SYMBOL token
        # Symbol literal like :foo, :bar, :my_symbol
        def tokenize_symbol
          start_line = @line
          start_column = @column

          @pos += 1 # Skip :
          @column += 1

          # Read identifier
          start = @pos
          @pos += 1 while @pos < @source.length && @source[@pos] =~ /[a-zA-Z0-9_]/

          symbol_name = @source[start...@pos]
          @column += symbol_name.length

          add_token(:SYMBOL, symbol_name, line: start_line, column: start_column)
        end

        # Tokenize @identifier -> SELF_ACCESS token
        # @field desugars to self.field in the parser
        def tokenize_self_access
          start_line = @line
          start_column = @column

          @pos += 1 # Skip @
          @column += 1

          # Check if followed by identifier
          return unless @pos < @source.length && @source[@pos] =~ /[a-zA-Z_]/

          start = @pos
          @pos += 1 while @pos < @source.length && @source[@pos] =~ /[a-zA-Z0-9_]/

          field_name = @source[start...@pos]
          @column += field_name.length

          add_token(:SELF_ACCESS, field_name, line: start_line, column: start_column)
        end

        # Tokenize percent literals: %w[], %i[], %W[], %I[]
        # %w[] - array of strings (no interpolation)
        # %i[] - array of symbols (no interpolation)
        # %W[] - array of strings (with interpolation)
        # %I[] - array of symbols (with interpolation)
        def tokenize_percent_literal
          start_line = @line
          start_column = @column

          @pos += 1  # Skip %
          @column += 1

          return unless @pos < @source.length

          modifier = @source[@pos]
          return unless %w[w i W I].include?(modifier)

          @pos += 1  # Skip modifier (w/i/W/I)
          @column += 1

          return unless @pos < @source.length

          # Determine delimiter - support [] {} () <>
          open_delim = @source[@pos]
          close_delim = case open_delim
                        when '[' then ']'
                        when '{' then '}'
                        when '(' then ')'
                        when '<' then '>'
                        else return # Invalid delimiter, skip
                        end

          @pos += 1 # Skip opening delimiter
          @column += 1

          # Parse whitespace-separated words
          words = []
          current_word = []

          while @pos < @source.length && @source[@pos] != close_delim
            char = @source[@pos]

            if char =~ /\s/
              finalize_percent_word(words, current_word)
              advance_whitespace_char(char)
              next
            end

            if char == '\\'
              consume_percent_escape(current_word, close_delim)
              next
            end

            current_word << char
            @pos += 1
            @column += 1
          end

          # Add last word if present
          words << current_word.join if current_word.any?

          # Skip closing delimiter
          if @pos < @source.length && @source[@pos] == close_delim
            @pos += 1
            @column += 1
          end

          # Create appropriate token
          interpolated = %w[W I].include?(modifier)
          is_symbol = %w[i I].include?(modifier)

          token_type = if is_symbol
                         interpolated ? :SYMBOL_ARRAY_INTERP : :SYMBOL_ARRAY
                       else
                         interpolated ? :STRING_ARRAY_INTERP : :STRING_ARRAY
                       end

          add_token(token_type, words, line: start_line, column: start_column)
        end

        def tokenize_operator
          start_line = @line
          start_column = @column
          char = @source[@pos]

          # Handle arrow operator ->
          if char == '-' && @pos + 1 < @source.length && @source[@pos + 1] == '>'
            @pos += 2
            @column += 2
            add_token(:ARROW, "->", line: start_line, column: start_column)
            return
          end

          # Handle fat arrow operator =>
          if char == '=' && @pos + 1 < @source.length && @source[@pos + 1] == '>'
            @pos += 2
            @column += 2
            add_token(:FAT_ARROW, "=>", line: start_line, column: start_column)
            return
          end

          # Handle pipe operator |>
          if char == '|' && @pos + 1 < @source.length && @source[@pos + 1] == '>'
            @pos += 2
            @column += 2
            add_token(:PIPE, "|>", line: start_line, column: start_column)
            return
          end

          # Handle left shift operator <<
          if char == '<' && @pos + 1 < @source.length && @source[@pos + 1] == '<'
            @pos += 2
            @column += 2
            add_token(:OPERATOR, "<<", line: start_line, column: start_column)
            return
          end

          # Handle right shift operator >>
          if char == '>' && @pos + 1 < @source.length && @source[@pos + 1] == '>'
            @pos += 2
            @column += 2
            add_token(:OPERATOR, ">>", line: start_line, column: start_column)
            return
          end

          # Handle bitwise NOT operator ~
          if char == '~'
            @pos += 1
            @column += 1
            add_token(:OPERATOR, "~", line: start_line, column: start_column)
            return
          end

          # Handle bitwise XOR operator ^
          if char == '^'
            @pos += 1
            @column += 1
            add_token(:OPERATOR, "^", line: start_line, column: start_column)
            return
          end

          # Handle spread operator ... (must check before range ..)
          if char == '.' && @pos + 2 < @source.length && @source[@pos + 1] == '.' && @source[@pos + 2] == '.'
            @pos += 3
            @column += 3
            add_token(:SPREAD, "...", line: start_line, column: start_column)
            return
          end

          # Handle range operator ..
          if char == '.' && @pos + 1 < @source.length && @source[@pos + 1] == '.'
            @pos += 2
            @column += 2
            add_token(:RANGE, "..", line: start_line, column: start_column)
            return
          end

          # Handle safe navigation operator ?.
          if char == '?' && @pos + 1 < @source.length && @source[@pos + 1] == '.'
            @pos += 2
            @column += 2
            add_token(:SAFE_NAV, "?.", line: start_line, column: start_column)
            return
          end

          # Handle try/propagation operator ? (postfix, like Rust's ?)
          if char == '?'
            @pos += 1
            @column += 1
            add_token(:QUESTION, "?", line: start_line, column: start_column)
            return
          end

          # Handle multi-character operators
          if @pos + 1 < @source.length
            next_char = @source[@pos + 1]
            two_char = char + next_char

            if %w[== != <= >= && ||].include?(two_char)
              @pos += 2
              @column += 2
              add_token(:OPERATOR, two_char, line: start_line, column: start_column)
              return
            end
          end

          # Special handling for single =
          if char == '='
            @pos += 1
            @column += 1
            add_token(:EQUAL, char, line: start_line, column: start_column)
            return
          end

          @pos += 1
          @column += 1
          add_token(:OPERATOR, char, line: start_line, column: start_column)
        end

        def add_token(type, value, line: @line, column: @column)
          @tokens << Token.new(
            type: type,
            value: value,
            line: line,
            column: column,
            file: @filename
          )
        end

        def advance
          @pos += 1
          @column += 1
        end
      end
    end
  end
end
# rubocop:enable Metrics/ParameterLists
