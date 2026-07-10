// HTTP router + response smoke (TRACK_STDLIB_NET_SERVER STEP=6).
// g++ -std=c++20 -pthread -I../include -o test_http_router test_http_router.cpp && ./test_http_router

#include "mlc/net/http_router.hpp"

#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <variant>

static int passed = 0;
static int failed = 0;

#define CHECK(expression) do { \
    if (expression) { ++passed; } \
    else { ++failed; std::cerr << "FAIL: " #expression " at line " << __LINE__ << "\n"; } \
} while (0)

void test_format_includes_status_and_body() {
  mlc::net::HttpResponse response;
  response.status = 200;
  response.headers.push_back(
      mlc::net::HttpHeader{mlc::String("Content-Type"), mlc::String("text/plain")}
  );
  response.body = mlc::String("hi");
  const mlc::String formatted = mlc::net::format_http_response(response);
  CHECK(formatted.view().find("HTTP/1.1 200 OK\r\n") == 0);
  CHECK(formatted.view().find("Content-Type: text/plain\r\n") != std::string_view::npos);
  CHECK(formatted.view().find("Content-Length: 2\r\n") != std::string_view::npos);
  CHECK(formatted.view().find("\r\n\r\nhi") != std::string_view::npos);
}

void test_router_exact_match_and_404() {
  mlc::net::HttpRouter router;
  router.add(mlc::String("GET"), mlc::String("/hello"), [](const mlc::net::HttpRequest&) {
    mlc::net::HttpResponse response;
    response.status = 200;
    response.body = mlc::String("world");
    return response;
  });

  mlc::net::HttpRequest hit;
  hit.method = mlc::String("GET");
  hit.path = mlc::String("/hello");
  const auto matched = router.dispatch(hit);
  CHECK(matched.status == 200);
  CHECK(matched.body.view() == "world");

  mlc::net::HttpRequest miss;
  miss.method = mlc::String("GET");
  miss.path = mlc::String("/missing");
  const auto not_found = router.dispatch(miss);
  CHECK(not_found.status == 404);
  CHECK(not_found.body.view() == "Not Found");
}

void test_write_http_response_over_tcp() {
  auto listener_result = mlc::net::TcpListener::bind(mlc::String("127.0.0.1"), 0);
  CHECK(std::holds_alternative<mlc::result::Ok<mlc::net::TcpListener>>(listener_result));
  if (!std::holds_alternative<mlc::result::Ok<mlc::net::TcpListener>>(listener_result)) {
    return;
  }
  auto listener = std::move(std::get<mlc::result::Ok<mlc::net::TcpListener>>(listener_result)._0);
  const std::int32_t port = listener.port();

  std::thread client([port] {
    int client_socket = ::socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(static_cast<std::uint16_t>(port));
    ::inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);
    ::connect(client_socket, reinterpret_cast<sockaddr*>(&address), sizeof(address));
    char buffer[256] = {};
    const ssize_t received = ::recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    CHECK(received > 0);
    const std::string text(buffer, static_cast<std::size_t>(received));
    CHECK(text.find("HTTP/1.1 404 Not Found\r\n") == 0);
    CHECK(text.find("Not Found") != std::string::npos);
    ::close(client_socket);
  });

  auto stream_result = listener.accept();
  CHECK(std::holds_alternative<mlc::result::Ok<mlc::net::TcpStream>>(stream_result));
  if (!std::holds_alternative<mlc::result::Ok<mlc::net::TcpStream>>(stream_result)) {
    client.join();
    return;
  }
  auto stream = std::move(std::get<mlc::result::Ok<mlc::net::TcpStream>>(stream_result)._0);
  auto write_result = mlc::net::write_http_response(stream, mlc::net::http_not_found());
  CHECK(std::holds_alternative<mlc::result::Ok<bool>>(write_result));
  stream.close();
  client.join();
  listener.close();
}

int main() {
  test_format_includes_status_and_body();
  test_router_exact_match_and_404();
  test_write_http_response_over_tcp();
  std::cout << "http_router smoke: " << passed << " passed, " << failed << " failed\n";
  return failed == 0 ? 0 : 1;
}
