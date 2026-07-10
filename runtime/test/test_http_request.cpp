// HTTP request parser smoke (TRACK_STDLIB_NET_SERVER STEP=5).
// g++ -std=c++20 -I../include -o test_http_request test_http_request.cpp && ./test_http_request

#include "mlc/net/http_request.hpp"

#include <iostream>
#include <string>
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

void test_get_with_headers() {
  const mlc::String raw(
      "GET /hello HTTP/1.1\r\n"
      "Host: example.com\r\n"
      "User-Agent: test\r\n"
      "\r\n"
  );
  auto parsed = mlc::net::parse_http_request(raw);
  CHECK(is_ok(parsed));
  if (!is_ok(parsed)) {
    return;
  }
  const auto& request = std::get<mlc::result::Ok<mlc::net::HttpRequest>>(parsed)._0;
  CHECK(request.method.view() == "GET");
  CHECK(request.path.view() == "/hello");
  CHECK(request.headers.size() == 2);
  CHECK(request.headers[0].name.view() == "Host");
  CHECK(request.headers[0].value.view() == "example.com");
  CHECK(request.body.is_empty());
}

void test_post_with_body_lf_only() {
  const mlc::String raw(
      "POST /api HTTP/1.0\n"
      "Content-Length: 5\n"
      "\n"
      "helloEXTRA"
  );
  auto parsed = mlc::net::parse_http_request(raw);
  CHECK(is_ok(parsed));
  if (!is_ok(parsed)) {
    return;
  }
  const auto& request = std::get<mlc::result::Ok<mlc::net::HttpRequest>>(parsed)._0;
  CHECK(request.method.view() == "POST");
  CHECK(request.path.view() == "/api");
  CHECK(request.body.view() == "hello");
}

void test_rejects_oversized_headers() {
  std::string raw = "GET / HTTP/1.1\r\n";
  raw.append(70u * 1024u, 'A');
  raw += ": x\r\n\r\n";
  auto parsed = mlc::net::parse_http_request(mlc::String(std::move(raw)));
  CHECK(!is_ok(parsed));
}

void test_rejects_truncated_body() {
  const mlc::String raw(
      "POST / HTTP/1.1\r\n"
      "Content-Length: 10\r\n"
      "\r\n"
      "short"
  );
  auto parsed = mlc::net::parse_http_request(raw);
  CHECK(!is_ok(parsed));
}

void test_rejects_bad_version() {
  const mlc::String raw("GET / HTTP/2.0\r\n\r\n");
  auto parsed = mlc::net::parse_http_request(raw);
  CHECK(!is_ok(parsed));
}

int main() {
  test_get_with_headers();
  test_post_with_body_lf_only();
  test_rejects_oversized_headers();
  test_rejects_truncated_body();
  test_rejects_bad_version();
  std::cout << "http_request smoke: " << passed << " passed, " << failed << " failed\n";
  return failed == 0 ? 0 : 1;
}
