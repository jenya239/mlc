#include "type_gen.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "record_field_default_initializer.hpp"
#include "context.hpp"
#include "cpp_naming.hpp"
#include "expr.hpp"

namespace type_gen {

using namespace ast;
using namespace registry;
using namespace record_field_default_initializer;
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

mlc::String field_def_member_declaration(context::CodegenContext context, std::shared_ptr<ast::FieldDef> fd) noexcept;

mlc::String variant_record_struct_inline_member_declarations(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept;

mlc::String variant_ctor_name(std::shared_ptr<ast::TypeVariant> variant) noexcept;

mlc::String gen_variant_struct(context::CodegenContext context, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept;

mlc::String struct_extra_using(context::CodegenContext context, mlc::String type_name) noexcept;

mlc::String gen_single_variant(context::CodegenContext context, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept;

mlc::String variant_forward_decl_line(context::CodegenContext context, mlc::Array<mlc::String> type_params, mlc::Array<mlc::String> phantom, std::shared_ptr<ast::TypeVariant> v) noexcept;

mlc::String variant_alias_part(context::CodegenContext context, mlc::Array<mlc::String> type_params, mlc::Array<mlc::String> phantom, std::shared_ptr<ast::TypeVariant> v) noexcept;

mlc::String gen_adt_fwd(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::String variant_def_with_prefix(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::String> phantom, std::shared_ptr<ast::TypeVariant> v) noexcept;

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

mlc::String derive_hash_std_cpp_type(std::shared_ptr<ast::TypeExpr> field_type) noexcept;

mlc::String derive_hash_combine_line(mlc::String cpp_type, mlc::String access_expr) noexcept;

mlc::String derive_hash_record_operator_body(mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs) noexcept;

mlc::String derive_hash_record(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs) noexcept;

mlc::String derive_hash_tuple_variant_inner(mlc::String variant_struct_name, mlc::Array<std::shared_ptr<ast::TypeExpr>> field_types, int discriminant_index) noexcept;

mlc::String derive_hash_record_variant_inner(mlc::String variant_struct_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs, int discriminant_index) noexcept;

mlc::String derive_hash_sum_variant_inner(std::shared_ptr<ast::TypeVariant> variant, int discriminant_index) noexcept;

mlc::String type_variant_constructor_name(std::shared_ptr<ast::TypeVariant> variant) noexcept;

mlc::String derive_hash_sum_branch(std::shared_ptr<ast::TypeVariant> variant, int discriminant_index) noexcept;

mlc::String derive_hash_sum_operator_body(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::String derive_hash_sum(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

bool variants_is_single_record(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::Array<std::shared_ptr<ast::FieldDef>> derive_record_field_defs(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::String gen_derive_record_trait(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::String trait_name) noexcept;

mlc::String gen_derive_sum_trait(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::String trait_name) noexcept;

mlc::String gen_derive_methods(context::CodegenContext context, mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::Array<mlc::String> derive_traits) noexcept;

mlc::String gen_type_decl_fwd_only(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::String gen_type_decl_body_only(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::String requires_clause(mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds) noexcept;

mlc::String cpp_generic_base_name(context::CodegenContext context, mlc::String type_name) noexcept{return type_name == mlc::String("Map") ? mlc::String("mlc::HashMap") : type_name == mlc::String("Shared") ? mlc::String("std::shared_ptr") : context::CodegenContext_resolve(context, type_name);}

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
  [&](const TNamed& tnamed) -> mlc::String { auto [type_name] = tnamed; return context::CodegenContext_resolve(context, type_name); },
  [&](const TTuple& ttuple) -> mlc::String { auto [types] = ttuple; return mlc::String("std::tuple<") + types.map([context](std::shared_ptr<registry::Type> t)  { return sem_type_to_cpp(context, t); }).join(mlc::String(", ")) + mlc::String(">"); },
  [&](const TPair& tpair) -> mlc::String { auto [left, right] = tpair; return mlc::String("std::pair<") + sem_type_to_cpp(context, left) + mlc::String(", ") + sem_type_to_cpp(context, right) + mlc::String(">"); },
  [&](const TGeneric& tgeneric) -> mlc::String { auto [type_name, type_args] = tgeneric; return type_name == mlc::String("Option") && type_args.size() == 1 ? mlc::String("std::optional<") + sem_type_to_cpp(context, type_args[0]) + mlc::String(">") : type_args.size() == 0 ? cpp_generic_base_name(context, type_name) : type_args.size() == 1 ? expr::cpp_template_single_type_argument(cpp_generic_base_name(context, type_name), sem_type_to_cpp(context, type_args[0])) : expr::cpp_template_two_type_arguments(cpp_generic_base_name(context, type_name), sem_type_to_cpp(context, type_args[0]), sem_type_to_cpp(context, type_args[1])); },
  [&](const TFn& tfn) -> mlc::String { auto [param_types, return_type] = tfn; return expr::cpp_std_function_type(sem_type_to_cpp(context, return_type), param_types.map([context](std::shared_ptr<registry::Type> t)  { return sem_type_to_cpp(context, t); }).join(mlc::String(", "))); },
  [&](const TAssoc& tassoc) -> mlc::String { auto [param, assoc] = tassoc; return mlc::String("typename ") + cpp_naming::cpp_safe(param) + mlc::String("::") + assoc; }
}, (*semantic_type));}

mlc::String type_name_to_cpp(context::CodegenContext context, mlc::String type_name) noexcept{return type_name == mlc::String("i32") ? mlc::String("int") : type_name == mlc::String("string") ? mlc::String("mlc::String") : type_name == mlc::String("bool") ? mlc::String("bool") : type_name == mlc::String("unit") ? mlc::String("void") : type_name == mlc::String("i64") ? mlc::String("int64_t") : type_name == mlc::String("f64") ? mlc::String("double") : type_name == mlc::String("u8") ? mlc::String("uint8_t") : type_name == mlc::String("usize") ? mlc::String("size_t") : type_name == mlc::String("char") ? mlc::String("char32_t") : type_name == mlc::String("Self") || type_name == mlc::String("self") ? context.self_type.length() > 0 ? type_name_to_cpp(context, context.self_type) : mlc::String("void") : context::CodegenContext_resolve(context, type_name);}

mlc::String type_to_cpp(context::CodegenContext context, std::shared_ptr<ast::TypeExpr> type_expr) noexcept{return std::visit(overloaded{
  [&](const TyI32& tyi32) -> mlc::String { return mlc::String("int"); },
  [&](const TyString& tystring) -> mlc::String { return mlc::String("mlc::String"); },
  [&](const TyBool& tybool) -> mlc::String { return mlc::String("bool"); },
  [&](const TyUnit& tyunit) -> mlc::String { return mlc::String("void"); },
  [&](const TyNamed& tynamed) -> mlc::String { auto [name] = tynamed; return type_name_to_cpp(context, name); },
  [&](const TyArray& tyarray) -> mlc::String { auto [inner] = tyarray; return expr::cpp_array_type_element(type_to_cpp(context, inner)); },
  [&](const TyShared& tyshared) -> mlc::String { auto [inner] = tyshared; return expr::cpp_shared_pointer_type(type_to_cpp(context, inner)); },
  [&](const TyGeneric& tygeneric) -> mlc::String { auto [name, targs] = tygeneric; return name == mlc::String("ref") && targs.size() == 1 ? expr::cpp_lvalue_reference_suffix(type_to_cpp(context, targs[0])) : [&]() -> mlc::String { 
  mlc::String safe = name == mlc::String("Map") ? mlc::String("mlc::HashMap") : name == mlc::String("Shared") ? mlc::String("std::shared_ptr") : context::CodegenContext_resolve(context, name);
  return targs.size() == 0 ? safe : targs.size() == 1 ? expr::cpp_template_single_type_argument(safe, type_to_cpp(context, targs[0])) : expr::cpp_template_two_type_arguments(safe, type_to_cpp(context, targs[0]), type_to_cpp(context, targs[1]));
 }(); },
  [&](const TyFn& tyfn) -> mlc::String { auto [params, ret] = tyfn; return expr::cpp_std_function_type(type_to_cpp(context, ret), params.map([context](std::shared_ptr<ast::TypeExpr> p)  { return type_to_cpp(context, p); }).join(mlc::String(", "))); },
  [&](const TyAssoc& tyassoc) -> mlc::String { auto [param, assoc] = tyassoc; return mlc::String("typename ") + cpp_naming::cpp_safe(param) + mlc::String("::") + assoc; }
}, (*type_expr));}

bool type_param_in_typeexpr(mlc::String param, std::shared_ptr<ast::TypeExpr> t) noexcept{return [&]() { if (std::holds_alternative<ast::TyNamed>((*t))) { auto _v_tynamed = std::get<ast::TyNamed>((*t)); auto [name] = _v_tynamed; return name == param; } if (std::holds_alternative<ast::TyArray>((*t))) { auto _v_tyarray = std::get<ast::TyArray>((*t)); auto [inner] = _v_tyarray; return type_param_in_typeexpr(param, inner); } if (std::holds_alternative<ast::TyShared>((*t))) { auto _v_tyshared = std::get<ast::TyShared>((*t)); auto [inner] = _v_tyshared; return type_param_in_typeexpr(param, inner); } if (std::holds_alternative<ast::TyGeneric>((*t))) { auto _v_tygeneric = std::get<ast::TyGeneric>((*t)); auto [_w0, targs] = _v_tygeneric; return type_param_in_typeexpr_list(param, targs); } if (std::holds_alternative<ast::TyFn>((*t))) { auto _v_tyfn = std::get<ast::TyFn>((*t)); auto [params, ret] = _v_tyfn; return type_param_in_typeexpr_list(param, params) || type_param_in_typeexpr(param, ret); } if (std::holds_alternative<ast::TyAssoc>((*t))) { auto _v_tyassoc = std::get<ast::TyAssoc>((*t)); auto [p, _w0] = _v_tyassoc; return p == param; } return false; }();}

bool type_param_in_typeexpr_list(mlc::String param, mlc::Array<std::shared_ptr<ast::TypeExpr>> types) noexcept{return types.any([param](std::shared_ptr<ast::TypeExpr> t)  { return type_param_in_typeexpr(param, t); });}

mlc::Array<std::shared_ptr<ast::TypeExpr>> variant_field_typeexprs(std::shared_ptr<ast::TypeVariant> variant) noexcept{return std::visit(overloaded{
  [&](const VarTuple& vartuple) -> mlc::Array<std::shared_ptr<ast::TypeExpr>> { auto [_w0, fts, _w1] = vartuple; return fts; },
  [&](const VarRecord& varrecord) -> mlc::Array<std::shared_ptr<ast::TypeExpr>> { auto [_w0, fds, _w1] = varrecord; return fds.map([](std::shared_ptr<ast::FieldDef> fd)  { return fd->typ; }); },
  [&](const VarUnit& varunit) -> mlc::Array<std::shared_ptr<ast::TypeExpr>> { auto [_w0, _w1] = varunit; return {}; }
}, (*variant));}

mlc::Array<mlc::String> variant_used_type_params(mlc::Array<mlc::String> type_params, std::shared_ptr<ast::TypeVariant> variant) noexcept{
mlc::Array<std::shared_ptr<ast::TypeExpr>> field_types = variant_field_typeexprs(variant);
return type_params.filter([field_types](mlc::String p)  { return type_param_in_typeexpr_list(p, field_types); });
}

mlc::Array<mlc::String> union_string_lists(mlc::Array<mlc::String> a, mlc::Array<mlc::String> b) noexcept{return a.concat(b.filter([a](mlc::String s)  { return !a.any([s](mlc::String x)  { return x == s; }); }));}

mlc::Array<mlc::String> type_phantom_params_for_variants(mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
auto all_fields = mlc::collections::flat_map(variants, [](std::shared_ptr<ast::TypeVariant> v)  { return variant_field_typeexprs(v); });
return type_params.filter([all_fields](mlc::String p)  { return !type_param_in_typeexpr_list(p, all_fields); });
}

mlc::String field_def_member_declaration(context::CodegenContext context, std::shared_ptr<ast::FieldDef> fd) noexcept{
mlc::String base = expr::struct_named_field_declaration(type_to_cpp(context, fd->typ), cpp_naming::cpp_safe(fd->name));
return fd->has_default_expression && record_field_default_initializer::record_field_default_expression_acceptable_for_codegen(fd->default_expression) ? base + mlc::String(" = ") + record_field_default_initializer::record_field_default_expression_cpp_initializer(fd->default_expression, context) : base;
}

mlc::String variant_record_struct_inline_member_declarations(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept{return field_definitions.map([context](std::shared_ptr<ast::FieldDef> fd)  { return field_def_member_declaration(context, fd); }).join(mlc::String(""));}

mlc::String variant_ctor_name(std::shared_ptr<ast::TypeVariant> variant) noexcept{return std::visit(overloaded{
  [&](const VarUnit& varunit) -> mlc::String { auto [name, _w0] = varunit; return name; },
  [&](const VarTuple& vartuple) -> mlc::String { auto [name, _w0, _w1] = vartuple; return name; },
  [&](const VarRecord& varrecord) -> mlc::String { auto [name, _w0, _w1] = varrecord; return name; }
}, (*variant));}

mlc::String gen_variant_struct(context::CodegenContext context, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept{return std::visit(overloaded{
  [&](const VarUnit& varunit) -> mlc::String { auto [name, _w0] = varunit; return expr::struct_empty_definition(context::CodegenContext_resolve(context, name)); },
  [&](const VarTuple& vartuple) -> mlc::String { auto [name, field_types, _w0] = vartuple; return [&]() -> mlc::String { 
  mlc::Array<mlc::String> parts = {};
  int i = 0;
  while (i < field_types.size()){
{
parts.push_back(expr::struct_tuple_field_declaration(type_to_cpp(context, field_types[i]), mlc::to_string(i)));
i = i + 1;
}
}
  return expr::struct_with_inline_members_definition(context::CodegenContext_resolve(context, name), parts.join(mlc::String("")));
 }(); },
  [&](const VarRecord& varrecord) -> mlc::String { auto [name, field_defs, _w0] = varrecord; return expr::struct_with_inline_members_definition(context::CodegenContext_resolve(context, name), variant_record_struct_inline_member_declarations(context, field_defs)); }
}, (*variant));}

mlc::String struct_extra_using(context::CodegenContext context, mlc::String type_name) noexcept{return context.struct_using_lines.has(type_name) ? context.struct_using_lines.get(type_name).join(mlc::String("")) : mlc::String("");}

mlc::String gen_single_variant(context::CodegenContext context, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept{return std::visit(overloaded{
  [&](const VarRecord& varrecord) -> mlc::String { auto [_w0, field_defs, _w1] = varrecord; return expr::struct_with_inline_members_definition(context::CodegenContext_resolve(context, type_name), variant_record_struct_inline_member_declarations(context, field_defs) + struct_extra_using(context, type_name)); },
  [&](const VarTuple& vartuple) -> mlc::String { auto [_w0, field_types, _w1] = vartuple; return [&]() -> mlc::String { 
  mlc::Array<mlc::String> parts = {};
  int i = 0;
  while (i < field_types.size()){
{
parts.push_back(expr::struct_tuple_field_declaration(type_to_cpp(context, field_types[i]), mlc::to_string(i)));
i = i + 1;
}
}
  return expr::struct_with_inline_members_definition(context::CodegenContext_resolve(context, type_name), parts.join(mlc::String("")) + struct_extra_using(context, type_name));
 }(); },
  [&](const VarUnit& varunit) -> mlc::String { auto [_w0, _w1] = varunit; return expr::struct_empty_definition(context::CodegenContext_resolve(context, type_name)); }
}, (*variant));}

mlc::String variant_forward_decl_line(context::CodegenContext context, mlc::Array<mlc::String> type_params, mlc::Array<mlc::String> phantom, std::shared_ptr<ast::TypeVariant> v) noexcept{
mlc::String variant_name = context::CodegenContext_resolve(context, variant_ctor_name(v));
mlc::Array<mlc::String> used = union_string_lists(variant_used_type_params(type_params, v), phantom);
return expr::struct_forward_declaration_line(cpp_naming::template_prefix(used), variant_name);
}

mlc::String variant_alias_part(context::CodegenContext context, mlc::Array<mlc::String> type_params, mlc::Array<mlc::String> phantom, std::shared_ptr<ast::TypeVariant> v) noexcept{
mlc::String variant_name = context::CodegenContext_resolve(context, variant_ctor_name(v));
mlc::Array<mlc::String> used = union_string_lists(variant_used_type_params(type_params, v), phantom);
mlc::String var_targs = used.size() > 0 ? mlc::String("<") + used.join(mlc::String(", ")) + mlc::String(">") : mlc::String("");
return variant_name + var_targs;
}

mlc::String gen_adt_fwd(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
mlc::String full_prefix = cpp_naming::template_prefix(type_params);
mlc::Array<mlc::String> phantom = type_phantom_params_for_variants(type_params, variants);
mlc::String forward_decls = variants.map([context, type_params, phantom](std::shared_ptr<ast::TypeVariant> v)  { return variant_forward_decl_line(context, type_params, phantom, v); }).join(mlc::String(""));
mlc::String alias_parts = variants.map([context, type_params, phantom](std::shared_ptr<ast::TypeVariant> v)  { return variant_alias_part(context, type_params, phantom, v); }).join(mlc::String(", "));
return forward_decls + expr::variant_using_alias_definition_line(full_prefix, context::CodegenContext_resolve(context, type_name), alias_parts);
}

mlc::String variant_def_with_prefix(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::String> phantom, std::shared_ptr<ast::TypeVariant> v) noexcept{
mlc::Array<mlc::String> used = union_string_lists(variant_used_type_params(type_params, v), phantom);
return cpp_naming::template_prefix(used) + gen_variant_struct(context, type_name, v);
}

mlc::String gen_adt_defs(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
mlc::Array<mlc::String> phantom = type_phantom_params_for_variants(type_params, variants);
return variants.map([context, type_name, type_params, phantom](std::shared_ptr<ast::TypeVariant> v)  { return variant_def_with_prefix(context, type_name, type_params, phantom, v); }).join(mlc::String(""));
}

mlc::String gen_type_decl_fwd(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{return variants.size() == 1 ? mlc::String("") : gen_adt_fwd(context, type_name, type_params, variants);}

mlc::String gen_type_decl_body(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{return variants.size() == 1 ? cpp_naming::template_prefix(type_params) + gen_single_variant(context, type_name, variants[0]) : gen_adt_defs(context, type_name, type_params, variants);}

mlc::String gen_type_decl(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{return gen_type_decl_fwd(context, type_name, type_params, variants) + gen_type_decl_body(context, type_name, type_params, variants);}

mlc::String derive_field_display(mlc::String field_name, std::shared_ptr<ast::TypeExpr> field_type) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::TyString>((*field_type))) {  return mlc::String("self.") + field_name; } return mlc::String("mlc::to_string(self.") + field_name + mlc::String(")"); }();}

mlc::String derive_display_record_fields(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs) noexcept{return mlc::String("mlc::String(\"") + type_name + mlc::String(" { ") + field_defs[0]->name + mlc::String(": \") + ") + derive_field_display(field_defs[0]->name, field_defs[0]->typ) + field_defs.drop(1).fold(mlc::String(""), [](mlc::String acc, std::shared_ptr<ast::FieldDef> fd)  { return acc + mlc::String(" + mlc::String(\", ") + fd->name + mlc::String(": \") + ") + derive_field_display(fd->name, fd->typ); }) + mlc::String(" + mlc::String(\" }\")");}

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
  mlc::String parts = mlc::String("mlc::to_string(std::get<") + name + mlc::String(">(self._).") + fds[0]->name + mlc::String(")") + fds.drop(1).fold(mlc::String(""), [name](mlc::String acc, std::shared_ptr<ast::FieldDef> fd)  { return acc + mlc::String(" + mlc::String(\", \") + mlc::to_string(std::get<") + name + mlc::String(">(self._).") + fd->name + mlc::String(")"); });
  return mlc::String("  if (std::holds_alternative<") + name + mlc::String(">(self._)) return mlc::String(\"") + name + mlc::String("(\") + ") + parts + mlc::String(" + mlc::String(\")\");\n");
 }();
}

mlc::String derive_display_sum(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
mlc::String cases = variants.map([](std::shared_ptr<ast::TypeVariant> v)  { return derive_display_variant_case(v); }).join(mlc::String(""));
return mlc::String("mlc::String ") + type_name + mlc::String("_to_string(const ") + type_name + mlc::String("& self) noexcept {\n") + cases + mlc::String("  return mlc::String(\"\");\n}\n");
}

mlc::String derive_eq_record(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs) noexcept{
mlc::String sig = mlc::String("bool operator==(const ") + type_name + mlc::String("& a, const ") + type_name + mlc::String("& b) noexcept");
return field_defs.size() == 0 ? sig + mlc::String(" { return true; }\n") : [&]() -> mlc::String { 
  mlc::String cond = mlc::String("a.") + field_defs[0]->name + mlc::String(" == b.") + field_defs[0]->name + field_defs.drop(1).fold(mlc::String(""), [](mlc::String acc, std::shared_ptr<ast::FieldDef> fd)  { return acc + mlc::String(" && a.") + fd->name + mlc::String(" == b.") + fd->name; });
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

mlc::String derive_hash_std_cpp_type(std::shared_ptr<ast::TypeExpr> field_type) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::TyString>((*field_type))) {  return mlc::String("mlc::String"); } if (std::holds_alternative<ast::TyI32>((*field_type))) {  return mlc::String("int"); } if (std::holds_alternative<ast::TyBool>((*field_type))) {  return mlc::String("bool"); } return mlc::String(""); }();}

mlc::String derive_hash_combine_line(mlc::String cpp_type, mlc::String access_expr) noexcept{return mlc::String("h ^= std::hash<") + cpp_type + mlc::String(">{}(") + access_expr + mlc::String(") + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);\n");}

mlc::String derive_hash_record_operator_body(mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs) noexcept{return field_defs.size() == 0 ? mlc::String("return static_cast<size_t>(1469598103934665603ULL);\n") : field_defs.fold(mlc::String("size_t h = 1469598103934665603ULL;\n"), [](mlc::String acc, std::shared_ptr<ast::FieldDef> fd)  { return acc + derive_hash_combine_line(derive_hash_std_cpp_type(fd->typ), mlc::String("self.") + fd->name); }) + mlc::String("return h;\n");}

mlc::String derive_hash_record(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs) noexcept{
mlc::String operator_body = derive_hash_record_operator_body(field_defs);
return mlc::String("namespace std {\ntemplate<>\nstruct hash<") + type_name + mlc::String("> {\n  size_t operator()(const ") + type_name + mlc::String("& self) const noexcept {\n    ") + operator_body + mlc::String("  }\n};\n}\n");
}

mlc::String derive_hash_tuple_variant_inner(mlc::String variant_struct_name, mlc::Array<std::shared_ptr<ast::TypeExpr>> field_types, int discriminant_index) noexcept{
mlc::String inner = derive_hash_combine_line(mlc::String("size_t"), mlc::to_string(discriminant_index));
int tuple_index = 0;
while (tuple_index < field_types.size()){
{
mlc::String slot_access = mlc::String("std::get<") + variant_struct_name + mlc::String(">(self._).field") + mlc::to_string(tuple_index);
inner = inner + derive_hash_combine_line(derive_hash_std_cpp_type(field_types[tuple_index]), slot_access);
tuple_index = tuple_index + 1;
}
}
return inner;
}

mlc::String derive_hash_record_variant_inner(mlc::String variant_struct_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs, int discriminant_index) noexcept{return field_defs.fold(derive_hash_combine_line(mlc::String("size_t"), mlc::to_string(discriminant_index)), [variant_struct_name](mlc::String acc, std::shared_ptr<ast::FieldDef> fd)  { return acc + derive_hash_combine_line(derive_hash_std_cpp_type(fd->typ), mlc::String("std::get<") + variant_struct_name + mlc::String(">(self._).") + fd->name); });}

mlc::String derive_hash_sum_variant_inner(std::shared_ptr<ast::TypeVariant> variant, int discriminant_index) noexcept{return std::visit(overloaded{
  [&](const VarUnit& varunit) -> mlc::String { auto [_w0, _w1] = varunit; return derive_hash_combine_line(mlc::String("size_t"), mlc::to_string(discriminant_index)); },
  [&](const VarTuple& vartuple) -> mlc::String { auto [vname, fts, _w0] = vartuple; return derive_hash_tuple_variant_inner(vname, fts, discriminant_index); },
  [&](const VarRecord& varrecord) -> mlc::String { auto [vname, fds, _w0] = varrecord; return derive_hash_record_variant_inner(vname, fds, discriminant_index); }
}, (*variant));}

mlc::String type_variant_constructor_name(std::shared_ptr<ast::TypeVariant> variant) noexcept{return std::visit(overloaded{
  [&](const VarUnit& varunit) -> mlc::String { auto [name, _w0] = varunit; return name; },
  [&](const VarTuple& vartuple) -> mlc::String { auto [name, _w0, _w1] = vartuple; return name; },
  [&](const VarRecord& varrecord) -> mlc::String { auto [name, _w0, _w1] = varrecord; return name; }
}, (*variant));}

mlc::String derive_hash_sum_branch(std::shared_ptr<ast::TypeVariant> variant, int discriminant_index) noexcept{
mlc::String constructor_name = type_variant_constructor_name(variant);
mlc::String variant_inner = derive_hash_sum_variant_inner(variant, discriminant_index);
return mlc::String("if (std::holds_alternative<") + constructor_name + mlc::String(">(self._)) {\n    ") + variant_inner + mlc::String("    return h;\n  }\n");
}

mlc::String derive_hash_sum_operator_body(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
mlc::String inner = mlc::String("size_t h = 1469598103934665603ULL;\n");
int variant_index = 0;
while (variant_index < variants.size()){
{
inner = inner + derive_hash_sum_branch(variants[variant_index], variant_index);
variant_index = variant_index + 1;
}
}
return inner + mlc::String("return h;\n");
}

mlc::String derive_hash_sum(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
mlc::String operator_body = derive_hash_sum_operator_body(variants);
return mlc::String("namespace std {\ntemplate<>\nstruct hash<") + type_name + mlc::String("> {\n  size_t operator()(const ") + type_name + mlc::String("& self) const noexcept {\n    ") + operator_body + mlc::String("  }\n};\n}\n");
}

bool variants_is_single_record(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{return variants.size() != 1 ? false : [&]() { if (std::holds_alternative<ast::VarRecord>((*variants[0]))) { auto _v_varrecord = std::get<ast::VarRecord>((*variants[0])); auto [_w0, _w1, _w2] = _v_varrecord; return true; } return false; }();}

mlc::Array<std::shared_ptr<ast::FieldDef>> derive_record_field_defs(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
mlc::Array<std::shared_ptr<ast::FieldDef>> empty = {};
return [&]() -> mlc::Array<std::shared_ptr<ast::FieldDef>> { if (std::holds_alternative<ast::VarRecord>((*variants[0]))) { auto _v_varrecord = std::get<ast::VarRecord>((*variants[0])); auto [_w0, fds, _w1] = _v_varrecord; return fds; } return empty; }();
}

mlc::String gen_derive_record_trait(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::String trait_name) noexcept{
mlc::Array<std::shared_ptr<ast::FieldDef>> fds = derive_record_field_defs(variants);
return trait_name == mlc::String("Display") ? derive_display_record(type_name, fds) : trait_name == mlc::String("Eq") ? derive_eq_record(type_name, fds) : trait_name == mlc::String("Ord") ? derive_ord_record(type_name, fds) : trait_name == mlc::String("Hash") ? derive_hash_record(type_name, fds) : mlc::String("");
}

mlc::String gen_derive_sum_trait(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::String trait_name) noexcept{return trait_name == mlc::String("Display") ? derive_display_sum(type_name, variants) : trait_name == mlc::String("Eq") ? derive_eq_sum(type_name) : trait_name == mlc::String("Ord") ? derive_ord_sum(type_name) : trait_name == mlc::String("Hash") ? derive_hash_sum(type_name, variants) : mlc::String("");}

mlc::String gen_derive_methods(context::CodegenContext context, mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::Array<mlc::String> derive_traits) noexcept{
return derive_traits.size() == 0 ? mlc::String("") : [&]() -> mlc::String { 
  bool is_record = variants_is_single_record(variants);
  return derive_traits.map([is_record, type_name, variants](mlc::String trait_name)  { return is_record ? gen_derive_record_trait(type_name, variants, trait_name) : gen_derive_sum_trait(type_name, variants, trait_name); }).join(mlc::String(""));
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
