#include "param_analysis.hpp"

#include "ast.hpp"

namespace param_analysis {

using namespace ast;
using namespace ast_tokens;

bool is_shared_type(std::shared_ptr<ast::TypeExpr> type_expression) noexcept;

bool is_shared_array_type(std::shared_ptr<ast::TypeExpr> type_expression) noexcept;

bool is_shared_map_type(std::shared_ptr<ast::TypeExpr> type_expression) noexcept;

mlc::String array_elem_name(std::shared_ptr<ast::TypeExpr> type_expression) noexcept;

mlc::Array<mlc::String> collect_shared_params(mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept;

mlc::Array<mlc::String> collect_shared_array_params(mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept;

mlc::HashMap<mlc::String, mlc::String> collect_array_elem_types(mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept;

mlc::Array<mlc::String> collect_shared_map_params(mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept;

mlc::Array<mlc::String> collect_value_params(mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept;

bool is_shared_type(std::shared_ptr<ast::TypeExpr> type_expression) noexcept{return [&]() { if (std::holds_alternative<ast::TyGeneric>((*type_expression))) { auto _v_tygeneric = std::get<ast::TyGeneric>((*type_expression)); auto [type_name, _w0] = _v_tygeneric; return type_name == mlc::String("Shared"); } if (std::holds_alternative<ast::TyShared>((*type_expression))) { auto _v_tyshared = std::get<ast::TyShared>((*type_expression)); auto [_w0] = _v_tyshared; return true; } return false; }();}

bool is_shared_array_type(std::shared_ptr<ast::TypeExpr> type_expression) noexcept{return [&]() { if (std::holds_alternative<ast::TyArray>((*type_expression))) { auto _v_tyarray = std::get<ast::TyArray>((*type_expression)); auto [inner] = _v_tyarray; return is_shared_type(inner); } return false; }();}

bool is_shared_map_type(std::shared_ptr<ast::TypeExpr> type_expression) noexcept{return [&]() { if (std::holds_alternative<ast::TyGeneric>((*type_expression))) { auto _v_tygeneric = std::get<ast::TyGeneric>((*type_expression)); auto [type_name, type_arguments] = _v_tygeneric; return type_name == mlc::String("Map") && type_arguments.size() >= 2 && is_shared_type(type_arguments[1]); } return false; }();}

mlc::String array_elem_name(std::shared_ptr<ast::TypeExpr> type_expression) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::TyArray>((*type_expression))) { auto _v_tyarray = std::get<ast::TyArray>((*type_expression)); auto [inner] = _v_tyarray; return [&]() -> mlc::String { if (std::holds_alternative<ast::TyNamed>((*inner))) { auto _v_tynamed = std::get<ast::TyNamed>((*inner)); auto [type_name] = _v_tynamed; return type_name; } return mlc::String(""); }(); } return mlc::String(""); }();}

mlc::Array<mlc::String> collect_shared_params(mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept{return parameters.filter([](std::shared_ptr<ast::Param> parameter) mutable { return is_shared_type(parameter->type_value); }).map([](std::shared_ptr<ast::Param> parameter) mutable { return parameter->name; });}

mlc::Array<mlc::String> collect_shared_array_params(mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept{return parameters.filter([](std::shared_ptr<ast::Param> parameter) mutable { return is_shared_array_type(parameter->type_value); }).map([](std::shared_ptr<ast::Param> parameter) mutable { return parameter->name; });}

mlc::HashMap<mlc::String, mlc::String> collect_array_elem_types(mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept{
mlc::HashMap<mlc::String, mlc::String> result = mlc::HashMap<mlc::String, mlc::String>();
int parameter_index = 0;
while (parameter_index < parameters.size()){
{
mlc::String element_name = array_elem_name(parameters[parameter_index]->type_value);
if (element_name.length() > 0){
{
result.set(parameters[parameter_index]->name, element_name);
}
}
parameter_index = parameter_index + 1;
}
}
return result;
}

mlc::Array<mlc::String> collect_shared_map_params(mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept{return parameters.filter([](std::shared_ptr<ast::Param> parameter) mutable { return is_shared_map_type(parameter->type_value); }).map([](std::shared_ptr<ast::Param> parameter) mutable { return parameter->name; });}

mlc::Array<mlc::String> collect_value_params(mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept{return parameters.filter([](std::shared_ptr<ast::Param> parameter) mutable { return !is_shared_type(parameter->type_value) && !is_shared_array_type(parameter->type_value); }).map([](std::shared_ptr<ast::Param> parameter) mutable { return parameter->name; });}

} // namespace param_analysis
