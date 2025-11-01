# frozen_string_literal: true

module MLC
  module Services
    # FunctionLookupService - Unified function metadata lookup
    # Phase 18-E: Extracted from IRGen
    #
    # Responsibilities:
    # - Lookup builtin function metadata (sqrt, print, println, etc.)
    # - Unified function lookup (registry → constructors → builtins)
    # - Function type placeholder generation for inference
    #
    # Dependencies:
    # - function_registry: FunctionRegistry (for user-defined functions)
    # - sum_type_constructors: Hash (for constructor functions)
    # - type_checker: TypeChecker (for io_return_type helper)
    # - type_inference: TypeInferenceService (for function_type_from_info)
    class FunctionLookupService
      IO_RETURN_TYPES = {
        "print" => "i32",
        "println" => "i32",
        "eprint" => "i32",
        "eprintln" => "i32",
        "read_line" => "string",
        "input" => "string",
        "args" => :array_of_string,
        "to_string" => "string",
        "format" => "string"
      }.freeze

      def initialize(function_registry:, sum_type_constructors:, type_checker:, type_inference:)
        @function_registry = function_registry
        @sum_type_constructors = sum_type_constructors
        @type_checker = type_checker
        @type_inference = type_inference
      end

      # Get builtin function metadata (sqrt, IO functions)
      # Returns MLC::IRGen::FunctionInfo for known builtins, nil otherwise
      #
      # @param name [String] Function name
      # @return [MLC::IRGen::FunctionInfo, nil]
      def builtin_function_info(name)
        case name
        when "sqrt"
          f32 = HighIR::Builder.primitive_type("f32")
          MLC::IRGen::FunctionInfo.new("sqrt", [f32], f32)
        else
          if IO_RETURN_TYPES.key?(name)
            MLC::IRGen::FunctionInfo.new(name, [], @type_checker.io_return_type(name))
          else
            nil
          end
        end
      end

      # Unified function metadata lookup
      # Searches: registry → constructors → builtins
      #
      # @param name [String] Function name
      # @return [MLC::IRGen::FunctionInfo, nil]
      def lookup_function_info(name)
        @function_registry.fetch(name) || @sum_type_constructors[name] || builtin_function_info(name)
      end

      # Generate function type placeholder for inference
      # Uses actual function info if available, otherwise creates auto-typed placeholder
      #
      # @param name [String] Function name
      # @return [HighIR::FunctionType]
      def function_placeholder_type(name)
        if (info = lookup_function_info(name))
          @type_inference.function_type_from_info(info)
        else
          HighIR::Builder.function_type([], HighIR::Builder.primitive_type("auto"))
        end
      end
    end
  end
end
