#include "mlc/net/curl_abi.hpp"

#include <chrono>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

namespace {

constexpr std::int32_t kInvalidRequest = 1;
constexpr std::int32_t kTimedOut = 4;
constexpr std::int32_t kCertificateRejected = 5;
constexpr std::int32_t kResponseTooLarge = 6;

std::string directory_path() {
  const char* value = std::getenv("HTTPS_CURL_ABI_DIR");
  return value == nullptr ? std::string() : std::string(value);
}

std::string read_text(const std::string& path) {
  std::ifstream input(path);
  std::ostringstream buffer;
  buffer << input.rdbuf();
  std::string text = buffer.str();
  while (!text.empty() && (text.back() == '\n' || text.back() == '\r')) {
    text.pop_back();
  }
  return text;
}

int read_counter(const std::string& name) {
  return std::stoi(read_text(directory_path() + "/" + name));
}

std::string good_url(const std::string& path) {
  return "https://127.0.0.1:" + read_text(directory_path() + "/good_port") + path;
}

std::string wrong_url(const std::string& path) {
  return "https://127.0.0.1:" + read_text(directory_path() + "/wrong_port") + path;
}

std::string silent_url() {
  return "https://127.0.0.1:" + read_text(directory_path() + "/silent_port") + "/silent";
}

std::string http2_url() {
  return "https://127.0.0.1:" + read_text(directory_path() + "/http2_port") + "/http2";
}

std::string authority_path() {
  return read_text(directory_path() + "/certificate_authority_path");
}

class ResultHandle {
 public:
  explicit ResultHandle(std::int32_t handle) : handle_(handle) {}
  ~ResultHandle() { mlc::curl_abi::release_result(handle_); }
  std::int32_t get() const { return handle_; }

