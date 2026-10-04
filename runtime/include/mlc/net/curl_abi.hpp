#pragma once

// curl_version() returns const char*. The FFI binder only accepts functions
// whose parameters and result are mlc::String, int32_t, or int64_t by value.
// Failure codes: 1 invalid request, 2 resolve failed, 3 connect failed,
// 4 timed out, 5 certificate rejected, 6 response too large, 7 transport.
// 0 means the transfer finished and result_status is the HTTP status.
// result_message is curl's error text only; request headers are never copied into it.
// result_stream_kind is 0 for a one-shot result. A stream event is
// 1 headers, 2 body chunk, 3 end, or 4 failure.
// stream_close sets the stop flag, wakes the worker, and joins it.
// A StopToken that is already requested, or becomes requested during the
// transfer, fails the call with transport code 7 and message "stop requested".
// The progress callback and the 50 millisecond poll both observe the token.
// An empty proxy URL sets CURLOPT_PROXY to "" so proxy environment variables
// are ignored. A non-empty proxy URL is CURLOPT_PROXY, and CURLOPT_NOPROXY is
// cleared for that transfer so NO_PROXY does not bypass it.
// The transfer offers HTTP/2 and falls back to HTTP/1.1.
// libcurl decodes the response body. max_response_bytes counts those decoded bytes.

#include "mlc/concurrency/stop.hpp"
#include "mlc/core/string.hpp"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <deque>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <utility>
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

// Process-wide tables. An anonymous namespace in this header would give each
// translation unit its own copy, so a store in one module would miss a read in another.
struct ResultSlot {
  bool occupied = false;
  std::int32_t status = 0;
  std::int32_t failure_code = 0;
  std::int32_t new_connection_count = 0;
  std::int32_t http_version = 0;
  std::int32_t stream_kind = 0;
  String message;
  String header_block;
  String body;
};

struct ResultTable {
  std::mutex mutex;
  std::vector<ResultSlot> slots;
  std::int32_t live_count = 0;
};

inline ResultTable& result_table() {
  static ResultTable table;
  return table;
}

struct SessionSlot {
  std::mutex mutex;
  bool occupied = false;
  CURL* curl_handle = nullptr;
  String certificate_bundle_path;
};

struct SessionTable {
  std::mutex mutex;
  std::vector<std::unique_ptr<SessionSlot>> slots;
  std::int32_t live_count = 0;
};

inline SessionTable& session_table() {
  static SessionTable table;
  return table;
}

struct TransferState {
  std::string body;
  std::string header_block;
  std::size_t maximum_body_bytes = 0;
  bool body_too_large = false;
  bool header_block_too_large = false;
};

constexpr std::int32_t kStreamHeaders = 1;
constexpr std::int32_t kStreamChunk = 2;
constexpr std::int32_t kStreamEnd = 3;
constexpr std::int32_t kStreamFailure = 4;
constexpr std::size_t kStreamQueueLimit = 32;

struct StreamEvent {
  std::int32_t kind = kStreamEnd;
  std::int32_t status = 0;
  std::int32_t failure_code = 0;
  std::string message;
  std::string header_block;
  std::string body;
};

struct StreamSlot {
  std::mutex mutex;
  std::condition_variable ready;
  std::condition_variable space;
  std::atomic<bool> stop_requested{false};
  std::atomic<bool> cancelled_by_token{false};
  std::stop_token cancel_token;
  bool occupied = false;
  bool closing = false;
  bool worker_finished = false;
  bool headers_queued = false;
  bool terminal_queued = false;
  std::deque<StreamEvent> events;
  std::thread worker;
  TransferState transfer;
  std::size_t body_bytes = 0;
  CURL* easy = nullptr;
  std::string method;
  std::string url;
  std::string header_lines;
  std::string body;
  std::string certificate_bundle_path;
  std::string proxy_url;
  std::int32_t timeout_milliseconds = 0;
};

struct StreamTable {
  std::mutex mutex;
  std::vector<std::unique_ptr<StreamSlot>> slots;
  std::int32_t live_count = 0;
};

