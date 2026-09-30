#pragma once

// curl_version() returns const char*. The FFI binder only accepts functions
// whose parameters and result are mlc::String, int32_t, or int64_t by value.

#include "mlc/core/string.hpp"

#if __has_include(<curl/curl.h>)
#include <curl/curl.h>
#else
#error "mlc/net/curl_abi.hpp requires curl/curl.h (install libcurl4-openssl-dev)"
#endif

namespace mlc {
namespace curl_abi {

inline String library_version_text() {
  const char* version = curl_version();
  if (version == nullptr) {
    return String();
  }
  return String(version);
}

}  // namespace curl_abi
}  // namespace mlc
