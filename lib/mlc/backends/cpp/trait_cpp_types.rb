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

        # @param trait_self [String, nil] when set, Self / associated refs use CRTP-style TraitSelf
        # @param associated_type_names [Set, nil] bare names declared in the trait (`type Item`)
        def ast_type_to_cpp(ast_type, trait_self: nil, associated_type_names: nil)
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
            inner = ast_type_to_cpp(ast_type.element_type, trait_self: trait_self)
            "mlc::Array<#{inner}>"
          when MLC::Source::AST::GenericType
            base_name = ast_type.base_type.respond_to?(:name) ? ast_type.base_type.name.to_s : ast_type.base_type.to_s
            args = Array(ast_type.type_params).map { |t| ast_type_to_cpp(t, trait_self: trait_self) }
            "#{base_name}<#{args.join(', ')}>"
          when MLC::Source::AST::Type
            n = ast_type.name.to_s
            PRIMITIVE_CPP[n] || n
          else
            n = ast_type.respond_to?(:name) ? ast_type.name.to_s : ast_type.to_s
            PRIMITIVE_CPP[n] || n
          end
        end
      end
    end
  end
end
