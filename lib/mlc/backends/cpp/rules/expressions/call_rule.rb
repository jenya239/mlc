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

              if context.checker.member_expr?(node.callee) &&
                 node.callee.object.type.is_a?(MLC::SemanticIR::GenericType) &&
                 %w[Result Option].include?(node.callee.object.type.base_type&.name)
                return lower_result_option_combinator(node)
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
              callee = maybe_add_variant_template_args(callee, node)
              callee_param_types = node.callee.respond_to?(:type) && node.callee.type.is_a?(MLC::SemanticIR::FunctionType) ? node.callee.type.params : []
              args = node.args.each_with_index.map do |arg, i|
                lowered = lower_argument_with_move(arg)
                param = callee_param_types[i]
                expected_type = param.is_a?(Hash) ? param[:type] : param
                maybe_wrap_trait_coercion(lowered, arg.type, expected_type)
              end

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
            IO_STRING_FUNCTIONS = %w[print println eprint eprintln].freeze

            def lower_io_function(call)
              target = IO_FUNCTIONS[call.callee.name]
              callee = context.factory.identifier(name: target)
              needs_string = IO_STRING_FUNCTIONS.include?(call.callee.name)
              args = call.args.each_with_index.map do |arg, _i|
                lowered = lower_expression(arg)
                if needs_string && arg.respond_to?(:type) && arg.type && !io_string_type?(arg.type)
                  context.factory.raw_expression(code: "mlc::to_string(#{lowered.to_source})")
                else
                  lowered
                end
              end

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
              concrete_type = resolve_concrete_generic_type(call)
              if concrete_type
                base = concrete_type.base_type
                if base.is_a?(MLC::SemanticIR::SumType)
                  variant_name = call.callee.respond_to?(:name) ? call.callee.name : nil
                  variant = variant_name && base.variants.find { |v| v[:name] == variant_name }
                  if variant && !(variant[:fields].nil? || variant[:fields].empty?)
                    all_names = collect_type_var_names_ordered(base)
                    used = collect_type_var_names_ordered_from_fields(variant[:fields] || [])
                    cpp_args = used.map do |vname|
                      idx = all_names.index(vname)
                      idx ? context.map_type(concrete_type.type_args[idx]) : vname
                    end
                    qualified_name = "#{qualified_name}<#{cpp_args.join(", ")}>" unless cpp_args.empty?
                  end
                end
              end
              callee = context.factory.identifier(name: qualified_name)
              args = call.args.map { |arg| lower_expression(arg) }

              context.factory.function_call(
                callee: callee,
                arguments: args,
                argument_separators: args.size > 1 ? Array.new(args.size - 1, ", ") : []
              )
            end

            def lower_result_option_combinator(call)
              method_name = call.callee.member
              base = call.callee.object.type.base_type&.name
              obj = wrap_for_member_access(lower_expression(call.callee.object), call.callee.object)
              cpp_ns = base == "Option" ? "mlc::opt::" : "mlc::result::"
              cpp_fn = (base == "Result" && method_name == "ok") ? "ok_into_optional" : method_name
              callee = context.factory.identifier(name: "#{cpp_ns}#{cpp_fn}")
              rest = call.args.map { |arg| lower_argument_with_move(arg) }
              args = [obj] + rest
              context.factory.function_call(
                callee: callee,
                arguments: args,
                argument_separators: args.size > 1 ? ::Array.new(args.size - 1, ", ") : []
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
                   "contains", "index_of", "sort", "enumerate",
                   "concat", "append", "zip", "sort_by", "take", "drop",
                   "each",
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

            # Wrap argument with trait coercion adapter if expected type is a trait
            # Add template args to variant constructor calls for generic ADTs.
            # Handles both fully-resolved types and TypeVariable-containing types by substituting
            # from the enclosing function's expected_return_type.
            def maybe_add_variant_template_args(callee_expr, call_node)
              return callee_expr unless call_node.respond_to?(:callee)

              callee_ir = call_node.callee
              return callee_expr unless callee_ir.respond_to?(:name)

              name = callee_ir.name
              return callee_expr unless name.match?(/\A[A-Z]/) # Constructor convention

              result_type = resolve_concrete_generic_type(call_node)
              return callee_expr unless result_type

              base = result_type.base_type
              return callee_expr unless base.is_a?(MLC::SemanticIR::SumType)

              variant = base.variants.find { |v| v[:name] == name }
              return callee_expr unless variant

              # Only include type args actually used by this variant's fields (per-variant params).
              all_param_names = collect_type_var_names_ordered(base)
              used_names = collect_type_var_names_ordered_from_fields(variant[:fields] || [])
              cpp_args = used_names.map do |vname|
                idx = all_param_names.index(vname)
                idx ? context.map_type(result_type.type_args[idx]) : vname
              end

              targs = cpp_args.empty? ? "" : "<#{cpp_args.join(", ")}>"
              context.factory.raw_expression(code: "#{callee_expr.to_source}#{targs}")
            end

            # Resolve concrete generic type for a call node, substituting TypeVariables
            # from expected_return_type when needed.
            def resolve_concrete_generic_type(call_node)
              result_type = call_node.respond_to?(:type) && call_node.type ? call_node.type : nil
              return nil unless result_type.is_a?(MLC::SemanticIR::GenericType)

              # If all concrete, use directly
              return result_type unless result_type.type_args.any? { |a| a.is_a?(MLC::SemanticIR::TypeVariable) }

              # Try to substitute from expected_return_type
              expected = context.expected_return_type
              return nil unless expected.is_a?(MLC::SemanticIR::GenericType)
              return nil unless expected.base_type == result_type.base_type ||
                                (expected.respond_to?(:name) && result_type.respond_to?(:name) && expected.name == result_type.name)
              return nil if expected.type_args.any? { |a| a.is_a?(MLC::SemanticIR::TypeVariable) }
              return nil unless expected.type_args.size == result_type.type_args.size

              # Build substitution map TypeVar.name → concrete type
              subst = {}
              result_type.type_args.each_with_index do |arg, i|
                subst[arg.name] = expected.type_args[i] if arg.is_a?(MLC::SemanticIR::TypeVariable)
              end

              resolved_args = result_type.type_args.map do |arg|
                arg.is_a?(MLC::SemanticIR::TypeVariable) ? (subst[arg.name] || arg) : arg
              end

              # Return only if all args are now concrete
              return nil if resolved_args.any? { |a| a.is_a?(MLC::SemanticIR::TypeVariable) }

              MLC::SemanticIR::GenericType.new(
                base_type: result_type.base_type,
                type_args: resolved_args
              )
            end

            # Collect TypeVariable names from all variants' fields in declaration order.
            def collect_type_var_names_ordered(sum_type)
              seen = []
              sum_type.variants.each do |v|
                collect_type_var_names_from_fields(v[:fields] || [], seen)
              end
              seen
            end

            # Collect TypeVariable names from a specific variant's fields.
            def collect_type_var_names_ordered_from_fields(fields)
              seen = []
              collect_type_var_names_from_fields(fields, seen)
              seen
            end

            def collect_type_var_names_from_fields(fields, seen)
              Array(fields).each do |f|
                collect_type_var_names_from_type(f[:type], seen) if f[:type]
              end
            end

            def collect_type_var_names_from_type(type, seen)
              if type.is_a?(MLC::SemanticIR::TypeVariable)
                seen << type.name unless seen.include?(type.name)
              elsif type.respond_to?(:type_args)
                type.type_args.each { |a| collect_type_var_names_from_type(a, seen) }
              end
            end

            def maybe_wrap_trait_coercion(lowered_arg, actual_type, expected_type)
              return lowered_arg unless expected_type && actual_type
              return lowered_arg unless context.trait_registry

              expected_name = expected_type.respond_to?(:name) ? expected_type.name : nil
              return lowered_arg unless expected_name
              return lowered_arg unless context.trait_registry.get_trait(expected_name)

              actual_name = actual_type.respond_to?(:name) ? actual_type.name : nil
              return lowered_arg unless actual_name
              return lowered_arg if actual_name == expected_name

              adapter_name = "#{actual_name}_as_#{expected_name}"
              context.factory.function_call(
                callee: context.factory.identifier(name: adapter_name),
                arguments: [lowered_arg],
                argument_separators: []
              )
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

            def io_string_type?(type)
              return true if type.is_a?(MLC::SemanticIR::SymbolType)
              return false unless type.respond_to?(:name)
              %w[string str String mlc::String].include?(type.name)
            end
          end
        end
      end
    end
  end
end
