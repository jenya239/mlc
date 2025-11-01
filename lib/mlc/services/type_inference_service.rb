# frozen_string_literal: true

module MLC
  module Services
    # TypeInferenceService - Real type inference logic
    # Phase 3: Moved from TypeInference module
    #
    # Responsibilities:
    # - Infer types for variables, calls, operators
    # - Generic type unification and substitution
    # - Lambda parameter type inference
    #
    # Dependencies (injected):
    # - var_type_registry: VarTypeRegistry
    # - type_registry: TypeRegistry
    # - function_registry: FunctionRegistry
    # - type_decl_table: Hash
    # - generic_call_resolver: GenericCallResolverService
    # - transformer: IRGen (for helper methods like type_error, type_name)
    class TypeInferenceService
      def initialize(var_type_registry:, type_registry:, function_registry:,
                     type_decl_table:, generic_call_resolver:, type_checker:, transformer:)
        @var_type_registry = var_type_registry
        @type_registry = type_registry
        @function_registry = function_registry
        @type_decl_table = type_decl_table
        @generic_call_resolver = generic_call_resolver
        @type_checker = type_checker
        @transformer = transformer  # Временно для методов, которые еще не перенесены
      end

      # Infer type for variable or function reference
      def infer_variable_type(name)
        return @var_type_registry.get(name) if @var_type_registry.has?(name)

        if (info = @function_registry.fetch(name))
          return function_type_from_info(info)
        end

        # Check if this is an IO built-in function
        if @type_checker.class::IO_RETURN_TYPES.key?(name)
          return_type = @type_checker.io_return_type(name)
          return HighIR::Builder.function_type([], return_type)
        end

        return HighIR::Builder.primitive_type("bool") if %w[true false].include?(name)

        scope = @var_type_registry.keys.sort.join(", ")
        type_error("Unknown identifier '#{name}' (in scope: #{scope})")
      end

      # Infer return type of function call
      def infer_call_type(callee, args)
        case callee
        when HighIR::VarExpr
          if @type_checker.class::IO_RETURN_TYPES.key?(callee.name)
            return @type_checker.io_return_type(callee.name)
          end

          info = @function_registry.fetch(callee.name)
          unless info
            return HighIR::Builder.primitive_type("auto")
          end

          # Check if this is a generic function
          if info.type_params && !info.type_params.empty?
            instantiation = @generic_call_resolver.instantiate(info, args, name: callee.name)
            instantiation.ret_type
          else
            # Non-generic function - use original logic
            @type_checker.validate_function_call(info, args, callee.name)
            info.ret_type
          end

        when HighIR::LambdaExpr
          function_type = callee.function_type
          expected = function_type.params || []

          if expected.length != args.length
            type_error("Lambda expects #{expected.length} argument(s), got #{args.length}")
          end

          expected.each_with_index do |param, index|
            @type_checker.ensure_compatible_type(args[index].type, param[:type], "lambda argument #{index + 1}")
          end

          function_type.ret_type

        when HighIR::MemberExpr
          member = callee.member

          if callee.object.is_a?(HighIR::VarExpr)
            if (info = @type_checker.module_member_info(callee.object.name, member))
              @type_checker.validate_function_call(info, args, member)
              return info.ret_type
            end
          end

          object_type = callee.object&.type
          type_error("Cannot call member on value without type") unless object_type

          infer_member_call_type(object_type, member, args)

        else
          type_error("Cannot call value of type #{describe_type(callee.type)}")
        end
      end

      # Infer type of binary operation
      def infer_binary_type(op, left_type, right_type)
        ensure_type!(left_type, "Left operand of '#{op}' has no type")
        ensure_type!(right_type, "Right operand of '#{op}' has no type")

        case op
        when "+"
          # Support both numeric addition and string concatenation
          if string_type?(left_type) && string_type?(right_type)
            HighIR::Builder.primitive_type("string")
          elsif numeric_type?(left_type) && numeric_type?(right_type)
            combine_numeric_type(left_type, right_type)
          else
            type_error("Cannot add #{describe_type(left_type)} and #{describe_type(right_type)}")
          end
        when "-", "*", "%"
          @type_checker.ensure_numeric_type(left_type, "left operand of '#{op}'")
          @type_checker.ensure_numeric_type(right_type, "right operand of '#{op}'")
          combine_numeric_type(left_type, right_type)
        when "/"
          @type_checker.ensure_numeric_type(left_type, "left operand of '/' ")
          @type_checker.ensure_numeric_type(right_type, "right operand of '/' ")
          if float_type?(left_type) || float_type?(right_type)
            HighIR::Builder.primitive_type("f32")
          else
            HighIR::Builder.primitive_type("i32")
          end
        when "==", "!="
          @type_checker.ensure_compatible_type(left_type, right_type, "comparison '#{op}'")
          HighIR::Builder.primitive_type("bool")
        when "<", ">", "<=", ">="
          @type_checker.ensure_numeric_type(left_type, "left operand of '#{op}'")
          @type_checker.ensure_numeric_type(right_type, "right operand of '#{op}'")
          HighIR::Builder.primitive_type("bool")
        when "&&", "||"
          @type_checker.ensure_boolean_type(left_type, "left operand of '#{op}'")
          @type_checker.ensure_boolean_type(right_type, "right operand of '#{op}'")
          HighIR::Builder.primitive_type("bool")
        else
          left_type
        end
      end

      # Infer type of unary operation
      def infer_unary_type(op, operand_type)
        ensure_type!(operand_type, "Unary operand for '#{op}' has no type")

        case op
        when "!"
          @type_checker.ensure_boolean_type(operand_type, "operand of '!'")
          HighIR::Builder.primitive_type("bool")
        when "-", "+"
          @type_checker.ensure_numeric_type(operand_type, "operand of '#{op}'")
          operand_type
        else
          operand_type
        end
      end

      # Infer expected lambda parameter types for map/filter/fold
      def expected_lambda_param_types(object_ir, member_name, _transformed_args, index)
        return [] unless object_ir && member_name

        object_type = object_ir.type
        return [] unless object_type

        case member_name
        when "map"
          if index.zero? && object_type.is_a?(HighIR::ArrayType)
            [object_type.element_type]
          else
            []
          end
        when "filter"
          if index.zero? && object_type.is_a?(HighIR::ArrayType)
            [object_type.element_type]
          else
            []
          end
        when "fold"
          if index == 1 && object_type.is_a?(HighIR::ArrayType)
            # fold(init, fn) - fn takes (accumulator, element)
            []
          else
            []
          end
        else
          []
        end
      end

      # Infer element type from iterable (for loops)
      def infer_iterable_type(iterable_ir, node: nil)
        if iterable_ir.type.is_a?(HighIR::ArrayType)
          iterable_ir.type.element_type
        else
          type_error("Iterable expression must be an array, got #{describe_type(iterable_ir.type)}", node: node)
        end
      end

      # Infer type of member access
      def infer_member_type(object_type, member, node: nil)
        type_error("Cannot access member '#{member}' on value without type", node: node) unless object_type

        if object_type.is_a?(HighIR::GenericType)
          base_name = type_name(object_type.base_type)

          if base_name && @type_registry.has_type?(base_name)
            base_member_type = @type_registry.resolve_member(base_name, member)
            if base_member_type
              instantiated = instantiate_generic_member_type(base_name, base_member_type, object_type.type_args)
              return instantiated if instantiated
            end
          end
        end

        if object_type.respond_to?(:name) && @type_registry.has_type?(object_type.name)
          member_type = @type_registry.resolve_member(object_type.name, member)
          return member_type if member_type

          resolved_type = @type_registry.lookup(object_type.name)&.core_ir_type
          if resolved_type && resolved_type != object_type
            return infer_member_type(resolved_type, member)
          end
        end

        if object_type.record?
          field = object_type.fields.find { |f| f[:name] == member }
          type_error("Unknown field '#{member}' for type #{object_type.name}", node: node) unless field
          field[:type]
        elsif object_type.is_a?(HighIR::ArrayType)
          infer_array_member_type(member, node: node)
        elsif string_type?(object_type)
          infer_string_member_type(member, node: node)
        elsif numeric_type?(object_type) && member == "sqrt"
          f32 = HighIR::Builder.primitive_type("f32")
          HighIR::Builder.function_type([], f32)
        else
          type_error("Unknown member '#{member}' for type #{describe_type(object_type)}", node: node)
        end
      end

      # Generic type unification
      def unify_types(pattern_type, concrete_type, type_map)
        case pattern_type
        when HighIR::TypeVariable
          # This is a type variable - bind it to the concrete type
          var_name = pattern_type.name
          if type_map.key?(var_name)
            # Already bound - verify consistency
            existing = type_map[var_name]
            if existing.is_a?(HighIR::TypeVariable)
              type_map[var_name] = concrete_type
            elsif concrete_type.is_a?(HighIR::TypeVariable)
              # keep existing concrete binding
            elsif normalized_type_name(type_name(existing)) == normalized_type_name(type_name(concrete_type))
              type_map[var_name] = concrete_type
            elsif !types_compatible?(existing, concrete_type)
              type_error("Type variable #{var_name} bound to both #{describe_type(existing)} and #{describe_type(concrete_type)}")
            end
          else
            # New binding
            type_map[var_name] = concrete_type
          end

        when HighIR::GenericType
          # Both should be generic with same base and compatible args
          if concrete_type.is_a?(HighIR::GenericType)
            unify_types(pattern_type.base_type, concrete_type.base_type, type_map)
            pattern_type.type_args.each_with_index do |pattern_arg, index|
              concrete_arg = concrete_type.type_args[index]
              unify_types(pattern_arg, concrete_arg, type_map) if concrete_arg
            end
          end

        when HighIR::ArrayType
          # Array types - unify element types
          if concrete_type.is_a?(HighIR::ArrayType)
            unify_types(pattern_type.element_type, concrete_type.element_type, type_map)
          end

        when HighIR::FunctionType
          # Function types - unify parameters and return types
          if concrete_type.is_a?(HighIR::FunctionType)
            pattern_type.params.each_with_index do |pattern_param, index|
              concrete_param = concrete_type.params[index]
              unify_types(pattern_param[:type], concrete_param[:type], type_map) if concrete_param
            end
            unify_types(pattern_type.ret_type, concrete_type.ret_type, type_map)
          end

        else
          # Primitive types, record types, etc. - just verify they match
          # No unification needed
        end
      end

      # Substitute type variables with concrete types
      def substitute_type(type, type_map)
        case type
        when HighIR::TypeVariable
          # Replace type variable with its binding
          type_map[type.name] || type

        when HighIR::GenericType
          # Recursively substitute in base type and type arguments
          new_base = substitute_type(type.base_type, type_map)
          new_args = type.type_args.map { |arg| substitute_type(arg, type_map) }
          if new_base != type.base_type || new_args != type.type_args
            HighIR::Builder.generic_type(new_base, new_args)
          else
            type
          end

        when HighIR::ArrayType
          # Substitute in element type
          new_element = substitute_type(type.element_type, type_map)
          new_element != type.element_type ? HighIR::Builder.array_type(new_element) : type

        when HighIR::FunctionType
          # Substitute in parameters and return type
          new_params = type.params.map do |p|
            new_type = substitute_type(p[:type], type_map)
            new_type != p[:type] ? {name: p[:name], type: new_type} : p
          end
          new_ret = substitute_type(type.ret_type, type_map)
          (new_params != type.params || new_ret != type.ret_type) ? HighIR::Builder.function_type(new_params, new_ret) : type

        else
          # Primitive types, record types, etc. - no substitution needed
          type
        end
      end

      # Infer generic type arguments from function call
      def infer_type_arguments(type_params, param_types, arg_types)
        type_map = {}

        param_types.each_with_index do |param_type, index|
          arg_type = arg_types[index]
          next unless arg_type

          unify_types(param_type, arg_type, type_map)
        end

        type_map
      end

      # Check if two types are compatible
      def types_compatible?(type1, type2)
        return true if type1 == type2

        name1 = type_name(type1)
        name2 = type_name(type2)

        return true if name1 == name2

        # Allow some flexibility for numeric types
        if numeric_type?(type1) && numeric_type?(type2)
          return true
        end

        false
      end

      # Type predicates
      def numeric_type?(type)
        # TypeVariable is assumed to be numeric-compatible (no constraints yet)
        return true if type.is_a?(HighIR::TypeVariable)

        type_str = normalized_type_name(type_name(type))
        return true if @transformer.class::NUMERIC_PRIMITIVES.include?(type_str)

        # Check if this is a generic type parameter with Numeric constraint
        type_param = current_type_params.find { |tp| tp.name == type_str }
        type_param && type_param.constraint == "Numeric"
      end

      def string_type?(type)
        %w[string str].include?(normalized_type_name(type_name(type)))
      end

      def void_type?(type)
        return true if type.is_a?(HighIR::UnitType)
        normalized_type_name(type_name(type)) == "void"
      end

      def float_type?(type)
        normalized_type_name(type_name(type)) == "f32"
      end

      private

      # Combine numeric types (for arithmetic operations)
      def combine_numeric_type(left_type, right_type)
        # If both are type variables, preserve the shared variable when possible
        if left_type.is_a?(HighIR::TypeVariable) && right_type.is_a?(HighIR::TypeVariable)
          return left_type if left_type.name == right_type.name
          return HighIR::Builder.primitive_type("i32")
        end

        # If one is a type variable, return the concrete type
        return right_type if left_type.is_a?(HighIR::TypeVariable)
        return left_type if right_type.is_a?(HighIR::TypeVariable)

        if type_name(left_type) == type_name(right_type)
          left_type
        elsif float_type?(left_type) || float_type?(right_type)
          HighIR::Builder.primitive_type("f32")
        else
          type_error("Numeric operands must have matching types, got #{describe_type(left_type)} and #{describe_type(right_type)}")
        end
      end

      # Infer member call type for arrays
      def infer_member_call_type(object_type, member, args)
        if object_type.is_a?(HighIR::ArrayType)
          case member
          when "length", "size"
            @type_checker.ensure_argument_count(member, args, 0)
            HighIR::Builder.primitive_type("i32")
          when "is_empty"
            @type_checker.ensure_argument_count(member, args, 0)
            HighIR::Builder.primitive_type("bool")
          when "map"
            @type_checker.ensure_argument_count(member, args, 1)
            element_type = lambda_return_type(args.first)
            type_error("Unable to infer return type of map lambda") unless element_type
            HighIR::ArrayType.new(element_type: element_type)
          when "filter"
            @type_checker.ensure_argument_count(member, args, 1)
            HighIR::ArrayType.new(element_type: object_type.element_type)
          when "fold"
            @type_checker.ensure_argument_count(member, args, 2)
            accumulator_type = args.first&.type
            ensure_type!(accumulator_type, "Unable to determine accumulator type for fold")
            accumulator_type
          else
            type_error("Unknown array method '#{member}'. Supported methods: length, size, is_empty, map, filter, fold")
          end
        elsif string_type?(object_type)
          case member
          when "split"
            @type_checker.ensure_argument_count(member, args, 1)
            HighIR::ArrayType.new(element_type: HighIR::Builder.primitive_type("string"))
          when "trim", "trim_start", "trim_end", "upper", "lower"
            @type_checker.ensure_argument_count(member, args, 0)
            HighIR::Builder.primitive_type("string")
          when "is_empty"
            @type_checker.ensure_argument_count(member, args, 0)
            HighIR::Builder.primitive_type("bool")
          when "length"
            @type_checker.ensure_argument_count(member, args, 0)
            HighIR::Builder.primitive_type("i32")
          else
            type_error("Unknown string method '#{member}'. Supported methods: split, trim, trim_start, trim_end, upper, lower, is_empty, length")
          end
        elsif numeric_type?(object_type) && member == "sqrt"
          @type_checker.ensure_argument_count(member, args, 0)
          HighIR::Builder.primitive_type("f32")
        else
          type_error("Unknown member '#{member}' for type #{describe_type(object_type)}")
        end
      end

      # Infer array member type (non-call)
      def infer_array_member_type(member, node: nil)
        case member
        when "length", "size"
          HighIR::Builder.primitive_type("i32")
        when "is_empty"
          HighIR::Builder.primitive_type("bool")
        when "map", "filter", "fold"
          HighIR::Builder.function_type([], HighIR::Builder.primitive_type("auto"))
        else
          type_error("Unknown array member '#{member}'. Known members: length, size, is_empty, map, filter, fold", node: node)
        end
      end

      # Infer string member type (non-call)
      def infer_string_member_type(member, node: nil)
        case member
        when "split"
          HighIR::ArrayType.new(element_type: HighIR::Builder.primitive_type("string"))
        when "trim", "trim_start", "trim_end", "upper", "lower"
          HighIR::Builder.primitive_type("string")
        when "is_empty"
          HighIR::Builder.primitive_type("bool")
        when "length"
          HighIR::Builder.primitive_type("i32")
        else
          type_error("Unknown string member '#{member}'. Known members: split, trim, trim_start, trim_end, upper, lower, is_empty, length", node: node)
        end
      end

      # Extract return type from lambda expression
      def lambda_return_type(arg)
        return nil unless arg

        if arg.respond_to?(:function_type) && arg.function_type
          arg.function_type.ret_type
        elsif arg.respond_to?(:type) && arg.type.is_a?(HighIR::FunctionType)
          arg.type.ret_type
        else
          nil
        end
      end

      # Instantiate generic member type
      def instantiate_generic_member_type(base_name, member_type, type_args)
        type_decl = @type_decl_table[base_name]
        return member_type unless type_decl && type_decl.type_params.any?

        type_map = {}
        type_decl.type_params.each_with_index do |tp, index|
          actual = type_args[index]
          next unless actual
          type_map[tp.name] = actual
        end

        substitute_type(member_type, type_map)
      end

      # Get function placeholder type (used in CallRule/MemberRule)
      def function_placeholder_type(name)
        if (info = @function_registry.fetch(name))
          function_type_from_info(info)
        else
          HighIR::Builder.function_type([], HighIR::Builder.primitive_type("auto"))
        end
      end

      # Build function type from function info
      def function_type_from_info(info)
        params = info.param_types.each_with_index.map do |type, index|
          {name: "arg#{index}", type: type}
        end
        HighIR::Builder.function_type(params, info.ret_type)
      end

      # Delegate to transformer helper methods
      # (these will move to TypeChecker in Phase 4)
      def type_error(message, node: nil)
        @type_checker.type_error(message, node: node)
      end

      def type_name(type)
        @type_checker.type_name(type)
      end

      def describe_type(type)
        @type_checker.describe_type(type)
      end

      def normalized_type_name(name)
        @type_checker.normalized_type_name(name)
      end

      def ensure_type!(type, message, node: nil)
        @type_checker.ensure_type!(type, message, node: node)
      end

      def current_type_params
        @transformer.send(:current_type_params)
      end
    end
  end
end
