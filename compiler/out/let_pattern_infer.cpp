#include "let_pattern_infer.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "pattern_env.hpp"

namespace let_pattern_infer {

using namespace ast;
using namespace registry;
using namespace pattern_env;
using namespace ast_tokens;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> build_let_substitution(std::shared_ptr<registry::Type> value_type, registry::TypeRegistry registry) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> apply_let_pattern_to_env(std::shared_ptr<ast::Pat> pat, std::shared_ptr<registry::Type> value_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env, registry::TypeRegistry registry) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> infer_let_pattern_env(std::shared_ptr<ast::Pat> pat, std::shared_ptr<registry::Type> value_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env, registry::TypeRegistry registry) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> build_let_substitution(std::shared_ptr<registry::Type> value_type, registry::TypeRegistry registry) noexcept{
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
[&]() -> void { if (std::holds_alternative<registry::TGeneric>((*value_type))) { auto _v_tgeneric = std::get<registry::TGeneric>((*value_type)); auto [type_name, type_args] = _v_tgeneric; return [&]() { 
  mlc::Array<mlc::String> param_names = registry::TypeRegistry_algebraic_decl_type_parameter_names_for(registry, type_name);
  int i = 0;
  return [&]() { 
  while (i < param_names.size() && i < type_args.size()){
{
substitution.set(param_names[i], type_args[i]);
i = i + 1;
}
}
 }();
 }(); } return; }();
return substitution;
}

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> apply_let_pattern_to_env(std::shared_ptr<ast::Pat> pat, std::shared_ptr<registry::Type> value_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env, registry::TypeRegistry registry) noexcept{
return [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { if (std::holds_alternative<ast::PatWild>((*pat))) { auto _v_patwild = std::get<ast::PatWild>((*pat)); auto [_w0] = _v_patwild; return base_env; } if (std::holds_alternative<ast::PatIdent>((*pat))) { auto _v_patident = std::get<ast::PatIdent>((*pat)); auto [n, _w0] = _v_patident; return [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { 
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> u = base_env;
  u.set(n, value_type);
  return u;
 }(); } if (std::holds_alternative<ast::PatTuple>((*pat))) { auto _v_pattuple = std::get<ast::PatTuple>((*pat)); auto [subs, _w0] = _v_pattuple; return [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { if (std::holds_alternative<registry::TTuple>((*value_type))) { auto _v_ttuple = std::get<registry::TTuple>((*value_type)); auto [ts] = _v_ttuple; return subs.size() != ts.size() ? base_env : [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { 
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> acc = base_env;
  int j = 0;
  while (j < subs.size()){
{
acc = apply_let_pattern_to_env(subs[j], ts[j], acc, registry);
j = j + 1;
}
}
  return acc;
 }(); } if (std::holds_alternative<registry::TPair>((*value_type))) { auto _v_tpair = std::get<registry::TPair>((*value_type)); auto [fst, snd] = _v_tpair; return subs.size() != 2 ? base_env : [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { 
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> e0 = apply_let_pattern_to_env(subs[0], fst, base_env, registry);
  return apply_let_pattern_to_env(subs[1], snd, e0, registry);
 }(); } return base_env; }(); } if (std::holds_alternative<ast::PatArray>((*pat))) { auto _v_patarray = std::get<ast::PatArray>((*pat)); auto [subs, rest, _w0] = _v_patarray; return [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { if (std::holds_alternative<registry::TArray>((*value_type))) { auto _v_tarray = std::get<registry::TArray>((*value_type)); auto [et] = _v_tarray; return [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { 
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> acc = base_env;
  int i = 0;
  while (i < subs.size()){
{
acc = apply_let_pattern_to_env(subs[i], et, acc, registry);
i = i + 1;
}
}
  return rest != mlc::String("") ? [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { 
  acc.set(rest, value_type);
  return acc;
 }() : acc;
 }(); } return base_env; }(); } if (std::holds_alternative<ast::PatRecord>((*pat))) { auto _v_patrecord = std::get<ast::PatRecord>((*pat)); auto [rec_name, field_pats, _w0] = _v_patrecord; return rec_name != mlc::String("") ? base_env : [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { 
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> acc = base_env;
  int i = 0;
  while (i < field_pats.size()){
{
std::shared_ptr<ast::Pat> fp = field_pats[i];
mlc::String field_name = [&]() -> mlc::String { if (std::holds_alternative<ast::PatIdent>((*fp))) { auto _v_patident = std::get<ast::PatIdent>((*fp)); auto [n, _w0] = _v_patident; return n; } return mlc::String(""); }();
if (field_name != mlc::String("")){
{
std::shared_ptr<registry::Type> ft = registry::field_type_from_object(value_type, field_name, registry);
acc = apply_let_pattern_to_env(fp, ft, acc, registry);
}
}
i = i + 1;
}
}
  return acc;
 }(); } if (std::holds_alternative<ast::PatCtor>((*pat))) { auto _v_patctor = std::get<ast::PatCtor>((*pat)); auto [_w0, _w1, _w2] = _v_patctor; return pattern_env::env_for_pattern_substituted(base_env, pat, registry, build_let_substitution(value_type, registry)); } return base_env; }();
}

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> infer_let_pattern_env(std::shared_ptr<ast::Pat> pat, std::shared_ptr<registry::Type> value_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env, registry::TypeRegistry registry) noexcept{return apply_let_pattern_to_env(pat, value_type, base_env, registry);}

} // namespace let_pattern_infer
