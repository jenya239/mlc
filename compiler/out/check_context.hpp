#ifndef CHECK_CONTEXT_HPP
#define CHECK_CONTEXT_HPP

#include "mlc.hpp"
#include <variant>

#include "registry.hpp"

namespace check_context {

struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

struct CheckContext {mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env;registry::TypeRegistry registry;mlc::String current_extend_type;};

check_context::CheckContext check_context_new(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment, registry::TypeRegistry registry) noexcept;

} // namespace check_context

#endif // CHECK_CONTEXT_HPP
