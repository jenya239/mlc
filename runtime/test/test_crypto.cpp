// libsodium crypto smoke (TRACK_STDLIB_CRYPTO STEP=2).
// Prefer: scripts/run_crypto_runtime_smoke.sh

#include "mlc/crypto/sodium.hpp"

#include <iostream>
#include <string>

static int passed = 0;
static int failed = 0;

#define CHECK(expression)                                                                          \
  do {                                                                                             \
    if (expression) {                                                                              \
      ++passed;                                                                                    \
    } else {                                                                                       \
      ++failed;                                                                                    \
      std::cerr << "FAIL: " #expression " at line " << __LINE__ << "\n";                            \
    }                                                                                              \
  } while (0)

void test_sha256_empty() {
  const auto digest = mlc::crypto::sha256(mlc::String(""));
  CHECK(digest.view() == "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
}

void test_hmac_sha256_rfc4231_case1() {
  // key = 20 × 0x0b, data = "Hi There"
  std::string key(20, '\x0b');
  const auto digest = mlc::crypto::hmac_sha256(mlc::String(key), mlc::String("Hi There"));
  CHECK(digest.view() == "b0344c61d8db38535ca8afceaf0bf12b881dc200c9833da726e9376c2e32cff7");
}

void test_random_bytes() {
  auto hex = mlc::crypto::random_bytes(16);
  CHECK(hex.has_value());
  if (hex.has_value()) {
    CHECK(hex->size() == 32);
  }
  CHECK(!mlc::crypto::random_bytes(0).has_value());
  CHECK(!mlc::crypto::random_bytes(2000).has_value());
}

void test_pwhash_roundtrip() {
  auto hashed = mlc::crypto::pwhash(mlc::String("correct horse"));
  CHECK(hashed.has_value());
  if (!hashed.has_value()) {
    return;
  }
  CHECK(mlc::crypto::pwhash_verify(*hashed, mlc::String("correct horse")));
  CHECK(!mlc::crypto::pwhash_verify(*hashed, mlc::String("wrong")));
}

int main() {
  test_sha256_empty();
  test_hmac_sha256_rfc4231_case1();
  test_random_bytes();
  test_pwhash_roundtrip();
  std::cout << "crypto smoke: " << passed << " passed, " << failed << " failed\n";
  return failed == 0 ? 0 : 1;
}
