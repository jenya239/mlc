# frozen_string_literal: true

require "minitest/autorun"
require_relative "../../lib/mlc/common/index"

class CompilerSourcesParseTest < Minitest::Test
  COMPILER_ROOT = File.expand_path("../../compiler", __dir__)
  SKIPPED_PREFIXES = [
    "tests/fuzz/negative_corpus/"
  ].freeze
  SKIPPED_FILES = [
    "tests/fixtures/error_union_sugar/bang_missing_error.mlc"
  ].freeze

  def test_ruby_parser_accepts_compiler_sources
    failures = []
    source_paths.each do |path|
      failure = parse_failure(path)
      failures << "#{relative_path(path)}: #{failure}" if failure
    end
    assert_empty failures
  end

  def source_paths
    Dir.glob(File.join(COMPILER_ROOT, "**", "*.mlc")).sort.reject do |path|
      relative = relative_path(path)
      SKIPPED_FILES.include?(relative) || SKIPPED_PREFIXES.any? { |prefix| relative.start_with?(prefix) }
    end
  end

  def test_region_expression_keeps_binder_and_body
    program = MLC.parse("fn main() -> i32 = region arena do\n  0\nend\n", filename: "region.mlc")
    body = program.declarations.first.body
    assert_instance_of MLC::Source::AST::RegionExpr, body
    assert_equal "arena", body.binder
    assert_instance_of MLC::Source::AST::BlockExpr, body.body
  end

  def test_region_inside_do_is_not_a_name
    program = MLC.parse("fn main() -> i32 = do\n  region r do\n    0\n  end\n  1\nend\n", filename: "region_stmt.mlc")
    block = program.declarations.first.body
    statement = block.statements.first
    assert_instance_of MLC::Source::AST::ExprStmt, statement
    assert_instance_of MLC::Source::AST::RegionExpr, statement.expr
    assert_equal "r", statement.expr.binder
  end

  def test_string_import_is_a_path
    program = MLC.parse("import \"./probe.h\"\nfn main() -> i32 = 0\n", filename: "header_import.mlc")
    assert_equal "./probe.h", program.imports.first.path
  end

  def test_extern_type_declaration
    source = "extern type Handle = \"Handle\" from \"<stdio.h>\"\nfn main() -> i32 = 0\n"
    program = MLC.parse(source, filename: "extern_type.mlc")
    extern_type = program.declarations.find { |declaration| declaration.is_a?(MLC::Source::AST::ExternTypeDecl) }
    refute_nil extern_type
    assert_equal "Handle", extern_type.name
    assert_equal "Handle", extern_type.native_type_name
    assert_equal "<stdio.h>", extern_type.header
  end

  def relative_path(path)
    path.delete_prefix("#{COMPILER_ROOT}/")
  end

  # A trailing `end` at EOF is the closer of the last block. Any earlier stop
  # drops the rest of the file: Ruby `parse_program` breaks without an error.
  def parse_failure(path)
    source = File.read(path)
    parser = MLC::Source::Parser::Parser.new(source, filename: path)
    parser.parse
    trailing = parser.trailing_token
    return nil if trailing.nil?

    following = parser.peek
    return nil if trailing.type == :END && (following.nil? || following.type == :EOF)

    following_type = following ? following.type : "none"
    "stopped at #{trailing.type} line #{trailing.line}, following #{following_type}"
  rescue StandardError => error
    "#{error.class}: #{error.message}"
  end
end
