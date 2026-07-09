#include "pattern_env.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "semantic_type_structure.hpp"

namespace pattern_env {

using namespace ast;
using namespace registry;
using namespace semantic_type_structure;
using namespace ast_tokens;

struct Constructor_pattern_environment_fold_state {mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> environment;int next_constructor_subpattern_index;};

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> env_for_pattern_with_type(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment, std::shared_ptr<ast::Pattern> pattern, std::shared_ptr<registry::Type> type_value, registry::TypeRegistry registry) noexcept;

pattern_env::Constructor_pattern_environment_fold_state constructor_subpattern_environment_fold_step(pattern_env::Constructor_pattern_environment_fold_state accumulator, std::shared_ptr<ast::Pattern> sub_pattern_value, mlc::Array<std::shared_ptr<registry::Type>> parameter_types, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> parameter_type_substitution, registry::TypeRegistry registry) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> environment_after_all_constructor_subpatterns(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> initial_environment, mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, mlc::Array<std::shared_ptr<registry::Type>> parameter_types, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> parameter_type_substitution, registry::TypeRegistry registry) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> env_for_pattern(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment, std::shared_ptr<ast::Pattern> pattern, registry::TypeRegistry registry) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> env_for_pattern_substituted(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment, std::shared_ptr<ast::Pattern> pattern, registry::TypeRegistry registry, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution, std::shared_ptr<registry::Type> scrutinee_type) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> env_for_pattern_with_type(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment, std::shared_ptr<ast::Pattern> pattern, std::shared_ptr<registry::Type> type_value, registry::TypeRegistry registry) noexcept{
return [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { if (std::holds_alternative<ast::PatternIdent>((*pattern))) { auto _v_patternident = std::get<ast::PatternIdent>((*pattern)); auto [binding_name, _w0] = _v_patternident; return [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { 
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> environment = type_environment;
  environment.set(binding_name, type_value);
  return environment;
 }(); } if (std::holds_alternative<ast::PatternOr>((*pattern))) { auto _v_patternor = std::get<ast::PatternOr>((*pattern)); auto [alts, _w0] = _v_patternor; return alts.size() > 0 ? env_for_pattern_with_type(type_environment, alts[0], type_value, registry) : type_environment; } return env_for_pattern(type_environment, pattern, registry); }();
}

pattern_env::Constructor_pattern_environment_fold_state constructor_subpattern_environment_fold_step(pattern_env::Constructor_pattern_environment_fold_state accumulator, std::shared_ptr<ast::Pattern> sub_pattern_value, mlc::Array<std::shared_ptr<registry::Type>> parameter_types, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> parameter_type_substitution, registry::TypeRegistry registry) noexcept{
int current_subpattern_index = accumulator.next_constructor_subpattern_index;
std::shared_ptr<registry::Type> raw_constructor_parameter_type = current_subpattern_index < parameter_types.size() ? parameter_types[current_subpattern_index] : std::make_shared<registry::Type>((registry::TUnknown{}));
std::shared_ptr<registry::Type> resolved_constructor_subpattern_type = semantic_type_structure::substitute_type(raw_constructor_parameter_type, parameter_type_substitution);
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> next_environment_after_subpattern = env_for_pattern_with_type(accumulator.environment, sub_pattern_value, resolved_constructor_subpattern_type, registry);
return pattern_env::Constructor_pattern_environment_fold_state{next_environment_after_subpattern, current_subpattern_index + 1};
}

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> environment_after_all_constructor_subpatterns(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> initial_environment, mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, mlc::Array<std::shared_ptr<registry::Type>> parameter_types, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> parameter_type_substitution, registry::TypeRegistry registry) noexcept{return sub_patterns.fold(pattern_env::Constructor_pattern_environment_fold_state{initial_environment, 0}, [parameter_types, parameter_type_substitution, registry](pattern_env::Constructor_pattern_environment_fold_state accumulator, std::shared_ptr<ast::Pattern> sub_pattern_value) mutable { return constructor_subpattern_environment_fold_step(accumulator, sub_pattern_value, parameter_types, parameter_type_substitution, registry); }).environment;}

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> env_for_pattern(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment, std::shared_ptr<ast::Pattern> pattern, registry::TypeRegistry registry) noexcept{
return [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { if (std::holds_alternative<ast::PatternIdent>((*pattern))) { auto _v_patternident = std::get<ast::PatternIdent>((*pattern)); auto [binding_name, _w0] = _v_patternident; return [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { 
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> environment = type_environment;
  environment.set(binding_name, std::make_shared<registry::Type>((registry::TUnknown{})));
  return environment;
 }(); } if (std::holds_alternative<ast::PatternCtor>((*pattern))) { auto _v_patternctor = std::get<ast::PatternCtor>((*pattern)); auto [constructor_name, sub_patterns, _w0] = _v_patternctor; return [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> parameter_types = registry::TypeRegistry_ctor_params_for(registry, constructor_name);
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> empty_type_parameter_substitution = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
  return environment_after_all_constructor_subpatterns(type_environment, sub_patterns, parameter_types, empty_type_parameter_substitution, registry);
 }(); } if (std::holds_alternative<ast::PatternOr>((*pattern))) { auto _v_patternor = std::get<ast::PatternOr>((*pattern)); auto [alts, _w0] = _v_patternor; return alts.size() > 0 ? env_for_pattern(type_environment, alts[0], registry) : type_environment; } return type_environment; }();
}

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> env_for_pattern_substituted(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment, std::shared_ptr<ast::Pattern> pattern, registry::TypeRegistry registry, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution, std::shared_ptr<registry::Type> scrutinee_type) noexcept{
return [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { if (std::holds_alternative<ast::PatternIdent>((*pattern))) { auto _v_patternident = std::get<ast::PatternIdent>((*pattern)); auto [binding_name, _w0] = _v_patternident; return [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { 
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> environment = type_environment;
  environment.set(binding_name, semantic_type_structure::substitute_type(scrutinee_type, substitution));
  return environment;
 }(); } if (std::holds_alternative<ast::PatternCtor>((*pattern))) { auto _v_patternctor = std::get<ast::PatternCtor>((*pattern)); auto [constructor_name, sub_patterns, _w0] = _v_patternctor; return [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> parameter_types = registry::TypeRegistry_ctor_params_for(registry, constructor_name);
  return environment_after_all_constructor_subpatterns(type_environment, sub_patterns, parameter_types, substitution, registry);
 }(); } if (std::holds_alternative<ast::PatternOr>((*pattern))) { auto _v_patternor = std::get<ast::PatternOr>((*pattern)); auto [alts, _w0] = _v_patternor; return alts.size() > 0 ? env_for_pattern_substituted(type_environment, alts[0], registry, substitution, scrutinee_type) : type_environment; } return type_environment; }();
}

} // namespace pattern_env
