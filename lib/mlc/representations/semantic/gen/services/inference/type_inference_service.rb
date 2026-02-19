# frozen_string_literal: true

# rubocop:disable Metrics/ParameterLists

# rubocop:disable Layout/LineLength

module MLC
  module Representations
    module Semantic
      module Gen
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
          # - transformer: MLC::Representations::Semantic::Gen::Pipeline (for helper methods like type_error, type_name)
          class TypeInferenceService
            attr_writer :generic_call_resolver # Allow injection after initialization

            def initialize(var_type_registry:, type_registry:, function_registry:,
                           type_decl_table:, generic_call_resolver:, type_checker:, scope_context:,
                           trait_registry: nil, _transformer: nil)
              @var_type_registry = var_type_registry
              @type_registry = type_registry
              @function_registry = function_registry
              @type_decl_table = type_decl_table
              @generic_call_resolver = generic_call_resolver # TypeSystem::GenericCallResolver (set after construction)
              @type_checker = type_checker
              @scope_context = scope_context
              @trait_registry = trait_registry
            end

            attr_writer :trait_registry

            # Infer type for variable or function reference
            def infer_variable_type(name)
              return @var_type_registry.get(name) if @var_type_registry.has?(name)

              if (info = @function_registry.fetch(name))
                return function_type_from_info(info)
              end

              # Check if this is an IO built-in function
              if @type_checker.class::IO_RETURN_TYPES.key?(name)
                return_type = @type_checker.io_return_type(name)
                return SemanticIR::Builder.function_type([], return_type)
              end

              return SemanticIR::Builder.primitive_type("bool") if %w[true false].include?(name)

              scope = @var_type_registry.keys.sort.join(", ")
              type_error("Unknown identifier '#{name}' (in scope: #{scope})")
            end

            # Infer return type of function call
            # @param expected_type [SemanticIR::Type, nil] Expected return type for bidirectional inference
            def infer_call_type(callee, args, expected_type: nil)
              case callee
              when SemanticIR::VarExpr
                return @type_checker.io_return_type(callee.name) if @type_checker.class::IO_RETURN_TYPES.key?(callee.name)

                info = @function_registry.fetch(callee.name)
                return SemanticIR::Builder.primitive_type("auto") unless info

                # Check if this is a generic function
                if info.type_params && !info.type_params.empty?
                  instantiation = @generic_call_resolver.instantiate(
                    info, args,
                    name: callee.name,
                    expected_ret_type: expected_type
                  )
                  instantiation.ret_type
                else
                  # Non-generic function - use original logic
                  @type_checker.validate_function_call(info, args, callee.name)
                  info.ret_type
                end

              when SemanticIR::LambdaExpr
                function_type = callee.function_type
                expected = function_type.params || []

                type_error("Lambda expects #{expected.length} argument(s), got #{args.length}") if expected.length != args.length

                expected.each_with_index do |param, index|
                  @type_checker.ensure_compatible_type(args[index].type, param[:type], "lambda argument #{index + 1}")
                end

                function_type.ret_type

              when SemanticIR::MemberExpr
                member = callee.member

                if callee.object.is_a?(SemanticIR::VarExpr) && (info = @type_checker.module_member_info(callee.object.name, member))
                  @type_checker.validate_function_call(info, args, member)
                  return info.ret_type
                end

                object_type = callee.object&.type
                type_error("Cannot call member on value without type") unless object_type

                infer_member_call_type(object_type, member, args, object_ir: callee.object)

              else
                type_error("Cannot call value of type #{describe_type(callee.type)}")
              end
            end

            # Infer type of binary operation
            def infer_binary_type(operator, left_type, right_type)
              ensure_type!(left_type, "Left operand of '#{operator}' has no type")
              ensure_type!(right_type, "Right operand of '#{operator}' has no type")

              return handle_addition(left_type, right_type) if operator == "+"
              return handle_arithmetic(operator, left_type, right_type) if ["-", "*", "%"].include?(operator)
              return handle_division(left_type, right_type) if operator == "/"
              return handle_equality(operator, left_type, right_type) if ["==", "!="].include?(operator)
              return handle_comparison(operator, left_type, right_type) if ["<", ">", "<=", ">="].include?(operator)
              return handle_logical(operator, left_type, right_type) if ["&&", "||"].include?(operator)
              return handle_bitwise(operator, left_type, right_type) if ["&", "|", "^"].include?(operator)
              return handle_shift(operator, left_type, right_type) if ["<<", ">>"].include?(operator)

              left_type
            end

            def handle_addition(left_type, right_type)
              if string_type?(left_type) && string_type?(right_type)
                SemanticIR::Builder.primitive_type("string")
              elsif numeric_type?(left_type) && numeric_type?(right_type)
                combine_numeric_type(left_type, right_type)
              else
                type_error("Cannot add #{describe_type(left_type)} and #{describe_type(right_type)}")
              end
            end

            def handle_arithmetic(operator, left_type, right_type)
              @type_checker.ensure_numeric_type(left_type, "left operand of '#{operator}'")
              @type_checker.ensure_numeric_type(right_type, "right operand of '#{operator}'")
              combine_numeric_type(left_type, right_type)
            end

            def handle_division(left_type, right_type)
              @type_checker.ensure_numeric_type(left_type, "left operand of '/' ")
              @type_checker.ensure_numeric_type(right_type, "right operand of '/' ")
              if float_type?(left_type) || float_type?(right_type)
                SemanticIR::Builder.primitive_type("f32")
              else
                SemanticIR::Builder.primitive_type("i32")
              end
            end

            def handle_equality(operator, left_type, right_type)
              @type_checker.ensure_compatible_type(left_type, right_type, "comparison '#{operator}'")
              SemanticIR::Builder.primitive_type("bool")
            end

            def handle_comparison(operator, left_type, right_type)
              if string_type?(left_type) && string_type?(right_type)
                return SemanticIR::Builder.primitive_type("bool")
              end
              @type_checker.ensure_numeric_type(left_type, "left operand of '#{operator}'")
              @type_checker.ensure_numeric_type(right_type, "right operand of '#{operator}'")
              SemanticIR::Builder.primitive_type("bool")
            end

            def handle_logical(operator, left_type, right_type)
              @type_checker.ensure_boolean_type(left_type, "left operand of '#{operator}'")
              @type_checker.ensure_boolean_type(right_type, "right operand of '#{operator}'")
              SemanticIR::Builder.primitive_type("bool")
            end

            def handle_bitwise(operator, left_type, right_type)
              @type_checker.ensure_integer_type(left_type, "left operand of '#{operator}'")
              @type_checker.ensure_integer_type(right_type, "right operand of '#{operator}'")
              combine_numeric_type(left_type, right_type)
            end

            def handle_shift(operator, left_type, right_type)
              @type_checker.ensure_integer_type(left_type, "left operand of '#{operator}'")
              @type_checker.ensure_integer_type(right_type, "right operand of '#{operator}'")
              left_type
            end

            # Infer type of unary operation
            def infer_unary_type(op, operand_type) # rubocop:disable Naming/MethodParameterName
              ensure_type!(operand_type, "Unary operand for '#{op}' has no type")

              case op
              when "!"
                @type_checker.ensure_boolean_type(operand_type, "operand of '!'")
                SemanticIR::Builder.primitive_type("bool")
              when "-", "+"
                @type_checker.ensure_numeric_type(operand_type, "operand of '#{op}'")
                operand_type
              when "~"
                # Bitwise NOT: requires integer, returns integer
                @type_checker.ensure_integer_type(operand_type, "operand of '~'")
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
                if index.zero? && object_type.is_a?(SemanticIR::ArrayType)
                  [object_type.element_type]
                else
                  []
                end
              when "filter"
                if index.zero? && object_type.is_a?(SemanticIR::ArrayType)
                  [object_type.element_type]
                else
                  []
                end
              when "fold"
                []
              else
                []
              end
            end

            # Infer element type from iterable (for loops)
            def infer_iterable_type(iterable_ir, node: nil)
              if iterable_ir.type.is_a?(SemanticIR::ArrayType)
                iterable_ir.type.element_type
              else
                type_error("Iterable expression must be an array, got #{describe_type(iterable_ir.type)}", node: node)
              end
            end

            # Infer type of member access
            def infer_member_type(object_type, member, node: nil)
              type_error("Cannot access member '#{member}' on value without type", node: node) unless object_type

              if object_type.is_a?(SemanticIR::GenericType)
                base_name = type_name(object_type.base_type)

                if base_name && @type_registry.has_type?(base_name)
                  base_member_type = @type_registry.resolve_member(base_name, member)
                  if base_member_type
                    instantiated = instantiate_generic_member_type(base_name, base_member_type, object_type.type_args)
                    return instantiated if instantiated
                  end
                end

                # Auto-deref for smart pointer types: Shared<T>, Weak<T>, Owned<T>
                # When accessing a member on Shared<Node>, try to resolve it on Node
                if %w[Shared Weak Owned].include?(base_name) && object_type.type_args&.any?
                  inner_type = object_type.type_args.first
                  begin
                    return infer_member_type(inner_type, member, node: node)
                  rescue MLC::Common::TypeError
                    # Fall through to regular error handling
                  end
                end
              end

              if object_type.respond_to?(:name) && @type_registry.has_type?(object_type.name)
                member_type = @type_registry.resolve_member(object_type.name, member)
                return member_type if member_type

                resolved_type = @type_registry.lookup(object_type.name)&.core_ir_type
                return infer_member_type(resolved_type, member) if resolved_type && resolved_type != object_type
              end

              if object_type.record?
                field = object_type.fields.find { |f| f[:name] == member }
                if field
                  return field[:type]
                else
                  # Not a field — check extend blocks for instance methods before erroring
                  instance_fn_type = resolve_extend_instance_method_type(object_type, member)
                  return instance_fn_type if instance_fn_type

                  type_error("Unknown field '#{member}' for type #{object_type.name}", node: node)
                end
              elsif object_type.is_a?(SemanticIR::ArrayType)
                infer_array_member_type(member, node: node)
              elsif object_type.is_a?(SemanticIR::MapType)
                infer_map_member_type(object_type, member, node: node)
              elsif string_type?(object_type)
                infer_string_member_type(member, node: node)
              elsif numeric_type?(object_type)
                infer_numeric_member_type(member, object_type, node: node)
              else
                # Fallback: check extend blocks for instance methods
                instance_fn_type = resolve_extend_instance_method_type(object_type, member)
                return instance_fn_type if instance_fn_type

                type_error("Unknown member '#{member}' for type #{describe_type(object_type)}", node: node)
              end
            end

            # Generic type unification
            def unify_types(pattern_type, concrete_type, type_map)
              Unifier.new(self, type_map).unify(pattern_type, concrete_type)
            end

            class Unifier
              def initialize(context, type_map)
                @context = context
                @type_map = type_map
              end

              def unify(pattern_type, concrete_type)
                case pattern_type
                when SemanticIR::TypeVariable
                  unify_type_variable(pattern_type, concrete_type)
                when SemanticIR::GenericType
                  unify_generic(pattern_type, concrete_type)
                when SemanticIR::ArrayType
                  unify_array(pattern_type, concrete_type)
                when SemanticIR::FunctionType
                  unify_function(pattern_type, concrete_type)
                when SemanticIR::RecordType
                  unify_record(pattern_type, concrete_type)
                end
              end

              private

              def unify_type_variable(pattern_type, concrete_type)
                var_name = pattern_type.name
                return bind_new(var_name, concrete_type) unless @type_map.key?(var_name)

                existing = @type_map[var_name]
                if existing.is_a?(SemanticIR::TypeVariable)
                  @type_map[var_name] = concrete_type
                elsif concrete_type.is_a?(SemanticIR::TypeVariable)
                  # keep existing concrete binding
                elsif normalized(existing) == normalized(concrete_type)
                  @type_map[var_name] = concrete_type
                elsif !types_compatible?(existing, concrete_type)
                  type_error("Type variable #{var_name} bound to both #{describe_type(existing)} and #{describe_type(concrete_type)}")
                end
              end

              def bind_new(var_name, concrete_type)
                @type_map[var_name] = concrete_type
              end

              def unify_generic(pattern_type, concrete_type)
                return unless concrete_type.is_a?(SemanticIR::GenericType)

                unify(pattern_type.base_type, concrete_type.base_type)
                pattern_type.type_args.each_with_index do |pattern_arg, index|
                  concrete_arg = concrete_type.type_args[index]
                  unify(pattern_arg, concrete_arg) if concrete_arg
                end
              end

              def unify_array(pattern_type, concrete_type)
                return unless concrete_type.is_a?(SemanticIR::ArrayType)

                unify(pattern_type.element_type, concrete_type.element_type)
              end

              def unify_function(pattern_type, concrete_type)
                return unless concrete_type.is_a?(SemanticIR::FunctionType)

                pattern_type.params.each_with_index do |pattern_param, index|
                  concrete_param = concrete_type.params[index]
                  next unless concrete_param

                  pattern_param_type = function_param_type(pattern_param)
                  concrete_param_type = function_param_type(concrete_param)
                  unify(pattern_param_type, concrete_param_type)
                end
                unify(pattern_type.ret_type, concrete_type.ret_type)
              end

              def unify_record(pattern_type, concrete_type)
                return unless concrete_type.is_a?(SemanticIR::RecordType)
                return if pattern_type.fields.length != concrete_type.fields.length

                pattern_type.fields.each do |pattern_field|
                  pattern_field_name = pattern_field[:name] || pattern_field.name
                  pattern_field_type = pattern_field[:type] || pattern_field.type

                  concrete_field = concrete_type.fields.find do |cf|
                    cf_name = cf[:name] || cf.name
                    cf_name == pattern_field_name
                  end

                  next unless concrete_field

                  concrete_field_type = concrete_field[:type] || concrete_field.type
                  unify(pattern_field_type, concrete_field_type)
                end
              end

              # delegate helpers
              def function_param_type(param)
                @context.function_param_type(param)
              end

              def types_compatible?(type1, type2)
                @context.types_compatible?(type1, type2)
              end

              def normalized(type)
                @context.normalized_type_name(@context.type_name(type))
              end

              def type_error(msg)
                @context.type_error(msg)
              end

              def describe_type(type)
                @context.describe_type(type)
              end
            end

            # Substitute type variables with concrete types
            def substitute_type(type, type_map)
              case type
              when SemanticIR::TypeVariable
                # Replace type variable with its binding
                type_map[type.name] || type

              when SemanticIR::GenericType
                # Recursively substitute in base type and type arguments
                new_base = substitute_type(type.base_type, type_map)
                new_args = type.type_args.map { |arg| substitute_type(arg, type_map) }
                if new_base != type.base_type || new_args != type.type_args
                  SemanticIR::Builder.generic_type(new_base, new_args)
                else
                  type
                end

              when SemanticIR::ArrayType
                # Substitute in element type
                new_element = substitute_type(type.element_type, type_map)
                new_element != type.element_type ? SemanticIR::Builder.array_type(new_element) : type

              when SemanticIR::FunctionType
                # Substitute in parameters and return type
                new_params = type.params.map do |p|
                  new_type = substitute_type(p[:type], type_map)
                  new_type != p[:type] ? { name: p[:name], type: new_type } : p
                end
                new_ret = substitute_type(type.ret_type, type_map)
                new_params != type.params || new_ret != type.ret_type ? SemanticIR::Builder.function_type(new_params, new_ret) : type

              when SemanticIR::RecordType
                # Substitute type variables in record fields
                new_fields = type.fields.map do |field|
                  field_name = field[:name] || field.name
                  field_type = field[:type] || field.type
                  new_field_type = substitute_type(field_type, type_map)

                  if new_field_type != field_type
                    { name: field_name, type: new_field_type }
                  else
                    field
                  end
                end

                if new_fields != type.fields
                  SemanticIR::RecordType.new(name: type.name, fields: new_fields, origin: type.origin)
                else
                  type
                end

              else
                # Primitive types, etc. - no substitution needed
                type
              end
            end

            # Infer generic type arguments from function call
            def infer_type_arguments(_type_params, param_types, arg_types)
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
              return true if numeric_type?(type1) && numeric_type?(type2)

              false
            end

            # Type predicates
            def numeric_type?(type)
              # TypeVariable is assumed to be numeric-compatible (no constraints yet)
              return true if type.is_a?(SemanticIR::TypeVariable)

              type_str = normalized_type_name(type_name(type))
              return true if MLC::Representations::Semantic::Gen::Services::TypeChecker::NUMERIC_PRIMITIVES.include?(type_str)

              # Check if this is a generic type parameter with Numeric constraint
              type_param = current_type_params.find { |tp| tp.name == type_str }
              type_param && type_param.constraint == "Numeric"
            end

            def string_type?(type)
              %w[string str].include?(normalized_type_name(type_name(type)))
            end

            def void_type?(type)
              return true if type.is_a?(SemanticIR::UnitType)

              normalized_type_name(type_name(type)) == "void"
            end

            def float_type?(type)
              normalized_type_name(type_name(type)) == "f32"
            end

            def function_param_type(param)
              return param[:type] if param.respond_to?(:[]) && param.key?(:type)
              return param.type if param.respond_to?(:type)

              param
            end

            private

            # Combine numeric types (for arithmetic operations)
            def combine_numeric_type(left_type, right_type)
              # If both are type variables, preserve the shared variable when possible
              if left_type.is_a?(SemanticIR::TypeVariable) && right_type.is_a?(SemanticIR::TypeVariable)
                return left_type if left_type.name == right_type.name

                return SemanticIR::Builder.primitive_type("i32")
              end

              # If one is a type variable, return the concrete type
              return right_type if left_type.is_a?(SemanticIR::TypeVariable)
              return left_type if right_type.is_a?(SemanticIR::TypeVariable)

              if type_name(left_type) == type_name(right_type)
                left_type
              elsif float_type?(left_type) || float_type?(right_type)
                SemanticIR::Builder.primitive_type("f32")
              else
                type_error("Numeric operands must have matching types, got #{describe_type(left_type)} and #{describe_type(right_type)}")
              end
            end

            class MemberResolver
              def initialize(context, type_checker)
                @context = context
                @type_checker = type_checker
              end

              def infer(object_type, member, args)
                return resolve_array_member(object_type, member, args) if object_type.is_a?(SemanticIR::ArrayType)
                return resolve_map_member(object_type, member, args) if object_type.is_a?(SemanticIR::MapType)
                return resolve_string_member(member, args) if string?(object_type)
                return resolve_numeric_member(object_type, member, args) if numeric?(object_type)

                # Fallback: check extend blocks for instance methods
                instance_ret = @context.resolve_instance_method_call_type(object_type, member, args)
                return instance_ret if instance_ret

                type_error("Unknown member '#{member}' for type #{describe(object_type)}")
              end

              private

              def resolve_array_member(object_type, member, args)
                if (handler = array_method_table[member])
                  ensure_args(member, args, handler[:args])
                  return instance_exec(object_type, &handler[:ret])
                end

                case member
                when "map"
                  ensure_args(member, args, 1)
                  element_type = lambda_return_type(args.first)
                  type_error("Unable to infer return type of map lambda") unless element_type
                  SemanticIR::ArrayType.new(element_type: element_type)
                when "fold"
                  ensure_args(member, args, 2)
                  accumulator_type = args.first&.type
                  @context.ensure_type!(accumulator_type, "Unable to determine accumulator type for fold")
                  accumulator_type
                when "flatten"
                  if object_type.element_type.is_a?(SemanticIR::ArrayType)
                    object_type.element_type
                  else
                    object_type
                  end
                else
                  type_error(
                    "Unknown array method '#{member}'. Supported methods: " \
                    "length, len, size, is_empty, first, last, reverse, take, drop, contains, join, sum, " \
                    "map, filter, fold, any, all, none, find, find_index, index_of, concat, append, flatten, " \
                    "zip, enumerate, min, max, slice, sort, sort_by, uniq, uniq_by, group_by, partition, " \
                    "take_while, drop_while, first_n, last_n, second, third, count, compact, rotate, sample, " \
                    "product, flat_map, push, pop, set, get"
                  )
                end
              end

              def resolve_map_member(map_type, member, args)
                case member
                when "size", "length"
                  ensure_args(member, args, 0)
                  prim("i32")
                when "is_empty"
                  ensure_args(member, args, 0)
                  prim("bool")
                when "has"
                  ensure_args(member, args, 1)
                  prim("bool")
                when "get"
                  ensure_args(member, args, 1)
                  map_type.value_type
                when "set"
                  ensure_args(member, args, 2)
                  SemanticIR::UnitType.new
                when "remove"
                  ensure_args(member, args, 1)
                  SemanticIR::UnitType.new
                when "keys"
                  ensure_args(member, args, 0)
                  SemanticIR::ArrayType.new(element_type: map_type.key_type)
                when "values"
                  ensure_args(member, args, 0)
                  SemanticIR::ArrayType.new(element_type: map_type.value_type)
                else
                  type_error("Unknown map method '#{member}'. Supported: size, length, is_empty, has, get, set, remove, keys, values")
                end
              end

              def resolve_string_member(member, args)
                case member
                when "split"
                  ensure_args(member, args, 1)
                  SemanticIR::ArrayType.new(element_type: prim("string"))
                when "trim", "trim_start", "trim_end", "upper", "lower"
                  ensure_args(member, args, 0)
                  prim("string")
                when "substring"
                  ensure_args(member, args, 2)
                  prim("string")
                when "is_empty"
                  ensure_args(member, args, 0)
                  prim("bool")
                when "contains", "starts_with", "ends_with"
                  ensure_args(member, args, 1)
                  prim("bool")
                when "length", "len"
                  ensure_args(member, args, 0)
                  prim("i32")
                when "index_of", "last_index_of"
                  ensure_args(member, args, 1)
                  prim("i32")
                when "replace"
                  ensure_args(member, args, 2)
                  prim("string")
                when "char_at"
                  ensure_args(member, args, 1)
                  prim("string")
                when "repeat"
                  ensure_args(member, args, 1)
                  prim("string")
                when "reverse"
                  ensure_args(member, args, 0)
                  prim("string")
                when "to_lower", "to_upper"
                  ensure_args(member, args, 0)
                  prim("string")
                when "is_blank", "is_present"
                  ensure_args(member, args, 0)
                  prim("bool")
                when "squish"
                  ensure_args(member, args, 0)
                  prim("string")
                when "truncate"
                  ensure_args(member, args, 1)
                  prim("string")
                when "titleize", "camelize", "underscore"
                  ensure_args(member, args, 0)
                  prim("string")
                when "pad_start", "pad_end"
                  ensure_args(member, args, 2)
                  prim("string")
                else
                  type_error(
                    "Unknown string method '#{member}'. Supported methods: split, substring, trim, trim_start, trim_end, upper, lower, to_upper, to_lower, is_empty, contains, starts_with, ends_with, length, len, index_of, last_index_of, replace, char_at, repeat, reverse, is_blank, is_present, squish, truncate, titleize, camelize, underscore, pad_start, pad_end"
                  )
                end
              end

              def resolve_numeric_member(object_type, member, args)
                @context.send(:infer_numeric_member_call_type, object_type, member, args)
              end

              def array_method_table
                @array_method_table ||= {
                  "length" => { args: 0, ret: ->(_t) { prim("i32") } },
                  "size" => { args: 0, ret: ->(_t) { prim("i32") } },
                  "len" => { args: 0, ret: ->(_t) { prim("i32") } },
                  "is_empty" => { args: 0, ret: ->(_t) { prim("bool") } },
                  "filter" => { args: 1, ret: ->(t) { array_of(t) } },
                  "first" => { args: 0, ret: ->(t) { element_type_of(t) } },
                  "last" => { args: 0, ret: ->(t) { element_type_of(t) } },
                  "reverse" => { args: 0, ret: ->(t) { array_of(t) } },
                  "take" => { args: 1, ret: ->(t) { array_of(t) } },
                  "drop" => { args: 1, ret: ->(t) { array_of(t) } },
                  "contains" => { args: 1, ret: ->(_t) { prim("bool") } },
                  "join" => { args: 1, ret: ->(_t) { prim("string") } },
                  "sum" => { args: 0, ret: ->(t) { element_type_of(t) } },
                  "any" => { args: 1, ret: ->(_t) { prim("bool") } },
                  "all" => { args: 1, ret: ->(_t) { prim("bool") } },
                  "none" => { args: 1, ret: ->(_t) { prim("bool") } },
                  "find" => { args: 1, ret: ->(t) { element_type_of(t) } },
                  "find_index" => { args: 1, ret: ->(_t) { prim("i32") } },
                  "index_of" => { args: 1, ret: ->(_t) { prim("i32") } },
                  "concat" => { args: 1, ret: ->(t) { array_of(t) } },
                  "append" => { args: 1, ret: ->(t) { array_of(t) } },
                  "zip" => { args: 1, ret: ->(_t) { prim("auto") } },
                  "enumerate" => { args: 0, ret: ->(_t) { prim("auto") } },
                  "min" => { args: 0, ret: ->(t) { element_type_of(t) } },
                  "max" => { args: 0, ret: ->(t) { element_type_of(t) } },
                  "slice" => { args: nil, ret: ->(t) { SemanticIR::ArrayType.new(element_type: t.element_type) } },
                  "sort" => { args: 0, ret: ->(t) { array_of(t) } },
                  "sort_by" => { args: 1, ret: ->(t) { array_of(t) } },
                  "uniq" => { args: 0, ret: ->(t) { array_of(t) } },
                  "uniq_by" => { args: 1, ret: ->(t) { array_of(t) } },
                  "group_by" => { args: 1, ret: ->(_t) { prim("auto") } },
                  "partition" => { args: 1, ret: ->(_t) { prim("auto") } },
                  "take_while" => { args: 1, ret: ->(t) { array_of(t) } },
                  "drop_while" => { args: 1, ret: ->(t) { array_of(t) } },
                  "first_n" => { args: 1, ret: ->(t) { array_of(t) } },
                  "last_n" => { args: 1, ret: ->(t) { array_of(t) } },
                  "second" => { args: 0, ret: ->(t) { element_type_of(t) } },
                  "third" => { args: 0, ret: ->(t) { element_type_of(t) } },
                  "count" => { args: 1, ret: ->(_t) { prim("i32") } },
                  "compact" => { args: 0, ret: ->(t) { array_of(t) } },
                  "rotate" => { args: 1, ret: ->(t) { array_of(t) } },
                  "sample" => { args: 0, ret: ->(t) { element_type_of(t) } },
                  "product" => { args: 0, ret: ->(t) { element_type_of(t) } },
                  "flat_map" => { args: 1, ret: ->(_t) { prim("auto") } },
                  # Mutating methods (require let mut)
                  "push" => { args: 1, ret: ->(_t) { SemanticIR::UnitType.new } }, # rubocop:disable Lint/UnusedBlockArgument
                  "pop" => { args: 0, ret: ->(_t) { SemanticIR::UnitType.new } }, # rubocop:disable Lint/UnusedBlockArgument
                  "set" => { args: 2, ret: ->(_t) { SemanticIR::UnitType.new } }, # rubocop:disable Lint/UnusedBlockArgument
                  "get" => { args: 1, ret: ->(t) { element_type_of(t) } }
                }
              end

              def ensure_args(member, args, count)
                @type_checker.ensure_argument_count(member, args, count) if count
              end

              def element_type_of(array_type)
                array_type.element_type
              end

              def numeric?(type)
                @context.numeric_type?(type)
              end

              def string?(type)
                @context.string_type?(type)
              end

              def prim(name)
                SemanticIR::Builder.primitive_type(name)
              end

              def array_of(array_type)
                SemanticIR::ArrayType.new(element_type: array_type.element_type)
              end

              def describe(type)
                @context.describe_type(type)
              end

              def lambda_return_type(arg)
                @context.send(:lambda_return_type, arg)
              end

              def type_error(message)
                @context.type_error(message)
              end
            end

            ARRAY_MUTATING_METHODS = %w[push pop set].freeze

            # Infer member call type for arrays/maps
            def infer_member_call_type(object_type, member, args, object_ir: nil)
              if ARRAY_MUTATING_METHODS.include?(member) && object_type.is_a?(SemanticIR::ArrayType)
                check_mutability(object_ir, member)
              end
              if HASHMAP_MUTATING_METHODS.include?(member) && object_type.is_a?(SemanticIR::MapType)
                check_mutability(object_ir, member)
              end
              MemberResolver.new(self, @type_checker).infer(object_type, member, args)
            end

            def check_mutability(object_ir, method_name)
              return unless object_ir.is_a?(SemanticIR::VarExpr)

              var_name = object_ir.name
              unless @var_type_registry.mutable?(var_name)
                type_error("Cannot call .#{method_name}() on immutable binding '#{var_name}'. Use 'let mut' to allow mutation")
              end
            end

            # Infer numeric member call type (method call with args)
            def infer_numeric_member_call_type(object_type, member, args)
              case member
              when "abs"
                @type_checker.ensure_argument_count(member, args, 0)
                object_type
              when "sqrt", "floor", "ceil", "round", "sin", "cos", "tan", "exp", "log", "log10"
                @type_checker.ensure_argument_count(member, args, 0)
                SemanticIR::Builder.primitive_type("f32")
              when "pow"
                @type_checker.ensure_argument_count(member, args, 1)
                object_type
              when "min", "max"
                @type_checker.ensure_argument_count(member, args, 1)
                object_type
              when "clamp"
                @type_checker.ensure_argument_count(member, args, 2)
                object_type
              when "to_f32"
                @type_checker.ensure_argument_count(member, args, 0)
                SemanticIR::Builder.primitive_type("f32")
              when "to_i32"
                @type_checker.ensure_argument_count(member, args, 0)
                SemanticIR::Builder.primitive_type("i32")
              when "to_string"
                @type_checker.ensure_argument_count(member, args, 0)
                SemanticIR::Builder.primitive_type("string")
              else
                type_error("Unknown numeric method '#{member}'. Supported methods: abs, sqrt, floor, ceil, round, sin, cos, tan, exp, log, log10, pow, min, max, clamp, to_f32, to_i32, to_string")
              end
            end

            # Infer array member type (non-call)
            def infer_array_member_type(member, node: nil)
              case member
              when "length", "size", "len"
                SemanticIR::Builder.primitive_type("i32")
              when "is_empty"
                SemanticIR::Builder.primitive_type("bool")
              when "first", "last"
                # Returns element type - use auto for deferred resolution
                SemanticIR::Builder.function_type([], SemanticIR::Builder.primitive_type("auto"))
              when "reverse"
                # Returns array of same type - use auto for deferred resolution
                SemanticIR::Builder.function_type([], SemanticIR::Builder.primitive_type("auto"))
              when "map", "filter", "fold", "take", "drop"
                SemanticIR::Builder.function_type([], SemanticIR::Builder.primitive_type("auto"))
              when "contains"
                SemanticIR::Builder.primitive_type("bool")
              when "join"
                SemanticIR::Builder.primitive_type("string")
              when "sum"
                # Returns element type - use auto for deferred resolution
                SemanticIR::Builder.function_type([], SemanticIR::Builder.primitive_type("auto"))
              when "any", "all", "none"
                # Returns bool - checks if predicate matches any/all/none elements
                SemanticIR::Builder.primitive_type("bool")
              when "find"
                # Returns Option<T> - first element matching predicate
                SemanticIR::Builder.function_type([], SemanticIR::Builder.primitive_type("auto"))
              when "find_index", "index_of"
                # Returns i32 (or -1 if not found)
                SemanticIR::Builder.primitive_type("i32")
              when "concat", "append"
                # Returns new array with elements added
                SemanticIR::Builder.function_type([], SemanticIR::Builder.primitive_type("auto"))
              when "flatten"
                # Flattens nested array one level
                SemanticIR::Builder.function_type([], SemanticIR::Builder.primitive_type("auto"))
              when "zip"
                # Zips two arrays together
                SemanticIR::Builder.function_type([], SemanticIR::Builder.primitive_type("auto"))
              when "enumerate"
                # Returns array of (index, element) pairs
                SemanticIR::Builder.function_type([], SemanticIR::Builder.primitive_type("auto"))
              when "min", "max"
                SemanticIR::Builder.function_type([], SemanticIR::Builder.primitive_type("auto"))
              when "slice"
                SemanticIR::Builder.function_type([], SemanticIR::Builder.primitive_type("auto"))
              when "sort", "sort_by", "uniq", "uniq_by", "take_while", "drop_while",
                   "first_n", "last_n", "compact", "rotate"
                SemanticIR::Builder.function_type([], SemanticIR::Builder.primitive_type("auto"))
              when "group_by", "partition", "flat_map"
                SemanticIR::Builder.function_type([], SemanticIR::Builder.primitive_type("auto"))
              when "second", "third", "sample", "product"
                SemanticIR::Builder.function_type([], SemanticIR::Builder.primitive_type("auto"))
              when "count"
                SemanticIR::Builder.primitive_type("i32")
              when "push"
                SemanticIR::Builder.function_type([], SemanticIR::UnitType.new)
              when "pop"
                SemanticIR::Builder.function_type([], SemanticIR::UnitType.new)
              when "set"
                SemanticIR::Builder.function_type([], SemanticIR::UnitType.new)
              when "get"
                SemanticIR::Builder.function_type([], SemanticIR::Builder.primitive_type("auto"))
              else
                type_error(
                  "Unknown array member '#{member}'. Known members: " \
                  "length, len, size, is_empty, first, last, reverse, take, drop, contains, join, sum, " \
                  "map, filter, fold, any, all, none, find, find_index, index_of, concat, append, flatten, " \
                  "zip, enumerate, min, max, slice, sort, sort_by, uniq, uniq_by, group_by, partition, " \
                  "take_while, drop_while, first_n, last_n, second, third, count, compact, rotate, sample, " \
                  "product, flat_map, push, pop, set, get", node: node
                )
              end
            end

            HASHMAP_MUTATING_METHODS = %w[set remove].freeze

            # Infer map member type (non-call)
            def infer_map_member_type(map_type, member, node: nil)
              case member
              when "size", "length"
                SemanticIR::Builder.primitive_type("i32")
              when "is_empty"
                SemanticIR::Builder.primitive_type("bool")
              when "has"
                SemanticIR::Builder.primitive_type("bool")
              when "get"
                SemanticIR::Builder.function_type([], map_type.value_type)
              when "set"
                SemanticIR::Builder.function_type([], SemanticIR::UnitType.new)
              when "remove"
                SemanticIR::Builder.function_type([], SemanticIR::UnitType.new)
              when "keys"
                SemanticIR::ArrayType.new(element_type: map_type.key_type)
              when "values"
                SemanticIR::ArrayType.new(element_type: map_type.value_type)
              else
                type_error("Unknown map member '#{member}'. Known members: size, length, is_empty, has, get, set, remove, keys, values", node: node)
              end
            end

            # Infer string member type (non-call)
            def infer_string_member_type(member, node: nil)
              str = SemanticIR::Builder.primitive_type("string")
              bool = SemanticIR::Builder.primitive_type("bool")
              i32 = SemanticIR::Builder.primitive_type("i32")
              case member
              when "split"
                SemanticIR::ArrayType.new(element_type: str)
              when "trim", "trim_start", "trim_end", "upper", "lower",
                   "to_lower", "to_upper", "substring", "replace",
                   "char_at", "repeat", "reverse", "squish", "truncate",
                   "titleize", "camelize", "underscore", "pad_start", "pad_end"
                str
              when "is_empty", "is_blank", "is_present",
                   "contains", "starts_with", "ends_with"
                bool
              when "length", "len", "index_of", "last_index_of"
                i32
              else
                type_error(
                  "Unknown string member '#{member}'. Supported: split, substring, trim, trim_start, trim_end, upper, lower, to_upper, to_lower, is_empty, is_blank, is_present, contains, starts_with, ends_with, length, len, index_of, last_index_of, replace, char_at, repeat, reverse, squish, truncate, titleize, camelize, underscore, pad_start, pad_end",
                  node: node
                )
              end
            end

            # Infer numeric member type (non-call)
            # Methods available on i32, i64, f32, f64
            def infer_numeric_member_type(member, object_type, node: nil)
              type_name = object_type.respond_to?(:name) ? object_type.name : "i32"
              %w[f32 f64 Float Double float double].include?(type_name)
              %w[i32 i64 i8 i16 u8 u16 u32 u64 Int Int32 Int64].include?(type_name)

              case member
              when "abs"
                # abs() returns same type - available for both int and float
                SemanticIR::Builder.function_type([], object_type)
              when "sqrt", "floor", "ceil", "round", "sin", "cos", "tan", "exp", "log", "log10"
                # Math functions return f32/f64 - primarily for floats but allow on int (will cast)
                f32 = SemanticIR::Builder.primitive_type("f32")
                SemanticIR::Builder.function_type([], f32)
              when "pow"
                # pow(exponent) returns same type
                SemanticIR::Builder.function_type([object_type], object_type)
              when "min", "max"
                # min(other), max(other) return same type
                SemanticIR::Builder.function_type([object_type], object_type)
              when "clamp"
                # clamp(min, max) returns same type
                SemanticIR::Builder.function_type([object_type, object_type], object_type)
              when "to_f32"
                # Convert to f32
                f32 = SemanticIR::Builder.primitive_type("f32")
                SemanticIR::Builder.function_type([], f32)
              when "to_i32"
                # Convert to i32
                i32 = SemanticIR::Builder.primitive_type("i32")
                SemanticIR::Builder.function_type([], i32)
              when "to_string"
                # Convert to string
                str = SemanticIR::Builder.primitive_type("string")
                SemanticIR::Builder.function_type([], str)
              else
                type_error(
                  "Unknown numeric member '#{member}'. Known members: abs, sqrt, floor, ceil, round, sin, cos, tan, exp, log, log10, pow, min, max, clamp, to_f32, to_i32, to_string", node: node
                )
              end
            end

            # Extract return type from lambda expression
            def lambda_return_type(arg)
              return nil unless arg

              if arg.respond_to?(:function_type) && arg.function_type
                arg.function_type.ret_type
              elsif arg.respond_to?(:type) && arg.type.is_a?(SemanticIR::FunctionType)
                arg.type.ret_type
              end
            end

            # Extract base type name from SemanticIR type for trait registry lookup
            def extract_base_type_name(type)
              case type
              when SemanticIR::RecordType
                type.name
              when SemanticIR::GenericType
                base = type.base_type
                base.respond_to?(:name) ? base.name : nil
              when SemanticIR::ArrayType
                "Array"
              when SemanticIR::MapType
                "Map"
              else
                type.respond_to?(:name) ? type.name : nil
              end
            end

            # Check if an extend block defines an instance method for this type+member.
            # Returns FunctionType (without self param) or nil.
            def resolve_extend_instance_method_type(object_type, member)
              return nil unless @trait_registry

              base_name = extract_base_type_name(object_type)
              return nil unless base_name

              mi = @trait_registry.resolve_instance_method(base_name, member)
              return nil unless mi

              mangled = "#{base_name}_#{member}"
              func_info = @function_registry.fetch(mangled)
              return nil unless func_info

              # Build FunctionType excluding the first (self) param
              params = func_info.param_types.drop(1).each_with_index.map do |t, i|
                { name: "arg#{i}", type: t }
              end
              SemanticIR::Builder.function_type(params, func_info.ret_type)
            end

            # Resolve instance method call return type (used by MemberResolver).
            # Returns the return type or nil.
            def resolve_instance_method_call_type(object_type, member, args)
              return nil unless @trait_registry

              base_name = extract_base_type_name(object_type)
              return nil unless base_name

              mi = @trait_registry.resolve_instance_method(base_name, member)
              return nil unless mi

              mangled = "#{base_name}_#{member}"
              func_info = @function_registry.fetch(mangled)
              return nil unless func_info

              # Validate arg count (excluding self)
              expected = func_info.param_types.length - 1
              actual = args.length
              type_error("#{member}() expects #{expected} argument(s), got #{actual}") if expected != actual

              func_info.ret_type
            end

            # Instantiate generic member type
            def instantiate_generic_member_type(base_name, member_type, type_args)
              type_decl = @type_decl_table[base_name]
              return member_type unless type_decl&.type_params&.any?

              type_map = {}
              type_decl.type_params.each_with_index do |tp, index|
                actual = type_args[index]
                next unless actual

                type_map[tp.name] = actual
              end

              substitute_type(member_type, type_map)
            end

            public

            # Get function placeholder type (used in CallRule/MemberRule)
            def function_placeholder_type(name)
              if (info = @function_registry.fetch(name))
                function_type_from_info(info)
              else
                SemanticIR::Builder.function_type([], SemanticIR::Builder.primitive_type("auto"))
              end
            end

            # Build function type from function info
            def function_type_from_info(info)
              params = info.param_types.each_with_index.map do |type, index|
                { name: "arg#{index}", type: type }
              end
              SemanticIR::Builder.function_type(params, info.ret_type)
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
              @scope_context.current_type_params
            end
          end
        end
      end
    end
  end
end
# rubocop:enable Metrics/ParameterLists
# rubocop:enable Layout/LineLength
