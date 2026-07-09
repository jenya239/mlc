#ifndef PARAM_ANALYSIS_HPP
#define PARAM_ANALYSIS_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
namespace param_analysis {

bool is_shared_type(std::shared_ptr<ast::TypeExpr> type_expression) noexcept;
bool is_shared_array_type(std::shared_ptr<ast::TypeExpr> type_expression) noexcept;
bool is_shared_map_type(std::shared_ptr<ast::TypeExpr> type_expression) noexcept;
mlc::String array_elem_name(std::shared_ptr<ast::TypeExpr> type_expression) noexcept;
mlc::Array<mlc::String> collect_shared_params(mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept;
mlc::Array<mlc::String> collect_shared_array_params(mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept;
mlc::HashMap<mlc::String, mlc::String> collect_array_elem_types(mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept;
mlc::Array<mlc::String> collect_shared_map_params(mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept;
mlc::Array<mlc::String> collect_value_params(mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept;

} // namespace param_analysis

#endif // PARAM_ANALYSIS_HPP