inline StreamTable& stream_table() {
  static StreamTable table;
  return table;
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

bool proxy_rejected(const String& proxy_url) {
  const char* data = proxy_url.raw_data();
  const std::size_t size = proxy_url.raw_size();
  for (std::size_t index = 0; index < size; ++index) {
    const unsigned char byte = static_cast<unsigned char>(data[index]);
    if (byte <= ' ' || byte > '~') return true;
  }
  if (size == 0) return false;
  const std::string_view text = proxy_url.view();
  constexpr std::string_view schemes[] = {
      "http://", "https://", "socks4://", "socks4a://", "socks5://", "socks5h://"};
  for (const std::string_view scheme : schemes) {
    if (text.size() > scheme.size() && text.substr(0, scheme.size()) == scheme &&
        text[scheme.size()] != '/') {
      return false;
    }
  }
  return true;
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
  return text != "GET" && text != "HEAD" && text != "POST" && text != "PUT" &&
         text != "PATCH" && text != "DELETE";
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

std::int32_t store_transport_message(const char* message) {
  ResultSlot slot;
  slot.occupied = true;
  slot.failure_code = kTransportFailed;
  slot.message = String(message);
  return store_slot(std::move(slot));
}

std::size_t write_callback(char* pointer, std::size_t size, std::size_t member_count, void* userdata);
std::size_t header_callback(char* pointer, std::size_t size, std::size_t member_count, void* userdata);
bool append_header_list(curl_slist** header_list, const String& header_lines);
std::int32_t failure_code_from_curl(CURLcode code, bool body_too_large);
String message_from_curl(CURLcode code, const char* error_buffer);
String text_from_bytes(const std::string& bytes);

bool bind_https_request(
    CURL* curl_handle,
    TransferState* transfer,
    char* error_buffer,
    const String& method,
    const String& url,
    const String& header_lines,
    const String& body,
    std::int32_t timeout_milliseconds,
    const String& ca_bundle_path,
    const String& proxy_url,
    bool force_new_connection,
    std::size_t (*body_callback)(char*, std::size_t, std::size_t, void*),
    void* body_data,
    curl_slist** header_list) {
  *header_list = nullptr;
  curl_easy_setopt(curl_handle, CURLOPT_NOSIGNAL, 1L);
  curl_easy_setopt(curl_handle, CURLOPT_URL, url.raw_data());
  curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT_MS, static_cast<long>(timeout_milliseconds));
  curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT_MS, static_cast<long>(timeout_milliseconds));
#if LIBCURL_VERSION_NUM >= 0x075500
  curl_easy_setopt(curl_handle, CURLOPT_PROTOCOLS_STR, "https");
#else
  curl_easy_setopt(curl_handle, CURLOPT_PROTOCOLS, CURLPROTO_HTTPS);
#endif
  curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 0L);
  if (proxy_url.raw_size() == 0) {
    curl_easy_setopt(curl_handle, CURLOPT_PROXY, "");
  } else {
    curl_easy_setopt(curl_handle, CURLOPT_PROXY, proxy_url.raw_data());
    curl_easy_setopt(curl_handle, CURLOPT_NOPROXY, "");
  }
  curl_easy_setopt(curl_handle, CURLOPT_ERRORBUFFER, error_buffer);
  curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 1L);
  curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, 2L);
  curl_easy_setopt(curl_handle, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2TLS);
  curl_easy_setopt(curl_handle, CURLOPT_ACCEPT_ENCODING, "");
  if (force_new_connection) {
    curl_easy_setopt(curl_handle, CURLOPT_FRESH_CONNECT, 1L);
  }
  if (ca_bundle_path.raw_size() > 0) {
    curl_easy_setopt(curl_handle, CURLOPT_CAINFO, ca_bundle_path.raw_data());
  }
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, body_callback);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, body_data);
  curl_easy_setopt(curl_handle, CURLOPT_HEADERFUNCTION, header_callback);
  curl_easy_setopt(curl_handle, CURLOPT_HEADERDATA, transfer);

  const std::string_view method_text = method.view();
  if (method_text == "POST" || method_text == "PUT" || method_text == "PATCH" ||
      method_text == "DELETE") {
    if (method_text == "POST") {
      curl_easy_setopt(curl_handle, CURLOPT_POST, 1L);
    } else {
      curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, method.raw_data());
    }
    curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, body.raw_data());
    curl_easy_setopt(
        curl_handle,
        CURLOPT_POSTFIELDSIZE_LARGE,
        static_cast<curl_off_t>(body.raw_size()));
  } else if (method_text == "HEAD") {
    curl_easy_setopt(curl_handle, CURLOPT_NOBODY, 1L);
  } else {
    curl_easy_setopt(curl_handle, CURLOPT_HTTPGET, 1L);
  }

  if (!append_header_list(header_list, header_lines)) {
    if (*header_list != nullptr) curl_slist_free_all(*header_list);
    *header_list = nullptr;
    return false;
  }
  if (*header_list != nullptr) {
    curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, *header_list);
  }
  return true;
}

