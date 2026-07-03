#ifndef CPP_NAMING_HPP
#define CPP_NAMING_HPP

#include "mlc.hpp"
#include <variant>
namespace cpp_naming {

mlc::String path_to_module_base(mlc::String path) noexcept;
bool cpp_keyword(mlc::String name) noexcept;
mlc::String cpp_safe(mlc::String name) noexcept;
mlc::String lower_first(mlc::String name) noexcept;
mlc::String map_method(mlc::String method_name) noexcept;
mlc::String map_builtin(mlc::String name) noexcept;
mlc::String map_builtin_identifier_reference(mlc::String name) noexcept;
mlc::String escape_str(mlc::String input) noexcept;
mlc::String template_prefix(mlc::Array<mlc::String> type_params) noexcept;
mlc::String include_lines(mlc::Array<mlc::String> import_paths) noexcept;
bool is_stdlib_import_path(mlc::String path) noexcept;
mlc::Array<mlc::String> using_namespace_names(mlc::Array<mlc::String> import_paths) noexcept;
mlc::String using_namespace_lines(mlc::Array<mlc::String> import_paths) noexcept;

} // namespace cpp_naming

#endif // CPP_NAMING_HPP
