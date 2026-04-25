# frozen_string_literal: true

require "test_helper"

class ResultOptionCombinatorTest < Minitest::Test
  def test_result_map_chain_emits_namespace_helper
    source = <<~MLC
      type Result<T, E> = Ok(T) | Err(E)

      fn f() -> Result<i32, string> =
        Ok(1).map(x => x + 1)
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "mlc::result::map"
  end

  def test_option_map_emits_opt_namespace
    source = <<~MLC
      type Option<T> = Some(T) | None

      fn f() -> Option<i32> =
        Some(0).map(x => x + 1)
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "mlc::opt::map"
  end
end
