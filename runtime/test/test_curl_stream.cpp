#include "mlc/concurrency/stop.hpp"
#include "mlc/net/curl_abi.hpp"

#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

namespace {

std::string directory_path() {
  const char* value = std::getenv("HTTPS_STREAM_DIR");
  return value == nullptr ? std::string() : std::string(value);
}

std::string read_text(const std::string& path) {
  std::ifstream input(path);
  std::ostringstream buffer;
  buffer << input.rdbuf();
  std::string text = buffer.str();
  while (!text.empty() && (text.back() == '\n' || text.back() == '\r')) text.pop_back();
  return text;
}

int read_counter(const std::string& name) {
  return std::stoi(read_text(directory_path() + "/" + name));
}

std::string good_url(const std::string& path) {
  return "https://127.0.0.1:" + read_text(directory_path() + "/good_port") + path;
}

std::string silent_url() {
  return "https://127.0.0.1:" + read_text(directory_path() + "/silent_port") + "/silent";
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

class StreamGuard {
 public:
  explicit StreamGuard(std::int32_t stream) : stream_(stream) {}
  ~StreamGuard() { mlc::curl_abi::stream_close(stream_); }
  std::int32_t get() const { return stream_; }

 private:
  std::int32_t stream_;
};

std::int32_t open_stream(const std::string& url, std::int32_t maximum_body_bytes) {
  return mlc::curl_abi::stream_open(
      mlc::String("GET"),
      mlc::String(url.c_str()),
      mlc::String(""),
      mlc::String(""),
      8000,
      maximum_body_bytes,
      mlc::String(authority_path().c_str()),
      mlc::String());
}

int check_event_stream() {
  const std::int32_t opened = open_stream(good_url("/event_stream"), 4194304);
  if (opened < 0) return 21;
  StreamGuard stream(opened);
  bool saw_headers = false;
  std::string body;
  for (int step = 0; step < 32; ++step) {
    ResultHandle event(mlc::curl_abi::stream_next(stream.get()));
    if (event.get() < 0) return 21;
    const std::int32_t kind = mlc::curl_abi::result_stream_kind(event.get());
    if (kind == 1) {
      if (mlc::curl_abi::result_status(event.get()) != 200) return 21;
      saw_headers = true;
    } else if (kind == 2) {
      const mlc::String piece = mlc::curl_abi::result_body(event.get());
      body.append(piece.raw_data(), piece.raw_size());
    } else if (kind == 3) {
      if (!saw_headers) return 21;
      if (body.find("data: one") == std::string::npos) return 21;
      if (body.find("data: two") == std::string::npos) return 21;
      return 0;
    } else {
      std::cerr << "event stream failure " << mlc::curl_abi::result_failure_code(event.get())
                << "\n";
      return 21;
    }
  }
  return 21;
}

int check_body_limit() {
  const std::int32_t opened = open_stream(good_url("/chunked_big"), 1024);
  if (opened < 0) return 22;
  StreamGuard stream(opened);
  for (int step = 0; step < 64; ++step) {
    ResultHandle event(mlc::curl_abi::stream_next(stream.get()));
    if (event.get() < 0) return 22;
    const std::int32_t kind = mlc::curl_abi::result_stream_kind(event.get());
    if (kind == 4) {
      if (mlc::curl_abi::result_failure_code(event.get()) != 6) return 22;
      return 0;
    }
    if (kind == 3) return 22;
  }
  return 22;
}

bool message_is_stop(std::int32_t handle) {
  return mlc::curl_abi::result_failure_code(handle) == 7 &&
         mlc::curl_abi::result_message(handle).view() == "stop requested";
}

std::int32_t perform_with_stop(
    const std::string& url,
    const mlc::concurrency::StopToken& stop_token) {
  return mlc::curl_abi::perform_request_with_stop(
      mlc::String("GET"),
      mlc::String(url.c_str()),
      mlc::String(""),
      mlc::String(""),
      8000,
      4194304,
      mlc::String(authority_path().c_str()),
      mlc::String(),
      stop_token);
}

int check_stop_before_connect() {
  mlc::concurrency::StopSource source;
  source.request();
  const int connections_before = read_counter("connection_count");
  ResultHandle handle(perform_with_stop(good_url("/echo"), source.token()));
  if (handle.get() < 0) return 25;
  if (!message_is_stop(handle.get())) return 25;
  if (read_counter("connection_count") != connections_before) return 25;
  return 0;
}

int check_stop_during_send() {
  mlc::concurrency::StopSource source;
  std::thread requester([&source] {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    source.request();
  });
  const auto started = std::chrono::steady_clock::now();
  ResultHandle handle(perform_with_stop(silent_url(), source.token()));
  const auto elapsed = std::chrono::steady_clock::now() - started;
  requester.join();
  if (elapsed > std::chrono::milliseconds(2000)) return 24;
  if (handle.get() < 0 || !message_is_stop(handle.get())) return 24;
  return 0;
}

int check_stop_during_stream() {
  mlc::concurrency::StopSource source;
  const std::int32_t opened = mlc::curl_abi::stream_open_with_stop(
      mlc::String("GET"),
      mlc::String(silent_url().c_str()),
      mlc::String(""),
      mlc::String(""),
      8000,
      4194304,
      mlc::String(authority_path().c_str()),
      mlc::String(),
      source.token());
  if (opened < 0) return 26;
  StreamGuard stream(opened);
  std::thread requester([&source] {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    source.request();
  });
  const auto started = std::chrono::steady_clock::now();
  ResultHandle event(mlc::curl_abi::stream_next(stream.get()));
  const auto elapsed = std::chrono::steady_clock::now() - started;
  requester.join();
  if (elapsed > std::chrono::milliseconds(2000)) return 26;
  if (event.get() < 0) return 26;
  if (mlc::curl_abi::result_stream_kind(event.get()) != 4) return 26;
  if (!message_is_stop(event.get())) return 26;
  return 0;
}

int check_close_wakes() {
  const std::int32_t opened = open_stream(silent_url(), 4194304);
  if (opened < 0) return 20;
  StreamGuard stream(opened);
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  const auto started = std::chrono::steady_clock::now();
  mlc::curl_abi::stream_close(stream.get());
  const auto elapsed = std::chrono::steady_clock::now() - started;
  if (elapsed > std::chrono::milliseconds(2000)) return 20;
  if (mlc::curl_abi::live_stream_count() != 0) return 20;
  return 0;
}

}  // namespace

int main() {
  if (directory_path().empty()) {
    std::cerr << "curl stream check missing directory\n";
    return 1;
  }
  const int checks[] = {
      check_event_stream(),
      check_body_limit(),
      check_close_wakes(),
      check_stop_before_connect(),
      check_stop_during_send(),
      check_stop_during_stream(),
  };
  for (int code : checks) {
    if (code != 0) {
      std::cerr << "curl stream check " << code << "\n";
      return code;
    }
  }
  if (mlc::curl_abi::live_stream_count() != 0) return 23;
  if (mlc::curl_abi::live_result_count() != 0) return 23;
  return 0;
}
