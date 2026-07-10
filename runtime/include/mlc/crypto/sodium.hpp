#pragma once

// libsodium crypto helpers (TRACK_STDLIB_CRYPTO STEP=2).
// Digests/random as lowercase hex strings; Option/bool + last_error.

#include "mlc/core/string.hpp"

#if __has_include(<sodium.h>)
#include <sodium.h>
#else
#error "mlc/crypto/sodium.hpp requires sodium.h (install libsodium-dev)"
#endif

#include <cstdint>
#include <mutex>
#include <optional>
#include <string>

namespace mlc {
namespace crypto {

namespace detail {

struct CryptoState {
  std::mutex mutex;
  String last_error;
  bool initialized = false;
};

inline CryptoState& crypto_state() {
  static CryptoState state;
  return state;
}

inline void set_last_error(String message) {
  std::lock_guard<std::mutex> lock(crypto_state().mutex);
  crypto_state().last_error = std::move(message);
}

inline bool ensure_sodium() {
  std::lock_guard<std::mutex> lock(crypto_state().mutex);
  if (crypto_state().initialized) {
    return true;
  }
  if (sodium_init() < 0) {
    crypto_state().last_error = String("Crypto: sodium_init failed");
    return false;
  }
  crypto_state().initialized = true;
  return true;
}

inline char hex_digit(unsigned value) {
  return static_cast<char>(value < 10 ? ('0' + value) : ('a' + (value - 10)));
}

inline String to_hex(const unsigned char* bytes, std::size_t length) {
  std::string hex;
  hex.resize(length * 2);
  for (std::size_t index = 0; index < length; ++index) {
    hex[index * 2] = hex_digit((bytes[index] >> 4) & 0x0f);
    hex[index * 2 + 1] = hex_digit(bytes[index] & 0x0f);
  }
  return String(std::move(hex));
}

} // namespace detail

inline String last_error() {
  std::lock_guard<std::mutex> lock(detail::crypto_state().mutex);
  return detail::crypto_state().last_error;
}

inline String sha256(const String& data) {
  if (!detail::ensure_sodium()) {
    return String();
  }
  unsigned char digest[crypto_hash_sha256_BYTES];
  crypto_hash_sha256(
      digest,
      reinterpret_cast<const unsigned char*>(data.view().data()),
      data.view().size()
  );
  return detail::to_hex(digest, crypto_hash_sha256_BYTES);
}

inline String hmac_sha256(const String& key, const String& data) {
  if (!detail::ensure_sodium()) {
    return String();
  }
  crypto_auth_hmacsha256_state state;
  crypto_auth_hmacsha256_init(
      &state,
      reinterpret_cast<const unsigned char*>(key.view().data()),
      key.view().size()
  );
  crypto_auth_hmacsha256_update(
      &state,
      reinterpret_cast<const unsigned char*>(data.view().data()),
      data.view().size()
  );
  unsigned char digest[crypto_auth_hmacsha256_BYTES];
  crypto_auth_hmacsha256_final(&state, digest);
  return detail::to_hex(digest, crypto_auth_hmacsha256_BYTES);
}

inline std::optional<String> random_bytes(std::int32_t count) {
  if (!detail::ensure_sodium()) {
    return std::nullopt;
  }
  if (count < 1 || count > 1024) {
    detail::set_last_error(String("Crypto.random_bytes: count must be 1..1024"));
    return std::nullopt;
  }
  std::string buffer(static_cast<std::size_t>(count), '\0');
  randombytes_buf(buffer.data(), buffer.size());
  return detail::to_hex(
      reinterpret_cast<const unsigned char*>(buffer.data()),
      buffer.size()
  );
}

inline std::optional<String> pwhash(const String& password) {
  if (!detail::ensure_sodium()) {
    return std::nullopt;
  }
  char hashed[crypto_pwhash_STRBYTES];
  if (crypto_pwhash_str(
          hashed,
          password.c_str(),
          password.size(),
          crypto_pwhash_OPSLIMIT_INTERACTIVE,
          crypto_pwhash_MEMLIMIT_INTERACTIVE
      ) != 0) {
    detail::set_last_error(String("Crypto.pwhash: crypto_pwhash_str failed"));
    return std::nullopt;
  }
  return String(hashed);
}

inline bool pwhash_verify(const String& hashed, const String& password) {
  if (!detail::ensure_sodium()) {
    return false;
  }
  if (hashed.size() >= crypto_pwhash_STRBYTES) {
    detail::set_last_error(String("Crypto.pwhash_verify: hashed too long"));
    return false;
  }
  const int status = crypto_pwhash_str_verify(hashed.c_str(), password.c_str(), password.size());
  if (status != 0) {
    // mismatch or malformed — do not overwrite with noisy error on mismatch
    return false;
  }
  return true;
}

} // namespace crypto
} // namespace mlc
