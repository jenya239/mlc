#ifndef VERIFY_SEMANTIC_IR_HPP
#define VERIFY_SEMANTIC_IR_HPP

#include "mlc.hpp"
#include <variant>

#include "registry.hpp"
#include "ast.hpp"
#include "semantic_ir.hpp"

namespace verify_semantic_ir {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

bool semantic_type_is_resolved(std::shared_ptr<registry::Type> type_value) noexcept;

mlc::Array<mlc::String> verify_semantic_ir_load_items(mlc::Array<semantic_ir::SemanticLoadItem> items) noexcept;

} // namespace verify_semantic_ir

#endif // VERIFY_SEMANTIC_IR_HPP
