# frozen_string_literal: true

module MLC
  module Diagnostics
    # Formats errors in a Rust/Elm-like style with source context
    #
    # Example output:
    #   error[E0001]: Type mismatch
    #     --> program.mlc:15:10
    #      |
    #   15 |   let x = add("hello", 5)
    #      |           ^^^ expected i32, found str
    #      |
    #      = help: The function 'add' expects two integers
    #
    class ErrorFormatter
      COLORS = {
        error: "\e[1;31m",      # bold red
        warning: "\e[1;33m",    # bold yellow
        info: "\e[1;36m",       # bold cyan
        note: "\e[1;34m",       # bold blue
        help: "\e[1;32m",       # bold green
        location: "\e[1;34m",   # bold blue
        line_num: "\e[1;34m",   # bold blue
        highlight: "\e[1;31m",  # bold red
        reset: "\e[0m"
      }.freeze

      def initialize(use_color: true)
        @use_color = use_color && $stdout.tty?
      end

      # Format a compile error with full context
      def format_error(message:, origin: nil, error_code: nil, help: nil, notes: [])
        lines = []

        # Error header
        code_part = error_code ? "[#{error_code}]" : ""
        lines << "#{color(:error)}error#{code_part}: #{message}#{color(:reset)}"

        # Location and source context
        if origin&.line
          lines << format_location(origin)
          lines << format_source_context(origin)
        end

        # Notes
        notes.each do |note|
          lines << "      #{color(:note)}= note:#{color(:reset)} #{note}"
        end

        # Help suggestion
        if help
          lines << "      #{color(:help)}= help:#{color(:reset)} #{help}"
        end

        lines << ""
        lines.join("\n")
      end

      # Format a type error with expected/found
      def format_type_error(expected:, found:, origin: nil, context: nil)
        message = "Type mismatch: expected `#{expected}`, found `#{found}`"
        help = suggest_type_fix(expected, found)

        format_error(
          message: message,
          origin: origin,
          error_code: "E0308",
          help: help
        )
      end

      # Format an undefined variable error
      def format_undefined_var(name:, origin: nil, similar_names: [])
        message = "Cannot find variable `#{name}` in this scope"
        help = nil

        if similar_names.any?
          suggestions = similar_names.first(3).map { |n| "`#{n}`" }.join(", ")
          help = "Did you mean: #{suggestions}?"
        end

        format_error(
          message: message,
          origin: origin,
          error_code: "E0425",
          help: help
        )
      end

      # Format an undefined function error
      def format_undefined_function(name:, origin: nil, similar_names: [])
        message = "Cannot find function `#{name}` in this scope"
        help = nil

        if similar_names.any?
          suggestions = similar_names.first(3).map { |n| "`#{n}`" }.join(", ")
          help = "Did you mean: #{suggestions}?"
        end

        format_error(
          message: message,
          origin: origin,
          error_code: "E0424",
          help: help
        )
      end

      # Format a parse/syntax error
      def format_syntax_error(message:, origin: nil, expected: nil)
        help = expected ? "Expected #{expected}" : nil

        format_error(
          message: "Syntax error: #{message}",
          origin: origin,
          error_code: "E0001",
          help: help
        )
      end

      private

      def color(name)
        @use_color ? COLORS[name] : ""
      end

      def format_location(origin)
        file = origin.file || "<input>"
        line = origin.line || 1
        col = origin.column || 1

        "  #{color(:location)}-->#{color(:reset)} #{file}:#{line}:#{col}"
      end

      def format_source_context(origin)
        return "" unless origin.line_text

        lines = []
        line_num = origin.line.to_s
        gutter_width = line_num.length + 2

        # Empty gutter line
        lines << "#{color(:line_num)}#{' ' * gutter_width}|#{color(:reset)}"

        # Source line with line number
        padded_num = line_num.rjust(gutter_width - 1)
        lines << "#{color(:line_num)}#{padded_num} |#{color(:reset)} #{origin.line_text}"

        # Highlight indicator
        if origin.column
          start_col = [origin.column, 1].max
          end_col = origin.end_column || start_col
          width = [end_col - start_col + 1, 1].max

          indicator = " " * (start_col - 1) + color(:highlight) + "^" * width + color(:reset)
          lines << "#{color(:line_num)}#{' ' * gutter_width}|#{color(:reset)} #{indicator}"
        end

        # Empty gutter line
        lines << "#{color(:line_num)}#{' ' * gutter_width}|#{color(:reset)}"

        lines.join("\n")
      end

      def suggest_type_fix(expected, found)
        # Common type mismatch suggestions
        case [expected.to_s.downcase, found.to_s.downcase]
        when ["i32", "f32"], ["f32", "i32"]
          "Consider using an explicit type conversion"
        when ["str", "i32"], ["string", "i32"], ["string", "int"]
          "Use `to_string()` to convert integers to strings"
        when ["i32", "str"], ["int", "string"]
          "Use `parse()` to convert strings to integers"
        when ["bool", "i32"], ["bool", "int"]
          "Use a comparison operator (==, !=) to get a boolean"
        else
          nil
        end
      end
    end
  end
end
