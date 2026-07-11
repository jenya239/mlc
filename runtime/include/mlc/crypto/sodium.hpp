#pragma once

// libsodium crypto helpers (Ruby / C++ path).
// TRACK_FFI_SHIM_MIGRATION STEP=3: hex + control flow mirror crypto.mlc;
// storage/init/HMAC malloc live in sodium_abi.hpp.

#include "mlc/core/string.hpp"
#include "mlc/crypto/sodium_abi.hpp"

#include <cstdint>
#include <optional>
#include <string>

namespace mlc {
namespace crypto {

namespace detail {

inline char hex_digit(unsigned value) {
  return static_cast<char>(value < 10 ? ('0' + value) : ('a' + (value - 10)));
}

inline String to_hex(const String& raw) {
  std::string hex;
  hex.resize(raw.size() * 2);
  for (std::size_t index = 0; index < raw.size(); ++index) {
    const unsigned value = static_cast<unsigned char>(raw.view().data()[index]);
    hex[index * 2] = hex_digit((value >> 4) & 0x0f);
    hex[index * 2 + 1] = hex_digit(value & 0x0f);
  }
  return String(std::move(hex));
}

} // namespace detail

inline String last_error() {
  return table_last_error();
}

inline String sha256(const String& data) {
  const String raw = sha256_raw(data);
  if (raw.size() == 0) {
    return String();
  }
  return detail::to_hex(raw);
}

inline String hmac_sha256(const String& key, const String& data) {
  if (ensure_sodium() == 0) {
    return String();
  }
  const std::int64_t state_pointer = hmac_state_alloc();
  if (state_pointer == 0) {
    table_set_error(String("Crypto.hmac_sha256: malloc failed"));
    return String();
  }
  hmac_init(state_pointer, key);
  hmac_update(state_pointer, data);
  const String raw = hmac_final_raw(state_pointer);
  hmac_state_free(state_pointer);
  if (raw.size() == 0) {
    return String();
  }
  return detail::to_hex(raw);
}

inline std::optional<String> random_bytes(std::int32_t count) {
  const String raw = random_bytes_raw(count);
  if (raw.size() == 0) {
    return std::nullopt;
  }
  return detail::to_hex(raw);
}

inline std::optional<String> pwhash(const String& password) {
  const String hashed = pwhash_str(password);
  if (hashed.size() == 0) {
    return std::nullopt;
  }
  return hashed;
}

inline bool pwhash_verify(const String& hashed, const String& password) {
  return pwhash_str_verify(hashed, password) != 0;
}

} // namespace crypto
} // namespace mlc