std::int32_t perform_on_curl_handle(
    CURL* curl_handle,
    bool force_new_connection,
    const String& method,
    const String& url,
    const String& header_lines,
    const String& body,
    std::int32_t timeout_milliseconds,
    std::int32_t max_response_bytes,
    const String& ca_bundle_path,
    const String& proxy_url) {
  curl_easy_reset(curl_handle);
  TransferState transfer;
  transfer.maximum_body_bytes =
      max_response_bytes > 0 ? static_cast<std::size_t>(max_response_bytes) : 0;
  char error_buffer[CURL_ERROR_SIZE];
  error_buffer[0] = '\0';
  curl_slist* header_list = nullptr;
  if (!bind_https_request(
          curl_handle,
          &transfer,
          error_buffer,
          method,
          url,
          header_lines,
          body,
          timeout_milliseconds,
          ca_bundle_path,
          proxy_url,
          force_new_connection,
          write_callback,
          &transfer,
          &header_list)) {
    return store_transport_message("curl header list failed");
  }

  const CURLcode code = curl_easy_perform(curl_handle);
  long response_code = 0;
  curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &response_code);
  long new_connection_count = 0;
  curl_easy_getinfo(curl_handle, CURLINFO_NUM_CONNECTS, &new_connection_count);
  long http_version = 0;
  curl_easy_getinfo(curl_handle, CURLINFO_HTTP_VERSION, &http_version);
  if (header_list != nullptr) curl_slist_free_all(header_list);

  ResultSlot slot;
  slot.occupied = true;
  slot.status = static_cast<std::int32_t>(response_code);
  slot.http_version = static_cast<std::int32_t>(http_version);
  slot.failure_code = failure_code_from_curl(code, transfer.body_too_large);
  if (slot.failure_code == 0 && transfer.header_block_too_large) {
    slot.failure_code = kTransportFailed;
  }
  slot.new_connection_count = static_cast<std::int32_t>(new_connection_count);
  slot.message = message_from_curl(code, error_buffer);
  slot.header_block = text_from_bytes(transfer.header_block);
  slot.body = text_from_bytes(transfer.body);
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

std::string copy_bytes(const String& text) {
  if (text.raw_size() == 0 || text.raw_data() == nullptr) return std::string();
  return std::string(text.raw_data(), text.raw_size());
}

void enqueue_headers(StreamSlot* slot) {
  StreamEvent event;
  event.kind = kStreamHeaders;
  long response_code = 0;
  if (slot->easy != nullptr) {
    curl_easy_getinfo(slot->easy, CURLINFO_RESPONSE_CODE, &response_code);
  }
  event.status = static_cast<std::int32_t>(response_code);
  event.header_block = slot->transfer.header_block;
  slot->events.push_back(std::move(event));
  slot->headers_queued = true;
  slot->ready.notify_all();
}

void enqueue_end(StreamSlot* slot) {
  StreamEvent event;
  event.kind = kStreamEnd;
  slot->events.push_back(std::move(event));
  slot->terminal_queued = true;
  slot->ready.notify_all();
}

void enqueue_failure(StreamSlot* slot, std::int32_t failure_code, const std::string& message) {
  StreamEvent event;
  event.kind = kStreamFailure;
  event.failure_code = failure_code;
  event.message = message;
  slot->events.push_back(std::move(event));
  slot->terminal_queued = true;
  slot->ready.notify_all();
}

int stream_transfer_progress(
    void* userdata,
    curl_off_t download_total,
    curl_off_t downloaded,
    curl_off_t upload_total,
    curl_off_t uploaded) {
  (void)download_total;
  (void)downloaded;
  (void)upload_total;
  (void)uploaded;
  auto* slot = static_cast<StreamSlot*>(userdata);
  if (slot->cancel_token.stop_requested()) {
    slot->cancelled_by_token.store(true);
    slot->stop_requested.store(true);
    return 1;
  }
  return slot->stop_requested.load() ? 1 : 0;
}

