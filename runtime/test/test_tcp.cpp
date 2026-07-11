// TCP listener/stream smoke (TRACK_STDLIB_NET_SERVER STEP=2).
// g++ -std=c++20 -pthread -I../include -o test_tcp test_tcp.cpp && ./test_tcp

#include "mlc/net/tcp_bridge.hpp"

#include <chrono>
#include <iostream>
#include <thread>
#include <variant>

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

void test_bind_accept_echo() {
  auto listener_result = mlc::net::TcpListener::bind(mlc::String("127.0.0.1"), 0);
  CHECK(is_ok(listener_result));
  if (!is_ok(listener_result)) {
    std::cerr << "bind failed\n";
    return;
  }
  auto listener = std::move(std::get<mlc::result::Ok<mlc::net::TcpListener>>(listener_result)._0);
  const std::int32_t port = listener.port();
  CHECK(port > 0);

  std::thread client([port] {
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    int client_socket = ::socket(AF_INET, SOCK_STREAM, 0);
    CHECK(client_socket >= 0);
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(static_cast<std::uint16_t>(port));
    ::inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);
    const int connected = ::connect(
        client_socket,
        reinterpret_cast<sockaddr*>(&address),
        sizeof(address)
    );
    CHECK(connected == 0);
    const char* payload = "ping";
    CHECK(::send(client_socket, payload, 4, 0) == 4);
    char buffer[8] = {};
    const ssize_t received = ::recv(client_socket, buffer, sizeof(buffer), 0);
    CHECK(received == 4);
    CHECK(std::string(buffer, static_cast<std::size_t>(received)) == "ping");
    ::close(client_socket);
  });

  auto stream_result = listener.accept();
  CHECK(is_ok(stream_result));
  if (!is_ok(stream_result)) {
    client.join();
    return;
  }
  auto stream = std::move(std::get<mlc::result::Ok<mlc::net::TcpStream>>(stream_result)._0);
  auto read_result = stream.read(64);
  CHECK(is_ok(read_result));
  if (is_ok(read_result)) {
    const auto& text = std::get<mlc::result::Ok<mlc::String>>(read_result)._0;
    CHECK(text.view() == "ping");
    auto write_result = stream.write_all(text);
    CHECK(is_ok(write_result));
  }
  stream.close();
  client.join();
  listener.close();
}

void test_invalid_host() {
  auto result = mlc::net::TcpListener::bind(mlc::String("not-an-ip"), 0);
  CHECK(!is_ok(result));
}

int main() {
  test_bind_accept_echo();
  test_invalid_host();
  std::cout << "tcp smoke: " << passed << " passed, " << failed << " failed\n";
  return failed == 0 ? 0 : 1;
}
