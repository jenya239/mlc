# frozen_string_literal: true

require_relative "../../base_rule"

module MLC
  module Rules
    module IRGen
      module Expression
        # ListComprehensionRule: Transform AST list comprehensions to HighIR
        # Contains FULL logic (no delegation to transformer)
        # Handles generators, filters, and output expression with scoping
        class ListComprehensionRule < BaseRule
          def applies?(node, _context = {})
            node.is_a?(MLC::AST::ListComprehension)
          end

          def apply(node, context = {})
            transformer = context.fetch(:transformer)
            type_inference = context.fetch(:type_inference)
            var_type_registry = transformer.instance_variable_get(:@var_type_registry)

            # Save variable types for scoping
            saved_var_types = var_type_registry.snapshot

            # Transform generators and add variables to scope
            generators = []

            node.generators.each do |gen|
              iterable_ir = transformer.send(:transform_expression, gen.iterable)
              element_type = type_inference.infer_iterable_type(iterable_ir)

              generators << {
                var_name: gen.var_name,
                iterable: iterable_ir,
                var_type: element_type
              }

              # Add generator variable to scope using new API
              var_type_registry.set(gen.var_name, element_type)
            end

            # Transform filters
            filters = node.filters.map { |filter| transformer.send(:transform_expression, filter) }

            # Transform output expression with generators in scope
            output_expr = transformer.send(:transform_expression, node.output_expr)
            element_type = output_expr.type || MLC::HighIR::Builder.primitive_type("i32")

            # Build array type and list comprehension expression
            array_type = MLC::HighIR::ArrayType.new(element_type: element_type)

            MLC::HighIR::ListCompExpr.new(
              element_type: element_type,
              generators: generators,
              filters: filters,
              output_expr: output_expr,
              type: array_type
            )
          ensure
            # Restore previous variable types scope
            var_type_registry.restore(saved_var_types) if saved_var_types
          end
        end
      end
    end
  end
end