std::size_t stream_body_callback(
    char* pointer,
    std::size_t size,
    std::size_t member_count,
    void* userdata) {
  if (size != 0 && member_count > (static_cast<std::size_t>(-1) / size)) return 0;
  const std::size_t byte_count = size * member_count;
  if (byte_count == 0) return 0;
  auto* slot = static_cast<StreamSlot*>(userdata);
  std::unique_lock<std::mutex> lock(slot->mutex);
  if (slot->stop_requested.load()) return 0;
  if (slot->body_bytes + byte_count > slot->transfer.maximum_body_bytes) {
    slot->transfer.body_too_large = true;
    enqueue_failure(slot, kResponseTooLarge, "response too large");
    slot->stop_requested.store(true);
    return 0;
  }
  while (slot->events.size() >= kStreamQueueLimit && !slot->stop_requested.load() &&
         !slot->cancel_token.stop_requested()) {
    slot->space.wait_for(lock, std::chrono::milliseconds(50));
  }
  if (slot->cancel_token.stop_requested()) {
    slot->cancelled_by_token.store(true);
    slot->stop_requested.store(true);
    return 0;
  }
  if (slot->stop_requested.load()) return 0;
  if (!slot->headers_queued) enqueue_headers(slot);
  StreamEvent event;
  event.kind = kStreamChunk;
  event.body.assign(pointer, byte_count);
  slot->events.push_back(std::move(event));
  slot->body_bytes += byte_count;
  slot->ready.notify_all();
  return byte_count;
}

void run_stream(StreamSlot* slot) {
  CURL* easy = curl_easy_init();
  CURLM* multi = easy == nullptr ? nullptr : curl_multi_init();
  if (easy == nullptr || multi == nullptr) {
    std::lock_guard<std::mutex> lock(slot->mutex);
    enqueue_failure(slot, kTransportFailed, "curl stream init failed");
    slot->worker_finished = true;
    slot->ready.notify_all();
    if (easy != nullptr) curl_easy_cleanup(easy);
    if (multi != nullptr) curl_multi_cleanup(multi);
    return;
  }
  slot->easy = easy;
  char error_buffer[CURL_ERROR_SIZE];
  error_buffer[0] = '\0';
  const String method(slot->method);
  const String url(slot->url);
  const String header_lines(slot->header_lines);
  const String body(slot->body);
  const String certificate_bundle(slot->certificate_bundle_path);
  const String proxy(slot->proxy_url);
  curl_slist* header_list = nullptr;
  if (!bind_https_request(
          easy,
          &slot->transfer,
          error_buffer,
          method,
          url,
          header_lines,
          body,
          slot->timeout_milliseconds,
          certificate_bundle,
          proxy,
          false,
          stream_body_callback,
          slot,
          &header_list)) {
    std::lock_guard<std::mutex> lock(slot->mutex);
    enqueue_failure(slot, kTransportFailed, "curl header list failed");
    slot->worker_finished = true;
    slot->ready.notify_all();
    slot->easy = nullptr;
    curl_easy_cleanup(easy);
    curl_multi_cleanup(multi);
    return;
  }
  curl_easy_setopt(easy, CURLOPT_NOPROGRESS, 0L);
  curl_easy_setopt(easy, CURLOPT_XFERINFOFUNCTION, stream_transfer_progress);
  curl_easy_setopt(easy, CURLOPT_XFERINFODATA, slot);
  curl_multi_add_handle(multi, easy);

  int running = 0;
  CURLMcode multi_code = CURLM_OK;
  while (!slot->stop_requested.load()) {
    if (slot->cancel_token.stop_requested()) {
      slot->cancelled_by_token.store(true);
      slot->stop_requested.store(true);
      break;
    }
    multi_code = curl_multi_perform(multi, &running);
    if (multi_code != CURLM_OK || running == 0) break;
    if (slot->stop_requested.load()) break;
    curl_multi_poll(multi, nullptr, 0, 50, nullptr);
  }

  CURLcode code = CURLE_OK;
  int pending = 0;
  while (CURLMsg* message = curl_multi_info_read(multi, &pending)) {
    if (message->msg == CURLMSG_DONE) code = message->data.result;
  }

  {
    std::lock_guard<std::mutex> lock(slot->mutex);
    if (!slot->terminal_queued) {
      if (slot->cancelled_by_token.load()) {
        enqueue_failure(slot, kTransportFailed, "stop requested");
      } else if (slot->stop_requested.load() && code != CURLE_OK) {
        enqueue_end(slot);
      } else if (code == CURLE_OK && !slot->transfer.header_block_too_large) {
        if (!slot->headers_queued) enqueue_headers(slot);
        enqueue_end(slot);
      } else if (slot->transfer.body_too_large) {
        enqueue_failure(slot, kResponseTooLarge, "response too large");
      } else {
        std::int32_t failure = failure_code_from_curl(code, false);
        if (slot->transfer.header_block_too_large) failure = kTransportFailed;
        enqueue_failure(slot, failure, copy_bytes(message_from_curl(code, error_buffer)));
      }
    }
    slot->worker_finished = true;
    slot->ready.notify_all();
  }

  curl_multi_remove_handle(multi, easy);
  if (header_list != nullptr) curl_slist_free_all(header_list);
  slot->easy = nullptr;
  curl_easy_cleanup(easy);
  curl_multi_cleanup(multi);
}

