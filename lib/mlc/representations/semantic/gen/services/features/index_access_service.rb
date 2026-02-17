# frozen_string_literal: true

module MLC
  module Representations
    module Semantic
      module Gen
        module Services
          # IndexAccessService - validates and resolves array index accesses
          class IndexAccessService
            def initialize(type_checker:)
              @type_checker = type_checker
            end

            def build(object_ir:, index_ir:, ir_builder:, origin:)
              ensure_indexable!(object_ir.type, origin: origin)
              @type_checker.ensure_numeric_type(index_ir.type, 'index', node: nil)

              element_type = if object_ir.type.is_a?(MLC::SemanticIR::ArrayType)
                               object_ir.type.element_type
                             elsif string_type?(object_ir.type)
                               MLC::SemanticIR::Builder.primitive_type("char")
                             else
                               @type_checker.type_error("Unexpected indexable type", node: origin)
                             end

              ir_builder.index(
                object: object_ir,
                index: index_ir,
                type: element_type,
                origin: origin
              )
            end

            private

            def ensure_indexable!(type, origin:)
              return if type.is_a?(MLC::SemanticIR::ArrayType)
              return if string_type?(type)

              message = "Indexing requires an array or string, got #{@type_checker.describe_type(type)}"
              @type_checker.type_error(message, node: origin)
            end

            def string_type?(type)
              type.is_a?(MLC::SemanticIR::Type) && type.kind == :prim && type.name == "string"
            end
          end
        end
      end
    end
  end
end
