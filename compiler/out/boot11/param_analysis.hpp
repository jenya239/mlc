#ifndef PARAM_ANALYSIS_HPP
#define PARAM_ANALYSIS_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"

namespace param_analysis {

bool is_shared_type(std::shared_ptr<ast::TypeExpr> t) noexcept;
bool is_shared_array_type(std::shared_ptr<ast::TypeExpr> t) noexcept;
bool is_shared_map_type(std::shared_ptr<ast::TypeExpr> t) noexcept;
mlc::String array_elem_name(std::shared_ptr<ast::TypeExpr> t) noexcept;
mlc::Array<mlc::String> collect_shared_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;
mlc::Array<mlc::String> collect_shared_array_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;
mlc::HashMap<mlc::String, mlc::String> collect_array_elem_types(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;
mlc::Array<mlc::String> collect_shared_map_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;
mlc::Array<mlc::String> collect_value_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;

} // namespace param_analysis

#endif // PARAM_ANALYSIS_HPP
