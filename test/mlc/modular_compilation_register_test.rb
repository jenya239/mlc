# frozen_string_literal: true

require_relative "../test_helper"
require "tmpdir"
require "fileutils"

class ModularCompilationRegisterTest < Minitest::Test
  def setup
    @tmpdir = Dir.mktmpdir("mlc_modular_reg")
  end

  def teardown
    FileUtils.rm_rf(@tmpdir) if @tmpdir && File.exist?(@tmpdir)
  end
  # Step 10: register_from_ir — types and functions from SemanticIR::Module into registry
  def test_register_from_ir_puts_types_and_functions_in_registry
    pipeline = MLC::Representations::Semantic::Gen::Pipeline.new
    registrar = MLC::Common::ModularCompilation::IrModuleRegistrar.new(
      type_registry: pipeline.type_registry,
      function_registry: pipeline.function_registry,
      type_decl_table: pipeline.services.type_decl_table,
      sum_type_constructor_service: pipeline.services.sum_type_constructor_service
    )

    int_type = MLC::SemanticIR::Builder.primitive_type("i32")
    record_type = MLC::SemanticIR::Builder.record_type("Point", [{ name: "x", type: int_type }, { name: "y", type: int_type }])
    type_decl = MLC::SemanticIR::TypeDecl.new(name: "Point", type: record_type, exported: true)
    func = MLC::SemanticIR::Func.new(
      name: "add",
      params: [MLC::SemanticIR::Param.new(name: "a", type: int_type), MLC::SemanticIR::Param.new(name: "b", type: int_type)],
      ret_type: int_type,
      body: nil,
      type_params: [],
      external: false,
      exported: true
    )
    module_ir = MLC::SemanticIR::Module.new(name: "foo", items: [type_decl, func], imports: [])

    registrar.register(module_ir, module_name: "foo")

    assert pipeline.type_registry.lookup("Point"), "Point type should be registered"
    assert pipeline.function_registry.registered?("add"), "add function should be registered"
    info = pipeline.function_registry.fetch("add")
    assert_equal 2, info.param_types.length
    assert_equal int_type, info.ret_type
  end

  def test_register_from_ir_sum_type_registers_constructors
    pipeline = MLC::Representations::Semantic::Gen::Pipeline.new
    registrar = MLC::Common::ModularCompilation::IrModuleRegistrar.new(
      type_registry: pipeline.type_registry,
      function_registry: pipeline.function_registry,
      type_decl_table: pipeline.services.type_decl_table,
      sum_type_constructor_service: pipeline.services.sum_type_constructor_service
    )

    int_type = MLC::SemanticIR::Builder.primitive_type("i32")
    sum_type = MLC::SemanticIR::Builder.sum_type("Maybe", [
      { name: "Some", fields: [{ name: "value", type: int_type }] },
      { name: "None", fields: [] }
    ])
    type_decl = MLC::SemanticIR::TypeDecl.new(name: "Maybe", type: sum_type, exported: true)
    module_ir = MLC::SemanticIR::Module.new(name: "bar", items: [type_decl], imports: [])

    registrar.register(module_ir, module_name: "bar")

    assert pipeline.type_registry.lookup("Maybe")
    assert pipeline.function_registry.registered?("Some"), "Some constructor should be registered"
    assert pipeline.function_registry.registered?("None"), "None constructor should be registered"
  end

  # Step 11: Transform bar (no imports) → SemanticIR, register
  def test_transform_bar_no_imports_registers_in_registry
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

    assert pipeline.type_registry.lookup("Point"), "Point should be in registry"
    assert pipeline.function_registry.registered?("f"), "f should be in registry"
    assert_equal "Point", core_ir.items.find { |i| i.is_a?(MLC::SemanticIR::TypeDecl) }&.name
  end

  # Step 12: Transform foo (import bar) — ImportService gets bar from in-memory registry
  def test_transform_foo_import_bar_uses_in_memory_registry
    bar_path = File.join(@tmpdir, "bar.mlc")
    foo_path = File.join(@tmpdir, "foo.mlc")
    File.write(bar_path, "fn g() -> i32 = 1")
    File.write(foo_path, <<~MLC)
      import { g } from "./bar"
      fn f() -> i32 = g()
    MLC

    pipeline = MLC::Representations::Semantic::Gen::Pipeline.new(
      source_file_dir: @tmpdir,
      module_name: nil
    )
    bar_ast = MLC.parse(File.read(bar_path), filename: bar_path)
    pipeline.transform(bar_ast)

    foo_ast = MLC.parse(File.read(foo_path), filename: foo_path)
    core_ir = pipeline.transform(foo_ast, module_name: "foo")

    assert pipeline.function_registry.registered?("g"), "g from bar should be in registry"
    assert pipeline.function_registry.registered?("f"), "f from foo should be in registry"
    assert core_ir.items.any? { |i| i.is_a?(MLC::SemanticIR::Func) && i.name == "f" }
  end

  # Step 13: Transform main (import foo) — registry already contains foo, bar
  def test_transform_main_import_foo_registry_has_all
    bar_path = File.join(@tmpdir, "bar.mlc")
    foo_path = File.join(@tmpdir, "foo.mlc")
    main_path = File.join(@tmpdir, "main.mlc")
    File.write(bar_path, "fn b() -> i32 = 1")
    File.write(foo_path, <<~MLC)
      import { b } from "./bar"
      fn f() -> i32 = b()
    MLC
    File.write(main_path, <<~MLC)
      import { f } from "./foo"
      fn main() -> i32 = f()
    MLC

    pipeline = MLC::Representations::Semantic::Gen::Pipeline.new(source_file_dir: @tmpdir)
    pipeline.transform(MLC.parse(File.read(bar_path), filename: bar_path), module_name: "bar")
    pipeline.transform(MLC.parse(File.read(foo_path), filename: foo_path), module_name: "foo")
    main_ir = pipeline.transform(MLC.parse(File.read(main_path), filename: main_path), module_name: "main")

    assert pipeline.function_registry.registered?("b")
    assert pipeline.function_registry.registered?("f")
    assert pipeline.function_registry.registered?("main")
    assert main_ir.items.any? { |i| i.is_a?(MLC::SemanticIR::Func) && i.name == "main" }
  end
end
