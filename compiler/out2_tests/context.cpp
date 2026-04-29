#define main mlc_user_main
#include "context.hpp"

namespace context {

mlc::String context_resolve(CodegenContext context, mlc::String name) noexcept{
if (context.qualified.has(name)) {
return (context.qualified.get(name) + cpp_naming::cpp_safe(name));
} else if ((context.namespace_prefix.length() > 0)) {
return (context.namespace_prefix + cpp_naming::cpp_safe(name));
} else {
return cpp_naming::cpp_safe(name);
}

}
CodegenContext make_body_context(CodegenContext base, mlc::Array<mlc::String> shared_params, mlc::Array<mlc::String> shared_array_params, mlc::HashMap<mlc::String, mlc::String> array_elem_types, mlc::Array<mlc::String> shared_map_params, mlc::String self_type, mlc::Array<mlc::String> value_params, mlc::Array<mlc::String> match_deref_params) noexcept{
return CodegenContext{base.field_orders, base.namespace_prefix, base.qualified, base.namespace_alias_prefixes, self_type, base.method_owners, shared_params, shared_array_params, array_elem_types, shared_map_params, base.ctor_type_infos, base.variant_types, value_params, match_deref_params, base.generic_variants};
}
CodegenContext context_add_shared(CodegenContext context, mlc::String name) noexcept{
auto new_shared_params = context.shared_params;
new_shared_params.push_back(name);
return make_body_context(context, new_shared_params, context.shared_array_params, context.array_elem_types, context.shared_map_params, context.self_type, context.value_params, context.match_deref_params);
}
CodegenContext context_add_shared_array(CodegenContext context, mlc::String name) noexcept{
auto new_shared_array_params = context.shared_array_params;
new_shared_array_params.push_back(name);
return make_body_context(context, context.shared_params, new_shared_array_params, context.array_elem_types, context.shared_map_params, context.self_type, context.value_params, context.match_deref_params);
}
CodegenContext context_add_value(CodegenContext context, mlc::String name) noexcept{
auto new_value_params = context.value_params;
new_value_params.push_back(name);
return make_body_context(context, context.shared_params, context.shared_array_params, context.array_elem_types, context.shared_map_params, context.self_type, new_value_params, context.match_deref_params);
}
CodegenContext context_add_match_deref(CodegenContext context, mlc::String name) noexcept{
auto md = context.match_deref_params;
md.push_back(name);
return make_body_context(context, context.shared_params, context.shared_array_params, context.array_elem_types, context.shared_map_params, context.self_type, context.value_params, md);
}
mlc::Array<mlc::String> collect_pat_bind_names_for_context(std::shared_ptr<ast::Pat> pat, mlc::Array<mlc::String> accumulator) noexcept{
return std::visit(overloaded{[&](const ast::PatIdent& patIdent) { auto [n, __1] = patIdent; return [&]() {
accumulator.push_back(n);
return accumulator;
}(); },
[&](const ast::PatCtor& patCtor) { auto [__0, subs, __2] = patCtor; return [&]() {
auto j = 0;
while ((j < subs.length())) {
accumulator = collect_pat_bind_names_for_context(subs[j], accumulator);
j = (j + 1);
}
return accumulator;
}(); },
[&](const ast::PatRecord& patRecord) { auto [__0, fps, __2] = patRecord; return [&]() {
auto j = 0;
while ((j < fps.length())) {
accumulator = collect_pat_bind_names_for_context(fps[j], accumulator);
j = (j + 1);
}
return accumulator;
}(); },
[&](const ast::PatTuple& patTuple) { auto [subs, __1] = patTuple; return [&]() {
auto j = 0;
while ((j < subs.length())) {
accumulator = collect_pat_bind_names_for_context(subs[j], accumulator);
j = (j + 1);
}
return accumulator;
}(); },
[&](const ast::PatArray& patArray) { auto [subs, rest, __2] = patArray; return [&]() {
auto j = 0;
while ((j < subs.length())) {
accumulator = collect_pat_bind_names_for_context(subs[j], accumulator);
j = (j + 1);
}
return (((rest != mlc::String("", 0)) && (rest != mlc::String("_", 1))) ? [&]() {
accumulator.push_back(rest);
return accumulator;
}() : accumulator);
}(); },
[&](const auto& __v) { return accumulator; }
}, (*pat));
}
CodegenContext update_context_from_statement(std::shared_ptr<semantic_ir::SStmt> stmt, CodegenContext context) noexcept{
return std::visit(overloaded{[&](const semantic_ir::SStmtLetConst& sStmtLetConst) { auto [binding_name, __1, __2, __3] = sStmtLetConst; return context_add_value(context, binding_name); },
[&](const semantic_ir::SStmtLet& sStmtLet) { auto [binding_name, __1, __2, value_type, __4] = sStmtLet; return std::visit(overloaded{[&](const registry::TShared& tShared) { auto [__0] = tShared; return context_add_shared(context, binding_name); },
[&](const registry::TArray& tArray) { auto [inner] = tArray; return std::visit(overloaded{[&](const registry::TShared& tShared) { auto [__0] = tShared; return context_add_shared_array(context, binding_name); },
[&](const auto& __v) { return context_add_value(context, binding_name); }
}, (*inner)); },
[&](const auto& __v) { return context_add_value(context, binding_name); }
}, (*value_type)); },
[&](const semantic_ir::SStmtLetPat& sStmtLetPat) { auto [pat, __1, __2, __3, __4] = sStmtLetPat; return [&]() {
auto names = collect_pat_bind_names_for_context(pat, {});
auto c = context;
auto k = 0;
while ((k < names.length())) {
c = context_add_value(c, names[k]);
k = (k + 1);
}
return c;
}(); },
[&](const auto& __v) { return context; }
}, (*stmt));
}
CodegenContext create_codegen_context(ast::Program prog) noexcept{
return CodegenContext{type_index::build_field_orders(prog), mlc::String("", 0), {}, {}, mlc::String("", 0), type_index::build_method_owners_from_decls(prog.decls), {}, {}, {}, {}, {}, type_index::build_variant_types_from_decls(prog.decls), {}, {}, type_index::build_generic_variants_from_decls(prog.decls)};
}
CodegenContext context_with_namespace_alias_prefixes(CodegenContext base, mlc::HashMap<mlc::String, mlc::String> namespace_alias_prefixes) noexcept{
return CodegenContext{base.field_orders, base.namespace_prefix, base.qualified, namespace_alias_prefixes, base.self_type, base.method_owners, base.shared_params, base.shared_array_params, base.array_elem_types, base.shared_map_params, base.ctor_type_infos, base.variant_types, base.value_params, base.match_deref_params, base.generic_variants};
}

} // namespace context
