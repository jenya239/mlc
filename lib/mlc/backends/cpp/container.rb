# frozen_string_literal: true

# RuleEngine, CppAstFactory, RuntimePolicy autoloaded by Zeitwerk
# - RuleEngine used only via attr_accessor
# - CppAstFactory instantiated in lazy methods (cpp_ast_factory, create_service)
# - RuntimePolicy instantiated in initialize, const resolved at runtime via const_missing

module MLC
  module Backends
    module Cpp
      # Dependency injection container for backend services
      class Container
        attr_reader :type_registry, :function_registry, :type_map, :runtime_policy, :stdlib_scanner, :event_bus
        attr_accessor :rule_engine, :in_generic_function, :user_functions, :declared_variables

        def initialize(type_registry:, function_registry:, runtime_policy: nil, stdlib_scanner: nil, event_bus: nil)
          @type_registry = type_registry
          @function_registry = function_registry
          @stdlib_scanner = stdlib_scanner
          @event_bus = event_bus
          @type_map = build_type_map
          @runtime_policy = runtime_policy || RuntimePolicy.new
          @services = {}
          @rule_engine = nil # Initialized after Context creation
          @in_generic_function = false # Mutable state for generic function lowering
          @user_functions = Set.new # Track user-defined functions
          @declared_variables = Set.new # Track declared variables for rebinding detection
          @temp_counter = 0 # Counter for generating unique temp variable names
        end

        # Generate unique temporary variable name
        def generate_temp_name
          name = "__tmp_#{@temp_counter}"
          @temp_counter += 1
          name
        end

        # Reset declared variables (call when entering new function scope)
        def reset_declared_variables
          @declared_variables = Set.new
        end

        # Check if variable was already declared (for rebinding)
        def variable_declared?(name)
          @declared_variables.include?(name)
        end

        # Mark variable as declared
        def declare_variable(name)
          @declared_variables.add(name)
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
          @services[:semantic_ir_checker] ||= MLC::Representations::Semantic::Gen::Services::SemanticIRTypeChecker.new
        end

        private

        # Primitive type mapping
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
            MLC::Representations::Semantic::Gen::Services::SemanticIRTypeChecker.new
          else
            raise "Unknown service: #{service_name}"
          end
        end
      end
    end
  end
end
