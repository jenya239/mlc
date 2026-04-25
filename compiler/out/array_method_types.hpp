#ifndef ARRAY_METHOD_TYPES_HPP
#define ARRAY_METHOD_TYPES_HPP

#include "mlc.hpp"
#include <variant>

#include "registry.hpp"
#include "semantic_type_structure.hpp"

namespace array_method_types {

struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

bool is_array_hof_method(mlc::String method_name) noexcept;

int array_hof_expected_argument_count(mlc::String method_name) noexcept;

std::shared_ptr<registry::Type> array_hof_call_result_type(std::shared_ptr<registry::Type> element_type, mlc::String method_name, mlc::Array<std::shared_ptr<registry::Type>> argument_inferred_types) noexcept;

} // namespace array_method_types

#endif // ARRAY_METHOD_TYPES_HPP
