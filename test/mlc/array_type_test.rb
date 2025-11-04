# frozen_string_literal: true

require_relative "../test_helper"
require_relative "../../lib/mlc"

class MLCArrayTypeTest < Minitest::Test
  def test_array_parameter_maps_to_std_vector
    mlc_source = <<~MLC
      fn count(items: str[]) -> i32 =
        if items.is_empty() then
          0
        else
          items.length()
    MLC

    cpp = MLC.to_cpp(mlc_source)

    assert_includes cpp, "std::vector<mlc::String>"
    assert_includes cpp, "items.size"
    assert_includes cpp, "items.empty"
  end
end
