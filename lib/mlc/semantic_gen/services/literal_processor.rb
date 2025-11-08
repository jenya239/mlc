# frozen_string_literal: true

module MLC
  module SemanticGen
    module Services
      # LiteralProcessor - Handles literal AST classification and SemanticIR construction
      class LiteralProcessor
        def initialize(ir_builder:)
          @ir_builder = ir_builder
        end

        def supported?(node, ast_type_checker)
          !ast_type_checker.literal_kind(node).nil?
        end

        def build(node, ast_type_checker)
          kind = ast_type_checker.literal_kind(node)
          return nil unless kind

          case kind
          when :int
            build_numeric_literal(node, 'i32')
          when :float
            build_numeric_literal(node, 'f32')
          when :string
            build_string_literal(node)
          when :regex
            build_regex_literal(node)
          when :unit
            build_unit_literal(node)
          end
        end

        private

        def build_numeric_literal(node, type_name)
          type = @ir_builder.prim_type(name: type_name, origin: node)
          @ir_builder.literal(value: node.value, type: type, origin: node)
        end

        def build_string_literal(node)
          type = @ir_builder.prim_type(name: 'string', origin: node)
          @ir_builder.literal(value: node.value, type: type, origin: node)
        end

        def build_regex_literal(node)
          type = @ir_builder.prim_type(name: 'regex', origin: node)
          @ir_builder.regex(pattern: node.pattern, flags: node.flags, type: type, origin: node)
        end

        def build_unit_literal(node)
          @ir_builder.unit_literal(origin: node)
        end
      end
    end
  end
end
