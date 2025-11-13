# frozen_string_literal: true

require_relative "services/rule_engine"
require_relative "services/builders/cpp_ast_factory"
require_relative "../../semantic_gen/services/type_checker"
require_relative "runtime_policy"

module MLC
  module Backends
    module Cpp
      # Dependency injection container for backend services
      class Container
        attr_reader :type_registry, :function_registry, :type_map, :runtime_policy, :stdlib_scanner, :event_bus
        attr_accessor :rule_engine, :in_generic_function, :user_functions

        def initialize(type_registry:, function_registry:, runtime_policy: nil, stdlib_scanner: nil, event_bus: nil)
          @type_registry = type_registry
          @function_registry = function_registry
          @stdlib_scanner = stdlib_scanner
          @event_bus = event_bus
          @type_map = build_type_map
          @runtime_policy = runtime_policy || MLC::Backend::RuntimePolicy.new
          @services = {}
          @rule_engine = nil # Initialized after Context creation
          @in_generic_function = false # Mutable state for generic function lowering
          @user_functions = Set.new # Track user-defined functions
        end

        # Lazy service initialization
        def resolve(service_name)
          @services[service_name] ||= create_service(service_name)
        end

        # Factory for CppAst::Nodes construction
        def cpp_ast_factory
          @services[:cpp_ast_factory] ||= Services::Builders::CppAstFactory.new
        end

        # Type checker for SemanticIR nodes
        def semantic_ir_checker
          @services[:semantic_ir_checker] ||= MLC::Services::SemanticIRTypeChecker.new
        end

        private

        # Primitive type mapping (migrated from Backend::CodeGen)
        def build_type_map
          {
            "i8" => "int8_t",
            "u8" => "uint8_t",
            "i16" => "int16_t",
            "u16" => "uint16_t",
            "i32" => "int",
            "u32" => "uint32_t",
            "i64" => "int64_t",
            "u64" => "uint64_t",
            "usize" => "size_t",
            "f32" => "float",
            "f64" => "double",
            "bool" => "bool",
            "void" => "void",
            "str" => "mlc::String",
            "string" => "mlc::String",
            "regex" => "mlc::Regex"
          }.freeze
        end

        def create_service(service_name)
          # Services will be registered here as they are implemented
          case service_name
          when :cpp_ast_factory
            Services::Builders::CppAstFactory.new
          when :semantic_ir_checker
            MLC::Services::SemanticIRTypeChecker.new
          else
            raise "Unknown service: #{service_name}"
          end
        end
      end
    end
  end
end
