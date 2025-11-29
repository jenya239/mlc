# frozen_string_literal: true


module MLC
  module Backends
    module Cpp
      module Rules
        module Expressions
          # Rule for lowering SemanticIR::SliceExpr to C++ vector constructor with iterators
          # Handles arr[start..end], arr[start..], arr[..end], arr[..]
          #
          # Generated C++ patterns:
          #   arr[start..end] → std::vector<T>(arr.begin() + start, arr.begin() + end)
          #   arr[start..]    → std::vector<T>(arr.begin() + start, arr.end())
          #   arr[..end]      → std::vector<T>(arr.begin(), arr.begin() + end)
          #   arr[..]         → std::vector<T>(arr)  // full copy
          class SliceRule < ExpressionRule
            def applies?(node)
              context.checker.slice_expr?(node)
            end

            def apply(node)
              array = lower_expression(node.object)
              start_idx = node.start_index ? lower_expression(node.start_index) : nil
              end_idx = node.end_index ? lower_expression(node.end_index) : nil

              # Get element type for the resulting vector
              element_type = context.map_type(node.type.element_type)

              build_slice_expression(array, start_idx, end_idx, element_type)
            end

            private

            def build_slice_expression(array, start_idx, end_idx, element_type)
              array_src = array.to_source

              if start_idx.nil? && end_idx.nil?
                # arr[..] - full copy
                # std::vector<T>(arr)
                context.factory.function_call(
                  callee: context.factory.identifier(name: "std::vector<#{element_type}>"),
                  arguments: [array],
                  argument_separators: []
                )
              elsif start_idx.nil?
                # arr[..end] - from beginning to end
                # std::vector<T>(arr.begin(), arr.begin() + end)
                start_iter = "#{array_src}.begin()"
                end_iter = "#{array_src}.begin() + #{end_idx.to_source}"
                build_vector_from_iterators(element_type, start_iter, end_iter)
              elsif end_idx.nil?
                # arr[start..] - from start to end
                # std::vector<T>(arr.begin() + start, arr.end())
                start_iter = "#{array_src}.begin() + #{start_idx.to_source}"
                end_iter = "#{array_src}.end()"
                build_vector_from_iterators(element_type, start_iter, end_iter)
              else
                # arr[start..end] - from start to end
                # std::vector<T>(arr.begin() + start, arr.begin() + end)
                start_iter = "#{array_src}.begin() + #{start_idx.to_source}"
                end_iter = "#{array_src}.begin() + #{end_idx.to_source}"
                build_vector_from_iterators(element_type, start_iter, end_iter)
              end
            end

            def build_vector_from_iterators(element_type, start_iter, end_iter)
              # Generate: std::vector<T>(start_iter, end_iter)
              context.factory.function_call(
                callee: context.factory.identifier(name: "std::vector<#{element_type}>"),
                arguments: [
                  context.factory.raw_expression(code: start_iter),
                  context.factory.raw_expression(code: end_iter)
                ],
                argument_separators: [", "]
              )
            end
          end
        end
      end
    end
  end
end
