// WebSocket server smoke (TRACK_STDLIB_WEBSOCKET STEP=2).
// g++ -std=c++20 -pthread -I../include -o test_websocket test_websocket.cpp && ./test_websocket

#include "mlc/net/websocket.hpp"

#include <arpa/inet.h>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <variant>
#include <vector>

static int passed = 0;
static int failed = 0;

#define CHECK(expression) do { \
    if (expression) { ++passed; } \
    else { ++failed; std::cerr << "FAIL: " #expression " at line " << __LINE__ << "\n"; } \
} while (0)

template <typename OkType, typename ErrType>
bool is_ok(const mlc::result::Result<OkType, ErrType>& value) {
  return std::holds_alternative<mlc::result::Ok<OkType>>(value);
}

void test_sec_websocket_accept_rfc6455() {
  // RFC 6455 §1.3 example
  const mlc::String key("dGhlIHNhbXBsZSBub25jZQ==");
  const mlc::String accept = mlc::net::sec_websocket_accept(key);
  CHECK(accept.view() == "s3pPLMBiTxaQ9kYGzzhZRbK+xOo=");
}

std::string encode_masked_text_frame(const std::string& payload) {
  std::string frame;
  frame.push_back(static_cast<char>(0x81)); // FIN + text
  const std::uint8_t mask[4] = {0x12, 0x34, 0x56, 0x78};
  if (payload.size() <= 125) {
    frame.push_back(static_cast<char>(0x80 | payload.size()));
  } else {
    frame.push_back(static_cast<char>(0x80 | 126));
    frame.push_back(static_cast<char>((payload.size() >> 8) & 0xFF));
    frame.push_back(static_cast<char>(payload.size() & 0xFF));
  }
  frame.append(reinterpret_cast<const char*>(mask), 4);
  for (std::size_t index = 0; index < payload.size(); ++index) {
    frame.push_back(static_cast<char>(
        static_cast<std::uint8_t>(payload[index]) ^ mask[index % 4]));
  }
  return frame;
}

void test_upgrade_text_echo() {
  auto listener_result = mlc::net::TcpListener::bind(mlc::String("127.0.0.1"), 0);
  CHECK(is_ok(listener_result));
  if (!is_ok(listener_result)) {
    return;
  }
  auto listener = std::move(std::get<mlc::result::Ok<mlc::net::TcpListener>>(listener_result)._0);
  const std::int32_t port = listener.port();
  CHECK(port > 0);

  std::string client_received;
  std::thread client([port, &client_received] {
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    int client_socket = ::socket(AF_INET, SOCK_STREAM, 0);
    CHECK(client_socket >= 0);
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(static_cast<std::uint16_t>(port));
    ::inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);
    CHECK(::connect(client_socket, reinterpret_cast<sockaddr*>(&address), sizeof(address)) == 0);

    const char* request =
        "GET /chat HTTP/1.1\r\n"
        "Host: 127.0.0.1\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
        "Sec-WebSocket-Version: 13\r\n"
        "\r\n";
    CHECK(::send(client_socket, request, std::strlen(request), 0) > 0);

    char response_buffer[512] = {};
    const ssize_t response_size =
        ::recv(client_socket, response_buffer, sizeof(response_buffer) - 1, 0);
    CHECK(response_size > 0);
    const std::string response(response_buffer, static_cast<std::size_t>(response_size));
    CHECK(response.find("101") != std::string::npos);
    CHECK(response.find("s3pPLMBiTxaQ9kYGzzhZRbK+xOo=") != std::string::npos);

    const std::string frame = encode_masked_text_frame("hello-ws");
    CHECK(::send(client_socket, frame.data(), frame.size(), 0) ==
          static_cast<ssize_t>(frame.size()));

    char echo_buffer[256] = {};
    const ssize_t echo_size = ::recv(client_socket, echo_buffer, sizeof(echo_buffer), 0);
    CHECK(echo_size >= 2);
    if (echo_size >= 2) {
      const std::uint8_t second = static_cast<std::uint8_t>(echo_buffer[1]);
      CHECK((second & 0x80u) == 0); // server unmasked
      const std::size_t length = second & 0x7Fu;
      CHECK(length == 8);
      client_received.assign(echo_buffer + 2, length);
    }
    ::close(client_socket);
  });

  auto stream_result = listener.accept();
  CHECK(is_ok(stream_result));
  if (!is_ok(stream_result)) {
    client.join();
    return;
  }
  auto stream = std::move(std::get<mlc::result::Ok<mlc::net::TcpStream>>(stream_result)._0);
  auto connection_option = mlc::net::upgrade(std::move(stream));
  CHECK(connection_option.has_value());
  if (!connection_option.has_value()) {
    std::cerr << "upgrade failed: " << mlc::net::websocket_last_error().view() << "\n";
    client.join();
    return;
  }
  auto connection = std::move(*connection_option);
  auto text_option = mlc::net::read_text(connection);
  CHECK(text_option.has_value());
  if (text_option.has_value()) {
    CHECK(text_option->view() == "hello-ws");
    CHECK(mlc::net::write_text(connection, *text_option));
  }
  mlc::net::close(connection);
  client.join();
  CHECK(client_received == "hello-ws");
  listener.close();
}

int main() {
  test_sec_websocket_accept_rfc6455();
  test_upgrade_text_echo();
  std::cout << "websocket smoke: " << passed << " passed, " << failed << " failed\n";
  return failed == 0 ? 0 : 1;
}
