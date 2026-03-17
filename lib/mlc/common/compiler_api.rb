# frozen_string_literal: true

# rubocop:disable Metrics/ParameterLists

# All classes autoloaded by Zeitwerk on-demand:
# - MLC::ParseError (line 49)
# - MLC::CompileError (lines 69, 73, 93, 97, 140, 145)
# - MLC::Source::Parser::Parser
# - MLC::Representations::Semantic::Gen::Pipeline
# - MLC::Backends::Cpp::Codegen
# - MLC::Backends::Cpp::HeaderGenerator
# - MLC::Backends::Cpp::MetadataGenerator
# - MLC::Common::Stdlib::Scanner

module MLC
  class << self
    # Main entry point: Parse MLC source and return C++ AST
    # @param source [String] MLC source code
    # @param filename [String, nil] Source filename for error reporting
    # @param runtime_policy [RuntimePolicy, nil] Policy for runtime lowering strategies
    # @return [Backends::Cpp::AST] C++ AST
    def compile(source, filename: nil, runtime_policy: nil)
      # Parse MLC source (use build_project for multi-file with imports)
      ast = parse(source, filename: filename)

      # 2. Transform to SemanticIR (with type_registry)
      source_file_dir = filename ? File.dirname(File.expand_path(filename)) : nil
      transformer = Representations::Semantic::Gen::Pipeline.new(source_file_dir: source_file_dir)
      core_ir, type_registry, function_registry, trait_registry = transform_to_core_with_registry(ast, transformer: transformer)

      # 3. Lower to C++ AST (with shared type_registry and stdlib_scanner)
      stdlib_scanner = MLC::Common::Stdlib::Scanner.new
      cpp_lowerer = Backends::Cpp::Codegen.new(
        type_registry: type_registry,
        function_registry: function_registry,
        trait_registry: trait_registry,
        stdlib_scanner: stdlib_scanner,
        runtime_policy: runtime_policy
      )
      cpp_lowerer.lower(core_ir)
    end

    # Parse MLC source to AST
    # @param source [String] MLC source code
    # @param filename [String, nil] Source filename for error reporting
    # @return [AST::Program] Parsed AST
    def parse(source, filename: nil)
      parser = MLC::Source::Parser::Parser.new(source, filename: filename)
      parser.parse
    rescue StandardError => e
      raise MLC::ParseError, "Parse error: #{e.message}"
    end

    # Transform MLC AST to SemanticIR
    # For backward compatibility, returns just core_ir
    # Use transform_to_core_with_registry if you need the type_registry
    # @param ast [AST::Program] MLC AST
    # @return [SemanticIR::Module] SemanticIR module
    def transform_to_core(ast)
      core_ir, _type_registry = transform_to_core_with_registry(ast)
      core_ir
    end

    # Transform MLC AST to SemanticIR (with TypeRegistry)
    # @param ast [AST::Program] MLC AST
    # @param transformer [Representations::Semantic::Gen::Pipeline] Optional custom transformer
    # @return [Array<SemanticIR::Module, TypeRegistry, FunctionRegistry>]
    def transform_to_core_with_registry(ast, transformer: Representations::Semantic::Gen::Pipeline.new)
      core_ir = transformer.transform(ast)
      [core_ir, transformer.type_registry, transformer.function_registry, transformer.services.trait_registry]
    rescue MLC::CompileError
      raise
    rescue StandardError => e
      origin = e.respond_to?(:origin) ? e.origin : nil
      raise MLC::CompileError.new("Transform error: #{e.message}", origin: origin)
    end

    # Lower SemanticIR to C++ AST
    # @param core_ir [SemanticIR::Module] SemanticIR module
    # @param type_registry [TypeRegistry] Shared type registry from ToCore
    # @param function_registry [FunctionRegistry] Function registry from ToCore
    # @param stdlib_scanner [Common::Stdlib::Scanner] Scanner for automatic stdlib function resolution
    # @param event_bus [EventBus, nil] Optional event bus for diagnostics
    # @param runtime_policy [RuntimePolicy, nil] Policy for choosing lowering strategies
    # @return [Backends::Cpp::AST] C++ AST
    def lower_to_cpp(core_ir, type_registry: nil, function_registry: nil, stdlib_scanner: nil, event_bus: nil, runtime_policy: nil)
      lowerer = Backends::Cpp::Codegen.new(
        type_registry: type_registry,
        function_registry: function_registry,
        stdlib_scanner: stdlib_scanner,
        event_bus: event_bus,
        runtime_policy: runtime_policy
      )
      lowerer.lower(core_ir)
    rescue MLC::CompileError
      raise
    rescue StandardError => e
      origin = e.respond_to?(:origin) ? e.origin : nil
      raise MLC::CompileError.new("Lowering error: #{e.message}", origin: origin)
    end

    # Full pipeline: MLC source -> C++ source
    # @param source [String] MLC source code
    # @param filename [String, nil] Source filename for error reporting
    # @param runtime_policy [RuntimePolicy, nil] Policy for runtime lowering strategies
    # @return [String] Generated C++ source code
    def to_cpp(source, filename: nil, runtime_policy: nil)
      cpp_ast = compile(source, filename: filename, runtime_policy: runtime_policy)
      cpp_ast.to_source
    end

    # Generate header, implementation, and metadata files for a module
    # @param source [String] MLC source code
    # @param filename [String, nil] Source filename for error reporting
    # @param runtime_policy [RuntimePolicy, nil] Policy for runtime lowering strategies
    # @return [Hash] { header: String, implementation: String, metadata: Hash }
    def to_hpp_cpp(source, filename: nil, runtime_policy: nil)
      # Parse and transform to SemanticIR
      ast = parse(source, filename: filename)
      transformer = Representations::Semantic::Gen::Pipeline.new
      core_ir, type_registry, function_registry, trait_registry = transform_to_core_with_registry(ast, transformer: transformer)

      # Create Compiler::Scanner
      stdlib_scanner = MLC::Common::Stdlib::Scanner.new

      # Generate header and implementation
      lowering = Backends::Cpp::Codegen.new(
        type_registry: type_registry,
        function_registry: function_registry,
        trait_registry: trait_registry,
        stdlib_scanner: stdlib_scanner,
        runtime_policy: runtime_policy
      )
      generator = Backends::Cpp::HeaderGenerator.new(lowering)
      cpp_result = generator.generate(core_ir)

      # Generate metadata for module exports
      metadata_gen = Backends::Cpp::MetadataGenerator.new
      metadata = metadata_gen.generate(core_ir)

      # Return header, implementation, and metadata
      cpp_result.merge(metadata: metadata)
    rescue MLC::CompileError
      raise
    rescue StandardError => e
      origin = e.respond_to?(:origin) ? e.origin : nil
      message = "Header generation error: #{e.message}\n#{e.backtrace.join("\n")}"
      raise MLC::CompileError.new(message, origin: origin)
    end

    # Format a CompileError with enhanced diagnostics
    # @param error [CompileError] The compile error to format
    # @param use_color [Boolean] Whether to use ANSI colors
    # @return [String] Formatted error message
    def format_error(error, use_color: true)
      require_relative "diagnostics/error_formatter"
      formatter = Diagnostics::ErrorFormatter.new(use_color: use_color)

      formatter.format_error(
        message: error.message.sub(/^.*?: /, ""), # Remove prefix like "Transform error: "
        origin: error.origin
      )
    end

    # Compile with formatted error output
    # Returns either { success: true, result: cpp_code } or { success: false, error: formatted_error }
    # @param source [String] MLC source code
    # @param filename [String, nil] Source filename
    # @param use_color [Boolean] Whether to use ANSI colors
    # @return [Hash] Result hash with :success, :result/:error keys
    def compile_with_diagnostics(source, filename: nil, use_color: true)
      cpp_code = to_cpp(source, filename: filename)
      { success: true, result: cpp_code }
    rescue MLC::CompileError, MLC::ParseError => e
      formatted = if e.is_a?(MLC::CompileError)
                    format_error(e, use_color: use_color)
                  else
                    require_relative "diagnostics/error_formatter"
                    formatter = Diagnostics::ErrorFormatter.new(use_color: use_color)
                    formatter.format_syntax_error(message: e.message.sub(/^Parse error: /, ""))
                  end
      { success: false, error: formatted }
    end

    # Create a new ErrorCollector for batch error collection
    # @param max_errors [Integer] Maximum number of errors to collect
    # @return [Diagnostics::ErrorCollector]
    def create_error_collector(max_errors: 100)
      require_relative "diagnostics/error_collector"
      Diagnostics::ErrorCollector.new(max_errors: max_errors)
    end

    # Compile project in modular mode: per-module .hpp/.cpp, optional g++ build.
    # @param entry_path [String] Path to entry .mlc file
    # @param out_dir [String] Output directory for generated files
    # @param root_dir [String, nil] Root for resolving imports (default: entry dir)
    # @return [Hash] { cpp_files: [...], hpp_files: [...] }
    def compile_project(entry_path:, out_dir:, root_dir: nil)
      require_relative "modular_compilation/modular_compiler"
      MLC::Common::ModularCompilation::ModularCompiler.new(
        entry_path: entry_path,
        out_dir: out_dir,
        root_dir: root_dir
      ).compile
    end

    # Compile project and build executable with g++.
    # @param binary_name [String] Output binary name (default: "app")
    # @return [Hash] { binary: path, cpp_files: [...], hpp_files: [...] }
    def build_project(entry_path:, out_dir:, root_dir: nil, binary_name: "app")
      require_relative "modular_compilation/modular_compiler"
      MLC::Common::ModularCompilation::ModularCompiler.new(
        entry_path: entry_path,
        out_dir: out_dir,
        root_dir: root_dir,
        binary_name: binary_name
      ).build
    end
  end
end
# rubocop:enable Metrics/ParameterLists
