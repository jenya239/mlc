#define main mlc_user_main
#include "let_pattern_infer.hpp"

namespace let_pattern_infer {

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> build_let_substitution(std::shared_ptr<registry::Type> value_type, registry::TypeRegistry registry) noexcept{
auto substitution = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
std::visit(overloaded{[&](const registry::TGeneric& tGeneric) { auto [type_name, type_args] = tGeneric; return [&]() {
auto param_names = registry::TypeRegistry_algebraic_decl_type_parameter_names_for(registry, type_name);
auto i = 0;
return [&]() {
while (((i < param_names.length()) && (i < type_args.length()))) {
substitution.set(param_names[i], type_args[i]);
i = (i + 1);
}
}();
}(); },
[&](const auto& __v) { return /* unit */; }
}, (*value_type));
return substitution;
}
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> apply_let_pattern_to_env(std::shared_ptr<ast::Pat> pat, std::shared_ptr<registry::Type> value_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env, registry::TypeRegistry registry) noexcept{
return std::visit(overloaded{[&](const ast::PatWild& patWild) { auto [__0] = patWild; return base_env; },
[&](const ast::PatIdent& patIdent) { auto [n, __1] = patIdent; return [&]() {
auto u = base_env;
u.set(n, value_type);
return u;
}(); },
[&](const ast::PatTuple& patTuple) { auto [subs, __1] = patTuple; return std::visit(overloaded{[&](const registry::TTuple& tTuple) { auto [ts] = tTuple; return ((subs.length() != ts.length()) ? base_env : [&]() {
auto acc = base_env;
auto j = 0;
while ((j < subs.length())) {
acc = apply_let_pattern_to_env(subs[j], ts[j], acc, registry);
j = (j + 1);
}
return acc;
}()); },
[&](const registry::TPair& tPair) { auto [fst, snd] = tPair; return ((subs.length() != 2) ? base_env : [&]() {
auto e0 = apply_let_pattern_to_env(subs[0], fst, base_env, registry);
return apply_let_pattern_to_env(subs[1], snd, e0, registry);
}()); },
[&](const auto& __v) { return base_env; }
}, (*value_type)); },
[&](const ast::PatArray& patArray) { auto [subs, rest, __2] = patArray; return std::visit(overloaded{[&](const registry::TArray& tArray) { auto [et] = tArray; return [&]() {
auto acc = base_env;
auto i = 0;
while ((i < subs.length())) {
acc = apply_let_pattern_to_env(subs[i], et, acc, registry);
i = (i + 1);
}
return ((rest != mlc::String("", 0)) ? [&]() {
acc.set(rest, value_type);
return acc;
}() : acc);
}(); },
[&](const auto& __v) { return base_env; }
}, (*value_type)); },
[&](const ast::PatRecord& patRecord) { auto [rec_name, field_pats, __2] = patRecord; return ((rec_name != mlc::String("", 0)) ? base_env : [&]() {
auto acc = base_env;
auto i = 0;
while ((i < field_pats.length())) {
auto fp = field_pats[i];
auto field_name = std::visit(overloaded{[&](const ast::PatIdent& patIdent) { auto [n, __1] = patIdent; return n; },
[&](const auto& __v) { return mlc::String("", 0); }
}, (*fp));
if ((field_name != mlc::String("", 0))) {
auto ft = registry::field_type_from_object(value_type, field_name, registry);
acc = apply_let_pattern_to_env(fp, ft, acc, registry);
}
i = (i + 1);
}
return acc;
}()); },
[&](const ast::PatCtor& patCtor) { auto [__0, __1, __2] = patCtor; return pattern_env::env_for_pattern_substituted(base_env, pat, registry, build_let_substitution(value_type, registry)); },
[&](const auto& __v) { return base_env; }
}, (*pat));
}
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> infer_let_pattern_env(std::shared_ptr<ast::Pat> pat, std::shared_ptr<registry::Type> value_type, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> base_env, registry::TypeRegistry registry) noexcept{
return apply_let_pattern_to_env(pat, value_type, base_env, registry);
}

} // namespace let_pattern_infer