std::int32_t store_stream_event(const StreamEvent& event) {
  ResultSlot slot;
  slot.occupied = true;
  slot.stream_kind = event.kind;
  slot.status = event.status;
  slot.failure_code = event.failure_code;
  slot.message = String(event.message);
  slot.header_block = String(event.header_block);
  slot.body = String(event.body);
  return store_slot(std::move(slot));
}

StreamSlot* stream_slot(std::int32_t stream) {
  StreamTable& table = stream_table();
  std::lock_guard<std::mutex> lock(table.mutex);
  if (stream < 0 || static_cast<std::size_t>(stream) >= table.slots.size()) return nullptr;
  return table.slots[static_cast<std::size_t>(stream)].get();
}

StreamEvent take_stream_event(StreamSlot* slot, bool* closed) {
  std::unique_lock<std::mutex> lock(slot->mutex);
  while (slot->events.empty() && !slot->worker_finished && slot->occupied) {
    slot->ready.wait(lock);
  }
  if (!slot->occupied && slot->events.empty()) {
    *closed = true;
    return StreamEvent{};
  }
  *closed = false;
  if (slot->events.empty()) {
    StreamEvent event;
    event.kind = kStreamEnd;
    return event;
  }
  StreamEvent event = std::move(slot->events.front());
  slot->events.pop_front();
  slot->space.notify_one();
  return event;
}

struct CancelWatch {
  std::stop_token token;
  std::atomic<bool> triggered{false};
};

int cancel_transfer_progress(
    void* userdata,
    curl_off_t download_total,
    curl_off_t downloaded,
    curl_off_t upload_total,
    curl_off_t uploaded) {
  (void)download_total;
  (void)downloaded;
  (void)upload_total;
  (void)uploaded;
  auto* watch = static_cast<CancelWatch*>(userdata);
  if (watch->token.stop_requested()) {
    watch->triggered.store(true);
    return 1;
  }
  return 0;
}

struct MultiGuard {
  CURL* easy = nullptr;
  CURLM* multi = nullptr;
  curl_slist* header_list = nullptr;
  ~MultiGuard() {
    if (multi != nullptr && easy != nullptr) curl_multi_remove_handle(multi, easy);
    if (header_list != nullptr) curl_slist_free_all(header_list);
    if (easy != nullptr) curl_easy_cleanup(easy);
    if (multi != nullptr) curl_multi_cleanup(multi);
  }
};

