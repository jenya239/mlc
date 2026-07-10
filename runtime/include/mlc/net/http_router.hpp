#pragma once

// HTTP response + exact-match router (TRACK_STDLIB_NET_SERVER STEP=6).

#include "mlc/net/http_request.hpp"
#include "mlc/net/tcp.hpp"

#include <functional>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace mlc {
namespace net {

struct HttpResponse {
  std::int32_t status = 200;
  std::vector<HttpHeader> headers;
  String body;
};

inline std::string_view http_status_text(std::int32_t status) {
  switch (status) {
    case 200: return "OK";
    case 201: return "Created";
    case 204: return "No Content";
    case 400: return "Bad Request";
    case 404: return "Not Found";
    case 405: return "Method Not Allowed";
    case 500: return "Internal Server Error";
    default: return "OK";
  }
}

inline HttpResponse http_not_found() {
  HttpResponse response;
  response.status = 404;
  response.headers.push_back(HttpHeader{String("Content-Type"), String("text/plain")});
  response.body = String("Not Found");
  return response;
}

inline String format_http_response(const HttpResponse& response) {
  std::ostringstream output;
  output << "HTTP/1.1 " << response.status << ' ' << http_status_text(response.status) << "\r\n";
  bool has_content_length = false;
  for (const auto& header : response.headers) {
    if (detail::starts_with_ignore_case(header.name.view(), "Content-Length") &&
        header.name.size() == 14) {
      has_content_length = true;
    }
    output << header.name.view() << ": " << header.value.view() << "\r\n";
  }
  if (!has_content_length) {
    output << "Content-Length: " << response.body.size() << "\r\n";
  }
  output << "Connection: close\r\n";
  output << "\r\n";
  output << response.body.view();
  return String(output.str());
}

inline result::Result<bool, String> write_http_response(
    TcpStream& stream,
    const HttpResponse& response
) {
  return stream.write_all(format_http_response(response));
}

using HttpHandler = std::function<HttpResponse(const HttpRequest&)>;

struct HttpRoute {
  String method;
  String path;
  HttpHandler handler;
};

class HttpRouter {
  std::vector<HttpRoute> routes_;

public:
  void add(String method, String path, HttpHandler handler) {
    routes_.push_back(HttpRoute{
        std::move(method),
        std::move(path),
        std::move(handler)
    });
  }

  [[nodiscard]] HttpResponse dispatch(const HttpRequest& request) const {
    for (const auto& route : routes_) {
      if (route.method.view() == request.method.view() &&
          route.path.view() == request.path.view()) {
        return route.handler(request);
      }
    }
    return http_not_found();
  }
};

} // namespace net
} // namespace mlc
