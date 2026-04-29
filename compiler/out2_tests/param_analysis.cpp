#define main mlc_user_main
#include "param_analysis.hpp"

namespace param_analysis {

bool is_shared_type(std::shared_ptr<ast::TypeExpr> t) noexcept{
return std::visit(overloaded{[&](const ast::TyGeneric& tyGeneric) { auto [n, __1] = tyGeneric; return (n == mlc::String("Shared", 6)); },
[&](const ast::TyShared& tyShared) { auto [__0] = tyShared; return true; },
[&](const auto& __v) { return false; }
}, (*t));
}
bool is_shared_array_type(std::shared_ptr<ast::TypeExpr> t) noexcept{
return std::visit(overloaded{[&](const ast::TyArray& tyArray) { auto [inner] = tyArray; return is_shared_type(inner); },
[&](const auto& __v) { return false; }
}, (*t));
}
bool is_shared_map_type(std::shared_ptr<ast::TypeExpr> t) noexcept{
return std::visit(overloaded{[&](const ast::TyGeneric& tyGeneric) { auto [n, targs] = tyGeneric; return (((n == mlc::String("Map", 3)) && (targs.length() >= 2)) && is_shared_type(targs[1])); },
[&](const auto& __v) { return false; }
}, (*t));
}
mlc::String array_elem_name(std::shared_ptr<ast::TypeExpr> t) noexcept{
return std::visit(overloaded{[&](const ast::TyArray& tyArray) { auto [inner] = tyArray; return std::visit(overloaded{[&](const ast::TyNamed& tyNamed) { auto [n] = tyNamed; return n; },
[&](const auto& __v) { return mlc::String("", 0); }
}, (*inner)); },
[&](const auto& __v) { return mlc::String("", 0); }
}, (*t));
}
mlc::Array<mlc::String> collect_shared_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept{
auto result = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < params.length())) {
if (is_shared_type(params[i]->typ)) {
result.push_back(params[i]->name);
}
i = (i + 1);
}
return result;
}
mlc::Array<mlc::String> collect_shared_array_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept{
auto result = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < params.length())) {
if (is_shared_array_type(params[i]->typ)) {
result.push_back(params[i]->name);
}
i = (i + 1);
}
return result;
}
mlc::HashMap<mlc::String, mlc::String> collect_array_elem_types(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept{
auto result = mlc::HashMap<mlc::String, mlc::String>();
auto i = 0;
while ((i < params.length())) {
auto en = array_elem_name(params[i]->typ);
if ((en.length() > 0)) {
result.set(params[i]->name, en);
}
i = (i + 1);
}
return result;
}
mlc::Array<mlc::String> collect_shared_map_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept{
auto result = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < params.length())) {
if (is_shared_map_type(params[i]->typ)) {
result.push_back(params[i]->name);
}
i = (i + 1);
}
return result;
}
mlc::Array<mlc::String> collect_value_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept{
auto result = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < params.length())) {
if (((!is_shared_type(params[i]->typ)) && (!is_shared_array_type(params[i]->typ)))) {
result.push_back(params[i]->name);
}
i = (i + 1);
}
return result;
}

} // namespace param_analysis
