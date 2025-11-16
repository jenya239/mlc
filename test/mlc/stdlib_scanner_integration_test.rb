# frozen_string_literal: true

require_relative '../test_helper'

class StdlibScannerIntegrationTest < Minitest::Test
  def test_compile_with_math_functions
    source = <<~MLCORA
      import Math::{ sqrt_f, sin_f }

      fn test() -> f32 =
        sqrt_f(sin_f(0.5))

      fn main() -> i32 = do
        let result = test();
        0
      end
    MLCORA

    cpp = MLC.compile(source).to_source

    # Should use proper C++ namespaced function names
    assert_includes cpp, "mlc::math::sqrt_f"
    assert_includes cpp, "mlc::math::sin_f"
  end

  def test_compile_with_graphics_functions
    source = <<~MLCORA
      import Graphics::{ create_window, flush_window }

      fn test() -> void = do
        let win = create_window(800, 600, "Test");
        flush_window(win)
      end

      fn main() -> i32 = do
        test();
        0
      end
    MLCORA

    cpp = MLC.compile(source).to_source

    # Should use proper C++ namespaced function names
    assert_includes cpp, "mlc::graphics::create_window"
    assert_includes cpp, "mlc::graphics::flush_window"
  end

  def test_imported_graphics_record_type_uses_namespace
    source = <<~MLCORA
      import { Event } from "Graphics"

      fn handle(evt: Event) -> i32 =
        evt.x

      fn main() -> i32 =
        0
    MLCORA

    cpp = MLC.compile(source).to_source

    assert_includes cpp, "int handle(mlc::graphics::Event evt)"
    refute_includes cpp, "mlc::graphics::Event*"
    assert_includes cpp, "return evt.x;"
  end

  def test_compile_with_io_functions
    source = <<~MLCORA
      import IO::{ println }

      fn main() -> i32 = do
        println("Hello, world!");
        0
      end
    MLCORA

    cpp = MLC.compile(source).to_source

    # IO functions should work
    assert_includes cpp, "mlc::io::println"
  end

  def test_compile_with_conv_functions
    source = <<~MLCORA
      import Conv::{ parse_i32, to_string_i32 }

      fn test() -> str =
        to_string_i32(42)

      fn main() -> i32 = do
        let s = test();
        0
      end
    MLCORA

    cpp = MLC.compile(source).to_source

    # Conv functions should use mlc namespace (not mlc::conv)
    assert_includes cpp, "mlc::to_string_i32"
  end

  def test_stdlib_resolver_uses_scanner
    resolver = MLC::Common::Stdlib::StdlibResolver.new

    # Should find all stdlib modules
    assert resolver.stdlib_module?('Math')
    assert resolver.stdlib_module?('Graphics')
    assert resolver.stdlib_module?('IO')
    assert resolver.stdlib_module?('Conv')
    assert resolver.stdlib_module?('String')

    # Should not find non-existent modules
    refute resolver.stdlib_module?('NonExistent')
  end

  def test_stdlib_resolver_resolve_returns_paths
    resolver = MLC::Common::Stdlib::StdlibResolver.new

    math_path = resolver.resolve('Math')
    refute_nil math_path
    assert File.exist?(math_path)
    assert math_path.end_with?('math.mlc')
  end

  def test_scanner_available_from_resolver
    resolver = MLC::Common::Stdlib::StdlibResolver.new
    scanner = resolver.scanner

    refute_nil scanner
    assert_instance_of MLC::Common::Stdlib::StdlibScanner, scanner

    # Can use scanner to look up functions
    assert_equal 'mlc::math::sqrt_f', scanner.cpp_function_name('sqrt_f')
  end

  def test_compile_with_multiple_modules
    source = <<~MLCORA
      import Math::{ sqrt_f }
      import Graphics::{ create_window }
      import IO::{ println }

      fn test() -> f32 = do
        let win = create_window(800, 600, "Test");
        let x = sqrt_f(4.0);
        println("Done");
        x
      end

      fn main() -> i32 = do
        let y = test();
        0
      end
    MLCORA

    cpp = MLC.compile(source).to_source

    # All namespaced functions should work
    assert_includes cpp, "mlc::math::sqrt_f"
    assert_includes cpp, "mlc::graphics::create_window"
    assert_includes cpp, "mlc::io::println"
  end

  def test_scanner_resolves_math_functions_without_overrides
    source = <<~MLCORA
      import Math::{ abs }

      fn test() -> i32 =
        abs(-5)

      fn main() -> i32 = do
        let x = test();
        0
      end
    MLCORA

    cpp = MLC.compile(source).to_source

    # Should use proper namespace provided by StdlibScanner
    assert_includes cpp, "mlc::math::abs"
  end

  def test_conv_to_f32_uses_static_cast_override
    source = <<~MLCORA
      import Conv::{ to_f32 }

      fn main() -> f32 =
        to_f32(42)
    MLCORA

    cpp = MLC.compile(source).to_source

    assert_includes cpp, "return static_cast<float>(42);"
  end

  def test_scanner_handles_extern_functions
    scanner = MLC::Common::Stdlib::StdlibScanner.new
    scanner.scan_all

    graphics = scanner.module_info('Graphics')

    # create_window is extern fn without export
    assert graphics.functions.key?('create_window')
    create_window = graphics.functions['create_window']
    assert create_window.extern?
    assert_equal 'mlc::graphics::create_window', create_window.qualified_name
  end

  def test_scanner_handles_export_functions
    scanner = MLC::Common::Stdlib::StdlibScanner.new
    scanner.scan_all

    graphics = scanner.module_info('Graphics')

    # is_quit_event is export fn (not extern)
    assert graphics.functions.key?('is_quit_event')
    is_quit = graphics.functions['is_quit_event']
    refute is_quit.extern?
    assert_equal 'mlc::graphics::is_quit_event', is_quit.qualified_name
  end
end
