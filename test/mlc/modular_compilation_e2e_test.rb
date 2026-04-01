# frozen_string_literal: true

require_relative "../test_helper"
require "tmpdir"
require "fileutils"
require "open3"

class ModularCompilationE2ETest < Minitest::Test
  def setup
    @tmpdir = Dir.mktmpdir("mlc_modular_e2e")
  end

  def teardown
    FileUtils.rm_rf(@tmpdir) if @tmpdir && File.exist?(@tmpdir)
  end

  # Step 19-20: Two files, generate foo.cpp, foo.hpp, bar.cpp, bar.hpp, main.cpp, main.hpp
  def test_compile_project_generates_per_module_files
    src_dir = File.join(@tmpdir, "src")
    out_dir = File.join(@tmpdir, "out")
    FileUtils.mkdir_p(src_dir)

    File.write(File.join(src_dir, "bar.mlc"), "export type Point = { x: i32, y: i32 }")
    File.write(File.join(src_dir, "foo.mlc"), <<~MLC)
      import { Point } from "./bar"
      export fn f() -> Point = Point { x: 1, y: 2 }
    MLC
    File.write(File.join(src_dir, "main.mlc"), <<~MLC)
      import { f } from "./foo"
      fn main() -> i32 = do
        let p = f()
        p.x + p.y
      end
    MLC

    result = MLC.compile_project(entry_path: File.join(src_dir, "main.mlc"), out_dir: out_dir, root_dir: src_dir)

    assert_equal 3, result[:cpp_files].length
    assert_equal 3, result[:hpp_files].length
    %w[bar foo main].each do |name|
      assert File.exist?(File.join(out_dir, "#{name}.hpp")), "missing #{name}.hpp"
      assert File.exist?(File.join(out_dir, "#{name}.cpp")), "missing #{name}.cpp"
    end
    assert_includes File.read(File.join(out_dir, "foo.hpp")), "#include \"bar.hpp\""
    assert_includes File.read(File.join(out_dir, "main.hpp")), "#include \"foo.hpp\""
  end

  # Step 21: g++ compiles and links → executable
  def test_build_project_produces_runnable_binary
    src_dir = File.join(@tmpdir, "src")
    out_dir = File.join(@tmpdir, "out")
    FileUtils.mkdir_p(src_dir)

    File.write(File.join(src_dir, "bar.mlc"), "export fn b() -> i32 = 1")
    File.write(File.join(src_dir, "app.mlc"), <<~MLC)
      import { b } from "./bar"
      fn main() -> i32 = b()
    MLC

    result = MLC.build_project(entry_path: File.join(src_dir, "app.mlc"), out_dir: out_dir, root_dir: src_dir)

    assert File.exist?(result[:binary])
    _stdout, stderr, status = Open3.capture3(result[:binary])
    assert_equal 1, status.exitstatus, "run failed: #{stderr}"
  end

  def test_cli_modular_compile
    src_dir = File.join(@tmpdir, "src")
    out_dir = File.join(@tmpdir, "out")
    FileUtils.mkdir_p(src_dir)
    File.write(File.join(src_dir, "bar.mlc"), "export fn b() -> i32 = 2")
    File.write(File.join(src_dir, "app.mlc"), <<~MLC)
      import { b } from "./bar"
      fn main() -> i32 = b()
    MLC

    cli = File.expand_path("../../bin/mlc", __dir__)
    _stdout, stderr, status = Open3.capture3(
      "bundle", "exec", "ruby", cli, "--emit-cpp", "-o", out_dir, File.join(src_dir, "app.mlc"),
      chdir: File.expand_path("../..", __dir__)
    )

    assert status.success?, "cli failed: #{stderr}"
    assert File.exist?(File.join(out_dir, "app.cpp"))
    assert File.exist?(File.join(out_dir, "bar.hpp"))
  end
end
