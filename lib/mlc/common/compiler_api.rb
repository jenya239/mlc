# frozen_string_literal: true

require_relative "errors"
require_relative "../source/parser/lexer"
require_relative "../source/parser/parser"
require_relative "../representations/semantic/semantic_gen"
require_relative "../backends/cpp/codegen"
require_relative "../backends/cpp/header_generator"
require_relative "../backends/cpp/metadata_generator"
require_relative "stdlib/scanner"

module MLC
  class << self
    # Main entry point: Parse Aurora source and return C++ AST
    # @param source [String] Aurora source code
    # @param filename [String, nil] Source filename for error reporting
    # @param runtime_policy [RuntimePolicy, nil] Policy for runtime lowering strategies
    # @return [Backends::Cpp::AST] C++ AST
    def compile(source, filename: nil, runtime_policy: nil)
      # 1. Parse Aurora source
      ast = parse(source, filename: filename)

      # 2. Transform to SemanticIR (with type_registry)
      transformer = Representations::Semantic::Gen::Pipeline.new
      core_ir, type_registry, function_registry = transform_to_core_with_registry(ast, transformer: transformer)

      # 3. Lower to C++ AST (with shared type_registry and stdlib_scanner)
      stdlib_scanner = MLC::Common::Stdlib::Scanner.new
      cpp_lowerer = Backends::Cpp::Codegen.new(
        type_registry: type_registry,
        function_registry: function_registry,
        stdlib_scanner: stdlib_scanner,
        runtime_policy: runtime_policy
      )
      cpp_ast = cpp_lowerer.lower(core_ir)

      cpp_ast
    end

    # Parse Aurora source to AST
    # @param source [String] Aurora source code
    # @param filename [String, nil] Source filename for error reporting
    # @return [AST::Program] Parsed AST
    def parse(source, filename: nil)
      parser = MLC::Source::Parser::Parser.new(source, filename: filename)
      parser.parse
    rescue => e
      raise ParseError, "Parse error: #{e.message}"
    end

    # Transform Aurora AST to SemanticIR
    # For backward compatibility, returns just core_ir
    # Use transform_to_core_with_registry if you need the type_registry
    # @param ast [AST::Program] Aurora AST
    # @return [SemanticIR::Module] SemanticIR module
    def transform_to_core(ast)
      core_ir, _type_registry = transform_to_core_with_registry(ast)
      core_ir
    end

    # Transform Aurora AST to SemanticIR (with TypeRegistry)
    # @param ast [AST::Program] Aurora AST
    # @param transformer [Representations::Semantic::Gen::Pipeline] Optional custom transformer
    # @return [Array<SemanticIR::Module, TypeRegistry, FunctionRegistry>]
    def transform_to_core_with_registry(ast, transformer: Representations::Semantic::Gen::Pipeline.new)
      core_ir = transformer.transform(ast)
      [core_ir, transformer.type_registry, transformer.function_registry]
    rescue CompileError
      raise
    rescue => e
      origin = e.respond_to?(:origin) ? e.origin : nil
      raise CompileError.new("Transform error: #{e.message}", origin: origin)
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
    rescue CompileError
      raise
    rescue => e
      origin = e.respond_to?(:origin) ? e.origin : nil
      raise CompileError.new("Lowering error: #{e.message}", origin: origin)
    end

    # Full pipeline: Aurora source -> C++ source
    # @param source [String] Aurora source code
    # @param filename [String, nil] Source filename for error reporting
    # @param runtime_policy [RuntimePolicy, nil] Policy for runtime lowering strategies
    # @return [String] Generated C++ source code
    def to_cpp(source, filename: nil, runtime_policy: nil)
      cpp_ast = compile(source, filename: filename, runtime_policy: runtime_policy)
      cpp_ast.to_source
    end

    # Generate header, implementation, and metadata files for a module
    # @param source [String] Aurora source code
    # @param filename [String, nil] Source filename for error reporting
    # @param runtime_policy [RuntimePolicy, nil] Policy for runtime lowering strategies
    # @return [Hash] { header: String, implementation: String, metadata: Hash }
    def to_hpp_cpp(source, filename: nil, runtime_policy: nil)
      # Parse and transform to SemanticIR
      ast = parse(source, filename: filename)
      transformer = Representations::Semantic::Gen::Pipeline.new
      core_ir, type_registry, function_registry = transform_to_core_with_registry(ast, transformer: transformer)

      # Create Compiler::Scanner
      stdlib_scanner = MLC::Common::Stdlib::Scanner.new

      # Generate header and implementation
      lowering = Backends::Cpp::Codegen.new(
        type_registry: type_registry,
        function_registry: function_registry,
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
    rescue CompileError
      raise
    rescue => e
      origin = e.respond_to?(:origin) ? e.origin : nil
      message = "Header generation error: #{e.message}\n#{e.backtrace.join("\n")}"
      raise CompileError.new(message, origin: origin)
    end
  end
end
