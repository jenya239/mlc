# frozen_string_literal: true

require_relative "../test_helper"
require_relative "../../lib/mlc"

class MLCCollectionsTest < Minitest::Test
  def test_map_and_filter_lowering
    mlc_source = <<~MLC
      fn process(nums: i32[]) -> i32[] =
        nums.map(x => x + 1).filter(x => x > 5)
    MLC

    cpp = MLC.to_cpp(mlc_source)

    assert_includes cpp, "mlc::collections::map"
    assert_includes cpp, "mlc::collections::filter"
  end

  def test_fold_lowering
    mlc_source = <<~MLC
      fn sum(nums: i32[]) -> i32 =
        nums.fold(0, (acc, num) => acc + num)
    MLC

    cpp = MLC.to_cpp(mlc_source)

    assert_includes cpp, "mlc::collections::fold"
  end
end
