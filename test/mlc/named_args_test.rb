# frozen_string_literal: true

require_relative "../test_helper"
require_relative "../../lib/mlc/common/index"

class NamedArgsTest < Minitest::Test
  def test_all_named_compiles
    cpp = MLC.to_cpp(<<~MLC)
      fn add(x: i32, y: i32) -> i32 = x + y
      fn main() -> i32 = add(x: 1, y: 2)
    MLC
    assert_includes cpp, "add("
  end

  def test_reverse_order_compiles
    cpp = MLC.to_cpp(<<~MLC)
      fn add(x: i32, y: i32) -> i32 = x + y
      fn main() -> i32 = add(y: 2, x: 1)
    MLC
    # C++ call must pass args in declaration order: add(1, 2)
    assert_match(/add\(.*1.*,.*2.*\)/, cpp)
  end

  def test_mixed_positional_named
    cpp = MLC.to_cpp(<<~MLC)
      fn connect(host: string, port: i32, timeout: i32) -> i32 = port
      fn main() -> i32 = connect("localhost", port: 5432, timeout: 30)
    MLC
    assert_includes cpp, "connect("
  end

  def test_unknown_name_raises
    assert_raises(MLC::CompileError) do
      MLC.to_cpp(<<~MLC)
        fn add(x: i32, y: i32) -> i32 = x + y
        fn main() -> i32 = add(bad: 1, y: 2)
      MLC
    end
  end
end
