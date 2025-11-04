# frozen_string_literal: true

module MLC
  module Services
    # IRGenContainer - Service initialization and dependency injection for IRGen
    # Phase 20: Extracted from IRGen
    #
    # Responsibilities:
    # - Initialize all IRGen services with proper dependency wiring
    # - Construct type system components (constraint solver, analyzers)
    # - Return container struct with all initialized components
    # - Fix scope_context_service initialization bug
    #
    # Dependencies: None (pure container/builder)
    #
    # This container follows the Dependency Injection pattern, centralizing
    # all service initialization logic that was previously cluttering IRGen.initialize
    class IRGenContainer
      # Container struct holding all IRGen components
      # Provides read-only access to initialized services and state
      Container = Struct.new(
        :type_registry,
        :function_registry,
        :rule_engine,
        :type_constraint_solver,
        :generic_call_resolver,
        :match_analyzer,
        :effect_analyzer,
        :stdlib_resolver,
        :stdlib_registry,
        :event_bus,
        :type_decl_table,
        :sum_type_constructors,
        :var_type_registry,
        :context,
        :scope_context_service,  # Alias for context (backward compatibility)
        :module_context_service,
        :type_checker_service,
        :type_inference_service,
        :record_builder_service,
        :purity_analyzer,
        :type_resolution_service,
        :function_registration_service,
        :sum_type_constructor_service,
        :type_unification_service,
        :function_lookup_service,
        :metadata_loader_service,
        :temp_counter,
        :loop_depth,
        :current_import_aliases,
        keyword_init: true
      )

      # Build all IRGen components with dependency injection
      #
      # @param rule_engine [Rules::RuleEngine, nil] Optional rule engine (uses default if nil)
      # @param generic_call_resolver [TypeSystem::GenericCallResolver, nil] Optional generic call resolver
      # @param type_constraint_solver [TypeSystem::TypeConstraintSolver, nil] Optional type constraint solver
      # @param match_analyzer [TypeSystem::MatchAnalyzer, nil] Optional match analyzer
      # @param effect_analyzer [TypeSystem::EffectAnalyzer, nil] Optional effect analyzer
      # @param event_bus [EventBus, nil] Optional event bus
      # @param transformer [IRGen] The transformer instance (for services that need it)
      # @return [Container] Container with all initialized components
      def self.build(
        rule_engine: nil,
        generic_call_resolver: nil,
        type_constraint_solver: nil,
        match_analyzer: nil,
        effect_analyzer: nil,
        event_bus: nil,
        transformer: nil
      )
        # Phase 1: Core registries and infrastructure
        type_registry = TypeRegistry.new
        function_registry = FunctionRegistry.new
        rule_engine ||= Services::RuleEngineBuilder.build_default
        event_bus ||= MLC::EventBus.new

        stdlib_resolver = StdlibResolver.new
        stdlib_registry = StdlibSignatureRegistry.new(scanner: stdlib_resolver.scanner)

        # Phase 2: State tables and counters
        type_decl_table = {}
        sum_type_constructors = {}
        var_type_registry = Services::VarTypeRegistry.new
        temp_counter = 0
        loop_depth = 0
        current_import_aliases = nil

        # Phase 3: Transformation context (manages scoped state)
        context = Services::TransformationContext.new(var_type_registry: var_type_registry)
        # Phase 20: Fix bug - set scope_context_service as alias to context
        scope_context_service = context

        # Phase 4: Module context service
        module_context_service = Services::ModuleContextService.new(
          function_registry: function_registry
        )

        # Phase 5: Core services (first tier - minimal dependencies)
        type_checker_service = Services::TypeChecker.new(
          function_registry: function_registry,
          type_decl_table: type_decl_table,
          event_bus: event_bus,
          current_node_proc: -> { context.current_node }
        )

        purity_analyzer = Services::PurityAnalyzer.new

        # Phase 6: Services with dependencies on core services
        type_inference_service = Services::TypeInferenceService.new(
          var_type_registry: var_type_registry,
          type_registry: type_registry,
          function_registry: function_registry,
          type_decl_table: type_decl_table,
          generic_call_resolver: nil,  # TypeSystem::GenericCallResolver created below, passed after instantiation
          type_checker: type_checker_service,
          transformer: transformer,
          scope_context: scope_context_service
        )

        type_resolution_service = Services::TypeResolutionService.new(
          function_registry: function_registry,
          type_registry: type_registry,
          type_checker: type_checker_service
        )

        function_registration_service = Services::FunctionRegistrationService.new(
          function_registry: function_registry,
          module_context_service: module_context_service
        )

        sum_type_constructor_service = Services::SumTypeConstructorService.new(
          sum_type_constructors: sum_type_constructors,
          function_registry: function_registry,
          type_decl_table: type_decl_table,
          type_checker: type_checker_service
        )

        type_unification_service = Services::TypeUnificationService.new(
          type_checker: type_checker_service,
          sum_type_constructors: sum_type_constructors
        )

        record_builder_service = Services::RecordBuilderService.new(
          transformer,
          type_unification_service: type_unification_service
        )

        function_lookup_service = Services::FunctionLookupService.new(
          function_registry: function_registry,
          sum_type_constructors: sum_type_constructors,
          type_checker: type_checker_service,
          type_inference: type_inference_service
        )

        metadata_loader_service = Services::MetadataLoaderService.new(
          function_registry: function_registry,
          type_registry: type_registry
        )

        # Phase 7: Type system components (lazily initialized if not provided)
        type_constraint_solver ||= TypeSystem::TypeConstraintSolver.new(
          infer_type_arguments: type_inference_service.method(:infer_type_arguments),
          substitute_type: type_inference_service.method(:substitute_type),
          ensure_compatible_type: type_checker_service.method(:ensure_compatible_type),
          type_error: ->(message) { type_checker_service.type_error(message) }
        )

        generic_call_resolver ||= TypeSystem::GenericCallResolver.new(
          constraint_solver: type_constraint_solver
        )

        # Inject generic_call_resolver into TypeInferenceService
        type_inference_service.generic_call_resolver = generic_call_resolver

        match_analyzer ||= TypeSystem::MatchAnalyzer.new(
          ensure_compatible_type: type_checker_service.method(:ensure_compatible_type),
          type_registry: type_registry,
          check_exhaustiveness: true
        )

        effect_analyzer ||= TypeSystem::EffectAnalyzer.new(
          pure_expression: purity_analyzer.method(:is_pure_expression),
          non_literal_type: purity_analyzer.method(:non_literal_type?)
        )

        # Phase 8: Validate rule engine configuration
        Services::RuleEngineBuilder.ensure_required_rules!(rule_engine)

        # Return container with all components
        Container.new(
          type_registry: type_registry,
          function_registry: function_registry,
          rule_engine: rule_engine,
          type_constraint_solver: type_constraint_solver,
          generic_call_resolver: generic_call_resolver,
          match_analyzer: match_analyzer,
          effect_analyzer: effect_analyzer,
          stdlib_resolver: stdlib_resolver,
          stdlib_registry: stdlib_registry,
          event_bus: event_bus,
          type_decl_table: type_decl_table,
          sum_type_constructors: sum_type_constructors,
          var_type_registry: var_type_registry,
          context: context,
          scope_context_service: scope_context_service,
          module_context_service: module_context_service,
          type_checker_service: type_checker_service,
          type_inference_service: type_inference_service,
          record_builder_service: record_builder_service,
          purity_analyzer: purity_analyzer,
          type_resolution_service: type_resolution_service,
          function_registration_service: function_registration_service,
          sum_type_constructor_service: sum_type_constructor_service,
          type_unification_service: type_unification_service,
          function_lookup_service: function_lookup_service,
          metadata_loader_service: metadata_loader_service,
          temp_counter: temp_counter,
          loop_depth: loop_depth,
          current_import_aliases: current_import_aliases
        )
      end
    end
  end
end
