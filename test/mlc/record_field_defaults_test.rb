# frozen_string_literal: true

require_relative "../test_helper"

class RecordFieldDefaultsTest < Minitest::Test
  def test_record_type_accepts_field_defaults_parse
    src = <<~MLC
      type Config = {
        host: string = "localhost",
        port: i32 = 8080
      }

      fn main() -> string =
        Config { }.host
    MLC
    ast = MLC.parse(src)
    type_decl = ast.declarations.find { |d| d.is_a?(MLC::Source::AST::TypeDecl) }
    assert_instance_of MLC::Source::AST::RecordType, type_decl.type
    fields = type_decl.type.fields
    assert_equal "host", fields[0][:name]
    refute_nil fields[0][:default]
    assert_equal "port", fields[1][:name]
    refute_nil fields[1][:default]
  end

  def test_empty_record_literal_uses_defaults_codegen
    src = <<~MLC
      type Config = {
        host: string = "localhost",
        port: i32 = 8080
      }

      fn main() -> string =
        Config { }.host
    MLC
    cpp = MLC.to_cpp(src)
    assert_match(/struct Config \{/, cpp)
    assert_match(/mlc::String\s+host\s*=\s*mlc::String\("localhost"\)/, cpp)
    assert_match(/int\s+port\s*=\s*8080/, cpp)
    assert_match(/Config\{\.host\s*=\s*/, cpp)
    assert_match(/\.port\s*=\s*/, cpp)
  end

  def test_partial_literal_designated_and_typedefault_merge
    src = <<~MLC
      type Config = {
        host: string = "localhost",
        port: i32 = 8080
      }

      fn main() -> i32 =
        Config { port: 9090 }.port
    MLC
    cpp = MLC.to_cpp(src)
    assert_match(/Config\{\.host\s*=/, cpp)
    assert_match(/9090/, cpp)
  end

  def test_record_field_default_arithmetic_codegen_in_struct
    src = <<~MLC
      type Line = { offset: i32 = 10 + 5 * 2 }

      fn main() -> i32 =
        Line { }.offset
    MLC
    cpp = MLC.to_cpp(src)
    assert_match(/offset\s*=\s*10\s*\+\s*5\s*\*\s*2/, cpp)
  end

  def test_generic_record_field_defaults_rejected
    src = <<~MLC
      type Box<T> = {
        value: T = 1
      }

      fn main() -> i32 = 0
    MLC
    pipeline = MLC::Representations::Semantic::Gen::Pipeline.new
    error = assert_raises(MLC::CompileError) do
      pipeline.transform(MLC.parse(src))
    end
    assert_match(/generic parameters/, error.message)
    assert_match(/default/, error.message)
  end

  def test_missing_field_without_default_errors
    src = <<~MLC
      type Point = {
        x: i32,
        y: i32 = 0
      }

      fn main() -> i32 =
        Point { }.x
    MLC
    pipeline = MLC::Representations::Semantic::Gen::Pipeline.new
    error = assert_raises(MLC::CompileError) do
      pipeline.transform(MLC.parse(src))
    end
    assert_match(/missing field/, error.message)
    assert_match(/x/, error.message)
  end
end
