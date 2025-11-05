# frozen_string_literal: true

module MLC
  module IRGenV2
    module Services
      # ArrayLiteralBuilder - builds HighIR array literals and validates element types
      class ArrayLiteralBuilder
        def initialize(ir_builder:, type_checker:)
          @ir_builder = ir_builder
          @type_checker = type_checker
        end

        def build(node:, elements_ir:)
          element_type = infer_element_type(elements_ir, node)
          validate_elements(elements_ir, element_type, node)

          array_type = @ir_builder.array_type(element_type: element_type)

          @ir_builder.array_literal(
            elements: elements_ir,
            type: array_type,
            origin: node
          )
        end

        private

        def infer_element_type(elements_ir, node)
          if elements_ir.empty?
            @ir_builder.prim_type(name: 'i32', origin: node)
          else
            first = elements_ir.first
            ensure_element_type!(first, 0, node)
            first.type
          end
        end

        def validate_elements(elements_ir, element_type, node)
          elements_ir.each_with_index do |element, index|
            ensure_element_type!(element, index, node)
            next if index.zero?

            @type_checker.ensure_compatible_type(
              element.type,
              element_type,
              "array element #{index + 1}",
              node: node
            )
          end
        end

        def ensure_element_type!(element, index, node)
          @type_checker.ensure_type!(
            element.type,
            "array element #{index + 1} has unknown type",
            node: node
          )
        end
      end
    end
  end
end
