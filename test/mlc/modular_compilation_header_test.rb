# frozen_string_literal: true

require_relative "../test_helper"
require "tmpdir"
require "fileutils"

class ModularCompilationHeaderTest < Minitest::Test
  def setup
    @tmpdir = Dir.mktmpdir("mlc_modular_header")
  end

  def teardown
    FileUtils.rm_rf(@tmpdir) if @tmpdir && File.exist?(@tmpdir)
  end

  # Step 16: HeaderGenerator for module: types in .hpp, functions decl in .hpp, def in .cpp
  def test_header_generator_splits_types_and_functions
    bar_path = File.join(@tmpdir, "bar.mlc")
    File.write(bar_path, <<~MLC)
      export type Point = { x: i32, y: i32 }
      export fn f() -> i32 = 42
    MLC

    ast = MLC.parse(File.read(bar_path), filename: bar_path)
    pipeline = MLC::Representations::Semantic::Gen::Pipeline.new(
      source_file_dir: @tmpdir,
      module_name: "bar"
    )
    core_ir = pipeline.transform(ast)

    stdlib_scanner = MLC::Common::Stdlib::Scanner.new
    lowerer = MLC::Backends::Cpp::Codegen.new(
      type_registry: pipeline.type_registry,
      function_registry: pipeline.function_registry,
      stdlib_scanner: stdlib_scanner
    )
    generator = MLC::Backends::Cpp::HeaderGenerator.new(lowerer)
    result = generator.generate(core_ir)

    assert_includes result[:header], "Point"
    assert_includes result[:header], "f("
    assert_includes result[:implementation], "#include \"bar.hpp\""
    assert (result[:header].include?("42") || result[:implementation].include?("42")), "42 should be in header or impl"
  end

  # Step 17: HeaderGenerator: #include "foo.hpp" for each import
  # Step 18: Path "./foo" -> "foo.hpp" in include
  def test_header_generator_includes_dependencies
    bar_path = File.join(@tmpdir, "bar.mlc")
    foo_path = File.join(@tmpdir, "foo.mlc")
    File.write(bar_path, "export type Point = { x: i32, y: i32 }")
    File.write(foo_path, <<~MLC)
      import { Point } from "./bar"
      fn f() -> Point = Point { x: 0, y: 0 }
    MLC

    pipeline = MLC::Representations::Semantic::Gen::Pipeline.new(source_file_dir: @tmpdir)
    pipeline.transform(MLC.parse(File.read(bar_path), filename: bar_path), module_name: "bar")
    foo_ir = pipeline.transform(MLC.parse(File.read(foo_path), filename: foo_path), module_name: "foo")

    stdlib_scanner = MLC::Common::Stdlib::Scanner.new
    lowerer = MLC::Backends::Cpp::Codegen.new(
      type_registry: pipeline.type_registry,
      function_registry: pipeline.function_registry,
      stdlib_scanner: stdlib_scanner
    )
    generator = MLC::Backends::Cpp::HeaderGenerator.new(lowerer)
    result = generator.generate(foo_ir)

    assert_includes result[:header], "#include \"bar.hpp\""
    assert_includes result[:implementation], "#include \"bar.hpp\""
  end

  # Step 23: Export — only export types/functions in .hpp
  def test_non_exported_stays_in_cpp_only
    bar_path = File.join(@tmpdir, "bar.mlc")
    File.write(bar_path, <<~MLC)
      type Internal = { x: i32 }
      export type Point = { x: i32, y: i32 }
      fn internal_fn() -> i32 = 0
      export fn f() -> i32 = 42
    MLC

    ast = MLC.parse(File.read(bar_path), filename: bar_path)
    pipeline = MLC::Representations::Semantic::Gen::Pipeline.new(source_file_dir: @tmpdir, module_name: "bar")
    core_ir = pipeline.transform(ast)

    stdlib_scanner = MLC::Common::Stdlib::Scanner.new
    lowerer = MLC::Backends::Cpp::Codegen.new(
      type_registry: pipeline.type_registry,
      function_registry: pipeline.function_registry,
      stdlib_scanner: stdlib_scanner
    )
    result = MLC::Backends::Cpp::HeaderGenerator.new(lowerer).generate(core_ir)

    assert_includes result[:header], "Point"
    refute_includes result[:header], "Internal"
    assert_includes result[:header], "f("
    refute_includes result[:header], "internal_fn"
    assert_includes result[:implementation], "Internal"
    assert_includes result[:implementation], "internal_fn"
  end
end
