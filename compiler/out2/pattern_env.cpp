#include "pattern_env.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "semantic_type_structure.hpp"

namespace pattern_env {

using namespace ast;
using namespace registry;
using namespace semantic_type_structure;
using namespace ast_tokens;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> env_for_pattern(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment, std::shared_ptr<ast::Pat> pattern, registry::TypeRegistry registry) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> env_for_pattern_with_type(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment, std::shared_ptr<ast::Pat> pattern, std::shared_ptr<registry::Type> type_value, registry::TypeRegistry registry) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> env_for_pattern_substituted(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment, std::shared_ptr<ast::Pat> pattern, registry::TypeRegistry registry, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> env_for_pattern(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment, std::shared_ptr<ast::Pat> pattern, registry::TypeRegistry registry) noexcept{
return [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { if (std::holds_alternative<ast::PatIdent>((*pattern))) { auto _v_patident = std::get<ast::PatIdent>((*pattern)); auto [binding_name, _w0] = _v_patident; return [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { 
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> environment = type_environment;
  environment.set(binding_name, std::make_shared<registry::Type>((registry::TUnknown{})));
  return environment;
 }(); } if (std::holds_alternative<ast::PatCtor>((*pattern))) { auto _v_patctor = std::get<ast::PatCtor>((*pattern)); auto [constructor_name, sub_patterns, _w0] = _v_patctor; return [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> parameter_types = registry::TypeRegistry_ctor_params_for(registry, constructor_name);
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> current_environment = type_environment;
  int sub_index = 0;
  while (sub_index < sub_patterns.size()){
{
std::shared_ptr<registry::Type> sub_type = sub_index < parameter_types.size() ? parameter_types[sub_index] : std::make_shared<registry::Type>((registry::TUnknown{}));
current_environment = env_for_pattern_with_type(current_environment, sub_patterns[sub_index], sub_type, registry);
sub_index = sub_index + 1;
}
}
  return current_environment;
 }(); } if (std::holds_alternative<ast::PatOr>((*pattern))) { auto _v_pator = std::get<ast::PatOr>((*pattern)); auto [alts, _w0] = _v_pator; return alts.size() > 0 ? env_for_pattern(type_environment, alts[0], registry) : type_environment; } return type_environment; }();
}

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> env_for_pattern_with_type(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment, std::shared_ptr<ast::Pat> pattern, std::shared_ptr<registry::Type> type_value, registry::TypeRegistry registry) noexcept{
return [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { if (std::holds_alternative<ast::PatIdent>((*pattern))) { auto _v_patident = std::get<ast::PatIdent>((*pattern)); auto [binding_name, _w0] = _v_patident; return [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { 
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> environment = type_environment;
  environment.set(binding_name, type_value);
  return environment;
 }(); } if (std::holds_alternative<ast::PatOr>((*pattern))) { auto _v_pator = std::get<ast::PatOr>((*pattern)); auto [alts, _w0] = _v_pator; return alts.size() > 0 ? env_for_pattern_with_type(type_environment, alts[0], type_value, registry) : type_environment; } return env_for_pattern(type_environment, pattern, registry); }();
}

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> env_for_pattern_substituted(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment, std::shared_ptr<ast::Pat> pattern, registry::TypeRegistry registry, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution) noexcept{
return [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { if (std::holds_alternative<ast::PatIdent>((*pattern))) { auto _v_patident = std::get<ast::PatIdent>((*pattern)); auto [binding_name, _w0] = _v_patident; return [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { 
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> environment = type_environment;
  environment.set(binding_name, std::make_shared<registry::Type>((registry::TUnknown{})));
  return environment;
 }(); } if (std::holds_alternative<ast::PatCtor>((*pattern))) { auto _v_patctor = std::get<ast::PatCtor>((*pattern)); auto [constructor_name, sub_patterns, _w0] = _v_patctor; return [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> parameter_types = registry::TypeRegistry_ctor_params_for(registry, constructor_name);
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> current_environment = type_environment;
  int sub_index = 0;
  while (sub_index < sub_patterns.size()){
{
std::shared_ptr<registry::Type> raw_type = sub_index < parameter_types.size() ? parameter_types[sub_index] : std::make_shared<registry::Type>((registry::TUnknown{}));
std::shared_ptr<registry::Type> sub_type = semantic_type_structure::substitute_type(raw_type, substitution);
current_environment = env_for_pattern_with_type(current_environment, sub_patterns[sub_index], sub_type, registry);
sub_index = sub_index + 1;
}
}
  return current_environment;
 }(); } if (std::holds_alternative<ast::PatOr>((*pattern))) { auto _v_pator = std::get<ast::PatOr>((*pattern)); auto [alts, _w0] = _v_pator; return alts.size() > 0 ? env_for_pattern_substituted(type_environment, alts[0], registry, substitution) : type_environment; } return type_environment; }();
}

} // namespace pattern_env
