# frozen_string_literal: true

module MLC
  module SemanticGen
    module Services
      # IndexAccessService - validates and resolves array index accesses
      class IndexAccessService
        def initialize(type_checker:)
          @type_checker = type_checker
        end

        def build(object_ir:, index_ir:, ir_builder:, origin:)
          ensure_array!(object_ir.type, origin: origin)
          @type_checker.ensure_numeric_type(index_ir.type, 'array index', node: nil)

          element_type = object_ir.type.element_type
          ir_builder.index(
            object: object_ir,
            index: index_ir,
            type: element_type,
            origin: origin
          )
        end

        private

        def ensure_array!(type, origin:)
          return if type.is_a?(MLC::SemanticIR::ArrayType)

          message = "Indexing requires an array, got #{@type_checker.describe_type(type)}"
          @type_checker.type_error(message, node: origin)
        end
      end
    end
  end
end
