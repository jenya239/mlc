# frozen_string_literal: true

require "test_helper"

class JsonValueTypeSyncTest < Minitest::Test
  JSON_MLC = File.expand_path("../../lib/mlc/common/stdlib/data/json.mlc", __dir__)

  def test_json_mlc_declares_f64_number_and_map_object
    source = File.read(JSON_MLC)
    assert_includes source, "JsonNumber(f64)"
    refute_includes source, "JsonNumber(f32)"
    assert_includes source, "JsonObject(Map<str, JsonValue>)"
    refute_match(/JsonObject\(str,\s*JsonValue\)/, source)
    assert_includes source, "as_number(value: JsonValue) -> Option<f64>"
    assert_includes source, "json_number(n: f64)"
    assert_includes source, "as_object(value: JsonValue) -> Option<Map<str, JsonValue>>"
  end

  def test_json_mlc_parses_with_ruby_bootstrap
    source = File.read(JSON_MLC)
    ast = MLC.parse(source)
    assert_kind_of MLC::Source::AST::Program, ast
  end
end
