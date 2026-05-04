# frozen_string_literal: true

require_relative "../test_helper"
require_relative "../../lib/mlc/common/index"

class MLCMatchGuardCppTest < Minitest::Test
  def test_guard_lowering_avoids_std_visit
    source = <<~SRC
      type Answer = Yes(i32) | No

      fn pick(answer: Answer) -> i32 =
        match answer {
          Yes(n) if n > 0 => n,
          Yes(_) => 0,
          No => -1
        }

      fn main() -> i32 = pick(Yes(4))
    SRC

    cpp = MLC.to_cpp(source)

    refute_includes cpp, "std::visit"
    assert_includes cpp, "if (std::holds_alternative"
  end
end
