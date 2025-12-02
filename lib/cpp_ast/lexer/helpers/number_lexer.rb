# frozen_string_literal: true

module CppAst
  class Lexer
    module NumberLexer
      def scan_number(first_char, line, column)
        lexeme = first_char.dup

        if first_char == "0" && current_char
          if current_char.match?(/[xX]/)
            lexeme << advance
            while current_char&.match?(/[0-9a-fA-F]/)
              lexeme << advance
            end
          elsif current_char.match?(/[bB]/)
            lexeme << advance
            while current_char&.match?(/[01]/)
              lexeme << advance
            end
          elsif current_char.match?(/[0-7]/)
            while current_char&.match?(/[0-7]/)
              lexeme << advance
            end
          elsif current_char == "."
            lexeme << advance
            scan_float_fraction(lexeme)
          end
        elsif first_char == "."
          scan_float_fraction(lexeme)
        else
          while current_char&.match?(/[0-9]/)
            lexeme << advance
          end

          if current_char == "."
            next_char = peek(1)
            if next_char&.match?(/[0-9]/)
              lexeme << advance
              scan_float_fraction(lexeme)
            end
          elsif current_char&.match?(/[eE]/)
            scan_exponent(lexeme)
          end
        end

        scan_number_suffix(lexeme)

        Token.new(kind: :number, lexeme: lexeme, line: line, column: column)
      end

      def scan_float_fraction(lexeme)
        while current_char&.match?(/[0-9]/)
          lexeme << advance
        end

        scan_exponent(lexeme) if current_char&.match?(/[eE]/)
      end

      def scan_exponent(lexeme)
        lexeme << advance

        lexeme << advance if current_char&.match?(/[+-]/)

        while current_char&.match?(/[0-9]/)
          lexeme << advance
        end
      end

      def scan_number_suffix(lexeme)
        lexeme << advance if current_char&.match?(/[uU]/)

        if current_char&.match?(/[lL]/)
          lexeme << advance
          lexeme << advance if current_char&.match?(/[lL]/)
        end

        lexeme << advance if current_char&.match?(/[fF]/)
      end
    end
  end
end
