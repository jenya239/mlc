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

  def test_void_nested_match_iife_arms_do_not_fall_through
    source = <<~SRC
      type Term = Ret(i32) | Halt

      fn check(term: Term) -> void =
        match term {
          Ret(value) =>
            match value {
              7 => {},
              _ => {}
            },
          _ => {}
        }
    SRC

    cpp = MLC.to_cpp(source)

    refute_match(/\}\s+results\.push_back/, cpp)
    assert_match(/if \(std::holds_alternative<Ret>\([^)]+\)\)[^{]+\{[^}]+return/, cpp)
  end

  def test_void_match_arm_with_return_in_string_literal_still_exits
    source = <<~SRC
      type Op = Hit(i32) | Miss

      fn probe(op: Op) -> void =
        match op {
          Hit(value) => print(mlc::String("got return value")),
          Miss => print(mlc::String("miss"))
        }
    SRC

    cpp = MLC.to_cpp(source)

    assert_match(/if \(std::holds_alternative<Hit>\(op\)\)[^{]+\{[^}]+return;/, cpp)
    refute_match(/got return value[^;]*\}\s+print/, cpp)
  end
end
