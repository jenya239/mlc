# frozen_string_literal: true

module MLC
  module Representations
    module Semantic
      module Gen
        module Services
          # TraitRegistry - Stores trait definitions and implementations
          # Enables lookup of associated functions for Type.method() calls
          class TraitRegistry
            def initialize
              # trait_name -> TraitInfo
              @traits = {}

              # (type_name, trait_name) -> ImplInfo
              @implementations = {}

              # type_name -> [associated functions without trait]
              @type_methods = {}
            end

            # Register a trait definition
            # @param name [String] trait name
            # @param type_params [Array<String>] generic params
            # @param methods [Array<Hash>] method signatures
            # @param associated_types [Array<Hash>] associated type declarations
            def register_trait(name:, type_params:, methods:, associated_types: [])
              @traits[name] = TraitInfo.new(
                name: name,
                type_params: type_params,
                trait_methods: methods,
                associated_types: associated_types
              )
            end

            # Register trait implementation for a type
            # @param type_name [String] implementing type
            # @param trait_name [String] trait being implemented (nil for standalone extend)
            # @param methods [Hash<String, MethodInfo>] implemented methods
            # @param associated_type_bindings [Hash<String, Type>] associated type bindings
            def register_implementation(type_name:, methods:, trait_name: nil, associated_type_bindings: {})
              if trait_name
                key = [type_name, trait_name]
                @implementations[key] = ImplInfo.new(
                  type_name: type_name,
                  trait_name: trait_name,
                  impl_methods: methods,
                  associated_type_bindings: associated_type_bindings
                )
              else
                # Methods without trait - add to type_methods
                @type_methods[type_name] ||= {}
                methods.each do |method_name, method_info|
                  @type_methods[type_name][method_name] = method_info
                end
              end
            end

            # Lookup trait by name
            def get_trait(name)
              @traits[name]
            end

            # Check if type implements trait
            def implements?(type_name, trait_name)
              @implementations.key?([type_name, trait_name])
            end

            # Get implementation for type + trait
            def get_implementation(type_name, trait_name)
              @implementations[[type_name, trait_name]]
            end

            # Resolve associated function call: Type.method()
            # Returns MethodInfo or nil
            def resolve_static_method(type_name, method_name)
              # First check type's own methods (from extend without trait)
              return @type_methods[type_name][method_name] if @type_methods[type_name]&.key?(method_name)

              # Then check all implementations for this type
              @implementations.each do |(tn, _trait), impl|
                next unless tn == type_name

                return impl.impl_methods[method_name] if impl.impl_methods.key?(method_name)
              end

              nil
            end

            # Resolve instance method call: obj.method() where first param is "self"
            # Returns MethodInfo or nil
            def resolve_instance_method(type_name, method_name)
              if @type_methods[type_name]&.key?(method_name)
                mi = @type_methods[type_name][method_name]
                return mi if instance_method?(mi)
              end

              @implementations.each do |(tn, _trait), impl|
                next unless tn == type_name
                next unless impl.impl_methods.key?(method_name)

                mi = impl.impl_methods[method_name]
                return mi if instance_method?(mi)
              end

              nil
            end

            # Get all traits
            def all_traits
              @traits.values
            end

            # Get all implementations for a type
            def implementations_for(type_name)
              @implementations.select { |(tn, _), _| tn == type_name }.values
            end

            # Get all implementations for a trait (across all types)
            def implementations_for_trait(trait_name)
              @implementations.select { |(_, tn), _| tn == trait_name }.values
            end

            # TraitInfo - trait definition
            TraitInfo = Struct.new(:name, :type_params, :trait_methods, :associated_types, keyword_init: true)

            # ImplInfo - trait implementation
            ImplInfo = Struct.new(:type_name, :trait_name, :impl_methods, :associated_type_bindings, keyword_init: true)

            # MethodInfo - method/function info
            MethodInfo = Struct.new(:name, :params, :ret_type, :body, :is_static, keyword_init: true)

            private

            def instance_method?(method_info)
              return false unless method_info.params&.any?

              first = method_info.params.first
              first.respond_to?(:name) && first.name == "self"
            end
          end
        end
      end
    end
  end
end
