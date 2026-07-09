# frozen_string_literal: true

require_relative "../test_helper"

# Tests for EscapeAnalyzer - non-escaping Fn(...) parameters compile to a
# template type parameter (zero allocation, zero refcount) instead of
# std::function<...>. See docs/agent/TRACK_LANG_CLOSURE_ESCAPE.md.
class ClosureEscapeAnalysisTest < Minitest::Test
  def test_direct_call_only_param_is_non_escaping_template
    mlc_source = <<~MLCORA
      fn apply_twice(f: fn(i32) -> i32, x: i32) -> i32 = f(f(x))
    MLCORA

    cpp_code = MLC.to_cpp(mlc_source)

    assert_match(/template<typename \w+>/, cpp_code)
    refute_includes cpp_code, "std::function"
  end

  def test_rule1_stored_in_record_field_is_escaping
    mlc_source = <<~MLCORA
      type Handler = { callback: fn(i32) -> i32 }

      fn make_handler(f: fn(i32) -> i32) -> Handler = Handler { callback: f }
    MLCORA

    cpp_code = MLC.to_cpp(mlc_source)

    assert_includes cpp_code, "std::function<int(int)> f"
    refute_match(/template<typename \w+>\nHandler make_handler/, cpp_code)
  end

  def test_rule2_returned_from_function_is_escaping
    mlc_source = <<~MLCORA
      fn identity_fn(f: fn(i32) -> i32) -> fn(i32) -> i32 = f
    MLCORA

    cpp_code = MLC.to_cpp(mlc_source)

    assert_includes cpp_code, "std::function<int(int)> f"
  end

  def test_rule3_passed_as_argument_to_another_call_is_escaping
    mlc_source = <<~MLCORA
      fn g(f: fn(i32) -> i32, x: i32) -> i32 = f(x)
      fn call_other(f: fn(i32) -> i32, x: i32) -> i32 = g(f, x)
    MLCORA

    cpp_code = MLC.to_cpp(mlc_source)

    assert_includes cpp_code, "std::function<int(int)> f, int x) noexcept{return g(f, x);}"
    assert_match(/template<typename \w+>\n\w+ int g/, cpp_code)
  end

  def test_rule4_aliased_by_non_call_reference_is_escaping
    mlc_source = <<~MLCORA
      fn schedule(f: fn(i32) -> i32) -> i32 = do
        let alias = f;
        alias(1)
      end
    MLCORA

    cpp_code = MLC.to_cpp(mlc_source)

    assert_includes cpp_code, "std::function<int(int)> f"
  end

  # Direct-call-only usage remains safely non-escaping even nested inside
  # another local lambda: the outer function still becomes a template, so the
  # capture is always a concrete (never named) closure type at each call site.
  def test_direct_call_inside_nested_lambda_stays_non_escaping
    mlc_source = <<~MLCORA
      fn schedule(f: fn(i32) -> i32) -> i32 = do
        let wrapper = () => f(1);
        wrapper()
      end
    MLCORA

    cpp_code = MLC.to_cpp(mlc_source)

    assert_match(/template<typename \w+>/, cpp_code)
    refute_includes cpp_code, "std::function"
  end

  def test_non_escaping_generated_cpp_compiles_and_runs
    require "tmpdir"

    mlc_source = <<~MLCORA
      fn apply_twice(f: fn(i32) -> i32, x: i32) -> i32 = f(f(x))

      fn main() -> i32 = do
        print(apply_twice(n => n * 2, 5));
        0
      end
    MLCORA

    cpp_code = MLC.to_cpp(mlc_source)

    Dir.mktmpdir do |dir|
      cpp_path = File.join(dir, "main.cpp")
      binary_path = File.join(dir, "main")
      File.write(cpp_path, cpp_code)

      root = File.expand_path("../..", __dir__)
      compile_ok = system(
        "g++", "-std=c++20", "-I", File.join(root, "runtime/include"),
        cpp_path,
        File.join(root, "runtime/src/io/io.cpp"),
        File.join(root, "runtime/src/core/string.cpp"),
        File.join(root, "runtime/src/core/profile.cpp"),
        "-o", binary_path,
        out: File::NULL, err: File::NULL
      )
      assert compile_ok, "generated C++ failed to compile"

      output = `#{binary_path}`
      assert_equal "20", output.strip
    end
  end
end
