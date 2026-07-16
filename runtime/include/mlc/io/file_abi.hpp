#pragma once

// By-value String FFI for mlcc (TRACK_EDITOR_MVP STEP=19).
// Wraps mlc::file::* which take const String&.

#include "mlc/io/file.hpp"

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

inline bool remove_file_value(String path) {
  return remove_file(path);
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

} // namespace file
} // namespace mlc
