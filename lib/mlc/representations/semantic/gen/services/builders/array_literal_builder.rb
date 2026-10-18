# frozen_string_literal: true

module MLC
  module Representations
    module Semantic
      module Gen
        module Services
          # ArrayLiteralBuilder - builds SemanticIR array literals and validates element types
          class ArrayLiteralBuilder
            def initialize(ir_builder:, type_checker:)
              @ir_builder = ir_builder
              @type_checker = type_checker
            end

            def build(node:, elements_ir:)
              elements_ir = elements_ir.map { |element| retype_nullary_constructor(element) }
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

            def retype_nullary_constructor(element)
              return element unless element.type.is_a?(SemanticIR::FunctionType)
              return element unless Array(element.type.params).empty?
              return element unless element.is_a?(SemanticIR::VarExpr)
              return element unless element.name.match?(/\A[A-Z]/)

              @type_checker.assign_expression_type(element, element.type.ret_type, update_registry: false)
              element
            end

            def infer_element_type(elements_ir, node)
              concrete = elements_ir.find { |element| !empty_array?(element) }
              if concrete
                ensure_element_type!(concrete, elements_ir.index(concrete), node)
                concrete.type
              elsif elements_ir.empty?
                @ir_builder.prim_type(name: 'i32', origin: node)
              else
                elements_ir.first.type
              end
            end

            def empty_array?(element)
              element.is_a?(SemanticIR::ArrayLiteralExpr) && element.elements.empty?
            end

            def validate_elements(elements_ir, element_type, node)
              elements_ir.each_with_index do |element, index|
                ensure_element_type!(element, index, node)
                if element_type.is_a?(SemanticIR::ArrayType) || element_type.is_a?(SemanticIR::MapType)
                  @type_checker.adopt_empty_container_type(element, element_type)
                end
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
  end
end
