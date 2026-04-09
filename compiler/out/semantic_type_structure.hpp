#ifndef SEMANTIC_TYPE_STRUCTURE_HPP
#define SEMANTIC_TYPE_STRUCTURE_HPP

#include "mlc.hpp"
#include <variant>

#include "registry.hpp"

namespace semantic_type_structure {

struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

std::shared_ptr<registry::Type> array_element_type_from_array_type(std::shared_ptr<registry::Type> array_type) noexcept;

bool type_is_shared_pointer(std::shared_ptr<registry::Type> type_value) noexcept;

std::shared_ptr<registry::Type> shared_pointer_inner_type(std::shared_ptr<registry::Type> shared_pointer_type) noexcept;

} // namespace semantic_type_structure

#endif // SEMANTIC_TYPE_STRUCTURE_HPP
