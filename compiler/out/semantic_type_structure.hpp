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

bool type_is_unknown(std::shared_ptr<registry::Type> type_value) noexcept;

bool types_structurally_equal(std::shared_ptr<registry::Type> left, std::shared_ptr<registry::Type> right) noexcept;

bool type_is_array(std::shared_ptr<registry::Type> type_value) noexcept;

std::shared_ptr<registry::Type> array_element_type_from_array_type(std::shared_ptr<registry::Type> array_type) noexcept;

bool type_is_shared_pointer(std::shared_ptr<registry::Type> type_value) noexcept;

std::shared_ptr<registry::Type> shared_pointer_inner_type(std::shared_ptr<registry::Type> shared_pointer_type) noexcept;

bool type_is_function(std::shared_ptr<registry::Type> type_value) noexcept;

mlc::Array<std::shared_ptr<registry::Type>> empty_type_parameter_list() noexcept;

std::shared_ptr<registry::Type> function_return_type(std::shared_ptr<registry::Type> function_type) noexcept;

mlc::Array<std::shared_ptr<registry::Type>> function_parameter_list(std::shared_ptr<registry::Type> function_type) noexcept;

bool type_is_i32(std::shared_ptr<registry::Type> type_value) noexcept;

bool type_is_bool(std::shared_ptr<registry::Type> type_value) noexcept;

bool type_is_string(std::shared_ptr<registry::Type> type_value) noexcept;

bool receiver_type_is_map(std::shared_ptr<registry::Type> receiver_type) noexcept;

bool is_numeric_type(std::shared_ptr<registry::Type> t) noexcept;

std::shared_ptr<registry::Type> binary_operation_result_type(mlc::String operation, std::shared_ptr<registry::Type> left_type) noexcept;

mlc::String type_description(std::shared_ptr<registry::Type> type_value) noexcept;

bool types_allowed_for_binary_plus(std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type) noexcept;

bool types_allowed_for_binary_int_only(std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type) noexcept;

bool types_allowed_for_ordering(std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type) noexcept;

bool types_allowed_for_logical_and_or(std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type) noexcept;

std::shared_ptr<registry::Type> builtin_method_return_type(mlc::String method_name) noexcept;

int builtin_method_expected_argument_count(mlc::String method_name) noexcept;

std::shared_ptr<registry::Type> substitute_type(std::shared_ptr<registry::Type> type_value, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution) noexcept;

mlc::String operator_method_for(mlc::String operation) noexcept;

} // namespace semantic_type_structure

#endif // SEMANTIC_TYPE_STRUCTURE_HPP
