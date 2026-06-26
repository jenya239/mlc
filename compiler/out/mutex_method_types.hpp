#ifndef MUTEX_METHOD_TYPES_HPP
#define MUTEX_METHOD_TYPES_HPP

#include "mlc.hpp"
#include <variant>

namespace mutex_method_types {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

bool is_mutex_method(mlc::String method_name) noexcept;

} // namespace mutex_method_types

#endif // MUTEX_METHOD_TYPES_HPP
