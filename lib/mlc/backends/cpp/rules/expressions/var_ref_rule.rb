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

                if ctor_name != identifier # Is a qualified constructor (contains ::)
                  if nullary_variant_constructor?(node)
                    context.factory.raw_expression(code: "(#{ctor_name}{})")
                  elsif variant_constructor?(node)
                    context.factory.raw_expression(code: "(#{ctor_name})")
                  else
                    context.factory.identifier(name: ctor_name)
                  end
                elsif nullary_variant_constructor?(node)
                  context.factory.raw_expression(code: "#{identifier}{}")
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
              base_name = type && (type.is_a?(MLC::SemanticIR::SumType) ? type.name : (type.is_a?(MLC::SemanticIR::FunctionType) && type.ret_type.respond_to?(:name) ? type.ret_type.name : nil))

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
                return ret.is_a?(MLC::SemanticIR::SumType) && ret.variants.any? { |v| v[:name] == node.name }
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

              # SumType - check if the variant with this name has no fields
              if type.is_a?(MLC::SemanticIR::SumType)
                variant = type.variants.find { |v| v[:name] == node.name }
                return variant && (variant[:fields].nil? || variant[:fields].empty?)
              end

              # RecordType with no fields is also a nullary variant
              return true if type.is_a?(MLC::SemanticIR::RecordType) && type.fields.empty?

              # FunctionType with no params whose return is a SumType containing
              # a variant with this name and no fields (nullary constructor coerced to fn)
              if type.is_a?(MLC::SemanticIR::FunctionType) && type.params.empty?
                ret = type.ret_type
                if ret.is_a?(MLC::SemanticIR::SumType)
                  variant = ret.variants.find { |v| v[:name] == node.name }
                  return variant && (variant[:fields].nil? || variant[:fields].empty?)
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
