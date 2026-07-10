#pragma once

// Server-side WebSocket (RFC 6455) over TcpStream (TRACK_STDLIB_WEBSOCKET STEP=2).
// Text frames only; client→server masked; server→client unmasked.
// SHA1 + base64 local (no libsodium). Errors: nullopt/false + last_error().

#include "mlc/net/http_request.hpp"
#include "mlc/net/http_server.hpp"
#include "mlc/net/tcp.hpp"

#include <array>
#include <cstdint>
#include <cstring>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace mlc {
namespace net {

inline constexpr std::size_t kWebSocketMaxPayloadBytes = kHttpBodyMaxBytes;
inline constexpr const char* kWebSocketGuid = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

namespace websocket_detail {

inline String& error_slot() {
  static thread_local String last_error;
  return last_error;
}

inline void set_error(const String& message) {
  error_slot() = message;
}

inline void set_error(const char* message) {
  error_slot() = String(message);
}

// Compact SHA-1 (public-domain style).
inline void sha1(const std::uint8_t* message, std::size_t length, std::uint8_t digest[20]) {
  std::uint32_t hash[5] = {
      0x67452301u, 0xEFCDAB89u, 0x98BADCFEu, 0x10325476u, 0xC3D2E1F0u};
  std::vector<std::uint8_t> padded(message, message + length);
  padded.push_back(0x80u);
  while ((padded.size() % 64u) != 56u) {
    padded.push_back(0u);
  }
  const std::uint64_t bit_length = static_cast<std::uint64_t>(length) * 8u;
  for (int shift = 56; shift >= 0; shift -= 8) {
    padded.push_back(static_cast<std::uint8_t>((bit_length >> shift) & 0xFFu));
  }

  auto left_rotate = [](std::uint32_t value, std::uint32_t bits) -> std::uint32_t {
    return (value << bits) | (value >> (32u - bits));
  };

  for (std::size_t chunk = 0; chunk < padded.size(); chunk += 64u) {
    std::uint32_t words[80];
    for (std::size_t index = 0; index < 16u; ++index) {
      const std::size_t offset = chunk + index * 4u;
      words[index] = (static_cast<std::uint32_t>(padded[offset]) << 24) |
                     (static_cast<std::uint32_t>(padded[offset + 1]) << 16) |
                     (static_cast<std::uint32_t>(padded[offset + 2]) << 8) |
                     static_cast<std::uint32_t>(padded[offset + 3]);
    }
    for (std::size_t index = 16; index < 80u; ++index) {
      words[index] = left_rotate(
          words[index - 3] ^ words[index - 8] ^ words[index - 14] ^ words[index - 16],
          1u);
    }
    std::uint32_t a = hash[0];
    std::uint32_t b = hash[1];
    std::uint32_t c = hash[2];
    std::uint32_t d = hash[3];
    std::uint32_t e = hash[4];
    for (std::size_t index = 0; index < 80u; ++index) {
      std::uint32_t f = 0;
      std::uint32_t k = 0;
      if (index < 20u) {
        f = (b & c) | ((~b) & d);
        k = 0x5A827999u;
      } else if (index < 40u) {
        f = b ^ c ^ d;
        k = 0x6ED9EBA1u;
      } else if (index < 60u) {
        f = (b & c) | (b & d) | (c & d);
        k = 0x8F1BBCDCu;
      } else {
        f = b ^ c ^ d;
        k = 0xCA62C1D6u;
      }
      const std::uint32_t temp = left_rotate(a, 5u) + f + e + k + words[index];
      e = d;
      d = c;
      c = left_rotate(b, 30u);
      b = a;
      a = temp;
    }
    hash[0] += a;
    hash[1] += b;
    hash[2] += c;
    hash[3] += d;
    hash[4] += e;
  }
  for (std::size_t index = 0; index < 5u; ++index) {
    digest[index * 4u] = static_cast<std::uint8_t>((hash[index] >> 24) & 0xFFu);
    digest[index * 4u + 1] = static_cast<std::uint8_t>((hash[index] >> 16) & 0xFFu);
    digest[index * 4u + 2] = static_cast<std::uint8_t>((hash[index] >> 8) & 0xFFu);
    digest[index * 4u + 3] = static_cast<std::uint8_t>(hash[index] & 0xFFu);
  }
}

inline String base64_encode(const std::uint8_t* data, std::size_t length) {
  static constexpr char alphabet[] =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  std::string output;
  output.reserve(((length + 2u) / 3u) * 4u);
  for (std::size_t index = 0; index < length; index += 3u) {
    const std::uint32_t byte0 = data[index];
    const std::uint32_t byte1 = (index + 1u < length) ? data[index + 1u] : 0u;
    const std::uint32_t byte2 = (index + 2u < length) ? data[index + 2u] : 0u;
    const std::uint32_t triple = (byte0 << 16) | (byte1 << 8) | byte2;
    output.push_back(alphabet[(triple >> 18) & 0x3Fu]);
    output.push_back(alphabet[(triple >> 12) & 0x3Fu]);
    output.push_back((index + 1u < length) ? alphabet[(triple >> 6) & 0x3Fu] : '=');
    output.push_back((index + 2u < length) ? alphabet[triple & 0x3Fu] : '=');
  }
  return String(std::move(output));
}

inline String compute_accept_key(const String& client_key) {
  const std::string material = std::string(client_key.view()) + kWebSocketGuid;
  std::uint8_t digest[20];
  sha1(reinterpret_cast<const std::uint8_t*>(material.data()), material.size(), digest);
  return base64_encode(digest, 20);
}

inline std::optional<String> header_value(const HttpRequest& request, std::string_view name) {
  for (const HttpHeader& header : request.headers) {
    if (detail::starts_with_ignore_case(header.name.view(), name) &&
        header.name.view().size() == name.size()) {
      return header.value;
    }
  }
  return std::nullopt;
}

inline bool header_token_has(const String& value, std::string_view token) {
  std::string_view view = value.view();
  while (!view.empty()) {
    const std::size_t comma = view.find(',');
    std::string_view part = (comma == std::string_view::npos) ? view : view.substr(0, comma);
    part = detail::trim_spaces(part);
    if (detail::starts_with_ignore_case(part, token) && part.size() == token.size()) {
      return true;
    }
    if (comma == std::string_view::npos) {
      break;
    }
    view.remove_prefix(comma + 1);
  }
  return false;
}

inline bool validate_upgrade_request(const HttpRequest& request, String& client_key_out) {
  if (request.method.view() != "GET") {
    set_error("websocket upgrade: method must be GET");
    return false;
  }
  const auto upgrade = header_value(request, "Upgrade");
  if (!upgrade.has_value() ||
      !detail::starts_with_ignore_case(upgrade->view(), "websocket") ||
      upgrade->view().size() != 9) {
    set_error("websocket upgrade: missing Upgrade: websocket");
    return false;
  }
  const auto connection = header_value(request, "Connection");
  if (!connection.has_value() || !header_token_has(*connection, "Upgrade")) {
    set_error("websocket upgrade: Connection must include Upgrade");
    return false;
  }
  const auto version = header_value(request, "Sec-WebSocket-Version");
  if (!version.has_value() || version->view() != "13") {
    set_error("websocket upgrade: Sec-WebSocket-Version must be 13");
    return false;
  }
  const auto key = header_value(request, "Sec-WebSocket-Key");
  if (!key.has_value() || key->is_empty()) {
    set_error("websocket upgrade: missing Sec-WebSocket-Key");
    return false;
  }
  client_key_out = *key;
  return true;
}

inline String encode_unmasked_frame(std::uint8_t opcode, const std::string_view payload) {
  std::string frame;
  frame.push_back(static_cast<char>(0x80u | (opcode & 0x0Fu)));
  const std::size_t length = payload.size();
  if (length <= 125u) {
    frame.push_back(static_cast<char>(length));
  } else if (length <= 0xFFFFu) {
    frame.push_back(126);
    frame.push_back(static_cast<char>((length >> 8) & 0xFFu));
    frame.push_back(static_cast<char>(length & 0xFFu));
  } else {
    frame.push_back(127);
    for (int shift = 56; shift >= 0; shift -= 8) {
      frame.push_back(static_cast<char>((static_cast<std::uint64_t>(length) >> shift) & 0xFFu));
    }
  }
  frame.append(payload.data(), payload.size());
  return String(std::move(frame));
}

struct DecodedFrame {
  std::uint8_t opcode = 0;
  std::string payload;
  bool fin = true;
};

inline std::optional<DecodedFrame> try_decode_frame(std::string& buffer) {
  if (buffer.size() < 2u) {
    return std::nullopt;
  }
  const std::uint8_t first = static_cast<std::uint8_t>(buffer[0]);
  const std::uint8_t second = static_cast<std::uint8_t>(buffer[1]);
  const bool fin = (first & 0x80u) != 0;
  const std::uint8_t opcode = first & 0x0Fu;
  const bool masked = (second & 0x80u) != 0;
  std::uint64_t payload_length = second & 0x7Fu;
  std::size_t header_size = 2u;
  if (payload_length == 126u) {
    if (buffer.size() < 4u) {
      return std::nullopt;
    }
    payload_length = (static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[2])) << 8) |
                     static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[3]));
    header_size = 4u;
  } else if (payload_length == 127u) {
    if (buffer.size() < 10u) {
      return std::nullopt;
    }
    payload_length = 0;
    for (std::size_t index = 0; index < 8u; ++index) {
      payload_length = (payload_length << 8) |
                       static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[2 + index]));
    }
    header_size = 10u;
  }
  if (payload_length > kWebSocketMaxPayloadBytes) {
    set_error("websocket: payload exceeds 1 MiB");
    buffer.clear();
    return DecodedFrame{0xFF, {}, fin};
  }
  const std::size_t mask_size = masked ? 4u : 0u;
  const std::size_t total = header_size + mask_size + static_cast<std::size_t>(payload_length);
  if (buffer.size() < total) {
    return std::nullopt;
  }
  DecodedFrame frame;
  frame.fin = fin;
  frame.opcode = opcode;
  frame.payload.assign(
      buffer.data() + header_size + mask_size,
      static_cast<std::size_t>(payload_length));
  if (masked) {
    const std::uint8_t* mask =
        reinterpret_cast<const std::uint8_t*>(buffer.data() + header_size);
    for (std::size_t index = 0; index < frame.payload.size(); ++index) {
      frame.payload[index] = static_cast<char>(
          static_cast<std::uint8_t>(frame.payload[index]) ^ mask[index % 4u]);
    }
  }
  buffer.erase(0, total);
  return frame;
}

} // namespace websocket_detail

