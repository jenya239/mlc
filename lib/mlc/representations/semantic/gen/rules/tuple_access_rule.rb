# frozen_string_literal: true

module MLC
  module Representations
    module Semantic
      module Gen
        module Rules
          # Rule for transforming AST TupleAccess to SemanticIR TupleAccessExpr
          # Handles: tuple.0 -> TupleAccessExpr(tuple, 0, element_type)
          class TupleAccessRule < BaseRule
            def matches?(node, context)
              services(context).ast_type_checker.tuple_access?(node)
            end

            def produce(node, context)
              svc = services(context)
              expression_visitor = context[:expression_visitor]

              # Lower the tuple expression
              tuple_ir = expression_visitor.visit(node.tuple)
              tuple_type = tuple_ir.type
              index = node.index

              # Verify tuple type and extract element type
              unless tuple_type.is_a?(MLC::SemanticIR::TupleType)
                raise MLC::CompileError.new(
                  "Cannot use positional access on non-tuple type '#{tuple_type.name}'",
                  origin: node.origin
                )
              end

              element_types = tuple_type.element_types
              if index < 0 || index >= element_types.size
                raise MLC::CompileError.new(
                  "Tuple index #{index} out of bounds for tuple with #{element_types.size} elements",
                  origin: node.origin
                )
              end

              element_type = element_types[index]

              svc.ir_builder.tuple_access_expr(
                tuple: tuple_ir,
                index: index,
                type: element_type,
                origin: node.origin
              )
            end
          end
        end
      end
    end
  end
end
