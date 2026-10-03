#include "mlc/json/json_abi.hpp"

#include <string>
#include <thread>
#include <vector>

namespace {

struct Read {
  std::int32_t kind = -1;
  std::int32_t flag = 0;
  std::string text;
};

Read take(std::int32_t handle) {
  Read read;
  read.kind = mlc::json_abi::result_kind(handle);
  read.flag = mlc::json_abi::result_bool(handle);
  const mlc::String text = mlc::json_abi::result_text(handle);
  read.text = std::string(text.raw_data(), text.raw_size());
  mlc::json_abi::release_result(handle);
  return read;
}

int check_released_handle() {
  const std::int32_t handle = mlc::json_abi::read_text(mlc::String("null"));
  mlc::json_abi::release_result(handle);
  if (mlc::json_abi::result_kind(handle) != -1) return 30;
  if (mlc::json_abi::release_result(handle) != 0) return 31;
  if (mlc::json_abi::result_kind(-1) != -1) return 32;
  return 0;
}

int check_values() {
  const Read null_value = take(mlc::json_abi::read_text(mlc::String("null")));
  if (null_value.kind != 0 || !null_value.text.empty()) return 1;
  const Read true_value = take(mlc::json_abi::read_text(mlc::String("true")));
  if (true_value.kind != 1 || true_value.flag != 1) return 2;
  const Read false_value = take(mlc::json_abi::read_text(mlc::String("false")));
  if (false_value.kind != 1 || false_value.flag != 0) return 3;
  const Read number_value = take(mlc::json_abi::read_text(mlc::String("9007199254740993")));
  if (number_value.kind != 2 || number_value.text != "9007199254740993") return 4;
  const Read string_value = take(mlc::json_abi::read_text(mlc::String("\"a\\\"b\"")));
  if (string_value.kind != 3 || string_value.text != "a\"b") return 5;
  const Read array_value = take(mlc::json_abi::read_text(mlc::String("[1,\"b\"]")));
  if (array_value.kind != 4 || array_value.text != "[1,\"b\"]") return 6;
  const Read object_value = take(mlc::json_abi::read_field(
      mlc::String("{\"user\":{\"id\":1}}"), mlc::String("user")));
  if (object_value.kind != 5 || object_value.text != "{\"id\":1}") return 7;
  const Read missing_value = take(mlc::json_abi::read_field(
      mlc::String("{\"id\":1}"), mlc::String("missing")));
  if (missing_value.kind != 6) return 8;
  const Read null_field = take(mlc::json_abi::read_field(
      mlc::String("{\"id\":null}"), mlc::String("id")));
  if (null_field.kind != 0) return 9;
  const Read empty_key = take(mlc::json_abi::read_field(
      mlc::String("{\"\":1}"), mlc::String("")));
  if (empty_key.kind != 2 || empty_key.text != "1") return 10;
  const Read broken = take(mlc::json_abi::read_text(mlc::String("{")));
  if (broken.kind != 7 || broken.text.empty()) return 11;
  const Read not_object = take(mlc::json_abi::read_field(
      mlc::String("[]"), mlc::String("name")));
  if (not_object.kind != 7 || not_object.text != "json value is not an object") return 12;
  const Read element = take(mlc::json_abi::read_index(mlc::String("[1,\"b\"]"), 1));
  if (element.kind != 3 || element.text != "b") return 13;
  const Read outside = take(mlc::json_abi::read_index(mlc::String("[1]"), 3));
  if (outside.kind != 6) return 14;
  const Read negative = take(mlc::json_abi::read_index(mlc::String("[1]"), -1));
  if (negative.kind != 6) return 15;
  const Read not_array = take(mlc::json_abi::read_index(mlc::String("{}"), 0));
  if (not_array.kind != 7 || not_array.text != "json value is not an array") return 16;
  std::string embedded("true");
  embedded.push_back('\0');
  embedded += "false";
  const Read embedded_null = take(mlc::json_abi::read_text(mlc::String(embedded)));
  if (embedded_null.kind != 7 || embedded_null.text != "json text contains a null byte") return 17;
  const Read trailing = take(mlc::json_abi::read_text(mlc::String("true false")));
  if (trailing.kind != 7) return 18;
  const std::string huge(4194305, ' ');
  const Read too_large = take(mlc::json_abi::read_text(mlc::String(huge)));
  if (too_large.kind != 7 || too_large.text != "json text exceeds 4194304 bytes") return 19;
  return 0;
}

int check_threads() {
  constexpr int kThreadCount = 4;
  constexpr int kReadsPerThread = 40;
  std::vector<int> failures(static_cast<std::size_t>(kThreadCount), 0);
  std::vector<std::thread> threads;
  for (int thread_index = 0; thread_index < kThreadCount; ++thread_index) {
    threads.emplace_back([thread_index, &failures] {
      for (int read_index = 0; read_index < kReadsPerThread; ++read_index) {
        const Read value = take(mlc::json_abi::read_text(mlc::String("true")));
        if (value.kind != 1 || value.flag != 1) {
          failures[static_cast<std::size_t>(thread_index)] = 40;
          return;
        }
      }
    });
  }
  for (std::thread& thread : threads) thread.join();
  for (int failure : failures) {
    if (failure != 0) return failure;
  }
  return 0;
}

}  // namespace

int main() {
  if (mlc::json_abi::live_result_count() != 0) return 50;
  const int values = check_values();
  if (values != 0) return values;
  const int released = check_released_handle();
  if (released != 0) return released;
  const int threads = check_threads();
  if (threads != 0) return threads;
  if (mlc::json_abi::live_result_count() != 0) return 51;
  return 0;
}
