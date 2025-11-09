# frozen_string_literal: true

module MLC
  module Core
    FunctionSignature = Struct.new(:name, :param_types, :ret_type, :type_params) do
      def initialize(name, param_types, ret_type, type_params = [])
        super(name, param_types, ret_type, type_params)
      end
    end
  end
end
