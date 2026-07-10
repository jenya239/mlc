// Client for TRACK_STDLIB_NET_SERVER STEP=4 echo gate.
#include <arpa/inet.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>

int main(int argc, char** argv) {
  if (argc != 2) return 2;
  const int port = std::atoi(argv[1]);
  const int client_socket = ::socket(AF_INET, SOCK_STREAM, 0);
  if (client_socket < 0) return 3;
  sockaddr_in address{};
  address.sin_family = AF_INET;
  address.sin_port = htons(static_cast<uint16_t>(port));
  ::inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);
  if (::connect(client_socket, reinterpret_cast<sockaddr*>(&address), sizeof(address)) != 0) {
    std::perror("connect");
    return 4;
  }
  const char* payload = "ping";
  if (::send(client_socket, payload, 4, 0) != 4) return 5;
  char buffer[8] = {};
  const ssize_t received = ::recv(client_socket, buffer, 4, MSG_WAITALL);
  ::close(client_socket);
  if (received != 4) return 6;
  if (std::string(buffer, 4) != "ping") return 7;
  return 0;
}
