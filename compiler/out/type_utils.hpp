#ifndef TYPE_UTILS_HPP
#define TYPE_UTILS_HPP

#include "mlc.hpp"
#include <variant>

#include "registry.hpp"
#include "semantic_type_structure.hpp"

namespace type_utils {

struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

mlc::String type_description(std::shared_ptr<registry::Type> type_value) noexcept;

bool type_is_i32(std::shared_ptr<registry::Type> type_value) noexcept;

bool type_is_bool(std::shared_ptr<registry::Type> type_value) noexcept;

bool type_is_string(std::shared_ptr<registry::Type> type_value) noexcept;

bool types_allowed_for_binary_plus(std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type) noexcept;

bool types_allowed_for_binary_int_only(std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type) noexcept;

bool types_allowed_for_ordering(std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type) noexcept;

bool types_allowed_for_logical_and_or(std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type) noexcept;

bool receiver_type_is_map(std::shared_ptr<registry::Type> receiver_type) noexcept;

std::shared_ptr<registry::Type> binary_operation_result_type(mlc::String operation, std::shared_ptr<registry::Type> left_type) noexcept;

std::shared_ptr<registry::Type> builtin_method_return_type(mlc::String method_name) noexcept;

std::shared_ptr<registry::Type> field_type_from_object(std::shared_ptr<registry::Type> object_type, mlc::String field_name, registry::TypeRegistry registry) noexcept;

int builtin_method_expected_argument_count(mlc::String method_name) noexcept;

} // namespace type_utils

#endif // TYPE_UTILS_HPP
