# frozen_string_literal: true

require_relative "../test_helper"
require_relative "../../lib/mlc"

class MLCErrorDiagnosticsTest < Minitest::Test
  def test_compile_error_includes_source_location
    mlc_source = <<~MLCORA
      fn main() -> void =
        if true then 1 else 2
    MLCORA

    error = assert_raises MLC::CompileError do
      MLC.to_cpp(mlc_source)
    end

    assert_match(/<input>:\d+:\d+: function 'main' should not return a value/, error.message)
  end
end
