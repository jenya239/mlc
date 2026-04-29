# frozen_string_literal: true

require "test_helper"

class C5WithTest < Minitest::Test
  RESOURCE_DEFS = <<~MLC
    type Resource = { value: i32 }
    fn make_resource(x: i32) -> Resource = Resource { value: x }
    extend Resource {
      fn drop(self: Resource) -> i32 = 0
      fn read(self: Resource) -> i32 = self.value
      fn write(self: Resource, v: i32) -> i32 = v
    }
  MLC

  def test_with_basic_codegen
    src = RESOURCE_DEFS + <<~MLC
      fn f(x: i32) -> i32 = do
        with make_resource(x) as res do
          res.read()
        end
        0
      end
    MLC
    cpp = MLC.to_cpp(src)
    assert_includes cpp, "auto res ="
    assert_includes cpp, "res.drop()"
  end

  def test_with_binder_name
    src = RESOURCE_DEFS + <<~MLC
      fn f(x: i32) -> i32 = do
        with make_resource(x) as handle do
          handle.read()
        end
        0
      end
    MLC
    cpp = MLC.to_cpp(src)
    assert_includes cpp, "auto handle ="
    assert_includes cpp, "handle.drop()"
  end

  def test_with_drop_after_body
    src = RESOURCE_DEFS + <<~MLC
      fn f(x: i32) -> i32 = do
        with make_resource(x) as res do
          res.read()
        end
        0
      end
    MLC
    cpp = MLC.to_cpp(src)
    drop_pos = cpp.index("res.drop()")
    read_pos = cpp.index("Resource_read")
    assert read_pos, "expected Resource_read (res.read() lowered) in output"
    assert drop_pos, "expected res.drop() in output"
    assert drop_pos > read_pos, "drop() must come after body"
  end

  def test_with_nested
    src = RESOURCE_DEFS + <<~MLC
      fn f(a: i32, b: i32) -> i32 = do
        with make_resource(a) as outer do
          with make_resource(b) as inner do
            inner.read()
          end
        end
        0
      end
    MLC
    cpp = MLC.to_cpp(src)
    assert_includes cpp, "auto outer ="
    assert_includes cpp, "outer.drop()"
    assert_includes cpp, "auto inner ="
    assert_includes cpp, "inner.drop()"
  end
end
