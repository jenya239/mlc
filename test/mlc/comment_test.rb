# frozen_string_literal: true

require_relative "../test_helper"
require_relative "../../lib/mlc/common/index"

class MLCCommentTest < Minitest::Test
  def test_block_comment_between_statements
    source = <<~MLC
      fn main() -> i32 =
        /* compute result */
        42
    MLC

    ast = MLC.parse(source)
    assert_equal 1, ast.declarations.size

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "return 42;"
  end

  def test_multiline_block_comment_is_ignored
    source = <<~MLC
      fn value() -> i32 =
        /* comment line 1
           comment line 2 */
        7
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "return 7;"
  end

  def test_doc_comment_like_syntax_does_not_break
    source = <<~MLC
      // overall description
      fn flag() -> bool =
        /// doc comment
        /* block doc */
        true
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "return true;"
  end
end
