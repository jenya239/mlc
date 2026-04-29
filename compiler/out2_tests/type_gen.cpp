#define main mlc_user_main
#include "type_gen.hpp"

namespace type_gen {

mlc::String cpp_generic_base_name(context::CodegenContext context, mlc::String type_name) noexcept{
if ((type_name == mlc::String("Map", 3))) {
return mlc::String("mlc::HashMap", 12);
} else if ((type_name == mlc::String("Shared", 6))) {
return mlc::String("std::shared_ptr", 15);
} else {
return context::context_resolve(context, type_name);
}

}
mlc::String sem_type_to_cpp(context::CodegenContext context, std::shared_ptr<registry::Type> semantic_type) noexcept{
return std::visit(overloaded{[&](const registry::TI32& tI32) { return mlc::String("int", 3); },
[&](const registry::TString& tString) { return mlc::String("mlc::String", 11); },
[&](const registry::TBool& tBool) { return mlc::String("bool", 4); },
[&](const registry::TUnit& tUnit) { return mlc::String("void", 4); },
[&](const registry::TUnknown& tUnknown) { return mlc::String("auto", 4); },
[&](const registry::TArray& tArray) { auto [inner] = tArray; return expr::cpp_array_type_element(sem_type_to_cpp(context, inner)); },
[&](const registry::TShared& tShared) { auto [inner] = tShared; return expr::cpp_shared_pointer_type(sem_type_to_cpp(context, inner)); },
[&](const registry::TNamed& tNamed) { auto [type_name] = tNamed; return context::context_resolve(context, type_name); },
[&](const registry::TTuple& tTuple) { auto [ts] = tTuple; return [&]() {
auto tparts = mlc::Array<mlc::String>{};
auto ti = 0;
while ((ti < ts.length())) {
tparts.push_back(sem_type_to_cpp(context, ts[ti]));
ti = (ti + 1);
}
return ((mlc::String("std::tuple<", 11) + tparts.join(mlc::String(", ", 2))) + mlc::String(">", 1));
}(); },
[&](const registry::TPair& tPair) { auto [left, right] = tPair; return ((((mlc::String("std::pair<", 10) + sem_type_to_cpp(context, left)) + mlc::String(", ", 2)) + sem_type_to_cpp(context, right)) + mlc::String(">", 1)); },
[&](const registry::TGeneric& tGeneric) { auto [type_name, type_args] = tGeneric; return (((type_name == mlc::String("Option", 6)) && (type_args.length() == 1)) ? ((mlc::String("std::optional<", 14) + sem_type_to_cpp(context, type_args[0])) + mlc::String(">", 1)) : ((type_args.length() == 0) ? cpp_generic_base_name(context, type_name) : ((type_args.length() == 1) ? expr::cpp_template_single_type_argument(cpp_generic_base_name(context, type_name), sem_type_to_cpp(context, type_args[0])) : expr::cpp_template_two_type_arguments(cpp_generic_base_name(context, type_name), sem_type_to_cpp(context, type_args[0]), sem_type_to_cpp(context, type_args[1]))))); },
[&](const registry::TFn& tFn) { auto [param_types, return_type] = tFn; return [&]() {
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < param_types.length())) {
parts.push_back(sem_type_to_cpp(context, param_types[i]));
i = (i + 1);
}
return expr::cpp_std_function_type(sem_type_to_cpp(context, return_type), parts.join(mlc::String(", ", 2)));
}(); }
}, (*semantic_type));
}
mlc::String type_name_to_cpp(context::CodegenContext context, mlc::String type_name) noexcept{
if ((type_name == mlc::String("i32", 3))) {
return mlc::String("int", 3);
} else if ((type_name == mlc::String("string", 6))) {
return mlc::String("mlc::String", 11);
} else if ((type_name == mlc::String("bool", 4))) {
return mlc::String("bool", 4);
} else if ((type_name == mlc::String("unit", 4))) {
return mlc::String("void", 4);
} else if (((type_name == mlc::String("Self", 4)) || (type_name == mlc::String("self", 4)))) {
if ((context.self_type.length() > 0)) {
return type_name_to_cpp(context, context.self_type);
} else {
return mlc::String("void", 4);
}
} else {
return context::context_resolve(context, type_name);
}




}
mlc::String type_to_cpp(context::CodegenContext context, std::shared_ptr<ast::TypeExpr> type_expr) noexcept{
return std::visit(overloaded{[&](const ast::TyI32& tyI32) { return mlc::String("int", 3); },
[&](const ast::TyString& tyString) { return mlc::String("mlc::String", 11); },
[&](const ast::TyBool& tyBool) { return mlc::String("bool", 4); },
[&](const ast::TyUnit& tyUnit) { return mlc::String("void", 4); },
[&](const ast::TyNamed& tyNamed) { auto [name] = tyNamed; return type_name_to_cpp(context, name); },
[&](const ast::TyArray& tyArray) { auto [inner] = tyArray; return expr::cpp_array_type_element(type_to_cpp(context, inner)); },
[&](const ast::TyShared& tyShared) { auto [inner] = tyShared; return expr::cpp_shared_pointer_type(type_to_cpp(context, inner)); },
[&](const ast::TyGeneric& tyGeneric) { auto [name, targs] = tyGeneric; return (((name == mlc::String("ref", 3)) && (targs.length() == 1)) ? expr::cpp_lvalue_reference_suffix(type_to_cpp(context, targs[0])) : [&]() {
auto safe = ((name == mlc::String("Map", 3)) ? mlc::String("mlc::HashMap", 12) : ((name == mlc::String("Shared", 6)) ? mlc::String("std::shared_ptr", 15) : context::context_resolve(context, name)));
return ((targs.length() == 0) ? safe : ((targs.length() == 1) ? expr::cpp_template_single_type_argument(safe, type_to_cpp(context, targs[0])) : expr::cpp_template_two_type_arguments(safe, type_to_cpp(context, targs[0]), type_to_cpp(context, targs[1]))));
}()); },
[&](const ast::TyFn& tyFn) { auto [params, ret] = tyFn; return [&]() {
auto param_types = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < params.length())) {
param_types.push_back(type_to_cpp(context, params[i]));
i = (i + 1);
}
return expr::cpp_std_function_type(type_to_cpp(context, ret), param_types.join(mlc::String(", ", 2)));
}(); }
}, (*type_expr));
}
bool type_param_in_typeexpr(mlc::String param, std::shared_ptr<ast::TypeExpr> t) noexcept{
return std::visit(overloaded{[&](const ast::TyNamed& tyNamed) { auto [name] = tyNamed; return (name == param); },
[&](const ast::TyArray& tyArray) { auto [inner] = tyArray; return type_param_in_typeexpr(param, inner); },
[&](const ast::TyShared& tyShared) { auto [inner] = tyShared; return type_param_in_typeexpr(param, inner); },
[&](const ast::TyGeneric& tyGeneric) { auto [__0, targs] = tyGeneric; return type_param_in_typeexpr_list(param, targs); },
[&](const ast::TyFn& tyFn) { auto [params, ret] = tyFn; return (type_param_in_typeexpr_list(param, params) || type_param_in_typeexpr(param, ret)); },
[&](const auto& __v) { return false; }
}, (*t));
}
bool type_param_in_typeexpr_list(mlc::String param, mlc::Array<std::shared_ptr<ast::TypeExpr>> types) noexcept{
auto found = false;
auto i = 0;
while ((i < types.length())) {
if ((!found)) {
found = type_param_in_typeexpr(param, types[i]);
}
i = (i + 1);
}
return found;
}
mlc::Array<std::shared_ptr<ast::TypeExpr>> variant_field_typeexprs(std::shared_ptr<ast::TypeVariant> variant) noexcept{
return std::visit(overloaded{[&](const ast::VarTuple& varTuple) { auto [__0, fts] = varTuple; return fts; },
[&](const ast::VarRecord& varRecord) { auto [__0, fds] = varRecord; return [&]() {
auto result = mlc::Array<std::shared_ptr<ast::TypeExpr>>{};
auto i = 0;
while ((i < fds.length())) {
result.push_back(fds[i]->typ);
i = (i + 1);
}
return result;
}(); },
[&](const ast::VarUnit& varUnit) { auto [__0] = varUnit; return [&]() {
auto r = mlc::Array<std::shared_ptr<ast::TypeExpr>>{};
return r;
}(); }
}, (*variant));
}
mlc::Array<mlc::String> variant_used_type_params(mlc::Array<mlc::String> type_params, std::shared_ptr<ast::TypeVariant> variant) noexcept{
auto field_types = variant_field_typeexprs(variant);
auto used = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < type_params.length())) {
if (type_param_in_typeexpr_list(type_params[i], field_types)) {
used.push_back(type_params[i]);
}
i = (i + 1);
}
return used;
}
mlc::String variant_ctor_name(std::shared_ptr<ast::TypeVariant> variant) noexcept{
return std::visit(overloaded{[&](const ast::VarUnit& varUnit) { auto [name] = varUnit; return name; },
[&](const ast::VarTuple& varTuple) { auto [name, __1] = varTuple; return name; },
[&](const ast::VarRecord& varRecord) { auto [name, __1] = varRecord; return name; }
}, (*variant));
}
mlc::String gen_variant_struct(context::CodegenContext context, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept{
return std::visit(overloaded{[&](const ast::VarUnit& varUnit) { auto [name] = varUnit; return expr::struct_empty_definition(context::context_resolve(context, name)); },
[&](const ast::VarTuple& varTuple) { auto [name, field_types] = varTuple; return [&]() {
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < field_types.length())) {
parts.push_back(expr::struct_tuple_field_declaration(type_to_cpp(context, field_types[i]), mlc::to_string(i)));
i = (i + 1);
}
return expr::struct_with_inline_members_definition(context::context_resolve(context, name), parts.join(mlc::String("", 0)));
}(); },
[&](const ast::VarRecord& varRecord) { auto [name, field_defs] = varRecord; return [&]() {
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < field_defs.length())) {
parts.push_back(expr::struct_named_field_declaration(type_to_cpp(context, field_defs[i]->typ), cpp_naming::cpp_safe(field_defs[i]->name)));
i = (i + 1);
}
return expr::struct_with_inline_members_definition(context::context_resolve(context, name), parts.join(mlc::String("", 0)));
}(); }
}, (*variant));
}
mlc::String gen_single_variant(context::CodegenContext context, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept{
return std::visit(overloaded{[&](const ast::VarRecord& varRecord) { auto [__0, field_defs] = varRecord; return [&]() {
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < field_defs.length())) {
parts.push_back(expr::struct_named_field_declaration(type_to_cpp(context, field_defs[i]->typ), cpp_naming::cpp_safe(field_defs[i]->name)));
i = (i + 1);
}
return expr::struct_with_inline_members_definition(context::context_resolve(context, type_name), parts.join(mlc::String("", 0)));
}(); },
[&](const ast::VarTuple& varTuple) { auto [__0, field_types] = varTuple; return [&]() {
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < field_types.length())) {
parts.push_back(expr::struct_tuple_field_declaration(type_to_cpp(context, field_types[i]), mlc::to_string(i)));
i = (i + 1);
}
return expr::struct_with_inline_members_definition(context::context_resolve(context, type_name), parts.join(mlc::String("", 0)));
}(); },
[&](const ast::VarUnit& varUnit) { auto [__0] = varUnit; return expr::struct_empty_definition(context::context_resolve(context, type_name)); }
}, (*variant));
}
mlc::String gen_adt_fwd(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
auto full_prefix = cpp_naming::template_prefix(type_params);
auto parts = mlc::Array<mlc::String>{};
auto alias_parts = mlc::Array<mlc::String>{};
auto index = 0;
while ((index < variants.length())) {
auto v = variants[index];
auto variant_name = context::context_resolve(context, variant_ctor_name(v));
auto used = variant_used_type_params(type_params, v);
auto var_prefix = cpp_naming::template_prefix(used);
auto var_targs = ((used.length() > 0) ? ((mlc::String("<", 1) + used.join(mlc::String(", ", 2))) + mlc::String(">", 1)) : mlc::String("", 0));
alias_parts.push_back((variant_name + var_targs));
parts.push_back(expr::struct_forward_declaration_line(var_prefix, variant_name));
index = (index + 1);
}
return (parts.join(mlc::String("", 0)) + expr::variant_using_alias_definition_line(full_prefix, context::context_resolve(context, type_name), alias_parts.join(mlc::String(", ", 2))));
}
mlc::String gen_adt_defs(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < variants.length())) {
auto v = variants[i];
auto used = variant_used_type_params(type_params, v);
parts.push_back((cpp_naming::template_prefix(used) + gen_variant_struct(context, type_name, v)));
i = (i + 1);
}
return parts.join(mlc::String("", 0));
}
mlc::String gen_type_decl_fwd(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
if ((variants.length() == 1)) {
return mlc::String("", 0);
} else {
return gen_adt_fwd(context, type_name, type_params, variants);
}
}
mlc::String gen_type_decl_body(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
if ((variants.length() == 1)) {
return (cpp_naming::template_prefix(type_params) + gen_single_variant(context, type_name, variants[0]));
} else {
return gen_adt_defs(context, type_name, type_params, variants);
}
}
mlc::String gen_type_decl(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
return (gen_type_decl_fwd(context, type_name, type_params, variants) + gen_type_decl_body(context, type_name, type_params, variants));
}
mlc::String derive_field_display(mlc::String field_name, std::shared_ptr<ast::TypeExpr> field_type) noexcept{
return std::visit(overloaded{[&](const ast::TyString& tyString) { return (mlc::String("self.", 5) + field_name); },
[&](const auto& __v) { return ((mlc::String("mlc::to_string(self.", 20) + field_name) + mlc::String(")", 1)); }
}, (*field_type));
}
mlc::String derive_display_record_fields(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs) noexcept{
auto body = (((((mlc::String("mlc::String(\"", 13) + type_name) + mlc::String(" { ", 3)) + field_defs[0]->name) + mlc::String(": \") + ", 7)) + derive_field_display(field_defs[0]->name, field_defs[0]->typ));
auto i = 1;
while ((i < field_defs.length())) {
body = ((((body + mlc::String(" + mlc::String(\", ", 18)) + field_defs[i]->name) + mlc::String(": \") + ", 7)) + derive_field_display(field_defs[i]->name, field_defs[i]->typ));
i = (i + 1);
}
return (body + mlc::String(" + mlc::String(\" }\")", 20));
}
mlc::String derive_display_record(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs) noexcept{
auto sig = ((((mlc::String("mlc::String ", 12) + type_name) + mlc::String("_to_string(const ", 17)) + type_name) + mlc::String("& self) noexcept", 16));
if ((field_defs.length() == 0)) {
return (((sig + mlc::String(" { return mlc::String(\"", 23)) + type_name) + mlc::String(" {}\"); }\n", 9));
} else {
return (((sig + mlc::String(" {\n  return ", 12)) + derive_display_record_fields(type_name, field_defs)) + mlc::String(";\n}\n", 4));
}
}
mlc::String derive_display_variant_case(std::shared_ptr<ast::TypeVariant> variant) noexcept{
return std::visit(overloaded{[&](const ast::VarUnit& varUnit) { auto [name] = varUnit; return ((((mlc::String("  if (std::holds_alternative<", 29) + name) + mlc::String(">(self._)) return mlc::String(\"", 31)) + name) + mlc::String("\");\n", 4)); },
[&](const ast::VarTuple& varTuple) { auto [name, field_types] = varTuple; return ((field_types.length() == 0) ? ((((mlc::String("  if (std::holds_alternative<", 29) + name) + mlc::String(">(self._)) return mlc::String(\"", 31)) + name) + mlc::String("\");\n", 4)) : ((((((mlc::String("  if (std::holds_alternative<", 29) + name) + mlc::String(">(self._)) return mlc::String(\"", 31)) + name) + mlc::String("(\") + mlc::to_string(std::get<", 30)) + name) + mlc::String(">(self._)._0) + mlc::String(\")\");\n", 34))); },
[&](const ast::VarRecord& varRecord) { auto [name, fds] = varRecord; return derive_display_variant_record(name, fds); }
}, (*variant));
}
mlc::String derive_display_variant_record(mlc::String name, mlc::Array<std::shared_ptr<ast::FieldDef>> fds) noexcept{
if ((fds.length() == 0)) {
return ((((mlc::String("  if (std::holds_alternative<", 29) + name) + mlc::String(">(self._)) return mlc::String(\"", 31)) + name) + mlc::String("\");\n", 4));
} else {
auto parts = ((((mlc::String("mlc::to_string(std::get<", 24) + name) + mlc::String(">(self._).", 10)) + fds[0]->name) + mlc::String(")", 1));
auto fi = 1;
while ((fi < fds.length())) {
parts = (((((parts + mlc::String(" + mlc::String(\", \") + mlc::to_string(std::get<", 47)) + name) + mlc::String(">(self._).", 10)) + fds[fi]->name) + mlc::String(")", 1));
fi = (fi + 1);
}
return ((((((mlc::String("  if (std::holds_alternative<", 29) + name) + mlc::String(">(self._)) return mlc::String(\"", 31)) + name) + mlc::String("(\") + ", 6)) + parts) + mlc::String(" + mlc::String(\")\");\n", 21));
}
}
mlc::String derive_display_sum(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
auto cases = mlc::String("", 0);
auto i = 0;
while ((i < variants.length())) {
cases = (cases + derive_display_variant_case(variants[i]));
i = (i + 1);
}
return ((((((mlc::String("mlc::String ", 12) + type_name) + mlc::String("_to_string(const ", 17)) + type_name) + mlc::String("& self) noexcept {\n", 19)) + cases) + mlc::String("  return mlc::String(\"\");\n}\n", 28));
}
mlc::String derive_eq_record(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs) noexcept{
auto sig = ((((mlc::String("bool operator==(const ", 22) + type_name) + mlc::String("& a, const ", 11)) + type_name) + mlc::String("& b) noexcept", 13));
if ((field_defs.length() == 0)) {
return (sig + mlc::String(" { return true; }\n", 18));
} else {
auto cond = (((mlc::String("a.", 2) + field_defs[0]->name) + mlc::String(" == b.", 6)) + field_defs[0]->name);
auto i = 1;
while ((i < field_defs.length())) {
cond = ((((cond + mlc::String(" && a.", 6)) + field_defs[i]->name) + mlc::String(" == b.", 6)) + field_defs[i]->name);
i = (i + 1);
}
return (((sig + mlc::String(" { return ", 10)) + cond) + mlc::String("; }\n", 4));
}
}
mlc::String derive_eq_sum(mlc::String type_name) noexcept{
return ((((mlc::String("bool operator==(const ", 22) + type_name) + mlc::String("& a, const ", 11)) + type_name) + mlc::String("& b) noexcept { return a._ == b._; }\n", 37));
}
mlc::String derive_ord_build_cond(mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs) noexcept{
auto cond = (((mlc::String("a.", 2) + field_defs[0]->name) + mlc::String(" < b.", 5)) + field_defs[0]->name);
auto i = 1;
while ((i < field_defs.length())) {
auto prev_eq = (((mlc::String("a.", 2) + field_defs[0]->name) + mlc::String(" == b.", 6)) + field_defs[0]->name);
auto j = 1;
while ((j < i)) {
prev_eq = ((((prev_eq + mlc::String(" && a.", 6)) + field_defs[j]->name) + mlc::String(" == b.", 6)) + field_defs[j]->name);
j = (j + 1);
}
cond = (((((((cond + mlc::String(" || (", 5)) + prev_eq) + mlc::String(" && a.", 6)) + field_defs[i]->name) + mlc::String(" < b.", 5)) + field_defs[i]->name) + mlc::String(")", 1));
i = (i + 1);
}
return cond;
}
mlc::String derive_ord_record(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs) noexcept{
auto sig = ((((mlc::String("bool operator<(const ", 21) + type_name) + mlc::String("& a, const ", 11)) + type_name) + mlc::String("& b) noexcept", 13));
if ((field_defs.length() == 0)) {
return (sig + mlc::String(" { return false; }\n", 19));
} else {
return (((sig + mlc::String(" { return ", 10)) + derive_ord_build_cond(field_defs)) + mlc::String("; }\n", 4));
}
}
mlc::String derive_ord_sum(mlc::String type_name) noexcept{
return ((((mlc::String("bool operator<(const ", 21) + type_name) + mlc::String("& a, const ", 11)) + type_name) + mlc::String("& b) noexcept { return a._ < b._; }\n", 36));
}
bool variants_is_single_record(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
if ((variants.length() != 1)) {
return false;
} else {
return std::visit(overloaded{[&](const ast::VarRecord& varRecord) { auto [__0, __1] = varRecord; return true; },
[&](const auto& __v) { return false; }
}, (*variants[0]));
}
}
mlc::Array<std::shared_ptr<ast::FieldDef>> derive_record_field_defs(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
return std::visit(overloaded{[&](const ast::VarRecord& varRecord) { auto [__0, fds] = varRecord; return fds; },
[&](const auto& __v) { return {}; }
}, (*variants[0]));
}
mlc::String gen_derive_record_trait(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::String trait_name) noexcept{
auto fds = derive_record_field_defs(variants);
if ((trait_name == mlc::String("Display", 7))) {
return derive_display_record(type_name, fds);
} else if ((trait_name == mlc::String("Eq", 2))) {
return derive_eq_record(type_name, fds);
} else if ((trait_name == mlc::String("Ord", 3))) {
return derive_ord_record(type_name, fds);
} else {
return mlc::String("", 0);
}


}
mlc::String gen_derive_sum_trait(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::String trait_name) noexcept{
if ((trait_name == mlc::String("Display", 7))) {
return derive_display_sum(type_name, variants);
} else if ((trait_name == mlc::String("Eq", 2))) {
return derive_eq_sum(type_name);
} else if ((trait_name == mlc::String("Ord", 3))) {
return derive_ord_sum(type_name);
} else {
return mlc::String("", 0);
}


}
mlc::String gen_derive_methods(context::CodegenContext context, mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::Array<mlc::String> derive_traits) noexcept{
if ((derive_traits.length() == 0)) {
return mlc::String("", 0);
} else {
auto is_record = variants_is_single_record(variants);
auto result = mlc::String("", 0);
auto i = 0;
while ((i < derive_traits.length())) {
auto trait_code = (is_record ? gen_derive_record_trait(type_name, variants, derive_traits[i]) : gen_derive_sum_trait(type_name, variants, derive_traits[i]));
result = (result + trait_code);
i = (i + 1);
}
return result;
}
}
mlc::String gen_type_decl_fwd_only(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
return gen_type_decl_fwd(context, type_name, type_params, variants);
}
mlc::String gen_type_decl_body_only(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
return gen_type_decl_body(context, type_name, type_params, variants);
}
mlc::String requires_clause(mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds) noexcept{
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < type_params.length())) {
if ((i < type_bounds.length())) {
auto k = 0;
[&]() {
while ((k < type_bounds[i].length())) {
parts.push_back(expr::concept_trait_constraint_on_type_parameter(cpp_naming::cpp_safe(type_bounds[i][k]), type_params[i]));
k = (k + 1);
}
}();
}
i = (i + 1);
}
if ((parts.length() > 0)) {
return expr::concept_requires_clause_line(parts.join(mlc::String(" && ", 4)));
} else {
return mlc::String("", 0);
}
}

} // namespace type_gen