std::int32_t perform_until_stop(
    const String& method,
    const String& url,
    const String& header_lines,
    const String& body,
    std::int32_t timeout_milliseconds,
    std::int32_t max_response_bytes,
    const String& ca_bundle_path,
    const String& proxy_url,
    const std::stop_token& cancel_token) {
  if (cancel_token.stop_requested()) return store_transport_message("stop requested");
  ensure_curl_ready();
  MultiGuard guard;
  guard.easy = curl_easy_init();
  guard.multi = guard.easy == nullptr ? nullptr : curl_multi_init();
  if (guard.easy == nullptr || guard.multi == nullptr) {
    return store_transport_message("curl easy init failed");
  }
  TransferState transfer;
  transfer.maximum_body_bytes =
      max_response_bytes > 0 ? static_cast<std::size_t>(max_response_bytes) : 0;
  char error_buffer[CURL_ERROR_SIZE];
  error_buffer[0] = '\0';
  if (!bind_https_request(
          guard.easy,
          &transfer,
          error_buffer,
          method,
          url,
          header_lines,
          body,
          timeout_milliseconds,
          ca_bundle_path,
          proxy_url,
          false,
          write_callback,
          &transfer,
          &guard.header_list)) {
    return store_transport_message("curl header list failed");
  }
  CancelWatch watch;
  watch.token = cancel_token;
  curl_easy_setopt(guard.easy, CURLOPT_NOPROGRESS, 0L);
  curl_easy_setopt(guard.easy, CURLOPT_XFERINFOFUNCTION, cancel_transfer_progress);
  curl_easy_setopt(guard.easy, CURLOPT_XFERINFODATA, &watch);
  curl_multi_add_handle(guard.multi, guard.easy);

  int running = 0;
  while (!watch.triggered.load()) {
    if (watch.token.stop_requested()) {
      watch.triggered.store(true);
      break;
    }
    const CURLMcode multi_code = curl_multi_perform(guard.multi, &running);
    if (multi_code != CURLM_OK || running == 0) break;
    if (watch.token.stop_requested()) {
      watch.triggered.store(true);
      break;
    }
    curl_multi_poll(guard.multi, nullptr, 0, 50, nullptr);
  }
  if (watch.triggered.load()) return store_transport_message("stop requested");

  CURLcode code = CURLE_OK;
  int pending = 0;
  while (CURLMsg* message = curl_multi_info_read(guard.multi, &pending)) {
    if (message->msg == CURLMSG_DONE) code = message->data.result;
  }
  long response_code = 0;
  curl_easy_getinfo(guard.easy, CURLINFO_RESPONSE_CODE, &response_code);
  long new_connection_count = 0;
  curl_easy_getinfo(guard.easy, CURLINFO_NUM_CONNECTS, &new_connection_count);
  long http_version = 0;
  curl_easy_getinfo(guard.easy, CURLINFO_HTTP_VERSION, &http_version);
  ResultSlot slot;
  slot.occupied = true;
  slot.status = static_cast<std::int32_t>(response_code);
  slot.http_version = static_cast<std::int32_t>(http_version);
  slot.failure_code = failure_code_from_curl(code, transfer.body_too_large);
  if (slot.failure_code == 0 && transfer.header_block_too_large) {
    slot.failure_code = kTransportFailed;
  }
  slot.new_connection_count = static_cast<std::int32_t>(new_connection_count);
  slot.message = message_from_curl(code, error_buffer);
  slot.header_block = text_from_bytes(transfer.header_block);
  slot.body = text_from_bytes(transfer.body);
  return store_slot(std::move(slot));
}

std::int32_t open_stream_with_token(
    const String& method,
    const String& url,
    const String& header_lines,
    const String& body,
    std::int32_t timeout_milliseconds,
    std::int32_t max_response_bytes,
    const String& ca_bundle_path,
    const String& proxy_url,
    std::stop_token cancel_token) {
  if (method_rejected(method) || url_rejected(url) || header_lines_rejected(header_lines) ||
      bytes_contain(ca_bundle_path, '\0') || proxy_rejected(proxy_url)) {
    return -1;
  }
  const bool already_cancelled = cancel_token.stop_requested();
  if (!already_cancelled) ensure_curl_ready();
  auto slot = std::make_unique<StreamSlot>();
  slot->occupied = true;
  slot->transfer.maximum_body_bytes =
      max_response_bytes > 0 ? static_cast<std::size_t>(max_response_bytes) : 0;
  slot->method = copy_bytes(method);
  slot->url = copy_bytes(url);
  slot->header_lines = copy_bytes(header_lines);
  slot->body = copy_bytes(body);
  slot->certificate_bundle_path = copy_bytes(ca_bundle_path);
  slot->proxy_url = copy_bytes(proxy_url);
  slot->timeout_milliseconds = timeout_milliseconds;
  slot->cancel_token = std::move(cancel_token);
  if (already_cancelled) {
    enqueue_failure(slot.get(), kTransportFailed, "stop requested");
    slot->worker_finished = true;
  }
  StreamTable& table = stream_table();
  std::lock_guard<std::mutex> lock(table.mutex);
  if (table.slots.size() >= 1000000) return -1;
  table.slots.push_back(std::move(slot));
  StreamSlot* pointer = table.slots.back().get();
  table.live_count += 1;
  if (!already_cancelled) pointer->worker = std::thread(run_stream, pointer);
  return static_cast<std::int32_t>(table.slots.size() - 1);
}

}  // namespace

