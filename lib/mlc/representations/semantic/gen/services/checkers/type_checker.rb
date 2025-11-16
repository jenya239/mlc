# frozen_string_literal: true

require_relative "../../../nodes/builder"

module MLC
  module Representations
    module Semantic
      module Gen
        module Services
          # TypeChecker - Type checking and validation service
          # Phase 4: Extracted from the legacy transformer and TypeInference modules
          #
          # Responsibilities:
          # - Type validation (ensure_compatible, ensure_boolean, ensure_numeric)
          # - Type name resolution and normalization
          # - Error reporting
          # - Function call validation
          #
          # Dependencies (injected):
          # - function_registry: FunctionRegistry
          # - event_bus: EventBus (optional)
          # - current_node: Proc (for error context)
          class TypeChecker
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

            NUMERIC_PRIMITIVES = %w[i32 f32 i64 f64 u32 u64].freeze

            BUILTIN_CONSTRAINTS = {
              "Numeric" => %w[i32 f32 i64 f64 u32 u64]
            }.freeze

            def initialize(function_registry:, type_decl_table: nil, event_bus: nil, current_node_proc: nil, var_type_registry: nil, type_registry: nil)
              @function_registry = function_registry
              @type_decl_table = type_decl_table || {}
              @event_bus = event_bus
              @current_node_proc = current_node_proc
              @var_type_registry = var_type_registry
              @type_registry = type_registry
            end

            # Type name resolution
            def type_name(type)
              type&.name
            end

            def normalized_type_name(name)
              case name
              when "str"
                "string"
              else
                name
              end
            end

            def describe_type(type)
              normalized_type_name(type_name(type)) || "unknown"
            end

            # Error reporting
            def type_error(message, node: nil, origin: nil)
              origin ||= node&.origin
              origin ||= @current_node_proc&.call&.origin
              raise MLC::CompileError.new(message, origin: origin)
            end

            def ensure_type!(type, message, node: nil)
              type_error(message, node: node) unless type
            end

            # Type validation
            def ensure_compatible_type(actual, expected, context, node: nil)
              ensure_type!(actual, "#{context} has unknown type", node: node)
              ensure_type!(expected, "#{context} has unspecified expected type", node: node)

              actual_name = normalized_type_name(type_name(actual))
              expected_name = normalized_type_name(type_name(expected))

              return if expected_name.nil? || expected_name.empty?
              return if expected_name == "auto"
              return if generic_type_name?(expected_name)
              return if actual_name == "auto"
              return if expected.is_a?(SemanticIR::TypeVariable)
              return if actual_name == expected_name
              return if unit_like?(actual_name, actual) && unit_like?(expected_name, expected)

              @event_bus&.publish(
                :type_mismatch,
                context: context,
                actual: actual_name,
                expected: expected_name,
                origin: node&.origin || @current_node_proc&.call&.origin
              )

              type_error("#{context} expected #{expected_name}, got #{actual_name}", node: node)
            end

            def ensure_boolean_type(type, context, node: nil)
              name = normalized_type_name(type_name(type))
              return if generic_type_name?(name)
              type_error("#{context} must be bool, got #{describe_type(type)}", node: node) unless name == "bool"
            end

            def ensure_numeric_type(type, context, node: nil)
              name = normalized_type_name(type_name(type))
              return if generic_type_name?(name)
              type_error("#{context} must be numeric, got #{describe_type(type)}", node: node) unless numeric_type?(type)
            end

            # Type predicates
            def numeric_type?(type)
              # TypeVariable is assumed to be numeric-compatible
              return true if type.is_a?(SemanticIR::TypeVariable)

              type_str = normalized_type_name(type_name(type))
              NUMERIC_PRIMITIVES.include?(type_str)
            end

            def generic_type_name?(name)
              return false unless name.is_a?(String)
              name.empty? || name[0]&.match?(/[A-Z]/)
            end

            def unit_like?(name, type)
              return true if %w[unit void].include?(name)
              type.is_a?(SemanticIR::UnitType)
            end

            # IO functions
            def io_return_type(name)
              case IO_RETURN_TYPES[name]
              when "i32"
                SemanticIR::Builder.primitive_type("i32")
              when "string"
                SemanticIR::Builder.primitive_type("string")
              when :array_of_string
                SemanticIR::ArrayType.new(element_type: SemanticIR::Builder.primitive_type("string"))
              else
                SemanticIR::Builder.primitive_type("i32")
              end
            end

            # Function validation
            def validate_function_call(info, args, name)
              expected = info.param_types || []
              return if expected.empty?

              if expected.length != args.length
                type_error("Function '#{name}' expects #{expected.length} argument(s), got #{args.length}")
              end

              expected.each_with_index do |type, index|
                arg_expr = args[index]
                ensure_compatible_type(arg_expr.type, type, "argument #{index + 1} of '#{name}'")
                assign_expression_type(arg_expr, type)
              end
            end

            def ensure_argument_count(member, args, expected)
              return if args.length == expected

              type_error("Method '#{member}' expects #{expected} argument(s), got #{args.length}")
            end

            # Module member lookup
            def module_member_info(container_name, member_name)
              return unless container_name && member_name

              qualified = [container_name, member_name].join(".")
              entry = @function_registry.fetch_entry(qualified)
              entry&.info
            end

            def assign_expression_type(expr, type, update_registry: true)
              return unless expr && type
              set_expression_type(expr, type)
              propagate_literal_types(expr, type)

              if update_registry && expr.is_a?(MLC::SemanticIR::VarExpr) && @var_type_registry
                @var_type_registry.update_type(expr.name, type)
                if (initializer = @var_type_registry.initializer(expr.name))
                  assign_expression_type(initializer, type, update_registry: false)
                end
              end
            end

            def set_expression_type(expr, type)
              if expr.respond_to?(:type=)
                expr.type = type
              else
                expr.instance_variable_set(:@type, type)
              end
            end

            def propagate_literal_types(expr, type)
              case expr
              when MLC::SemanticIR::ArrayLiteralExpr
                return unless type.is_a?(MLC::SemanticIR::ArrayType)
                expr.elements.each do |element|
                  assign_expression_type(element, type.element_type, update_registry: false)
                end
              when MLC::SemanticIR::RecordExpr
                field_map = record_field_type_map(type)
                return unless field_map
                expr.fields.each do |field_name, field_expr|
                  assign_expression_type(field_expr, field_map[field_name], update_registry: false)
                end
              end
            end

            # Type constraint validation
            def validate_constraint_name(name)
              return if name.nil? || name.empty?
              return if BUILTIN_CONSTRAINTS.key?(name)

              type_error("Unknown constraint '#{name}'")
            end

            def type_satisfies_constraint?(constraint, type_name)
              allowed = BUILTIN_CONSTRAINTS[constraint]
              allowed && allowed.include?(type_name)
            end

            def extract_actual_type_name(type_node)
              case type_node
              when MLC::Source::AST::PrimType
                name = type_node.name
                return nil if name.nil?
                return nil if name[0]&.match?(/[A-Z]/)
                name
              else
                nil
              end
            end

            def record_field_type_map(type)
              case type
              when MLC::SemanticIR::RecordType
                type.fields.each_with_object({}) { |field, memo| memo[field[:name].to_s] = field[:type] }
              when MLC::SemanticIR::GenericType
                resolve_generic_record_field_map(type)
              else
                nil
              end
            end

            def resolve_generic_record_field_map(type)
              base_name = type.base_type&.name
              return nil unless base_name && @type_registry

              info = @type_registry.lookup(base_name)
              return nil unless info&.fields

              substitution = build_generic_substitution(info, type)

              info.fields.each_with_object({}) do |field, memo|
                memo[field[:name].to_s] = substitute_type_variables(field[:type], substitution)
              end
            end

            def build_generic_substitution(type_info, generic_type)
              params = type_info.ast_node&.type_params || []
              substitution = {}

              params.each_with_index do |param, index|
                arg = generic_type.type_args[index]
                substitution[param.name] = arg if arg
              end

              substitution
            end

            def substitute_type_variables(type, substitution)
              case type
              when MLC::SemanticIR::TypeVariable
                substitution[type.name] || type
              when MLC::SemanticIR::ArrayType
                new_element = substitute_type_variables(type.element_type, substitution)
                new_element == type.element_type ? type : MLC::SemanticIR::Builder.array_type(new_element)
              when MLC::SemanticIR::RecordType
                new_fields = type.fields.map do |field|
                  { name: field[:name], type: substitute_type_variables(field[:type], substitution) }
                end
                MLC::SemanticIR::Builder.record_type(type.name, new_fields)
              else
                type
              end
            end

            def normalize_type_params(params)
              params.map do |tp|
                name = tp.respond_to?(:name) ? tp.name : tp
                constraint = tp.respond_to?(:constraint) ? tp.constraint : nil
                validate_constraint_name(constraint)
                SemanticIR::TypeParam.new(name: name, constraint: constraint)
              end
            end

            def validate_type_constraints(base_name, actual_type_nodes)
              decl = @type_decl_table[base_name]
              return unless decl && decl.type_params.any?

              decl.type_params.zip(actual_type_nodes).each do |param_info, actual_node|
                next unless param_info.respond_to?(:constraint) && param_info.constraint && !param_info.constraint.empty?

                actual_name = extract_actual_type_name(actual_node)
                next if actual_name.nil?

                unless type_satisfies_constraint?(param_info.constraint, actual_name)
                  type_error("Type '#{actual_name}' does not satisfy constraint '#{param_info.constraint}' for '#{param_info.name}'")
                end
              end
            end

            # Type kind inference
            # Determines the kind of a type from AST and SemanticIR information
            #
            # @param ast_decl [MLC::Source::AST::TypeDecl] AST type declaration
            # @param core_ir_type [SemanticIR::Type] SemanticIR type
            # @return [Symbol] :primitive, :record, :sum, :opaque, or :unknown
            def infer_type_kind(ast_decl, core_ir_type)
              # Check if it's an opaque type (explicit MLC::Source::AST::OpaqueType or old-style implicit)
              if core_ir_type.is_a?(SemanticIR::OpaqueType) || ast_decl.type.is_a?(MLC::Source::AST::OpaqueType)
                return :opaque
              end

              # Legacy: Check if it's an old-style opaque type (PrimType with same name as decl)
              # This handles types declared before MLC::Source::AST::OpaqueType was introduced
              if core_ir_type.is_a?(SemanticIR::Type) &&
                 core_ir_type.primitive? &&
                 ast_decl.type.is_a?(MLC::Source::AST::PrimType) &&
                 ast_decl.type.name == ast_decl.name
                return :opaque
              end

              # Otherwise determine from SemanticIR type
              return :record if core_ir_type.is_a?(SemanticIR::RecordType)
              return :sum if core_ir_type.is_a?(SemanticIR::SumType)
              return :primitive if core_ir_type.primitive?

              :unknown
            end

            # Backwards compatibility aliases
            alias_method :ensure_boolean, :ensure_boolean_type
            alias_method :ensure_compatible, :ensure_compatible_type
          end
        end
      end
    end
  end
end