 private:
  std::int32_t handle_;
};

ResultHandle perform(
    const std::string& method,
    const mlc::String& url,
    const mlc::String& header_lines,
    const mlc::String& body,
    std::int32_t timeout_milliseconds,
    std::int32_t max_response_bytes,
    const std::string& certificate_bundle_path,
    const std::string& proxy_url = "") {
  return ResultHandle(mlc::curl_abi::perform_request(
      mlc::String(method.c_str()),
      url,
      header_lines,
      body,
      timeout_milliseconds,
      max_response_bytes,
      mlc::String(certificate_bundle_path.c_str()),
      mlc::String(proxy_url.c_str())));
}

ResultHandle perform_text(
    const std::string& method,
    const std::string& url,
    const std::string& header_lines,
    const std::string& body,
    std::int32_t timeout_milliseconds,
    std::int32_t max_response_bytes,
    const std::string& certificate_bundle_path,
    const std::string& proxy_url = "") {
  return perform(
      method,
      mlc::String(url.c_str()),
      mlc::String(header_lines.c_str()),
      mlc::String(body.c_str()),
      timeout_milliseconds,
      max_response_bytes,
      certificate_bundle_path,
      proxy_url);
}

bool contains_text(const mlc::String& text, const char* needle) {
  return text.view().find(needle) != std::string_view::npos;
}

int check_success() {
  ResultHandle handle = perform_text("GET", good_url("/echo"), "", "", 5000, 4194304, authority_path());
  if (handle.get() < 0) return 1;
  if (mlc::curl_abi::result_failure_code(handle.get()) != 0) return 1;
  if (mlc::curl_abi::result_status(handle.get()) != 200) return 1;
  if (mlc::curl_abi::result_body(handle.get()).raw_size() != 0) return 1;
  if (!contains_text(mlc::curl_abi::result_header_block(handle.get()), "Content-Type")) return 1;
  return 0;
}

int check_put_body() {
  ResultHandle handle = perform_text("PUT", good_url("/echo"), "", "put-body", 5000, 4194304, authority_path());
  if (handle.get() < 0) return 15;
  if (mlc::curl_abi::result_failure_code(handle.get()) != 0) return 15;
  if (mlc::curl_abi::result_status(handle.get()) != 200) return 15;
  if (mlc::curl_abi::result_body(handle.get()).view() != "put-body") return 15;
  return 0;
}

int check_head() {
  ResultHandle handle = perform_text("HEAD", good_url("/echo"), "", "", 5000, 4194304, authority_path());
  if (handle.get() < 0) return 16;
  if (mlc::curl_abi::result_failure_code(handle.get()) != 0) return 16;
  if (mlc::curl_abi::result_status(handle.get()) != 200) return 16;
  if (mlc::curl_abi::result_body(handle.get()).raw_size() != 0) return 16;
  return 0;
}

int check_post_body() {
  const char payload[] = {'a', '\0', 'b', 'c'};
  ResultHandle handle = perform(
      "POST",
      mlc::String(good_url("/echo").c_str()),
      mlc::String(""),
      mlc::String(payload, sizeof(payload)),
      5000,
      4194304,
      authority_path());
  if (handle.get() < 0) return 2;
  if (mlc::curl_abi::result_failure_code(handle.get()) != 0) return 2;
  const mlc::String body = mlc::curl_abi::result_body(handle.get());
  if (body.raw_size() != sizeof(payload)) return 2;
  if (std::memcmp(body.raw_data(), payload, sizeof(payload)) != 0) return 2;
  return 0;
}

int check_binary_body() {
  const char payload[] = {'a', '\0', 'b', '\0', 'c'};
  ResultHandle handle = perform_text("GET", good_url("/binary"), "", "", 5000, 4194304, authority_path());
  if (handle.get() < 0) return 3;
  if (mlc::curl_abi::result_failure_code(handle.get()) != 0) return 3;
  const mlc::String body = mlc::curl_abi::result_body(handle.get());
  if (body.raw_size() != sizeof(payload)) return 3;
  if (std::memcmp(body.raw_data(), payload, sizeof(payload)) != 0) return 3;
  return 0;
}

int check_status_not_found() {
  ResultHandle handle = perform_text("GET", good_url("/status/404"), "", "", 5000, 4194304, authority_path());
  if (handle.get() < 0) return 4;
  if (mlc::curl_abi::result_failure_code(handle.get()) != 0) return 4;
  if (mlc::curl_abi::result_status(handle.get()) != 404) return 4;
  return 0;
}

int check_missing_authority() {
  const int requests_before = read_counter("request_count");
  ResultHandle handle = perform_text("GET", good_url("/echo"), "", "", 5000, 4194304, "");
  if (handle.get() < 0) return 5;
  if (mlc::curl_abi::result_failure_code(handle.get()) != kCertificateRejected) return 5;
  if (read_counter("request_count") != requests_before) return 5;
  return 0;
}

int check_wrong_host() {
  ResultHandle handle = perform_text("GET", wrong_url("/echo"), "", "", 5000, 4194304, authority_path());
  if (handle.get() < 0) return 6;
  if (mlc::curl_abi::result_failure_code(handle.get()) != kCertificateRejected) return 6;
  return 0;
}

int check_http_scheme() {
  const int connections_before = read_counter("connection_count");
  const std::string url = "http://127.0.0.1:" + read_text(directory_path() + "/good_port") + "/echo";
  ResultHandle handle = perform_text("GET", url, "", "", 5000, 4194304, authority_path());
  if (handle.get() < 0) return 7;
  if (mlc::curl_abi::result_failure_code(handle.get()) != kInvalidRequest) return 7;
  if (read_counter("connection_count") != connections_before) return 7;
  return 0;
}

ResultHandle perform_raw_url(const std::string& url_bytes, const std::string& header_lines) {
  return perform(
      "GET",
      mlc::String(url_bytes.data(), url_bytes.size()),
      mlc::String(header_lines.c_str()),
      mlc::String(""),
      5000,
      4194304,
      authority_path());
}

int expect_invalid_without_connection(const ResultHandle& handle, int connections_before) {
  if (handle.get() < 0) return 8;
  if (mlc::curl_abi::result_failure_code(handle.get()) != kInvalidRequest) return 8;
  if (read_counter("connection_count") != connections_before) return 8;
  return 0;
}

int check_injection() {
  const std::string base = good_url("/echo");
  {
    const int connections_before = read_counter("connection_count");
    ResultHandle handle = perform_raw_url(base + "\r", "");
    if (expect_invalid_without_connection(handle, connections_before) != 0) return 8;
  }
  {
    const int connections_before = read_counter("connection_count");
    ResultHandle handle = perform_raw_url(base + "\n", "");
    if (expect_invalid_without_connection(handle, connections_before) != 0) return 8;
  }
  {
    const int connections_before = read_counter("connection_count");
    std::string url_bytes = base;
    url_bytes.push_back('\0');
    url_bytes.push_back('x');
    ResultHandle handle = perform_raw_url(url_bytes, "");
    if (expect_invalid_without_connection(handle, connections_before) != 0) return 8;
  }
  {
    const int connections_before = read_counter("connection_count");
    ResultHandle handle = perform_text("GET", base, "Bad\rName: value\n", "", 5000, 4194304, authority_path());
    if (expect_invalid_without_connection(handle, connections_before) != 0) return 8;
  }
  {
    const int connections_before = read_counter("connection_count");
    ResultHandle handle = perform_text("GET", base, "Name: bad\rvalue\n", "", 5000, 4194304, authority_path());
    if (expect_invalid_without_connection(handle, connections_before) != 0) return 8;
  }
  {
    const int connections_before = read_counter("connection_count");
    ResultHandle handle = perform_text("GET", base, "Name: bad\nvalue\n", "", 5000, 4194304, authority_path());
    if (expect_invalid_without_connection(handle, connections_before) != 0) return 8;
  }
  {
    const int connections_before = read_counter("connection_count");
    std::string header_lines = "Name: value";
    header_lines.push_back('\0');
    header_lines += "\n";
    ResultHandle handle = perform(
        "GET",
        mlc::String(base.c_str()),
        mlc::String(header_lines.data(), header_lines.size()),
        mlc::String(""),
        5000,
        4194304,
        authority_path());
    if (expect_invalid_without_connection(handle, connections_before) != 0) return 8;
  }
  return 0;
}

int check_timeout() {
  const auto started = std::chrono::steady_clock::now();
  ResultHandle handle = perform_text("GET", silent_url(), "", "", 500, 4194304, authority_path());
  const auto elapsed = std::chrono::steady_clock::now() - started;
  if (handle.get() < 0) return 9;
  if (mlc::curl_abi::result_failure_code(handle.get()) != kTimedOut) return 9;
  if (elapsed >= std::chrono::seconds(3)) return 9;
  return 0;
}

int check_body_limit() {
  ResultHandle limited = perform_text("GET", good_url("/big"), "", "", 5000, 65536, authority_path());
  if (limited.get() < 0) return 10;
  if (mlc::curl_abi::result_failure_code(limited.get()) != kResponseTooLarge) return 10;
  if (mlc::curl_abi::result_body(limited.get()).raw_size() > 65536) return 10;
  ResultHandle chunked = perform_text("GET", good_url("/chunked_big"), "", "", 5000, 65536, authority_path());
  if (chunked.get() < 0) return 10;
  if (mlc::curl_abi::result_failure_code(chunked.get()) != kResponseTooLarge) return 10;
  if (mlc::curl_abi::result_body(chunked.get()).raw_size() > 65536) return 10;
  return 0;
}

int check_decoded_body_limit() {
  ResultHandle decoded = perform_text("GET", good_url("/compressed"), "", "", 5000, 16, authority_path());
  if (decoded.get() < 0) return 28;
  if (mlc::curl_abi::result_failure_code(decoded.get()) != 0) return 28;
  if (mlc::curl_abi::result_status(decoded.get()) != 200) return 28;
  if (mlc::curl_abi::result_body(decoded.get()).view() != "hello-compressed") return 28;

  ResultHandle too_small = perform_text("GET", good_url("/compressed"), "", "", 5000, 15, authority_path());
  if (too_small.get() < 0) return 28;
  if (mlc::curl_abi::result_failure_code(too_small.get()) != kResponseTooLarge) return 28;
  if (mlc::curl_abi::result_body(too_small.get()).raw_size() > 15) return 28;

  ResultHandle expanded = perform_text(
      "GET", good_url("/compressed_large"), "", "", 5000, 1000, authority_path());
  if (expanded.get() < 0) return 28;
  if (mlc::curl_abi::result_failure_code(expanded.get()) != kResponseTooLarge) return 28;
  if (mlc::curl_abi::result_body(expanded.get()).raw_size() > 1000) return 28;

  ResultHandle full = perform_text(
      "GET", good_url("/compressed_large"), "", "", 5000, 200000, authority_path());
  if (full.get() < 0) return 28;
  if (mlc::curl_abi::result_failure_code(full.get()) != 0) return 28;
  const mlc::String body = mlc::curl_abi::result_body(full.get());
  if (body.raw_size() != 200000) return 28;
  if (body.raw_data()[0] != 'Z' || body.raw_data()[199999] != 'Z') return 28;
  return 0;
}

int check_http2() {
  ResultHandle handle = perform_text("GET", http2_url(), "", "", 5000, 4194304, authority_path());
  if (handle.get() < 0) return 29;
  if (mlc::curl_abi::result_failure_code(handle.get()) != 0) {
    std::cerr << "http2 failure " << mlc::curl_abi::result_failure_code(handle.get()) << " "
              << mlc::curl_abi::result_message(handle.get()).view() << "\n";
    return 29;
  }
  if (mlc::curl_abi::result_status(handle.get()) != 200) return 29;
  if (mlc::curl_abi::result_body(handle.get()).view() != "http2-ok") return 29;
  if (mlc::curl_abi::result_http_version(handle.get()) != CURL_HTTP_VERSION_2_0) return 29;
  return 0;
}

int check_proxy_ignored() {
  ResultHandle handle = perform_text("GET", good_url("/echo"), "", "", 5000, 4194304, authority_path());
  if (handle.get() < 0) return 11;
  if (mlc::curl_abi::result_failure_code(handle.get()) != 0) return 11;
  if (mlc::curl_abi::result_status(handle.get()) != 200) return 11;
  return 0;
}

std::string explicit_proxy_url() {
  return "http://127.0.0.1:" + read_text(directory_path() + "/proxy_port");
}

int check_explicit_proxy() {
  {
    const int connections_before = read_counter("connection_count");
    ResultHandle handle = perform_text(
        "GET", good_url("/echo"), "", "", 5000, 4194304, authority_path(), "not-a-proxy");
    if (expect_invalid_without_connection(handle, connections_before) != 0) return 19;
  }
  {
    const int connections_before = read_counter("connection_count");
    const int proxy_before = read_counter("proxy_connect_count");
    ResultHandle handle = perform_text(
        "GET",
        good_url("/echo"),
        "",
        "",
        5000,
        4194304,
        authority_path(),
        "http://127.0.0.1:9");
    if (handle.get() < 0) return 19;
    if (mlc::curl_abi::result_failure_code(handle.get()) != 3) return 19;
    if (read_counter("connection_count") != connections_before) return 19;
    if (read_counter("proxy_connect_count") != proxy_before) return 19;
  }
  {
    const int proxy_before = read_counter("proxy_connect_count");
    ResultHandle handle = perform_text(
        "GET", good_url("/echo"), "", "", 5000, 4194304, authority_path(), explicit_proxy_url());
    if (handle.get() < 0) return 19;
    if (mlc::curl_abi::result_failure_code(handle.get()) != 0) return 19;
    if (mlc::curl_abi::result_status(handle.get()) != 200) return 19;
    if (read_counter("proxy_connect_count") != proxy_before + 1) return 19;
  }
  return 0;
}

int check_secret_not_in_message() {
  ResultHandle handle = perform_text(
      "GET",
      good_url("/echo"),
      "Authorization: Bearer SENTINEL\n",
      "",
      5000,
      4194304,
      authority_path());
  if (handle.get() < 0) return 12;
  if (mlc::curl_abi::result_failure_code(handle.get()) != 0) return 12;
  if (contains_text(mlc::curl_abi::result_message(handle.get()), "SENTINEL")) return 12;
  return 0;
}

int check_lifecycle() {
  ResultHandle handle = perform_text("GET", good_url("/echo"), "", "", 5000, 4194304, authority_path());
  if (handle.get() < 0) return 13;
  if (mlc::curl_abi::result_status(handle.get()) != 200) return 13;
  const std::int32_t stored = handle.get();
  mlc::curl_abi::release_result(stored);
  if (mlc::curl_abi::result_status(stored) != -1) return 13;
  if (mlc::curl_abi::result_failure_code(stored) != -1) return 13;
  if (mlc::curl_abi::result_message(stored).raw_size() != 0) return 13;
  if (mlc::curl_abi::result_header_block(stored).raw_size() != 0) return 13;
  if (mlc::curl_abi::result_body(stored).raw_size() != 0) return 13;
  mlc::curl_abi::release_result(stored);
  if (mlc::curl_abi::live_result_count() != 0) return 13;
  return 0;
}

class SessionGuard {
 public:
  explicit SessionGuard(std::int32_t session) : session_(session) {}
  ~SessionGuard() { mlc::curl_abi::session_close(session_); }
  std::int32_t get() const { return session_; }

