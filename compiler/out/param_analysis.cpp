#include "param_analysis.hpp"

#include "ast.hpp"

namespace param_analysis {

using namespace ast;
using namespace ast_tokens;

bool is_shared_type(std::shared_ptr<ast::TypeExpr> t) noexcept;

bool is_shared_array_type(std::shared_ptr<ast::TypeExpr> t) noexcept;

bool is_shared_map_type(std::shared_ptr<ast::TypeExpr> t) noexcept;

mlc::String array_elem_name(std::shared_ptr<ast::TypeExpr> t) noexcept;

mlc::Array<mlc::String> collect_shared_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;

mlc::Array<mlc::String> collect_shared_array_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;

mlc::HashMap<mlc::String, mlc::String> collect_array_elem_types(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;

mlc::Array<mlc::String> collect_shared_map_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;

mlc::Array<mlc::String> collect_value_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;

bool is_shared_type(std::shared_ptr<ast::TypeExpr> t) noexcept{return [&]() { if (std::holds_alternative<ast::TyGeneric>((*t))) { auto _v_tygeneric = std::get<ast::TyGeneric>((*t)); auto [n, _w0] = _v_tygeneric; return n == mlc::String("Shared"); } if (std::holds_alternative<ast::TyShared>((*t))) { auto _v_tyshared = std::get<ast::TyShared>((*t)); auto [_w0] = _v_tyshared; return true; } return false; }();}

bool is_shared_array_type(std::shared_ptr<ast::TypeExpr> t) noexcept{return [&]() { if (std::holds_alternative<ast::TyArray>((*t))) { auto _v_tyarray = std::get<ast::TyArray>((*t)); auto [inner] = _v_tyarray; return is_shared_type(inner); } return false; }();}

bool is_shared_map_type(std::shared_ptr<ast::TypeExpr> t) noexcept{return [&]() { if (std::holds_alternative<ast::TyGeneric>((*t))) { auto _v_tygeneric = std::get<ast::TyGeneric>((*t)); auto [n, targs] = _v_tygeneric; return n == mlc::String("Map") && targs.size() >= 2 && is_shared_type(targs[1]); } return false; }();}

mlc::String array_elem_name(std::shared_ptr<ast::TypeExpr> t) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::TyArray>((*t))) { auto _v_tyarray = std::get<ast::TyArray>((*t)); auto [inner] = _v_tyarray; return [&]() -> mlc::String { if (std::holds_alternative<ast::TyNamed>((*inner))) { auto _v_tynamed = std::get<ast::TyNamed>((*inner)); auto [n] = _v_tynamed; return n; } return mlc::String(""); }(); } return mlc::String(""); }();}

mlc::Array<mlc::String> collect_shared_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept{
mlc::Array<mlc::String> result = {};
int i = 0;
while (i < params.size()){
{
if (is_shared_type(params[i]->typ)){
{
result.push_back(params[i]->name);
}
}
i = i + 1;
}
}
return result;
}

mlc::Array<mlc::String> collect_shared_array_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept{
mlc::Array<mlc::String> result = {};
int i = 0;
while (i < params.size()){
{
if (is_shared_array_type(params[i]->typ)){
{
result.push_back(params[i]->name);
}
}
i = i + 1;
}
}
return result;
}

mlc::HashMap<mlc::String, mlc::String> collect_array_elem_types(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept{
mlc::HashMap<mlc::String, mlc::String> result = mlc::HashMap<mlc::String, mlc::String>();
int i = 0;
while (i < params.size()){
{
mlc::String en = array_elem_name(params[i]->typ);
if (en.length() > 0){
{
result.set(params[i]->name, en);
}
}
i = i + 1;
}
}
return result;
}

mlc::Array<mlc::String> collect_shared_map_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept{
mlc::Array<mlc::String> result = {};
int i = 0;
while (i < params.size()){
{
if (is_shared_map_type(params[i]->typ)){
{
result.push_back(params[i]->name);
}
}
i = i + 1;
}
}
return result;
}

mlc::Array<mlc::String> collect_value_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept{
mlc::Array<mlc::String> result = {};
int i = 0;
while (i < params.size()){
{
if (!is_shared_type(params[i]->typ) && !is_shared_array_type(params[i]->typ)){
{
result.push_back(params[i]->name);
}
}
i = i + 1;
}
}
return result;
}

} // namespace param_analysis
