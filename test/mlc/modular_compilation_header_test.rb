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
    refute_includes result[:header], "struct Internal"
    assert_includes result[:header], "f("
    assert_includes result[:header], "internal_fn("
    refute_includes result[:header], "return 0"
    assert_includes result[:implementation], "struct Internal"
    assert_includes result[:implementation], "internal_fn"
  end

  def test_sum_alias_precedes_record_that_names_it
    path = File.join(@tmpdir, "ast_order.mlc")
    File.write(path, <<~MLC)
      export type MatchArm = { body: Shared<Expr> }
      export type Expr = Int(i32) | Add(Shared<Expr>, Shared<Expr>)
    MLC

    ast = MLC.parse(File.read(path), filename: path)
    pipeline = MLC::Representations::Semantic::Gen::Pipeline.new(source_file_dir: @tmpdir, module_name: "ast_order")
    core_ir = pipeline.transform(ast)
    lowerer = MLC::Backends::Cpp::Codegen.new(
      type_registry: pipeline.type_registry,
      function_registry: pipeline.function_registry,
      stdlib_scanner: MLC::Common::Stdlib::Scanner.new
    )
    header = MLC::Backends::Cpp::HeaderGenerator.new(lowerer).generate(core_ir)[:header]
    alias_at = header.index("using Expr =")
    record_at = header.index("struct MatchArm")
    refute_nil alias_at
    refute_nil record_at
    assert_operator alias_at, :<, record_at
  end

  def test_header_template_qualifies_imported_call_and_declares_local_helper
    helper_path = File.join(@tmpdir, "helper.mlc")
    user_path = File.join(@tmpdir, "user.mlc")
    File.write(helper_path, <<~MLC)
      export fn helper_value() -> i32 = 1
    MLC
    File.write(user_path, <<~MLC)
      import { helper_value } from "./helper"

      fn local_secret() -> i32 = 3

      export fn use_local(callback: () -> i32) -> i32 = local_secret() + helper_value() + callback()
    MLC

    pipeline = MLC::Representations::Semantic::Gen::Pipeline.new(source_file_dir: @tmpdir)
    pipeline.transform(MLC.parse(File.read(helper_path), filename: helper_path), module_name: "helper")
    user_ir = pipeline.transform(MLC.parse(File.read(user_path), filename: user_path), module_name: "user")
    lowerer = MLC::Backends::Cpp::Codegen.new(
      type_registry: pipeline.type_registry,
      function_registry: pipeline.function_registry,
      stdlib_scanner: MLC::Common::Stdlib::Scanner.new
    )
    header = MLC::Backends::Cpp::HeaderGenerator.new(lowerer).generate(user_ir)[:header]

    assert_includes header, "helper::helper_value("
    assert_includes header, "local_secret("
    assert_match(/[^\w:]local_secret\(/, header)
  end

  def test_extern_binding_wrapper_is_defined_in_cpp
    path = File.join(@tmpdir, "native.mlc")
    File.write(path, <<~MLC)
      extern fn mir_host_file_exists(path: string) -> bool =
        "mlc::file::exists_value" from "mlc/io/file_abi.hpp" blocking

      export fn use_it(path: string) -> bool = mir_host_file_exists(path)
    MLC

    pipeline = MLC::Representations::Semantic::Gen::Pipeline.new(
      source_file_dir: @tmpdir,
      module_name: "native"
    )
    core_ir = pipeline.transform(MLC.parse(File.read(path), filename: path))
    lowerer = MLC::Backends::Cpp::Codegen.new(
      type_registry: pipeline.type_registry,
      function_registry: pipeline.function_registry,
      stdlib_scanner: MLC::Common::Stdlib::Scanner.new
    )
    result = MLC::Backends::Cpp::HeaderGenerator.new(lowerer).generate(core_ir)

    assert_includes result[:header], "mir_host_file_exists("
    assert_includes result[:implementation], "mlc_ffi_bind_mir_host_file_exists"
    assert_includes result[:implementation], "mlc::file::exists_value"
  end
end
