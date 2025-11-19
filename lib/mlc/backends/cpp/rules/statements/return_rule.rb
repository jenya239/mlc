# frozen_string_literal: true


module MLC
  module Backends
    module Cpp
      module Rules
        module Statements
          # Rule for lowering SemanticIR::Return to C++ return statements
          class ReturnRule < StatementRule
            def applies?(node)
              context.checker.return?(node)
            end

            def apply(node)
              if node.expr
                # Return with expression
                expr = context.lower_expression(node.expr)
                context.factory.return_statement(expression: expr)
              else
                # Void return
                context.factory.return_statement(expression: nil, keyword_suffix: " ")
              end
            end
          end
        end
      end
    end
  end
end
