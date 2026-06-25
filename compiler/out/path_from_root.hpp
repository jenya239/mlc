#ifndef PATH_FROM_ROOT_HPP
#define PATH_FROM_ROOT_HPP

#include "mlc.hpp"
#include <variant>

namespace path_from_root {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

bool test_relative_path_character_is_safe(mlc::String character) noexcept;

bool test_relative_path_is_safe(mlc::String relative_path) noexcept;

mlc::String resolve_test_fixture_path(mlc::String relative_path) noexcept;

} // namespace path_from_root

#endif // PATH_FROM_ROOT_HPP
