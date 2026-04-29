# frozen_string_literal: true

module MLC
  module Registries
    # required_arity: number of leading parameters that must be passed at a call (no default).
    FunctionSignature = Struct.new(:name, :param_types, :ret_type, :type_params, :required_arity, :param_names, keyword_init: false)
  end
end
