# frozen_string_literal: true

require_relative "../test_helper"

class ArraySlicingTest < Minitest::Test
  # Test 1: Parsing basic slice syntax arr[start..end]
  def test_parse_slice_start_end
    source = <<~MLC
      fn get_middle(arr: i32[]) -> i32[] = arr[1..3]
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.find { |d| d.is_a?(MLC::Source::AST::FuncDecl) && d.name == "get_middle" }
    assert func, "Function get_middle not found"

    slice = func.body
    assert_instance_of MLC::Source::AST::SliceAccess, slice
    assert_equal "arr", slice.object.name
    refute_nil slice.start_index
    refute_nil slice.end_index
    assert_equal 1, slice.start_index.value
    assert_equal 3, slice.end_index.value
  end

  # Test 2: Parsing open-ended slice arr[start..]
  def test_parse_slice_start_open
    source = <<~MLC
      fn tail(arr: i32[]) -> i32[] = arr[1..]
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.find { |d| d.is_a?(MLC::Source::AST::FuncDecl) && d.name == "tail" }
    assert func, "Function tail not found"

    slice = func.body
    assert_instance_of MLC::Source::AST::SliceAccess, slice
    refute_nil slice.start_index
    assert_nil slice.end_index
  end

  # Test 3: Parsing open-start slice arr[..end]
  def test_parse_slice_open_end
    source = <<~MLC
      fn head(arr: i32[]) -> i32[] = arr[..3]
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.find { |d| d.is_a?(MLC::Source::AST::FuncDecl) && d.name == "head" }
    assert func, "Function head not found"

    slice = func.body
    assert_instance_of MLC::Source::AST::SliceAccess, slice
    assert_nil slice.start_index
    refute_nil slice.end_index
    assert_equal 3, slice.end_index.value
  end

  # Test 4: Parsing full slice arr[..]
  def test_parse_slice_full_copy
    source = <<~MLC
      fn copy(arr: i32[]) -> i32[] = arr[..]
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.find { |d| d.is_a?(MLC::Source::AST::FuncDecl) && d.name == "copy" }
    assert func, "Function copy not found"

    slice = func.body
    assert_instance_of MLC::Source::AST::SliceAccess, slice
    assert_nil slice.start_index
    assert_nil slice.end_index
  end

  # Test 5: C++ lowering for arr[start..end]
  def test_slice_lowering_start_end
    source = <<~MLC
      fn get_slice(arr: i32[]) -> i32[] = arr[1..3]
    MLC

    cpp = MLC.to_cpp(source)

    # Should generate vector constructor with iterators
    assert_includes cpp, "std::vector<int>"
    assert_includes cpp, "begin()"
  end

  # Test 6: C++ lowering for arr[start..]
  def test_slice_lowering_start_open
    source = <<~MLC
      fn get_tail(arr: i32[]) -> i32[] = arr[2..]
    MLC

    cpp = MLC.to_cpp(source)

    assert_includes cpp, "std::vector<int>"
    assert_includes cpp, "begin()"
    assert_includes cpp, "end()"
  end

  # Test 7: C++ lowering for arr[..end]
  def test_slice_lowering_open_end
    source = <<~MLC
      fn get_head(arr: i32[]) -> i32[] = arr[..5]
    MLC

    cpp = MLC.to_cpp(source)

    assert_includes cpp, "std::vector<int>"
    assert_includes cpp, "begin()"
  end

  # Test 8: C++ lowering for arr[..] (full copy)
  def test_slice_lowering_full_copy
    source = <<~MLC
      fn make_copy(arr: i32[]) -> i32[] = arr[..]
    MLC

    cpp = MLC.to_cpp(source)

    # Full copy should use simple vector construction
    assert_includes cpp, "std::vector<int>"
  end

  # Test 9: Slice with expressions as indices
  def test_slice_with_expression_indices
    source = <<~MLC
      fn dynamic_slice(arr: i32[], start: i32, stop: i32) -> i32[] = arr[start..stop]
    MLC

    cpp = MLC.to_cpp(source)

    assert_includes cpp, "std::vector<int>"
    assert_includes cpp, "begin()"
    # Should include the variable references
    assert_includes cpp, "start"
    assert_includes cpp, "stop"
  end

  # Test 10: Index access still works (regression)
  def test_index_access_still_works
    source = <<~MLC
      fn first(arr: i32[]) -> i32 = arr[0]
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.find { |d| d.is_a?(MLC::Source::AST::FuncDecl) && d.name == "first" }
    assert func, "Function first not found"

    index = func.body
    assert_instance_of MLC::Source::AST::IndexAccess, index
    assert_equal "arr", index.object.name
    assert_equal 0, index.index.value
  end
end
