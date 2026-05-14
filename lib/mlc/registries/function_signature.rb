# frozen_string_literal: true

module MLC
  module Registries
    # required_arity: number of leading parameters that must be passed at a call (no default).
    # param_mutability_flags: parallel to param_types; true iff the source parameter used `mut` (C++ emits T&).
    FunctionSignature = Struct.new(
      :name,
      :param_types,
      :ret_type,
      :type_params,
      :required_arity,
      :param_names,
      :param_mutability_flags,
      keyword_init: false
    )
  end
end
