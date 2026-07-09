#ifndef SEND_SAFE_HPP
#define SEND_SAFE_HPP

#include "mlc.hpp"
#include <variant>

#include "registry.hpp"

namespace send_safe {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

bool type_is_send_safe(std::shared_ptr<registry::Type> type_value, registry::TypeRegistry registry) noexcept;

} // namespace send_safe

#endif // SEND_SAFE_HPP
