# frozen_string_literal: true

require_relative "../test_helper"
require_relative "../../lib/mlc/common/index"

class MLCIOTest < Minitest::Test
  def test_print_lowering
    mlc_source = <<~MLC
      fn main() -> i32 = println("hello")
    MLC

    cpp = MLC.to_cpp(mlc_source)

    assert_includes cpp, "mlc::io::println"
    assert_includes cpp, 'mlc::String("hello")'
  end

  def test_read_line_and_args_lowering
    mlc_source = <<~MLC
      fn main() -> str = read_line()
      fn second() -> str[] = args()
    MLC

    cpp = MLC.to_cpp(mlc_source)

    assert_includes cpp, "mlc::io::read_line"
    assert_includes cpp, "mlc::io::args"
  end
end
