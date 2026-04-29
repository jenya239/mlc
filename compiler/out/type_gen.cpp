#include "type_gen.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "context.hpp"
#include "cpp_naming.hpp"
#include "expr.hpp"

namespace type_gen {

using namespace ast;
using namespace registry;
using namespace context;
using namespace cpp_naming;
using namespace expr;
using namespace ast_tokens;

mlc::String cpp_generic_base_name(context::CodegenContext context, mlc::String type_name) noexcept;

mlc::String sem_type_to_cpp(context::CodegenContext context, std::shared_ptr<registry::Type> semantic_type) noexcept;

mlc::String type_name_to_cpp(context::CodegenContext context, mlc::String type_name) noexcept;

mlc::String type_to_cpp(context::CodegenContext context, std::shared_ptr<ast::TypeExpr> type_expr) noexcept;

bool type_param_in_typeexpr(mlc::String param, std::shared_ptr<ast::TypeExpr> t) noexcept;

bool type_param_in_typeexpr_list(mlc::String param, mlc::Array<std::shared_ptr<ast::TypeExpr>> types) noexcept;

mlc::Array<std::shared_ptr<ast::TypeExpr>> variant_field_typeexprs(std::shared_ptr<ast::TypeVariant> variant) noexcept;

mlc::Array<mlc::String> variant_used_type_params(mlc::Array<mlc::String> type_params, std::shared_ptr<ast::TypeVariant> variant) noexcept;

mlc::Array<mlc::String> union_string_lists(mlc::Array<mlc::String> a, mlc::Array<mlc::String> b) noexcept;

mlc::Array<mlc::String> type_phantom_params_for_variants(mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::String variant_ctor_name(std::shared_ptr<ast::TypeVariant> variant) noexcept;

mlc::String gen_variant_struct(context::CodegenContext context, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept;

mlc::String gen_single_variant(context::CodegenContext context, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept;

mlc::String gen_adt_fwd(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::String gen_adt_defs(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::String gen_type_decl_fwd(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::String gen_type_decl_body(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::String gen_type_decl(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::String derive_field_display(mlc::String field_name, std::shared_ptr<ast::TypeExpr> field_type) noexcept;

mlc::String derive_display_record_fields(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs) noexcept;

mlc::String derive_display_record(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs) noexcept;

mlc::String derive_display_variant_case(std::shared_ptr<ast::TypeVariant> variant) noexcept;

mlc::String derive_display_variant_record(mlc::String name, mlc::Array<std::shared_ptr<ast::FieldDef>> fds) noexcept;

mlc::String derive_display_sum(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::String derive_eq_record(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs) noexcept;

mlc::String derive_eq_sum(mlc::String type_name) noexcept;

mlc::String derive_ord_build_cond(mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs) noexcept;

mlc::String derive_ord_record(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs) noexcept;

mlc::String derive_ord_sum(mlc::String type_name) noexcept;

bool variants_is_single_record(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::Array<std::shared_ptr<ast::FieldDef>> derive_record_field_defs(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::String gen_derive_record_trait(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::String trait_name) noexcept;

mlc::String gen_derive_sum_trait(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::String trait_name) noexcept;

mlc::String gen_derive_methods(context::CodegenContext context, mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::Array<mlc::String> derive_traits) noexcept;

mlc::String gen_type_decl_fwd_only(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::String gen_type_decl_body_only(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::String requires_clause(mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds) noexcept;

mlc::String cpp_generic_base_name(context::CodegenContext context, mlc::String type_name) noexcept{return type_name == mlc::String("Map") ? mlc::String("mlc::HashMap") : type_name == mlc::String("Shared") ? mlc::String("std::shared_ptr") : context::context_resolve(context, type_name);}

mlc::String sem_type_to_cpp(context::CodegenContext context, std::shared_ptr<registry::Type> semantic_type) noexcept{return std::visit(overloaded{
  [&](const TI32& ti32) -> mlc::String { return mlc::String("int"); },
  [&](const TString& tstring) -> mlc::String { return mlc::String("mlc::String"); },
  [&](const TBool& tbool) -> mlc::String { return mlc::String("bool"); },
  [&](const TUnit& tunit) -> mlc::String { return mlc::String("void"); },
  [&](const TI64& ti64) -> mlc::String { return mlc::String("int64_t"); },
  [&](const TF64& tf64) -> mlc::String { return mlc::String("double"); },
  [&](const TU8& tu8) -> mlc::String { return mlc::String("uint8_t"); },
  [&](const TUsize& tusize) -> mlc::String { return mlc::String("size_t"); },
  [&](const TChar& tchar) -> mlc::String { return mlc::String("char32_t"); },
  [&](const TUnknown& tunknown) -> mlc::String { return mlc::String("auto"); },
  [&](const TArray& tarray) -> mlc::String { auto [inner] = tarray; return expr::cpp_array_type_element(sem_type_to_cpp(context, inner)); },
  [&](const TShared& tshared) -> mlc::String { auto [inner] = tshared; return expr::cpp_shared_pointer_type(sem_type_to_cpp(context, inner)); },
  [&](const TNamed& tnamed) -> mlc::String { auto [type_name] = tnamed; return context::context_resolve(context, type_name); },
  [&](const TTuple& ttuple) -> mlc::String { auto [ts] = ttuple; return [&]() -> mlc::String { 
  mlc::Array<mlc::String> tparts = {};
  int ti = 0;
  while (ti < ts.size()){
{
tparts.push_back(sem_type_to_cpp(context, ts[ti]));
ti = ti + 1;
}
}
  return mlc::String("std::tuple<") + tparts.join(mlc::String(", ")) + mlc::String(">");
 }(); },
  [&](const TPair& tpair) -> mlc::String { auto [left, right] = tpair; return mlc::String("std::pair<") + sem_type_to_cpp(context, left) + mlc::String(", ") + sem_type_to_cpp(context, right) + mlc::String(">"); },
  [&](const TGeneric& tgeneric) -> mlc::String { auto [type_name, type_args] = tgeneric; return type_name == mlc::String("Option") && type_args.size() == 1 ? mlc::String("std::optional<") + sem_type_to_cpp(context, type_args[0]) + mlc::String(">") : type_args.size() == 0 ? cpp_generic_base_name(context, type_name) : type_args.size() == 1 ? expr::cpp_template_single_type_argument(cpp_generic_base_name(context, type_name), sem_type_to_cpp(context, type_args[0])) : expr::cpp_template_two_type_arguments(cpp_generic_base_name(context, type_name), sem_type_to_cpp(context, type_args[0]), sem_type_to_cpp(context, type_args[1])); },
  [&](const TFn& tfn) -> mlc::String { auto [param_types, return_type] = tfn; return [&]() -> mlc::String { 
  mlc::Array<mlc::String> parts = {};
  int i = 0;
  while (i < param_types.size()){
{
parts.push_back(sem_type_to_cpp(context, param_types[i]));
i = i + 1;
}
}
  return expr::cpp_std_function_type(sem_type_to_cpp(context, return_type), parts.join(mlc::String(", ")));
 }(); }
}, (*semantic_type));}

mlc::String type_name_to_cpp(context::CodegenContext context, mlc::String type_name) noexcept{return type_name == mlc::String("i32") ? mlc::String("int") : type_name == mlc::String("string") ? mlc::String("mlc::String") : type_name == mlc::String("bool") ? mlc::String("bool") : type_name == mlc::String("unit") ? mlc::String("void") : type_name == mlc::String("i64") ? mlc::String("int64_t") : type_name == mlc::String("f64") ? mlc::String("double") : type_name == mlc::String("u8") ? mlc::String("uint8_t") : type_name == mlc::String("usize") ? mlc::String("size_t") : type_name == mlc::String("char") ? mlc::String("char32_t") : type_name == mlc::String("Self") || type_name == mlc::String("self") ? context.self_type.length() > 0 ? type_name_to_cpp(context, context.self_type) : mlc::String("void") : context::context_resolve(context, type_name);}

mlc::String type_to_cpp(context::CodegenContext context, std::shared_ptr<ast::TypeExpr> type_expr) noexcept{return std::visit(overloaded{
  [&](const TyI32& tyi32) -> mlc::String { return mlc::String("int"); },
  [&](const TyString& tystring) -> mlc::String { return mlc::String("mlc::String"); },
  [&](const TyBool& tybool) -> mlc::String { return mlc::String("bool"); },
  [&](const TyUnit& tyunit) -> mlc::String { return mlc::String("void"); },
  [&](const TyNamed& tynamed) -> mlc::String { auto [name] = tynamed; return type_name_to_cpp(context, name); },
  [&](const TyArray& tyarray) -> mlc::String { auto [inner] = tyarray; return expr::cpp_array_type_element(type_to_cpp(context, inner)); },
  [&](const TyShared& tyshared) -> mlc::String { auto [inner] = tyshared; return expr::cpp_shared_pointer_type(type_to_cpp(context, inner)); },
  [&](const TyGeneric& tygeneric) -> mlc::String { auto [name, targs] = tygeneric; return name == mlc::String("ref") && targs.size() == 1 ? expr::cpp_lvalue_reference_suffix(type_to_cpp(context, targs[0])) : [&]() -> mlc::String { 
  mlc::String safe = name == mlc::String("Map") ? mlc::String("mlc::HashMap") : name == mlc::String("Shared") ? mlc::String("std::shared_ptr") : context::context_resolve(context, name);
  return targs.size() == 0 ? safe : targs.size() == 1 ? expr::cpp_template_single_type_argument(safe, type_to_cpp(context, targs[0])) : expr::cpp_template_two_type_arguments(safe, type_to_cpp(context, targs[0]), type_to_cpp(context, targs[1]));
 }(); },
  [&](const TyFn& tyfn) -> mlc::String { auto [params, ret] = tyfn; return [&]() -> mlc::String { 
  mlc::Array<mlc::String> param_types = {};
  int i = 0;
  while (i < params.size()){
{
param_types.push_back(type_to_cpp(context, params[i]));
i = i + 1;
}
}
  return expr::cpp_std_function_type(type_to_cpp(context, ret), param_types.join(mlc::String(", ")));
 }(); }
}, (*type_expr));}

bool type_param_in_typeexpr(mlc::String param, std::shared_ptr<ast::TypeExpr> t) noexcept{return [&]() { if (std::holds_alternative<ast::TyNamed>((*t))) { auto _v_tynamed = std::get<ast::TyNamed>((*t)); auto [name] = _v_tynamed; return name == param; } if (std::holds_alternative<ast::TyArray>((*t))) { auto _v_tyarray = std::get<ast::TyArray>((*t)); auto [inner] = _v_tyarray; return type_param_in_typeexpr(param, inner); } if (std::holds_alternative<ast::TyShared>((*t))) { auto _v_tyshared = std::get<ast::TyShared>((*t)); auto [inner] = _v_tyshared; return type_param_in_typeexpr(param, inner); } if (std::holds_alternative<ast::TyGeneric>((*t))) { auto _v_tygeneric = std::get<ast::TyGeneric>((*t)); auto [_w0, targs] = _v_tygeneric; return type_param_in_typeexpr_list(param, targs); } if (std::holds_alternative<ast::TyFn>((*t))) { auto _v_tyfn = std::get<ast::TyFn>((*t)); auto [params, ret] = _v_tyfn; return type_param_in_typeexpr_list(param, params) || type_param_in_typeexpr(param, ret); } return false; }();}

bool type_param_in_typeexpr_list(mlc::String param, mlc::Array<std::shared_ptr<ast::TypeExpr>> types) noexcept{
bool found = false;
int i = 0;
while (i < types.size()){
{
if (!found){
{
found = type_param_in_typeexpr(param, types[i]);
}
}
i = i + 1;
}
}
return found;
}

mlc::Array<std::shared_ptr<ast::TypeExpr>> variant_field_typeexprs(std::shared_ptr<ast::TypeVariant> variant) noexcept{return std::visit(overloaded{
  [&](const VarTuple& vartuple) -> mlc::Array<std::shared_ptr<ast::TypeExpr>> { auto [_w0, fts, _w1] = vartuple; return fts; },
  [&](const VarRecord& varrecord) -> mlc::Array<std::shared_ptr<ast::TypeExpr>> { auto [_w0, fds, _w1] = varrecord; return [&]() -> mlc::Array<std::shared_ptr<ast::TypeExpr>> { 
  mlc::Array<std::shared_ptr<ast::TypeExpr>> result = {};
  int i = 0;
  while (i < fds.size()){
{
result.push_back(fds[i]->typ);
i = i + 1;
}
}
  return result;
 }(); },
  [&](const VarUnit& varunit) -> mlc::Array<std::shared_ptr<ast::TypeExpr>> { auto [_w0, _w1] = varunit; return [&]() -> mlc::Array<std::shared_ptr<ast::TypeExpr>> { 
  mlc::Array<std::shared_ptr<ast::TypeExpr>> r = {};
  return r;
 }(); }
}, (*variant));}

mlc::Array<mlc::String> variant_used_type_params(mlc::Array<mlc::String> type_params, std::shared_ptr<ast::TypeVariant> variant) noexcept{
mlc::Array<std::shared_ptr<ast::TypeExpr>> field_types = variant_field_typeexprs(variant);
mlc::Array<mlc::String> used = {};
int i = 0;
while (i < type_params.size()){
{
if (type_param_in_typeexpr_list(type_params[i], field_types)){
{
used.push_back(type_params[i]);
}
}
i = i + 1;
}
}
return used;
}

mlc::Array<mlc::String> union_string_lists(mlc::Array<mlc::String> a, mlc::Array<mlc::String> b) noexcept{
mlc::Array<mlc::String> result = {};
int i = 0;
while (i < a.size()){
{
result.push_back(a[i]);
i = i + 1;
}
}
i = 0;
while (i < b.size()){
{
bool found = false;
int j = 0;
while (j < a.size()){
{
if (a[j] == b[i]){
{
found = true;
}
}
j = j + 1;
}
}
if (!found){
{
result.push_back(b[i]);
}
}
i = i + 1;
}
}
return result;
}

mlc::Array<mlc::String> type_phantom_params_for_variants(mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
mlc::Array<std::shared_ptr<ast::TypeExpr>> all_fields = {};
int vi = 0;
while (vi < variants.size()){
{
mlc::Array<std::shared_ptr<ast::TypeExpr>> fts = variant_field_typeexprs(variants[vi]);
int fi = 0;
while (fi < fts.size()){
{
all_fields.push_back(fts[fi]);
fi = fi + 1;
}
}
vi = vi + 1;
}
}
mlc::Array<mlc::String> phantom = {};
int i = 0;
while (i < type_params.size()){
{
if (!type_param_in_typeexpr_list(type_params[i], all_fields)){
{
phantom.push_back(type_params[i]);
}
}
i = i + 1;
}
}
return phantom;
}

mlc::String variant_ctor_name(std::shared_ptr<ast::TypeVariant> variant) noexcept{return std::visit(overloaded{
  [&](const VarUnit& varunit) -> mlc::String { auto [name, _w0] = varunit; return name; },
  [&](const VarTuple& vartuple) -> mlc::String { auto [name, _w0, _w1] = vartuple; return name; },
  [&](const VarRecord& varrecord) -> mlc::String { auto [name, _w0, _w1] = varrecord; return name; }
}, (*variant));}

mlc::String gen_variant_struct(context::CodegenContext context, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept{return std::visit(overloaded{
  [&](const VarUnit& varunit) -> mlc::String { auto [name, _w0] = varunit; return expr::struct_empty_definition(context::context_resolve(context, name)); },
  [&](const VarTuple& vartuple) -> mlc::String { auto [name, field_types, _w0] = vartuple; return [&]() -> mlc::String { 
  mlc::Array<mlc::String> parts = {};
  int i = 0;
  while (i < field_types.size()){
{
parts.push_back(expr::struct_tuple_field_declaration(type_to_cpp(context, field_types[i]), mlc::to_string(i)));
i = i + 1;
}
}
  return expr::struct_with_inline_members_definition(context::context_resolve(context, name), parts.join(mlc::String("")));
 }(); },
  [&](const VarRecord& varrecord) -> mlc::String { auto [name, field_defs, _w0] = varrecord; return [&]() -> mlc::String { 
  mlc::Array<mlc::String> parts = {};
  int i = 0;
  while (i < field_defs.size()){
{
parts.push_back(expr::struct_named_field_declaration(type_to_cpp(context, field_defs[i]->typ), cpp_naming::cpp_safe(field_defs[i]->name)));
i = i + 1;
}
}
  return expr::struct_with_inline_members_definition(context::context_resolve(context, name), parts.join(mlc::String("")));
 }(); }
}, (*variant));}

mlc::String gen_single_variant(context::CodegenContext context, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept{return std::visit(overloaded{
  [&](const VarRecord& varrecord) -> mlc::String { auto [_w0, field_defs, _w1] = varrecord; return [&]() -> mlc::String { 
  mlc::Array<mlc::String> parts = {};
  int i = 0;
  while (i < field_defs.size()){
{
parts.push_back(expr::struct_named_field_declaration(type_to_cpp(context, field_defs[i]->typ), cpp_naming::cpp_safe(field_defs[i]->name)));
i = i + 1;
}
}
  return expr::struct_with_inline_members_definition(context::context_resolve(context, type_name), parts.join(mlc::String("")));
 }(); },
  [&](const VarTuple& vartuple) -> mlc::String { auto [_w0, field_types, _w1] = vartuple; return [&]() -> mlc::String { 
  mlc::Array<mlc::String> parts = {};
  int i = 0;
  while (i < field_types.size()){
{
parts.push_back(expr::struct_tuple_field_declaration(type_to_cpp(context, field_types[i]), mlc::to_string(i)));
i = i + 1;
}
}
  return expr::struct_with_inline_members_definition(context::context_resolve(context, type_name), parts.join(mlc::String("")));
 }(); },
  [&](const VarUnit& varunit) -> mlc::String { auto [_w0, _w1] = varunit; return expr::struct_empty_definition(context::context_resolve(context, type_name)); }
}, (*variant));}

mlc::String gen_adt_fwd(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
mlc::String full_prefix = cpp_naming::template_prefix(type_params);
mlc::Array<mlc::String> phantom = type_phantom_params_for_variants(type_params, variants);
mlc::Array<mlc::String> parts = {};
mlc::Array<mlc::String> alias_parts = {};
int index = 0;
while (index < variants.size()){
{
std::shared_ptr<ast::TypeVariant> v = variants[index];
mlc::String variant_name = context::context_resolve(context, variant_ctor_name(v));
mlc::Array<mlc::String> used = union_string_lists(variant_used_type_params(type_params, v), phantom);
mlc::String var_prefix = cpp_naming::template_prefix(used);
mlc::String var_targs = used.size() > 0 ? mlc::String("<") + used.join(mlc::String(", ")) + mlc::String(">") : mlc::String("");
alias_parts.push_back(variant_name + var_targs);
parts.push_back(expr::struct_forward_declaration_line(var_prefix, variant_name));
index = index + 1;
}
}
return parts.join(mlc::String("")) + expr::variant_using_alias_definition_line(full_prefix, context::context_resolve(context, type_name), alias_parts.join(mlc::String(", ")));
}

mlc::String gen_adt_defs(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
mlc::Array<mlc::String> phantom = type_phantom_params_for_variants(type_params, variants);
mlc::Array<mlc::String> parts = {};
int i = 0;
while (i < variants.size()){
{
std::shared_ptr<ast::TypeVariant> v = variants[i];
mlc::Array<mlc::String> used = union_string_lists(variant_used_type_params(type_params, v), phantom);
parts.push_back(cpp_naming::template_prefix(used) + gen_variant_struct(context, type_name, v));
i = i + 1;
}
}
return parts.join(mlc::String(""));
}

mlc::String gen_type_decl_fwd(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{return variants.size() == 1 ? mlc::String("") : gen_adt_fwd(context, type_name, type_params, variants);}

mlc::String gen_type_decl_body(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{return variants.size() == 1 ? cpp_naming::template_prefix(type_params) + gen_single_variant(context, type_name, variants[0]) : gen_adt_defs(context, type_name, type_params, variants);}

mlc::String gen_type_decl(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{return gen_type_decl_fwd(context, type_name, type_params, variants) + gen_type_decl_body(context, type_name, type_params, variants);}

mlc::String derive_field_display(mlc::String field_name, std::shared_ptr<ast::TypeExpr> field_type) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::TyString>((*field_type))) {  return mlc::String("self.") + field_name; } return mlc::String("mlc::to_string(self.") + field_name + mlc::String(")"); }();}

mlc::String derive_display_record_fields(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs) noexcept{
mlc::String body = mlc::String("mlc::String(\"") + type_name + mlc::String(" { ") + field_defs[0]->name + mlc::String(": \") + ") + derive_field_display(field_defs[0]->name, field_defs[0]->typ);
int i = 1;
while (i < field_defs.size()){
{
body = body + mlc::String(" + mlc::String(\", ") + field_defs[i]->name + mlc::String(": \") + ") + derive_field_display(field_defs[i]->name, field_defs[i]->typ);
i = i + 1;
}
}
return body + mlc::String(" + mlc::String(\" }\")");
}

mlc::String derive_display_record(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs) noexcept{
mlc::String sig = mlc::String("mlc::String ") + type_name + mlc::String("_to_string(const ") + type_name + mlc::String("& self) noexcept");
return field_defs.size() == 0 ? sig + mlc::String(" { return mlc::String(\"") + type_name + mlc::String(" {}\"); }\n") : sig + mlc::String(" {\n  return ") + derive_display_record_fields(type_name, field_defs) + mlc::String(";\n}\n");
}

mlc::String derive_display_variant_case(std::shared_ptr<ast::TypeVariant> variant) noexcept{return std::visit(overloaded{
  [&](const VarUnit& varunit) -> mlc::String { auto [name, _w0] = varunit; return mlc::String("  if (std::holds_alternative<") + name + mlc::String(">(self._)) return mlc::String(\"") + name + mlc::String("\");\n"); },
  [&](const VarTuple& vartuple) -> mlc::String { auto [name, field_types, _w0] = vartuple; return field_types.size() == 0 ? mlc::String("  if (std::holds_alternative<") + name + mlc::String(">(self._)) return mlc::String(\"") + name + mlc::String("\");\n") : mlc::String("  if (std::holds_alternative<") + name + mlc::String(">(self._)) return mlc::String(\"") + name + mlc::String("(\") + mlc::to_string(std::get<") + name + mlc::String(">(self._)._0) + mlc::String(\")\");\n"); },
  [&](const VarRecord& varrecord) -> mlc::String { auto [name, fds, _w0] = varrecord; return derive_display_variant_record(name, fds); }
}, (*variant));}

mlc::String derive_display_variant_record(mlc::String name, mlc::Array<std::shared_ptr<ast::FieldDef>> fds) noexcept{
return fds.size() == 0 ? mlc::String("  if (std::holds_alternative<") + name + mlc::String(">(self._)) return mlc::String(\"") + name + mlc::String("\");\n") : [&]() -> mlc::String { 
  mlc::String parts = mlc::String("mlc::to_string(std::get<") + name + mlc::String(">(self._).") + fds[0]->name + mlc::String(")");
  int fi = 1;
  while (fi < fds.size()){
{
parts = parts + mlc::String(" + mlc::String(\", \") + mlc::to_string(std::get<") + name + mlc::String(">(self._).") + fds[fi]->name + mlc::String(")");
fi = fi + 1;
}
}
  return mlc::String("  if (std::holds_alternative<") + name + mlc::String(">(self._)) return mlc::String(\"") + name + mlc::String("(\") + ") + parts + mlc::String(" + mlc::String(\")\");\n");
 }();
}

mlc::String derive_display_sum(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
mlc::String cases = mlc::String("");
int i = 0;
while (i < variants.size()){
{
cases = cases + derive_display_variant_case(variants[i]);
i = i + 1;
}
}
return mlc::String("mlc::String ") + type_name + mlc::String("_to_string(const ") + type_name + mlc::String("& self) noexcept {\n") + cases + mlc::String("  return mlc::String(\"\");\n}\n");
}

mlc::String derive_eq_record(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs) noexcept{
mlc::String sig = mlc::String("bool operator==(const ") + type_name + mlc::String("& a, const ") + type_name + mlc::String("& b) noexcept");
return field_defs.size() == 0 ? sig + mlc::String(" { return true; }\n") : [&]() -> mlc::String { 
  mlc::String cond = mlc::String("a.") + field_defs[0]->name + mlc::String(" == b.") + field_defs[0]->name;
  int i = 1;
  while (i < field_defs.size()){
{
cond = cond + mlc::String(" && a.") + field_defs[i]->name + mlc::String(" == b.") + field_defs[i]->name;
i = i + 1;
}
}
  return sig + mlc::String(" { return ") + cond + mlc::String("; }\n");
 }();
}

mlc::String derive_eq_sum(mlc::String type_name) noexcept{return mlc::String("bool operator==(const ") + type_name + mlc::String("& a, const ") + type_name + mlc::String("& b) noexcept { return a._ == b._; }\n");}

mlc::String derive_ord_build_cond(mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs) noexcept{
mlc::String cond = mlc::String("a.") + field_defs[0]->name + mlc::String(" < b.") + field_defs[0]->name;
int i = 1;
while (i < field_defs.size()){
{
mlc::String prev_eq = mlc::String("a.") + field_defs[0]->name + mlc::String(" == b.") + field_defs[0]->name;
int j = 1;
while (j < i){
{
prev_eq = prev_eq + mlc::String(" && a.") + field_defs[j]->name + mlc::String(" == b.") + field_defs[j]->name;
j = j + 1;
}
}
cond = cond + mlc::String(" || (") + prev_eq + mlc::String(" && a.") + field_defs[i]->name + mlc::String(" < b.") + field_defs[i]->name + mlc::String(")");
i = i + 1;
}
}
return cond;
}

mlc::String derive_ord_record(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs) noexcept{
mlc::String sig = mlc::String("bool operator<(const ") + type_name + mlc::String("& a, const ") + type_name + mlc::String("& b) noexcept");
return field_defs.size() == 0 ? sig + mlc::String(" { return false; }\n") : sig + mlc::String(" { return ") + derive_ord_build_cond(field_defs) + mlc::String("; }\n");
}

mlc::String derive_ord_sum(mlc::String type_name) noexcept{return mlc::String("bool operator<(const ") + type_name + mlc::String("& a, const ") + type_name + mlc::String("& b) noexcept { return a._ < b._; }\n");}

bool variants_is_single_record(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{return variants.size() != 1 ? false : [&]() { if (std::holds_alternative<ast::VarRecord>((*variants[0]))) { auto _v_varrecord = std::get<ast::VarRecord>((*variants[0])); auto [_w0, _w1, _w2] = _v_varrecord; return true; } return false; }();}

mlc::Array<std::shared_ptr<ast::FieldDef>> derive_record_field_defs(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{return [&]() -> mlc::Array<std::shared_ptr<ast::FieldDef>> { if (std::holds_alternative<ast::VarRecord>((*variants[0]))) { auto _v_varrecord = std::get<ast::VarRecord>((*variants[0])); auto [_w0, fds, _w1] = _v_varrecord; return fds; } return {}; }();}

mlc::String gen_derive_record_trait(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::String trait_name) noexcept{
mlc::Array<std::shared_ptr<ast::FieldDef>> fds = derive_record_field_defs(variants);
return trait_name == mlc::String("Display") ? derive_display_record(type_name, fds) : trait_name == mlc::String("Eq") ? derive_eq_record(type_name, fds) : trait_name == mlc::String("Ord") ? derive_ord_record(type_name, fds) : mlc::String("");
}

mlc::String gen_derive_sum_trait(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::String trait_name) noexcept{return trait_name == mlc::String("Display") ? derive_display_sum(type_name, variants) : trait_name == mlc::String("Eq") ? derive_eq_sum(type_name) : trait_name == mlc::String("Ord") ? derive_ord_sum(type_name) : mlc::String("");}

mlc::String gen_derive_methods(context::CodegenContext context, mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::Array<mlc::String> derive_traits) noexcept{
return derive_traits.size() == 0 ? mlc::String("") : [&]() -> mlc::String { 
  bool is_record = variants_is_single_record(variants);
  mlc::String result = mlc::String("");
  int i = 0;
  while (i < derive_traits.size()){
{
mlc::String trait_code = is_record ? gen_derive_record_trait(type_name, variants, derive_traits[i]) : gen_derive_sum_trait(type_name, variants, derive_traits[i]);
result = result + trait_code;
i = i + 1;
}
}
  return result;
 }();
}

mlc::String gen_type_decl_fwd_only(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{return gen_type_decl_fwd(context, type_name, type_params, variants);}

mlc::String gen_type_decl_body_only(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{return gen_type_decl_body(context, type_name, type_params, variants);}

mlc::String requires_clause(mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds) noexcept{
mlc::Array<mlc::String> parts = {};
int i = 0;
while (i < type_params.size()){
{
if (i < type_bounds.size()){
{
int k = 0;
[&]() { 
  while (k < type_bounds[i].size()){
{
parts.push_back(expr::concept_trait_constraint_on_type_parameter(cpp_naming::cpp_safe(type_bounds[i][k]), type_params[i]));
k = k + 1;
}
}
 }();
}
}
i = i + 1;
}
}
return parts.size() > 0 ? expr::concept_requires_clause_line(parts.join(mlc::String(" && "))) : mlc::String("");
}

} // namespace type_gen
