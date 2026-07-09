# frozen_string_literal: true

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
        "g++", "-std=c++20",
        "-I", File.join(runtime_dir, "include"),
        "-o", binary_path,
        source_path
      ]
      assert system(*compile_cmd), "g++ failed for derive Json round-trip"
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
        "g++", "-std=c++20",
        "-I", File.join(runtime_dir, "include"),
        "-o", binary_path,
        source_path
      ]
      assert system(*compile_cmd), "g++ failed for derive Json sum round-trip"
      assert system(binary_path), "sum round-trip binary failed"
    end
  end
end
