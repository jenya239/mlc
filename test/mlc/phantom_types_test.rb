# frozen_string_literal: true

require_relative "../test_helper"
require_relative "../../lib/mlc/common/index"

class MLCPhantomTypesTest < Minitest::Test
  def test_phantom_type_declaration_compiles
    cpp = MLC.to_cpp(<<~MLC)
      type Unvalidated
      type Ast<Phase> = Ast { value: i32 }
      fn main() -> i32 = 0
    MLC
    assert_includes cpp, "template"
    assert_includes cpp, "Phase"
  end

  def test_phantom_single_variant_template_in_cpp
    cpp = MLC.to_cpp(<<~MLC)
      type Unvalidated
      type Ast<Phase> = Ast { value: i32 }
      fn main() -> i32 = 0
    MLC
    assert_match(/template\s*<\s*typename\s+Phase\s*>/, cpp)
  end

  def test_phantom_multi_variant_preserves_phantom_in_structs
    cpp = MLC.to_cpp(<<~MLC)
      type Tag
      type State<S> = Active(i32) | Inactive
      fn main() -> i32 = 0
    MLC
    assert_match(/template\s*<\s*typename\s+S\s*>/, cpp)
    assert_includes cpp, "Active<S>"
  end

  def test_phantom_type_fn_returning_phantom_record_compiles
    cpp = MLC.to_cpp(<<~MLC)
      type Unvalidated
      type Ast<Phase> = Ast { value: i32 }
      fn make() -> Ast<Unvalidated> = do
        let ast: Ast<Unvalidated> = Ast { value: 1 }
        ast
      end
      fn main() -> i32 = 0
    MLC
    assert_includes cpp, "Ast"
    assert_includes cpp, "Unvalidated"
  end
end
