# frozen_string_literal: true

require_relative "../test_helper"

class ExternFfiParseTest < Minitest::Test
  def test_trailing_token_nil_when_fully_consumed
    parser = parse_source("export fn a() -> i32 = 1")
    assert_nil parser.trailing_token
  end

  def test_trailing_token_reports_stray_identifier
    parser = parse_source("export fn a() -> i32 = 1\nstray")
    assert_equal "stray", parser.trailing_token.value
  end

  def test_real_stdlib_functions_survive_extern_attributes
    {
      "lib/mlc/common/stdlib/env/env.mlc" => %w[get get_or has],
      "lib/mlc/common/stdlib/net/tcp.mlc" => %w[bind accept last_error],
      "lib/mlc/common/stdlib/io/file.mlc" => %w[is_regular_file append_text safe_rename_file],
      "lib/mlc/common/stdlib/net/https_client.mlc" => %w[https_send https_get https_post https_live_handle_count]
    }.each do |path, names|
      found = function_names(File.read(path), path)
      names.each { |name| assert_includes found, name, path }
    end
  end

  def test_stdlib_files_are_fully_consumed
    leftovers = []
    Dir.glob("lib/mlc/common/stdlib/**/*.mlc").sort.each do |path|
      parser = MLC::Source::Parser::Parser.new(File.read(path), filename: path)
      begin
        parser.parse
      rescue StandardError
        next
      end
      token = parser.trailing_token
      leftovers << "#{path}: #{token.value.inspect}" unless token.nil?
    end
    assert_empty leftovers
  end

  def test_extern_concurrency_attributes_do_not_truncate
    [
      "blocking",
      "thread_safe",
      "thread_affine",
      "thread_affine(ui)",
      "!blocking",
      "!thread_affine(ui)",
      "blocking thread_safe",
      "blocking\nthread_safe"
    ].each do |attributes|
      source = "extern fn f() -> i32 = \"c::f\" from \"h.hpp\" #{attributes}\nexport fn after() -> i32 = 1\n"
      parser, names = parse_names(source)
      assert_nil parser.trailing_token, attributes
      assert_equal %w[f after], names, attributes
    end
  end

  def test_plain_function_does_not_swallow_blocking
    parser = parse_source("fn g() -> i32 = 1\nblocking")
    assert_equal "blocking", parser.trailing_token.value
  end

  def test_mistyped_attribute_is_not_swallowed
    parser = parse_source("extern fn f() -> i32 = \"c::f\" from \"h.hpp\" blocknig")
    assert_equal "blocknig", parser.trailing_token.value
  end

  def test_extern_lib_does_not_truncate_the_following_function
    parser, names = parse_names("module Probe\nextern lib \"curl\"\nexport fn ping() -> i32 = 1\n")
    assert_nil parser.trailing_token
    assert_equal ["ping"], names
  end

  def test_extern_lib_after_attributed_extern_fn
    source = <<~MLC
      module Probe
      extern fn f() -> i32 = "c::f" from "h.hpp" blocking
      extern lib "curl"
      export fn after() -> i32 = 1
    MLC
    parser, names = parse_names(source)
    assert_nil parser.trailing_token
    assert_equal %w[f after], names
  end

  def test_extern_lib_without_string_is_an_error
    error = assert_raises(MLC::CompileError) do
      parse_source("module Probe\nextern lib\n")
    end
    assert_match(/library name string/, error.message)
  end

  private

  def parse_names(source)
    parser = MLC::Source::Parser::Parser.new(source, filename: "fixture.mlc")
    names = parser.parse.declarations.grep(MLC::Source::AST::FuncDecl).map(&:name)
    [parser, names]
  end

  def function_names(source, filename)
    parser = MLC::Source::Parser::Parser.new(source, filename: filename)
    parser.parse.declarations.grep(MLC::Source::AST::FuncDecl).map(&:name)
  end

  def parse_source(source, filename = "fixture.mlc")
    parser = MLC::Source::Parser::Parser.new(source, filename: filename)
    parser.parse
    parser
  end
end
