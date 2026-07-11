#pragma once

// Thin libsodium ABI for TRACK_FFI_SHIM_MIGRATION STEP=3.
// Digests returned as raw byte strings; hex lives in crypto.mlc.
// HMAC state via statebytes()+malloc (no stack C++ state object).

#include "mlc/core/string.hpp"

#if __has_include(<sodium.h>)
#include <sodium.h>
#else
#error "mlc/crypto/sodium_abi.hpp requires sodium.h (install libsodium-dev)"
#endif

#include <cstdint>
#include <cstdlib>
#include <cstring>

namespace mlc {
namespace crypto {

namespace abi_detail {

struct CryptoAbiState {
  String last_error;
  bool initialized = false;
};

inline CryptoAbiState& crypto_abi_state() {
  static CryptoAbiState state;
  return state;
}

inline String raw_bytes_string(const unsigned char* bytes, std::size_t length) {
  return String(reinterpret_cast<const char*>(bytes), length);
}

} // namespace abi_detail

inline void table_set_error(String message) {
  abi_detail::crypto_abi_state().last_error = std::move(message);
}

inline String table_last_error() {
  return abi_detail::crypto_abi_state().last_error;
}

inline std::int32_t ensure_sodium() {
  if (abi_detail::crypto_abi_state().initialized) {
    return 1;
  }
  if (sodium_init() < 0) {
    table_set_error(String("Crypto: sodium_init failed"));
    return 0;
  }
  abi_detail::crypto_abi_state().initialized = true;
  return 1;
}

inline std::int32_t string_byte_u8(String text, std::int32_t index) {
  if (index < 0 || static_cast<std::size_t>(index) >= text.size()) {
    return -1;
  }
  return static_cast<std::int32_t>(
      static_cast<unsigned char>(text.view().data()[static_cast<std::size_t>(index)]));
}

// One-byte string for building binary keys (MLC has no \xHH string escapes).
inline String string_from_byte_u8(std::int32_t value) {
  const char byte = static_cast<char>(static_cast<unsigned char>(value & 0xff));
  return String(&byte, 1);
}

inline String sha256_raw(String data) {
  if (ensure_sodium() == 0) {
    return String();
  }
  unsigned char digest[crypto_hash_sha256_BYTES];
  crypto_hash_sha256(
      digest,
      reinterpret_cast<const unsigned char*>(data.view().data()),
      data.view().size()
  );
  return abi_detail::raw_bytes_string(digest, crypto_hash_sha256_BYTES);
}

inline std::int64_t hmac_statebytes() {
  return static_cast<std::int64_t>(crypto_auth_hmacsha256_statebytes());
}

inline std::int64_t hmac_state_alloc() {
  const std::size_t bytes = crypto_auth_hmacsha256_statebytes();
  void* memory = std::malloc(bytes);
  if (memory == nullptr) {
    return 0;
  }
  std::memset(memory, 0, bytes);
  return reinterpret_cast<std::int64_t>(memory);
}

inline void hmac_state_free(std::int64_t state_pointer) {
  if (state_pointer != 0) {
    std::free(reinterpret_cast<void*>(state_pointer));
  }
}

inline std::int32_t hmac_init(std::int64_t state_pointer, String key) {
  if (state_pointer == 0) {
    return -1;
  }
  auto* state = reinterpret_cast<crypto_auth_hmacsha256_state*>(state_pointer);
  return crypto_auth_hmacsha256_init(
      state,
      reinterpret_cast<const unsigned char*>(key.view().data()),
      key.view().size()
  );
}

inline std::int32_t hmac_update(std::int64_t state_pointer, String data) {
  if (state_pointer == 0) {
    return -1;
  }
  auto* state = reinterpret_cast<crypto_auth_hmacsha256_state*>(state_pointer);
  return crypto_auth_hmacsha256_update(
      state,
      reinterpret_cast<const unsigned char*>(data.view().data()),
      data.view().size()
  );
}

inline String hmac_final_raw(std::int64_t state_pointer) {
  if (state_pointer == 0) {
    return String();
  }
  auto* state = reinterpret_cast<crypto_auth_hmacsha256_state*>(state_pointer);
  unsigned char digest[crypto_auth_hmacsha256_BYTES];
  crypto_auth_hmacsha256_final(state, digest);
  return abi_detail::raw_bytes_string(digest, crypto_auth_hmacsha256_BYTES);
}

inline String random_bytes_raw(std::int32_t count) {
  if (ensure_sodium() == 0) {
    return String();
  }
  if (count < 1 || count > 1024) {
    table_set_error(String("Crypto.random_bytes: count must be 1..1024"));
    return String();
  }
  std::string buffer(static_cast<std::size_t>(count), '\0');
  randombytes_buf(buffer.data(), buffer.size());
  return String(std::move(buffer));
}

inline String pwhash_str(String password) {
  if (ensure_sodium() == 0) {
    return String();
  }
  char hashed[crypto_pwhash_STRBYTES];
  if (crypto_pwhash_str(
          hashed,
          password.c_str(),
          password.size(),
          crypto_pwhash_OPSLIMIT_INTERACTIVE,
          crypto_pwhash_MEMLIMIT_INTERACTIVE
      ) != 0) {
    table_set_error(String("Crypto.pwhash: crypto_pwhash_str failed"));
    return String();
  }
  return String(hashed);
}

inline std::int32_t pwhash_str_verify(String hashed, String password) {
  if (ensure_sodium() == 0) {
    return 0;
  }
  if (hashed.size() >= crypto_pwhash_STRBYTES) {
    table_set_error(String("Crypto.pwhash_verify: hashed too long"));
    return 0;
  }
  if (crypto_pwhash_str_verify(hashed.c_str(), password.c_str(), password.size()) != 0) {
    return 0;
  }
  return 1;
}

inline std::int32_t sha256_bytes() {
  return static_cast<std::int32_t>(crypto_hash_sha256_BYTES);
}

inline std::int32_t hmac_sha256_bytes() {
  return static_cast<std::int32_t>(crypto_auth_hmacsha256_BYTES);
}

} // namespace crypto
} // namespace mlc
