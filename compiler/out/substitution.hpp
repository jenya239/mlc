#ifndef SUBSTITUTION_HPP
#define SUBSTITUTION_HPP

#include "mlc.hpp"
#include <variant>

#include "registry.hpp"

namespace substitution {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution_from_generic(std::shared_ptr<registry::Type> type_value, registry::TypeRegistry registry) noexcept;

} // namespace substitution

#endif // SUBSTITUTION_HPP
