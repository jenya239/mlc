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
mlc::String find_project_root(mlc::String from_path) noexcept;
int string_index_of(mlc::String haystack, mlc::String needle) noexcept;
bool path_has_prefix(mlc::String path, mlc::String prefix) noexcept;
bool is_package_name_charset(mlc::String package_name) noexcept;
mlc::String first_path_segment(mlc::String import_path) noexcept;
mlc::String path_after_first_segment(mlc::String import_path) noexcept;
bool manifest_lists_dependency(mlc::String manifest_text, mlc::String package_name) noexcept;
mlc::String append_mlc_extension_if_needed(mlc::String module_path) noexcept;
mlc::String try_resolve_package_import(mlc::String base_path, mlc::String import_path) noexcept;
mlc::String resolve_import_path(mlc::String base_path, mlc::String import_path) noexcept;

} // namespace path_normalize

#endif // PATH_NORMALIZE_HPP