inline std::int32_t perform_request(
    String method,
    String url,
    String header_lines,
    String body,
    std::int32_t timeout_milliseconds,
    std::int32_t max_response_bytes,
    String ca_bundle_path,
    String proxy_url) {
  if (method_rejected(method) || url_rejected(url) || header_lines_rejected(header_lines) ||
      bytes_contain(ca_bundle_path, '\0') || proxy_rejected(proxy_url)) {
    return store_invalid_request();
  }

  ensure_curl_ready();
  EasyGuard guard;
  guard.easy = curl_easy_init();
  if (guard.easy == nullptr) {
    return store_transport_message("curl easy init failed");
  }
  return perform_on_curl_handle(
      guard.easy,
      false,
      method,
      url,
      header_lines,
      body,
      timeout_milliseconds,
      max_response_bytes,
      ca_bundle_path,
      proxy_url);
}

inline std::int32_t perform_request_with_stop(
    String method,
    String url,
    String header_lines,
    String body,
    std::int32_t timeout_milliseconds,
    std::int32_t max_response_bytes,
    String ca_bundle_path,
    String proxy_url,
    mlc::concurrency::StopToken stop_token) {
  if (method_rejected(method) || url_rejected(url) || header_lines_rejected(header_lines) ||
      bytes_contain(ca_bundle_path, '\0') || proxy_rejected(proxy_url)) {
    return store_invalid_request();
  }
  return perform_until_stop(
      method,
      url,
      header_lines,
      body,
      timeout_milliseconds,
      max_response_bytes,
      ca_bundle_path,
      proxy_url,
      stop_token.native_token());
}

inline std::int32_t session_open() {
  ensure_curl_ready();
  CURL* curl_handle = curl_easy_init();
  if (curl_handle == nullptr) return -1;
  auto slot = std::make_unique<SessionSlot>();
  slot->occupied = true;
  slot->curl_handle = curl_handle;
  SessionTable& table = session_table();
  std::lock_guard<std::mutex> lock(table.mutex);
  if (table.slots.size() >= 1000000) {
    curl_easy_cleanup(curl_handle);
    return -1;
  }
  table.slots.push_back(std::move(slot));
  table.live_count += 1;
  return static_cast<std::int32_t>(table.slots.size() - 1);
}

inline std::int32_t session_close(std::int32_t session) {
  SessionTable& table = session_table();
  std::lock_guard<std::mutex> table_lock(table.mutex);
  if (session < 0 || static_cast<std::size_t>(session) >= table.slots.size()) return 0;
  SessionSlot* slot = table.slots[static_cast<std::size_t>(session)].get();
  std::lock_guard<std::mutex> session_lock(slot->mutex);
  if (!slot->occupied) return 0;
  if (slot->curl_handle != nullptr) curl_easy_cleanup(slot->curl_handle);
  slot->curl_handle = nullptr;
  slot->occupied = false;
  slot->certificate_bundle_path = String();
  table.live_count -= 1;
  return 0;
}

inline std::int32_t live_session_count() {
  SessionTable& table = session_table();
  std::lock_guard<std::mutex> lock(table.mutex);
  return table.live_count;
}

inline std::int32_t session_perform(
    std::int32_t session,
    String method,
    String url,
    String header_lines,
    String body,
    std::int32_t timeout_milliseconds,
    std::int32_t max_response_bytes,
    String ca_bundle_path,
    String proxy_url) {
  if (method_rejected(method) || url_rejected(url) || header_lines_rejected(header_lines) ||
      bytes_contain(ca_bundle_path, '\0') || proxy_rejected(proxy_url)) {
    return store_invalid_request();
  }
  SessionSlot* slot = nullptr;
  {
    SessionTable& table = session_table();
    std::lock_guard<std::mutex> table_lock(table.mutex);
    if (session >= 0 && static_cast<std::size_t>(session) < table.slots.size()) {
      slot = table.slots[static_cast<std::size_t>(session)].get();
    }
  }
  if (slot == nullptr) return store_transport_message("session is closed");
  std::lock_guard<std::mutex> session_lock(slot->mutex);
  if (!slot->occupied || slot->curl_handle == nullptr) {
    return store_transport_message("session is closed");
  }
  const bool certificate_changed = slot->certificate_bundle_path.view() != ca_bundle_path.view();
  slot->certificate_bundle_path = ca_bundle_path;
  return perform_on_curl_handle(
      slot->curl_handle,
      certificate_changed,
      method,
      url,
      header_lines,
      body,
      timeout_milliseconds,
      max_response_bytes,
      ca_bundle_path,
      proxy_url);
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
  slot.new_connection_count = 0;
  slot.http_version = 0;
  slot.stream_kind = 0;
  table.live_count -= 1;
  return 0;
}

