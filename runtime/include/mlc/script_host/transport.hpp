#pragma once

#include "mlc/core/array.hpp"

#include <cstdint>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <vector>

namespace mlc {
namespace script_host {

inline constexpr std::int32_t transport_loopback_max_words() {
  return 15;
}

inline constexpr std::int32_t transport_loopback_handle_ok() {
  return 1;
}

struct TransportLoopbackPair {
  int file_descriptor_zero;
  int file_descriptor_one;
  std::int32_t handle;
};

inline TransportLoopbackPair& transport_loopback_pair() {
  static TransportLoopbackPair pair{-1, -1, 0};
  return pair;
}

inline std::int32_t transport_loopback_used_shared_memory() {
  return 0;
}

inline std::int32_t transport_loopback_used_file_descriptor_passing() {
  return 0;
}

inline void transport_loopback_close_pair() {
  TransportLoopbackPair& pair = transport_loopback_pair();
  if (pair.file_descriptor_zero >= 0) {
    ::close(pair.file_descriptor_zero);
  }
  if (pair.file_descriptor_one >= 0) {
    ::close(pair.file_descriptor_one);
  }
  pair.file_descriptor_zero = -1;
  pair.file_descriptor_one = -1;
  pair.handle = 0;
}

inline std::int32_t transport_loopback_open() {
  transport_loopback_close_pair();
  int descriptors[2] = {-1, -1};
  if (::socketpair(AF_UNIX, SOCK_STREAM, 0, descriptors) != 0) {
    return 0;
  }
  TransportLoopbackPair& pair = transport_loopback_pair();
  pair.file_descriptor_zero = descriptors[0];
  pair.file_descriptor_one = descriptors[1];
  pair.handle = transport_loopback_handle_ok();
  return pair.handle;
}

inline std::int32_t transport_loopback_close(std::int32_t handle) {
  TransportLoopbackPair& pair = transport_loopback_pair();
  if (handle == 0 || handle != pair.handle) {
    return 0;
  }
  transport_loopback_close_pair();
  return 1;
}

inline int transport_loopback_file_descriptor(std::int32_t handle, std::int32_t socket_end) {
  TransportLoopbackPair& pair = transport_loopback_pair();
  if (handle == 0 || handle != pair.handle) {
    return -1;
  }
  if (socket_end == 0) {
    return pair.file_descriptor_zero;
  }
  if (socket_end == 1) {
    return pair.file_descriptor_one;
  }
  return -1;
}

inline bool transport_loopback_write_all(
  int file_descriptor,
  const std::uint8_t* bytes,
  std::size_t length
) {
  std::size_t offset = 0;
  while (offset < length) {
    const ssize_t written_count = ::write(
      file_descriptor,
      bytes + offset,
      length - offset
    );
    if (written_count <= 0) {
      return false;
    }
    offset += static_cast<std::size_t>(written_count);
  }
  return true;
}

inline bool transport_loopback_read_all(
  int file_descriptor,
  std::uint8_t* bytes,
  std::size_t length
) {
  std::size_t offset = 0;
  while (offset < length) {
    const ssize_t read_count = ::read(
      file_descriptor,
      bytes + offset,
      length - offset
    );
    if (read_count <= 0) {
      return false;
    }
    offset += static_cast<std::size_t>(read_count);
  }
  return true;
}

inline void transport_loopback_store_i32(std::uint8_t* bytes, std::int32_t value) {
  const std::uint32_t unsigned_value = static_cast<std::uint32_t>(value);
  bytes[0] = static_cast<std::uint8_t>(unsigned_value & 0xffu);
  bytes[1] = static_cast<std::uint8_t>((unsigned_value >> 8) & 0xffu);
  bytes[2] = static_cast<std::uint8_t>((unsigned_value >> 16) & 0xffu);
  bytes[3] = static_cast<std::uint8_t>((unsigned_value >> 24) & 0xffu);
}

inline std::int32_t transport_loopback_load_i32(const std::uint8_t* bytes) {
  const std::uint32_t unsigned_value =
    static_cast<std::uint32_t>(bytes[0])
    | (static_cast<std::uint32_t>(bytes[1]) << 8)
    | (static_cast<std::uint32_t>(bytes[2]) << 16)
    | (static_cast<std::uint32_t>(bytes[3]) << 24);
  return static_cast<std::int32_t>(unsigned_value);
}

inline std::int32_t transport_loopback_send(
  std::int32_t handle,
  std::int32_t socket_end,
  Array<int32_t> words
) {
  const int file_descriptor = transport_loopback_file_descriptor(handle, socket_end);
  if (file_descriptor < 0) {
    return 0;
  }
  if (words.length() > transport_loopback_max_words()) {
    return 0;
  }
  const std::int32_t word_count = words.length();
  std::vector<std::uint8_t> owned_copy(
    static_cast<std::size_t>(4 + word_count * 4)
  );
  transport_loopback_store_i32(owned_copy.data(), word_count);
  std::int32_t index = 0;
  while (index < word_count) {
    transport_loopback_store_i32(
      owned_copy.data() + static_cast<std::size_t>(4 + index * 4),
      words[index]
    );
    index = index + 1;
  }
  if (!transport_loopback_write_all(file_descriptor, owned_copy.data(), owned_copy.size())) {
    return 0;
  }
  return 1;
}

inline Array<int32_t> transport_loopback_receive(std::int32_t handle, std::int32_t socket_end) {
  Array<int32_t> words;
  const int file_descriptor = transport_loopback_file_descriptor(handle, socket_end);
  if (file_descriptor < 0) {
    return words;
  }
  std::uint8_t count_bytes[4];
  if (!transport_loopback_read_all(file_descriptor, count_bytes, 4)) {
    return words;
  }
  const std::int32_t word_count = transport_loopback_load_i32(count_bytes);
  if (word_count < 0 || word_count > transport_loopback_max_words()) {
    return words;
  }
  std::vector<std::uint8_t> owned_copy(static_cast<std::size_t>(word_count * 4));
  if (word_count > 0) {
    if (!transport_loopback_read_all(file_descriptor, owned_copy.data(), owned_copy.size())) {
      return Array<int32_t>{};
    }
  }
  std::int32_t index = 0;
  while (index < word_count) {
    words.push_back(
      transport_loopback_load_i32(
        owned_copy.data() + static_cast<std::size_t>(index * 4)
      )
    );
    index = index + 1;
  }
  return words;
}

} // namespace script_host
} // namespace mlc
