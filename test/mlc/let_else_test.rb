# frozen_string_literal: true

require_relative "../test_helper"

class LetElseTest < Minitest::Test
  def test_let_else_generates_holds_alternative
    source = <<~MLC
      type Opt = Some(i32) | None

      fn f(o: Opt) -> i32 = do
        let Some(v) = o else return 0 end
        v
      end
    MLC
    cpp = MLC.to_cpp(source)
    assert_includes cpp, "holds_alternative"
  end

  def test_let_else_no_abort
    source = <<~MLC
      type Opt = Some(i32) | None

      fn f(o: Opt) -> i32 = do
        let Some(v) = o else return 0 end
        v
      end
    MLC
    cpp = MLC.to_cpp(source)
    refute_includes cpp, "abort"
  end

  def test_let_else_contains_else_code
    source = <<~MLC
      type Opt = Some(i32) | None

      fn f(o: Opt) -> i32 = do
        let Some(v) = o else return 0 end
        v
      end
    MLC
    cpp = MLC.to_cpp(source)
    assert_includes cpp, "return 0"
  end

  def test_let_else_missing_raises
    source = <<~MLC
      type Opt = Some(i32) | None

      fn f(o: Opt) -> i32 = do
        let Some(v) = o
        v
      end
    MLC
    assert_raises(MLC::CompileError) { MLC.to_cpp(source) }
  end
end
