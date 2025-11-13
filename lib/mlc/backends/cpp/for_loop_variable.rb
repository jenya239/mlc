# frozen_string_literal: true

module MLC
  module Backends
    module Cpp
      # Simple struct for representing C++ for loop variables
      # Used by ForRule and ListCompRule to generate loop declarations
      ForLoopVariable = Struct.new(:type_str, :name) do
        def to_source
          "#{type_str} #{name}"
        end
      end
    end
  end
end
