#pragma once

// HTTP serve helpers + ThreadPool dispatch (TRACK_STDLIB_NET_SERVER STEP=7).

#include "mlc/concurrency/thread_pool.hpp"
#include "mlc/net/http_router.hpp"

#include <atomic>
#include <cstddef>
#include <memory>
#include <utility>

namespace mlc {
namespace net {

inline constexpr std::int32_t kHttpReadChunkBytes = 4096;

// Read one HTTP message (headers + optional Content-Length body) from stream.
inline result::Result<String, String> read_http_message(TcpStream& stream) {
  std::string buffer;
  buffer.reserve(1024);
  while (true) {
    if (buffer.size() > kHttpHeaderBlockMaxBytes + 4) {
      return result::err(String("read_http_message: header block exceeds 64 KiB"));
    }
    auto chunk_result = stream.read(kHttpReadChunkBytes);
    if (std::holds_alternative<result::Err<String>>(chunk_result)) {
      return result::err(std::get<result::Err<String>>(chunk_result)._0);
    }
    const String chunk = std::get<result::Ok<String>>(chunk_result)._0;
    if (chunk.is_empty()) {
      if (buffer.empty()) {
        return result::err(String("read_http_message: connection closed"));
      }
      break;
    }
    buffer.append(chunk.view());
    const std::string_view view(buffer);
    const auto header_end = detail::find_header_end(view);
    if (!header_end.has_value()) {
      continue;
    }
    const std::size_t separator = detail::header_separator_length(view, *header_end);
    const std::size_t body_offset = *header_end + separator;
    std::optional<std::size_t> content_length;
    std::size_t line_start = 0;
    const std::string_view head = view.substr(0, *header_end);
    // Skip request line, scan headers for Content-Length.
    const std::size_t first_nl = head.find('\n');
    if (first_nl != std::string_view::npos) {
      line_start = first_nl + 1;
    }
    while (line_start < head.size()) {
      std::size_t line_end = head.find('\n', line_start);
      if (line_end == std::string_view::npos) {
        line_end = head.size();
      }
      std::string_view line = head.substr(line_start, line_end - line_start);
      if (!line.empty() && line.back() == '\r') {
        line.remove_suffix(1);
      }
      line_start = line_end + 1;
      const std::size_t colon = line.find(':');
      if (colon == std::string_view::npos) {
        continue;
      }
      const std::string_view name = detail::trim_spaces(line.substr(0, colon));
      const std::string_view value = detail::trim_spaces(line.substr(colon + 1));
      if (detail::starts_with_ignore_case(name, "Content-Length") && name.size() == 14) {
        std::size_t length = 0;
        if (!detail::parse_content_length(value, length)) {
          return result::err(String("read_http_message: invalid Content-Length"));
        }
        content_length = length;
        break;
      }
    }
    if (!content_length.has_value()) {
      return result::ok(String(std::move(buffer)));
    }
    if (*content_length > kHttpBodyMaxBytes) {
      return result::err(String("read_http_message: body exceeds 1 MiB cap"));
    }
    const std::size_t needed = body_offset + *content_length;
    while (buffer.size() < needed) {
      auto more_result = stream.read(kHttpReadChunkBytes);
      if (std::holds_alternative<result::Err<String>>(more_result)) {
        return result::err(std::get<result::Err<String>>(more_result)._0);
      }
      const String more = std::get<result::Ok<String>>(more_result)._0;
      if (more.is_empty()) {
        return result::err(String("read_http_message: truncated body"));
      }
      buffer.append(more.view());
    }
    if (buffer.size() > needed) {
      buffer.resize(needed);
    }
    return result::ok(String(std::move(buffer)));
  }
  return result::err(String("read_http_message: incomplete headers"));
}

// Handle one connection synchronously: read → parse → route → write → close.
inline void serve_http_connection(TcpStream stream, HttpRouter router) {
  auto raw_result = read_http_message(stream);
  if (std::holds_alternative<result::Err<String>>(raw_result)) {
    HttpResponse bad;
    bad.status = 400;
    bad.body = String("Bad Request");
    (void)write_http_response(stream, bad);
    stream.close();
    return;
  }
  auto parse_result = parse_http_request(std::get<result::Ok<String>>(raw_result)._0);
  if (std::holds_alternative<result::Err<String>>(parse_result)) {
    HttpResponse bad;
    bad.status = 400;
    bad.body = String("Bad Request");
    (void)write_http_response(stream, bad);
    stream.close();
    return;
  }
  const HttpRequest request = std::move(std::get<result::Ok<HttpRequest>>(parse_result)._0);
  const HttpResponse response = router.dispatch(request);
  (void)write_http_response(stream, response);
  stream.close();
}

// Accept loop: each connection is submitted to ThreadPool. Stops after
// `connection_limit` accepts (0 = unlimited until listener close/error) or
// when `stop_flag` is set.
inline void serve_http_with_thread_pool(
    TcpListener& listener,
    HttpRouter router,
    concurrency::ThreadPool& pool,
    std::size_t connection_limit,
    std::atomic<bool>* stop_flag
) {
  std::size_t accepted = 0;
  auto shared_router = std::make_shared<HttpRouter>(std::move(router));
  while (true) {
    if (stop_flag != nullptr && stop_flag->load(std::memory_order_acquire)) {
      break;
    }
    if (connection_limit != 0 && accepted >= connection_limit) {
      break;
    }
    auto stream_result = listener.accept();
    if (std::holds_alternative<result::Err<String>>(stream_result)) {
      break;
    }
    TcpStream stream = std::move(std::get<result::Ok<TcpStream>>(stream_result)._0);
    ++accepted;
    auto shared_stream = std::make_shared<TcpStream>(std::move(stream));
    const bool submitted = pool.submit(
        [shared_stream, shared_router]() {
          serve_http_connection(std::move(*shared_stream), *shared_router);
        }
    );
    if (!submitted) {
      break;
    }
  }
}

} // namespace net
} // namespace mlc
