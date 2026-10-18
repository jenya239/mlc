# frozen_string_literal: true

module MLC
  module Backends
    module Cpp
      # Maps trait signature AST types to C++ for vtable structs and adapters.
      module TraitCppTypes
        module_function

        PRIMITIVE_CPP = {
          "unit" => "void", "void" => "void", "string" => "mlc::String", "str" => "mlc::String",
          "bool" => "bool", "i32" => "int", "i64" => "int64_t", "i8" => "int8_t",
          "u32" => "uint32_t", "u64" => "uint64_t", "f32" => "float", "f64" => "double",
          "int" => "int"
        }.freeze

        def trait_method_has_self_receiver?(method_entry)
          first = Array(method_entry[:params]).first
          return false unless first

          type_syntax = first.respond_to?(:type) ? first.type : first[:type]
          type_syntax.is_a?(MLC::Source::AST::Type) && type_syntax.name == "Self"
        end

        def vtable_std_function_parameter_types(
          method_entry,
          trait_self_name:,
          trait_self_keyword:,
          associated_type_names:,
          trait_type_param_names: nil,
          type_registry: nil
        )
          if trait_self_keyword && trait_method_has_self_receiver?(method_entry)
            return trait_self_name
          end

          Array(method_entry[:params])
            .reject { |parameter| parameter.respond_to?(:name) ? parameter.name == "self" : parameter[:name] == "self" }
            .map do |parameter|
              type_syntax = parameter.respond_to?(:type) ? parameter.type : parameter[:type]
              ast_type_to_cpp(
                type_syntax,
                trait_self: trait_self_keyword,
                associated_type_names: associated_type_names,
                trait_type_param_names: trait_type_param_names,
                type_registry: type_registry
              )
            end.join(", ")
        end

        # @param trait_self [String, nil] when set, Self / associated refs use CRTP-style TraitSelf
        # @param associated_type_names [Set, nil] bare names declared in the trait (`type Item`)
        def ast_type_to_cpp(ast_type, trait_self: nil, associated_type_names: nil, trait_type_param_names: nil, type_registry: nil)
          return "void" unless ast_type

          if trait_self.is_a?(String) && !trait_self.empty?
            case ast_type
            when MLC::Source::AST::AssocTypeRef
              return "typename #{trait_self}::#{ast_type.assoc_name}"
            when MLC::Source::AST::Type
              return trait_self if ast_type.name == "Self"
              if associated_type_names&.include?(ast_type.name.to_s)
                return "typename #{trait_self}::#{ast_type.name}"
              end
            end
          end

          case ast_type
          when MLC::Source::AST::AssocTypeRef
            ast_type.name.to_s
          when MLC::Source::AST::ArrayType
            inner = ast_type_to_cpp(ast_type.element_type, trait_self: trait_self, trait_type_param_names: trait_type_param_names, type_registry: type_registry)
            "mlc::Array<#{inner}>"
          when MLC::Source::AST::GenericType
            base_name = ast_type.base_type.respond_to?(:name) ? ast_type.base_type.name.to_s : ast_type.base_type.to_s
            args = Array(ast_type.type_params).map { |t| ast_type_to_cpp(t, trait_self: trait_self, trait_type_param_names: trait_type_param_names, type_registry: type_registry) }
            if base_name == "Shared" && args.length == 1
              "std::shared_ptr<#{args.first}>"
            else
              "#{base_name}<#{args.join(', ')}>"
            end
          when MLC::Source::AST::Type
            n = ast_type.name.to_s
            return n if trait_type_param_names&.include?(n)
            return PRIMITIVE_CPP[n] if PRIMITIVE_CPP.key?(n)

            qualify_user_type(n, type_registry)
          else
            n = ast_type.respond_to?(:name) ? ast_type.name.to_s : ast_type.to_s
            return PRIMITIVE_CPP[n] if PRIMITIVE_CPP.key?(n)

            qualify_user_type(n, type_registry)
          end
        end

        def qualify_user_type(name, type_registry)
          return name unless type_registry

          qualified = type_registry.cpp_name(name)
          qualified.nil? || qualified.empty? ? name : qualified
        end
      end
    end
  end
end