class WsConnection {
  TcpStream stream_;
  std::string read_buffer_;
  bool closed_ = false;

  friend std::optional<WsConnection> upgrade_from_request(TcpStream, const HttpRequest&);
  friend std::optional<WsConnection> upgrade(TcpStream);
  friend std::optional<String> read_text(WsConnection&);
  friend bool write_text(WsConnection&, const String&);
  friend void close(WsConnection&);

  explicit WsConnection(TcpStream stream) : stream_(std::move(stream)) {}

public:
  WsConnection() = default;
  WsConnection(const WsConnection&) = delete;
  WsConnection& operator=(const WsConnection&) = delete;
  WsConnection(WsConnection&&) noexcept = default;
  WsConnection& operator=(WsConnection&&) noexcept = default;

  [[nodiscard]] bool is_open() const noexcept {
    return !closed_ && stream_.is_open();
  }
};

inline String websocket_last_error() {
  return websocket_detail::error_slot();
}

inline String sec_websocket_accept(const String& client_key) {
  return websocket_detail::compute_accept_key(client_key);
}

inline std::optional<WsConnection> upgrade_from_request(
    TcpStream stream,
    const HttpRequest& request) {
  String client_key;
  if (!websocket_detail::validate_upgrade_request(request, client_key)) {
    return std::nullopt;
  }
  const String accept = websocket_detail::compute_accept_key(client_key);
  const String response = String(
      std::string("HTTP/1.1 101 Switching Protocols\r\n") +
      "Upgrade: websocket\r\n" +
      "Connection: Upgrade\r\n" +
      "Sec-WebSocket-Accept: " + std::string(accept.view()) + "\r\n\r\n");
  auto write_result = stream.write_all(response);
  if (std::holds_alternative<result::Err<String>>(write_result)) {
    websocket_detail::set_error(std::get<result::Err<String>>(write_result)._0);
    return std::nullopt;
  }
  websocket_detail::error_slot() = String();
  return WsConnection(std::move(stream));
}

