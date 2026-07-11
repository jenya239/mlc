#pragma once

// Temporary HTTP parse helpers for WebSocket upgrade handshake only
// (TRACK_STDLIB_HTTP_MLC STEP=5). Public C++ HTTP server/router removed;
// MLC path is lib/.../http_server.mlc. Delete with TRACK_STDLIB_WEBSOCKET_TO_MLC.

#include "mlc/core/result.hpp"
#include "mlc/core/string.hpp"
#include "mlc/net/tcp_bridge.hpp"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

namespace mlc {
namespace net {

inline constexpr std::size_t kHttpHeaderBlockMaxBytes = 64u * 1024u;
inline constexpr std::size_t kHttpBodyMaxBytes = 1024u * 1024u;

struct HttpHeader {
  String name;
  String value;
};

struct HttpRequest {
  String method;
  String path;
  std::vector<HttpHeader> headers;
  String body;
};

namespace detail {

inline std::string_view view_of(const String& text) {
  return text.view();
}

inline bool starts_with_ignore_case(std::string_view text, std::string_view prefix) {
  if (text.size() < prefix.size()) {
    return false;
  }
  for (std::size_t index = 0; index < prefix.size(); ++index) {
    const unsigned char left = static_cast<unsigned char>(text[index]);
    const unsigned char right = static_cast<unsigned char>(prefix[index]);
    if (std::tolower(left) != std::tolower(right)) {
      return false;
    }
  }
  return true;
}

inline std::string_view trim_spaces(std::string_view text) {
  while (!text.empty() && (text.front() == ' ' || text.front() == '\t')) {
    text.remove_prefix(1);
  }
  while (!text.empty() && (text.back() == ' ' || text.back() == '\t')) {
    text.remove_suffix(1);
  }
  return text;
}

inline std::optional<std::size_t> find_header_end(std::string_view raw) {
  const std::size_t crlf = raw.find("\r\n\r\n");
  if (crlf != std::string_view::npos) {
    return crlf;
  }
  const std::size_t lf = raw.find("\n\n");
  if (lf != std::string_view::npos) {
    return lf;
  }
  return std::nullopt;
}

inline std::size_t header_separator_length(std::string_view raw, std::size_t header_end) {
  if (header_end + 4 <= raw.size() && raw.substr(header_end, 4) == "\r\n\r\n") {
    return 4;
  }
  return 2;
}

inline bool parse_content_length(std::string_view value, std::size_t& out_length) {
  value = trim_spaces(value);
  if (value.empty()) {
    return false;
  }
  std::size_t length = 0;
  for (char character : value) {
    if (character < '0' || character > '9') {
      return false;
    }
    const std::size_t digit = static_cast<std::size_t>(character - '0');
    if (length > (static_cast<std::size_t>(-1) - digit) / 10u) {
      return false;
    }
    length = length * 10u + digit;
  }
  out_length = length;
  return true;
}

} // namespace detail

inline result::Result<HttpRequest, String> parse_http_request(const String& raw_text) {
  const std::string_view raw = detail::view_of(raw_text);
  const auto header_end_opt = detail::find_header_end(raw);
  if (!header_end_opt.has_value()) {
    return result::err(String("HttpRequest.parse: missing header terminator"));
  }
  const std::size_t header_end = *header_end_opt;
  if (header_end > kHttpHeaderBlockMaxBytes) {
    return result::err(String("HttpRequest.parse: header block exceeds 64 KiB"));
  }

  const std::size_t separator_length = detail::header_separator_length(raw, header_end);
  const std::string_view head = raw.substr(0, header_end);

  std::size_t line_start = 0;
  std::size_t first_line_end = head.find('\n');
  if (first_line_end == std::string_view::npos) {
    return result::err(String("HttpRequest.parse: missing request line"));
  }
  std::string_view request_line = head.substr(0, first_line_end);
  if (!request_line.empty() && request_line.back() == '\r') {
    request_line.remove_suffix(1);
  }
  line_start = first_line_end + 1;

  const std::size_t first_space = request_line.find(' ');
  if (first_space == std::string_view::npos) {
    return result::err(String("HttpRequest.parse: malformed request line"));
  }
  const std::size_t second_space = request_line.find(' ', first_space + 1);
  if (second_space == std::string_view::npos) {
    return result::err(String("HttpRequest.parse: malformed request line"));
  }
  const std::string_view method = request_line.substr(0, first_space);
  const std::string_view path = request_line.substr(first_space + 1, second_space - first_space - 1);
  const std::string_view version = request_line.substr(second_space + 1);
  if (method.empty() || path.empty()) {
    return result::err(String("HttpRequest.parse: empty method or path"));
  }
  if (!detail::starts_with_ignore_case(version, "HTTP/1.")) {
    return result::err(String("HttpRequest.parse: unsupported HTTP version"));
  }

  HttpRequest request;
  request.method = String(std::string(method));
  request.path = String(std::string(path));

  std::optional<std::size_t> content_length;
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
    if (line.empty()) {
      continue;
    }
    const std::size_t colon = line.find(':');
    if (colon == std::string_view::npos) {
      return result::err(String("HttpRequest.parse: malformed header line"));
    }
    const std::string_view name = detail::trim_spaces(line.substr(0, colon));
    const std::string_view value = detail::trim_spaces(line.substr(colon + 1));
    if (name.empty()) {
      return result::err(String("HttpRequest.parse: empty header name"));
    }
    if (detail::starts_with_ignore_case(name, "Content-Length") && name.size() == 14) {
      std::size_t length = 0;
      if (!detail::parse_content_length(value, length)) {
        return result::err(String("HttpRequest.parse: invalid Content-Length"));
      }
      content_length = length;
    }
    request.headers.push_back(HttpHeader{
        String(std::string(name)),
        String(std::string(value))
    });
  }

  const std::size_t body_offset = header_end + separator_length;
  const std::string_view remainder =
      body_offset <= raw.size() ? raw.substr(body_offset) : std::string_view{};

  if (content_length.has_value()) {
    if (*content_length > kHttpBodyMaxBytes) {
      return result::err(String("HttpRequest.parse: body exceeds 1 MiB cap"));
    }
    if (remainder.size() < *content_length) {
      return result::err(String("HttpRequest.parse: truncated body"));
    }
    request.body = String(std::string(remainder.substr(0, *content_length)));
  } else {
    request.body = String();
  }

  return result::ok(std::move(request));
}

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

} // namespace net
} // namespace mlc
