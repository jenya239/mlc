# frozen_string_literal: true

module MLC
  module Representations
    module Semantic
      module Gen
        module Services
          # OperatorTraitMapper - Maps binary operators to trait methods
          # Used for operator overloading: Vec2 + Vec2 -> Vec2.add(a, b)
          #
          # Operator to trait/method mapping:
          #   +   -> Add.add
          #   -   -> Sub.sub
          #   *   -> Mul.mul
          #   /   -> Div.div
          #   %   -> Rem.rem
          #   ==  -> Eq.eq
          #   !=  -> Eq.ne (derived from eq)
          #   <   -> Ord.lt
          #   >   -> Ord.gt
          #   <=  -> Ord.le
          #   >=  -> Ord.ge
          #   &   -> BitAnd.bitand
          #   |   -> BitOr.bitor
          #   ^   -> BitXor.bitxor
          #   <<  -> Shl.shl
          #   >>  -> Shr.shr
          class OperatorTraitMapper
            # Operator -> [trait_name, method_name]
            OPERATOR_TRAITS = {
              "+" => ["Add", "add"],
              "-" => ["Sub", "sub"],
              "*" => ["Mul", "mul"],
              "/" => ["Div", "div"],
              "%" => ["Rem", "rem"],
              "==" => ["Eq", "eq"],
              "!=" => ["Eq", "ne"],
              "<" => ["Ord", "lt"],
              ">" => ["Ord", "gt"],
              "<=" => ["Ord", "le"],
              ">=" => ["Ord", "ge"],
              "&" => ["BitAnd", "bitand"],
              "|" => ["BitOr", "bitor"],
              "^" => ["BitXor", "bitxor"],
              "<<" => ["Shl", "shl"],
              ">>" => ["Shr", "shr"]
            }.freeze

            # Primitive types that use native C++ operators
            PRIMITIVE_TYPES = %w[
              i8 i16 i32 i64
              u8 u16 u32 u64
              f32 f64
              bool
              char
            ].freeze

            def initialize(trait_registry)
              @trait_registry = trait_registry
            end

            # Check if operator can be overloaded for this type
            # @param op [String] operator
            # @param type [SemanticIR::Type] left operand type
            # @return [Boolean]
            def overloadable?(op, type) # rubocop:disable Naming/MethodParameterName
              return false unless OPERATOR_TRAITS.key?(op)
              return false if primitive_type?(type)

              type_name = extract_type_name(type)
              return false unless type_name

              trait_name, method_name = OPERATOR_TRAITS[op]

              # Check if type has operator trait implementation
              @trait_registry.implements?(type_name, trait_name) ||
                has_method_directly?(type_name, method_name)
            end

            # Get method info for operator on type
            # @param op [String] operator
            # @param type [SemanticIR::Type] left operand type
            # @return [Hash, nil] { trait_name:, method_name:, mangled_name: }
            def resolve(op, type) # rubocop:disable Naming/MethodParameterName
              return nil unless OPERATOR_TRAITS.key?(op)

              type_name = extract_type_name(type)
              return nil unless type_name

              trait_name, method_name = OPERATOR_TRAITS[op]

              # Try trait implementation first
              if @trait_registry.implements?(type_name, trait_name)
                impl = @trait_registry.get_implementation(type_name, trait_name)
                return nil unless impl&.impl_methods&.key?(method_name)

                {
                  trait_name: trait_name,
                  method_name: method_name,
                  mangled_name: "#{type_name}_#{method_name}"
                }
              # Fallback: check for method without trait
              elsif has_method_directly?(type_name, method_name)
                {
                  trait_name: nil,
                  method_name: method_name,
                  mangled_name: "#{type_name}_#{method_name}"
                }
              end
            end

            # Get the Output associated type for an overloaded operator
            # @param op [String] operator
            # @param type [SemanticIR::Type] left operand type
            # @param type_builder [TypeBuilder] for AST->SemanticIR conversion
            # @return [SemanticIR::Type, nil]
            def output_type(op, type, type_builder) # rubocop:disable Naming/MethodParameterName
              return nil unless OPERATOR_TRAITS.key?(op)

              type_name = extract_type_name(type)
              return nil unless type_name

              trait_name, = OPERATOR_TRAITS[op]
              impl = @trait_registry.get_implementation(type_name, trait_name)
              return nil unless impl&.associated_type_bindings

              ast_output = impl.associated_type_bindings["Output"]
              return nil unless ast_output

              type_builder.transform(ast_output)
            rescue StandardError
              nil
            end

            # Get trait name for operator
            def trait_for(op) # rubocop:disable Naming/MethodParameterName
              OPERATOR_TRAITS.dig(op, 0)
            end

            # Get method name for operator
            def method_for(op) # rubocop:disable Naming/MethodParameterName
              OPERATOR_TRAITS.dig(op, 1)
            end

            private

            def primitive_type?(type)
              return false unless type

              name = extract_type_name(type)
              PRIMITIVE_TYPES.include?(name)
            end

            def extract_type_name(type)
              case type
              when MLC::SemanticIR::Type
                type.name
              when String
                type
              else
                type&.name if type.respond_to?(:name)
              end
            end

            def method_directly?(type_name, method_name)
              method_info = @trait_registry.resolve_static_method(type_name, method_name)
              !method_info.nil?
            end

            alias has_method_directly? method_directly?
          end
        end
      end
    end
  end
end
