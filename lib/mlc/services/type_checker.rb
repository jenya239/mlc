# frozen_string_literal: true

module MLC
  module Services
    # TypeChecker - Type checking and validation service
    # Phase 4: Extracted from IRGen BaseTransformer and TypeInference
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

      def initialize(function_registry:, type_decl_table: nil, event_bus: nil, current_node_proc: nil)
        @function_registry = function_registry
        @type_decl_table = type_decl_table || {}
        @event_bus = event_bus
        @current_node_proc = current_node_proc
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
        return if expected.is_a?(HighIR::TypeVariable)
        return if actual_name == expected_name

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
        return true if type.is_a?(HighIR::TypeVariable)

        type_str = normalized_type_name(type_name(type))
        NUMERIC_PRIMITIVES.include?(type_str)
      end

      def generic_type_name?(name)
        return false unless name.is_a?(String)
        name.empty? || name[0]&.match?(/[A-Z]/)
      end

      # IO functions
      def io_return_type(name)
        case IO_RETURN_TYPES[name]
        when "i32"
          HighIR::Builder.primitive_type("i32")
        when "string"
          HighIR::Builder.primitive_type("string")
        when :array_of_string
          HighIR::ArrayType.new(element_type: HighIR::Builder.primitive_type("string"))
        else
          HighIR::Builder.primitive_type("i32")
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
          ensure_compatible_type(args[index].type, type, "argument #{index + 1} of '#{name}'")
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
        when AST::PrimType
          name = type_node.name
          return nil if name.nil?
          return nil if name[0]&.match?(/[A-Z]/)
          name
        else
          nil
        end
      end

      def normalize_type_params(params)
        params.map do |tp|
          name = tp.respond_to?(:name) ? tp.name : tp
          constraint = tp.respond_to?(:constraint) ? tp.constraint : nil
          validate_constraint_name(constraint)
          HighIR::TypeParam.new(name: name, constraint: constraint)
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

      # Backwards compatibility aliases
      alias_method :ensure_boolean, :ensure_boolean_type
      alias_method :ensure_compatible, :ensure_compatible_type
    end
  end
end
