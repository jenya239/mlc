#pragma once

// By-value String FFI for mlcc (TRACK_EDITOR_MVP STEP=19).
// Wraps mlc::file::* which take const String&.

#include "mlc/io/file.hpp"
#include <cstdint>
#include <cstdio>
#include <ctime>
#include <sys/stat.h>
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

inline bool rename_file_value(String old_path, String new_path) {
  return rename_file(old_path, new_path);
}

inline bool remove_tree_value(String path) {
  try {
    std::error_code error_code;
    const auto removed = std::filesystem::remove_all(path.as_std_string(), error_code);
    if (error_code) {
      return false;
    }
    return removed > 0;
  } catch (...) {
    return false;
  }
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

inline String file_read_range_value(String path, int32_t offset, int32_t length) {
  if (offset < 0 || length <= 0) {
    return String("");
  }
  try {
    std::ifstream input(path.as_std_string(), std::ios::binary);
    if (!input) {
      return String("");
    }
    input.seekg(static_cast<std::streamoff>(offset), std::ios::beg);
    if (!input) {
      return String("");
    }
    std::string buffer(static_cast<std::size_t>(length), '\0');
    input.read(buffer.data(), static_cast<std::streamsize>(length));
    const auto read_count = input.gcount();
    if (read_count <= 0) {
      return String("");
    }
    buffer.resize(static_cast<std::size_t>(read_count));
    return String(buffer);
  } catch (...) {
    return String("");
  }
}

inline bool file_write_range_value(String path, int32_t offset, String content) {
  if (offset < 0) {
    return false;
  }
  try {
    std::fstream output(
      path.as_std_string(),
      std::ios::binary | std::ios::in | std::ios::out
    );
    if (!output) {
      return false;
    }
    output.seekp(static_cast<std::streamoff>(offset), std::ios::beg);
    if (!output) {
      return false;
    }
    const std::string bytes = content.as_std_string();
    output.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
    return static_cast<bool>(output);
  } catch (...) {
    return false;
  }
}

// Seconds since the Unix epoch, or -1 when the file cannot be stated.
// i32 saturates in 2038; a failed stat must stay distinguishable from a real stamp.
inline int32_t file_modified_seconds_value(String path) {
  struct stat file_status;
  if (stat(path.as_std_string().c_str(), &file_status) != 0) {
    return -1;
  }
  if (file_status.st_mtime < 0 || file_status.st_mtime > static_cast<time_t>(INT32_MAX)) {
    return -1;
  }
  return static_cast<int32_t>(file_status.st_mtime);
}

// Compact local stamp "08.11 20:47". Empty when the seconds are not a real stamp.
inline String format_local_stamp_value(int32_t seconds) {
  if (seconds < 0) {
    return String();
  }
  const time_t stamp = static_cast<time_t>(seconds);
  struct tm local_time;
  if (localtime_r(&stamp, &local_time) == nullptr) {
    return String();
  }
  char buffer[16];
  const int written = std::snprintf(
    buffer,
    sizeof(buffer),
    "%02d.%02d %02d:%02d",
    local_time.tm_mday,
    local_time.tm_mon + 1,
    local_time.tm_hour,
    local_time.tm_min
  );
  if (written <= 0) {
    return String();
  }
  return String(buffer);
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
