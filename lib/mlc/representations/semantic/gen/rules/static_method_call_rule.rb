# frozen_string_literal: true

module MLC
  module Representations
    module Semantic
      module Gen
        module Rules
          # StaticMethodCallRule - handles Type.method() calls
          # Transforms Type.method(args) -> Type_method(args)
          class StaticMethodCallRule < BaseRule
            # Built-in type constructors: Map.new(), HashMap.new()
            BUILTIN_CONSTRUCTORS = %w[Map HashMap].freeze

            # File I/O methods and their return types
            FILE_METHODS = {
              "read"       => :string,
              "write"      => :unit,
              "exists"     => :bool,
              "append"     => :unit,
              "read_lines" => :string_array
            }.freeze

            def matches?(node, context)
              svc = services(context)

              # Must be a call expression
              return false unless svc.ast_type_checker.call?(node)

              # Check for skip_callee_visit flag (set by expression_visitor for static calls)
              return true if context[:skip_callee_visit]

              callee = node.callee
              # Callee must be member access: Type.method
              return false unless svc.ast_type_checker.member_access?(callee)

              # Object must be a var_ref (the type name)
              return false unless svc.ast_type_checker.var_ref?(callee.object)

              type_name = callee.object.name
              method_name = callee.member

              # If it's a variable in scope, it's an instance method call, not static
              return false if svc.var_type_registry.has?(type_name)

              # Built-in constructors (Map.new, HashMap.new)
              return true if BUILTIN_CONSTRUCTORS.include?(type_name) && method_name == "new"

              # Built-in File methods
              return true if type_name == "File" && FILE_METHODS.key?(method_name)

              # Check if this is a known type with static methods
              static_method_exists?(svc, type_name, method_name)
            end

            def produce(node, context)
              svc = services(context)

              callee = node.callee
              type_name = callee.object.name
              method_name = callee.member

              # Handle built-in constructors
              if BUILTIN_CONSTRUCTORS.include?(type_name) && method_name == "new"
                return produce_builtin_constructor(node, context, svc, type_name)
              end

              # Handle built-in File methods
              if type_name == "File" && FILE_METHODS.key?(method_name)
                return produce_builtin_file_method(node, context, svc, method_name)
              end

              # Build mangled function name: Type_method
              mangled_name = "#{type_name}_#{method_name}"

              # Get args_ir from context (pre-visited by expression_visitor)
              args_ir = context[:args_ir]

              # Get function info
              func_info = svc.function_registry.fetch(mangled_name)

              raise MLC::CompileError, "Static method '#{method_name}' not found on type '#{type_name}'" unless func_info

              # Infer return type (with expected_type for bidirectional inference)
              expected_type = context[:expected_type]
              ret_type = svc.type_inference_service.infer_call_type(
                svc.ir_builder.var(name: mangled_name, type: func_type_for(func_info), origin: node),
                args_ir,
                expected_type: expected_type
              )

              svc.ir_builder.call(
                func: svc.ir_builder.var(name: mangled_name, type: func_type_for(func_info), origin: callee),
                args: args_ir,
                type: ret_type,
                origin: node
              )
            end

            private

            def produce_builtin_constructor(node, context, svc, type_name)
              # Map.new() / HashMap.new() — returns MapType from expected type annotation
              expected_type = context[:expected_type]

              map_type = if expected_type.is_a?(SemanticIR::MapType)
                           expected_type
                         else
                           # Default to Map<string, string> if no annotation
                           svc.ir_builder.map_type(
                             key_type: SemanticIR::Builder.primitive_type("string"),
                             value_type: SemanticIR::Builder.primitive_type("string")
                           )
                         end

              svc.ir_builder.call(
                func: svc.ir_builder.var(name: "#{type_name}_new", type: SemanticIR::Builder.function_type([], map_type), origin: node),
                args: context[:args_ir] || [],
                type: map_type,
                origin: node
              )
            end

            def produce_builtin_file_method(node, context, svc, method_name)
              ret_type = case FILE_METHODS[method_name]
                         when :string       then SemanticIR::Builder.primitive_type("string")
                         when :bool         then SemanticIR::Builder.primitive_type("bool")
                         when :unit         then SemanticIR::UnitType.new
                         when :string_array then SemanticIR::ArrayType.new(element_type: SemanticIR::Builder.primitive_type("string"))
                         end

              mangled = "File_#{method_name}"
              args_ir = context[:args_ir] || []

              svc.ir_builder.call(
                func: svc.ir_builder.var(
                  name: mangled,
                  type: SemanticIR::Builder.function_type([], ret_type),
                  origin: node
                ),
                args: args_ir,
                type: ret_type,
                origin: node
              )
            end

            def static_method_exists?(svc, type_name, method_name)
              # Check trait registry first
              method_info = svc.trait_registry.resolve_static_method(type_name, method_name)
              return true if method_info

              # Check function registry for mangled name
              mangled_name = "#{type_name}_#{method_name}"
              svc.function_registry.fetch(mangled_name)
            end

            def func_type_for(func_info)
              SemanticIR::Builder.function_type(
                func_info.param_types,
                func_info.ret_type
              )
            end
          end
        end
      end
    end
  end
end
