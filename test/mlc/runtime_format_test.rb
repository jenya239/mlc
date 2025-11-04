# frozen_string_literal: true

require_relative "../test_helper"
require_relative "../../lib/mlc"

class MLCRuntimeFormatTest < Minitest::Test
  def test_format_generates_runtime_call
    source = <<~MLC
      fn banner(x: i32, y: bool) -> str =
        format("x={}, y={}", x, y)
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "mlc::format"
  end

end
