# frozen_string_literal: true

module MLC
  module Backends
    module Cpp
      module Rules
        module Expressions
          # Rule for lowering SemanticIR::VarExpr to C++ identifiers
          # Handles variable references and boolean literals (true/false)
          class VarRefRule < ExpressionRule
            def applies?(node)
              context.checker.var_expr?(node)
            end

            def apply(node)
              case node.name
              when "true"
                context.factory.boolean_literal(value: true)
              when "false"
                context.factory.boolean_literal(value: false)
              else
                identifier = context.sanitize_identifier(node.name)
                ctor_name = qualified_constructor_name(node)
                targs = generic_variant_template_args(node)

                if ctor_name != identifier # Is a qualified constructor (contains ::)
                  if nullary_variant_constructor?(node)
                    context.factory.raw_expression(code: "(#{ctor_name}#{targs}{})")
                  elsif variant_constructor?(node)
                    context.factory.raw_expression(code: "(#{ctor_name}#{targs})")
                  else
                    context.factory.identifier(name: ctor_name)
                  end
                elsif nullary_variant_constructor?(node)
                  context.factory.raw_expression(code: "#{identifier}#{targs}{}")
                elsif variant_constructor?(node)
                  context.factory.identifier(name: identifier)
                else
                  context.factory.identifier(name: identifier)
                end
              end
            end

            private

            def qualified_constructor_name(node)
              name = context.sanitize_identifier(node.name)
              return name if context.user_function?(name)
              return name unless name.match?(/\A[A-Z]/) # Constructor convention

              type = node.respond_to?(:type) && node.type ? node.type : nil
              base_name = if type.is_a?(MLC::SemanticIR::SumType)
                            type.name
                          elsif type.is_a?(MLC::SemanticIR::GenericType)
                            type.base_type&.name
                          elsif type.is_a?(MLC::SemanticIR::FunctionType) && type.ret_type.respond_to?(:name)
                            type.ret_type.name
                          end

              info = (base_name && context.type_registry&.lookup(base_name)) || context.type_registry&.find_type_with_variant(name)
              ns = nil
              if info
                ns = info.namespace || (info.cpp_name[/\A([^:]+)::/, 1] if info.cpp_name&.include?("::")) || (module_name_to_namespace(info.module_name) if info.module_name)
              else
                entry = context.function_registry&.fetch_entry(name)
                ns = entry&.namespace || (module_name_to_namespace(entry&.module_name) if entry&.module_name)
              end
              return name unless ns

              "#{ns}::#{name}"
            end

            def variant_constructor?(node)
              return false unless node.respond_to?(:type) && node.type

              type = node.type
              if type.is_a?(MLC::SemanticIR::SumType)
                return type.variants.any? { |v| v[:name] == node.name }
              end
              if type.is_a?(MLC::SemanticIR::FunctionType)
                ret = type.ret_type
                if ret.is_a?(MLC::SemanticIR::SumType)
                  return ret.variants.any? { |v| v[:name] == node.name }
                end
                if ret.is_a?(MLC::SemanticIR::GenericType)
                  base = ret.base_type
                  return base.is_a?(MLC::SemanticIR::SumType) && base.variants.any? { |v| v[:name] == node.name }
                end
              end
              if type.is_a?(MLC::SemanticIR::GenericType)
                base = type.base_type
                return base.is_a?(MLC::SemanticIR::SumType) && base.variants.any? { |v| v[:name] == node.name }
              end
              false
            end

            def module_name_to_namespace(name)
              return nil unless name && !name.empty?
              base = name.gsub("/", "::").split("::").map(&:downcase).join("::")
              base == "main" ? "mlc_main" : base
            end

            def nullary_variant_constructor?(node)
              return false unless node.respond_to?(:type) && node.type

              type = node.type

              if type.is_a?(MLC::SemanticIR::SumType)
                variant = type.variants.find { |v| v[:name] == node.name }
                return variant && (variant[:fields].nil? || variant[:fields].empty?)
              end

              return true if type.is_a?(MLC::SemanticIR::RecordType) && type.fields.empty?

              if type.is_a?(MLC::SemanticIR::FunctionType) && type.params.empty?
                ret = type.ret_type
                sum = if ret.is_a?(MLC::SemanticIR::SumType) then ret
                      elsif ret.is_a?(MLC::SemanticIR::GenericType) && ret.base_type.is_a?(MLC::SemanticIR::SumType) then ret.base_type
                      end
                if sum
                  variant = sum.variants.find { |v| v[:name] == node.name }
                  return variant && (variant[:fields].nil? || variant[:fields].empty?)
                end
              end

              if type.is_a?(MLC::SemanticIR::GenericType)
                sum = resolve_sum_type(type.base_type)
                if sum
                  variant = sum.variants.find { |v| v[:name] == node.name }
                  return variant && (variant[:fields].nil? || variant[:fields].empty?)
                end
              end

              false
            end

            # Returns template args string like "<int>" for variants of generic ADTs, else ""
            def generic_variant_template_args(node)
              return "" unless node.respond_to?(:type) && node.type

              type = node.type

              if type.is_a?(MLC::SemanticIR::GenericType)
                sum = resolve_sum_type(type.base_type)
                if sum
                  variant = sum.variants.find { |v| v[:name] == node.name }
                  return "" unless variant
                  return "" if type.type_args.empty?
                  resolved = resolve_type_args(type.type_args, type.base_type)
                  cpp_args = resolved.map { |t| context.map_type(t) }
                  return "<#{cpp_args.join(", ")}>"
                end
              end

              if type.is_a?(MLC::SemanticIR::FunctionType)
                ret = type.ret_type
                if ret.is_a?(MLC::SemanticIR::GenericType)
                  sum = resolve_sum_type(ret.base_type)
                  if sum
                    variant = sum.variants.find { |v| v[:name] == node.name }
                    return "" unless variant
                    return "" if ret.type_args.empty?
                    resolved = resolve_type_args(ret.type_args, ret.base_type)
                    cpp_args = resolved.map { |t| context.map_type(t) }
                    return "<#{cpp_args.join(", ")}>"
                  end
                end
              end

              ""
            end

            # Resolve a potential SumType from a base_type (which may be PrimitiveType or actual SumType)
            def resolve_sum_type(base)
              return base if base.is_a?(MLC::SemanticIR::SumType)
              return nil unless base.respond_to?(:name) && base.name

              info = context.type_registry&.lookup(base.name)
              return nil unless info

              core = info.respond_to?(:core_ir_type) ? info.core_ir_type : nil
              core.is_a?(MLC::SemanticIR::SumType) ? core : nil
            end

            # Resolve TypeVariable args using expected_return_type from context
            def resolve_type_args(type_args, base_type)
              return type_args unless type_args.any? { |a| a.is_a?(MLC::SemanticIR::TypeVariable) }

              expected = context.expected_return_type
              return type_args unless expected.is_a?(MLC::SemanticIR::GenericType)
              return type_args unless expected.base_type.respond_to?(:name) && base_type.respond_to?(:name) &&
                                      expected.base_type.name == base_type.name
              return type_args if expected.type_args.any? { |a| a.is_a?(MLC::SemanticIR::TypeVariable) }
              return type_args unless expected.type_args.size == type_args.size

              subst = {}
              type_args.each_with_index do |arg, i|
                subst[arg.name] = expected.type_args[i] if arg.is_a?(MLC::SemanticIR::TypeVariable)
              end
              type_args.map { |a| a.is_a?(MLC::SemanticIR::TypeVariable) ? (subst[a.name] || a) : a }
            end
          end
        end
      end
    end
  end
end
