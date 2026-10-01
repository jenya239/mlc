# frozen_string_literal: true

require "open3"
require "test_helper"

class YamlTest < Minitest::Test
  def test_yaml_module_declares_value_and_parse
    source = File.read(File.expand_path("../../lib/mlc/common/stdlib/data/yaml.mlc", __dir__))
    assert_includes source, "type YamlValue"
    assert_includes source, "YamlNumber(f64)"
    assert_includes source, "extern lib \"yaml\""
    assert_includes source, "extern fn parse_yaml"
    assert_includes source, "extern fn stringify_yaml"
  end

  def test_libyaml_parse_and_stringify
    runtime_include = File.expand_path("../../runtime/include", __dir__)
    Dir.mktmpdir("mlc_yaml") do |directory|
      source_path = File.join(directory, "yaml_driver.cpp")
      binary_path = File.join(directory, "yaml_driver")
      File.write(source_path, driver_source)
      output, status = Open3.capture2e(
        "clang++", "-std=c++20",
        "-I", runtime_include,
        "-o", binary_path,
        source_path,
        "-lyaml"
      )
      assert status.success?, output
      run_output, run_status = Open3.capture2e(binary_path)
      assert run_status.success?, run_output
    end
  end

  def driver_source
    <<~'CPP'
      #include "mlc/yaml/yaml.hpp"
      #include <cmath>
      #include <iostream>

      static int fail(int code, const char* label) {
        std::cerr << label << "\n";
        return code;
      }

      static mlc::yaml::YamlValue parse_ok(const char* text, int code, const char* label) {
        mlc::yaml::YamlParseResult result = mlc::yaml::parse_yaml(mlc::String(text));
        if (mlc::yaml::yaml_is_failure(result)) {
          std::cerr << label << " " << std::get<mlc::yaml::YamlFailure>(result).message.c_str() << "\n";
          std::exit(code);
        }
        return std::get<mlc::yaml::YamlValue>(result);
      }

      static int expect_failure(const char* text, const char* message, int code) {
        mlc::yaml::YamlParseResult result = mlc::yaml::parse_yaml(mlc::String(text));
        if (!mlc::yaml::yaml_is_failure(result)) return code;
        if (std::get<mlc::yaml::YamlFailure>(result).message != mlc::String(message)) return code;
        return 0;
      }

      static bool same_value(const mlc::yaml::YamlValue& left, const mlc::yaml::YamlValue& right) {
        if (left.is_null()) return right.is_null();
        if (left.is_bool()) return right.is_bool() && *left.as_bool() == *right.as_bool();
        if (left.is_number()) {
          if (!right.is_number()) return false;
          double left_number = *left.as_number();
          double right_number = *right.as_number();
          if (std::isnan(left_number) && std::isnan(right_number)) return true;
          return left_number == right_number;
        }
        if (left.is_string()) return right.is_string() && *left.as_string() == *right.as_string();
        if (left.is_array()) {
          if (!right.is_array()) return false;
          auto left_items = *left.as_array();
          auto right_items = *right.as_array();
          if (left_items.size() != right_items.size()) return false;
          for (size_t index = 0; index < left_items.size(); ++index) {
            if (!same_value(left_items[index], right_items[index])) return false;
          }
          return true;
        }
        if (!left.is_object() || !right.is_object()) return false;
        auto left_object = *left.as_object();
        auto right_object = *right.as_object();
        if (left_object.size() != right_object.size()) return false;
        for (size_t index = 0; index < left_object.size(); ++index) {
          if (left_object[index].first != right_object[index].first) return false;
          if (!same_value(left_object[index].second, right_object[index].second)) return false;
        }
        return true;
      }

      int main() {
        mlc::yaml::YamlValue empty = parse_ok("", 1, "empty");
        if (!empty.is_null()) return fail(1, "empty");

        mlc::yaml::YamlValue flag = parse_ok("yes\n", 2, "yes");
        if (!flag.is_bool() || *flag.as_bool() != true) return fail(2, "yes");

        mlc::yaml::YamlValue quoted = parse_ok("\"true\"\n", 3, "quoted");
        if (!quoted.is_string() || *quoted.as_string() != mlc::String("true")) return fail(3, "quoted");

        mlc::yaml::YamlValue forced = parse_ok("!!str true\n", 4, "str tag");
        if (!forced.is_string() || *forced.as_string() != mlc::String("true")) return fail(4, "str tag");

        mlc::yaml::YamlValue document = parse_ok(
          "name: ada\ncount: 7\nready: true\nnote:\ntags:\n  - a\n  - b\n",
          5,
          "document");
        std::optional<mlc::yaml::YamlValue> name = mlc::yaml::yaml_get(document, mlc::String("name"));
        std::optional<mlc::yaml::YamlValue> count = mlc::yaml::yaml_get(document, mlc::String("count"));
        std::optional<mlc::yaml::YamlValue> ready = mlc::yaml::yaml_get(document, mlc::String("ready"));
        std::optional<mlc::yaml::YamlValue> note = mlc::yaml::yaml_get(document, mlc::String("note"));
        std::optional<mlc::yaml::YamlValue> tags = mlc::yaml::yaml_get(document, mlc::String("tags"));
        if (!name.has_value() || !name->is_string() || *name->as_string() != mlc::String("ada")) return fail(5, "name");
        if (!count.has_value() || !count->is_number() || *count->as_number() != 7.0) return fail(6, "count");
        if (!ready.has_value() || !ready->is_bool() || *ready->as_bool() != true) return fail(7, "ready");
        if (!note.has_value() || !note->is_null()) return fail(8, "note");
        if (!tags.has_value() || !tags->is_array() || tags->as_array()->size() != 2) return fail(9, "tags");

        mlc::yaml::YamlValue anchored = parse_ok("name: &label ada\ncopy: *label\n", 10, "anchor");
        std::optional<mlc::yaml::YamlValue> copy = mlc::yaml::yaml_get(anchored, mlc::String("copy"));
        if (!copy.has_value() || !copy->is_string() || *copy->as_string() != mlc::String("ada")) return fail(10, "anchor");

        mlc::yaml::YamlValue nested = parse_ok("item: &node\n  name: ada\nreuse: *node\n", 11, "nested anchor");
        std::optional<mlc::yaml::YamlValue> reuse = mlc::yaml::yaml_get(nested, mlc::String("reuse"));
        if (!reuse.has_value() || !reuse->is_object()) return fail(11, "nested anchor");
        std::optional<mlc::yaml::YamlValue> reuse_name = mlc::yaml::yaml_get(*reuse, mlc::String("name"));
        if (!reuse_name.has_value() || *reuse_name->as_string() != mlc::String("ada")) return fail(12, "nested name");

        if (expect_failure("loop: &node\n  self: *node\n", "cyclic alias", 13) != 0) return fail(13, "cycle");
        if (expect_failure("copy: *missing\n", "unknown alias", 14) != 0) return fail(14, "missing alias");
        if (expect_failure("---\na: 1\n---\nb: 2\n", "multiple documents", 15) != 0) return fail(15, "documents");
        if (expect_failure("name: !custom ada\n", "tag", 16) != 0) return fail(16, "tag");
        mlc::yaml::YamlParseResult broken = mlc::yaml::parse_yaml(mlc::String("[\n"));
        if (!mlc::yaml::yaml_is_failure(broken)) return fail(17, "syntax");

        mlc::yaml::YamlValue again = parse_ok(mlc::yaml::stringify_yaml(document).c_str(), 18, "round trip");
        if (!same_value(document, again)) return fail(18, "round trip");

        mlc::yaml::YamlValue octal = parse_ok("01\n", 21, "leading zero");
        if (!octal.is_string() || *octal.as_string() != mlc::String("01")) return fail(21, "leading zero");
        mlc::yaml::YamlValue hexadecimal = parse_ok("0x10\n", 22, "hex");
        if (!hexadecimal.is_string() || *hexadecimal.as_string() != mlc::String("0x10")) return fail(22, "hex");

        mlc::yaml::YamlValue block = parse_ok("text: |\n  hello\n  world\n", 19, "block");
        std::optional<mlc::yaml::YamlValue> block_text = mlc::yaml::yaml_get(block, mlc::String("text"));
        if (!block_text.has_value() || !block_text->is_string()) return fail(19, "block");
        if (*block_text->as_string() != mlc::String("hello\nworld\n")) return fail(20, "block text");
        return 0;
      }
    CPP
  end
end
