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

bool types_structurally_equal(std::shared_ptr<registry::Type> left, std::shared_ptr<registry::Type> right) noexcept;

bool type_is_unknown(std::shared_ptr<registry::Type> type_value) noexcept;

bool type_is_array(std::shared_ptr<registry::Type> type_value) noexcept;

std::shared_ptr<registry::Type> array_element_type_from_array_type(std::shared_ptr<registry::Type> array_type) noexcept;

bool type_is_shared_pointer(std::shared_ptr<registry::Type> type_value) noexcept;

std::shared_ptr<registry::Type> shared_pointer_inner_type(std::shared_ptr<registry::Type> shared_pointer_type) noexcept;

bool type_is_function(std::shared_ptr<registry::Type> type_value) noexcept;

mlc::Array<std::shared_ptr<registry::Type>> empty_type_parameter_list() noexcept;

mlc::Array<std::shared_ptr<registry::Type>> function_parameter_list(std::shared_ptr<registry::Type> function_type) noexcept;

} // namespace semantic_type_structure

#endif // SEMANTIC_TYPE_STRUCTURE_HPP
