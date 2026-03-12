# frozen_string_literal: true

require "minitest/autorun"
require_relative "../../lib/mlc/common/index"

class MoveSemanticsTest < Minitest::Test
  # ===========================================
  # Copy types: no move, no use-after-move error
  # ===========================================

  def test_copy_type_i32_no_move
    source = <<~MLC
      fn foo() -> i32 = do
        let a = 42
        let b = a
        a + b
      end
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "a + b"
  end

  def test_copy_type_bool_no_move
    source = <<~MLC
      fn foo() -> bool = do
        let a = true
        let b = a
        a
      end
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "foo"
  end

  def test_copy_type_f32_no_move
    source = <<~MLC
      fn foo() -> f32 = do
        let a: f32 = 3.14
        let b = a
        a + b
      end
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "a + b"
  end

  # ===========================================
  # Move on bind: non-Copy types moved by let
  # ===========================================

  def test_move_on_bind_array
    source = <<~MLC
      fn foo() -> i32[] = do
        let a = [1, 2, 3]
        let b = a
        a
      end
    MLC

    # Array is COW (Copy-on-Write), no move error expected
    cpp = MLC.to_cpp(source)
    assert_includes cpp, "foo"
  end

  def test_move_on_bind_string
    source = <<~MLC
      fn foo() -> str = do
        let a = "hello"
        let b = a
        a
      end
    MLC

    # String is COW (Copy-on-Write), no move error expected
    cpp = MLC.to_cpp(source)
    assert_includes cpp, "foo"
  end

  def test_move_on_bind_owned
    source = <<~MLC
      fn foo() -> i32 = do
        let a: Owned<i32> = 0
        let b = a
        a
      end
    MLC

    error = assert_raises(MLC::CompileError) { MLC.to_cpp(source) }
    assert_match(/moved.*'a'/, error.message)
  end

  # ===========================================
  # No move on bind for first use (no double-use)
  # ===========================================

  def test_move_on_bind_single_use_ok
    source = <<~MLC
      fn foo() -> i32[] = do
        let a = [1, 2, 3]
        let b = a
        b
      end
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "foo"
  end

  # ===========================================
  # RC types: Shared/Weak are not moved
  # ===========================================

  def test_shared_not_moved_on_bind
    source = <<~MLC
      fn foo(x: Shared<i32>) -> i32 = do
        let a = x
        let b = x
        0
      end
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "shared_ptr"
  end

  def test_weak_not_moved_on_bind
    source = <<~MLC
      fn foo(x: Weak<i32>) -> i32 = do
        let a = x
        let b = x
        0
      end
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "weak_ptr"
  end

  # ===========================================
  # Function arguments: only Owned<T> is moved
  # ===========================================

  def test_array_arg_not_moved_on_pass
    source = <<~MLC
      fn len(arr: i32[]) -> i32 = 0

      fn foo() -> i32 = do
        let a = [1, 2, 3]
        let x = len(a)
        let y = len(a)
        x + y
      end
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "len"
  end

  def test_owned_arg_moved_on_pass
    source = <<~MLC
      fn consume(x: Owned<i32>) -> i32 = 0

      fn foo() -> i32 = do
        let a: Owned<i32> = 0
        let x = consume(a)
        a
      end
    MLC

    error = assert_raises(MLC::CompileError) { MLC.to_cpp(source) }
    assert_match(/moved.*'a'/, error.message)
  end

  # ===========================================
  # Mutable reassignment resets moved state
  # ===========================================

  def test_mutable_reassignment_resets_moved
    source = <<~MLC
      fn foo() -> i32[] = do
        let a = [1, 2, 3]
        let b = a
        a = [4, 5, 6]
        a
      end
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "foo"
  end

  # ===========================================
  # C++ codegen: std::move generation
  # ===========================================

  def test_codegen_move_on_array_bind
    source = <<~MLC
      fn foo() -> i32[] = do
        let a = [1, 2, 3]
        let b = a
        b
      end
    MLC

    # Array is COW, no std::move expected - simple copy
    cpp = MLC.to_cpp(source)
    refute_includes cpp, "std::move(a)"
  end

  def test_codegen_no_move_on_copy_bind
    source = <<~MLC
      fn foo() -> i32 = do
        let a = 42
        let b = a
        b
      end
    MLC

    cpp = MLC.to_cpp(source)
    refute_includes cpp, "std::move"
  end

  def test_codegen_move_on_string_bind
    source = <<~MLC
      fn foo() -> str = do
        let a = "hello"
        let b = a
        b
      end
    MLC

    # String is COW, no std::move expected - simple copy
    cpp = MLC.to_cpp(source)
    refute_includes cpp, "std::move(a)"
  end

  # ===========================================
  # VarTypeRegistry unit tests
  # ===========================================

  def test_copy_type_predicate
    vtr = MLC::Representations::Semantic::Gen::Services::VarTypeRegistry

    assert vtr.copy_type?(prim("i32"))
    assert vtr.copy_type?(prim("bool"))
    assert vtr.copy_type?(prim("f64"))
    assert vtr.copy_type?(prim("char"))
    assert vtr.copy_type?(prim("u8"))
    assert vtr.copy_type?(prim("void"))
    assert vtr.copy_type?(nil)

    assert vtr.copy_type?(prim("string"))
    assert vtr.copy_type?(prim("str"))
    assert vtr.copy_type?(MLC::SemanticIR::ArrayType.new(element_type: prim("i32")))
    refute vtr.copy_type?(MLC::SemanticIR::RecordType.new(name: "Point", fields: []))
  end

  def test_move_on_bind_predicate
    vtr = MLC::Representations::Semantic::Gen::Services::VarTypeRegistry

    refute vtr.move_on_bind?(prim("i32"))
    refute vtr.move_on_bind?(prim("bool"))

    refute vtr.move_on_bind?(prim("string"))
    refute vtr.move_on_bind?(MLC::SemanticIR::ArrayType.new(element_type: prim("i32")))
    assert vtr.move_on_bind?(owned_type("i32"))
  end

  def test_move_on_pass_predicate
    vtr = MLC::Representations::Semantic::Gen::Services::VarTypeRegistry

    refute vtr.move_on_pass?(prim("i32"))
    refute vtr.move_on_pass?(prim("string"))
    refute vtr.move_on_pass?(MLC::SemanticIR::ArrayType.new(element_type: prim("i32")))
    refute vtr.move_on_pass?(shared_type("i32"))

    assert vtr.move_on_pass?(owned_type("i32"))
  end

  def test_rc_types_are_copy
    vtr = MLC::Representations::Semantic::Gen::Services::VarTypeRegistry

    assert vtr.copy_type?(shared_type("i32"))
    assert vtr.copy_type?(weak_type("i32"))
  end

  private

  def prim(name)
    MLC::SemanticIR::Builder.primitive_type(name)
  end

  def owned_type(inner)
    MLC::SemanticIR::GenericType.new(
      base_type: prim("Owned"),
      type_args: [prim(inner)]
    )
  end

  def shared_type(inner)
    MLC::SemanticIR::GenericType.new(
      base_type: prim("Shared"),
      type_args: [prim(inner)]
    )
  end

  def weak_type(inner)
    MLC::SemanticIR::GenericType.new(
      base_type: prim("Weak"),
      type_args: [prim(inner)]
    )
  end
end
