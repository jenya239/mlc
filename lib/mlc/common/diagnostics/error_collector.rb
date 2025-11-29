# frozen_string_literal: true

module MLC
  module Diagnostics
    # Represents a collected diagnostic (error, warning, note)
    class Diagnostic
      attr_reader :severity, :message, :origin, :error_code, :help, :notes

      SEVERITIES = [:error, :warning, :note, :info].freeze

      def initialize(severity:, message:, origin: nil, error_code: nil, help: nil, notes: [])
        raise ArgumentError, "Unknown severity: #{severity}" unless SEVERITIES.include?(severity)

        @severity = severity
        @message = message
        @origin = origin
        @error_code = error_code
        @help = help
        @notes = notes
      end

      def error?
        @severity == :error
      end

      def warning?
        @severity == :warning
      end
    end

    # Collects multiple diagnostics during compilation
    # Allows type checking to continue after errors
    class ErrorCollector
      attr_reader :diagnostics

      def initialize(max_errors: 100)
        @diagnostics = []
        @max_errors = max_errors
        @error_count = 0
      end

      # Add an error diagnostic
      def add_error(message, origin: nil, error_code: nil, help: nil, notes: [])
        return if @error_count >= @max_errors

        @error_count += 1
        @diagnostics << Diagnostic.new(
          severity: :error,
          message: message,
          origin: origin,
          error_code: error_code,
          help: help,
          notes: notes
        )
      end

      # Add a warning diagnostic
      def add_warning(message, origin: nil, error_code: nil, help: nil, notes: [])
        @diagnostics << Diagnostic.new(
          severity: :warning,
          message: message,
          origin: origin,
          error_code: error_code,
          help: help,
          notes: notes
        )
      end

      # Add a note diagnostic
      def add_note(message, origin: nil)
        @diagnostics << Diagnostic.new(
          severity: :note,
          message: message,
          origin: origin
        )
      end

      def errors
        @diagnostics.select(&:error?)
      end

      def warnings
        @diagnostics.select(&:warning?)
      end

      def has_errors?
        @error_count > 0
      end

      def error_count
        @error_count
      end

      def clear
        @diagnostics.clear
        @error_count = 0
      end

      # Format all diagnostics using ErrorFormatter
      def format_all(use_color: true)
        require_relative "error_formatter"
        formatter = ErrorFormatter.new(use_color: use_color)

        @diagnostics.map do |diag|
          formatter.format_error(
            message: diag.message,
            origin: diag.origin,
            error_code: diag.error_code,
            help: diag.help,
            notes: diag.notes
          )
        end.join("\n")
      end

      # Raise if there are any errors (for compatibility with existing code)
      def raise_if_errors!
        return unless has_errors?

        first_error = errors.first
        raise MLC::CompileError.new(first_error.message, origin: first_error.origin)
      end

      # Get a summary line
      def summary
        error_word = @error_count == 1 ? "error" : "errors"
        warning_count = warnings.count
        warning_word = warning_count == 1 ? "warning" : "warnings"

        parts = []
        parts << "#{@error_count} #{error_word}" if @error_count > 0
        parts << "#{warning_count} #{warning_word}" if warning_count > 0

        parts.empty? ? "no errors" : parts.join(", ")
      end
    end
  end
end
