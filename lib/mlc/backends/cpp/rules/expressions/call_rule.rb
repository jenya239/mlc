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
              "input" => "mlc::io::read_all",
              "args" => "mlc::io::args",
              "to_string" => "mlc::to_string",
              "format" => "mlc::format"
            }.freeze

            # Stdlib function overrides
            STDLIB_OVERRIDES = {
              "to_f32" => "static_cast<float>"
            }.freeze

            def applies?(node)
              context.checker.call_expr?(node)
            end

            def apply(node)
              # Check for IO functions
              if context.checker.var_expr?(node.callee) && IO_FUNCTIONS.key?(node.callee.name)
                return lower_io_function(node)
              end

              # Check for stdlib overrides
              if context.checker.var_expr?(node.callee)
                override = lower_stdlib_override(node)
                return override if override
              end

              # Check for qualified function names (stdlib functions)
              if context.checker.var_expr?(node.callee)
                qualified = context.resolve_qualified_name(node.callee.name)
                if qualified
                  return lower_qualified_function(node, qualified)
                end
              end

              # Check for array method calls
              if context.checker.member_expr?(node.callee) && node.callee.object.type.is_a?(MLC::SemanticIR::ArrayType)
                return lower_array_method(node)
              end

              # Check for string method calls
              if context.checker.member_expr?(node.callee) &&
                 node.callee.object.type.respond_to?(:primitive?) &&
                 node.callee.object.type.primitive? &&
                 node.callee.object.type.name == "String"
                return lower_string_method(node)
              end

              # Regular function call
              callee = lower_expression(node.callee)
              args = node.args.map { |arg| lower_expression(arg) }

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
              else
                nil
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

            # Lower array method calls
            def lower_array_method(call)
              method_name = call.callee.member
              array_obj = lower_expression(call.callee.object)

              case method_name
              when "length"
                # arr.length() -> arr.size()
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

              when "map"
                # arr.map(f) -> mlc::collections::map(arr, f)
                func_arg = call.args.first ? lower_expression(call.args.first) : nil
                args = [array_obj, func_arg].compact
                context.factory.function_call(
                  callee: context.factory.identifier(name: "mlc::collections::map"),
                  arguments: args,
                  argument_separators: func_arg ? [", "] : []
                )

              when "filter"
                # arr.filter(pred) -> mlc::collections::filter(arr, pred)
                predicate = call.args.first ? lower_expression(call.args.first) : nil
                args = [array_obj, predicate].compact
                context.factory.function_call(
                  callee: context.factory.identifier(name: "mlc::collections::filter"),
                  arguments: args,
                  argument_separators: predicate ? [", "] : []
                )

              when "fold"
                # arr.fold(init, f) -> mlc::collections::fold(arr, init, f)
                init_arg = call.args[0] ? lower_expression(call.args[0]) : nil
                func_arg = call.args[1] ? lower_expression(call.args[1]) : nil
                arguments = [array_obj]
                separators = []
                if init_arg
                  arguments << init_arg
                  separators << ", "
                end
                if func_arg
                  arguments << func_arg
                  separators << ", "
                end
                context.factory.function_call(
                  callee: context.factory.identifier(name: "mlc::collections::fold"),
                  arguments: arguments,
                  argument_separators: separators
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

            # Lower string method calls
            def lower_string_method(call)
              method_name = call.callee.member
              string_obj = lower_expression(call.callee.object)

              case method_name
              when "upper", "lower", "trim"
                # str.upper() -> str.upper()
                # str.lower() -> str.lower()
                # str.trim() -> str.trim()
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

              when "split", "contains", "starts_with", "ends_with"
                # str.split(delim) -> str.split(delim)
                # str.contains(sub) -> str.contains(sub)
                # str.starts_with(prefix) -> str.starts_with(prefix)
                # str.ends_with(suffix) -> str.ends_with(suffix)
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

              when "length"
                # str.length() -> str.length()
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
          end
        end
      end
    end
  end
end
