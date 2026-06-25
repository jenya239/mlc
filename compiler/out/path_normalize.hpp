#ifndef PATH_NORMALIZE_HPP
#define PATH_NORMALIZE_HPP

#include "mlc.hpp"
#include <variant>

#include "header_import.hpp"

namespace path_normalize {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

bool driver_source_path_is_safe(mlc::String path) noexcept;

mlc::String dirname(mlc::String path) noexcept;

mlc::String resolve_dotdot(mlc::String path) noexcept;

mlc::String resolve_import_path(mlc::String base_path, mlc::String import_path) noexcept;

} // namespace path_normalize

#endif // PATH_NORMALIZE_HPP
