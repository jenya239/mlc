# frozen_string_literal: true

module MLC
  module SemanticGen
    module Services
      # ListComprehensionService - builds SemanticIR list comprehension expressions
      class ListComprehensionService
        def initialize(ir_builder:, loop_service:, var_type_registry:)
          @ir_builder = ir_builder
          @loop_service = loop_service
          @var_type_registry = var_type_registry
        end

        def build(node, expression_visitor:)
          snapshot = @var_type_registry.snapshot

          generators_ir = build_generators(node.generators, expression_visitor)
          filters_ir = Array(node.filters).map { |filter| expression_visitor.visit(filter) }
          output_ir = expression_visitor.visit(node.output_expr)

          element_type = output_ir.type || default_element_type(node)
          array_type = @ir_builder.array_type(element_type: element_type, origin: node)

          @ir_builder.list_comprehension(
            element_type: element_type,
            generators: generators_ir,
            filters: filters_ir,
            output_expr: output_ir,
            type: array_type,
            origin: node
          )
        ensure
          @var_type_registry.restore(snapshot)
        end

        private

        def build_generators(generators, expression_visitor)
          generators.each_with_object([]) do |generator_ast, acc|
            iterable_ir = expression_visitor.visit(generator_ast.iterable)
            element_type = @loop_service.infer_iterable_element(iterable_ir, node: generator_ast.iterable)

            @var_type_registry.set(generator_ast.var_name, element_type)

            acc << {
              var_name: generator_ast.var_name,
              iterable: iterable_ir,
              var_type: element_type
            }
          end
        end

        def default_element_type(node)
          @ir_builder.prim_type(name: 'i32', origin: node)
        end
      end
    end
  end
end
