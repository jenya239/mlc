# frozen_string_literal: true

require_relative "../test_helper"
require "tmpdir"
require "fileutils"
require_relative "../../lib/mlc/common/modular_compilation/module_graph"

class ModularCompilationModuleGraphTest < Minitest::Test
  def setup
    @tmpdir = Dir.mktmpdir("mlc_modular_test")
  end

  def teardown
    FileUtils.rm_rf(@tmpdir) if @tmpdir && File.exist?(@tmpdir)
  end

  # Step 1
  def test_resolve_import_path_same_dir
    main_path = File.join(@tmpdir, "src", "main.mlc")
    FileUtils.mkdir_p(File.dirname(main_path))
    File.write(main_path, "")

    resolved = MLC::Common::ModularCompilation::ModuleGraph.resolve_import_path(
      "./foo",
      from_file: main_path
    )
    assert_nil resolved, "foo.mlc does not exist yet"

    foo_path = File.join(@tmpdir, "src", "foo.mlc")
    File.write(foo_path, "")

    resolved = MLC::Common::ModularCompilation::ModuleGraph.resolve_import_path(
      "./foo",
      from_file: main_path
    )
    assert_equal foo_path, resolved
  end

  # Step 2
  def test_resolve_import_path_parent_dir
    main_path = File.join(@tmpdir, "src", "sub", "main.mlc")
    FileUtils.mkdir_p(File.dirname(main_path))
    bar_path = File.join(@tmpdir, "src", "lib", "bar.mlc")
    FileUtils.mkdir_p(File.dirname(bar_path))
    File.write(bar_path, "")

    resolved = MLC::Common::ModularCompilation::ModuleGraph.resolve_import_path(
      "../lib/bar",
      from_file: main_path
    )
    assert_equal bar_path, resolved
  end

  # Step 3
  def test_module_graph_single_import
    main_path = File.join(@tmpdir, "main.mlc")
    foo_path = File.join(@tmpdir, "foo.mlc")
    File.write(main_path, 'import { x } from "./foo"')
    File.write(foo_path, "fn x() -> i32 = 42")

    graph = MLC::Common::ModularCompilation::ModuleGraph.new(root_dir: @tmpdir)
    graph.add_file(main_path)

    assert_equal [foo_path], graph.graph[main_path]
    assert_equal [], graph.graph[foo_path]
  end

  # Step 4
  def test_module_graph_transitive
    main_path = File.join(@tmpdir, "main.mlc")
    foo_path = File.join(@tmpdir, "foo.mlc")
    bar_path = File.join(@tmpdir, "bar.mlc")
    File.write(main_path, 'import { f } from "./foo"')
    File.write(foo_path, 'import { b } from "./bar"')
    File.write(bar_path, "fn b() -> i32 = 1")

    graph = MLC::Common::ModularCompilation::ModuleGraph.new(root_dir: @tmpdir)
    graph.add_file(main_path)

    assert_equal [foo_path], graph.graph[main_path]
    assert_equal [bar_path], graph.graph[foo_path]
    assert_equal [], graph.graph[bar_path]
  end

  # Step 5
  def test_dependency_order
    main_path = File.join(@tmpdir, "main.mlc")
    foo_path = File.join(@tmpdir, "foo.mlc")
    bar_path = File.join(@tmpdir, "bar.mlc")
    File.write(main_path, 'import { f } from "./foo"')
    File.write(foo_path, 'import { b } from "./bar"')
    File.write(bar_path, "fn b() -> i32 = 1")

    graph = MLC::Common::ModularCompilation::ModuleGraph.new(root_dir: @tmpdir)
    graph.add_file(main_path)

    order = graph.dependency_order
    bar_idx = order.index(bar_path)
    foo_idx = order.index(foo_path)
    main_idx = order.index(main_path)

    assert bar_idx < foo_idx, "bar before foo"
    assert foo_idx < main_idx, "foo before main"
  end

  # Step 6
  def test_circular_import_raises
    main_path = File.join(@tmpdir, "main.mlc")
    foo_path = File.join(@tmpdir, "foo.mlc")
    File.write(main_path, 'import { f } from "./foo"')
    File.write(foo_path, 'import { m } from "./main"')

    graph = MLC::Common::ModularCompilation::ModuleGraph.new(root_dir: @tmpdir)
    graph.add_file(main_path)

    error = assert_raises(MLC::Common::ModularCompilation::ModuleGraph::CircularImportError) do
      graph.dependency_order
    end
    assert_includes error.cycle, main_path
    assert_includes error.cycle, foo_path
  end
end
