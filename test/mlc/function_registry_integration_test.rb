# frozen_string_literal: true

require "test_helper"

module MLC
  class FunctionRegistryIntegrationTest < Minitest::Test
    def test_registers_user_function_metadata
      source = <<~MLCORA
        module Math::Vector

        export fn add(a: i32, b: i32) -> i32 = a + b
      MLCORA

      transformer = MLC::SemanticGen::Pipeline.new
      ast = MLC.parse(source)
      transformer.transform(ast)

      entry = transformer.function_registry.fetch_entry("add")
      refute_nil entry
      assert_equal "Math::Vector", entry.module_name
      assert_equal "math::vector", entry.namespace
      assert entry.exported?
      refute entry.external?
      assert_equal [:constexpr, :noexcept], entry.effects
    end

    def test_registers_stdlib_function_metadata
      source = <<~MLCORA
        import Math::{hypotenuse}

        fn length(a: f32, b: f32) -> f32 =
          hypotenuse(a, b)
      MLCORA

      transformer = MLC::SemanticGen::Pipeline.new
      ast = MLC.parse(source)
      transformer.transform(ast)

      entry = transformer.function_registry.fetch_entry("hypotenuse")
      refute_nil entry
      assert_equal "Math", entry.module_name
      assert_equal "mlc::math", entry.namespace
      assert entry.exported?
      refute entry.external?
    end

    def test_wildcard_stdlib_alias_generates_function_lookup
      source = <<~MLCORA
        import * as Math from "Math"

        fn calc(a: f32, b: f32) -> f32 =
          Math.hypotenuse(a, b)
      MLCORA

      transformer = MLC::SemanticGen::Pipeline.new
      ast = MLC.parse(source)
      core = transformer.transform(ast)

      calc_func = core.items.find { |item| item.is_a?(MLC::SemanticIR::Func) && item.name == "calc" }
      refute_nil calc_func
      call = calc_func.body

      assert_instance_of MLC::SemanticIR::CallExpr, call
      assert_instance_of MLC::SemanticIR::VarExpr, call.callee
      assert_equal "Math.hypotenuse", call.callee.name

      alias_entry = transformer.function_registry.fetch_entry_for_member("Math", "hypotenuse")
      refute_nil alias_entry
      assert_equal "hypotenuse", alias_entry.name
    end

    def test_wildcard_user_module_alias_resolves_member
      module_source = <<~MLCORA
        module MyMath

        export fn add(a: i32, b: i32) -> i32 = a + b
      MLCORA

      transformer = MLC::SemanticGen::Pipeline.new
      transformer.transform(MLC.parse(module_source))

      use_source = <<~MLCORA
        module Demo

        import * as Math from MyMath

        fn sum(a: i32, b: i32) -> i32 = Math.add(a, b)
      MLCORA

      core = transformer.transform(MLC.parse(use_source))
      sum_func = core.items.find { |item| item.is_a?(MLC::SemanticIR::Func) && item.name == "sum" }
      refute_nil sum_func

      call = sum_func.body
      assert_instance_of MLC::SemanticIR::CallExpr, call
      assert_instance_of MLC::SemanticIR::VarExpr, call.callee
      assert_equal "MyMath.add", call.callee.name

      alias_entry = transformer.function_registry.fetch_entry_for_member("Math", "add")
      refute_nil alias_entry
      assert_equal "add", alias_entry.name
    end

    def test_selective_user_module_import_registers_direct_alias
      module_source = <<~MLCORA
        module Geometry

        export fn area(a: f32, b: f32) -> f32 = a * b
        export fn perimeter(a: f32, b: f32) -> f32 = 2.0 * (a + b)
      MLCORA

      transformer = MLC::SemanticGen::Pipeline.new
      transformer.transform(MLC.parse(module_source))

      use_source = <<~MLCORA
        module Demo

        import Geometry::{area}

        fn square_area(x: f32) -> f32 = area(x, x)
      MLCORA

      core = transformer.transform(MLC.parse(use_source))
      func = core.items.find { |item| item.is_a?(MLC::SemanticIR::Func) && item.name == "square_area" }
      refute_nil func
      call = func.body

      assert_instance_of MLC::SemanticIR::CallExpr, call
      assert_equal "area", call.callee.name

      assert transformer.function_registry.registered?("area"), "area should remain registered"
      refute_includes transformer.function_registry.aliases_for("perimeter"), "Geometry.perimeter"
      assert_nil transformer.function_registry.fetch_member("Geometry", "perimeter"), "Member alias for non-imported function should be absent"
    end

    def test_selective_user_module_import_lowering_uses_namespace
      geometry_source = <<~MLCORA
        module Geometry

        export fn area(a: f32, b: f32) -> f32 = a * b
        export fn perimeter(a: f32, b: f32) -> f32 = 2.0 * (a + b)
      MLCORA

      demo_source = <<~MLCORA
        module Demo

        import Geometry::{area}

        fn compute(a: f32, b: f32) -> f32 =
          area(a, b)
      MLCORA

      transformer = MLC::SemanticGen::Pipeline.new
      transformer.transform(MLC.parse(geometry_source))
      core = transformer.transform(MLC.parse(demo_source))

      lowerer = Backends::Cpp::Codegen.new(
        type_registry: transformer.type_registry,
        function_registry: transformer.function_registry
      )

      cpp_program = lowerer.lower(core)
      cpp_source = cpp_program.to_source

      assert_includes cpp_source, "geometry::area"
      refute_includes cpp_source, "geometry::perimeter"

      header_generator = Backends::Cpp::HeaderGenerator.new(lowerer)
      header_result = header_generator.generate(core)
      header = header_result[:header]

      assert_includes header, '#include "geometry.hpp"'
    end
  end
end
