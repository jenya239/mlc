# frozen_string_literal: true

module MLC
  module Common
    module Typing
      class GenericCallResolver
      def initialize(constraint_solver:)
        @constraint_solver = constraint_solver
      end

      def instantiate(info, args, name:, expected_ret_type: nil)
        @constraint_solver.solve(info, args, name: name, expected_ret_type: expected_ret_type)
      end
      end
    end
  end
end
