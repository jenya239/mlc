# frozen_string_literal: true

module MLC
  module Backends
    module Cpp
      module Services
        module Policies
          # Policy decisions for lowering strategies
          module LoweringPolicy
            module_function

            # Check if expression/type should be lowered as statement (not expression)
            def should_lower_as_statement?(expr_or_type)
              return true if expr_or_type.is_a?(SemanticIR::UnitLiteral)
              return true if expr_or_type.is_a?(SemanticIR::UnitType)
              return true if expr_or_type.is_a?(SemanticIR::IfExpr) && expr_or_type.type.is_a?(SemanticIR::UnitType)
              false
            end
          end
        end
      end
    end
  end
end
