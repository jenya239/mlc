# frozen_string_literal: true

module MLC
  module Backends
    module Cpp
      VERSION = "2.0.0"
    end
  end
end

require_relative "cpp/container"
require_relative "cpp/context"
