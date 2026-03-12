# frozen_string_literal: true

require_relative "../test_helper"
require "tmpdir"
require "fileutils"

class ModularCompilationLowerTest < Minitest::Test
  def setup
    @tmpdir = Dir.mktmpdir("mlc_modular_lower")
  end

  def teardown
    FileUtils.rm_rf(@tmpdir) if @tmpdir && File.exist?(@tmpdir)
  end

  # Step 14: Lower one module (no imports) → C++ AST
  def test_lower_single_module_no_imports_to_cpp_ast
    bar_path = File.join(@tmpdir, "bar.mlc")
    File.write(bar_path, <<~MLC)
      type Point = { x: i32, y: i32 }
      fn f() -> i32 = 42
    MLC

    ast = MLC.parse(File.read(bar_path), filename: bar_path)
    pipeline = MLC::Representations::Semantic::Gen::Pipeline.new(
      source_file_dir: @tmpdir,
      module_name: "bar"
    )
    core_ir = pipeline.transform(ast)

    stdlib_scanner = MLC::Common::Stdlib::Scanner.new
    cpp_ast = MLC.lower_to_cpp(
      core_ir,
      type_registry: pipeline.type_registry,
      function_registry: pipeline.function_registry,
      stdlib_scanner: stdlib_scanner
    )

    assert cpp_ast
    cpp_source = cpp_ast.to_source
    assert_includes cpp_source, "Point"
    assert_includes cpp_source, "f"
    assert_includes cpp_source, "42"
  end

  # Step 15: Lower module with import { T } from "./foo" — lowerer finds T in registry
  def test_lower_module_with_import_uses_type_from_registry
    bar_path = File.join(@tmpdir, "bar.mlc")
    foo_path = File.join(@tmpdir, "foo.mlc")
    File.write(bar_path, "type Point = { x: i32, y: i32 }")
    File.write(foo_path, <<~MLC)
      import { Point } from "./bar"
      fn f() -> Point = Point { x: 1, y: 2 }
    MLC

    pipeline = MLC::Representations::Semantic::Gen::Pipeline.new(source_file_dir: @tmpdir)
    pipeline.transform(MLC.parse(File.read(bar_path), filename: bar_path), module_name: "bar")
    foo_ir = pipeline.transform(MLC.parse(File.read(foo_path), filename: foo_path), module_name: "foo")

    stdlib_scanner = MLC::Common::Stdlib::Scanner.new
    cpp_ast = MLC.lower_to_cpp(
      foo_ir,
      type_registry: pipeline.type_registry,
      function_registry: pipeline.function_registry,
      stdlib_scanner: stdlib_scanner
    )

    cpp_source = cpp_ast.to_source
    assert_includes cpp_source, "Point"
    assert_includes cpp_source, "f"
    assert_includes cpp_source, "1"
    assert_includes cpp_source, "2"
  end
end
