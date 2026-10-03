#pragma once

// FFI results are int32_t and mlc::String. Kind codes:
// 0 null, 1 bool, 2 number, 3 string, 4 array, 5 object, 6 missing, 7 error.
// result_bool is 1 or 0 when the kind is bool, and 0 otherwise.
// result_text is the number's decimal text, the decoded string, compact JSON
// for an array or object, or the error message. Other kinds leave it empty.
// Text longer than 4194304 bytes, or text that contains a null byte, is an
// error and is not parsed.
// A missing object key or an array index outside the array is kind 6.
// json.mlc parse_json is a different representation and is not this ABI.

#include "mlc/core/string.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <exception>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

#include "../../../vendor/nlohmann/json.hpp"

namespace mlc {
namespace json_abi {

namespace {

constexpr std::int32_t kKindNull = 0;
constexpr std::int32_t kKindBool = 1;
constexpr std::int32_t kKindNumber = 2;
constexpr std::int32_t kKindString = 3;
constexpr std::int32_t kKindArray = 4;
constexpr std::int32_t kKindObject = 5;
constexpr std::int32_t kKindMissing = 6;
constexpr std::int32_t kKindError = 7;
constexpr std::size_t kMaximumJsonBytes = 4194304;
constexpr std::size_t kMaximumMessageBytes = 240;

struct ResultSlot {
  bool occupied = false;
  std::int32_t kind = kKindError;
  std::int32_t bool_value = 0;
  String text;
};

struct ResultTable {
  std::mutex mutex;
  std::vector<ResultSlot> slots;
  std::int32_t live_count = 0;
};

ResultTable& result_table() {
  static ResultTable table;
  return table;
}

std::int32_t store_slot(ResultSlot slot) {
  ResultTable& table = result_table();
  std::lock_guard<std::mutex> lock(table.mutex);
  if (table.slots.size() >= 1000000) return -1;
  table.slots.push_back(std::move(slot));
  table.live_count += 1;
  return static_cast<std::int32_t>(table.slots.size() - 1);
}

const ResultSlot* live_slot(std::int32_t handle) {
  ResultTable& table = result_table();
  if (handle < 0 || static_cast<std::size_t>(handle) >= table.slots.size()) return nullptr;
  ResultSlot& slot = table.slots[static_cast<std::size_t>(handle)];
  if (!slot.occupied) return nullptr;
  return &slot;
}

String bounded_message(const char* text) {
  if (text == nullptr || text[0] == '\0') return String("json parse failed");
  const std::size_t length = std::strlen(text);
  if (length <= kMaximumMessageBytes) return String(text, length);
  return String(text, kMaximumMessageBytes);
}

std::int32_t store_error(String message) {
  ResultSlot slot;
  slot.occupied = true;
  slot.kind = kKindError;
  slot.text = std::move(message);
  return store_slot(std::move(slot));
}

std::int32_t store_missing() {
  ResultSlot slot;
  slot.occupied = true;
  slot.kind = kKindMissing;
  return store_slot(std::move(slot));
}

std::int32_t store_classified(const nlohmann::json& value) {
  ResultSlot slot;
  slot.occupied = true;
  if (value.is_null()) {
    slot.kind = kKindNull;
  } else if (value.is_boolean()) {
    slot.kind = kKindBool;
    slot.bool_value = value.get<bool>() ? 1 : 0;
  } else if (value.is_number()) {
    slot.kind = kKindNumber;
    const std::string literal = value.dump();
    slot.text = String(literal.data(), literal.size());
  } else if (value.is_string()) {
    slot.kind = kKindString;
    const std::string decoded = value.get<std::string>();
    slot.text = String(decoded.data(), decoded.size());
  } else if (value.is_array()) {
    slot.kind = kKindArray;
    const std::string literal = value.dump();
    slot.text = String(literal.data(), literal.size());
  } else if (value.is_object()) {
    slot.kind = kKindObject;
    const std::string literal = value.dump();
    slot.text = String(literal.data(), literal.size());
  } else {
    slot.kind = kKindError;
    slot.text = String("json value kind is unsupported");
  }
  return store_slot(std::move(slot));
}

bool text_contains_null(const String& text) {
  const char* data = text.raw_data();
  const std::size_t size = text.raw_size();
  for (std::size_t index = 0; index < size; ++index) {
    if (data[index] == '\0') return true;
  }
  return false;
}

bool parse_document(const String& text, nlohmann::json* document, String* message) {
  if (text.raw_size() > kMaximumJsonBytes) {
    *message = String("json text exceeds 4194304 bytes");
    return false;
  }
  if (text_contains_null(text)) {
    *message = String("json text contains a null byte");
    return false;
  }
  try {
    *document = nlohmann::json::parse(
        text.raw_data(),
        text.raw_data() + text.raw_size(),
        nullptr,
        true,
        false);
    return true;
  } catch (const std::exception& error) {
    *message = bounded_message(error.what());
    return false;
  } catch (...) {
    *message = String("json parse failed");
    return false;
  }
}

}  // namespace

inline std::int32_t read_text(String text) {
  nlohmann::json document;
  String message;
  if (!parse_document(text, &document, &message)) return store_error(std::move(message));
  try {
    return store_classified(document);
  } catch (const std::exception& error) {
    return store_error(bounded_message(error.what()));
  } catch (...) {
    return store_error(String("json parse failed"));
  }
}

inline std::int32_t read_field(String text, String key) {
  nlohmann::json document;
  String message;
  if (!parse_document(text, &document, &message)) return store_error(std::move(message));
  if (!document.is_object()) return store_error(String("json value is not an object"));
  try {
    const std::string key_bytes(key.raw_data(), key.raw_size());
    const auto found = document.find(key_bytes);
    if (found == document.end()) return store_missing();
    return store_classified(*found);
  } catch (const std::exception& error) {
    return store_error(bounded_message(error.what()));
  } catch (...) {
    return store_error(String("json parse failed"));
  }
}

inline std::int32_t read_index(String text, std::int32_t index) {
  nlohmann::json document;
  String message;
  if (!parse_document(text, &document, &message)) return store_error(std::move(message));
  if (!document.is_array()) return store_error(String("json value is not an array"));
  if (index < 0) return store_missing();
  const auto element_index = static_cast<std::size_t>(index);
  if (element_index >= document.size()) return store_missing();
  try {
    return store_classified(document.at(element_index));
  } catch (const std::exception& error) {
    return store_error(bounded_message(error.what()));
  } catch (...) {
    return store_error(String("json parse failed"));
  }
}

inline std::int32_t result_kind(std::int32_t handle) {
  ResultTable& table = result_table();
  std::lock_guard<std::mutex> lock(table.mutex);
  const ResultSlot* slot = live_slot(handle);
  if (slot == nullptr) return -1;
  return slot->kind;
}

inline std::int32_t result_bool(std::int32_t handle) {
  ResultTable& table = result_table();
  std::lock_guard<std::mutex> lock(table.mutex);
  const ResultSlot* slot = live_slot(handle);
  if (slot == nullptr) return 0;
  return slot->bool_value;
}

inline String result_text(std::int32_t handle) {
  ResultTable& table = result_table();
  std::lock_guard<std::mutex> lock(table.mutex);
  const ResultSlot* slot = live_slot(handle);
  if (slot == nullptr) return String();
  return slot->text;
}

inline std::int32_t release_result(std::int32_t handle) {
  ResultTable& table = result_table();
  std::lock_guard<std::mutex> lock(table.mutex);
  if (handle < 0 || static_cast<std::size_t>(handle) >= table.slots.size()) return 0;
  ResultSlot& slot = table.slots[static_cast<std::size_t>(handle)];
  if (!slot.occupied) return 0;
  slot.occupied = false;
  slot.kind = kKindError;
  slot.bool_value = 0;
  slot.text = String();
  table.live_count -= 1;
  return 0;
}

inline std::int32_t live_result_count() {
  ResultTable& table = result_table();
  std::lock_guard<std::mutex> lock(table.mutex);
  return table.live_count;
}

}  // namespace json_abi
}  // namespace mlc
