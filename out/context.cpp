#define main mlc_user_main
#include "context.hpp"

namespace context {

GenStmtsResult GenStmtsResult_append_stmt(GenStmtsResult self, GenStmtResult statement_result) noexcept{
return GenStmtsResult{(self.code + statement_result.output), statement_result.next_try};
}
mlc::String context_resolve(CodegenContext context, mlc::String name) noexcept{
if (context.qualified.has(name)) {
return (context.qualified.get(name) + cpp_naming::cpp_safe(name));
} else if ((context.namespace_prefix.length() > 0)) {
return (context.namespace_prefix + cpp_naming::cpp_safe(name));
} else {
return cpp_naming::cpp_safe(name);
}

}
mlc::Array<mlc::String> collect_pat_bind_names_for_context(std::shared_ptr<ast::Pat> pat, mlc::Array<mlc::String> accumulator) noexcept{
return std::visit(overloaded{[&](const ast::PatIdent& patIdent) { auto [n, __1] = patIdent; return [&]() {
accumulator.push_back(n);
return accumulator;
}(); },
[&](const ast::PatCtor& patCtor) { auto [__0, subs, __2] = patCtor; return [&]() {
auto index = 0;
while ((index < subs.length())) {
accumulator = collect_pat_bind_names_for_context(subs[index], accumulator);
index = (index + 1);
}
return accumulator;
}(); },
[&](const ast::PatRecord& patRecord) { auto [__0, field_patterns, __2] = patRecord; return [&]() {
auto index = 0;
while ((index < field_patterns.length())) {
accumulator = collect_pat_bind_names_for_context(field_patterns[index], accumulator);
index = (index + 1);
}
return accumulator;
}(); },
[&](const ast::PatTuple& patTuple) { auto [subs, __1] = patTuple; return [&]() {
auto index = 0;
while ((index < subs.length())) {
accumulator = collect_pat_bind_names_for_context(subs[index], accumulator);
index = (index + 1);
}
return accumulator;
}(); },
[&](const ast::PatArray& patArray) { auto [subs, rest, __2] = patArray; return [&]() {
auto index = 0;
while ((index < subs.length())) {
accumulator = collect_pat_bind_names_for_context(subs[index], accumulator);
index = (index + 1);
}
return (((rest != mlc::String("", 0)) && (rest != mlc::String("_", 1))) ? ([&]() {
accumulator.push_back(rest);
return accumulator;
}()) : (accumulator));
}(); },
[&](const auto& __v) { return accumulator; }
}, (*pat));
}
mlc::String CodegenContext_resolve(CodegenContext self, mlc::String name) noexcept{
return context_resolve(self, name);
}
CodegenContext CodegenContext_for_type_body(CodegenContext self, mlc::String type_name) noexcept{
return CodegenContext{self.field_orders, self.namespace_prefix, self.qualified, self.namespace_alias_prefixes, type_name, self.method_owners, {}, {}, {}, {}, self.ctor_type_infos, self.variant_types, {}, {}, self.generic_variants, self.struct_using_lines};
}
CodegenContext CodegenContext_add_shared(CodegenContext self, mlc::String name) noexcept{
auto new_shared_params = self.shared_params;
new_shared_params.push_back(name);
return CodegenContext{self.field_orders, self.namespace_prefix, self.qualified, self.namespace_alias_prefixes, self.self_type, self.method_owners, new_shared_params, self.shared_array_params, self.array_elem_types, self.shared_map_params, self.ctor_type_infos, self.variant_types, self.value_params, self.match_deref_params, self.generic_variants, self.struct_using_lines};
}
CodegenContext CodegenContext_add_shared_array(CodegenContext self, mlc::String name) noexcept{
auto new_shared_array_params = self.shared_array_params;
new_shared_array_params.push_back(name);
return CodegenContext{self.field_orders, self.namespace_prefix, self.qualified, self.namespace_alias_prefixes, self.self_type, self.method_owners, self.shared_params, new_shared_array_params, self.array_elem_types, self.shared_map_params, self.ctor_type_infos, self.variant_types, self.value_params, self.match_deref_params, self.generic_variants, self.struct_using_lines};
}
CodegenContext CodegenContext_add_value(CodegenContext self, mlc::String name) noexcept{
auto new_value_params = self.value_params;
new_value_params.push_back(name);
return CodegenContext{self.field_orders, self.namespace_prefix, self.qualified, self.namespace_alias_prefixes, self.self_type, self.method_owners, self.shared_params, self.shared_array_params, self.array_elem_types, self.shared_map_params, self.ctor_type_infos, self.variant_types, new_value_params, self.match_deref_params, self.generic_variants, self.struct_using_lines};
}
CodegenContext CodegenContext_add_match_deref(CodegenContext self, mlc::String name) noexcept{
auto new_match_deref_params = self.match_deref_params;
new_match_deref_params.push_back(name);
return CodegenContext{self.field_orders, self.namespace_prefix, self.qualified, self.namespace_alias_prefixes, self.self_type, self.method_owners, self.shared_params, self.shared_array_params, self.array_elem_types, self.shared_map_params, self.ctor_type_infos, self.variant_types, self.value_params, new_match_deref_params, self.generic_variants, self.struct_using_lines};
}
CodegenContext CodegenContext_with_struct_using_lines(CodegenContext self, mlc::HashMap<mlc::String, mlc::Array<mlc::String>> lines) noexcept{
return CodegenContext{self.field_orders, self.namespace_prefix, self.qualified, self.namespace_alias_prefixes, self.self_type, self.method_owners, self.shared_params, self.shared_array_params, self.array_elem_types, self.shared_map_params, self.ctor_type_infos, self.variant_types, self.value_params, self.match_deref_params, self.generic_variants, lines};
}
CodegenContext CodegenContext_with_namespace_alias_prefixes(CodegenContext self, mlc::HashMap<mlc::String, mlc::String> prefixes) noexcept{
return CodegenContext{self.field_orders, self.namespace_prefix, self.qualified, prefixes, self.self_type, self.method_owners, self.shared_params, self.shared_array_params, self.array_elem_types, self.shared_map_params, self.ctor_type_infos, self.variant_types, self.value_params, self.match_deref_params, self.generic_variants, self.struct_using_lines};
}
CodegenContext CodegenContext_update_from_statement(CodegenContext self, std::shared_ptr<semantic_ir::SStmt> statement) noexcept{
return std::visit(overloaded{[&](const semantic_ir::SStmtLetConst& sStmtLetConst) { auto [binding_name, __1, __2, __3] = sStmtLetConst; return CodegenContext_add_value(self, binding_name); },
[&](const semantic_ir::SStmtLet& sStmtLet) { auto [binding_name, __1, __2, value_type, __4] = sStmtLet; return std::visit(overloaded{[&](const registry::TShared& tShared) { auto [__0] = tShared; return CodegenContext_add_shared(self, binding_name); },
[&](const registry::TArray& tArray) { auto [inner] = tArray; return std::visit(overloaded{[&](const registry::TShared& tShared) { auto [__0] = tShared; return CodegenContext_add_shared_array(self, binding_name); },
[&](const auto& __v) { return CodegenContext_add_value(self, binding_name); }
}, (*inner)); },
[&](const auto& __v) { return CodegenContext_add_value(self, binding_name); }
}, (*value_type)); },
[&](const semantic_ir::SStmtLetPat& sStmtLetPat) { auto [pattern, __1, __2, __3, __4, __5, __6] = sStmtLetPat; return [&]() {
auto names = [&]() {
mlc::Array<mlc::String> __mut_actual_argument_holder_1 = mlc::Array<mlc::String>{};

return collect_pat_bind_names_for_context(pattern, __mut_actual_argument_holder_1);
}();
auto codegen_context = self;
auto name_index = 0;
while ((name_index < names.length())) {
codegen_context = CodegenContext_add_value(codegen_context, names[name_index]);
name_index = (name_index + 1);
}
return codegen_context;
}(); },
[&](const auto& __v) { return self; }
}, (*statement));
}
CodegenContext make_body_context(CodegenContext base, mlc::Array<mlc::String> shared_params, mlc::Array<mlc::String> shared_array_params, mlc::HashMap<mlc::String, mlc::String> array_elem_types, mlc::Array<mlc::String> shared_map_params, mlc::String self_type, mlc::Array<mlc::String> value_params, mlc::Array<mlc::String> match_deref_params) noexcept{
return CodegenContext{base.field_orders, base.namespace_prefix, base.qualified, base.namespace_alias_prefixes, self_type, base.method_owners, shared_params, shared_array_params, array_elem_types, shared_map_params, base.ctor_type_infos, base.variant_types, value_params, match_deref_params, base.generic_variants, base.struct_using_lines};
}
CodegenContext context_add_shared(CodegenContext context, mlc::String name) noexcept{
return CodegenContext_add_shared(context, name);
}
CodegenContext context_add_shared_array(CodegenContext context, mlc::String name) noexcept{
return CodegenContext_add_shared_array(context, name);
}
CodegenContext context_add_value(CodegenContext context, mlc::String name) noexcept{
return CodegenContext_add_value(context, name);
}
CodegenContext context_add_match_deref(CodegenContext context, mlc::String name) noexcept{
return CodegenContext_add_match_deref(context, name);
}
CodegenContext update_context_from_statement(std::shared_ptr<semantic_ir::SStmt> statement, CodegenContext context) noexcept{
return CodegenContext_update_from_statement(context, statement);
}
CodegenContext create_codegen_context(ast::Program prog) noexcept{
return CodegenContext{type_index::build_field_orders(prog), mlc::String("", 0), {}, {}, mlc::String("", 0), type_index::build_method_owners_from_decls(prog.decls), {}, {}, {}, {}, {}, type_index::build_variant_types_from_decls(prog.decls), {}, {}, type_index::build_generic_variants_from_decls(prog.decls), {}};
}
CodegenContext context_with_struct_using_lines(CodegenContext base, mlc::HashMap<mlc::String, mlc::Array<mlc::String>> struct_using_lines) noexcept{
return CodegenContext_with_struct_using_lines(base, struct_using_lines);
}
CodegenContext context_with_namespace_alias_prefixes(CodegenContext base, mlc::HashMap<mlc::String, mlc::String> namespace_alias_prefixes) noexcept{
return CodegenContext_with_namespace_alias_prefixes(base, namespace_alias_prefixes);
}

} // namespace context
