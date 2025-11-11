# frozen_string_literal: true

module MLC
  module Backends
    module Cpp
      # Dependency injection container for backend services
      class Container
        attr_reader :type_registry, :function_registry

        def initialize(type_registry:, function_registry:)
          @type_registry = type_registry
          @function_registry = function_registry
          @services = {}
        end

        # Lazy service initialization
        def resolve(service_name)
          @services[service_name] ||= create_service(service_name)
        end

        private

        def create_service(service_name)
          # Services will be registered here as they are implemented
          # Example:
          # when :string_helpers
          #   Services::Utils::StringHelpers.new
          raise "Unknown service: #{service_name}"
        end
      end
    end
  end
end