inline std::optional<WsConnection> upgrade(TcpStream stream) {
  auto message_result = read_http_message(stream);
  if (std::holds_alternative<result::Err<String>>(message_result)) {
    websocket_detail::set_error(std::get<result::Err<String>>(message_result)._0);
    return std::nullopt;
  }
  const String raw = std::get<result::Ok<String>>(message_result)._0;
  auto parse_result = parse_http_request(raw);
  if (std::holds_alternative<result::Err<String>>(parse_result)) {
    websocket_detail::set_error(std::get<result::Err<String>>(parse_result)._0);
    return std::nullopt;
  }
  const HttpRequest request = std::get<result::Ok<HttpRequest>>(parse_result)._0;
  return upgrade_from_request(std::move(stream), request);
}

inline bool write_text(WsConnection& connection, const String& data) {
  if (!connection.is_open()) {
    websocket_detail::set_error("websocket write_text: closed");
    return false;
  }
  if (data.size() > kWebSocketMaxPayloadBytes) {
    websocket_detail::set_error("websocket write_text: payload exceeds 1 MiB");
    return false;
  }
  const String frame =
      websocket_detail::encode_unmasked_frame(0x1u, data.view());
  auto write_result = connection.stream_.write_all(frame);
  if (std::holds_alternative<result::Err<String>>(write_result)) {
    websocket_detail::set_error(std::get<result::Err<String>>(write_result)._0);
    return false;
  }
  return true;
}

