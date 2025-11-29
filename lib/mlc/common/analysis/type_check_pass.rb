# frozen_string_literal: true


module MLC
  module Common
    module Analysis
      # TypeCheckPass - validates type consistency in SemanticIR
      # Simplified demo: checks function body presence vs external flag.
      # TODO: Fully implement type checking for all SemanticIR node types
      class TypeCheckPass < BasePass
        def initialize(type_registry:, name: "type_check")
          super(name: name)
          @type_registry = type_registry
        end

        def required_keys
          [:core_ir]
        end

        def produced_keys
          [:type_errors, :type_check_passed]
        end

        def run(context)
          core_ir = context[:core_ir]
          return unless core_ir

          @errors = []

          core_ir.items.each do |item|
            next unless item.is_a?(SemanticIR::Func)

            if item.external && item.body
              @errors << "External function '#{item.name}' should not have a body"
            elsif !item.external && !item.body
              @errors << "Function '#{item.name}' must have a body or be declared external"
            end
          end

          context[:type_errors] = @errors
          context[:type_check_passed] = @errors.empty?
        end
      end
    end
  end
end
