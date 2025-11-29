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
            def register_trait(name:, type_params:, methods:)
              @traits[name] = TraitInfo.new(
                name: name,
                type_params: type_params,
                methods: methods
              )
            end

            # Register trait implementation for a type
            # @param type_name [String] implementing type
            # @param trait_name [String] trait being implemented (nil for standalone extend)
            # @param methods [Hash<String, MethodInfo>] implemented methods
            def register_implementation(type_name:, trait_name: nil, methods:)
              if trait_name
                key = [type_name, trait_name]
                @implementations[key] = ImplInfo.new(
                  type_name: type_name,
                  trait_name: trait_name,
                  methods: methods
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
              if @type_methods[type_name]&.key?(method_name)
                return @type_methods[type_name][method_name]
              end

              # Then check all implementations for this type
              @implementations.each do |(tn, _trait), impl|
                next unless tn == type_name

                if impl.methods.key?(method_name)
                  return impl.methods[method_name]
                end
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

            # TraitInfo - trait definition
            TraitInfo = Struct.new(:name, :type_params, :methods, keyword_init: true)

            # ImplInfo - trait implementation
            ImplInfo = Struct.new(:type_name, :trait_name, :methods, keyword_init: true)

            # MethodInfo - method/function info
            MethodInfo = Struct.new(:name, :params, :ret_type, :body, :is_static, keyword_init: true)
          end
        end
      end
    end
  end
end
