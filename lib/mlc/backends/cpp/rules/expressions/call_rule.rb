# frozen_string_literal: true

module MLC
  module Backends
    module Cpp
      module Rules
        module Expressions
          # Rule for lowering SemanticIR::CallExpr to C++ function calls
          # Handles: IO functions, stdlib overrides, array methods, regular calls
          class CallRule < ExpressionRule
            # IO function mappings
            IO_FUNCTIONS = {
              "print" => "mlc::io::print",
              "println" => "mlc::io::println",
              "eprint" => "mlc::io::eprint",
              "eprintln" => "mlc::io::eprintln",
              "read_line" => "mlc::io::read_line",
              "read_all" => "mlc::io::read_all",
              "input" => "mlc::io::read_all",
              "args" => "mlc::io::args",
              "exit" => "mlc::io::exit",
              "to_string" => "mlc::to_string",
              "format" => "mlc::format"
            }.freeze

            # Stdlib function overrides
            STDLIB_OVERRIDES = {
              "to_f32" => "static_cast<float>"
            }.freeze

            # Smart pointer function mappings
            # Type_method pattern from extend blocks
            SMART_POINTER_FUNCTIONS = {
              # Shared<T> methods
              "Shared_new" => :make_shared,
              "Shared_null" => :shared_null,
              "Shared_downgrade" => :shared_downgrade,
              # Weak<T> methods
              "Weak_null" => :weak_null,
              "Weak_lock" => :weak_lock,
              "Weak_is_valid" => :weak_is_valid,
              # Owned<T> methods
              "Owned_new" => :make_unique,
              "Owned_null" => :owned_null,
              # Functional style (from stdlib)
              "make_shared" => :make_shared,
              "shared_null" => :shared_null,
              "shared_downgrade" => :shared_downgrade,
              "weak_null" => :weak_null,
              "weak_lock" => :weak_lock,
              "weak_is_valid" => :weak_is_valid,
              "make_owned" => :make_unique,
              "owned_null" => :owned_null
            }.freeze

            def applies?(node)
              context.checker.call_expr?(node)
            end

            def apply(node)
              # Check for IO functions
              return lower_io_function(node) if context.checker.var_expr?(node.callee) && IO_FUNCTIONS.key?(node.callee.name)

              # Check for smart pointer functions
              if context.checker.var_expr?(node.callee) && SMART_POINTER_FUNCTIONS.key?(node.callee.name)
                return lower_smart_pointer_function(node)
              end

              # Check for Map/HashMap constructor
              if context.checker.var_expr?(node.callee) && %w[Map_new HashMap_new].include?(node.callee.name)
                return lower_map_constructor(node)
              end

              # Check for File methods
              if context.checker.var_expr?(node.callee) && node.callee.name.start_with?("File_")
                return lower_file_method(node)
              end

              # Check for stdlib overrides
              if context.checker.var_expr?(node.callee)
                override = lower_stdlib_override(node)
                return override if override
              end

              # Check for qualified function names (stdlib functions)
              if context.checker.var_expr?(node.callee)
                qualified = context.resolve_qualified_name(node.callee.name)
                return lower_qualified_function(node, qualified) if qualified
              end

              # Check for array method calls
              if context.checker.member_expr?(node.callee) && node.callee.object.type.is_a?(MLC::SemanticIR::ArrayType)
                return lower_array_method(node)
              end

              # Check for map method calls
              if context.checker.member_expr?(node.callee) && node.callee.object.type.is_a?(MLC::SemanticIR::MapType)
                return lower_map_method(node)
              end

              # Check for string method calls
              if context.checker.member_expr?(node.callee) &&
                 node.callee.object.type.respond_to?(:primitive?) &&
                 node.callee.object.type.primitive? &&
                 %w[String string str].include?(node.callee.object.type.name)
                return lower_string_method(node)
              end

              # Check for numeric method calls
              if context.checker.member_expr?(node.callee) &&
                 node.callee.object.type.respond_to?(:primitive?) &&
                 node.callee.object.type.primitive? &&
                 numeric_type_name?(node.callee.object.type.name)
                return lower_numeric_method(node)
              end

              # Regular function call.
              callee = lower_expression(node.callee)
              args = node.args.map { |arg| lower_argument_with_move(arg) }

              # Nullary variant constructors: CtorName{} or (ns::Ctor{}) for ternary safety.
              # Adding () would produce invalid CtorName{}(). Return callee directly.
              if args.empty? && callee.is_a?(CppAst::Nodes::RawExpression)
                src = callee.to_source
                return callee if src.end_with?("{}") || src.match?(/\([^)]*\{\}\)\z/)
              end

              context.factory.function_call(
                callee: callee,
                arguments: args,
                argument_separators: args.size > 1 ? Array.new(args.size - 1, ", ") : []
              )
            end

            private

            # Lower IO function calls
            def lower_io_function(call)
              target = IO_FUNCTIONS[call.callee.name]
              callee = context.factory.identifier(name: target)
              args = call.args.map { |arg| lower_expression(arg) }

              context.factory.function_call(
                callee: callee,
                arguments: args,
                argument_separators: args.size > 1 ? Array.new(args.size - 1, ", ") : []
              )
            end

            # Lower smart pointer function calls
            # Maps MLC smart pointer functions to C++ equivalents
            def lower_smart_pointer_function(call)
              func_type = SMART_POINTER_FUNCTIONS[call.callee.name]
              args = call.args.map { |arg| lower_expression(arg) }

              # Get the return type to determine template argument
              ret_type = call.type
              inner_type = extract_smart_pointer_inner_type(ret_type)

              case func_type
              when :make_shared
                # std::make_shared<T>(value)
                callee = context.factory.identifier(name: "std::make_shared<#{inner_type}>")
                context.factory.function_call(
                  callee: callee,
                  arguments: args,
                  argument_separators: args.size > 1 ? Array.new(args.size - 1, ", ") : []
                )

              when :shared_null
                # std::shared_ptr<T>()
                context.factory.identifier(name: "std::shared_ptr<#{inner_type}>()")

              when :shared_downgrade
                # std::weak_ptr<T>(shared) - constructor from shared_ptr
                context.factory.function_call(
                  callee: context.factory.identifier(name: "std::weak_ptr<#{inner_type}>"),
                  arguments: args,
                  argument_separators: []
                )

              when :weak_null
                # std::weak_ptr<T>()
                context.factory.identifier(name: "std::weak_ptr<#{inner_type}>()")

              when :weak_lock
                # weak.lock() - returns std::shared_ptr, need to wrap in Option
                # For now, generate: mlc::option::from_nullable(weak.lock())
                if args.first
                  member = context.factory.member_access(
                    object: args.first,
                    operator: ".",
                    member: context.factory.identifier(name: "lock")
                  )
                  lock_call = context.factory.function_call(
                    callee: member,
                    arguments: [],
                    argument_separators: []
                  )
                  # Wrap in Option - using from_nullable helper
                  context.factory.function_call(
                    callee: context.factory.identifier(name: "mlc::option::from_nullable"),
                    arguments: [lock_call],
                    argument_separators: []
                  )
                else
                  context.factory.identifier(name: "std::nullopt")
                end

              when :weak_is_valid
                # !weak.expired()
                if args.first
                  member = context.factory.member_access(
                    object: args.first,
                    operator: ".",
                    member: context.factory.identifier(name: "expired")
                  )
                  expired_call = context.factory.function_call(
                    callee: member,
                    arguments: [],
                    argument_separators: []
                  )
                  context.factory.unary_expression(
                    operator: "!",
                    operand: expired_call
                  )
                else
                  context.factory.identifier(name: "false")
                end

              when :make_unique
                # std::make_unique<T>(value)
                callee = context.factory.identifier(name: "std::make_unique<#{inner_type}>")
                context.factory.function_call(
                  callee: callee,
                  arguments: args,
                  argument_separators: args.size > 1 ? Array.new(args.size - 1, ", ") : []
                )

              when :owned_null
                # std::unique_ptr<T>()
                context.factory.identifier(name: "std::unique_ptr<#{inner_type}>()")

              else
                # Fallback to regular call
                callee = lower_expression(call.callee)
                context.factory.function_call(
                  callee: callee,
                  arguments: args,
                  argument_separators: args.size > 1 ? Array.new(args.size - 1, ", ") : []
                )
              end
            end

            # Extract inner type from smart pointer type (e.g., Node from Shared<Node>)
            def extract_smart_pointer_inner_type(type)
              case type
              when MLC::SemanticIR::GenericType
                if type.type_args&.first
                  arg_type = type.type_args.first
                  # If inner type is a zero-arg FunctionType (() -> T), unwrap to T.
                  # This happens when a nullary variant constructor (TyI32 : () -> TypeExpr)
                  # is passed to Shared.new and the type is inferred as Shared<() -> T>.
                  if arg_type.is_a?(MLC::SemanticIR::FunctionType) &&
                     arg_type.params.empty? &&
                     arg_type.respond_to?(:ret_type)
                    context.map_type(arg_type.ret_type)
                  else
                    context.map_type(arg_type)
                  end
                else
                  "auto"
                end
              when MLC::SemanticIR::Type
                "auto"
              else
                "auto"
              end
            end

            # Lower stdlib override functions
            def lower_stdlib_override(call)
              override = STDLIB_OVERRIDES[call.callee.name]
              return nil unless override

              case call.callee.name
              when "to_f32"
                arg = call.args.first ? lower_expression(call.args.first) : context.factory.identifier(name: "0")
                context.factory.function_call(
                  callee: context.factory.identifier(name: override),
                  arguments: [arg],
                  argument_separators: []
                )
              end
            end

            # Lower qualified function calls (stdlib functions with namespace)
            def lower_qualified_function(call, qualified_name)
              callee = context.factory.identifier(name: qualified_name)
              args = call.args.map { |arg| lower_expression(arg) }

              context.factory.function_call(
                callee: callee,
                arguments: args,
                argument_separators: args.size > 1 ? Array.new(args.size - 1, ", ") : []
              )
            end

            def direct_method_call(receiver, method_name, args)
              member = context.factory.member_access(
                object: receiver,
                operator: ".",
                member: context.factory.identifier(name: method_name)
              )
              context.factory.function_call(
                callee: member,
                arguments: args,
                argument_separators: args.size > 1 ? ::Array.new(args.size - 1, ", ") : []
              )
            end

            # Lower array method calls
            def lower_array_method(call)
              method_name = call.callee.member
              array_obj = wrap_for_member_access(lower_expression(call.callee.object), call.callee.object)

              case method_name
              when "length", "len"
                # arr.length() / arr.len() -> arr.size()
                member = context.factory.member_access(
                  object: array_obj,
                  operator: ".",
                  member: context.factory.identifier(name: "size")
                )
                context.factory.function_call(
                  callee: member,
                  arguments: [],
                  argument_separators: []
                )

              when "is_empty"
                # arr.is_empty() -> arr.empty()
                member = context.factory.member_access(
                  object: array_obj,
                  operator: ".",
                  member: context.factory.identifier(name: "empty")
                )
                context.factory.function_call(
                  callee: member,
                  arguments: [],
                  argument_separators: []
                )

              when "first"
                # arr.first() -> arr.front()
                member = context.factory.member_access(
                  object: array_obj,
                  operator: ".",
                  member: context.factory.identifier(name: "front")
                )
                context.factory.function_call(
                  callee: member,
                  arguments: [],
                  argument_separators: []
                )

              when "last"
                # arr.last() -> arr.back()
                member = context.factory.member_access(
                  object: array_obj,
                  operator: ".",
                  member: context.factory.identifier(name: "back")
                )
                context.factory.function_call(
                  callee: member,
                  arguments: [],
                  argument_separators: []
                )

              when "reverse"
                direct_method_call(array_obj, "reverse", [])

              when "push"
                # arr.push(x) -> arr.push_back(x)
                member = context.factory.member_access(
                  object: array_obj,
                  operator: ".",
                  member: context.factory.identifier(name: "push_back")
                )
                args = call.args.map { |arg| lower_expression(arg) }
                context.factory.function_call(
                  callee: member,
                  arguments: args,
                  argument_separators: args.size > 1 ? Array.new(args.size - 1, ", ") : []
                )

              when "pop"
                # arr.pop() -> arr.pop_back() (returns void in C++, but we handle it)
                member = context.factory.member_access(
                  object: array_obj,
                  operator: ".",
                  member: context.factory.identifier(name: "pop_back")
                )
                context.factory.function_call(
                  callee: member,
                  arguments: [],
                  argument_separators: []
                )

              when "set"
                # arr.set(i, v) -> arr.set(i, v) (COW-safe)
                member = context.factory.member_access(
                  object: array_obj,
                  operator: ".",
                  member: context.factory.identifier(name: "set")
                )
                args = call.args.map { |arg| lower_expression(arg) }
                context.factory.function_call(
                  callee: member,
                  arguments: args,
                  argument_separators: [", "]
                )

              when "get"
                # arr.get(i) -> arr[i]
                args = call.args.map { |arg| lower_expression(arg) }
                CppAst::Nodes::ArraySubscriptExpression.new(
                  array: array_obj,
                  index: args[0]
                )

              when "map", "filter", "any", "all", "none", "find", "find_index",
                   "contains", "index_of", "reverse", "sort", "enumerate",
                   "concat", "append", "zip", "sort_by", "take", "drop",
                   "first_n", "last_n", "flat_map", "each",
                   "uniq", "uniq_by", "group_by", "partition", "flatten",
                   "min", "max"
                args = call.args.map { |a| lower_expression(a) }
                direct_method_call(array_obj, method_name, args)

              when "join"
                args = call.args.map { |a| lower_expression(a) }
                direct_method_call(array_obj, "join", args)

              when "sum"
                direct_method_call(
                  array_obj, "fold",
                  [context.factory.identifier(name: "0"),
                   context.factory.raw_expression(code: "[](auto acc, auto x) { return acc + x; }")]
                )

              when "fold"
                args = call.args.map { |a| lower_expression(a) }
                direct_method_call(array_obj, "fold", args)

              when "slice"
                args = call.args.map { |a| lower_expression(a) }
                context.factory.function_call(
                  callee: context.factory.identifier(name: "mlc::collections::slice"),
                  arguments: [array_obj] + args,
                  argument_separators: ::Array.new(args.size, ", ")
                )

              when "take_while"
                # arr.take_while(pred) -> mlc::collections::take_while(arr, pred)
                predicate = call.args.first ? lower_expression(call.args.first) : nil
                args = [array_obj, predicate].compact
                context.factory.function_call(
                  callee: context.factory.identifier(name: "mlc::collections::take_while"),
                  arguments: args,
                  argument_separators: predicate ? [", "] : []
                )

              when "drop_while"
                # arr.drop_while(pred) -> mlc::collections::drop_while(arr, pred)
                predicate = call.args.first ? lower_expression(call.args.first) : nil
                args = [array_obj, predicate].compact
                context.factory.function_call(
                  callee: context.factory.identifier(name: "mlc::collections::drop_while"),
                  arguments: args,
                  argument_separators: predicate ? [", "] : []
                )

              when "first_n"
                # arr.first_n(n) -> mlc::collections::first_n(arr, n)
                count = call.args.first ? lower_expression(call.args.first) : nil
                args = [array_obj, count].compact
                context.factory.function_call(
                  callee: context.factory.identifier(name: "mlc::collections::first_n"),
                  arguments: args,
                  argument_separators: count ? [", "] : []
                )

              when "last_n"
                # arr.last_n(n) -> mlc::collections::last_n(arr, n)
                count = call.args.first ? lower_expression(call.args.first) : nil
                args = [array_obj, count].compact
                context.factory.function_call(
                  callee: context.factory.identifier(name: "mlc::collections::last_n"),
                  arguments: args,
                  argument_separators: count ? [", "] : []
                )

              when "second"
                # arr.second() -> mlc::collections::second(arr)
                context.factory.function_call(
                  callee: context.factory.identifier(name: "mlc::collections::second"),
                  arguments: [array_obj],
                  argument_separators: []
                )

              when "third"
                # arr.third() -> mlc::collections::third(arr)
                context.factory.function_call(
                  callee: context.factory.identifier(name: "mlc::collections::third"),
                  arguments: [array_obj],
                  argument_separators: []
                )

              when "count"
                # arr.count(pred) -> mlc::collections::count(arr, pred)
                predicate = call.args.first ? lower_expression(call.args.first) : nil
                args = [array_obj, predicate].compact
                context.factory.function_call(
                  callee: context.factory.identifier(name: "mlc::collections::count"),
                  arguments: args,
                  argument_separators: predicate ? [", "] : []
                )

              when "compact"
                # arr.compact() -> mlc::collections::compact(arr)
                context.factory.function_call(
                  callee: context.factory.identifier(name: "mlc::collections::compact"),
                  arguments: [array_obj],
                  argument_separators: []
                )

              when "rotate"
                # arr.rotate(n) -> mlc::collections::rotate(arr, n)
                count = call.args.first ? lower_expression(call.args.first) : nil
                args = [array_obj, count].compact
                context.factory.function_call(
                  callee: context.factory.identifier(name: "mlc::collections::rotate"),
                  arguments: args,
                  argument_separators: count ? [", "] : []
                )

              when "sample"
                # arr.sample() -> mlc::collections::sample(arr)
                context.factory.function_call(
                  callee: context.factory.identifier(name: "mlc::collections::sample"),
                  arguments: [array_obj],
                  argument_separators: []
                )

              when "product"
                # arr.product() -> mlc::collections::product(arr)
                context.factory.function_call(
                  callee: context.factory.identifier(name: "mlc::collections::product"),
                  arguments: [array_obj],
                  argument_separators: []
                )

              when "flat_map"
                # arr.flat_map(f) -> mlc::collections::flat_map(arr, f)
                func = call.args.first ? lower_expression(call.args.first) : nil
                args = [array_obj, func].compact
                context.factory.function_call(
                  callee: context.factory.identifier(name: "mlc::collections::flat_map"),
                  arguments: args,
                  argument_separators: func ? [", "] : []
                )

              else
                # Fallback: call method directly
                member = context.factory.member_access(
                  object: array_obj,
                  operator: ".",
                  member: context.factory.identifier(name: method_name)
                )
                args = call.args.map { |arg| lower_expression(arg) }
                context.factory.function_call(
                  callee: member,
                  arguments: args,
                  argument_separators: args.size > 1 ? Array.new(args.size - 1, ", ") : []
                )
              end
            end

            # Map.new() / HashMap.new() -> mlc::HashMap<K,V>()
            def lower_map_constructor(node)
              map_type = context.map_type(node.type)
              context.factory.function_call(
                callee: context.factory.identifier(name: map_type),
                arguments: [],
                argument_separators: []
              )
            end

            # File.read(path) -> mlc::file::read_to_string(path)
            # File.write(path, content) -> mlc::file::write_string(path, content)
            # File.exists(path) -> mlc::file::exists(path)
            # File.append(path, content) -> mlc::file::append_string(path, content)
            # File.read_lines(path) -> mlc::file::read_lines(path)
            FILE_CPP_NAMES = {
              "File_read"       => "mlc::file::read_to_string",
              "File_write"      => "mlc::file::write_string",
              "File_exists"     => "mlc::file::exists",
              "File_append"     => "mlc::file::append_string",
              "File_read_lines" => "mlc::file::read_lines"
            }.freeze

            def lower_file_method(node)
              cpp_name = FILE_CPP_NAMES[node.callee.name]
              args = node.args.map { |arg| lower_expression(arg) }
              seps = Array.new([args.size - 1, 0].max, ", ")
              context.factory.function_call(
                callee: context.factory.identifier(name: cpp_name),
                arguments: args,
                argument_separators: seps
              )
            end

            # Lower map method calls: m.get(k), m.set(k,v), m.has(k), m.remove(k), m.keys(), m.values(), m.size()
            def lower_map_method(call)
              map_obj = wrap_for_member_access(lower_expression(call.callee.object), call.callee.object)
              method = call.callee.member

              case method
              when "get", "has", "remove"
                # Direct member call: m.get(k) -> m.get(k), m.has(k) -> m.has(k), m.remove(k) -> m.remove(k)
                member = context.factory.member_access(
                  object: map_obj,
                  operator: ".",
                  member: context.factory.identifier(name: method)
                )
                args = call.args.map { |arg| lower_expression(arg) }
                context.factory.function_call(
                  callee: member,
                  arguments: args,
                  argument_separators: args.size > 1 ? Array.new(args.size - 1, ", ") : []
                )
              when "set"
                # m.set(k, v) -> m.set(k, v)
                member = context.factory.member_access(
                  object: map_obj,
                  operator: ".",
                  member: context.factory.identifier(name: "set")
                )
                args = call.args.map { |arg| lower_expression(arg) }
                context.factory.function_call(
                  callee: member,
                  arguments: args,
                  argument_separators: [", "]
                )
              when "size", "length"
                member = context.factory.member_access(
                  object: map_obj,
                  operator: ".",
                  member: context.factory.identifier(name: "size")
                )
                context.factory.function_call(callee: member, arguments: [], argument_separators: [])
              when "is_empty"
                member = context.factory.member_access(
                  object: map_obj,
                  operator: ".",
                  member: context.factory.identifier(name: "empty")
                )
                context.factory.function_call(callee: member, arguments: [], argument_separators: [])
              when "keys"
                member = context.factory.member_access(
                  object: map_obj,
                  operator: ".",
                  member: context.factory.identifier(name: "keys")
                )
                context.factory.function_call(callee: member, arguments: [], argument_separators: [])
              when "values"
                member = context.factory.member_access(
                  object: map_obj,
                  operator: ".",
                  member: context.factory.identifier(name: "values")
                )
                context.factory.function_call(callee: member, arguments: [], argument_separators: [])
              else
                raise "Unknown map method: #{method}"
              end
            end

            # Lower string method calls
            # Wrap a lowered expression in parentheses if its SemanticIR source
            # is a compound expression (binary/unary) to preserve operator precedence
            # when used as the object of a member access, e.g. (a + b).length()
            def wrap_for_member_access(lowered, source_node)
              case source_node
              when MLC::SemanticIR::BinaryExpr, MLC::SemanticIR::UnaryExpr
                context.factory.parenthesized(expression: lowered)
              else
                lowered
              end
            end

            def lower_string_method(call)
              method_name = call.callee.member
              string_obj = wrap_for_member_access(
                lower_expression(call.callee.object), call.callee.object
              )

              case method_name
              when "upper", "lower", "trim", "trim_start", "trim_end"
                # str.upper() -> str.upper()
                # str.lower() -> str.lower()
                # str.trim() -> str.trim()
                # str.trim_start() -> str.trim_start()
                # str.trim_end() -> str.trim_end()
                member = context.factory.member_access(
                  object: string_obj,
                  operator: ".",
                  member: context.factory.identifier(name: method_name)
                )
                context.factory.function_call(
                  callee: member,
                  arguments: [],
                  argument_separators: []
                )

              # rubocop:disable Layout/LineLength
              when "split", "chars", "lines", "contains", "starts_with", "ends_with", "substring", "byte_substring", "index_of", "last_index_of", "replace", "char_at", "byte_at", "byte_size", "to_i", "repeat", "reverse", "to_lower", "to_upper", "is_blank", "is_present", "squish", "truncate", "titleize", "camelize", "underscore", "pad_start", "pad_end"
                # rubocop:enable Layout/LineLength
                # String methods that pass through to C++ mlc::String methods
                member = context.factory.member_access(
                  object: string_obj,
                  operator: ".",
                  member: context.factory.identifier(name: method_name)
                )
                args = call.args.map { |arg| lower_expression(arg) }
                context.factory.function_call(
                  callee: member,
                  arguments: args,
                  argument_separators: args.size > 1 ? Array.new(args.size - 1, ", ") : []
                )

              when "length", "len"
                # str.length() / str.len() -> str.length()
                member = context.factory.member_access(
                  object: string_obj,
                  operator: ".",
                  member: context.factory.identifier(name: "length")
                )
                context.factory.function_call(
                  callee: member,
                  arguments: [],
                  argument_separators: []
                )

              when "is_empty"
                # str.is_empty() -> str.is_empty()
                member = context.factory.member_access(
                  object: string_obj,
                  operator: ".",
                  member: context.factory.identifier(name: "is_empty")
                )
                context.factory.function_call(
                  callee: member,
                  arguments: [],
                  argument_separators: []
                )

              else
                # Fallback: call method directly
                member = context.factory.member_access(
                  object: string_obj,
                  operator: ".",
                  member: context.factory.identifier(name: method_name)
                )
                args = call.args.map { |arg| lower_expression(arg) }
                context.factory.function_call(
                  callee: member,
                  arguments: args,
                  argument_separators: args.size > 1 ? Array.new(args.size - 1, ", ") : []
                )
              end
            end

            # Lower an argument, wrapping in std::move() if needed for move-only types (Owned<T>)
            def lower_argument_with_move(arg)
              lowered = lower_expression(arg)

              # Check if argument type is Owned<T> (unique_ptr) which requires std::move
              if requires_move?(arg.type)
                wrap_in_std_move(lowered)
              else
                lowered
              end
            end

            # Check if a type requires std::move() when passed to a function
            def requires_move?(type)
              return false unless type

              # Owned<T> maps to unique_ptr which is move-only
              if type.is_a?(MLC::SemanticIR::GenericType)
                base_name = type.base_type.respond_to?(:name) ? type.base_type.name : nil
                return true if base_name == "Owned"
              end

              false
            end

            # Wrap an expression in std::move()
            def wrap_in_std_move(expr)
              context.factory.function_call(
                callee: context.factory.identifier(name: "std::move"),
                arguments: [expr],
                argument_separators: []
              )
            end

            # Check if type name is numeric
            def numeric_type_name?(name)
              %w[i8 i16 i32 i64 u8 u16 u32 u64 f32 f64 Int Int32 Int64 Float Double].include?(name)
            end

            # Lower numeric method calls
            def lower_numeric_method(call)
              method_name = call.callee.member
              numeric_obj = lower_expression(call.callee.object)

              case method_name
              when "abs"
                # x.abs() -> std::abs(x)
                context.factory.function_call(
                  callee: context.factory.identifier(name: "std::abs"),
                  arguments: [numeric_obj],
                  argument_separators: []
                )

              when "sqrt"
                # x.sqrt() -> std::sqrt(x)
                context.factory.function_call(
                  callee: context.factory.identifier(name: "std::sqrt"),
                  arguments: [numeric_obj],
                  argument_separators: []
                )

              when "floor"
                # x.floor() -> std::floor(x)
                context.factory.function_call(
                  callee: context.factory.identifier(name: "std::floor"),
                  arguments: [numeric_obj],
                  argument_separators: []
                )

              when "ceil"
                # x.ceil() -> std::ceil(x)
                context.factory.function_call(
                  callee: context.factory.identifier(name: "std::ceil"),
                  arguments: [numeric_obj],
                  argument_separators: []
                )

              when "round"
                # x.round() -> std::round(x)
                context.factory.function_call(
                  callee: context.factory.identifier(name: "std::round"),
                  arguments: [numeric_obj],
                  argument_separators: []
                )

              when "sin"
                # x.sin() -> std::sin(x)
                context.factory.function_call(
                  callee: context.factory.identifier(name: "std::sin"),
                  arguments: [numeric_obj],
                  argument_separators: []
                )

              when "cos"
                # x.cos() -> std::cos(x)
                context.factory.function_call(
                  callee: context.factory.identifier(name: "std::cos"),
                  arguments: [numeric_obj],
                  argument_separators: []
                )

              when "tan"
                # x.tan() -> std::tan(x)
                context.factory.function_call(
                  callee: context.factory.identifier(name: "std::tan"),
                  arguments: [numeric_obj],
                  argument_separators: []
                )

              when "exp"
                # x.exp() -> std::exp(x)
                context.factory.function_call(
                  callee: context.factory.identifier(name: "std::exp"),
                  arguments: [numeric_obj],
                  argument_separators: []
                )

              when "log"
                # x.log() -> std::log(x)
                context.factory.function_call(
                  callee: context.factory.identifier(name: "std::log"),
                  arguments: [numeric_obj],
                  argument_separators: []
                )

              when "log10"
                # x.log10() -> std::log10(x)
                context.factory.function_call(
                  callee: context.factory.identifier(name: "std::log10"),
                  arguments: [numeric_obj],
                  argument_separators: []
                )

              when "pow"
                # x.pow(y) -> std::pow(x, y)
                exp_arg = call.args.first ? lower_expression(call.args.first) : context.factory.identifier(name: "1")
                context.factory.function_call(
                  callee: context.factory.identifier(name: "std::pow"),
                  arguments: [numeric_obj, exp_arg],
                  argument_separators: [", "]
                )

              when "min"
                # x.min(y) -> std::min(x, y)
                other_arg = call.args.first ? lower_expression(call.args.first) : numeric_obj
                context.factory.function_call(
                  callee: context.factory.identifier(name: "std::min"),
                  arguments: [numeric_obj, other_arg],
                  argument_separators: [", "]
                )

              when "max"
                # x.max(y) -> std::max(x, y)
                other_arg = call.args.first ? lower_expression(call.args.first) : numeric_obj
                context.factory.function_call(
                  callee: context.factory.identifier(name: "std::max"),
                  arguments: [numeric_obj, other_arg],
                  argument_separators: [", "]
                )

              when "clamp"
                # x.clamp(lo, hi) -> std::clamp(x, lo, hi)
                lo_arg = call.args[0] ? lower_expression(call.args[0]) : context.factory.identifier(name: "0")
                hi_arg = call.args[1] ? lower_expression(call.args[1]) : context.factory.identifier(name: "0")
                context.factory.function_call(
                  callee: context.factory.identifier(name: "std::clamp"),
                  arguments: [numeric_obj, lo_arg, hi_arg],
                  argument_separators: [", ", ", "]
                )

              when "to_f32"
                # x.to_f32() -> static_cast<float>(x)
                context.factory.function_call(
                  callee: context.factory.identifier(name: "static_cast<float>"),
                  arguments: [numeric_obj],
                  argument_separators: []
                )

              when "to_i32"
                # x.to_i32() -> static_cast<int32_t>(x)
                context.factory.function_call(
                  callee: context.factory.identifier(name: "static_cast<int32_t>"),
                  arguments: [numeric_obj],
                  argument_separators: []
                )

              when "to_string"
                # x.to_string() -> mlc::to_string(x)
                context.factory.function_call(
                  callee: context.factory.identifier(name: "mlc::to_string"),
                  arguments: [numeric_obj],
                  argument_separators: []
                )

              else
                # Fallback: method not recognized, generate as is
                member = context.factory.member_access(
                  object: numeric_obj,
                  operator: ".",
                  member: context.factory.identifier(name: method_name)
                )
                args = call.args.map { |arg| lower_expression(arg) }
                context.factory.function_call(
                  callee: member,
                  arguments: args,
                  argument_separators: args.size > 1 ? Array.new(args.size - 1, ", ") : []
                )
              end
            end

            # True when a VarExpr refers to a nullary (no-arg) ADT variant constructor.
            # Mirrors VarRefRule#nullary_variant_constructor? but operates on the raw node.
            def nullary_variant_callee?(var_expr)
              return false unless var_expr.respond_to?(:type) && var_expr.type

              type = var_expr.type
              name = var_expr.name

              if type.is_a?(MLC::SemanticIR::SumType)
                v = type.variants.find { |vt| vt[:name] == name }
                return v && (v[:fields].nil? || v[:fields].empty?)
              end

              return true if type.is_a?(MLC::SemanticIR::RecordType) && type.fields.empty?

              if type.is_a?(MLC::SemanticIR::FunctionType) && type.params.empty?
                ret = type.ret_type
                if ret.is_a?(MLC::SemanticIR::SumType)
                  v = ret.variants.find { |vt| vt[:name] == name }
                  return v && (v[:fields].nil? || v[:fields].empty?)
                end
              end

              false
            end
          end
        end
      end
    end
  end
end
