# frozen_string_literal: true

module MLC
  # Enhanced error handling with rich diagnostics
  class EnhancedError < StandardError
    attr_reader :location, :suggestion, :context, :error_type

    def initialize(message, location: nil, suggestion: nil, context: nil, error_type: :general)
      super(message)
      @location = location
      @suggestion = suggestion
      @context = context
      @error_type = error_type
    end

    def formatted_message
      lines = []

      # Add location if available
      lines << if @location
        "#{@location}: #{message}"
      else
        message
               end

      # Add suggestion if available
      lines << "  💡 Suggestion: #{@suggestion}" if @suggestion

      # Add context if available
      lines << "  📍 Context: #{@context}" if @context

      # Add error type specific help
      lines << error_type_help

      lines.join("\n")
    end

    private

    def error_type_help
      case @error_type
      when :syntax
        "  🔧 This is a syntax error. Check your grammar and punctuation."
      when :type
        "  🔍 This is a type error. Check your variable types and function signatures."
      when :scope
        "  📦 This is a scope error. Check if the variable is defined in the current scope."
      when :import
        "  📁 This is an import error. Check if the module exists and is accessible."
      else
        "  ❓ This is a general error. Check your code for typos and logic issues."
      end
    end
  end

  # Syntax error with enhanced diagnostics
  class MLCSyntaxError < EnhancedError
    def initialize(message, location: nil, suggestion: nil, context: nil)
      super(message, location: location, suggestion: suggestion, context: context, error_type: :syntax)
    end
  end

  # Type error with enhanced diagnostics
  class MLCTypeError < EnhancedError
    def initialize(message, location: nil, suggestion: nil, context: nil)
      super(message, location: location, suggestion: suggestion, context: context, error_type: :type)
    end
  end

  # Scope error with enhanced diagnostics
  class MLCScopeError < EnhancedError
    def initialize(message, location: nil, suggestion: nil, context: nil)
      super(message, location: location, suggestion: suggestion, context: context, error_type: :scope)
    end
  end

  # Import error with enhanced diagnostics
  class MLCImportError < EnhancedError
    def initialize(message, location: nil, suggestion: nil, context: nil)
      super(message, location: location, suggestion: suggestion, context: context, error_type: :import)
    end
  end
end