inline std::int32_t live_result_count() {
  ResultTable& table = result_table();
  std::lock_guard<std::mutex> lock(table.mutex);
  return table.live_count;
}

inline std::int32_t result_new_connection_count(std::int32_t handle) {
  ResultTable& table = result_table();
  std::lock_guard<std::mutex> lock(table.mutex);
  const ResultSlot* slot = live_slot(handle);
  if (slot == nullptr) return -1;
  return slot->new_connection_count;
}

inline std::int32_t result_http_version(std::int32_t handle) {
  ResultTable& table = result_table();
  std::lock_guard<std::mutex> lock(table.mutex);
  const ResultSlot* slot = live_slot(handle);
  if (slot == nullptr) return -1;
  return slot->http_version;
}

inline std::int32_t result_stream_kind(std::int32_t handle) {
  ResultTable& table = result_table();
  std::lock_guard<std::mutex> lock(table.mutex);
  const ResultSlot* slot = live_slot(handle);
  if (slot == nullptr) return -1;
  return slot->stream_kind;
}

inline std::int32_t stream_open(
    String method,
    String url,
    String header_lines,
    String body,
    std::int32_t timeout_milliseconds,
    std::int32_t max_response_bytes,
    String ca_bundle_path,
    String proxy_url) {
  return open_stream_with_token(
      method,
      url,
      header_lines,
      body,
      timeout_milliseconds,
      max_response_bytes,
      ca_bundle_path,
      proxy_url,
      std::stop_token{});
}

inline std::int32_t stream_open_with_stop(
    String method,
    String url,
    String header_lines,
    String body,
    std::int32_t timeout_milliseconds,
    std::int32_t max_response_bytes,
    String ca_bundle_path,
    String proxy_url,
    mlc::concurrency::StopToken stop_token) {
  return open_stream_with_token(
      method,
      url,
      header_lines,
      body,
      timeout_milliseconds,
      max_response_bytes,
      ca_bundle_path,
      proxy_url,
      stop_token.native_token());
}

inline std::int32_t stream_next(std::int32_t stream) {
  StreamSlot* slot = stream_slot(stream);
  if (slot == nullptr) return store_transport_message("stream is closed");
  bool closed = false;
  const StreamEvent event = take_stream_event(slot, &closed);
  if (closed) return store_transport_message("stream is closed");
  return store_stream_event(event);
}

inline std::int32_t stream_close(std::int32_t stream) {
  StreamTable& table = stream_table();
  std::thread worker;
  {
    std::lock_guard<std::mutex> table_lock(table.mutex);
    if (stream < 0 || static_cast<std::size_t>(stream) >= table.slots.size()) return 0;
    StreamSlot* slot = table.slots[static_cast<std::size_t>(stream)].get();
    std::lock_guard<std::mutex> stream_lock(slot->mutex);
    if (!slot->occupied || slot->closing) return 0;
    slot->closing = true;
    slot->stop_requested.store(true);
    slot->space.notify_all();
    slot->ready.notify_all();
    worker = std::move(slot->worker);
  }
  if (worker.joinable()) worker.join();
  {
    std::lock_guard<std::mutex> table_lock(table.mutex);
    StreamSlot* slot = table.slots[static_cast<std::size_t>(stream)].get();
    std::lock_guard<std::mutex> stream_lock(slot->mutex);
    if (slot->occupied) {
      slot->occupied = false;
      slot->events.clear();
      table.live_count -= 1;
    }
    slot->closing = false;
  }
  return 0;
}

inline std::int32_t live_stream_count() {
  StreamTable& table = stream_table();
  std::lock_guard<std::mutex> lock(table.mutex);
  return table.live_count;
}

}  // namespace curl_abi
}  // namespace mlc
