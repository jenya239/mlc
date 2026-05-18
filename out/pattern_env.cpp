#define main mlc_user_main
#include "pattern_env.hpp"

namespace pattern_env {

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> env_for_pattern_with_type(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment, std::shared_ptr<ast::Pat> pattern, std::shared_ptr<registry::Type> type_value, registry::TypeRegistry registry) noexcept{
return std::visit(overloaded{[&](const ast::PatIdent& patIdent) { auto [binding_name, __1] = patIdent; return [&]() {
auto environment = type_environment;
environment.set(binding_name, type_value);
return environment;
}(); },
[&](const ast::PatOr& patOr) { auto [alts, __1] = patOr; return ((alts.length() > 0) ? (env_for_pattern_with_type(type_environment, alts[0], type_value, registry)) : (type_environment)); },
[&](const auto& __v) { return env_for_pattern(type_environment, pattern, registry); }
}, (*pattern));
}
Constructor_pattern_environment_fold_state constructor_subpattern_environment_fold_step(Constructor_pattern_environment_fold_state accumulator, std::shared_ptr<ast::Pat> sub_pattern_value, mlc::Array<std::shared_ptr<registry::Type>> parameter_types, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> parameter_type_substitution, registry::TypeRegistry registry) noexcept{
auto current_subpattern_index = accumulator.next_constructor_subpattern_index;
auto raw_constructor_parameter_type = ((current_subpattern_index < parameter_types.length()) ? (parameter_types[current_subpattern_index]) : (std::make_shared<registry::Type>(registry::TUnknown{})));
auto resolved_constructor_subpattern_type = semantic_type_structure::substitute_type(raw_constructor_parameter_type, parameter_type_substitution);
auto next_environment_after_subpattern = env_for_pattern_with_type(accumulator.environment, sub_pattern_value, resolved_constructor_subpattern_type, registry);
return Constructor_pattern_environment_fold_state{next_environment_after_subpattern, (current_subpattern_index + 1)};
}
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> environment_after_all_constructor_subpatterns(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> initial_environment, mlc::Array<std::shared_ptr<ast::Pat>> sub_patterns, mlc::Array<std::shared_ptr<registry::Type>> parameter_types, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> parameter_type_substitution, registry::TypeRegistry registry) noexcept{
return sub_patterns.fold(Constructor_pattern_environment_fold_state{initial_environment, 0}, [=](Constructor_pattern_environment_fold_state accumulator, std::shared_ptr<ast::Pat> sub_pattern_value) mutable { return constructor_subpattern_environment_fold_step(accumulator, sub_pattern_value, parameter_types, parameter_type_substitution, registry); }).environment;
}
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> env_for_pattern(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment, std::shared_ptr<ast::Pat> pattern, registry::TypeRegistry registry) noexcept{
return std::visit(overloaded{[&](const ast::PatIdent& patIdent) { auto [binding_name, __1] = patIdent; return [&]() {
auto environment = type_environment;
environment.set(binding_name, std::make_shared<registry::Type>(registry::TUnknown{}));
return environment;
}(); },
[&](const ast::PatCtor& patCtor) { auto [constructor_name, sub_patterns, __2] = patCtor; return [&]() {
auto parameter_types = registry::TypeRegistry_ctor_params_for(registry, constructor_name);
auto empty_type_parameter_substitution = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
return environment_after_all_constructor_subpatterns(type_environment, sub_patterns, parameter_types, empty_type_parameter_substitution, registry);
}(); },
[&](const ast::PatOr& patOr) { auto [alts, __1] = patOr; return ((alts.length() > 0) ? (env_for_pattern(type_environment, alts[0], registry)) : (type_environment)); },
[&](const auto& __v) { return type_environment; }
}, (*pattern));
}
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> env_for_pattern_substituted(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment, std::shared_ptr<ast::Pat> pattern, registry::TypeRegistry registry, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution, std::shared_ptr<registry::Type> scrutinee_type) noexcept{
return std::visit(overloaded{[&](const ast::PatIdent& patIdent) { auto [binding_name, __1] = patIdent; return [&]() {
auto environment = type_environment;
environment.set(binding_name, semantic_type_structure::substitute_type(scrutinee_type, substitution));
return environment;
}(); },
[&](const ast::PatCtor& patCtor) { auto [constructor_name, sub_patterns, __2] = patCtor; return [&]() {
auto parameter_types = registry::TypeRegistry_ctor_params_for(registry, constructor_name);
return environment_after_all_constructor_subpatterns(type_environment, sub_patterns, parameter_types, substitution, registry);
}(); },
[&](const ast::PatOr& patOr) { auto [alts, __1] = patOr; return ((alts.length() > 0) ? (env_for_pattern_substituted(type_environment, alts[0], registry, substitution, scrutinee_type)) : (type_environment)); },
[&](const auto& __v) { return type_environment; }
}, (*pattern));
}

} // namespace pattern_env
