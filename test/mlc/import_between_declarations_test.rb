# frozen_string_literal: true

require "minitest/autorun"
require_relative "../../lib/mlc/common/index"

class ImportBetweenDeclarationsTest < Minitest::Test
  def test_import_between_functions_keeps_following_declarations
    program = MLC.parse(<<~MLC, filename: "mid_import.mlc")
      fn before() -> i32 = 1

      import * as helper from "./helper"

      fn after() -> i32 = helper.value()
    MLC

    function_names = program.declarations.select { |decl| decl.is_a?(MLC::Source::AST::FuncDecl) }.map(&:name)
    assert_equal %w[before after], function_names

    import_decl = program.imports.last
    assert_equal "./helper", import_decl.path
    assert_equal true, import_decl.import_all
    assert_equal "helper", import_decl.alias
  end

  def test_stmt_eval_keeps_functions_after_mid_file_import
    path = File.expand_path("../../compiler/codegen/stmt/stmt_eval.mlc", __dir__)
    program = MLC.parse(File.read(path), filename: path)
    function_names = program.declarations.select { |decl| decl.is_a?(MLC::Source::AST::FuncDecl) }.map(&:name)

    assert_includes function_names, "eval_try_unwrap"
    assert_includes function_names, "eval_stmt_with_try"
    assert_includes function_names, "eval_stmts_str_with_try"

    emit_import = program.imports.find { |import_decl| import_decl.alias == "emit_helpers" }
    refute_nil emit_import
    assert_equal "../../cpp_emit/emit_helpers", emit_import.path
  end
end
