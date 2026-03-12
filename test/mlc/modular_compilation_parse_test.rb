# frozen_string_literal: true

require_relative "../test_helper"
require "tmpdir"
require "fileutils"

class ModularCompilationParseTest < Minitest::Test
  def setup
    @tmpdir = Dir.mktmpdir("mlc_modular_parse")
  end

  def teardown
    FileUtils.rm_rf(@tmpdir) if @tmpdir && File.exist?(@tmpdir)
  end

  # Step 7: parse_file without inline
  def test_parse_file_without_inline
    foo_path = File.join(@tmpdir, "foo.mlc")
    File.write(foo_path, "fn x() -> i32 = 42")

    ast = parse_file(foo_path)
    assert_equal 1, ast.declarations.length
    assert_empty ast.imports
  end

  # Step 8: file with import parses, bar not substituted
  def test_parse_file_with_import_no_substitution
    bar_path = File.join(@tmpdir, "bar.mlc")
    main_path = File.join(@tmpdir, "main.mlc")
    File.write(bar_path, "fn y() -> i32 = 1")
    File.write(main_path, <<~MLC)
      import { y } from "./bar"
      fn main() -> i32 = y()
    MLC

    ast = parse_file(main_path)
    assert_equal 1, ast.imports.length
    assert_equal "./bar", ast.imports[0].path
    assert_equal 1, ast.declarations.length
    # bar's content not in ast — only main's declarations
    decl_names = ast.declarations.map { |d| d.respond_to?(:name) ? d.name : nil }.compact
    assert_includes decl_names, "main"
    refute_includes decl_names, "y"
  end

  # Step 9: error in bar.mlc doesn't break parse main.mlc
  def test_parse_main_independent_of_bar_error
    bar_path = File.join(@tmpdir, "bar.mlc")
    main_path = File.join(@tmpdir, "main.mlc")
    File.write(bar_path, "fn y( -> i32 = 1") # syntax error
    File.write(main_path, <<~MLC)
      import { y } from "./bar"
      fn main() -> i32 = 42
    MLC

    ast = parse_file(main_path)
    assert_equal 1, ast.imports.length
    assert_equal 1, ast.declarations.length
  end

  private

  def parse_file(path)
    source = File.read(path)
    MLC.parse(source, filename: path)
  end
end
