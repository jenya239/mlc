# frozen_string_literal: true

module MLC
  module Backends
    module Cpp
      # std::visit needs a variant lvalue; compiler AST sums are often stored as
      # std::shared_ptr<std::variant<...>> while IR may drop the Shared<> wrapper.
      module MatchScrutineeDeref
        module_function

        def ast_sum_needs_star?(type_registry, type)
          return false if type.nil?
          return false if shared_generic?(type)

          sum_name = sum_base_name(type)
          return false unless sum_name

          info = type_registry&.lookup(sum_name)
          return false unless info&.sum?

          cpp = info.cpp_name.to_s
          cpp.start_with?("ast::")
        end

        def shared_generic?(type)
          type.is_a?(MLC::SemanticIR::GenericType) &&
            type.base_type.respond_to?(:name) &&
            type.base_type.name == "Shared"
        end

        def sum_base_name(type)
          case type
          when MLC::SemanticIR::SumType
            type.name
          when MLC::SemanticIR::GenericType
            bt = type.base_type
            return bt.name if bt.is_a?(MLC::SemanticIR::SumType)

            bt.respond_to?(:name) ? bt.name : nil
          else
            nil
          end
        end
      end
    end
  end
end
