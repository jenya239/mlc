#pragma once

#include "mlc/core/array.hpp"
#include "mlc/core/string.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace mlc {
namespace file_columns {

inline int32_t& bound_list() {
  static int32_t list_index = -1;
  return list_index;
}

inline std::vector<std::string>& size_column() {
  static std::vector<std::string> values;
  return values;
}

inline std::vector<std::string>& date_column() {
  static std::vector<std::string> values;
  return values;
}

inline std::vector<std::string> copy_column(const Array<String>& column) {
  std::vector<std::string> values;
  values.reserve(column.size());
  for (const String& item : column) {
    values.push_back(item.as_std_string());
  }
  return values;
}

inline void publish_value(int32_t list_index, Array<String> sizes, Array<String> dates) {
  bound_list() = list_index;
  size_column() = copy_column(sizes);
  date_column() = copy_column(dates);
}

inline String text_at(const std::vector<std::string>& column, int32_t list_index, int32_t row) {
  if (list_index != bound_list() || row < 0 || static_cast<size_t>(row) >= column.size()) {
    return String();
  }
  return String(column[static_cast<size_t>(row)]);
}

inline String size_at_value(int32_t list_index, int32_t row) {
  return text_at(size_column(), list_index, row);
}

inline String date_at_value(int32_t list_index, int32_t row) {
  return text_at(date_column(), list_index, row);
}

inline int32_t bound_value() {
  return bound_list();
}

}  // namespace file_columns
}  // namespace mlc
