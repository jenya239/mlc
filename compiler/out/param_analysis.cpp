#define main mlc_user_main
#include "param_analysis.hpp"

#include "ast.hpp"

namespace param_analysis {

using namespace ast;
using namespace ast_tokens;

bool is_shared_type(std::shared_ptr<ast::TypeExpr> type_expression) noexcept{
  return [&]() -> bool {
auto __match_subject = type_expression;
if (std::holds_alternative<ast::TyGeneric>((*__match_subject))) {
const ast::TyGeneric& tyGeneric = std::get<ast::TyGeneric>((*__match_subject));
auto [type_name, __1] = tyGeneric; return (type_name == mlc::String("Shared", 6));
}
if (std::holds_alternative<ast::TyShared>((*__match_subject))) {
const ast::TyShared& tyShared = std::get<ast::TyShared>((*__match_subject));
auto [__0] = tyShared; return true;
}
return false;
std::abort();
}();
}
bool is_shared_array_type(std::shared_ptr<ast::TypeExpr> type_expression) noexcept{
  return [&]() -> bool {
auto __match_subject = type_expression;
if (std::holds_alternative<ast::TyArray>((*__match_subject))) {
const ast::TyArray& tyArray = std::get<ast::TyArray>((*__match_subject));
auto [inner] = tyArray; return is_shared_type(inner);
}
return false;
std::abort();
}();
}
bool is_shared_map_type(std::shared_ptr<ast::TypeExpr> type_expression) noexcept{
  return [&]() -> bool {
auto __match_subject = type_expression;
if (std::holds_alternative<ast::TyGeneric>((*__match_subject))) {
const ast::TyGeneric& tyGeneric = std::get<ast::TyGeneric>((*__match_subject));
auto [type_name, type_arguments] = tyGeneric; return (((type_name == mlc::String("Map", 3)) && (type_arguments.length() >= 2)) && is_shared_type(type_arguments[1]));
}
return false;
std::abort();
}();
}
mlc::String array_elem_name(std::shared_ptr<ast::TypeExpr> type_expression) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = type_expression;
if (std::holds_alternative<ast::TyArray>((*__match_subject))) {
const ast::TyArray& tyArray = std::get<ast::TyArray>((*__match_subject));
auto [inner] = tyArray; return [&]() -> mlc::String {
auto __match_subject = inner;
if (std::holds_alternative<ast::TyNamed>((*__match_subject))) {
const ast::TyNamed& tyNamed = std::get<ast::TyNamed>((*__match_subject));
auto [type_name] = tyNamed; return type_name;
}
return mlc::String("", 0);
std::abort();
}();
}
return mlc::String("", 0);
std::abort();
}();
}
mlc::Array<mlc::String> collect_shared_params(mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept{
  return parameters.filter([=](std::shared_ptr<ast::Param> parameter) mutable { return is_shared_type(parameter->type_value); }).map([=](std::shared_ptr<ast::Param> parameter) mutable { return parameter->name; });
}
mlc::Array<mlc::String> collect_shared_array_params(mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept{
  return parameters.filter([=](std::shared_ptr<ast::Param> parameter) mutable { return is_shared_array_type(parameter->type_value); }).map([=](std::shared_ptr<ast::Param> parameter) mutable { return parameter->name; });
}
mlc::HashMap<mlc::String, mlc::String> collect_array_elem_types(mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept{
  auto result = mlc::HashMap<mlc::String, mlc::String>();
  auto parameter_index = 0;
  while ((parameter_index < parameters.length()))   {
    auto element_name = array_elem_name(parameters[parameter_index]->type_value);
    if ((element_name.length() > 0))     {
      result.set(parameters[parameter_index]->name, element_name);
    }
    (parameter_index = (parameter_index + 1));
  }
  return result;
}
mlc::Array<mlc::String> collect_shared_map_params(mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept{
  return parameters.filter([=](std::shared_ptr<ast::Param> parameter) mutable { return is_shared_map_type(parameter->type_value); }).map([=](std::shared_ptr<ast::Param> parameter) mutable { return parameter->name; });
}
mlc::Array<mlc::String> collect_value_params(mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept{
  return parameters.filter([=](std::shared_ptr<ast::Param> parameter) mutable { return ((!is_shared_type(parameter->type_value)) && (!is_shared_array_type(parameter->type_value))); }).map([=](std::shared_ptr<ast::Param> parameter) mutable { return parameter->name; });
}

} // namespace param_analysis
