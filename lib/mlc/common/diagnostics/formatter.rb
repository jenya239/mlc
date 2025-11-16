# frozen_string_literal: true

module MLC
  module Common
    module Diagnostics
      # Rich diagnostic formatter with color support and multi-line context
      class Formatter
      SEVERITY_COLORS = {
        error: "\e[31m",    # Red
        warning: "\e[33m",  # Yellow
        info: "\e[36m",     # Cyan
        note: "\e[37m"      # White
      }.freeze

      RESET = "\e[0m"
      BOLD = "\e[1m"
      DIM = "\e[2m"

      attr_reader :color_enabled

      def initialize(color: :auto, context_lines: 2)
        @color_enabled = determine_color_support(color)
        @context_lines = context_lines
      end

      # Format a diagnostic message with source location and context
      #
      # @param severity [Symbol] :error, :warning, :info, or :note
      # @param message [String] The diagnostic message
      # @param origin [SourceOrigin, nil] Source location information
      # @param source [String, nil] Full source code for context display
      # @return [String] Formatted diagnostic message
      def format(severity:, message:, origin: nil, source: nil)
        parts = []

        # Location header
        if origin
          location = colorize(origin.label || "<unknown>", :bold)
          severity_label = colorize("#{severity}:", SEVERITY_COLORS[severity], :bold)
          parts << "#{location} #{severity_label} #{message}"
        else
          severity_label = colorize("#{severity}:", SEVERITY_COLORS[severity], :bold)
          parts << "#{severity_label} #{message}"
        end

        # Source context
        if origin && source
          context = format_source_context(origin, source)
          parts << context if context
        elsif origin && origin.line_text
          # Fallback to line_text in origin
          parts << format_single_line_context(origin)
        end

        parts.join("\n")
      end

      # Format source context with multiple lines around the error
      def format_source_context(origin, source)
        return nil unless origin.line

        lines = source.lines
        target_line = origin.line - 1 # 0-indexed
        return nil if target_line < 0 || target_line >= lines.size

        start_line = [target_line - @context_lines, 0].max
        end_line = [target_line + @context_lines, lines.size - 1].min

        output = []
        line_num_width = (end_line + 1).to_s.length

        (start_line..end_line).each do |idx|
          line_num = idx + 1
          line_content = lines[idx].chomp

          # Format line number
          if idx == target_line
            # Error line - bold and colored
            prefix = colorize(sprintf("%#{line_num_width}d", line_num), :bold)
            prefix += colorize(" | ", :bold)
            output << "#{prefix}#{line_content}"

            # Add highlight indicator
            if origin.start_column
              indicator = build_indicator(origin, line_content.length)
              indicator_prefix = " " * line_num_width + colorize(" | ", :bold)
              colored_indicator = colorize(indicator, SEVERITY_COLORS[:error], :bold)
              output << "#{indicator_prefix}#{colored_indicator}"
            end
          else
            # Context line - dimmed
            prefix = colorize(sprintf("%#{line_num_width}d", line_num), :dim)
            prefix += colorize(" | ", :dim)
            output << "#{prefix}#{line_content}"
          end
        end

        output.join("\n")
      end

      # Format single-line context (fallback when full source not available)
      def format_single_line_context(origin)
        return nil unless origin.line_text

        output = []
        output << origin.line_text.chomp

        if origin.start_column
          indicator = build_indicator(origin, origin.line_text.length)
          output << colorize(indicator, SEVERITY_COLORS[:error], :bold)
        end

        output.join("\n")
      end

      private

      def determine_color_support(color_option)
        case color_option
        when :auto
          # Check if stdout/stderr is a TTY and supports colors
          $stderr.tty? && (ENV["TERM"] && ENV["TERM"] != "dumb")
        when true, :always
          true
        when false, :never
          false
        else
          false
        end
      end

      def colorize(text, *codes)
        return text unless @color_enabled

        codes_str = codes.map { |code|
          code.is_a?(Symbol) ? const_get(code.upcase) : code
        }.join

        "#{codes_str}#{text}#{RESET}"
      end

      def build_indicator(origin, line_length)
        start_col = [origin.start_column - 1, 0].max
        finish_col = if origin.end_column && origin.end_column >= origin.start_column
                       [origin.end_column - 1, line_length].min
                     else
                       start_col
                     end

        width = finish_col - start_col + 1
        width = 1 if width <= 0

        " " * start_col + "^" * width
      end

      # Convenience class methods for common use cases
      class << self
        def format_error(message, origin: nil, source: nil, color: :auto)
          new(color: color).format(
            severity: :error,
            message: message,
            origin: origin,
            source: source
          )
        end

        def format_warning(message, origin: nil, source: nil, color: :auto)
          new(color: color).format(
            severity: :warning,
            message: message,
            origin: origin,
            source: source
          )
        end

        def format_info(message, origin: nil, source: nil, color: :auto)
          new(color: color).format(
            severity: :info,
            message: message,
            origin: origin,
            source: source
          )
        end

        def format_note(message, origin: nil, source: nil, color: :auto)
          new(color: color).format(
            severity: :note,
            message: message,
            origin: origin,
            source: source
          )
        end
      end
      end
    end
  end
end
