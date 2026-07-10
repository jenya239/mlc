// HTTP ThreadPool serve smoke (TRACK_STDLIB_NET_SERVER STEP=7).
// g++ -std=c++20 -pthread -I../include -o test_http_server test_http_server.cpp && ./test_http_server

#include "mlc/net/http_server.hpp"

#include <arpa/inet.h>
#include <atomic>
#include <chrono>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>

static int passed = 0;
static int failed = 0;

#define CHECK(expression) do { \
    if (expression) { ++passed; } \
    else { ++failed; std::cerr << "FAIL: " #expression " at line " << __LINE__ << "\n"; } \
} while (0)

std::string client_get(std::int32_t port, const char* path) {
  const int client_socket = ::socket(AF_INET, SOCK_STREAM, 0);
  sockaddr_in address{};
  address.sin_family = AF_INET;
  address.sin_port = htons(static_cast<std::uint16_t>(port));
  ::inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);
  if (::connect(client_socket, reinterpret_cast<sockaddr*>(&address), sizeof(address)) != 0) {
    ::close(client_socket);
    return {};
  }
  const std::string request =
      std::string("GET ") + path + " HTTP/1.1\r\nHost: localhost\r\n\r\n";
  ::send(client_socket, request.data(), request.size(), 0);
  std::string response;
  char buffer[512];
  while (true) {
    const ssize_t received = ::recv(client_socket, buffer, sizeof(buffer), 0);
    if (received <= 0) {
      break;
    }
    response.append(buffer, static_cast<std::size_t>(received));
  }
  ::close(client_socket);
  return response;
}

void test_thread_pool_serves_two_clients() {
  auto listener_result = mlc::net::TcpListener::bind(mlc::String("127.0.0.1"), 0);
  CHECK(std::holds_alternative<mlc::result::Ok<mlc::net::TcpListener>>(listener_result));
  if (!std::holds_alternative<mlc::result::Ok<mlc::net::TcpListener>>(listener_result)) {
    return;
  }
  auto listener = std::move(std::get<mlc::result::Ok<mlc::net::TcpListener>>(listener_result)._0);
  const std::int32_t port = listener.port();

  mlc::net::HttpRouter router;
  router.add(mlc::String("GET"), mlc::String("/a"), [](const mlc::net::HttpRequest&) {
    mlc::net::HttpResponse response;
    response.status = 200;
    response.body = mlc::String("alpha");
    return response;
  });
  router.add(mlc::String("GET"), mlc::String("/b"), [](const mlc::net::HttpRequest&) {
    mlc::net::HttpResponse response;
    response.status = 200;
    response.body = mlc::String("bravo");
    return response;
  });

  mlc::concurrency::ThreadPool pool(2, 8);
  std::atomic<bool> stop{false};
  std::thread accept_thread([&] {
    mlc::net::serve_http_with_thread_pool(listener, router, pool, 2, &stop);
  });

  std::this_thread::sleep_for(std::chrono::milliseconds(20));

  std::string response_a;
  std::string response_b;
  std::thread client_a([&] { response_a = client_get(port, "/a"); });
  std::thread client_b([&] { response_b = client_get(port, "/b"); });
  client_a.join();
  client_b.join();

  stop.store(true, std::memory_order_release);
  listener.close();
  accept_thread.join();
  pool.shutdown();

  CHECK(response_a.find("HTTP/1.1 200 OK") == 0);
  CHECK(response_a.find("alpha") != std::string::npos);
  CHECK(response_b.find("HTTP/1.1 200 OK") == 0);
  CHECK(response_b.find("bravo") != std::string::npos);
}

int main() {
  test_thread_pool_serves_two_clients();
  std::cout << "http_server smoke: " << passed << " passed, " << failed << " failed\n";
  return failed == 0 ? 0 : 1;
}