 private:
  std::int32_t session_;
};

int check_session_reuses_connection() {
  const std::int32_t opened = mlc::curl_abi::session_open();
  if (opened < 0) return 18;
  SessionGuard session(opened);
  const int connections_before = read_counter("connection_count");
  ResultHandle first(mlc::curl_abi::session_perform(
      session.get(),
      mlc::String("POST"),
      mlc::String(good_url("/session_echo").c_str()),
      mlc::String(""),
      mlc::String("one"),
      5000,
      4194304,
      mlc::String(authority_path().c_str()),
      mlc::String()));
  if (first.get() < 0) return 18;
  if (mlc::curl_abi::result_failure_code(first.get()) != 0) return 18;
  if (mlc::curl_abi::result_status(first.get()) != 200) return 18;
  if (mlc::curl_abi::result_body(first.get()).view() != "one") return 18;
  if (mlc::curl_abi::result_new_connection_count(first.get()) != 1) return 18;
  ResultHandle second(mlc::curl_abi::session_perform(
      session.get(),
      mlc::String("POST"),
      mlc::String(good_url("/session_echo").c_str()),
      mlc::String(""),
      mlc::String("two"),
      5000,
      4194304,
      mlc::String(authority_path().c_str()),
      mlc::String()));
  if (second.get() < 0) return 18;
  if (mlc::curl_abi::result_failure_code(second.get()) != 0) return 18;
  if (mlc::curl_abi::result_status(second.get()) != 200) return 18;
  if (mlc::curl_abi::result_body(second.get()).view() != "two") return 18;
  if (mlc::curl_abi::result_new_connection_count(second.get()) != 0) {
    std::cerr << "session second new_connection_count "
              << mlc::curl_abi::result_new_connection_count(second.get())
              << " connection_count " << read_counter("connection_count") << "\n";
    return 18;
  }
  if (read_counter("connection_count") != connections_before + 1) {
    std::cerr << "session connection_count " << read_counter("connection_count")
              << " before " << connections_before << "\n";
    return 18;
  }
  return 0;
}

int check_threads() {
  constexpr int kThreadCount = 8;
  constexpr int kRequestsPerThread = 20;
  std::vector<int> failures(static_cast<std::size_t>(kThreadCount), 0);
  std::vector<std::thread> threads;
  threads.reserve(static_cast<std::size_t>(kThreadCount));
  for (int thread_index = 0; thread_index < kThreadCount; ++thread_index) {
    threads.emplace_back([thread_index, &failures] {
      for (int request_index = 0; request_index < kRequestsPerThread; ++request_index) {
        ResultHandle handle = perform_text("GET", good_url("/echo"), "", "", 5000, 4194304, authority_path());
        if (handle.get() < 0 || mlc::curl_abi::result_failure_code(handle.get()) != 0) {
          failures[static_cast<std::size_t>(thread_index)] = 1;
          return;
        }
      }
    });
  }
  for (std::thread& thread : threads) thread.join();
  for (int failure : failures) {
    if (failure != 0) return 14;
  }
  if (mlc::curl_abi::live_result_count() != 0) return 14;
  return 0;
}

}  // namespace

int main() {
  if (directory_path().empty()) {
    std::cerr << "curl abi check missing directory\n";
    return 1;
  }
  const int checks[] = {
      check_success(),
      check_post_body(),
      check_put_body(),
      check_head(),
      check_binary_body(),
      check_status_not_found(),
      check_missing_authority(),
      check_wrong_host(),
      check_http_scheme(),
      check_injection(),
      check_timeout(),
      check_body_limit(),
      check_decoded_body_limit(),
      check_http2(),
      check_proxy_ignored(),
      check_explicit_proxy(),
      check_secret_not_in_message(),
      check_lifecycle(),
      check_session_reuses_connection(),
      check_threads(),
  };
  for (int code : checks) {
    if (code != 0) {
      std::cerr << "curl abi check " << code << "\n";
      return code;
    }
  }
  return 0;
}
