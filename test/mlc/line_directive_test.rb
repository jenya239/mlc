# frozen_string_literal: true

require_relative "../test_helper"
require_relative "../../lib/mlc/common/index"

# TRACK_DEBUG_SOURCE_MAP STEP=2 — Ruby codegen emits #line per statement.
class MLCLineDirectiveTest < Minitest::Test
  def test_line_directives_before_let_and_return
    source = <<~MLC
      fn main() -> i32 = do
        let x = 1
        x + 1
      end
    MLC

    cpp = MLC.to_cpp(source, filename: "probe_line.mlc")

    assert_includes cpp, "#line 2 \"probe_line.mlc\""
    assert_includes cpp, "#line 3 \"probe_line.mlc\""
    assert_includes cpp, "int x = 1;"
    assert_match(/#line 2 "probe_line\.mlc"\s*int x = 1;/, cpp)
    assert_match(/#line 3 "probe_line\.mlc"\s*return /, cpp)
  end

  def test_line_directive_skipped_without_filename
    source = <<~MLC
      fn main() -> i32 = do
        let x = 1
        x
      end
    MLC

    cpp = MLC.to_cpp(source)

    refute_includes cpp, "#line"
  end
end
