# frozen_string_literal: true

require_relative "../../backend/codegen"
require_relative "bootstrap"

module MLC
  module Backends
    module Cpp
      # LegacyAdapter - Strangler Fig Pattern adapter
      #
      # Purpose: Provides drop-in replacement for Backend::CodeGen while:
      # 1. Using new architecture (Bootstrap, Container, Context, Rules) for lowering
      # 2. Delegating high-level operations (lower_module, lower_function) to legacy
      # 3. Enabling gradual migration of individual components
      #
      # Strategy:
      # - Phase 1 (current): Delegate all lowering to Backend::CodeGen
      # - Phase 2 (future): Replace expression/statement lowering with new rules
      # - Phase 3 (future): Replace function/module lowering with new implementation
      class LegacyAdapter
        attr_reader :legacy_backend, :container, :context

        def initialize(type_registry:, function_registry: nil, stdlib_scanner: nil, rule_engine: nil, event_bus: nil, runtime_policy: nil)
          # Initialize legacy backend with all original parameters
          @legacy_backend = Backend::CodeGen.new(
            type_registry: type_registry,
            function_registry: function_registry,
            stdlib_scanner: stdlib_scanner,
            rule_engine: rule_engine,
            event_bus: event_bus,
            runtime_policy: runtime_policy
          )

          # Initialize new architecture (for future migration)
          # Currently not used, but ready for gradual replacement
          backend = Bootstrap.create_backend(
            type_registry: type_registry,
            function_registry: function_registry,
            runtime_policy: runtime_policy
          )
          @container = backend[:container]
          @context = backend[:context]
        end

        # Main entry point - delegates to legacy Backend::CodeGen
        def lower(core_ir)
          @legacy_backend.lower(core_ir)
        end

        # Expose legacy backend attributes for compatibility
        def type_registry
          @legacy_backend.type_registry
        end

        def function_registry
          @legacy_backend.function_registry
        end

        def rule_engine
          @legacy_backend.rule_engine
        end

        def event_bus
          @legacy_backend.event_bus
        end

        def runtime_policy
          @legacy_backend.runtime_policy
        end

        def type_map
          @legacy_backend.type_map
        end
      end
    end
  end
end
