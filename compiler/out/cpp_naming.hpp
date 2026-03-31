#ifndef CPP_NAMING_HPP
#define CPP_NAMING_HPP

#include "mlc.hpp"
#include <variant>

namespace cpp_naming {

struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

mlc::String path_to_module_base(mlc::String path) noexcept;

bool cpp_keyword(mlc::String name) noexcept;

mlc::String cpp_safe(mlc::String name) noexcept;

mlc::String lower_first(mlc::String name) noexcept;

mlc::String map_method(mlc::String method_name) noexcept;

mlc::String map_builtin(mlc::String name) noexcept;

mlc::String escape_str(mlc::String input) noexcept;

mlc::String template_prefix(mlc::Array<mlc::String> type_params) noexcept;

mlc::String include_lines(mlc::Array<mlc::String> import_paths) noexcept;

} // namespace cpp_naming

#endif // CPP_NAMING_HPP
