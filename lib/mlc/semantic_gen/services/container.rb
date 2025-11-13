# frozen_string_literal: true

require_relative 'scope/var_type_registry'
require_relative 'checkers/type_checker'
require_relative 'scope/scope_context_service'
require_relative 'utils/type_unification_service'
require_relative 'inference/type_inference_service'
require_relative 'registration/sum_type_constructor_service'
require_relative 'scope/module_context_service'
require_relative '../../tools/compiler/stdlib/signature_registry'
require_relative 'imports/module_resolver'
require_relative 'imports/module_path_resolver'
require_relative 'checkers/ast_type_checker'
require_relative 'builders/ir_builder'
require_relative 'literal_processor'
require_relative 'type_checker'
require_relative 'builders/record_literal_builder'
require_relative 'builders/array_literal_builder'
require_relative 'features/loop_service'
require_relative 'features/match_service'
require_relative 'features/lambda_service'
require_relative 'features/list_comprehension_service'
require_relative 'features/index_access_service'
require_relative 'type_registration_service'
require_relative 'builders/type_builder'
require_relative 'type_declaration_service'
require_relative 'imports/import_service'
require_relative '../../type_system/match_analyzer'
require_relative '../../type_system/type_constraint_solver'
require_relative '../../type_system/generic_call_resolver'
require_relative 'builders/ast_factory'
require_relative '../../tools/compiler/metadata_loader_service'

module MLC
  module SemanticGen
    module Services
      # ServicesContainer - Container for all services
      # Services are ONLY state holders, no conversion logic
      class Container
        attr_reader :module_resolver, :module_path_resolver, :function_registry, :type_registry,
                    :ast_type_checker, :ir_builder, :literal_processor,
                    :ast_factory, :var_type_registry,
                    :type_checker, :type_decl_table,
                    :semantic_ir_classifier, :record_literal_builder, :array_literal_builder,
                    :scope_context, :loop_service, :type_unification_service,
                    :match_analyzer, :match_service,
                    :lambda_service, :list_comprehension_service, :index_access_service,
                    :module_context_service, :sum_type_constructor_service,
                    :type_registration_service, :type_builder, :type_declaration_service,
                    :stdlib_registry, :import_service, :metadata_loader,
                    :sum_type_constructors, :type_inference_service

        def initialize(function_registry:, type_registry:)
          @module_resolver = ModuleResolver.new
          @module_path_resolver = ModulePathResolver.new
          @function_registry = function_registry
          @type_registry = type_registry
          @ast_type_checker = ASTTypeChecker.new
          @var_type_registry = MLC::Services::VarTypeRegistry.new
          @ir_builder = IRBuilder.new
          @literal_processor = LiteralProcessor.new(ir_builder: @ir_builder)
          @type_decl_table = {}
          @sum_type_constructors = {}
          @type_checker = MLC::Services::TypeChecker.new(
            function_registry: @function_registry,
            type_decl_table: @type_decl_table,
            current_node_proc: -> { nil },
            var_type_registry: @var_type_registry,
            type_registry: @type_registry
          )
          @type_builder = TypeBuilder.new(
            ir_builder: @ir_builder,
            type_checker: @type_checker
          )
          @scope_context = MLC::Services::TransformationContext.new(
            var_type_registry: @var_type_registry
          )
          @semantic_ir_classifier = MLC::Services::SemanticIRTypeChecker.new
          @record_literal_builder = RecordLiteralBuilder.new(
            ir_builder: @ir_builder,
            type_registry: @type_registry,
            type_checker: @type_checker
          )
          @array_literal_builder = ArrayLiteralBuilder.new(
            ir_builder: @ir_builder,
            type_checker: @type_checker
          )
          @ast_factory = ASTFactory.new
          @loop_service = LoopService.new(
            ir_builder: @ir_builder,
            type_checker: @type_checker,
            ast_factory: @ast_factory,
            ast_type_checker: @ast_type_checker,
            scope_context: @scope_context,
            variable_types: @var_type_registry
          )
          @module_context_service = MLC::Services::ModuleContextService.new(
            function_registry: @function_registry
          )
          @sum_type_constructor_service = MLC::Services::SumTypeConstructorService.new(
            sum_type_constructors: @sum_type_constructors,
            function_registry: @function_registry,
            type_decl_table: @type_decl_table,
            type_checker: @type_checker
          )
          @type_unification_service = MLC::Services::TypeUnificationService.new(
            type_checker: @type_checker,
            sum_type_constructors: @sum_type_constructors
          )
          @type_inference_service = MLC::Services::TypeInferenceService.new(
            var_type_registry: @var_type_registry,
            type_registry: @type_registry,
            function_registry: @function_registry,
            type_decl_table: @type_decl_table,
            generic_call_resolver: nil,
            type_checker: @type_checker,
            scope_context: @scope_context
          )
          type_constraint_solver = MLC::TypeSystem::TypeConstraintSolver.new(
            infer_type_arguments: @type_inference_service.method(:infer_type_arguments),
            substitute_type: @type_inference_service.method(:substitute_type),
            ensure_compatible_type: @type_checker.method(:ensure_compatible_type),
            type_error: ->(message) { @type_checker.type_error(message) },
            assign_expression_type: @type_checker.method(:assign_expression_type)
          )
          @generic_call_resolver = MLC::TypeSystem::GenericCallResolver.new(
            constraint_solver: type_constraint_solver
          )
          @type_inference_service.generic_call_resolver = @generic_call_resolver
          @match_analyzer = MLC::TypeSystem::MatchAnalyzer.new(
            ensure_compatible_type: @type_checker.method(:ensure_compatible_type),
            type_registry: @type_registry,
            check_exhaustiveness: true
          )
          @match_service = MatchService.new(
            ir_builder: @ir_builder,
            type_checker: @type_checker,
            var_type_registry: @var_type_registry,
            ast_factory: @ast_factory,
            ast_type_checker: @ast_type_checker,
            type_unification_service: @type_unification_service,
            match_analyzer: @match_analyzer
          )
          @lambda_service = LambdaService.new(
            ir_builder: @ir_builder,
            type_checker: @type_checker,
            var_type_registry: @var_type_registry,
            scope_context: @scope_context,
            type_builder: @type_builder
          )
          @list_comprehension_service = ListComprehensionService.new(
            ir_builder: @ir_builder,
            loop_service: @loop_service,
            var_type_registry: @var_type_registry
          )
          @index_access_service = IndexAccessService.new(
            type_checker: @type_checker
          )
          @type_registration_service = TypeRegistrationService.new(
            type_registry: @type_registry,
            type_decl_table: @type_decl_table,
            sum_type_constructor_service: @sum_type_constructor_service
          )
          @type_declaration_service = TypeDeclarationService.new(
            type_checker: @type_checker,
            type_builder: @type_builder,
            type_registration_service: @type_registration_service,
            module_context_service: @module_context_service
          )
          @stdlib_registry = MLC::Compiler::StdlibSignatureRegistry.new
          @metadata_loader = MLC::Compiler::MetadataLoaderService.new(
            function_registry: @function_registry,
            type_registry: @type_registry
          )
          @import_service = ImportService.new(
            stdlib_registry: @stdlib_registry,
            module_resolver: @module_resolver,
            module_path_resolver: @module_path_resolver,
            type_builder: @type_builder,
            type_declaration_service: @type_declaration_service,
            type_registration_service: @type_registration_service,
            type_checker: @type_checker,
            metadata_loader: @metadata_loader
          )
        end
      end
    end
  end
end
