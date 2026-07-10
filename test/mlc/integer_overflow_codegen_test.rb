# frozen_string_literal: true

require_relative "../test_helper"
require_relative "../../lib/mlc/common/index"
require "fileutils"
require "open3"
require "tmpdir"

class MLCIntegerOverflowCodegenTest < Minitest::Test
  def test_signed_add_uses_checked_add
    cpp = MLC.compile("fn add(a: i32, b: i32) -> i32 = a + b").to_source
    assert_includes cpp, "mlc::arith::checked_add(a, b)"
    assert_includes cpp, "mlc/core/arith.hpp"
    refute_match(/return a \+ b/, cpp)
  end

  def test_signed_sub_mul_use_helpers
    cpp = MLC.compile(<<~MLC).to_source
      fn sub(a: i64, b: i64) -> i64 = a - b
      fn mul(a: i8, b: i8) -> i8 = a * b
    MLC
    assert_includes cpp, "mlc::arith::checked_sub(a, b)"
    assert_includes cpp, "mlc::arith::checked_mul(a, b)"
  end

  def test_signed_i16_char_use_helpers
    cpp = MLC.compile(<<~MLC).to_source
      fn add_i16(a: i16, b: i16) -> i16 = a + b
      fn add_char(a: char, b: char) -> char = a + b
    MLC
    assert_includes cpp, "mlc::arith::checked_add(a, b)"
  end

  def test_unsigned_add_wraps_raw
    %w[u8 u16 u32 u64].each do |type_name|
      cpp = MLC.compile("fn add(a: #{type_name}, b: #{type_name}) -> #{type_name} = a + b").to_source
      assert_includes cpp, "a + b", type_name
      refute_includes cpp, "checked_add", type_name
    end
  end

  def test_integer_div_mod_use_helpers
    cpp = MLC.compile(<<~MLC).to_source
      fn div(a: i32, b: i32) -> i32 = a / b
      fn mod(a: u32, b: u32) -> u32 = a % b
    MLC
    assert_includes cpp, "mlc::arith::checked_div(a, b)"
    assert_includes cpp, "mlc::arith::checked_mod(a, b)"
  end

  def test_float_add_unchanged
    cpp = MLC.compile("fn add(a: f64, b: f64) -> f64 = a + b").to_source
    assert_includes cpp, "a + b"
    refute_includes cpp, "checked_add"
  end

  def build_and_run(dir, source_text)
    src_dir = File.join(dir, "src")
    out_dir = File.join(dir, "out")
    FileUtils.mkdir_p(src_dir)
    entry = File.join(src_dir, "main.mlc")
    File.write(entry, source_text)
    result = MLC.build_project(entry_path: entry, out_dir: out_dir, root_dir: src_dir)
    Open3.capture3(result[:binary])
  end

  def test_signed_overflow_panics_at_runtime
    Dir.mktmpdir do |dir|
      _stdout, stderr, status = build_and_run(dir, <<~MLC)
        fn main() -> i32 = do
          let a: i32 = 2147483647
          let b: i32 = 1
          let _c = a + b
          0
        end
      MLC
      assert_equal 1, status.exitstatus
      assert_match(/integer overflow|PANIC/i, stderr)
    end
  end

  def test_div_zero_panics_at_runtime
    Dir.mktmpdir do |dir|
      _stdout, stderr, status = build_and_run(dir, <<~MLC)
        fn main() -> i32 = do
          let a: i32 = 1
          let b: i32 = 0
          let _c = a / b
          0
        end
      MLC
      assert_equal 1, status.exitstatus
      assert_match(/division by zero|PANIC/i, stderr)
    end
  end

  def test_unsigned_wrap_no_panic_at_runtime
    Dir.mktmpdir do |dir|
      _stdout, stderr, status = build_and_run(dir, <<~MLC)
        fn main() -> i32 = do
          let a: u32 = 4294967295u32
          let b: u32 = 1u32
          let c = a + b
          let _keep = c
          0
        end
      MLC
      assert_equal 0, status.exitstatus, stderr
      assert_equal "", stderr
    end
  end
end
