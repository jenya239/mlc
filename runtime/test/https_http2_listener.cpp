#include <nghttp2/nghttp2.h>
#include <openssl/ssl.h>

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

namespace {

const char kResponseBody[] = "http2-ok";

struct ResponseBytes {
  const uint8_t* data = nullptr;
  std::size_t size = 0;
  std::size_t offset = 0;
};

struct SecureConnection {
  SSL* secure_socket = nullptr;
  ResponseBytes response;
};

bool tls_would_block(SSL* secure_socket, int result) {
  const int error = SSL_get_error(secure_socket, result);
  return error == SSL_ERROR_WANT_READ || error == SSL_ERROR_WANT_WRITE;
}

ssize_t send_tls(nghttp2_session*, const uint8_t* data, std::size_t length, int, void* user_data) {
  auto* connection = static_cast<SecureConnection*>(user_data);
  const int written = SSL_write(connection->secure_socket, data, static_cast<int>(length));
  if (written > 0) return written;
  if (tls_would_block(connection->secure_socket, written)) return NGHTTP2_ERR_WOULDBLOCK;
  return NGHTTP2_ERR_CALLBACK_FAILURE;
}

ssize_t receive_tls(nghttp2_session*, uint8_t* data, std::size_t length, int, void* user_data) {
  auto* connection = static_cast<SecureConnection*>(user_data);
  const int received = SSL_read(connection->secure_socket, data, static_cast<int>(length));
  if (received > 0) return received;
  if (tls_would_block(connection->secure_socket, received)) return NGHTTP2_ERR_WOULDBLOCK;
  if (SSL_get_error(connection->secure_socket, received) == SSL_ERROR_ZERO_RETURN) return NGHTTP2_ERR_EOF;
  return NGHTTP2_ERR_CALLBACK_FAILURE;
}

ssize_t read_response_body(
    nghttp2_session*,
    std::int32_t,
    uint8_t* data,
    std::size_t length,
    std::uint32_t* data_flags,
    nghttp2_data_source* source,
    void*) {
  auto* response = static_cast<ResponseBytes*>(source->ptr);
  if (response->offset >= response->size) {
    *data_flags |= NGHTTP2_DATA_FLAG_EOF;
    return 0;
  }
  const std::size_t remaining = response->size - response->offset;
  const std::size_t count = remaining < length ? remaining : length;
  std::memcpy(data, response->data + response->offset, count);
  response->offset += count;
  if (response->offset >= response->size) *data_flags |= NGHTTP2_DATA_FLAG_EOF;
  return static_cast<ssize_t>(count);
}

nghttp2_nv header_field(const char* name, const char* value) {
  nghttp2_nv field;
  field.name = reinterpret_cast<uint8_t*>(const_cast<char*>(name));
  field.value = reinterpret_cast<uint8_t*>(const_cast<char*>(value));
  field.namelen = std::strlen(name);
  field.valuelen = std::strlen(value);
  field.flags = NGHTTP2_NV_FLAG_NONE;
  return field;
}

int on_frame_received(nghttp2_session* session, const nghttp2_frame* frame, void* user_data) {
  if (frame->hd.type != NGHTTP2_HEADERS || frame->headers.cat != NGHTTP2_HCAT_REQUEST) return 0;
  if ((frame->hd.flags & NGHTTP2_FLAG_END_STREAM) == 0) return 0;
  auto* connection = static_cast<SecureConnection*>(user_data);
  nghttp2_nv fields[] = {header_field(":status", "200"), header_field("content-type", "text/plain")};
  nghttp2_data_provider provider;
  provider.source.ptr = &connection->response;
  provider.read_callback = read_response_body;
  if (nghttp2_submit_response(session, frame->hd.stream_id, fields, 2, &provider) != 0) {
    return NGHTTP2_ERR_CALLBACK_FAILURE;
  }
  return 0;
}

int select_http2(
    SSL*,
    const unsigned char** selected,
    unsigned char* selected_length,
    const unsigned char* offered,
    unsigned int offered_length,
    void*) {
  static const unsigned char protocol[] = "\x02h2";
  const int status = SSL_select_next_proto(
      const_cast<unsigned char**>(selected),
      selected_length,
      protocol,
      sizeof(protocol) - 1,
      offered,
      offered_length);
  if (status != OPENSSL_NPN_NEGOTIATED) return SSL_TLSEXT_ERR_ALERT_FATAL;
  return SSL_TLSEXT_ERR_OK;
}

void serve_connection(SSL* secure_socket) {
  SecureConnection connection;
  connection.secure_socket = secure_socket;
  connection.response.data = reinterpret_cast<const uint8_t*>(kResponseBody);
  connection.response.size = sizeof(kResponseBody) - 1;

  nghttp2_session_callbacks* callbacks = nullptr;
  nghttp2_session_callbacks_new(&callbacks);
  nghttp2_session_callbacks_set_send_callback(callbacks, send_tls);
  nghttp2_session_callbacks_set_recv_callback(callbacks, receive_tls);
  nghttp2_session_callbacks_set_on_frame_recv_callback(callbacks, on_frame_received);

  nghttp2_session* session = nullptr;
  if (nghttp2_session_server_new(&session, callbacks, &connection) != 0) {
    nghttp2_session_callbacks_del(callbacks);
    return;
  }
  nghttp2_session_callbacks_del(callbacks);

  nghttp2_settings_entry settings[1];
  settings[0].settings_id = NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS;
  settings[0].value = 1;
  nghttp2_submit_settings(session, NGHTTP2_FLAG_NONE, settings, 1);

  const int descriptor = SSL_get_fd(secure_socket);
  const int flags = fcntl(descriptor, F_GETFL, 0);
  fcntl(descriptor, F_SETFL, flags | O_NONBLOCK);

  while (nghttp2_session_want_read(session) != 0 || nghttp2_session_want_write(session) != 0) {
    if (nghttp2_session_want_write(session) != 0) {
      const int sent = nghttp2_session_send(session);
      if (sent != 0 && sent != NGHTTP2_ERR_WOULDBLOCK) break;
    }
    if (nghttp2_session_want_read(session) != 0) {
      const int received = nghttp2_session_recv(session);
      if (received == NGHTTP2_ERR_EOF) break;
      if (received != 0 && received != NGHTTP2_ERR_WOULDBLOCK) break;
    }
    fd_set read_set;
    fd_set write_set;
    FD_ZERO(&read_set);
    FD_ZERO(&write_set);
    if (nghttp2_session_want_read(session) != 0) FD_SET(descriptor, &read_set);
    if (nghttp2_session_want_write(session) != 0) FD_SET(descriptor, &write_set);
    if (select(descriptor + 1, &read_set, &write_set, nullptr, nullptr) < 0) break;
  }
  nghttp2_session_del(session);
}

int accept_loop(int listening_socket, SSL_CTX* context) {
  while (true) {
    const int client_socket = accept(listening_socket, nullptr, nullptr);
    if (client_socket < 0) return 1;
    const int no_delay = 1;
    setsockopt(client_socket, IPPROTO_TCP, TCP_NODELAY, &no_delay, sizeof(no_delay));
    SSL* secure_socket = SSL_new(context);
    SSL_set_fd(secure_socket, client_socket);
    if (SSL_accept(secure_socket) == 1) {
      const unsigned char* selected = nullptr;
      unsigned int selected_length = 0;
      SSL_get0_alpn_selected(secure_socket, &selected, &selected_length);
      if (selected_length == 2 && selected[0] == 'h' && selected[1] == '2') {
        serve_connection(secure_socket);
      }
    }
    SSL_shutdown(secure_socket);
    SSL_free(secure_socket);
    close(client_socket);
  }
}

}  // namespace

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "usage: https_http2_listener DIRECTORY\n";
    return 1;
  }
  const std::string directory = argv[1];
  SSL_CTX* context = SSL_CTX_new(TLS_server_method());
  if (context == nullptr) return 1;
  SSL_CTX_set_min_proto_version(context, TLS1_2_VERSION);
  SSL_CTX_set_alpn_select_cb(context, select_http2, nullptr);
  const std::string certificate_path = directory + "/good_certificate.pem";
  const std::string key_path = directory + "/good_private_key.pem";
  if (SSL_CTX_use_certificate_file(context, certificate_path.c_str(), SSL_FILETYPE_PEM) != 1 ||
      SSL_CTX_use_PrivateKey_file(context, key_path.c_str(), SSL_FILETYPE_PEM) != 1) {
    SSL_CTX_free(context);
    return 1;
  }

  const int listening_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (listening_socket < 0) {
    SSL_CTX_free(context);
    return 1;
  }
  const int reuse = 1;
  setsockopt(listening_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
  sockaddr_in address;
  std::memset(&address, 0, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_port = 0;
  inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);
  if (bind(listening_socket, reinterpret_cast<sockaddr*>(&address), sizeof(address)) != 0 ||
      listen(listening_socket, 16) != 0) {
    close(listening_socket);
    SSL_CTX_free(context);
    return 1;
  }
  sockaddr_in bound;
  socklen_t bound_length = sizeof(bound);
  if (getsockname(listening_socket, reinterpret_cast<sockaddr*>(&bound), &bound_length) != 0) {
    close(listening_socket);
    SSL_CTX_free(context);
    return 1;
  }
  std::ofstream port_file(directory + "/http2_port");
  port_file << ntohs(bound.sin_port);
  port_file.close();

  const int status = accept_loop(listening_socket, context);
  close(listening_socket);
  SSL_CTX_free(context);
  return status;
}
