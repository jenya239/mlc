# frozen_string_literal: true

require_relative "infrastructure/event_bus"
require_relative "rules/rule_engine"
require_relative "diagnostics/event_logger"
require_relative "semantic_gen"
require_relative "backend/codegen"
require_relative "backends/cpp/legacy_adapter"
require_relative "compiler/stdlib/scanner"

module MLC
  class Application
    attr_reader :event_bus, :rule_engine

    def initialize(event_bus: nil, rule_engine: nil, logger: Diagnostics::EventLogger, subscribers: [], configure_rules: nil)
      @event_bus = event_bus || Infrastructure::EventBus.new
      logger.attach(@event_bus) if logger
      Array(subscribers).each { |subscriber| subscriber.call(@event_bus) }
      @rule_engine = rule_engine || build_default_rule_engine
      configure_rules&.call(@rule_engine)
    end

    def build_to_core
      SemanticGen::Pipeline.new(rule_engine: @rule_engine)
    end

    def build_cpp_lowering(type_registry:, function_registry: nil, stdlib_scanner: Compiler::StdlibScanner.new, runtime_policy: nil)
      # Environment variable to switch between legacy and new backend architecture
      # MLC_CPP_BACKEND=v2 enables Strangler Fig adapter (gradual migration path)
      # Default: legacy Backend::CodeGen
      backend_version = ENV.fetch("MLC_CPP_BACKEND", "legacy")

      case backend_version
      when "v2"
        # New architecture: LegacyAdapter delegates to Backend::CodeGen
        # but initializes new Backends::Cpp infrastructure for gradual migration
        Backends::Cpp::LegacyAdapter.new(
          type_registry: type_registry,
          function_registry: function_registry,
          stdlib_scanner: stdlib_scanner,
          rule_engine: @rule_engine,
          event_bus: @event_bus,
          runtime_policy: runtime_policy
        )
      else
        # Legacy architecture: Direct Backend::CodeGen usage
        Backend::CodeGen.new(
          type_registry: type_registry,
          function_registry: function_registry,
          stdlib_scanner: stdlib_scanner,
          rule_engine: @rule_engine,
          event_bus: @event_bus,
          runtime_policy: runtime_policy
        )
      end
    end

    private

    def build_default_rule_engine
      MLC::Rules::RuleEngine.new
    end
  end
end
