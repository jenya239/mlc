# frozen_string_literal: true

require_relative '../../services/var_type_registry'
require_relative '../../services/type_checker'
require_relative '../../services/scope_context_service'
require_relative 'module_resolver'
require_relative 'ast_type_checker'
require_relative 'ir_builder'
require_relative 'literal_processor'
require_relative 'expression_type_resolver'
require_relative 'identifier_type_resolver'
require_relative 'high_ir_classifier'
require_relative 'record_literal_builder'
require_relative 'array_literal_builder'
require_relative 'loop_service'
require_relative 'ast_factory'

module MLC
  module IRGenV2
    module Services
      # ServicesContainer - Container for all services
      # Services are ONLY state holders, no conversion logic
      class Container
        attr_reader :module_resolver, :function_registry, :type_registry,
                    :ast_type_checker, :ir_builder, :literal_processor,
                    :ast_factory, :var_type_registry, :identifier_type_resolver,
                    :type_checker, :type_decl_table, :expression_type_resolver,
                    :high_ir_classifier, :record_literal_builder, :array_literal_builder,
                    :scope_context, :loop_service

        def initialize(function_registry:, type_registry:)
          @module_resolver = ModuleResolver.new
          @function_registry = function_registry
          @type_registry = type_registry
          @ast_type_checker = ASTTypeChecker.new
          @var_type_registry = MLC::Services::VarTypeRegistry.new
          @ir_builder = IRBuilder.new
          @literal_processor = LiteralProcessor.new(ir_builder: @ir_builder)
          @type_decl_table = {}
          @type_checker = MLC::Services::TypeChecker.new(
            function_registry: @function_registry,
            type_decl_table: @type_decl_table,
            current_node_proc: -> { nil }
          )
          @scope_context = MLC::Services::TransformationContext.new(
            var_type_registry: @var_type_registry
          )
          @expression_type_resolver = ExpressionTypeResolver.new(
            type_checker: @type_checker,
            ir_builder: @ir_builder
          )
          @identifier_type_resolver = IdentifierTypeResolver.new(
            function_registry: @function_registry,
            ir_builder: @ir_builder,
            var_type_registry: @var_type_registry
          )
          @high_ir_classifier = HighIRClassifier.new
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
            scope_context: @scope_context,
            variable_types: @var_type_registry
          )
        end
      end
    end
  end
end
