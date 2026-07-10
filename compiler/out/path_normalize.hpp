#ifndef PATH_NORMALIZE_HPP
#define PATH_NORMALIZE_HPP

#include "mlc.hpp"
#include <variant>
#include "header_import.hpp"
namespace path_normalize {

bool path_character_is_safe(mlc::String character) noexcept;
bool path_contains_parent_segment(mlc::String path) noexcept;
bool driver_source_path_is_safe(mlc::String path) noexcept;
mlc::String dirname(mlc::String path) noexcept;
mlc::String resolve_dotdot(mlc::String path) noexcept;
bool import_path_contains_slash(mlc::String import_path) noexcept;
bool is_bare_module_import_name(mlc::String import_path) noexcept;
mlc::String stdlib_module_relative_path(mlc::String module_name) noexcept;
mlc::String parent_directory(mlc::String path) noexcept;
mlc::String find_stdlib_root(mlc::String from_path) noexcept;
mlc::String resolve_import_path(mlc::String base_path, mlc::String import_path) noexcept;

} // namespace path_normalize

#endif // PATH_NORMALIZE_HPP
