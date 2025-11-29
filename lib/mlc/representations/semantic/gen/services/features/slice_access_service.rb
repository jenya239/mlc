# frozen_string_literal: true

module MLC
  module Representations
    module Semantic
      module Gen
        module Services
          # SliceAccessService - validates and resolves array slice accesses
          class SliceAccessService
            def initialize(type_checker:)
              @type_checker = type_checker
            end

            def build(object_ir:, start_ir:, end_ir:, ir_builder:, origin:)
              ensure_array!(object_ir.type, origin: origin)

              # Validate index types if present
              if start_ir
                @type_checker.ensure_numeric_type(start_ir.type, 'slice start index', node: nil)
              end
              if end_ir
                @type_checker.ensure_numeric_type(end_ir.type, 'slice end index', node: nil)
              end

              # Slice returns the same array type
              ir_builder.slice(
                object: object_ir,
                start_index: start_ir,
                end_index: end_ir,
                type: object_ir.type,
                origin: origin
              )
            end

            private

            def ensure_array!(type, origin:)
              return if type.is_a?(MLC::SemanticIR::ArrayType)

              message = "Slicing requires an array, got #{@type_checker.describe_type(type)}"
              @type_checker.type_error(message, node: origin)
            end
          end
        end
      end
    end
  end
end
