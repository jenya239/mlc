#pragma once

// By-value String FFI for mlcc (TRACK_EDITOR_MVP STEP=19).
// Wraps mlc::file::* which take const String&.

#include "mlc/io/file.hpp"
#include <cstdint>
#include <system_error>

namespace mlc {
namespace file {

inline String read_to_string_value(String path) {
  return read_to_string(path);
}

inline bool write_string_value(String path, String content) {
  try {
    return write_string(path, content);
  } catch (...) {
    return false;
  }
}

inline bool exists_value(String path) {
  return exists(path);
}

inline bool is_directory_value(String path) {
  try {
    return is_directory(path);
  } catch (...) {
    return false;
  }
}

inline bool is_regular_file_value(String path) {
  try {
    return is_regular_file(path);
  } catch (...) {
    return false;
  }
}

inline String absolute_path_value(String path) {
  try {
    return absolute_path(path);
  } catch (...) {
    return path;
  }
}

inline bool create_directories_value(String path) {
  try {
    return create_directories(path);
  } catch (...) {
    return false;
  }
}

inline bool remove_file_value(String path) {
  return remove_file(path);
}

// TRACK_EDITOR_HYGIENE §107m — pre-read size so oversized opens refuse without loading.
// Returns byte size, or -1 on error / non-regular / overflow past i32.
inline int32_t file_byte_size_value(String path) {
  try {
    std::error_code error_code;
    const auto size = std::filesystem::file_size(
      path.as_std_string(), error_code
    );
    if (error_code) {
      return -1;
    }
    if (size > static_cast<std::uintmax_t>(INT32_MAX)) {
      return -1;
    }
    return static_cast<int32_t>(size);
  } catch (...) {
    return -1;
  }
}

inline Array<String> list_dir_names_value(String path) {
  try {
    return list_dir_names(path);
  } catch (...) {
    return Array<String>{};
  }
}

inline Array<int32_t> list_dir_is_directory_value(String path) {
  try {
    return list_dir_is_directory_flags(path);
  } catch (...) {
    return Array<int32_t>{};
  }
}

inline String make_temp_directory_value(String prefix) {
  return make_temp_directory(prefix);
}

} // namespace file
} // namespace mlc
