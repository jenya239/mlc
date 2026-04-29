#define main mlc_user_main
#include "pattern_env.hpp"

namespace pattern_env {

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> env_for_pattern(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment, std::shared_ptr<ast::Pat> pattern, registry::TypeRegistry registry) noexcept{
return std::visit(overloaded{[&](const ast::PatIdent& patIdent) { auto [binding_name, __1] = patIdent; return [&]() {
auto environment = type_environment;
environment.set(binding_name, std::make_shared<registry::Type>(registry::TUnknown{}));
return environment;
}(); },
[&](const ast::PatCtor& patCtor) { auto [constructor_name, sub_patterns, __2] = patCtor; return [&]() {
auto parameter_types = registry::TypeRegistry_ctor_params_for(registry, constructor_name);
auto current_environment = type_environment;
auto sub_index = 0;
while ((sub_index < sub_patterns.length())) {
auto sub_type = ((sub_index < parameter_types.length()) ? parameter_types[sub_index] : std::make_shared<registry::Type>(registry::TUnknown{}));
current_environment = env_for_pattern_with_type(current_environment, sub_patterns[sub_index], sub_type, registry);
sub_index = (sub_index + 1);
}
return current_environment;
}(); },
[&](const ast::PatOr& patOr) { auto [alts, __1] = patOr; return ((alts.length() > 0) ? env_for_pattern(type_environment, alts[0], registry) : type_environment); },
[&](const auto& __v) { return type_environment; }
}, (*pattern));
}
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> env_for_pattern_with_type(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment, std::shared_ptr<ast::Pat> pattern, std::shared_ptr<registry::Type> type_value, registry::TypeRegistry registry) noexcept{
return std::visit(overloaded{[&](const ast::PatIdent& patIdent) { auto [binding_name, __1] = patIdent; return [&]() {
auto environment = type_environment;
environment.set(binding_name, type_value);
return environment;
}(); },
[&](const ast::PatOr& patOr) { auto [alts, __1] = patOr; return ((alts.length() > 0) ? env_for_pattern_with_type(type_environment, alts[0], type_value, registry) : type_environment); },
[&](const auto& __v) { return env_for_pattern(type_environment, pattern, registry); }
}, (*pattern));
}
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> env_for_pattern_substituted(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment, std::shared_ptr<ast::Pat> pattern, registry::TypeRegistry registry, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution) noexcept{
return std::visit(overloaded{[&](const ast::PatIdent& patIdent) { auto [binding_name, __1] = patIdent; return [&]() {
auto environment = type_environment;
environment.set(binding_name, std::make_shared<registry::Type>(registry::TUnknown{}));
return environment;
}(); },
[&](const ast::PatCtor& patCtor) { auto [constructor_name, sub_patterns, __2] = patCtor; return [&]() {
auto parameter_types = registry::TypeRegistry_ctor_params_for(registry, constructor_name);
auto current_environment = type_environment;
auto sub_index = 0;
while ((sub_index < sub_patterns.length())) {
auto raw_type = ((sub_index < parameter_types.length()) ? parameter_types[sub_index] : std::make_shared<registry::Type>(registry::TUnknown{}));
auto sub_type = semantic_type_structure::substitute_type(raw_type, substitution);
current_environment = env_for_pattern_with_type(current_environment, sub_patterns[sub_index], sub_type, registry);
sub_index = (sub_index + 1);
}
return current_environment;
}(); },
[&](const ast::PatOr& patOr) { auto [alts, __1] = patOr; return ((alts.length() > 0) ? env_for_pattern_substituted(type_environment, alts[0], registry, substitution) : type_environment); },
[&](const auto& __v) { return type_environment; }
}, (*pattern));
}

} // namespace pattern_env
