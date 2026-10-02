# frozen_string_literal: true

require "open3"
require "tmpdir"
require_relative "../test_helper"
require_relative "../../lib/mlc/common/index"

class MLCDeriveJsonTest < Minitest::Test
  def test_json_error_declared_in_stdlib
    source = File.read(File.expand_path("../../lib/mlc/common/stdlib/data/json.mlc", __dir__))
    assert_includes source, "type JsonError"
    assert_includes source, "MissingField(string)"
    assert_includes source, "TypeMismatch(string, string)"
  end

  def test_derive_json_record_generates_to_json_from_json
    cpp = MLC.to_cpp(<<~MLC)
      type Point = { x: i64, name: string } derive { Json }
      fn main() -> i32 = 0
    MLC
    assert_includes cpp, "Point_to_json"
    assert_includes cpp, "Point_from_json"
    assert_includes cpp, "mlc::json::json_set"
    assert_includes cpp, "mlc::json::JsonError"
  end

  def test_derive_json_round_trip_option_and_array
    cpp = MLC.to_cpp(<<~MLC)
      type User = {
        id: i64,
        name: string,
        email: Option<string>,
        tags: string[]
      } derive { Json }

      fn main() -> i32 = 0
    MLC

    assert_includes cpp, "User_to_json"
    assert_includes cpp, "User_from_json"
    assert_includes cpp, "self.email.has_value()"
    assert_includes cpp, "json_array"

    runtime_dir = File.expand_path("../../runtime", __dir__)
    work_root = ENV.fetch("TMPDIR", "/tmp")
    Dir.mktmpdir("mlc_derive_json", work_root) do |work_dir|
      source_path = File.join(work_dir, "derive_json.cpp")
      binary_path = File.join(work_dir, "derive_json_bin")

      body = cpp
        .gsub(/int main\(int argc, char\*\* argv\) noexcept\{return 0;\}\n?/, "")
        .gsub(/int main\(int argc, char\*\* argv\) noexcept;\n?/, "")

      full_cpp = <<~CPP
        #{body}

        int main() {
          User original{
            .id = 7,
            .name = mlc::String("Ada"),
            .email = mlc::String("ada@example.com"),
            .tags = mlc::Array<mlc::String>{mlc::String("a"), mlc::String("b")}
          };
          auto encoded = User_to_json(original);
          auto decoded = User_from_json(encoded);
          if (!std::holds_alternative<mlc::result::Ok<User>>(decoded)) {
            return 1;
          }
          User again = std::get<mlc::result::Ok<User>>(decoded)._0;
          if (again.id != 7) return 2;
          if (again.name != mlc::String("Ada")) return 3;
          if (!again.email.has_value() || *again.email != mlc::String("ada@example.com")) return 4;
          if (again.tags.size() != 2) return 5;
          if (again.tags[0] != mlc::String("a") || again.tags[1] != mlc::String("b")) return 6;

          User missing_email{
            .id = 1,
            .name = mlc::String("Bob"),
            .email = std::nullopt,
            .tags = mlc::Array<mlc::String>{}
          };
          auto encoded_missing = User_to_json(missing_email);
          auto decoded_missing = User_from_json(encoded_missing);
          if (!std::holds_alternative<mlc::result::Ok<User>>(decoded_missing)) {
            return 7;
          }
          User again_missing = std::get<mlc::result::Ok<User>>(decoded_missing)._0;
          if (again_missing.email.has_value()) return 8;
          return 0;
        }
      CPP
      File.write(source_path, full_cpp)

      compile_cmd = [
        "clang++", "-std=c++20",
        "-I", File.join(runtime_dir, "include"),
        "-o", binary_path,
        source_path
      ]
      assert system(*compile_cmd), "clang++ failed for derive Json round-trip"
      assert system(binary_path), "round-trip binary failed"
    end
  end

  def test_derive_json_call_sites_lower_without_result_namespace
    cpp = MLC.to_cpp(<<~MLC)
      import { Result } from "Result"
      import { JsonValue, JsonError } from "Json"
      type User = { id: i64 } derive { Json }
      fn round_trip(user: User) -> Result<User, JsonError> = User.from_json(user.to_json())
      fn main() -> i32 = 0
    MLC
    assert_includes cpp, "User_from_json(User_to_json(user))"
    refute_includes cpp, "mlc::result::User_from_json"
  end

  def test_derive_json_sum_generates_tagged_helpers
    cpp = MLC.to_cpp(<<~MLC)
      type Status = Active | Inactive(string) | Pair(i64, string) derive { Json }
      fn main() -> i32 = 0
    MLC
    assert_includes cpp, "Status_to_json"
    assert_includes cpp, "Status_from_json"
    assert_includes cpp, 'json_string(mlc::String("Active"))'
    assert_includes cpp, 'mlc::String("tag")'
    assert_includes cpp, 'mlc::String("value")'
    assert_includes cpp, 'mlc::String("fields")'
  end

  def test_derive_json_sum_round_trip_each_variant
    cpp = MLC.to_cpp(<<~MLC)
      type Status = Active | Inactive(string) | Pair(i64, string) derive { Json }
      fn main() -> i32 = 0
    MLC

    runtime_dir = File.expand_path("../../runtime", __dir__)
    work_root = ENV.fetch("TMPDIR", "/tmp")
    Dir.mktmpdir("mlc_derive_json_sum", work_root) do |work_dir|
      source_path = File.join(work_dir, "derive_json_sum.cpp")
      binary_path = File.join(work_dir, "derive_json_sum_bin")

      body = cpp
        .gsub(/int main\(int argc, char\*\* argv\) noexcept\{return 0;\}\n?/, "")
        .gsub(/int main\(int argc, char\*\* argv\) noexcept;\n?/, "")

      full_cpp = <<~CPP
        #{body}

        int main() {
          {
            auto encoded = Status_to_json(Active{});
            auto decoded = Status_from_json(encoded);
            if (!std::holds_alternative<mlc::result::Ok<Status>>(decoded)) return 1;
            Status again = std::get<mlc::result::Ok<Status>>(decoded)._0;
            if (!std::holds_alternative<Active>(again)) return 2;
          }
          {
            auto encoded = Status_to_json(Inactive{mlc::String("paused")});
            auto decoded = Status_from_json(encoded);
            if (!std::holds_alternative<mlc::result::Ok<Status>>(decoded)) return 11;
            Status again = std::get<mlc::result::Ok<Status>>(decoded)._0;
            if (!std::holds_alternative<Inactive>(again)) return 12;
            if (std::get<Inactive>(again).field0 != mlc::String("paused")) return 13;
          }
          {
            auto encoded = Status_to_json(Pair{42, mlc::String("x")});
            auto decoded = Status_from_json(encoded);
            if (!std::holds_alternative<mlc::result::Ok<Status>>(decoded)) return 21;
            Status again = std::get<mlc::result::Ok<Status>>(decoded)._0;
            if (!std::holds_alternative<Pair>(again)) return 22;
            if (std::get<Pair>(again).field0 != 42) return 23;
            if (std::get<Pair>(again).field1 != mlc::String("x")) return 24;
          }
          return 0;
        }
      CPP
      File.write(source_path, full_cpp)

      compile_cmd = [
        "clang++", "-std=c++20",
        "-I", File.join(runtime_dir, "include"),
        "-o", binary_path,
        source_path
      ]
      assert system(*compile_cmd), "clang++ failed for derive Json sum round-trip"
      assert system(binary_path), "sum round-trip binary failed"
    end
  end

  def test_derive_json_flat_record_and_sum_round_trip_on_ruby_and_mlcc
    source = <<~MLC
      type FlatRecord = {
        email: string,
        count: i32,
        ready: bool,
        note: Option<string>,
        tags: [string]
      } derive { Json }
      type Status = Active | Inactive(string) | Pair(i64, string) derive { Json }
      fn main() -> i32 = 0
    MLC
    checker = <<~'CPP'
      static int missing_field(
          const mlc::result::Result<FlatRecord, mlc::json::JsonError>& decoded,
          const char* field_name) {
        if (!std::holds_alternative<mlc::result::Err<mlc::json::JsonError>>(decoded)) return 1;
        auto error = std::get<mlc::result::Err<mlc::json::JsonError>>(decoded)._0;
        if (!std::holds_alternative<mlc::json::MissingField>(error)) return 1;
        if (std::get<mlc::json::MissingField>(error).field0 != mlc::String(field_name)) return 1;
        return 0;
      }
      static int type_mismatch(
          const mlc::json::JsonError& error,
          const char* field_name,
          const char* expected) {
        if (!std::holds_alternative<mlc::json::TypeMismatch>(error)) return 1;
        auto mismatch = std::get<mlc::json::TypeMismatch>(error);
        if (mismatch.field0 != mlc::String(field_name)) return 1;
        if (mismatch.field1 != mlc::String(expected)) return 1;
        return 0;
      }
      int main() {
        FlatRecord original{
          .email = mlc::String("ada@example.com"),
          .count = 7,
          .ready = true,
          .note = mlc::String("note"),
          .tags = mlc::Array<mlc::String>{mlc::String("a"), mlc::String("b")}
        };
        auto encoded = FlatRecord_to_json(original);
        auto decoded = FlatRecord_from_json(encoded);
        if (!std::holds_alternative<mlc::result::Ok<FlatRecord>>(decoded)) return 2;
        FlatRecord again = std::get<mlc::result::Ok<FlatRecord>>(decoded)._0;
        if (again.email != mlc::String("ada@example.com")) return 3;
        if (again.count != 7) return 4;
        if (!again.ready) return 5;
        if (!again.note.has_value() || *again.note != mlc::String("note")) return 6;
        if (again.tags.size() != 2) return 7;
        if (again.tags[0] != mlc::String("a") || again.tags[1] != mlc::String("b")) return 8;

        if (missing_field(FlatRecord_from_json(mlc::json::json_object()), "email") != 0) return 10;

        mlc::json::JsonValue wrong_count = mlc::json::json_object();
        wrong_count = mlc::json::json_set(wrong_count, mlc::String("email"), mlc::json::json_string(mlc::String("ada@example.com")));
        wrong_count = mlc::json::json_set(wrong_count, mlc::String("count"), mlc::json::json_string(mlc::String("nope")));
        auto wrong_count_decoded = FlatRecord_from_json(wrong_count);
        if (!std::holds_alternative<mlc::result::Err<mlc::json::JsonError>>(wrong_count_decoded)) return 11;
        if (type_mismatch(std::get<mlc::result::Err<mlc::json::JsonError>>(wrong_count_decoded)._0, "count", "number") != 0) return 12;

        mlc::json::JsonValue wrong_note = FlatRecord_to_json(original);
        wrong_note = mlc::json::json_set(wrong_note, mlc::String("note"), mlc::json::json_number(1.0));
        auto wrong_note_decoded = FlatRecord_from_json(wrong_note);
        if (!std::holds_alternative<mlc::result::Err<mlc::json::JsonError>>(wrong_note_decoded)) return 13;
        if (type_mismatch(std::get<mlc::result::Err<mlc::json::JsonError>>(wrong_note_decoded)._0, "note", "string") != 0) return 14;

        {
          auto status_encoded = Status_to_json(Active{});
          auto status_decoded = Status_from_json(status_encoded);
          if (!std::holds_alternative<mlc::result::Ok<Status>>(status_decoded)) return 20;
          if (!std::holds_alternative<Active>(std::get<mlc::result::Ok<Status>>(status_decoded)._0)) return 21;
        }
        {
          auto status_encoded = Status_to_json(Inactive{mlc::String("paused")});
          auto status_decoded = Status_from_json(status_encoded);
          if (!std::holds_alternative<mlc::result::Ok<Status>>(status_decoded)) return 22;
          Status status_again = std::get<mlc::result::Ok<Status>>(status_decoded)._0;
          if (!std::holds_alternative<Inactive>(status_again)) return 23;
          if (std::get<Inactive>(status_again).field0 != mlc::String("paused")) return 24;
        }
        {
          auto status_encoded = Status_to_json(Pair{42, mlc::String("x")});
          auto status_decoded = Status_from_json(status_encoded);
          if (!std::holds_alternative<mlc::result::Ok<Status>>(status_decoded)) return 25;
          Status status_again = std::get<mlc::result::Ok<Status>>(status_decoded)._0;
          if (!std::holds_alternative<Pair>(status_again)) return 26;
          if (std::get<Pair>(status_again).field0 != 42) return 27;
          if (std::get<Pair>(status_again).field1 != mlc::String("x")) return 28;
        }

        auto unknown_tag = Status_from_json(mlc::json::json_string(mlc::String("Missing")));
        if (!std::holds_alternative<mlc::result::Err<mlc::json::JsonError>>(unknown_tag)) return 30;
        if (type_mismatch(std::get<mlc::result::Err<mlc::json::JsonError>>(unknown_tag)._0, "tag", "known unit variant") != 0) return 31;

        mlc::json::JsonValue short_fields = mlc::json::json_object();
        short_fields = mlc::json::json_set(short_fields, mlc::String("tag"), mlc::json::json_string(mlc::String("Pair")));
        short_fields = mlc::json::json_set(short_fields, mlc::String("fields"), mlc::json::json_array(std::vector<mlc::json::JsonValue>{}));
        auto short_fields_decoded = Status_from_json(short_fields);
        if (!std::holds_alternative<mlc::result::Err<mlc::json::JsonError>>(short_fields_decoded)) return 32;
        if (type_mismatch(std::get<mlc::result::Err<mlc::json::JsonError>>(short_fields_decoded)._0, "fields", "array length 2") != 0) return 33;
        return 0;
      }
    CPP

    runtime_directory = File.expand_path("../../runtime", __dir__)
    work_root = ENV.fetch("TMPDIR", "/tmp")
    Dir.mktmpdir("mlc_derive_json_both", work_root) do |work_directory|
      ruby_source = MLC.to_cpp(source)
        .gsub(/int main\(int argc, char\*\* argv\) noexcept\{return 0;\}\n?/, "")
        .gsub(/int main\(int argc, char\*\* argv\) noexcept;\n?/, "")
      ruby_path = File.join(work_directory, "ruby_round_trip.cpp")
      ruby_binary = File.join(work_directory, "ruby_round_trip")
      File.write(ruby_path, ruby_source + "\n" + checker)
      ruby_compile = ["clang++", "-std=c++20", "-I", File.join(runtime_directory, "include"), "-o", ruby_binary, ruby_path]
      assert system(*ruby_compile), "clang++ failed for Ruby derive Json round-trip"
      assert system(ruby_binary), "Ruby derive Json round-trip failed"

      mlcc = File.expand_path("../../compiler/out/mlcc", __dir__)
      assert File.executable?(mlcc), "mlcc binary missing"
      generated_directory = File.join(work_directory, "mlcc")
      Dir.mkdir(generated_directory)
      entry_path = File.join(work_directory, "json_derive_round_trip.mlc")
      File.write(entry_path, source)
      assert system(mlcc, "-o", generated_directory, entry_path), "mlcc failed for derive Json round-trip"
      header_path = File.join(generated_directory, "json_derive_round_trip.hpp")
      assert File.file?(header_path), "mlcc did not emit json_derive_round_trip.hpp"
      mlcc_path = File.join(work_directory, "mlcc_round_trip.cpp")
      mlcc_binary = File.join(work_directory, "mlcc_round_trip")
      File.write(mlcc_path, <<~CPP)
        #include "json_derive_round_trip.hpp"
        using namespace json_derive_round_trip;
        #{checker}
      CPP
      mlcc_compile = [
        "clang++", "-std=c++20",
        "-I", File.join(runtime_directory, "include"),
        "-I", generated_directory,
        "-o", mlcc_binary,
        mlcc_path
      ]
      assert system(*mlcc_compile), "clang++ failed for mlcc derive Json round-trip"
      assert system(mlcc_binary), "mlcc derive Json round-trip failed"
    end
  end

  def test_derive_json_keyword_field_names_round_trip_on_ruby_and_mlcc
    source = <<~MLC
      type KeywordRecord = { value: string, class: string, default: i32 } derive { Json }
      fn main() -> i32 = 0
    MLC
    checker = <<~'CPP'
      int main() {
        KeywordRecord original{
          .value = mlc::String("payload"),
          .class_ = mlc::String("widget"),
          .default_ = 4
        };
        auto encoded = KeywordRecord_to_json(original);
        auto class_field = mlc::json::json_get(encoded, mlc::String("class"));
        auto default_field = mlc::json::json_get(encoded, mlc::String("default"));
        auto value_field = mlc::json::json_get(encoded, mlc::String("value"));
        if (!class_field.has_value() || !class_field->is_string()) return 1;
        if (*class_field->as_string() != mlc::String("widget")) return 2;
        if (!default_field.has_value() || !default_field->is_number()) return 3;
        if (*default_field->as_number() != 4.0) return 4;
        if (!value_field.has_value() || !value_field->is_string()) return 5;
        if (*value_field->as_string() != mlc::String("payload")) return 6;
        auto decoded = KeywordRecord_from_json(encoded);
        if (!std::holds_alternative<mlc::result::Ok<KeywordRecord>>(decoded)) return 7;
        KeywordRecord again = std::get<mlc::result::Ok<KeywordRecord>>(decoded)._0;
        if (again.value != mlc::String("payload")) return 8;
        if (again.class_ != mlc::String("widget")) return 9;
        if (again.default_ != 4) return 10;
        return 0;
      }
    CPP
    runtime_directory = File.expand_path("../../runtime", __dir__)
    work_root = ENV.fetch("TMPDIR", "/tmp")
    Dir.mktmpdir("mlc_derive_json_keywords", work_root) do |work_directory|
      ruby_source = MLC.to_cpp(source)
        .gsub(/int main\(int argc, char\*\* argv\) noexcept\{return 0;\}\n?/, "")
        .gsub(/int main\(int argc, char\*\* argv\) noexcept;\n?/, "")
      ruby_path = File.join(work_directory, "ruby_keywords.cpp")
      ruby_binary = File.join(work_directory, "ruby_keywords")
      File.write(ruby_path, ruby_source + "\n" + checker)
      ruby_compile = ["clang++", "-std=c++20", "-I", File.join(runtime_directory, "include"), "-o", ruby_binary, ruby_path]
      assert system(*ruby_compile), "clang++ failed for Ruby keyword field round-trip"
      assert system(ruby_binary), "Ruby keyword field round-trip failed"

      mlcc = File.expand_path("../../compiler/out/mlcc", __dir__)
      assert File.executable?(mlcc), "mlcc binary missing"
      generated_directory = File.join(work_directory, "mlcc")
      Dir.mkdir(generated_directory)
      entry_path = File.join(work_directory, "json_keyword_fields.mlc")
      File.write(entry_path, source)
      assert system(mlcc, "-o", generated_directory, entry_path), "mlcc failed for keyword fields"
      header_path = File.join(generated_directory, "json_keyword_fields.hpp")
      assert File.file?(header_path), "mlcc did not emit json_keyword_fields.hpp"
      mlcc_path = File.join(work_directory, "mlcc_keywords.cpp")
      mlcc_binary = File.join(work_directory, "mlcc_keywords")
      File.write(mlcc_path, <<~CPP)
        #include "json_keyword_fields.hpp"
        using namespace json_keyword_fields;
        #{checker}
      CPP
      mlcc_compile = [
        "clang++", "-std=c++20",
        "-I", File.join(runtime_directory, "include"),
        "-I", generated_directory,
        "-o", mlcc_binary,
        mlcc_path
      ]
      assert system(*mlcc_compile), "clang++ failed for mlcc keyword field round-trip"
      assert system(mlcc_binary), "mlcc keyword field round-trip failed"
    end
  end

  def test_derive_json_nested_types_round_trip_on_ruby_and_mlcc
    source = <<~MLC
      type InnerRecord = { name: string } derive { Json }
      type OuterRecord = {
        inner: InnerRecord,
        items: [InnerRecord],
        optional_inner: Option<InnerRecord>,
        matrix: [[i32]],
        flags: [Option<string>]
      } derive { Json }
      fn main() -> i32 = 0
    MLC
    checker = <<~'CPP'
      int main() {
        InnerRecord inner{.name = mlc::String("ada")};
        mlc::Array<InnerRecord> items;
        items.push_back(InnerRecord{.name = mlc::String("one")});
        items.push_back(InnerRecord{.name = mlc::String("two")});
        mlc::Array<int> row;
        row.push_back(1);
        row.push_back(2);
        mlc::Array<mlc::Array<int>> matrix;
        matrix.push_back(row);
        mlc::Array<std::optional<mlc::String>> flags;
        flags.push_back(mlc::String("on"));
        flags.push_back(std::nullopt);
        OuterRecord original{
          .inner = inner,
          .items = items,
          .optional_inner = inner,
          .matrix = matrix,
          .flags = flags
        };
        auto decoded = OuterRecord_from_json(OuterRecord_to_json(original));
        if (!std::holds_alternative<mlc::result::Ok<OuterRecord>>(decoded)) return 1;
        OuterRecord again = std::get<mlc::result::Ok<OuterRecord>>(decoded)._0;
        if (again.inner.name != mlc::String("ada")) return 2;
        if (again.items.size() != 2) return 3;
        if (again.items[0].name != mlc::String("one") || again.items[1].name != mlc::String("two")) return 4;
        if (!again.optional_inner.has_value() || again.optional_inner->name != mlc::String("ada")) return 5;
        if (again.matrix.size() != 1 || again.matrix[0].size() != 2) return 6;
        if (again.matrix[0][0] != 1 || again.matrix[0][1] != 2) return 7;
        if (again.flags.size() != 2) return 8;
        if (!again.flags[0].has_value() || *again.flags[0] != mlc::String("on")) return 9;
        if (again.flags[1].has_value()) return 10;
        original.optional_inner = std::nullopt;
        auto decoded_none = OuterRecord_from_json(OuterRecord_to_json(original));
        if (!std::holds_alternative<mlc::result::Ok<OuterRecord>>(decoded_none)) return 11;
        if (std::get<mlc::result::Ok<OuterRecord>>(decoded_none)._0.optional_inner.has_value()) return 12;
        return 0;
      }
    CPP
    compile_both_compilers(source, "json_nested_types", checker)
  end

  def test_derive_json_rejects_map_shared_and_function_fields
    map_source = <<~MLC
      type DictionaryHolder = { table: Map<str, string> } derive { Json }
      fn main() -> i32 = 0
    MLC
    shared_source = <<~MLC
      type SharedHolder = { item: Shared<string> } derive { Json }
      fn main() -> i32 = 0
    MLC
    function_source = <<~MLC
      type FunctionHolder = { callback: fn(i32) -> i32 } derive { Json }
      fn main() -> i32 = 0
    MLC
    generic_source = <<~MLC
      type Box<T> = { item: T } derive { Json }
      fn main() -> i32 = 0
    MLC
    assert_json_field_rejected(map_source, "table", "Map")
    assert_json_field_rejected(shared_source, "item", "Shared")
    assert_json_field_rejected(function_source, "callback", "function")

    error = assert_raises(MLC::CompileError) { MLC.to_cpp(generic_source) }
    assert_includes error.message, "derive Json is not supported for generic types"
    mlcc = File.expand_path("../../compiler/out/mlcc", __dir__)
    work_root = ENV.fetch("TMPDIR", "/tmp")
    Dir.mktmpdir("mlc_derive_json_generic", work_root) do |work_directory|
      entry_path = File.join(work_directory, "generic.mlc")
      File.write(entry_path, generic_source)
      output, status = Open3.capture2e(mlcc, "-o", work_directory, entry_path)
      refute status.success?, "mlcc accepted generic derive Json"
      assert_includes output, "derive Json is not supported for generic types"
      assert_includes output, "E072"
    end
  end

  def test_derive_json_integer_bounds_on_ruby_and_mlcc
    source = <<~MLC
      type NumberSample = { count: i32, wide: i64, width: u32 } derive { Json }
      fn main() -> i32 = 0
    MLC
    checker = <<~'CPP'
      static int integer_mismatch(
          const mlc::result::Result<NumberSample, mlc::json::JsonError>& decoded) {
        if (!std::holds_alternative<mlc::result::Err<mlc::json::JsonError>>(decoded)) return 1;
        auto error = std::get<mlc::result::Err<mlc::json::JsonError>>(decoded)._0;
        if (!std::holds_alternative<mlc::json::TypeMismatch>(error)) return 1;
        auto mismatch = std::get<mlc::json::TypeMismatch>(error);
        if (mismatch.field0 != mlc::String("count")) return 1;
        if (mismatch.field1 != mlc::String("integer")) return 1;
        return 0;
      }
      int main() {
        NumberSample original{.count = 7, .wide = 42, .width = 9};
        auto decoded = NumberSample_from_json(NumberSample_to_json(original));
        if (!std::holds_alternative<mlc::result::Ok<NumberSample>>(decoded)) return 1;
        NumberSample again = std::get<mlc::result::Ok<NumberSample>>(decoded)._0;
        if (again.count != 7 || again.wide != 42 || again.width != 9) return 2;

        auto fraction = NumberSample_to_json(original);
        fraction = mlc::json::json_set(fraction, mlc::String("count"), mlc::json::json_number(1.5));
        if (integer_mismatch(NumberSample_from_json(fraction)) != 0) return 3;

        auto too_wide = NumberSample_to_json(original);
        too_wide = mlc::json::json_set(too_wide, mlc::String("count"), mlc::json::json_number(3000000000.0));
        if (integer_mismatch(NumberSample_from_json(too_wide)) != 0) return 4;

        auto not_a_number = NumberSample_to_json(original);
        not_a_number = mlc::json::json_set(not_a_number, mlc::String("count"), mlc::json::json_number(0.0 / 0.0));
        if (integer_mismatch(NumberSample_from_json(not_a_number)) != 0) return 5;

        NumberSample beyond_mantissa = original;
        beyond_mantissa.wide = 9007199254740993LL;
        auto mantissa = NumberSample_from_json(NumberSample_to_json(beyond_mantissa));
        if (!std::holds_alternative<mlc::result::Ok<NumberSample>>(mantissa)) return 6;
        if (std::get<mlc::result::Ok<NumberSample>>(mantissa)._0.wide != 9007199254740992LL) return 7;
        return 0;
      }
    CPP
    compile_both_compilers(source, "json_number_bounds", checker)
  end

  def test_derive_json_remaining_integer_widths_on_ruby_and_mlcc
    source = <<~MLC
      type WidthSample = { tiny: i8, small: i16, byte: u8, half: u16, huge: u64, index: usize } derive { Json }
      fn main() -> i32 = 0
    MLC
    checker = <<~'CPP'
      static int integer_mismatch(
          const mlc::result::Result<WidthSample, mlc::json::JsonError>& decoded,
          const char* field_name) {
        if (!std::holds_alternative<mlc::result::Err<mlc::json::JsonError>>(decoded)) return 1;
        auto error = std::get<mlc::result::Err<mlc::json::JsonError>>(decoded)._0;
        if (!std::holds_alternative<mlc::json::TypeMismatch>(error)) return 1;
        auto mismatch = std::get<mlc::json::TypeMismatch>(error);
        if (mismatch.field0 != mlc::String(field_name)) return 1;
        if (mismatch.field1 != mlc::String("integer")) return 1;
        return 0;
      }
      int main() {
        WidthSample original{
          .tiny = static_cast<int8_t>(7),
          .small = static_cast<int16_t>(300),
          .byte = static_cast<uint8_t>(200),
          .half = static_cast<uint16_t>(40000),
          .huge = static_cast<uint64_t>(42),
          .index = static_cast<size_t>(9)
        };
        auto decoded = WidthSample_from_json(WidthSample_to_json(original));
        if (!std::holds_alternative<mlc::result::Ok<WidthSample>>(decoded)) return 1;
        WidthSample again = std::get<mlc::result::Ok<WidthSample>>(decoded)._0;
        if (again.tiny != 7 || again.small != 300 || again.byte != 200) return 2;
        if (again.half != 40000 || again.huge != 42 || again.index != 9) return 2;

        auto too_tiny = WidthSample_to_json(original);
        too_tiny = mlc::json::json_set(too_tiny, mlc::String("tiny"), mlc::json::json_number(128.0));
        if (integer_mismatch(WidthSample_from_json(too_tiny), "tiny") != 0) return 3;

        auto negative_half = WidthSample_to_json(original);
        negative_half = mlc::json::json_set(negative_half, mlc::String("half"), mlc::json::json_number(-1.0));
        if (integer_mismatch(WidthSample_from_json(negative_half), "half") != 0) return 4;

        auto negative_huge = WidthSample_to_json(original);
        negative_huge = mlc::json::json_set(negative_huge, mlc::String("huge"), mlc::json::json_number(-1.0));
        if (integer_mismatch(WidthSample_from_json(negative_huge), "huge") != 0) return 5;
        return 0;
      }
    CPP
    compile_both_compilers(source, "json_integer_widths", checker)
  end

  def test_derive_json_cyclic_sums_round_trip_on_ruby_and_mlcc
    source = <<~MLC
      type Tree = Leaf | Node(Tree, Tree) derive { Json }
      type Left = EndLeft | GoLeft(Right) derive { Json }
      type Right = StopRight | GoRight(Left) derive { Json }
      fn main() -> i32 = 0
    MLC
    checker = <<~'CPP'
      int main() {
        Tree leaf{Leaf{}};
        auto leaf_json = Tree_to_json(leaf);
        if (!leaf_json.is_string() || *leaf_json.as_string() != mlc::String("Leaf")) return 1;
        auto leaf_decoded = Tree_from_json(leaf_json);
        if (!std::holds_alternative<mlc::result::Ok<Tree>>(leaf_decoded)) return 2;
        if (!std::holds_alternative<Leaf>(std::get<mlc::result::Ok<Tree>>(leaf_decoded)._0._)) return 3;

        Tree tree{Node{std::make_shared<Tree>(Leaf{}), std::make_shared<Tree>(Leaf{})}};
        auto tree_json = Tree_to_json(tree);
        if (!tree_json.is_object()) return 4;
        auto decoded = Tree_from_json(tree_json);
        if (!std::holds_alternative<mlc::result::Ok<Tree>>(decoded)) return 5;
        Tree again = std::get<mlc::result::Ok<Tree>>(decoded)._0;
        if (!std::holds_alternative<Node>(again._)) return 6;
        Node node = std::get<Node>(again._);
        if (!node.field0 || !node.field1) return 7;
        if (!std::holds_alternative<Leaf>(node.field0->_)) return 8;
        if (!std::holds_alternative<Leaf>(node.field1->_)) return 9;

        Left left{GoLeft{std::make_shared<Right>(StopRight{})}};
        auto left_decoded = Left_from_json(Left_to_json(left));
        if (!std::holds_alternative<mlc::result::Ok<Left>>(left_decoded)) return 10;
        Left left_again = std::get<mlc::result::Ok<Left>>(left_decoded)._0;
        if (!std::holds_alternative<GoLeft>(left_again._)) return 11;
        GoLeft step = std::get<GoLeft>(left_again._);
        if (!step.field0 || !std::holds_alternative<StopRight>(step.field0->_)) return 12;
        return 0;
      }
    CPP
    compile_both_compilers(source, "json_cyclic_sums", checker)
  end

  def test_cyclic_sum_match_binding_is_the_sum_on_ruby_and_mlcc
    source = <<~MLC
      type Tree = Leaf | Node(Tree, Tree)
      fn child_is_leaf(tree: Tree) -> i32 =
        match tree {
          Leaf => 0,
          Node(left, right) =>
            match left {
              Leaf => 1,
              Node(_, _) => 2
            }
        }
      fn main() -> i32 = 0
    MLC
    checker = <<~'CPP'
      int main() {
        Tree leaf{Leaf{}};
        if (child_is_leaf(leaf) != 0) return 1;
        Tree tree{Node{std::make_shared<Tree>(Leaf{}), std::make_shared<Tree>(Leaf{})}};
        if (child_is_leaf(tree) != 1) return 2;
        Tree inner{Node{std::make_shared<Tree>(Leaf{}), std::make_shared<Tree>(Leaf{})}};
        Tree outer{Node{std::make_shared<Tree>(std::move(inner)), std::make_shared<Tree>(Leaf{})}};
        if (child_is_leaf(outer) != 2) return 3;
        return 0;
      }
    CPP
    compile_both_compilers(source, "cyclic_sum_match", checker, link_user_translation: true)
  end

  def test_cyclic_sum_constructor_in_mlc_runs_on_ruby_and_mlcc
    source = <<~MLC
      type Tree = Leaf | Node(Tree, Tree)
      fn child_is_leaf(tree: Tree) -> i32 =
        match tree {
          Leaf => 0,
          Node(left, right) =>
            match left {
              Leaf => 1,
              Node(_, _) => 2
            }
        }
      fn main() -> i32 = child_is_leaf(Node(Leaf, Leaf))
    MLC
    runtime_directory = File.expand_path("../../runtime", __dir__)
    runtime_sources = [
      File.join(runtime_directory, "src/io/io.cpp"),
      File.join(runtime_directory, "src/core/string.cpp"),
      File.join(runtime_directory, "src/core/profile.cpp")
    ]
    work_root = ENV.fetch("TMPDIR", "/tmp")
    Dir.mktmpdir("mlc_cyclic_construct", work_root) do |work_directory|
      ruby_source = MLC.to_cpp(source)
      refute_includes ruby_source, "mlc_main::"
      ruby_path = File.join(work_directory, "ruby.cpp")
      ruby_binary = File.join(work_directory, "ruby_binary")
      File.write(ruby_path, ruby_source)
      assert system(
        "clang++", "-std=c++20", "-I", File.join(runtime_directory, "include"),
        "-o", ruby_binary, ruby_path, *runtime_sources
      ), "clang++ failed for Ruby cyclic constructor"
      system(ruby_binary)
      assert_equal 1, $?.exitstatus

      mlcc = File.expand_path("../../compiler/out/mlcc", __dir__)
      generated_directory = File.join(work_directory, "mlcc")
      Dir.mkdir(generated_directory)
      entry_path = File.join(work_directory, "cyclic_construct.mlc")
      File.write(entry_path, source)
      assert system(mlcc, "-o", generated_directory, entry_path), "mlcc failed for cyclic constructor"
      mlcc_binary = File.join(work_directory, "mlcc_binary")
      assert system(
        "clang++", "-std=c++20",
        "-I", File.join(runtime_directory, "include"),
        "-I", generated_directory,
        "-o", mlcc_binary,
        File.join(generated_directory, "cyclic_construct.cpp"),
        *runtime_sources
      ), "clang++ failed for mlcc cyclic constructor"
      system(mlcc_binary)
      assert_equal 1, $?.exitstatus

      header_pair = MLC.to_hpp_cpp(source, filename: "cyclic_construct.mlc")
      assert_includes header_pair[:header], "namespace mlc_main"
      assert_includes header_pair[:implementation], "Tree(Leaf{})"
      refute_includes header_pair[:implementation], "mlc_main::"
    end
  end

  def test_cyclic_sum_display_eq_ord_follow_the_sum_on_ruby_and_mlcc
    plain = MLC.to_cpp("type Status = On | Off derive { Display, Eq, Ord }\nfn main() -> i32 = 0\n")
    assert_includes plain, "return a._ == b._;"
    assert_includes plain, "return a._ < b._;"
    refute_includes plain, "._.index()"

    source = <<~MLC
      type Tree = Leaf | Node(Tree, Tree) derive { Display, Eq, Ord }
      type Item = End | Box(i32, Item) derive { Display, Eq, Ord }
      type Left = StopLeft | GoLeft(Right) derive { Display, Eq }
      type Right = StopRight | GoRight(Left) derive { Display, Eq }
      fn main() -> i32 = 0
    MLC
    checker = <<~'CPP'
      int main() {
        Tree left{Node{std::make_shared<Tree>(Tree(Leaf{})), std::make_shared<Tree>(Tree(Leaf{}))}};
        Tree right{Node{std::make_shared<Tree>(Tree(Leaf{})), std::make_shared<Tree>(Tree(Leaf{}))}};
        if (!(left == right)) return 1;
        if (left < right || right < left) return 2;
        Tree deeper{Node{std::make_shared<Tree>(Tree(Leaf{})), std::make_shared<Tree>(std::move(left))}};
        if (deeper == right) return 3;
        if (!(right < deeper)) return 4;
        if (deeper < right) return 5;
        if (Tree_to_string(right) != mlc::String("Node(Leaf, Leaf)")) return 6;
        if (Tree_to_string(deeper) != mlc::String("Node(Leaf, Node(Leaf, Leaf))")) return 7;
        Item first{Box{1, std::make_shared<Item>(Item(End{}))}};
        Item second{Box{1, std::make_shared<Item>(Item(End{}))}};
        if (!(first == second)) return 8;
        Item third{Box{2, std::make_shared<Item>(Item(End{}))}};
        if (!(first < third)) return 9;
        if (Item_to_string(first) != mlc::String("Box(1, End)")) return 10;
        Left step{GoLeft{std::make_shared<Right>(Right(StopRight{}))}};
        Left again{GoLeft{std::make_shared<Right>(Right(StopRight{}))}};
        if (!(step == again)) return 11;
        if (Left_to_string(step) != mlc::String("GoLeft(StopRight)")) return 12;
        return 0;
      }
    CPP
    compile_both_compilers(source, "cyclic_sum_display", checker)
  end

  def test_cyclic_sum_hash_follows_the_sum_on_ruby_and_mlcc
    plain = MLC.to_cpp("type Shape = Circle(i32) | Empty derive { Hash }\nfn main() -> i32 = 0\n")
    assert_includes plain, "std::get<Circle>(self._).field0"
    refute_includes plain, "Shape_hash"

    wrap = <<~MLC
      type Tree = Leaf | Node(Tree, Tree)
      type Wrap = { tree: Tree } derive { Hash }
      fn main() -> i32 = 0
    MLC
    error = assert_raises(MLC::CompileError) { MLC.to_cpp(wrap) }
    assert_includes error.message, "derive Hash: unsupported field type for \"tree\""

    source = <<~MLC
      type Tree = Leaf | Node(Tree, Tree) derive { Hash, Eq }
      type Item = End | Box(i32, Item) derive { Hash }
      type Left = StopLeft | GoLeft(Right) derive { Hash }
      type Right = StopRight | GoRight(Left) derive { Hash }
      fn main() -> i32 = 0
    MLC
    checker = <<~'CPP'
      int main() {
        Tree left{Node{std::make_shared<Tree>(Tree(Leaf{})), std::make_shared<Tree>(Tree(Leaf{}))}};
        Tree right{Node{std::make_shared<Tree>(Tree(Leaf{})), std::make_shared<Tree>(Tree(Leaf{}))}};
        if (Tree_hash(left) != Tree_hash(right)) return 1;
        if (!(left == right)) return 2;
        Tree deeper{Node{std::make_shared<Tree>(Tree(Leaf{})), std::make_shared<Tree>(std::move(left))}};
        if (Tree_hash(deeper) == Tree_hash(right)) return 3;
        Item first{Box{1, std::make_shared<Item>(Item(End{}))}};
        Item second{Box{1, std::make_shared<Item>(Item(End{}))}};
        if (Item_hash(first) != Item_hash(second)) return 4;
        Item third{Box{2, std::make_shared<Item>(Item(End{}))}};
        if (Item_hash(first) == Item_hash(third)) return 5;
        Left step{GoLeft{std::make_shared<Right>(Right(StopRight{}))}};
        Left again{GoLeft{std::make_shared<Right>(Right(StopRight{}))}};
        if (Left_hash(step) != Left_hash(again)) return 6;
        return 0;
      }
    CPP
    compile_both_compilers(source, "cyclic_sum_hash", checker)
  end

  def test_shared_field_does_not_make_a_sum_cyclic_on_ruby_and_mlcc
    source = <<~MLC
      type Expr = Lit(i32) | Add(Shared<Expr>, Shared<Expr>)
      fn main() -> i32 = 0
    MLC
    ruby_source = MLC.to_cpp(source)
    assert_includes ruby_source, "using Expr = std::variant<Lit, Add>"
    refute_includes ruby_source, "struct Expr {"

    tree_source = MLC.to_cpp("type Tree = Leaf | Node(Tree, Tree)\nfn main() -> i32 = 0\n")
    assert_includes tree_source, "struct Tree {"
    assert_includes tree_source, "std::shared_ptr<Tree>"

    work_root = ENV.fetch("TMPDIR", "/tmp")
    Dir.mktmpdir("mlc_shared_sum", work_root) do |work_directory|
      mlcc = File.expand_path("../../compiler/out/mlcc", __dir__)
      generated_directory = File.join(work_directory, "mlcc")
      Dir.mkdir(generated_directory)
      entry_path = File.join(work_directory, "shared_sum.mlc")
      File.write(entry_path, source)
      assert system(mlcc, "-o", generated_directory, entry_path), "mlcc failed for shared sum"
      header = File.read(File.join(generated_directory, "shared_sum.hpp"))
      assert_includes header, "using Expr = std::variant<Lit, Add>"
      refute_includes header, "struct Expr {"
    end

    compile_both_compilers(source, "shared_sum", "int main() { return 0; }\n")
  end

  def test_option_and_array_of_a_cyclic_sum_box_the_sum_on_ruby_and_mlcc
    source = <<~MLC
      import { Option } from "Option"
      type Tree = Leaf | Node(Option<Tree>) | Kids([Tree])
      fn with_child() -> Tree = Node(Some(Leaf))
      fn without_child() -> Tree = Node(None)
      fn with_kids() -> Tree = Kids([Leaf])
      fn count(tree: Tree) -> i32 = match tree
        | Kids(items) => items.length()
        | Node(_) => 2
        | Leaf => 0
      fn main() -> i32 = 0
    MLC
    ruby_source = MLC.to_cpp(source)
    assert_includes ruby_source, "std::optional<std::shared_ptr<Tree>>"
    assert_includes ruby_source, "mlc::Array<std::shared_ptr<Tree>>"
    refute_includes ruby_source, "std::optional<Tree>"
    refute_includes ruby_source, "mlc::Array<Tree>"
    assert_includes ruby_source, "std::make_shared<Tree>(Tree(Leaf{}))"

    plain_option = MLC.to_cpp("type Box = Empty | Hold(Option<i32>)\nfn main() -> i32 = 0\n")
    assert_includes plain_option, "std::optional<int>"
    refute_includes plain_option, "shared_ptr"

    checker = <<~CPP
      int main() {
        Tree kids_value = with_kids();
        if (!std::holds_alternative<Kids>(kids_value._)) return 1;
        if (std::get<Kids>(kids_value._).field0.size() != 1) return 2;
        if (!std::get<Kids>(kids_value._).field0[0]) return 3;
        if (!std::holds_alternative<Leaf>(std::get<Kids>(kids_value._).field0[0]->_)) return 4;
        if (count(kids_value) != 1) return 5;

        Tree with_child_value = with_child();
        if (!std::holds_alternative<Node>(with_child_value._)) return 6;
        const Node& with_child_node = std::get<Node>(with_child_value._);
        if (!with_child_node.field0.has_value() || !(*with_child_node.field0)) return 7;
        if (!std::holds_alternative<Leaf>((*with_child_node.field0)->_)) return 8;

        Tree without_child_value = without_child();
        if (!std::holds_alternative<Node>(without_child_value._)) return 9;
        if (std::get<Node>(without_child_value._).field0.has_value()) return 10;
        if (count(without_child_value) != 2) return 11;
        return 0;
      }
    CPP
    compile_both_compilers(source, "cyclic_sum_option_array", checker, link_user_translation: true)
  end

  def test_derive_of_option_and_array_fields_of_a_cyclic_sum_reads_the_sum_on_ruby_and_mlcc
    source = <<~MLC
      import { Option } from "Option"
      type Tree = Leaf | Node(Option<Tree>) | Kids([Tree]) derive { Json, Display, Eq, Ord, Hash }
      fn main() -> i32 = 0
    MLC
    ruby_source = MLC.to_cpp(source)
    assert_includes ruby_source, "Tree_to_json(*(*"
    assert_includes ruby_source, "Tree_to_string(*(*"
    assert_includes ruby_source, "Tree_hash(*(*"
    assert_includes ruby_source, "Tree_to_json(*item)"
    assert_includes ruby_source, "std::optional<std::shared_ptr<Tree>>"
    assert_includes ruby_source, "mlc::Array<std::shared_ptr<Tree>>"

    checker = <<~'CPP'
      int main() {
        Tree leaf = Tree(Leaf{});
        Tree node = Tree(Node{std::optional<std::shared_ptr<Tree>>(std::make_shared<Tree>(leaf))});
        Tree node_none = Tree(Node{std::optional<std::shared_ptr<Tree>>{}});
        Tree kids = Tree(Kids{mlc::Array<std::shared_ptr<Tree>>{std::make_shared<Tree>(leaf)}});
        Tree kids_empty = Tree(Kids{mlc::Array<std::shared_ptr<Tree>>{}});
        auto node_decoded = Tree_from_json(Tree_to_json(node));
        if (!std::holds_alternative<mlc::result::Ok<Tree>>(node_decoded)) return 1;
        Tree node_again = std::get<mlc::result::Ok<Tree>>(node_decoded)._0;
        if (!(node == node_again)) return 2;
        if (Tree_hash(node) != Tree_hash(node_again)) return 3;
        if (Tree_to_string(node) != mlc::String("Node(Some(Leaf))")) return 4;
        if (Tree_to_string(node_none) != mlc::String("Node(None)")) return 5;
        if (node_none == node) return 6;
        if (!(node_none < node)) return 7;
        auto none_decoded = Tree_from_json(Tree_to_json(node_none));
        if (!std::holds_alternative<mlc::result::Ok<Tree>>(none_decoded)) return 8;
        if (!(node_none == std::get<mlc::result::Ok<Tree>>(none_decoded)._0)) return 9;
        auto kids_decoded = Tree_from_json(Tree_to_json(kids));
        if (!std::holds_alternative<mlc::result::Ok<Tree>>(kids_decoded)) return 10;
        Tree kids_again = std::get<mlc::result::Ok<Tree>>(kids_decoded)._0;
        if (!(kids == kids_again)) return 11;
        if (Tree_hash(kids) != Tree_hash(kids_again)) return 12;
        if (Tree_to_string(kids) != mlc::String("Kids([Leaf])")) return 13;
        if (Tree_to_string(kids_empty) != mlc::String("Kids([])")) return 14;
        if (!(kids_empty < kids)) return 15;
        if (kids < node) return 16;
        return 0;
      }
    CPP
    compile_both_compilers(source, "cyclic_sum_derive_containers", checker)
  end

  def test_cyclic_option_match_and_array_element_read_the_sum_on_ruby_and_mlcc
    source = <<~MLC
      import { Option } from "Option"
      type Tree = Leaf | Node(Option<Tree>) | Kids([Tree])
      fn label(tree: Tree) -> i32 = match tree
        | Leaf => 1
        | Node(_) => 2
        | Kids(_) => 3
      fn from_some(child: Option<Tree>) -> i32 = match child
        | Some(tree) => label(tree)
        | None => 0
      fn first_kid(items: [Tree]) -> i32 = label(items[0])
      fn present() -> Option<Tree> = Some(Leaf)
      fn missing() -> Option<Tree> = None
      fn kids() -> [Tree] = [Leaf]
      fn main() -> i32 = 0
    MLC
    ruby_source = MLC.to_cpp(source)
    assert_includes ruby_source, "*(*"
    assert_includes ruby_source, ".has_value()"

    checker = <<~CPP
      int main() {
        if (from_some(present()) != 1) return 1;
        if (from_some(missing()) != 0) return 2;
        if (first_kid(kids()) != 1) return 3;
        return 0;
      }
    CPP
    compile_both_compilers(source, "cyclic_sum_option_match", checker, link_user_translation: true)
  end

  def test_nested_some_on_a_cyclic_sum_reads_the_sum_on_ruby_and_mlcc
    source = <<~MLC
      import { Option } from "Option"
      type Tree = Leaf | Node(Option<Tree>) | Kids([Tree])
      fn label(tree: Tree) -> i32 = match tree
        | Leaf => 1
        | Node(_) => 2
        | Kids(_) => 3
      fn nested(tree: Tree) -> i32 = match tree
        | Node(Some(child)) => label(child)
        | Node(None) => 0
        | Leaf => 4
        | Kids(_) => 5
      fn present() -> Tree = Node(Some(Leaf))
      fn missing() -> Tree = Node(None)
      fn main() -> i32 = 0
    MLC
    ruby_source = MLC.to_cpp(source)
    assert_includes ruby_source, "has_value()"
    assert_includes ruby_source, "*(*"
    refute_includes ruby_source, "holds_alternative<Some>"
    refute_includes ruby_source, "auto [None]"

    checker = <<~CPP
      int main() {
        if (nested(present()) != 1) return 1;
        if (nested(missing()) != 0) return 2;
        if (nested(Tree(Leaf{})) != 4) return 3;
        return 0;
      }
    CPP
    compile_both_compilers(source, "cyclic_sum_nested_some", checker, link_user_translation: true)
  end

  def test_nested_some_constructor_on_a_cyclic_sum_reads_the_variant_on_ruby_and_mlcc
    source = <<~MLC
      import { Option } from "Option"
      type Tree = Leaf | Node(Option<Tree>) | Kids([Tree])
      fn kind(tree: Tree) -> i32 = match tree
        | Node(Some(Leaf)) => 1
        | Node(Some(_)) => 2
        | Node(None) => 0
        | Leaf => 4
        | Kids(_) => 5
      fn present() -> Tree = Node(Some(Leaf))
      fn deeper() -> Tree = Node(Some(Node(None)))
      fn missing() -> Tree = Node(None)
      fn main() -> i32 = 0
    MLC
    ruby_source = MLC.to_cpp(source)
    assert_includes ruby_source, "holds_alternative<Leaf>"
    assert_includes ruby_source, "(*(*"
    refute_includes ruby_source, "holds_alternative<Some>"

    checker = <<~CPP
      int main() {
        if (kind(present()) != 1) return 1;
        if (kind(deeper()) != 2) return 2;
        if (kind(missing()) != 0) return 3;
        if (kind(Tree(Leaf{})) != 4) return 4;
        return 0;
      }
    CPP
    compile_both_compilers(source, "cyclic_sum_nested_leaf", checker, link_user_translation: true)
  end

  def test_constructor_inside_some_of_a_cyclic_sum_reads_the_sum_on_ruby_and_mlcc
    source = <<~MLC
      import { Option } from "Option"
      type Tree = Leaf | Pair(Tree, Tree) | Node(Option<Tree>)
      fn label(tree: Tree) -> i32 = match tree
        | Leaf => 1
        | Pair(_, _) => 2
        | Node(_) => 3
      fn kind(tree: Tree) -> i32 = match tree
        | Node(Some(Pair(left, _))) => label(left)
        | Node(Some(Node(Some(child)))) => label(child)
        | Node(Some(Node(None))) => 6
        | Node(Some(Leaf)) => 7
        | Node(Some(_)) => 8
        | Node(None) => 0
        | Leaf => 4
        | Pair(_, _) => 5
      fn pair_leaf() -> Tree = Node(Some(Pair(Leaf, Leaf)))
      fn deep() -> Tree = Node(Some(Node(Some(Leaf))))
      fn inner_none() -> Tree = Node(Some(Node(None)))
      fn shallow() -> Tree = Node(Some(Leaf))
      fn missing() -> Tree = Node(None)
      fn main() -> i32 = 0
    MLC
    ruby_source = MLC.to_cpp(source)
    assert_includes ruby_source, "holds_alternative<Pair>"
    assert_includes ruby_source, "holds_alternative<Leaf>"
    assert_includes ruby_source, "(*(*"
    assert_includes ruby_source, "(*_v_pair.field0)"
    refute_includes ruby_source, "holds_alternative<Some>"

    checker = <<~CPP
      int main() {
        if (kind(pair_leaf()) != 1) return 1;
        if (kind(deep()) != 1) return 2;
        if (kind(inner_none()) != 6) return 3;
        if (kind(shallow()) != 7) return 4;
        if (kind(missing()) != 0) return 5;
        return 0;
      }
    CPP
    compile_both_compilers(source, "cyclic_sum_some_constructor", checker, link_user_translation: true)
  end

  def test_array_pattern_on_a_cyclic_sum_reads_the_sum_on_ruby_and_mlcc
    source = <<~MLC
      type Tree = Leaf | Kids([Tree])
      fn label(tree: Tree) -> i32 = match tree
        | Leaf => 1
        | Kids(_) => 2
      fn first_of(tree: Tree) -> i32 = match tree
        | Kids([first]) => label(first)
        | Kids([]) => 0
        | Kids(_) => 3
        | Leaf => 4
      fn present() -> Tree = Kids([Leaf])
      fn empty_kids() -> Tree = Kids([])
      fn two() -> Tree = Kids([Leaf, Leaf])
      fn main() -> i32 = 0
    MLC
    ruby_source = MLC.to_cpp(source)
    assert_includes ruby_source, ".size() == 1"
    assert_includes ruby_source, ".size() == 0"
    assert_includes ruby_source, "(*_v_kids.field0[0])"
    refute_includes ruby_source, "holds_alternative<Some>"

    checker = <<~CPP
      int main() {
        if (first_of(present()) != 1) return 1;
        if (first_of(empty_kids()) != 0) return 2;
        if (first_of(two()) != 3) return 3;
        if (first_of(Tree(Leaf{})) != 4) return 4;
        return 0;
      }
    CPP
    compile_both_compilers(source, "cyclic_sum_array_pattern", checker, link_user_translation: true)
  end

  def test_array_rest_pattern_on_a_cyclic_sum_reads_the_sum_on_ruby_and_mlcc
    source = <<~MLC
      type Tree = Leaf | Kids([Tree])
      fn label(tree: Tree) -> i32 = match tree
        | Leaf => 1
        | Kids(_) => 2
      fn head_tail(tree: Tree) -> i32 = match tree
        | Kids([first, ...rest]) => if rest.length() == 0 then label(first) else 2 end
        | Kids([]) => 0
        | Leaf => 4
      fn second(tree: Tree) -> i32 = match tree
        | Kids([_, ...rest]) => label(rest[0])
        | Kids([]) => 0
        | Leaf => 4
      fn one() -> Tree = Kids([Leaf])
      fn two() -> Tree = Kids([Leaf, Leaf])
      fn empty_kids() -> Tree = Kids([])
      fn main() -> i32 = 0
    MLC
    ruby_source = MLC.to_cpp(source)
    assert_includes ruby_source, ".size() >= 1"
    assert_includes ruby_source, ".size() == 0"
    assert_includes ruby_source, "(*_v_kids.field0[0])"
    assert_includes ruby_source, "mlc::Array<std::shared_ptr<Tree>>(_v_kids.field0.cbegin() + 1, _v_kids.field0.cend())"
    assert_includes ruby_source, "(*rest[0])"

    checker = <<~CPP
      int main() {
        if (head_tail(one()) != 1) return 1;
        if (head_tail(two()) != 2) return 2;
        if (head_tail(empty_kids()) != 0) return 3;
        if (head_tail(Tree(Leaf{})) != 4) return 4;
        if (second(two()) != 1) return 5;
        if (second(empty_kids()) != 0) return 6;
        if (second(Tree(Leaf{})) != 4) return 7;
        return 0;
      }
    CPP
    compile_both_compilers(source, "cyclic_sum_array_rest", checker, link_user_translation: true)
  end

  def test_nullary_constructor_inside_a_cyclic_array_reads_the_sum_on_ruby_and_mlcc
    source = <<~MLC
      type Tree = Leaf | Kids([Tree])
      fn classify(tree: Tree) -> i32 = match tree
        | Kids([Leaf]) => 1
        | Kids([Leaf, Leaf]) => 2
        | Kids([]) => 0
        | Kids(_) => 3
        | Leaf => 4
      fn one() -> Tree = Kids([Leaf])
      fn two() -> Tree = Kids([Leaf, Leaf])
      fn nested() -> Tree = Kids([Kids([Leaf])])
      fn empty_kids() -> Tree = Kids([])
      fn main() -> i32 = 0
    MLC
    ruby_source = MLC.to_cpp(source)
    assert_includes ruby_source, ".size() == 1"
    assert_includes ruby_source, ".size() == 2"
    assert_includes ruby_source, ".size() == 0"
    assert_includes ruby_source, "std::holds_alternative<Leaf>((*_v_kids.field0[0])._)"
    assert_includes ruby_source, "std::holds_alternative<Leaf>((*_v_kids.field0[1])._)"

    checker = <<~CPP
      int main() {
        if (classify(one()) != 1) return 1;
        if (classify(two()) != 2) return 2;
        if (classify(empty_kids()) != 0) return 3;
        if (classify(nested()) != 3) return 4;
        if (classify(Tree(Leaf{})) != 4) return 5;
        return 0;
      }
    CPP
    compile_both_compilers(source, "cyclic_sum_array_leaf", checker, link_user_translation: true)
  end

  def test_constructor_inside_a_cyclic_array_reads_the_sum_on_ruby_and_mlcc
    source = <<~MLC
      type Tree = Leaf | Node(Tree, Tree) | Kids([Tree])
      fn label(tree: Tree) -> i32 = match tree
        | Leaf => 1
        | Node(_, _) => 2
        | Kids(_) => 6
      fn left_of(tree: Tree) -> i32 = match tree
        | Kids([Node(left, _)]) => label(left)
        | Kids([Leaf]) => 0
        | Kids(_) => 3
        | Leaf => 4
        | Node(_, _) => 5
      fn pair() -> Tree = Kids([Node(Leaf, Leaf)])
      fn nested() -> Tree = Kids([Node(Kids([Leaf]), Leaf)])
      fn leaf_kid() -> Tree = Kids([Leaf])
      fn bare() -> Tree = Node(Leaf, Leaf)
      fn main() -> i32 = 0
    MLC
    ruby_source = MLC.to_cpp(source)
    assert_includes ruby_source, ".size() == 1"
    assert_includes ruby_source, "std::holds_alternative<Node>((*_v_kids.field0[0])._)"
    assert_includes ruby_source, "(*_v_node.field0)"
    assert_includes ruby_source, "std::holds_alternative<Leaf>((*_v_kids.field0[0])._)"

    checker = <<~CPP
      int main() {
        if (left_of(pair()) != 1) return 1;
        if (left_of(nested()) != 6) return 2;
        if (left_of(leaf_kid()) != 0) return 3;
        if (left_of(Tree(Leaf{})) != 4) return 4;
        if (left_of(bare()) != 5) return 5;
        return 0;
      }
    CPP
    compile_both_compilers(source, "cyclic_sum_array_node", checker, link_user_translation: true)
  end

  def test_nested_constructor_inside_a_cyclic_array_reads_the_sum_on_ruby_and_mlcc
    source = <<~MLC
      type Tree = Leaf | Node(Tree, Tree) | Kids([Tree])
      fn label(tree: Tree) -> i32 = match tree
        | Leaf => 1
        | Node(_, _) => 2
        | Kids(_) => 6
      fn deep_left(tree: Tree) -> i32 = match tree
        | Kids([Node(Node(left, _), _)]) => label(left)
        | Kids([Node(Leaf, _)]) => 0
        | Kids(_) => 3
        | Leaf => 4
        | Node(_, _) => 5
      fn deep() -> Tree = Kids([Node(Node(Leaf, Leaf), Leaf)])
      fn shallow() -> Tree = Kids([Node(Leaf, Leaf)])
      fn other() -> Tree = Kids([Node(Kids([Leaf]), Leaf)])
      fn bare() -> Tree = Node(Leaf, Leaf)
      fn main() -> i32 = 0
    MLC
    ruby_source = MLC.to_cpp(source)
    assert_includes ruby_source, "std::holds_alternative<Node>((*_v_kids.field0[0])._)"
    assert_includes ruby_source, "*_v_node.field0"
    assert_includes ruby_source, "std::holds_alternative<Node>(_nested_0._)"
    assert_includes ruby_source, "(*_v_node.field0)"

    checker = <<~CPP
      int main() {
        if (deep_left(deep()) != 1) return 1;
        if (deep_left(shallow()) != 0) return 2;
        if (deep_left(other()) != 3) return 3;
        if (deep_left(Tree(Leaf{})) != 4) return 4;
        if (deep_left(bare()) != 5) return 5;
        return 0;
      }
    CPP
    compile_both_compilers(source, "cyclic_sum_array_nested_node", checker, link_user_translation: true)
  end

  def test_deeper_constructor_inside_a_cyclic_array_reads_the_sum_on_ruby_and_mlcc
    source = <<~MLC
      type Tree = Leaf | Node(Tree, Tree) | Kids([Tree])
      fn label(tree: Tree) -> i32 = match tree
        | Leaf => 1
        | Node(_, _) => 2
        | Kids(_) => 6
      fn deeper(tree: Tree) -> i32 = match tree
        | Kids([Node(Node(Node(left, _), _), _)]) => label(left)
        | Kids([Node(Node(Leaf, _), _)]) => 0
        | Kids(_) => 3
        | Leaf => 4
        | Node(_, _) => 5
      fn triple() -> Tree = Kids([Node(Node(Node(Leaf, Leaf), Leaf), Leaf)])
      fn two_deep() -> Tree = Kids([Node(Node(Leaf, Leaf), Leaf)])
      fn other() -> Tree = Kids([Leaf])
      fn bare() -> Tree = Node(Leaf, Leaf)
      fn main() -> i32 = 0
    MLC
    ruby_source = MLC.to_cpp(source)
    assert_includes ruby_source, "std::holds_alternative<Node>((*_v_kids.field0[0])._)"
    assert_includes ruby_source, "*_v_node.field0"
    assert_includes ruby_source, "std::holds_alternative<Node>(_nested_0._)"
    assert_includes ruby_source, "(*_v_node.field0)"
    assert_includes ruby_source, "std::holds_alternative<Leaf>("

    checker = <<~CPP
      int main() {
        if (deeper(triple()) != 1) return 1;
        if (deeper(two_deep()) != 0) return 2;
        if (deeper(other()) != 3) return 3;
        if (deeper(Tree(Leaf{})) != 4) return 4;
        if (deeper(bare()) != 5) return 5;
        return 0;
      }
    CPP
    compile_both_compilers(source, "cyclic_sum_array_deeper_node", checker, link_user_translation: true)
  end

  def test_sibling_constructors_inside_a_cyclic_array_read_the_sum_on_ruby_and_mlcc
    source = <<~MLC
      type Tree = Leaf | Node(Tree, Tree) | Kids([Tree])
      fn label(tree: Tree) -> i32 = match tree
        | Leaf => 1
        | Node(_, _) => 2
        | Kids(_) => 6
      fn sides(tree: Tree) -> i32 = match tree
        | Kids([Node(Node(left, _), Node(right, _))]) => label(left) + label(right)
        | Kids(_) => 3
        | Leaf => 4
        | Node(_, _) => 5
      fn deep_sides(tree: Tree) -> i32 = match tree
        | Kids([Node(Node(Node(left, _), _), Node(right, _))]) => label(left) + label(right)
        | Kids(_) => 3
        | Leaf => 4
        | Node(_, _) => 5
      fn pair() -> Tree = Kids([Node(Node(Leaf, Leaf), Node(Kids([Leaf]), Leaf))])
      fn side() -> Tree = Kids([Node(Node(Leaf, Leaf), Leaf)])
      fn deep_pair() -> Tree = Kids([Node(Node(Node(Leaf, Leaf), Leaf), Node(Kids([Leaf]), Leaf))])
      fn deep_side() -> Tree = Kids([Node(Node(Node(Leaf, Leaf), Leaf), Leaf)])
      fn other() -> Tree = Kids([Leaf])
      fn bare() -> Tree = Node(Leaf, Leaf)
      fn main() -> i32 = 0
    MLC
    ruby_source = MLC.to_cpp(source)
    assert_includes ruby_source, "std::holds_alternative<Node>((*_v_kids.field0[0])._)"
    assert_includes ruby_source, "(*_v_node.field0)"
    assert_includes ruby_source, "std::holds_alternative<Node>(_nested_0._)"
    assert_includes ruby_source, "std::holds_alternative<Node>(_nested_1._)"

    checker = <<~CPP
      int main() {
        if (sides(pair()) != 7) return 1;
        if (sides(side()) != 3) return 2;
        if (sides(other()) != 3) return 3;
        if (sides(Tree(Leaf{})) != 4) return 4;
        if (sides(bare()) != 5) return 5;
        if (deep_sides(deep_pair()) != 7) return 6;
        if (deep_sides(deep_side()) != 3) return 7;
        return 0;
      }
    CPP
    compile_both_compilers(source, "cyclic_sum_array_sibling_node", checker, link_user_translation: true)
  end

  def test_two_constructors_inside_a_cyclic_array_read_the_sum_on_ruby_and_mlcc
    source = <<~MLC
      type Tree = Leaf | Node(Tree, Tree) | Kids([Tree])
      fn label(tree: Tree) -> i32 = match tree
        | Leaf => 1
        | Node(_, _) => 2
        | Kids(_) => 6
      fn both(tree: Tree) -> i32 = match tree
        | Kids([Node(left, _), Node(right, _)]) => label(left) + label(right)
        | Kids([Leaf, Node(child, _)]) => label(child)
        | Kids(_) => 3
        | Leaf => 4
        | Node(_, _) => 5
      fn pair() -> Tree = Kids([Node(Leaf, Leaf), Node(Kids([Leaf]), Leaf)])
      fn single() -> Tree = Kids([Node(Leaf, Leaf)])
      fn leaves() -> Tree = Kids([Leaf, Leaf])
      fn mixed() -> Tree = Kids([Leaf(), Node(Leaf(), Leaf())])
      fn bare() -> Tree = Node(Leaf, Leaf)
      fn main() -> i32 = 0
    MLC
    ruby_source = MLC.to_cpp(source)
    assert_includes ruby_source, "std::holds_alternative<Node>((*_v_kids.field0[0])._)"
    assert_includes ruby_source, "std::holds_alternative<Node>((*_v_kids.field0[1])._)"
    assert_includes ruby_source, "(*_v_node.field0)"
    assert_includes ruby_source, "std::holds_alternative<Leaf>((*_v_kids.field0[0])._)"

    checker = <<~CPP
      int main() {
        if (both(pair()) != 7) return 1;
        if (both(single()) != 3) return 2;
        if (both(leaves()) != 3) return 3;
        if (both(mixed()) != 1) return 4;
        if (both(Tree(Leaf{})) != 4) return 5;
        if (both(bare()) != 5) return 6;
        return 0;
      }
    CPP
    compile_both_compilers(source, "cyclic_sum_array_two_nodes", checker, link_user_translation: true)
  end

  def test_nested_and_sibling_constructors_inside_a_cyclic_array_read_the_sum_on_ruby_and_mlcc
    source = <<~MLC
      type Tree = Leaf | Node(Tree, Tree) | Kids([Tree])
      fn label(tree: Tree) -> i32 = match tree
        | Leaf => 1
        | Node(_, _) => 2
        | Kids(_) => 6
      fn sides(tree: Tree) -> i32 = match tree
        | Kids([Node(Node(left, _), _), Node(right, _)]) => label(left) + label(right)
        | Kids([Node(left, _), Node(Node(right, _), _)]) => label(left) + label(right)
        | Kids(_) => 3
        | Leaf => 4
        | Node(_, _) => 5
      fn deep_first() -> Tree = Kids([Node(Node(Leaf, Leaf), Leaf), Node(Kids([Leaf]), Leaf)])
      fn deep_second() -> Tree = Kids([Node(Leaf, Leaf), Node(Node(Kids([Leaf]), Leaf), Leaf)])
      fn other() -> Tree = Kids([Leaf, Leaf])
      fn single() -> Tree = Kids([Node(Node(Leaf, Leaf), Leaf)])
      fn bare() -> Tree = Node(Leaf, Leaf)
      fn main() -> i32 = 0
    MLC
    ruby_source = MLC.to_cpp(source)
    assert_includes ruby_source, "std::holds_alternative<Node>((*_v_kids.field0[0])._)"
    assert_includes ruby_source, "std::holds_alternative<Node>((*_v_kids.field0[1])._)"
    assert_includes ruby_source, "std::holds_alternative<Node>(_nested_0._)"
    assert_includes ruby_source, "(*_v_node.field0)"

    checker = <<~CPP
      int main() {
        if (sides(deep_first()) != 7) return 1;
        if (sides(deep_second()) != 7) return 2;
        if (sides(other()) != 3) return 3;
        if (sides(single()) != 3) return 4;
        if (sides(Tree(Leaf{})) != 4) return 5;
        if (sides(bare()) != 5) return 6;
        return 0;
      }
    CPP
    compile_both_compilers(source, "cyclic_sum_array_nested_sibling", checker, link_user_translation: true)
  end

  def test_constructor_and_rest_inside_a_cyclic_array_read_the_sum_on_ruby_and_mlcc
    source = <<~MLC
      type Tree = Leaf | Node(Tree, Tree) | Kids([Tree])
      fn label(tree: Tree) -> i32 = match tree
        | Leaf => 1
        | Node(_, _) => 2
        | Kids(_) => 6
      fn head(tree: Tree) -> i32 = match tree
        | Kids([Node(left, _), ...rest]) => if rest.length() == 0 then label(left) else label(left) + label(rest[0]) end
        | Kids(_) => 3
        | Leaf => 4
        | Node(_, _) => 5
      fn deep(tree: Tree) -> i32 = match tree
        | Kids([Node(Node(left, _), _), ...rest]) => if rest.length() == 0 then label(left) else label(rest[0]) end
        | Kids(_) => 3
        | Leaf => 4
        | Node(_, _) => 5
      fn pair() -> Tree = Kids([Node(Leaf, Leaf), Kids([Leaf])])
      fn only() -> Tree = Kids([Node(Kids([Leaf]), Leaf)])
      fn deep_pair() -> Tree = Kids([Node(Node(Leaf, Leaf), Leaf), Kids([Leaf])])
      fn deep_only() -> Tree = Kids([Node(Node(Leaf, Leaf), Leaf)])
      fn leaves() -> Tree = Kids([Leaf, Leaf])
      fn bare() -> Tree = Node(Leaf, Leaf)
      fn main() -> i32 = 0
    MLC
    ruby_source = MLC.to_cpp(source)
    assert_includes ruby_source, ".size() >= 1"
    assert_includes ruby_source, "std::holds_alternative<Node>((*_v_kids.field0[0])._)"
    assert_includes ruby_source, "(*_v_node.field0)"
    assert_includes ruby_source, "mlc::Array<std::shared_ptr<Tree>>(_v_kids.field0.cbegin() + 1, _v_kids.field0.cend())"
    assert_includes ruby_source, "(*rest[0])"
    assert_includes ruby_source, "std::holds_alternative<Node>(_nested_0._)"

    checker = <<~CPP
      int main() {
        if (head(pair()) != 7) return 1;
        if (head(only()) != 6) return 2;
        if (head(leaves()) != 3) return 3;
        if (head(Tree(Leaf{})) != 4) return 4;
        if (head(bare()) != 5) return 5;
        if (deep(deep_pair()) != 6) return 6;
        if (deep(deep_only()) != 1) return 7;
        return 0;
      }
    CPP
    compile_both_compilers(source, "cyclic_sum_array_constructor_rest", checker, link_user_translation: true)
  end

  def test_two_nested_constructors_inside_a_cyclic_array_read_the_sum_on_ruby_and_mlcc
    source = <<~MLC
      type Tree = Leaf | Node(Tree, Tree) | Kids([Tree])
      fn label(tree: Tree) -> i32 = match tree
        | Leaf => 1
        | Node(_, _) => 2
        | Kids(_) => 6
      fn both(tree: Tree) -> i32 = match tree
        | Kids([Node(Node(left, _), _), Node(Node(right, _), _)]) => label(left) + label(right)
        | Kids(_) => 3
        | Leaf => 4
        | Node(_, _) => 5
      fn pair() -> Tree = Kids([Node(Node(Leaf, Leaf), Leaf), Node(Node(Kids([Leaf]), Leaf), Leaf)])
      fn one() -> Tree = Kids([Node(Node(Leaf, Leaf), Leaf)])
      fn mixed() -> Tree = Kids([Node(Node(Leaf, Leaf), Leaf), Node(Kids([Leaf]), Leaf)])
      fn leaves() -> Tree = Kids([Leaf, Leaf])
      fn bare() -> Tree = Node(Leaf, Leaf)
      fn main() -> i32 = 0
    MLC
    ruby_source = MLC.to_cpp(source)
    assert_includes ruby_source, ".size() == 2"
    assert_includes ruby_source, "std::holds_alternative<Node>((*_v_kids.field0[0])._)"
    assert_includes ruby_source, "std::holds_alternative<Node>((*_v_kids.field0[1])._)"
    assert_includes ruby_source, "std::holds_alternative<Node>(_nested_0._)"
    assert_includes ruby_source, "(*_v_node.field0)"

    checker = <<~CPP
      int main() {
        if (both(pair()) != 7) return 1;
        if (both(one()) != 3) return 2;
        if (both(mixed()) != 3) return 3;
        if (both(leaves()) != 3) return 4;
        if (both(Tree(Leaf{})) != 4) return 5;
        if (both(bare()) != 5) return 6;
        return 0;
      }
    CPP
    compile_both_compilers(source, "cyclic_sum_array_two_nested", checker, link_user_translation: true)
  end

  def test_nested_sibling_pair_and_array_constructor_inside_a_cyclic_array_read_the_sum_on_ruby_and_mlcc
    source = <<~MLC
      type Tree = Leaf | Node(Tree, Tree) | Kids([Tree])
      fn label(tree: Tree) -> i32 = match tree
        | Leaf => 1
        | Node(_, _) => 2
        | Kids(_) => 6
      fn sides(tree: Tree) -> i32 = match tree
        | Kids([Node(Node(left, _), Node(mid, _)), Node(right, _)]) => label(left) + label(mid) + label(right)
        | Kids(_) => 3
        | Leaf => 4
        | Node(_, _) => 5
      fn trio() -> Tree = Kids([Node(Node(Leaf, Leaf), Node(Kids([Leaf]), Leaf)), Node(Leaf, Leaf)])
      fn one() -> Tree = Kids([Node(Node(Leaf, Leaf), Node(Kids([Leaf]), Leaf))])
      fn shallow() -> Tree = Kids([Node(Node(Leaf, Leaf), Leaf), Node(Leaf, Leaf)])
      fn leaves() -> Tree = Kids([Leaf, Leaf])
      fn bare() -> Tree = Node(Leaf, Leaf)
      fn main() -> i32 = 0
    MLC
    ruby_source = MLC.to_cpp(source)
    assert_includes ruby_source, ".size() == 2"
    assert_includes ruby_source, "std::holds_alternative<Node>((*_v_kids.field0[0])._)"
    assert_includes ruby_source, "std::holds_alternative<Node>((*_v_kids.field0[1])._)"
    assert_includes ruby_source, "std::holds_alternative<Node>(_nested_0._)"
    assert_includes ruby_source, "std::holds_alternative<Node>(_nested_1._)"
    assert_includes ruby_source, "(*_v_node.field0)"

    checker = <<~CPP
      int main() {
        if (sides(trio()) != 8) return 1;
        if (sides(one()) != 3) return 2;
        if (sides(shallow()) != 3) return 3;
        if (sides(leaves()) != 3) return 4;
        if (sides(Tree(Leaf{})) != 4) return 5;
        if (sides(bare()) != 5) return 6;
        return 0;
      }
    CPP
    compile_both_compilers(source, "cyclic_sum_array_sibling_pair", checker, link_user_translation: true)
  end

  def test_array_inside_a_nested_constructor_of_a_cyclic_sum_reads_the_sum_on_ruby_and_mlcc
    source = <<~MLC
      type Tree = Leaf | Node(Tree, Tree) | Kids([Tree])
      fn label(tree: Tree) -> i32 = match tree
        | Leaf => 1
        | Node(_, _) => 2
        | Kids(_) => 6
      fn inner(tree: Tree) -> i32 = match tree
        | Node(Kids([child]), _) => label(child)
        | Kids(_) => 3
        | Leaf => 4
        | Node(_, _) => 5
      fn pair(tree: Tree) -> i32 = match tree
        | Node(Kids([child]), Node(right, _)) => label(child) + label(right)
        | Kids(_) => 3
        | Leaf => 4
        | Node(_, _) => 5
      fn head(tree: Tree) -> i32 = match tree
        | Node(Kids([child, ...rest]), _) => if rest.length() == 0 then label(child) else label(child) + label(rest[0]) end
        | Kids(_) => 3
        | Leaf => 4
        | Node(_, _) => 5
      fn present() -> Tree = Node(Kids([Leaf]), Leaf)
      fn empty_inner() -> Tree = Node(Kids([]), Leaf)
      fn with_right() -> Tree = Node(Kids([Kids([Leaf])]), Node(Leaf, Leaf))
      fn two() -> Tree = Node(Kids([Leaf(), Kids([Leaf])]), Leaf)
      fn bare_kids() -> Tree = Kids([Leaf])
      fn main() -> i32 = 0
    MLC
    ruby_source = MLC.to_cpp(source)
    assert_includes ruby_source, "std::holds_alternative<Kids>"
    assert_includes ruby_source, ".size() == 1"
    assert_includes ruby_source, ".size() >= 1"
    assert_includes ruby_source, "(*_v_kids.field0[0])"
    assert_includes ruby_source, "mlc::Array<std::shared_ptr<Tree>>"
    assert_includes ruby_source, "(*rest[0])"
    assert_includes ruby_source, "std::holds_alternative<Node>(_nested_1._)"

    checker = <<~CPP
      int main() {
        if (inner(present()) != 1) return 1;
        if (inner(empty_inner()) != 5) return 2;
        if (inner(bare_kids()) != 3) return 3;
        if (inner(Tree(Leaf{})) != 4) return 4;
        if (pair(with_right()) != 7) return 5;
        if (pair(present()) != 5) return 6;
        if (head(two()) != 7) return 7;
        if (head(present()) != 1) return 8;
        if (head(empty_inner()) != 5) return 9;
        return 0;
      }
    CPP
    compile_both_compilers(source, "cyclic_sum_nested_array", checker, link_user_translation: true)
  end

  def test_array_inside_an_array_element_constructor_of_a_cyclic_sum_reads_the_sum_on_ruby_and_mlcc
    source = <<~MLC
      type Tree = Leaf | Node(Tree, Tree) | Kids([Tree])
      fn label(tree: Tree) -> i32 = match tree
        | Leaf => 1
        | Node(_, _) => 2
        | Kids(_) => 6
      fn inner(tree: Tree) -> i32 = match tree
        | Kids([Node(Kids([child]), _)]) => label(child)
        | Kids(_) => 3
        | Leaf => 4
        | Node(_, _) => 5
      fn head(tree: Tree) -> i32 = match tree
        | Kids([Node(Kids([child, ...rest]), _)]) => if rest.length() == 0 then label(child) else label(child) + label(rest[0]) end
        | Kids(_) => 3
        | Leaf => 4
        | Node(_, _) => 5
      fn present() -> Tree = Kids([Node(Kids([Leaf]), Leaf)])
      fn empty_inner() -> Tree = Kids([Node(Kids([]), Leaf)])
      fn two() -> Tree = Kids([Node(Kids([Leaf(), Kids([Leaf])]), Leaf)])
      fn leaf_kid() -> Tree = Kids([Leaf])
      fn bare() -> Tree = Node(Kids([Leaf]), Leaf)
      fn main() -> i32 = 0
    MLC
    ruby_source = MLC.to_cpp(source)
    assert_includes ruby_source, ".size() == 1"
    assert_includes ruby_source, ".size() >= 1"
    assert_includes ruby_source, "std::holds_alternative<Node>((*_v_kids.field0[0])._)"
    assert_includes ruby_source, "std::holds_alternative<Kids>"
    assert_includes ruby_source, "(*_v_kids.field0[0])"
    assert_includes ruby_source, "mlc::Array<std::shared_ptr<Tree>>"
    assert_includes ruby_source, "(*rest[0])"

    checker = <<~CPP
      int main() {
        if (inner(present()) != 1) return 1;
        if (inner(empty_inner()) != 3) return 2;
        if (inner(leaf_kid()) != 3) return 3;
        if (inner(Tree(Leaf{})) != 4) return 4;
        if (inner(bare()) != 5) return 5;
        if (head(two()) != 7) return 6;
        if (head(present()) != 1) return 7;
        if (head(empty_inner()) != 3) return 8;
        return 0;
      }
    CPP
    compile_both_compilers(source, "cyclic_sum_element_array", checker, link_user_translation: true)
  end

  def test_constructor_inside_an_array_element_array_of_a_cyclic_sum_reads_the_sum_on_ruby_and_mlcc
    source = <<~MLC
      type Tree = Leaf | Node(Tree, Tree) | Kids([Tree])
      fn label(tree: Tree) -> i32 = match tree
        | Leaf => 1
        | Node(_, _) => 2
        | Kids(_) => 6
      fn deep(tree: Tree) -> i32 = match tree
        | Kids([Node(Kids([Node(left, _)]), _)]) => label(left)
        | Kids(_) => 3
        | Leaf => 4
        | Node(_, _) => 5
      fn sides(tree: Tree) -> i32 = match tree
        | Kids([Node(Kids([Node(left, _), Node(right, _)]), _)]) => label(left) + label(right)
        | Kids(_) => 3
        | Leaf => 4
        | Node(_, _) => 5
      fn present() -> Tree = Kids([Node(Kids([Node(Leaf, Leaf)]), Leaf)])
      fn pair() -> Tree = Kids([Node(Kids([Node(Leaf, Leaf), Node(Kids([Leaf]), Leaf)]), Leaf)])
      fn shallow() -> Tree = Kids([Node(Kids([Leaf]), Leaf)])
      fn bare() -> Tree = Node(Leaf, Leaf)
      fn main() -> i32 = 0
    MLC
    ruby_source = MLC.to_cpp(source)
    assert_includes ruby_source, "std::holds_alternative<Node>((*_v_kids.field0[0])._)"
    assert_includes ruby_source, "(*_v_node.field0)"
    assert_includes ruby_source, ".size() == 1"
    assert_includes ruby_source, ".size() == 2"

    checker = <<~CPP
      int main() {
        if (deep(present()) != 1) return 1;
        if (deep(shallow()) != 3) return 2;
        if (deep(Tree(Leaf{})) != 4) return 3;
        if (deep(bare()) != 5) return 4;
        if (sides(pair()) != 7) return 5;
        if (sides(present()) != 3) return 6;
        if (sides(shallow()) != 3) return 7;
        return 0;
      }
    CPP
    compile_both_compilers(source, "cyclic_sum_element_constructor", checker, link_user_translation: true)
  end

  def test_nested_constructor_inside_an_array_element_array_of_a_cyclic_sum_reads_the_sum_on_ruby_and_mlcc
    source = <<~MLC
      type Tree = Leaf | Node(Tree, Tree) | Kids([Tree])
      fn label(tree: Tree) -> i32 = match tree
        | Leaf => 1
        | Node(_, _) => 2
        | Kids(_) => 6
      fn deep(tree: Tree) -> i32 = match tree
        | Kids([Node(Kids([Node(Node(left, _), _)]), _)]) => label(left)
        | Kids(_) => 3
        | Leaf => 4
        | Node(_, _) => 5
      fn present() -> Tree = Kids([Node(Kids([Node(Node(Leaf, Leaf), Leaf)]), Leaf)])
      fn shallow() -> Tree = Kids([Node(Kids([Node(Leaf, Leaf)]), Leaf)])
      fn bare() -> Tree = Node(Leaf, Leaf)
      fn main() -> i32 = 0
    MLC
    ruby_source = MLC.to_cpp(source)
    assert_includes ruby_source, "std::holds_alternative<Node>(_nested_0._)"
    assert_includes ruby_source, "(*_v_node.field0)"
    assert_includes ruby_source, ".size() == 1"

    checker = <<~CPP
      int main() {
        if (deep(present()) != 1) return 1;
        if (deep(shallow()) != 3) return 2;
        if (deep(Tree(Leaf{})) != 4) return 3;
        if (deep(bare()) != 5) return 4;
        return 0;
      }
    CPP
    compile_both_compilers(source, "cyclic_sum_element_nested", checker, link_user_translation: true)
  end

  def test_sibling_constructors_inside_an_array_element_array_of_a_cyclic_sum_read_the_sum_on_ruby_and_mlcc
    source = <<~MLC
      type Tree = Leaf | Node(Tree, Tree) | Kids([Tree])
      fn label(tree: Tree) -> i32 = match tree
        | Leaf => 1
        | Node(_, _) => 2
        | Kids(_) => 6
      fn sides(tree: Tree) -> i32 = match tree
        | Kids([Node(Kids([Node(Node(left, _), Node(right, _))]), _)]) => label(left) + label(right)
        | Kids(_) => 3
        | Leaf => 4
        | Node(_, _) => 5
      fn pair() -> Tree = Kids([Node(Kids([Node(Node(Leaf, Leaf), Node(Kids([Leaf]), Leaf))]), Leaf)])
      fn one_side() -> Tree = Kids([Node(Kids([Node(Node(Leaf, Leaf), Leaf)]), Leaf)])
      fn shallow() -> Tree = Kids([Node(Kids([Node(Leaf, Leaf)]), Leaf)])
      fn bare() -> Tree = Node(Leaf, Leaf)
      fn main() -> i32 = 0
    MLC
    ruby_source = MLC.to_cpp(source)
    assert_includes ruby_source, "std::holds_alternative<Node>(_nested_0._)"
    assert_includes ruby_source, "std::holds_alternative<Node>(_nested_1._)"
    assert_includes ruby_source, "(*_v_node.field0)"
    assert_includes ruby_source, ".size() == 1"

    checker = <<~CPP
      int main() {
        if (sides(pair()) != 7) return 1;
        if (sides(one_side()) != 3) return 2;
        if (sides(shallow()) != 3) return 3;
        if (sides(Tree(Leaf{})) != 4) return 4;
        if (sides(bare()) != 5) return 5;
        return 0;
      }
    CPP
    compile_both_compilers(source, "cyclic_sum_element_siblings", checker, link_user_translation: true)
  end

  def test_two_nested_constructors_inside_an_array_element_array_of_a_cyclic_sum_read_the_sum_on_ruby_and_mlcc
    source = <<~MLC
      type Tree = Leaf | Node(Tree, Tree) | Kids([Tree])
      fn label(tree: Tree) -> i32 = match tree
        | Leaf => 1
        | Node(_, _) => 2
        | Kids(_) => 6
      fn both(tree: Tree) -> i32 = match tree
        | Kids([Node(Kids([Node(Node(left, _), _), Node(Node(right, _), _)]), _)]) => label(left) + label(right)
        | Kids(_) => 3
        | Leaf => 4
        | Node(_, _) => 5
      fn pair() -> Tree = Kids([Node(Kids([Node(Node(Leaf, Leaf), Leaf), Node(Node(Kids([Leaf]), Leaf), Leaf)]), Leaf)])
      fn one() -> Tree = Kids([Node(Kids([Node(Node(Leaf, Leaf), Leaf)]), Leaf)])
      fn bare() -> Tree = Node(Leaf, Leaf)
      fn main() -> i32 = 0
    MLC
    ruby_source = MLC.to_cpp(source)
    assert_includes ruby_source, ".size() == 2"
    assert_includes ruby_source, "std::holds_alternative<Node>(_nested_0._)"
    assert_includes ruby_source, "(*_v_node.field0)"

    checker = <<~CPP
      int main() {
        if (both(pair()) != 7) return 1;
        if (both(one()) != 3) return 2;
        if (both(Tree(Leaf{})) != 4) return 3;
        if (both(bare()) != 5) return 4;
        return 0;
      }
    CPP
    compile_both_compilers(source, "cyclic_sum_element_two_nested", checker, link_user_translation: true)
  end

  def test_deeper_sibling_constructors_inside_an_array_element_array_of_a_cyclic_sum_read_the_sum_on_ruby_and_mlcc
    source = <<~MLC
      type Tree = Leaf | Node(Tree, Tree) | Kids([Tree])
      fn label(tree: Tree) -> i32 = match tree
        | Leaf => 1
        | Node(_, _) => 2
        | Kids(_) => 6
      fn sides(tree: Tree) -> i32 = match tree
        | Kids([Node(Kids([Node(Node(Node(left, _), _), Node(right, _))]), _)]) => label(left) + label(right)
        | Kids(_) => 3
        | Leaf => 4
        | Node(_, _) => 5
      fn pair() -> Tree = Kids([Node(Kids([Node(Node(Node(Leaf, Leaf), Leaf), Node(Kids([Leaf]), Leaf))]), Leaf)])
      fn one_side() -> Tree = Kids([Node(Kids([Node(Node(Node(Leaf, Leaf), Leaf), Leaf)]), Leaf)])
      fn shallow() -> Tree = Kids([Node(Kids([Node(Node(Leaf, Leaf), Node(Kids([Leaf]), Leaf))]), Leaf)])
      fn bare() -> Tree = Node(Leaf, Leaf)
      fn main() -> i32 = 0
    MLC
    ruby_source = MLC.to_cpp(source)
    assert_includes ruby_source, "std::holds_alternative<Node>(_nested_0._)"
    assert_includes ruby_source, "std::holds_alternative<Node>(_nested_1._)"
    assert_includes ruby_source, "(*_v_node.field0)"
    assert_includes ruby_source, ".size() == 1"

    checker = <<~CPP
      int main() {
        if (sides(pair()) != 7) return 1;
        if (sides(one_side()) != 3) return 2;
        if (sides(shallow()) != 3) return 3;
        if (sides(Tree(Leaf{})) != 4) return 4;
        if (sides(bare()) != 5) return 5;
        return 0;
      }
    CPP
    compile_both_compilers(source, "cyclic_sum_element_deep_sibling", checker, link_user_translation: true)
  end

  def test_nested_constructor_and_rest_inside_an_array_element_array_of_a_cyclic_sum_read_the_sum_on_ruby_and_mlcc
    source = <<~MLC
      type Tree = Leaf | Node(Tree, Tree) | Kids([Tree])
      fn label(tree: Tree) -> i32 = match tree
        | Leaf => 1
        | Node(_, _) => 2
        | Kids(_) => 6
      fn head(tree: Tree) -> i32 = match tree
        | Kids([Node(Kids([Node(Node(left, _), _), ...rest]), _)]) => if rest.length() == 0 then label(left) else label(left) + 10 end
        | Kids(_) => 3
        | Leaf => 4
        | Node(_, _) => 5
      fn pair() -> Tree = Kids([Node(Kids([Node(Node(Leaf, Leaf), Leaf), Node(Kids([Leaf]), Leaf)]), Leaf)])
      fn only() -> Tree = Kids([Node(Kids([Node(Node(Leaf, Leaf), Leaf)]), Leaf)])
      fn shallow() -> Tree = Kids([Node(Kids([Node(Leaf, Leaf)]), Leaf)])
      fn bare() -> Tree = Node(Leaf, Leaf)
      fn main() -> i32 = 0
    MLC
    ruby_source = MLC.to_cpp(source)
    assert_includes ruby_source, ".size() >= 1"
    assert_includes ruby_source, "(*_v_node.field0)"
    assert_includes ruby_source, "cbegin() + 1"

    checker = <<~CPP
      int main() {
        if (head(pair()) != 11) return 1;
        if (head(only()) != 1) return 2;
        if (head(shallow()) != 3) return 3;
        if (head(Tree(Leaf{})) != 4) return 4;
        if (head(bare()) != 5) return 5;
        return 0;
      }
    CPP
    compile_both_compilers(source, "cyclic_sum_element_nested_rest", checker, link_user_translation: true)
  end

  def test_two_array_constructors_inside_an_array_element_array_of_a_cyclic_sum_read_the_sum_on_ruby_and_mlcc
    source = <<~MLC
      type Tree = Leaf | Node(Tree, Tree) | Kids([Tree])
      fn label(tree: Tree) -> i32 = match tree
        | Leaf => 1
        | Node(_, _) => 2
        | Kids(_) => 6
      fn both(tree: Tree) -> i32 = match tree
        | Kids([Node(Kids([Node(Kids([left]), _), Node(Kids([right]), _)]), _)]) => label(left) + label(right)
        | Kids(_) => 3
        | Leaf => 4
        | Node(_, _) => 5
      fn pair() -> Tree = Kids([Node(Kids([Node(Kids([Leaf]), Leaf), Node(Kids([Kids([Leaf])]), Leaf)]), Leaf)])
      fn one() -> Tree = Kids([Node(Kids([Node(Kids([Leaf]), Leaf)]), Leaf)])
      fn shallow() -> Tree = Kids([Node(Kids([Node(Leaf, Leaf)]), Leaf)])
      fn bare() -> Tree = Node(Leaf, Leaf)
      fn main() -> i32 = 0
    MLC
    ruby_source = MLC.to_cpp(source)
    assert_includes ruby_source, ".size() == 2"
    assert_includes ruby_source, ".size() == 1"
    assert_includes ruby_source, "const auto& left = (*"
    assert_includes ruby_source, "const auto& right = (*"

    checker = <<~CPP
      int main() {
        if (both(pair()) != 7) return 1;
        if (both(one()) != 3) return 2;
        if (both(shallow()) != 3) return 3;
        if (both(Tree(Leaf{})) != 4) return 4;
        if (both(bare()) != 5) return 5;
        return 0;
      }
    CPP
    compile_both_compilers(source, "cyclic_sum_element_two_arrays", checker, link_user_translation: true)
  end

  def test_array_constructor_and_rest_inside_an_array_element_array_of_a_cyclic_sum_read_the_sum_on_ruby_and_mlcc
    source = <<~MLC
      type Tree = Leaf | Node(Tree, Tree) | Kids([Tree])
      fn label(tree: Tree) -> i32 = match tree
        | Leaf => 1
        | Node(_, _) => 2
        | Kids(_) => 6
      fn head(tree: Tree) -> i32 = match tree
        | Kids([Node(Kids([Node(Kids([left]), _), ...rest]), _)]) => if rest.length() == 0 then label(left) else label(left) + 10 end
        | Kids(_) => 3
        | Leaf => 4
        | Node(_, _) => 5
      fn pair() -> Tree = Kids([Node(Kids([Node(Kids([Leaf]), Leaf), Node(Kids([Leaf]), Leaf)]), Leaf)])
      fn only() -> Tree = Kids([Node(Kids([Node(Kids([Leaf]), Leaf)]), Leaf)])
      fn shallow() -> Tree = Kids([Node(Kids([Node(Leaf, Leaf)]), Leaf)])
      fn bare() -> Tree = Node(Leaf, Leaf)
      fn main() -> i32 = 0
    MLC
    ruby_source = MLC.to_cpp(source)
    assert_includes ruby_source, ".size() >= 1"
    assert_includes ruby_source, ".size() == 1"
    assert_includes ruby_source, "const auto& left = (*"
    assert_includes ruby_source, "cbegin() + 1"

    checker = <<~CPP
      int main() {
        if (head(pair()) != 11) return 1;
        if (head(only()) != 1) return 2;
        if (head(shallow()) != 3) return 3;
        if (head(Tree(Leaf{})) != 4) return 4;
        if (head(bare()) != 5) return 5;
        return 0;
      }
    CPP
    compile_both_compilers(source, "cyclic_sum_element_array_rest", checker, link_user_translation: true)
  end

  def test_ident_and_nested_constructor_inside_an_array_element_array_of_a_cyclic_sum_read_the_sum_on_ruby_and_mlcc
    source = <<~MLC
      type Tree = Leaf | Node(Tree, Tree) | Kids([Tree])
      fn label(tree: Tree) -> i32 = match tree
        | Leaf => 1
        | Node(_, _) => 2
        | Kids(_) => 6
      fn both(tree: Tree) -> i32 = match tree
        | Kids([Node(Kids([left, Node(Node(child, _), _)]), _)]) => label(left) + label(child)
        | Kids(_) => 3
        | Leaf => 4
        | Node(_, _) => 5
      fn pair() -> Tree = Kids([Node(Kids([Leaf(), Node(Node(Kids([Leaf]), Leaf), Leaf)]), Leaf)])
      fn one() -> Tree = Kids([Node(Kids([Leaf()]), Leaf)])
      fn shallow() -> Tree = Kids([Node(Kids([Leaf(), Node(Leaf, Leaf)]), Leaf)])
      fn bare() -> Tree = Node(Leaf, Leaf)
      fn main() -> i32 = 0
    MLC
    ruby_source = MLC.to_cpp(source)
    assert_includes ruby_source, ".size() == 2"
    assert_includes ruby_source, "const auto& left = (*"
    assert_includes ruby_source, "std::holds_alternative<Node>(_nested_0._)"
    assert_includes ruby_source, "(*_v_node.field0)"

    checker = <<~CPP
      int main() {
        if (both(pair()) != 7) return 1;
        if (both(one()) != 3) return 2;
        if (both(shallow()) != 3) return 3;
        if (both(Tree(Leaf{})) != 4) return 4;
        if (both(bare()) != 5) return 5;
        return 0;
      }
    CPP
    compile_both_compilers(source, "cyclic_sum_element_ident_nested", checker, link_user_translation: true)
  end

  def test_two_array_fields_of_an_array_element_of_a_cyclic_sum_read_the_sum_on_ruby_and_mlcc
    source = <<~MLC
      type Tree = Leaf | Node(Tree, Tree) | Kids([Tree])
      fn label(tree: Tree) -> i32 = match tree
        | Leaf => 1
        | Node(_, _) => 2
        | Kids(_) => 6
      fn both(tree: Tree) -> i32 = match tree
        | Kids([Node(Kids([left]), Kids([right]))]) => label(left) + label(right)
        | Kids(_) => 3
        | Leaf => 4
        | Node(_, _) => 5
      fn pair() -> Tree = Kids([Node(Kids([Leaf]), Kids([Kids([Leaf])]))])
      fn one_side() -> Tree = Kids([Node(Kids([Leaf]), Leaf)])
      fn empty_left() -> Tree = Kids([Node(Kids([]), Kids([Leaf]))])
      fn bare() -> Tree = Node(Leaf, Leaf)
      fn main() -> i32 = 0
    MLC
    ruby_source = MLC.to_cpp(source)
    assert_includes ruby_source, ".size() == 1"
    assert_includes ruby_source, "const auto& left = (*"
    assert_includes ruby_source, "const auto& right = (*"

    checker = <<~CPP
      int main() {
        if (both(pair()) != 7) return 1;
        if (both(one_side()) != 3) return 2;
        if (both(empty_left()) != 3) return 3;
        if (both(Tree(Leaf{})) != 4) return 4;
        if (both(bare()) != 5) return 5;
        return 0;
      }
    CPP
    compile_both_compilers(source, "cyclic_sum_element_two_fields", checker, link_user_translation: true)
  end

  def test_two_array_fields_and_rest_of_an_array_element_of_a_cyclic_sum_read_the_sum_on_ruby_and_mlcc
    source = <<~MLC
      type Tree = Leaf | Node(Tree, Tree) | Kids([Tree])
      fn label(tree: Tree) -> i32 = match tree
        | Leaf => 1
        | Node(_, _) => 2
        | Kids(_) => 6
      fn both(tree: Tree) -> i32 = match tree
        | Kids([Node(Kids([left]), Kids([Node(child, _)])), ...rest]) => if rest.length() == 0 then label(left) + label(child) else label(left) + label(child) + 10 end
        | Kids(_) => 3
        | Leaf => 4
        | Node(_, _) => 5
      fn pair() -> Tree = Kids([Node(Kids([Leaf]), Kids([Node(Kids([Leaf]), Leaf)])), Leaf()])
      fn only() -> Tree = Kids([Node(Kids([Leaf]), Kids([Node(Kids([Leaf]), Leaf)]))])
      fn shallow() -> Tree = Kids([Node(Kids([Leaf]), Kids([Leaf]))])
      fn bare() -> Tree = Node(Leaf, Leaf)
      fn main() -> i32 = 0
    MLC
    ruby_source = MLC.to_cpp(source)
    assert_includes ruby_source, ".size() >= 1"
    assert_includes ruby_source, "const auto& left = (*"
    assert_includes ruby_source, "(*_v_node.field0)"
    assert_includes ruby_source, "cbegin() + 1"

    checker = <<~CPP
      int main() {
        if (both(pair()) != 17) return 1;
        if (both(only()) != 7) return 2;
        if (both(shallow()) != 3) return 3;
        if (both(Tree(Leaf{})) != 4) return 4;
        if (both(bare()) != 5) return 5;
        return 0;
      }
    CPP
    compile_both_compilers(source, "cyclic_sum_element_fields_rest", checker, link_user_translation: true)
  end

  def test_nullary_constructor_beside_a_binding_inside_an_array_element_array_of_a_cyclic_sum_reads_the_sum_on_ruby_and_mlcc
    source = <<~MLC
      type Tree = Leaf | Node(Tree, Tree) | Kids([Tree])
      fn label(tree: Tree) -> i32 = match tree
        | Leaf => 1
        | Node(_, _) => 2
        | Kids(_) => 6
      fn side(tree: Tree) -> i32 = match tree
        | Kids([Node(Kids([Node(Leaf, child)]), _)]) => label(child)
        | Kids(_) => 3
        | Leaf => 4
        | Node(_, _) => 5
      fn present() -> Tree = Kids([Node(Kids([Node(Leaf, Kids([Leaf]))]), Leaf)])
      fn other() -> Tree = Kids([Node(Kids([Node(Kids([Leaf]), Leaf)]), Leaf)])
      fn shallow() -> Tree = Kids([Node(Kids([Leaf]), Leaf)])
      fn bare() -> Tree = Node(Leaf, Leaf)
      fn main() -> i32 = 0
    MLC
    ruby_source = MLC.to_cpp(source)
    assert_includes ruby_source, "std::holds_alternative<Leaf>"
    assert_includes ruby_source, "const auto& child = (*"
    assert_includes ruby_source, ".size() == 1"

    checker = <<~CPP
      int main() {
        if (side(present()) != 6) return 1;
        if (side(other()) != 3) return 2;
        if (side(shallow()) != 3) return 3;
        if (side(Tree(Leaf{})) != 4) return 4;
        if (side(bare()) != 5) return 5;
        return 0;
      }
    CPP
    compile_both_compilers(source, "cyclic_sum_element_nullary_side", checker, link_user_translation: true)
  end

  def test_rest_element_inside_an_array_element_array_of_a_cyclic_sum_reads_the_sum_on_ruby_and_mlcc
    source = <<~MLC
      type Tree = Leaf | Node(Tree, Tree) | Kids([Tree])
      fn label(tree: Tree) -> i32 = match tree
        | Leaf => 1
        | Node(_, _) => 2
        | Kids(_) => 6
      fn head(tree: Tree) -> i32 = match tree
        | Kids([Node(Kids([Node(Kids([left]), _), ...rest]), _)]) => if rest.length() == 0 then label(left) else label(rest[0]) end
        | Kids(_) => 3
        | Leaf => 4
        | Node(_, _) => 5
      fn pair() -> Tree = Kids([Node(Kids([Node(Kids([Leaf]), Leaf), Node(Kids([Leaf]), Leaf)]), Leaf)])
      fn only() -> Tree = Kids([Node(Kids([Node(Kids([Leaf]), Leaf)]), Leaf)])
      fn shallow() -> Tree = Kids([Node(Kids([Node(Leaf, Leaf)]), Leaf)])
      fn bare() -> Tree = Node(Leaf, Leaf)
      fn main() -> i32 = 0
    MLC
    ruby_source = MLC.to_cpp(source)
    assert_includes ruby_source, "(*rest[0])"
    assert_includes ruby_source, "const auto& left = (*"
    assert_includes ruby_source, "cbegin() + 1"

    checker = <<~CPP
      int main() {
        if (head(pair()) != 2) return 1;
        if (head(only()) != 1) return 2;
        if (head(shallow()) != 3) return 3;
        if (head(Tree(Leaf{})) != 4) return 4;
        if (head(bare()) != 5) return 5;
        return 0;
      }
    CPP
    compile_both_compilers(source, "cyclic_sum_element_rest_index", checker, link_user_translation: true)
  end

  def test_guard_on_an_array_element_array_of_a_cyclic_sum_sees_the_sum_on_ruby_and_mlcc
    source = <<~MLC
      type Tree = Leaf | Node(Tree, Tree) | Kids([Tree])
      fn label(tree: Tree) -> i32 = match tree
        | Leaf => 1
        | Node(_, _) => 2
        | Kids(_) => 6
      fn head(tree: Tree) -> i32 = match tree
        | Kids([Node(Kids([child]), _)]) if label(child) > 1 => label(child)
        | Kids(_) => 3
        | Leaf => 4
        | Node(_, _) => 5
      fn big() -> Tree = Kids([Node(Kids([Kids([Leaf])]), Leaf)])
      fn small() -> Tree = Kids([Node(Kids([Leaf]), Leaf)])
      fn leaf_kid() -> Tree = Kids([Leaf])
      fn bare() -> Tree = Node(Leaf, Leaf)
      fn main() -> i32 = 0
    MLC
    ruby_source = MLC.to_cpp(source)
    assert_includes ruby_source, "const auto& child = (*"
    assert_includes ruby_source, "label(child) > 1"
    assert_includes ruby_source, ".size() == 1"

    checker = <<~CPP
      int main() {
        if (head(big()) != 6) return 1;
        if (head(small()) != 3) return 2;
        if (head(leaf_kid()) != 3) return 3;
        if (head(Tree(Leaf{})) != 4) return 4;
        if (head(bare()) != 5) return 5;
        return 0;
      }
    CPP
    compile_both_compilers(source, "cyclic_sum_element_guard", checker, link_user_translation: true)
  end

  def compile_both_compilers(source, entry_name, checker, link_user_translation: false)
    runtime_directory = File.expand_path("../../runtime", __dir__)
    work_root = ENV.fetch("TMPDIR", "/tmp")
    Dir.mktmpdir("mlc_derive_json_#{entry_name}", work_root) do |work_directory|
      ruby_source = MLC.to_cpp(source)
        .gsub(/int main\(int argc, char\*\* argv\) noexcept\{return 0;\}\n?/, "")
        .gsub(/int main\(int argc, char\*\* argv\) noexcept;\n?/, "")
      ruby_path = File.join(work_directory, "ruby.cpp")
      ruby_binary = File.join(work_directory, "ruby_binary")
      File.write(ruby_path, ruby_source + "\n" + checker)
      ruby_compile = ["clang++", "-std=c++20", "-I", File.join(runtime_directory, "include"), "-o", ruby_binary, ruby_path]
      assert system(*ruby_compile), "clang++ failed for Ruby #{entry_name}"
      assert system(ruby_binary), "Ruby #{entry_name} failed"

      mlcc = File.expand_path("../../compiler/out/mlcc", __dir__)
      assert File.executable?(mlcc), "mlcc binary missing"
      generated_directory = File.join(work_directory, "mlcc")
      Dir.mkdir(generated_directory)
      entry_path = File.join(work_directory, "#{entry_name}.mlc")
      File.write(entry_path, source)
      assert system(mlcc, "-o", generated_directory, entry_path), "mlcc failed for #{entry_name}"
      header_path = File.join(generated_directory, "#{entry_name}.hpp")
      assert File.file?(header_path), "mlcc did not emit #{entry_name}.hpp"
      mlcc_path = File.join(work_directory, "mlcc.cpp")
      mlcc_binary = File.join(work_directory, "mlcc_binary")
      File.write(mlcc_path, <<~CPP)
        #include "#{entry_name}.hpp"
        using namespace #{entry_name};
        #{checker}
      CPP
      translation_units = [mlcc_path]
      if link_user_translation
        generated_cpp = File.read(File.join(generated_directory, "#{entry_name}.cpp"))
        stripped_path = File.join(work_directory, "user_translation.cpp")
        File.write(stripped_path, generated_cpp.sub(/\n#undef main\n.*\z/m, "\n"))
        translation_units << stripped_path
        translation_units << File.join(runtime_directory, "src/io/io.cpp")
        translation_units << File.join(runtime_directory, "src/core/string.cpp")
        translation_units << File.join(runtime_directory, "src/core/profile.cpp")
      end
      mlcc_compile = [
        "clang++", "-std=c++20",
        "-I", File.join(runtime_directory, "include"),
        "-I", generated_directory,
        "-o", mlcc_binary,
        *translation_units
      ]
      assert system(*mlcc_compile), "clang++ failed for mlcc #{entry_name}"
      assert system(mlcc_binary), "mlcc #{entry_name} failed"
    end
  end

  def assert_json_field_rejected(source, field_name, problem)
    error = assert_raises(MLC::CompileError) { MLC.to_cpp(source) }
    assert_includes error.message, "derive Json: unsupported field type for \"#{field_name}\""
    assert_includes error.message, "(#{problem})"

    mlcc = File.expand_path("../../compiler/out/mlcc", __dir__)
    assert File.executable?(mlcc), "mlcc binary missing"
    work_root = ENV.fetch("TMPDIR", "/tmp")
    Dir.mktmpdir("mlc_derive_json_reject", work_root) do |work_directory|
      entry_path = File.join(work_directory, "rejected.mlc")
      File.write(entry_path, source)
      output, status = Open3.capture2e(mlcc, "-o", work_directory, entry_path)
      refute status.success?, "mlcc accepted derive Json field #{field_name}"
      assert_includes output, "derive Json: unsupported field type for \"#{field_name}\""
      assert_includes output, "(#{problem})"
    end
  end
end
