#pragma once

// curl_version() returns const char*. The FFI binder only accepts functions
// whose parameters and result are mlc::String, int32_t, or int64_t by value.
// Failure codes: 1 invalid request, 2 resolve failed, 3 connect failed,
// 4 timed out, 5 certificate rejected, 6 response too large, 7 transport.
// 0 means the transfer finished and result_status is the HTTP status.
// result_message is curl's error text only; request headers are never copied into it.

#include "mlc/core/string.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <mutex>
#include <string>
#include <vector>

#if __has_include(<curl/curl.h>)
#include <curl/curl.h>
#else
#error "mlc/net/curl_abi.hpp requires curl/curl.h (install libcurl4-openssl-dev)"
#endif

namespace mlc {
namespace curl_abi {

inline String library_version_text() {
  const char* version = curl_version();
  if (version == nullptr) {
    return String();
  }
  return String(version);
}

namespace {

constexpr std::int32_t kInvalidRequest = 1;
constexpr std::int32_t kResolveFailed = 2;
constexpr std::int32_t kConnectFailed = 3;
constexpr std::int32_t kTimedOut = 4;
constexpr std::int32_t kCertificateRejected = 5;
constexpr std::int32_t kResponseTooLarge = 6;
constexpr std::int32_t kTransportFailed = 7;
constexpr std::size_t kHeaderBlockLimit = 64 * 1024;

struct ResultSlot {
  bool occupied = false;
  std::int32_t status = 0;
  std::int32_t failure_code = 0;
  String message;
  String header_block;
  String body;
};

struct ResultTable {
  std::mutex mutex;
  std::vector<ResultSlot> slots;
  std::int32_t live_count = 0;
};

struct TransferState {
  std::string body;
  std::string header_block;
  std::size_t maximum_body_bytes = 0;
  bool body_too_large = false;
  bool header_block_too_large = false;
};

struct EasyGuard {
  CURL* easy = nullptr;
  curl_slist* header_list = nullptr;
  ~EasyGuard() {
    if (header_list != nullptr) {
      curl_slist_free_all(header_list);
    }
    if (easy != nullptr) {
      curl_easy_cleanup(easy);
    }
  }
};

ResultTable& result_table() {
  static ResultTable table;
  return table;
}

void ensure_curl_ready() {
  static std::once_flag once;
  std::call_once(once, [] { curl_global_init(CURL_GLOBAL_DEFAULT); });
}

bool bytes_contain(const String& text, char byte) {
  const char* data = text.raw_data();
  const std::size_t size = text.raw_size();
  for (std::size_t index = 0; index < size; ++index) {
    if (data[index] == byte) return true;
  }
  return false;
}

bool url_rejected(const String& url) {
  if (bytes_contain(url, '\0') || bytes_contain(url, '\r') || bytes_contain(url, '\n')) {
    return true;
  }
  const std::string_view text = url.view();
  constexpr std::string_view prefix = "https://";
  return text.size() < prefix.size() || text.substr(0, prefix.size()) != prefix;
}

bool header_lines_rejected(const String& header_lines) {
  const char* data = header_lines.raw_data();
  const std::size_t size = header_lines.raw_size();
  std::size_t line_start = 0;
  for (std::size_t index = 0; index <= size; ++index) {
    const bool at_end = index == size;
    if (!at_end && (data[index] == '\0' || data[index] == '\r')) return true;
    if (!at_end && data[index] != '\n') continue;
    const std::size_t line_length = index - line_start;
    if (line_length > 0) {
      const void* colon = std::memchr(data + line_start, ':', line_length);
      if (colon == nullptr || static_cast<const char*>(colon) == data + line_start) return true;
    }
    line_start = index + 1;
  }
  return false;
}

bool method_rejected(const String& method) {
  const std::string_view text = method.view();
  return text != "GET" && text != "POST";
}

std::int32_t store_slot(ResultSlot slot) {
  ResultTable& table = result_table();
  std::lock_guard<std::mutex> lock(table.mutex);
  if (table.slots.size() >= 1000000) return -1;
  table.slots.push_back(std::move(slot));
  table.live_count += 1;
  return static_cast<std::int32_t>(table.slots.size() - 1);
}

std::int32_t store_invalid_request() {
  ResultSlot slot;
  slot.occupied = true;
  slot.status = 0;
  slot.failure_code = kInvalidRequest;
  slot.message = String("invalid request");
  return store_slot(std::move(slot));
}

const ResultSlot* live_slot(std::int32_t handle) {
  ResultTable& table = result_table();
  if (handle < 0 || static_cast<std::size_t>(handle) >= table.slots.size()) return nullptr;
  ResultSlot& slot = table.slots[static_cast<std::size_t>(handle)];
  if (!slot.occupied) return nullptr;
  return &slot;
}

std::size_t write_callback(char* pointer, std::size_t size, std::size_t member_count, void* userdata) {
  if (size != 0 && member_count > (static_cast<std::size_t>(-1) / size)) return 0;
  const std::size_t byte_count = size * member_count;
  auto* state = static_cast<TransferState*>(userdata);
  if (state->body.size() + byte_count > state->maximum_body_bytes) {
    state->body_too_large = true;
    return 0;
  }
  try {
    state->body.append(pointer, byte_count);
  } catch (...) {
    state->body_too_large = true;
    return 0;
  }
  return byte_count;
}

std::size_t header_callback(char* pointer, std::size_t size, std::size_t member_count, void* userdata) {
  if (size != 0 && member_count > (static_cast<std::size_t>(-1) / size)) return 0;
  const std::size_t byte_count = size * member_count;
  auto* state = static_cast<TransferState*>(userdata);
  if (byte_count >= 5 && std::memcmp(pointer, "HTTP/", 5) == 0) {
    state->header_block.clear();
  }
  if (state->header_block.size() + byte_count > kHeaderBlockLimit) {
    state->header_block_too_large = true;
    return 0;
  }
  try {
    state->header_block.append(pointer, byte_count);
  } catch (...) {
    state->header_block_too_large = true;
    return 0;
  }
  return byte_count;
}

bool append_header_list(curl_slist** header_list, const String& header_lines) {
  const char* data = header_lines.raw_data();
  const std::size_t size = header_lines.raw_size();
  std::size_t line_start = 0;
  for (std::size_t index = 0; index <= size; ++index) {
    const bool at_end = index == size;
    if (!at_end && data[index] != '\n') continue;
    if (index > line_start) {
      const std::string line(data + line_start, index - line_start);
      curl_slist* updated = curl_slist_append(*header_list, line.c_str());
      if (updated == nullptr) return false;
      *header_list = updated;
    }
    line_start = index + 1;
  }
  return true;
}

std::int32_t failure_code_from_curl(CURLcode code, bool body_too_large) {
  if (code == CURLE_OK) return 0;
  if (code == CURLE_WRITE_ERROR && body_too_large) return kResponseTooLarge;
  if (code == CURLE_COULDNT_RESOLVE_HOST) return kResolveFailed;
  if (code == CURLE_COULDNT_CONNECT) return kConnectFailed;
  if (code == CURLE_OPERATION_TIMEDOUT) return kTimedOut;
  if (code == CURLE_PEER_FAILED_VERIFICATION) return kCertificateRejected;
  return kTransportFailed;
}

String message_from_curl(CURLcode code, const char* error_buffer) {
  if (code == CURLE_OK) return String();
  std::string message = curl_easy_strerror(code);
  if (error_buffer != nullptr && error_buffer[0] != '\0') {
    message.append(": ");
    message.append(error_buffer);
  }
  return String(message);
}

String text_from_bytes(const std::string& bytes) {
  if (bytes.empty()) return String();
  return String(bytes.data(), bytes.size());
}

}  // namespace

inline std::int32_t perform_request(
    String method,
    String url,
    String header_lines,
    String body,
    std::int32_t timeout_milliseconds,
    std::int32_t max_response_bytes,
    String ca_bundle_path) {
  if (method_rejected(method) || url_rejected(url) || header_lines_rejected(header_lines) ||
      bytes_contain(ca_bundle_path, '\0')) {
    return store_invalid_request();
  }

  ensure_curl_ready();
  EasyGuard guard;
  guard.easy = curl_easy_init();
  if (guard.easy == nullptr) {
    ResultSlot slot;
    slot.occupied = true;
    slot.failure_code = kTransportFailed;
    slot.message = String("curl easy init failed");
    return store_slot(std::move(slot));
  }

  TransferState transfer;
  transfer.maximum_body_bytes =
      max_response_bytes > 0 ? static_cast<std::size_t>(max_response_bytes) : 0;
  char error_buffer[CURL_ERROR_SIZE];
  error_buffer[0] = '\0';

  curl_easy_setopt(guard.easy, CURLOPT_NOSIGNAL, 1L);
  curl_easy_setopt(guard.easy, CURLOPT_URL, url.raw_data());
  curl_easy_setopt(guard.easy, CURLOPT_TIMEOUT_MS, static_cast<long>(timeout_milliseconds));
  curl_easy_setopt(guard.easy, CURLOPT_CONNECTTIMEOUT_MS, static_cast<long>(timeout_milliseconds));
#if LIBCURL_VERSION_NUM >= 0x075500
  curl_easy_setopt(guard.easy, CURLOPT_PROTOCOLS_STR, "https");
#else
  curl_easy_setopt(guard.easy, CURLOPT_PROTOCOLS, CURLPROTO_HTTPS);
#endif
  curl_easy_setopt(guard.easy, CURLOPT_FOLLOWLOCATION, 0L);
  curl_easy_setopt(guard.easy, CURLOPT_PROXY, "");
  curl_easy_setopt(guard.easy, CURLOPT_ERRORBUFFER, error_buffer);
  curl_easy_setopt(guard.easy, CURLOPT_SSL_VERIFYPEER, 1L);
  curl_easy_setopt(guard.easy, CURLOPT_SSL_VERIFYHOST, 2L);
  curl_easy_setopt(guard.easy, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);
  if (ca_bundle_path.raw_size() > 0) {
    curl_easy_setopt(guard.easy, CURLOPT_CAINFO, ca_bundle_path.raw_data());
  }
  curl_easy_setopt(guard.easy, CURLOPT_WRITEFUNCTION, write_callback);
  curl_easy_setopt(guard.easy, CURLOPT_WRITEDATA, &transfer);
  curl_easy_setopt(guard.easy, CURLOPT_HEADERFUNCTION, header_callback);
  curl_easy_setopt(guard.easy, CURLOPT_HEADERDATA, &transfer);

  if (method.view() == "POST") {
    curl_easy_setopt(guard.easy, CURLOPT_POST, 1L);
    curl_easy_setopt(guard.easy, CURLOPT_POSTFIELDS, body.raw_data());
    curl_easy_setopt(
        guard.easy,
        CURLOPT_POSTFIELDSIZE_LARGE,
        static_cast<curl_off_t>(body.raw_size()));
  } else {
    curl_easy_setopt(guard.easy, CURLOPT_HTTPGET, 1L);
  }

  if (!append_header_list(&guard.header_list, header_lines)) {
    ResultSlot slot;
    slot.occupied = true;
    slot.failure_code = kTransportFailed;
    slot.message = String("curl header list failed");
    return store_slot(std::move(slot));
  }
  if (guard.header_list != nullptr) {
    curl_easy_setopt(guard.easy, CURLOPT_HTTPHEADER, guard.header_list);
  }

  const CURLcode code = curl_easy_perform(guard.easy);
  long response_code = 0;
  curl_easy_getinfo(guard.easy, CURLINFO_RESPONSE_CODE, &response_code);

  ResultSlot slot;
  slot.occupied = true;
  slot.status = static_cast<std::int32_t>(response_code);
  slot.failure_code = failure_code_from_curl(code, transfer.body_too_large);
  if (slot.failure_code == 0 && transfer.header_block_too_large) {
    slot.failure_code = kTransportFailed;
  }
  slot.message = message_from_curl(code, error_buffer);
  slot.header_block = text_from_bytes(transfer.header_block);
  slot.body = text_from_bytes(transfer.body);
  return store_slot(std::move(slot));
}

inline std::int32_t result_status(std::int32_t handle) {
  ResultTable& table = result_table();
  std::lock_guard<std::mutex> lock(table.mutex);
  const ResultSlot* slot = live_slot(handle);
  if (slot == nullptr) return -1;
  return slot->status;
}

inline std::int32_t result_failure_code(std::int32_t handle) {
  ResultTable& table = result_table();
  std::lock_guard<std::mutex> lock(table.mutex);
  const ResultSlot* slot = live_slot(handle);
  if (slot == nullptr) return -1;
  return slot->failure_code;
}

inline String result_message(std::int32_t handle) {
  ResultTable& table = result_table();
  std::lock_guard<std::mutex> lock(table.mutex);
  const ResultSlot* slot = live_slot(handle);
  if (slot == nullptr) return String();
  return slot->message;
}

inline String result_header_block(std::int32_t handle) {
  ResultTable& table = result_table();
  std::lock_guard<std::mutex> lock(table.mutex);
  const ResultSlot* slot = live_slot(handle);
  if (slot == nullptr) return String();
  return slot->header_block;
}

inline String result_body(std::int32_t handle) {
  ResultTable& table = result_table();
  std::lock_guard<std::mutex> lock(table.mutex);
  const ResultSlot* slot = live_slot(handle);
  if (slot == nullptr) return String();
  return slot->body;
}

inline std::int32_t release_result(std::int32_t handle) {
  ResultTable& table = result_table();
  std::lock_guard<std::mutex> lock(table.mutex);
  if (handle < 0 || static_cast<std::size_t>(handle) >= table.slots.size()) return 0;
  ResultSlot& slot = table.slots[static_cast<std::size_t>(handle)];
  if (!slot.occupied) return 0;
  slot.occupied = false;
  slot.message = String();
  slot.header_block = String();
  slot.body = String();
  slot.status = 0;
  slot.failure_code = 0;
  table.live_count -= 1;
  return 0;
}

inline std::int32_t live_result_count() {
  ResultTable& table = result_table();
  std::lock_guard<std::mutex> lock(table.mutex);
  return table.live_count;
}

}  // namespace curl_abi
}  // namespace mlc
