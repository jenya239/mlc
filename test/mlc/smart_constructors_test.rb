# frozen_string_literal: true

require_relative "../test_helper"
require_relative "../../lib/mlc/common/index"

class SmartConstructorsTest < Minitest::Test
  def test_private_ctor_declaration_compiles
    cpp = MLC.to_cpp(<<~MLC)
      type Email = private Email { raw: string }
      fn main() -> i32 = 0
    MLC
    assert_includes cpp, "Email"
  end

  def test_private_ctor_outside_extend_raises
    assert_raises(MLC::CompileError) do
      MLC.to_cpp(<<~MLC)
        type Email = private Email { raw: string }
        fn f(s: string) -> Email = Email { raw: s }
        fn main() -> i32 = 0
      MLC
    end
  end

  def test_private_ctor_inside_extend_compiles
    cpp = MLC.to_cpp(<<~MLC)
      type Email = private Email { raw: string }
      extend Email {
        fn make(s: string) -> Email = Email { raw: s }
      }
      fn main() -> i32 = 0
    MLC
    assert_includes cpp, "Email"
  end
end
