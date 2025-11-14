# frozen_string_literal: true

require_relative "../test_helper"
require_relative "../../lib/mlc/common/index"

class MLCErrorDiagnosticsTest < Minitest::Test
  def test_compile_error_includes_source_location
    mlc_source = <<~MLCORA
      fn main() -> void =
        if true then 1 else 2
    MLCORA

    error = assert_raises MLC::CompileError do
      MLC.to_cpp(mlc_source)
    end

    assert_match(/Pass lower_declarations failed: <input>:\d+:\d+: function 'main' result expected void, got i32/, error.message)
  end
end
