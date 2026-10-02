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