inline void close(WsConnection& connection) {
  if (!connection.closed_ && connection.stream_.is_open()) {
    const String frame = websocket_detail::encode_unmasked_frame(0x8u, "");
    (void)connection.stream_.write_all(frame);
    connection.stream_.close();
  }
  connection.closed_ = true;
}

inline std::optional<String> read_text(WsConnection& connection) {
  if (!connection.is_open()) {
    websocket_detail::set_error("websocket read_text: closed");
    return std::nullopt;
  }
  while (true) {
    auto frame = websocket_detail::try_decode_frame(connection.read_buffer_);
    if (frame.has_value()) {
      if (frame->opcode == 0xFFu) {
        close(connection);
        return std::nullopt;
      }
      if (!frame->fin) {
        websocket_detail::set_error("websocket: fragmented frames not supported");
        close(connection);
        return std::nullopt;
      }
      if (frame->opcode == 0x1u) {
        websocket_detail::error_slot() = String();
        return String(std::move(frame->payload));
      }
      if (frame->opcode == 0x9u) {
        const String pong =
            websocket_detail::encode_unmasked_frame(0xAu, frame->payload);
        auto write_result = connection.stream_.write_all(pong);
        if (std::holds_alternative<result::Err<String>>(write_result)) {
          websocket_detail::set_error(std::get<result::Err<String>>(write_result)._0);
          close(connection);
          return std::nullopt;
        }
        continue;
      }
      if (frame->opcode == 0x8u) {
        close(connection);
        websocket_detail::set_error("websocket: peer closed");
        return std::nullopt;
      }
      if (frame->opcode == 0x2u) {
        websocket_detail::set_error("websocket: binary frames not supported");
        close(connection);
        return std::nullopt;
      }
      websocket_detail::set_error("websocket: unsupported opcode");
      close(connection);
      return std::nullopt;
    }

    auto chunk_result = connection.stream_.read(kHttpReadChunkBytes);
    if (std::holds_alternative<result::Err<String>>(chunk_result)) {
      websocket_detail::set_error(std::get<result::Err<String>>(chunk_result)._0);
      close(connection);
      return std::nullopt;
    }
    const String chunk = std::get<result::Ok<String>>(chunk_result)._0;
    if (chunk.is_empty()) {
      websocket_detail::set_error("websocket: connection closed");
      close(connection);
      return std::nullopt;
    }
    connection.read_buffer_.append(chunk.view());
    if (connection.read_buffer_.size() > kWebSocketMaxPayloadBytes + 16u) {
      websocket_detail::set_error("websocket: read buffer overflow");
      close(connection);
      return std::nullopt;
    }
  }
}

} // namespace net
} // namespace mlc
