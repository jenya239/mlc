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
          Hit(value) => do
            let ignored = print("got return value")
          end,
          Miss => {}
        }
    SRC

    cpp = MLC.to_cpp(source)

    assert_match(/\[&\]\(const Hit& hit\) -> void \{.*?mlc::String\("got return value"\)/m, cpp)
    assert_match(/got return value.*?\[&\]\(const Miss& miss\) -> void/m, cpp)
    refute_match(/got return value[^;]*\}\s+print/, cpp)
  end

  def test_sibling_match_arms_each_declare_the_same_local
    source = <<~SRC
      type Shape = Circle | Line

      fn label(shape: Shape) -> i32 = do
        let mut total = 0
        match shape {
          Circle => do
            const later_elements = 1
            total = later_elements
          end,
          Line => do
            const later_elements = 2
            total = later_elements
          end
        }
        total
      end
    SRC

    cpp = MLC.to_cpp(source)

    assert_equal 2, cpp.scan(/int later_elements =/).length
    refute_match(/^\s*later_elements =/m, cpp)
  end

  def test_sibling_while_bodies_each_declare_the_same_local
    source = <<~SRC
      fn count(declarations: [i32]) -> i32 = do
        let mut declaration_index = 0
        let mut total = 0
        while declaration_index < declarations.length() do
          const type_name = declarations[declaration_index]
          total = total + type_name
          declaration_index = declaration_index + 1
        end
        declaration_index = 0
        while declaration_index < declarations.length() do
          const type_name = declarations[declaration_index]
          total = total + type_name
          declaration_index = declaration_index + 1
        end
        total
      end
    SRC

    cpp = MLC.to_cpp(source)

    assert_equal 2, cpp.scan(/int type_name =/).length
    refute_match(/^\s*type_name =/m, cpp)
  end

  def test_statement_match_on_generic_result_instantiates_variant_templates
    source = <<~SRC
      type Result<T, E> = Ok(T) | Err(E)

      fn apply(value: Result<i32, string>) -> unit = do
        match value {
          Err(message) => do
            let ignored = message.length()
          end,
          Ok(number) => do
            let ignored = number
          end
        }
      end
    SRC

    cpp = MLC.to_cpp(source)

    assert_includes cpp, "Ok<int>"
    assert_includes cpp, "Err<mlc::String>"
    refute_match(/const Ok&/, cpp)
    refute_match(/const Err&/, cpp)
  end

  def test_nullary_variant_passed_to_ok_is_the_sum_value
    source = <<~SRC
      type Value = Unit | Num(i32)
      type Result<T, E> = Ok(T) | Err(E)

      fn unit_ok(tag: i32) -> Result<Value, string> =
        match tag {
          0 => Ok(Unit),
          _ => Ok(Num(tag))
        }
    SRC

    cpp = MLC.to_cpp(source)

    refute_includes cpp, "function<"
    assert_includes cpp, "Unit{}"
    assert_includes cpp, "Ok<Value>"
  end

  def test_zero_argument_function_value_stays_a_function
    source = <<~SRC
      fn answer() -> i32 = 1

      fn take(callback: () -> i32) -> i32 = callback()

      fn main() -> i32 = take(answer)
    SRC

    cpp = MLC.to_cpp(source)

    refute_includes cpp, "answer{}"
    assert_match(/take\(answer\)|take\(std::function[^;]*answer/, cpp)
  end
end
