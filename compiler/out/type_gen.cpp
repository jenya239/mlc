#include "type_gen.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "record_defaults.hpp"
#include "context.hpp"
#include "decl_index.hpp"
#include "cpp_naming.hpp"
#include "expr.hpp"

namespace type_gen {

using namespace ast;
using namespace registry;
using namespace record_defaults;
using namespace context;
using namespace decl_index;
using namespace cpp_naming;
using namespace expr;
using namespace ast_tokens;

struct TupleStructCodegenFoldState {int field_slot_index;mlc::String declaration_text_so_far;};

struct TupleHashCodegenFoldState {int tuple_field_slot;mlc::String concatenated_combine_lines;};

struct HashSumOperatorFoldState {int next_discriminant_index;mlc::String operator_body_text;};

mlc::String cpp_generic_base_name(context::CodegenContext context, mlc::String type_name) noexcept;

mlc::String sem_type_to_cpp(context::CodegenContext context, std::shared_ptr<registry::Type> semantic_type) noexcept;

mlc::String type_alias_annotation_cpp(context::CodegenContext context, mlc::String alias_name) noexcept;

mlc::String type_name_to_cpp(context::CodegenContext context, mlc::String type_name) noexcept;

mlc::String type_to_cpp(context::CodegenContext context, std::shared_ptr<ast::TypeExpr> type_expr) noexcept;

bool type_param_in_typeexpr(mlc::String param, std::shared_ptr<ast::TypeExpr> type_expression) noexcept;

bool type_param_in_typeexpr_list(mlc::String param, mlc::Array<std::shared_ptr<ast::TypeExpr>> types) noexcept;

mlc::Array<std::shared_ptr<ast::TypeExpr>> variant_field_typeexprs(std::shared_ptr<ast::TypeVariant> variant) noexcept;

mlc::Array<mlc::String> variant_used_type_parameters(mlc::Array<mlc::String> type_parameters, std::shared_ptr<ast::TypeVariant> variant) noexcept;

mlc::Array<mlc::String> union_string_lists(mlc::Array<mlc::String> first, mlc::Array<mlc::String> second) noexcept;

mlc::Array<mlc::String> type_phantom_params_for_variants(mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::String field_def_member_declaration(context::CodegenContext context, std::shared_ptr<ast::FieldDef> field_definition) noexcept;

mlc::String variant_record_struct_inline_member_declarations(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept;

mlc::String variant_ctor_name(std::shared_ptr<ast::TypeVariant> variant) noexcept;

mlc::String tuple_variant_struct_members_fragment(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::TypeExpr>> field_types) noexcept;

mlc::String gen_variant_struct(context::CodegenContext context, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept;

mlc::String struct_extra_using(context::CodegenContext context, mlc::String type_name) noexcept;

mlc::String gen_single_variant(context::CodegenContext context, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept;

mlc::String variant_forward_decl_line(context::CodegenContext context, mlc::Array<mlc::String> type_parameters, mlc::Array<mlc::String> phantom, std::shared_ptr<ast::TypeVariant> variant) noexcept;

mlc::String variant_alias_part(context::CodegenContext context, mlc::Array<mlc::String> type_parameters, mlc::Array<mlc::String> phantom, std::shared_ptr<ast::TypeVariant> variant) noexcept;

mlc::String gen_adt_fwd(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::String variant_def_with_prefix(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_parameters, mlc::Array<mlc::String> phantom, std::shared_ptr<ast::TypeVariant> variant) noexcept;

mlc::String gen_adt_defs(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::String gen_type_decl_fwd(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::String gen_type_decl_body(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::String gen_type_decl(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::String derive_field_display(mlc::String field_name, std::shared_ptr<ast::TypeExpr> field_type) noexcept;

mlc::String derive_display_record_fields(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs) noexcept;

mlc::String derive_display_record(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs) noexcept;

mlc::String derive_display_variant_case(std::shared_ptr<ast::TypeVariant> variant) noexcept;

mlc::String derive_display_variant_record(mlc::String name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept;

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

mlc::String gen_type_decl_fwd_only(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::String gen_type_decl_body_only(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::String requires_clause_bound_part(mlc::Array<mlc::String> type_parameters, int type_parameter_index, mlc::String bound_trait_name) noexcept;

mlc::String requires_clause(mlc::Array<mlc::String> type_parameters, mlc::Array<mlc::Array<mlc::String>> type_bounds) noexcept;

mlc::String cpp_generic_base_name(context::CodegenContext context, mlc::String type_name) noexcept{return [&]() -> mlc::String { if (type_name == mlc::String("Map")) { return mlc::String("mlc::HashMap"); } if (type_name == mlc::String("Shared")) { return mlc::String("std::shared_ptr"); } return context::CodegenContext_resolve(context, type_name); }();}

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
  [&](const TTuple& ttuple) -> mlc::String { auto [types] = ttuple; return mlc::String("std::tuple<") + types.map([context](std::shared_ptr<registry::Type> type_value) mutable { return sem_type_to_cpp(context, type_value); }).join(mlc::String(", ")) + mlc::String(">"); },
  [&](const TPair& tpair) -> mlc::String { auto [left, right] = tpair; return mlc::String("std::pair<") + sem_type_to_cpp(context, left) + mlc::String(", ") + sem_type_to_cpp(context, right) + mlc::String(">"); },
  [&](const TGeneric& tgeneric) -> mlc::String { auto [type_name, type_arguments] = tgeneric; return type_name == mlc::String("Option") && type_arguments.size() == 1 ? mlc::String("std::optional<") + sem_type_to_cpp(context, type_arguments[0]) + mlc::String(">") : type_arguments.size() == 0 ? cpp_generic_base_name(context, type_name) : type_arguments.size() == 1 ? expr::cpp_template_single_type_argument(cpp_generic_base_name(context, type_name), sem_type_to_cpp(context, type_arguments[0])) : expr::cpp_template_two_type_arguments(cpp_generic_base_name(context, type_name), sem_type_to_cpp(context, type_arguments[0]), sem_type_to_cpp(context, type_arguments[1])); },
  [&](const TFn& tfn) -> mlc::String { auto [param_types, return_type] = tfn; return expr::cpp_std_function_type(sem_type_to_cpp(context, return_type), param_types.map([context](std::shared_ptr<registry::Type> type_value) mutable { return sem_type_to_cpp(context, type_value); }).join(mlc::String(", "))); },
  [&](const TAssoc& tassoc) -> mlc::String { auto [param, assoc] = tassoc; return mlc::String("typename ") + cpp_naming::cpp_safe(param) + mlc::String("::") + assoc; }
}, (*semantic_type));}

mlc::String type_alias_annotation_cpp(context::CodegenContext context, mlc::String alias_name) noexcept{return context.type_alias_annotations.has(alias_name) ? type_to_cpp(context, context.type_alias_annotations.get(alias_name)) : mlc::String("");}

mlc::String type_name_to_cpp(context::CodegenContext context, mlc::String type_name) noexcept{
mlc::String alias_type_cpp = type_alias_annotation_cpp(context, type_name);
return alias_type_cpp != mlc::String("") ? alias_type_cpp : [&]() -> mlc::String { if (type_name == mlc::String("i32")) { return mlc::String("int"); } if (type_name == mlc::String("string")) { return mlc::String("mlc::String"); } if (type_name == mlc::String("bool")) { return mlc::String("bool"); } if (type_name == mlc::String("unit")) { return mlc::String("void"); } if (type_name == mlc::String("i64")) { return mlc::String("int64_t"); } if (type_name == mlc::String("f64")) { return mlc::String("double"); } if (type_name == mlc::String("u8")) { return mlc::String("uint8_t"); } if (type_name == mlc::String("usize")) { return mlc::String("size_t"); } if (type_name == mlc::String("char")) { return mlc::String("char32_t"); } if (type_name == mlc::String("Self") || type_name == mlc::String("self")) { return context.self_type.length() > 0 ? type_name_to_cpp(context, context.self_type) : mlc::String("void"); } return context::CodegenContext_resolve(context, type_name); }();
}

mlc::String type_to_cpp(context::CodegenContext context, std::shared_ptr<ast::TypeExpr> type_expr) noexcept{return std::visit(overloaded{
  [&](const TyI32& tyi32) -> mlc::String { return mlc::String("int"); },
  [&](const TyString& tystring) -> mlc::String { return mlc::String("mlc::String"); },
  [&](const TyBool& tybool) -> mlc::String { return mlc::String("bool"); },
  [&](const TyUnit& tyunit) -> mlc::String { return mlc::String("void"); },
  [&](const TyNamed& tynamed) -> mlc::String { auto [name] = tynamed; return type_name_to_cpp(context, name); },
  [&](const TyArray& tyarray) -> mlc::String { auto [inner] = tyarray; return expr::cpp_array_type_element(type_to_cpp(context, inner)); },
  [&](const TyShared& tyshared) -> mlc::String { auto [inner] = tyshared; return expr::cpp_shared_pointer_type(type_to_cpp(context, inner)); },
  [&](const TyGeneric& tygeneric) -> mlc::String { auto [name, type_arguments] = tygeneric; return name == mlc::String("ref") && type_arguments.size() == 1 ? expr::cpp_lvalue_reference_suffix(type_to_cpp(context, type_arguments[0])) : [&]() -> mlc::String { 
  mlc::String safe = name == mlc::String("Map") ? mlc::String("mlc::HashMap") : name == mlc::String("Shared") ? mlc::String("std::shared_ptr") : context::CodegenContext_resolve(context, name);
  return type_arguments.size() == 0 ? safe : type_arguments.size() == 1 ? expr::cpp_template_single_type_argument(safe, type_to_cpp(context, type_arguments[0])) : expr::cpp_template_two_type_arguments(safe, type_to_cpp(context, type_arguments[0]), type_to_cpp(context, type_arguments[1]));
 }(); },
  [&](const TyFn& tyfn) -> mlc::String { auto [parameter_types, return_type_expression] = tyfn; return expr::cpp_std_function_type(type_to_cpp(context, return_type_expression), parameter_types.map([context](std::shared_ptr<ast::TypeExpr> parameter_type) mutable { return type_to_cpp(context, parameter_type); }).join(mlc::String(", "))); },
  [&](const TyAssoc& tyassoc) -> mlc::String { auto [param, assoc] = tyassoc; return mlc::String("typename ") + cpp_naming::cpp_safe(param) + mlc::String("::") + assoc; }
}, (*type_expr));}

bool type_param_in_typeexpr(mlc::String param, std::shared_ptr<ast::TypeExpr> type_expression) noexcept{return [&]() { if (std::holds_alternative<ast::TyNamed>((*type_expression))) { auto _v_tynamed = std::get<ast::TyNamed>((*type_expression)); auto [name] = _v_tynamed; return name == param; } if (std::holds_alternative<ast::TyArray>((*type_expression))) { auto _v_tyarray = std::get<ast::TyArray>((*type_expression)); auto [inner] = _v_tyarray; return type_param_in_typeexpr(param, inner); } if (std::holds_alternative<ast::TyShared>((*type_expression))) { auto _v_tyshared = std::get<ast::TyShared>((*type_expression)); auto [inner] = _v_tyshared; return type_param_in_typeexpr(param, inner); } if (std::holds_alternative<ast::TyGeneric>((*type_expression))) { auto _v_tygeneric = std::get<ast::TyGeneric>((*type_expression)); auto [_w0, type_arguments] = _v_tygeneric; return type_param_in_typeexpr_list(param, type_arguments); } if (std::holds_alternative<ast::TyFn>((*type_expression))) { auto _v_tyfn = std::get<ast::TyFn>((*type_expression)); auto [parameter_types, return_type_expression] = _v_tyfn; return type_param_in_typeexpr_list(param, parameter_types) || type_param_in_typeexpr(param, return_type_expression); } if (std::holds_alternative<ast::TyAssoc>((*type_expression))) { auto _v_tyassoc = std::get<ast::TyAssoc>((*type_expression)); auto [associated_parameter, _w0] = _v_tyassoc; return associated_parameter == param; } return false; }();}

bool type_param_in_typeexpr_list(mlc::String param, mlc::Array<std::shared_ptr<ast::TypeExpr>> types) noexcept{return types.any([param](std::shared_ptr<ast::TypeExpr> candidate) mutable { return type_param_in_typeexpr(param, candidate); });}

mlc::Array<std::shared_ptr<ast::TypeExpr>> variant_field_typeexprs(std::shared_ptr<ast::TypeVariant> variant) noexcept{return std::visit(overloaded{
  [&](const VarTuple& vartuple) -> mlc::Array<std::shared_ptr<ast::TypeExpr>> { auto [_w0, field_types, _w1] = vartuple; return field_types; },
  [&](const VarRecord& varrecord) -> mlc::Array<std::shared_ptr<ast::TypeExpr>> { auto [_w0, field_definitions, _w1] = varrecord; return field_definitions.map([](std::shared_ptr<ast::FieldDef> field_definition) mutable { return field_definition->type_value; }); },
  [&](const VarUnit& varunit) -> mlc::Array<std::shared_ptr<ast::TypeExpr>> { auto [_w0, _w1] = varunit; return [&]() -> mlc::Array<std::shared_ptr<ast::TypeExpr>> { 
  mlc::Array<std::shared_ptr<ast::TypeExpr>> empty = {};
  return empty;
 }(); }
}, (*variant));}

mlc::Array<mlc::String> variant_used_type_parameters(mlc::Array<mlc::String> type_parameters, std::shared_ptr<ast::TypeVariant> variant) noexcept{
mlc::Array<std::shared_ptr<ast::TypeExpr>> field_types = variant_field_typeexprs(variant);
return type_parameters.filter([field_types](mlc::String type_parameter) mutable { return type_param_in_typeexpr_list(type_parameter, field_types); });
}

mlc::Array<mlc::String> union_string_lists(mlc::Array<mlc::String> first, mlc::Array<mlc::String> second) noexcept{return first.concat(second.filter([first](mlc::String element) mutable { return !first.any([element](mlc::String existing) mutable { return existing == element; }); }));}

mlc::Array<mlc::String> type_phantom_params_for_variants(mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
auto all_fields = mlc::collections::flat_map(variants, [](std::shared_ptr<ast::TypeVariant> variant) mutable { return variant_field_typeexprs(variant); });
return type_parameters.filter([all_fields](mlc::String type_parameter) mutable { return !type_param_in_typeexpr_list(type_parameter, all_fields); });
}

mlc::String field_def_member_declaration(context::CodegenContext context, std::shared_ptr<ast::FieldDef> field_definition) noexcept{
mlc::String base = expr::struct_named_field_declaration(type_to_cpp(context, field_definition->type_value), cpp_naming::cpp_safe(field_definition->name));
return field_definition->has_default_expression && record_defaults::record_field_default_expression_acceptable_for_codegen(field_definition->default_expression) ? base + mlc::String(" = ") + record_defaults::record_field_default_expression_cpp_initializer(field_definition->default_expression, context) : base;
}

mlc::String variant_record_struct_inline_member_declarations(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept{return field_definitions.map([context](std::shared_ptr<ast::FieldDef> field_definition) mutable { return field_def_member_declaration(context, field_definition); }).join(mlc::String(""));}

mlc::String variant_ctor_name(std::shared_ptr<ast::TypeVariant> variant) noexcept{return std::visit(overloaded{
  [&](const VarUnit& varunit) -> mlc::String { auto [name, _w0] = varunit; return name; },
  [&](const VarTuple& vartuple) -> mlc::String { auto [name, _w0, _w1] = vartuple; return name; },
  [&](const VarRecord& varrecord) -> mlc::String { auto [name, _w0, _w1] = varrecord; return name; }
}, (*variant));}

mlc::String tuple_variant_struct_members_fragment(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::TypeExpr>> field_types) noexcept{return field_types.fold(type_gen::TupleStructCodegenFoldState{0, mlc::String("")}, [context](type_gen::TupleStructCodegenFoldState accumulated_tuple_state, std::shared_ptr<ast::TypeExpr> field_type_expression_under_tuple) mutable { return type_gen::TupleStructCodegenFoldState{accumulated_tuple_state.field_slot_index + 1, accumulated_tuple_state.declaration_text_so_far + expr::struct_tuple_field_declaration(type_to_cpp(context, field_type_expression_under_tuple), mlc::to_string(accumulated_tuple_state.field_slot_index))}; }).declaration_text_so_far;}

mlc::String gen_variant_struct(context::CodegenContext context, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept{return std::visit(overloaded{
  [&](const VarUnit& varunit) -> mlc::String { auto [name, _w0] = varunit; return expr::struct_empty_definition(context::CodegenContext_resolve(context, name)); },
  [&](const VarTuple& vartuple) -> mlc::String { auto [name, field_types, _w0] = vartuple; return expr::struct_with_inline_members_definition(context::CodegenContext_resolve(context, name), tuple_variant_struct_members_fragment(context, field_types)); },
  [&](const VarRecord& varrecord) -> mlc::String { auto [name, field_defs, _w0] = varrecord; return expr::struct_with_inline_members_definition(context::CodegenContext_resolve(context, name), variant_record_struct_inline_member_declarations(context, field_defs)); }
}, (*variant));}

mlc::String struct_extra_using(context::CodegenContext context, mlc::String type_name) noexcept{return context.struct_using_lines.has(type_name) ? context.struct_using_lines.get(type_name).join(mlc::String("")) : mlc::String("");}

mlc::String gen_single_variant(context::CodegenContext context, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept{return std::visit(overloaded{
  [&](const VarRecord& varrecord) -> mlc::String { auto [_w0, field_defs, _w1] = varrecord; return expr::struct_with_inline_members_definition(context::CodegenContext_resolve(context, type_name), variant_record_struct_inline_member_declarations(context, field_defs) + struct_extra_using(context, type_name)); },
  [&](const VarTuple& vartuple) -> mlc::String { auto [_w0, field_types, _w1] = vartuple; return expr::struct_with_inline_members_definition(context::CodegenContext_resolve(context, type_name), tuple_variant_struct_members_fragment(context, field_types) + struct_extra_using(context, type_name)); },
  [&](const VarUnit& varunit) -> mlc::String { auto [_w0, _w1] = varunit; return expr::struct_empty_definition(context::CodegenContext_resolve(context, type_name)); }
}, (*variant));}

mlc::String variant_forward_decl_line(context::CodegenContext context, mlc::Array<mlc::String> type_parameters, mlc::Array<mlc::String> phantom, std::shared_ptr<ast::TypeVariant> variant) noexcept{
mlc::String variant_name = context::CodegenContext_resolve(context, variant_ctor_name(variant));
mlc::Array<mlc::String> used = union_string_lists(variant_used_type_parameters(type_parameters, variant), phantom);
return expr::struct_forward_declaration_line(cpp_naming::template_prefix(used), variant_name);
}

mlc::String variant_alias_part(context::CodegenContext context, mlc::Array<mlc::String> type_parameters, mlc::Array<mlc::String> phantom, std::shared_ptr<ast::TypeVariant> variant) noexcept{
mlc::String variant_name = context::CodegenContext_resolve(context, variant_ctor_name(variant));
mlc::Array<mlc::String> used = union_string_lists(variant_used_type_parameters(type_parameters, variant), phantom);
mlc::String var_type_arguments = used.size() > 0 ? mlc::String("<") + used.join(mlc::String(", ")) + mlc::String(">") : mlc::String("");
return variant_name + var_type_arguments;
}

mlc::String gen_adt_fwd(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
mlc::String full_prefix = cpp_naming::template_prefix(type_parameters);
mlc::Array<mlc::String> phantom = type_phantom_params_for_variants(type_parameters, variants);
mlc::String forward_decls = variants.map([context, type_parameters, phantom](std::shared_ptr<ast::TypeVariant> variant) mutable { return variant_forward_decl_line(context, type_parameters, phantom, variant); }).join(mlc::String(""));
mlc::String alias_parts = variants.map([context, type_parameters, phantom](std::shared_ptr<ast::TypeVariant> variant) mutable { return variant_alias_part(context, type_parameters, phantom, variant); }).join(mlc::String(", "));
return forward_decls + expr::variant_using_alias_definition_line(full_prefix, context::CodegenContext_resolve(context, type_name), alias_parts);
}

mlc::String variant_def_with_prefix(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_parameters, mlc::Array<mlc::String> phantom, std::shared_ptr<ast::TypeVariant> variant) noexcept{
mlc::Array<mlc::String> used = union_string_lists(variant_used_type_parameters(type_parameters, variant), phantom);
return cpp_naming::template_prefix(used) + gen_variant_struct(context, type_name, variant);
}

mlc::String gen_adt_defs(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
mlc::Array<mlc::String> phantom = type_phantom_params_for_variants(type_parameters, variants);
return variants.map([context, type_name, type_parameters, phantom](std::shared_ptr<ast::TypeVariant> variant) mutable { return variant_def_with_prefix(context, type_name, type_parameters, phantom, variant); }).join(mlc::String(""));
}

mlc::String gen_type_decl_fwd(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{return variants.size() == 1 ? mlc::String("") : gen_adt_fwd(context, type_name, type_parameters, variants);}

mlc::String gen_type_decl_body(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{return variants.size() == 1 ? cpp_naming::template_prefix(type_parameters) + gen_single_variant(context, type_name, variants[0]) : gen_adt_defs(context, type_name, type_parameters, variants);}

mlc::String gen_type_decl(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{return gen_type_decl_fwd(context, type_name, type_parameters, variants) + gen_type_decl_body(context, type_name, type_parameters, variants);}

mlc::String derive_field_display(mlc::String field_name, std::shared_ptr<ast::TypeExpr> field_type) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::TyString>((*field_type))) {  return mlc::String("self.") + field_name; } return mlc::String("mlc::to_string(self.") + field_name + mlc::String(")"); }();}

mlc::String derive_display_record_fields(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs) noexcept{return mlc::String("mlc::String(\"") + type_name + mlc::String(" { ") + field_defs[0]->name + mlc::String(": \") + ") + derive_field_display(field_defs[0]->name, field_defs[0]->type_value) + field_defs.drop(1).fold(mlc::String(""), [](mlc::String accumulator, std::shared_ptr<ast::FieldDef> field_definition) mutable { return accumulator + mlc::String(" + mlc::String(\", ") + field_definition->name + mlc::String(": \") + ") + derive_field_display(field_definition->name, field_definition->type_value); }) + mlc::String(" + mlc::String(\" }\")");}

mlc::String derive_display_record(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs) noexcept{
mlc::String signature = mlc::String("mlc::String ") + type_name + mlc::String("_to_string(const ") + type_name + mlc::String("& self) noexcept");
return field_defs.size() == 0 ? signature + mlc::String(" { return mlc::String(\"") + type_name + mlc::String(" {}\"); }\n") : signature + mlc::String(" {\n  return ") + derive_display_record_fields(type_name, field_defs) + mlc::String(";\n}\n");
}

mlc::String derive_display_variant_case(std::shared_ptr<ast::TypeVariant> variant) noexcept{return std::visit(overloaded{
  [&](const VarUnit& varunit) -> mlc::String { auto [name, _w0] = varunit; return mlc::String("  if (std::holds_alternative<") + name + mlc::String(">(self._)) return mlc::String(\"") + name + mlc::String("\");\n"); },
  [&](const VarTuple& vartuple) -> mlc::String { auto [name, field_types, _w0] = vartuple; return field_types.size() == 0 ? mlc::String("  if (std::holds_alternative<") + name + mlc::String(">(self._)) return mlc::String(\"") + name + mlc::String("\");\n") : mlc::String("  if (std::holds_alternative<") + name + mlc::String(">(self._)) return mlc::String(\"") + name + mlc::String("(\") + mlc::to_string(std::get<") + name + mlc::String(">(self._)._0) + mlc::String(\")\");\n"); },
  [&](const VarRecord& varrecord) -> mlc::String { auto [name, field_definitions, _w0] = varrecord; return derive_display_variant_record(name, field_definitions); }
}, (*variant));}

mlc::String derive_display_variant_record(mlc::String name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept{
return field_definitions.size() == 0 ? mlc::String("  if (std::holds_alternative<") + name + mlc::String(">(self._)) return mlc::String(\"") + name + mlc::String("\");\n") : [&]() -> mlc::String { 
  mlc::String parts = mlc::String("mlc::to_string(std::get<") + name + mlc::String(">(self._).") + field_definitions[0]->name + mlc::String(")") + field_definitions.drop(1).fold(mlc::String(""), [name](mlc::String accumulator, std::shared_ptr<ast::FieldDef> field_definition) mutable { return accumulator + mlc::String(" + mlc::String(\", \") + mlc::to_string(std::get<") + name + mlc::String(">(self._).") + field_definition->name + mlc::String(")"); });
  return mlc::String("  if (std::holds_alternative<") + name + mlc::String(">(self._)) return mlc::String(\"") + name + mlc::String("(\") + ") + parts + mlc::String(" + mlc::String(\")\");\n");
 }();
}

mlc::String derive_display_sum(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
mlc::String cases = variants.map([](std::shared_ptr<ast::TypeVariant> variant) mutable { return derive_display_variant_case(variant); }).join(mlc::String(""));
return mlc::String("mlc::String ") + type_name + mlc::String("_to_string(const ") + type_name + mlc::String("& self) noexcept {\n") + cases + mlc::String("  return mlc::String(") + mlc::String("\"") + mlc::String("\"") + mlc::String(");\n}\n");
}

mlc::String derive_eq_record(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs) noexcept{
mlc::String signature = mlc::String("bool operator==(const ") + type_name + mlc::String("& a, const ") + type_name + mlc::String("& b) noexcept");
return field_defs.size() == 0 ? signature + mlc::String(" { return true; }\n") : [&]() -> mlc::String { 
  mlc::String condition = mlc::String("a.") + field_defs[0]->name + mlc::String(" == b.") + field_defs[0]->name + field_defs.drop(1).fold(mlc::String(""), [](mlc::String accumulator, std::shared_ptr<ast::FieldDef> field_definition) mutable { return accumulator + mlc::String(" && a.") + field_definition->name + mlc::String(" == b.") + field_definition->name; });
  return signature + mlc::String(" { return ") + condition + mlc::String("; }\n");
 }();
}

mlc::String derive_eq_sum(mlc::String type_name) noexcept{return mlc::String("bool operator==(const ") + type_name + mlc::String("& a, const ") + type_name + mlc::String("& b) noexcept { return a._ == b._; }\n");}

mlc::String derive_ord_build_cond(mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs) noexcept{
mlc::String condition = mlc::String("a.") + field_defs[0]->name + mlc::String(" < b.") + field_defs[0]->name;
int field_index = 1;
while (field_index < field_defs.size()){
{
mlc::String previous_equality = mlc::String("a.") + field_defs[0]->name + mlc::String(" == b.") + field_defs[0]->name;
int previous_field_index = 1;
while (previous_field_index < field_index){
{
previous_equality = previous_equality + mlc::String(" && a.") + field_defs[previous_field_index]->name + mlc::String(" == b.") + field_defs[previous_field_index]->name;
previous_field_index = previous_field_index + 1;
}
}
condition = condition + mlc::String(" || (") + previous_equality + mlc::String(" && a.") + field_defs[field_index]->name + mlc::String(" < b.") + field_defs[field_index]->name + mlc::String(")");
field_index = field_index + 1;
}
}
return condition;
}

mlc::String derive_ord_record(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs) noexcept{
mlc::String signature = mlc::String("bool operator<(const ") + type_name + mlc::String("& a, const ") + type_name + mlc::String("& b) noexcept");
return field_defs.size() == 0 ? signature + mlc::String(" { return false; }\n") : signature + mlc::String(" { return ") + derive_ord_build_cond(field_defs) + mlc::String("; }\n");
}

mlc::String derive_ord_sum(mlc::String type_name) noexcept{return mlc::String("bool operator<(const ") + type_name + mlc::String("& a, const ") + type_name + mlc::String("& b) noexcept { return a._ < b._; }\n");}

mlc::String derive_hash_std_cpp_type(std::shared_ptr<ast::TypeExpr> field_type) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::TyString>((*field_type))) {  return mlc::String("mlc::String"); } if (std::holds_alternative<ast::TyI32>((*field_type))) {  return mlc::String("int"); } if (std::holds_alternative<ast::TyBool>((*field_type))) {  return mlc::String("bool"); } return mlc::String(""); }();}

mlc::String derive_hash_combine_line(mlc::String cpp_type, mlc::String access_expr) noexcept{return mlc::String("h ^= std::hash<") + cpp_type + mlc::String(">{}(") + access_expr + mlc::String(") + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);\n");}

mlc::String derive_hash_record_operator_body(mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs) noexcept{return field_defs.size() == 0 ? mlc::String("return static_cast<size_t>(1469598103934665603ULL);\n") : field_defs.fold(mlc::String("size_t h = 1469598103934665603ULL;\n"), [](mlc::String accumulator, std::shared_ptr<ast::FieldDef> field_definition) mutable { return accumulator + derive_hash_combine_line(derive_hash_std_cpp_type(field_definition->type_value), mlc::String("self.") + field_definition->name); }) + mlc::String("return h;\n");}

mlc::String derive_hash_record(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs) noexcept{
mlc::String operator_body = derive_hash_record_operator_body(field_defs);
return mlc::String("namespace std {\ntemplate<>\nstruct hash<") + type_name + mlc::String("> {\n  size_t operator()(const ") + type_name + mlc::String("& self) const noexcept {\n    ") + operator_body + mlc::String("  }\n};\n}\n");
}

mlc::String derive_hash_tuple_variant_inner(mlc::String variant_struct_name, mlc::Array<std::shared_ptr<ast::TypeExpr>> field_types, int discriminant_index) noexcept{return field_types.fold(type_gen::TupleHashCodegenFoldState{0, derive_hash_combine_line(mlc::String("size_t"), mlc::to_string(discriminant_index))}, [variant_struct_name](type_gen::TupleHashCodegenFoldState accumulated_hash_tuple_state, std::shared_ptr<ast::TypeExpr> field_type_expression_under_tuple) mutable { return type_gen::TupleHashCodegenFoldState{accumulated_hash_tuple_state.tuple_field_slot + 1, accumulated_hash_tuple_state.concatenated_combine_lines + derive_hash_combine_line(derive_hash_std_cpp_type(field_type_expression_under_tuple), mlc::String("std::get<") + variant_struct_name + mlc::String(">(self._).field") + mlc::to_string(accumulated_hash_tuple_state.tuple_field_slot))}; }).concatenated_combine_lines;}

mlc::String derive_hash_record_variant_inner(mlc::String variant_struct_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs, int discriminant_index) noexcept{return field_defs.fold(derive_hash_combine_line(mlc::String("size_t"), mlc::to_string(discriminant_index)), [variant_struct_name](mlc::String accumulator, std::shared_ptr<ast::FieldDef> field_definition) mutable { return accumulator + derive_hash_combine_line(derive_hash_std_cpp_type(field_definition->type_value), mlc::String("std::get<") + variant_struct_name + mlc::String(">(self._).") + field_definition->name); });}

mlc::String derive_hash_sum_variant_inner(std::shared_ptr<ast::TypeVariant> variant, int discriminant_index) noexcept{return std::visit(overloaded{
  [&](const VarUnit& varunit) -> mlc::String { auto [_w0, _w1] = varunit; return derive_hash_combine_line(mlc::String("size_t"), mlc::to_string(discriminant_index)); },
  [&](const VarTuple& vartuple) -> mlc::String { auto [variant_struct_name, field_types, _w0] = vartuple; return derive_hash_tuple_variant_inner(variant_struct_name, field_types, discriminant_index); },
  [&](const VarRecord& varrecord) -> mlc::String { auto [variant_struct_name, field_definitions, _w0] = varrecord; return derive_hash_record_variant_inner(variant_struct_name, field_definitions, discriminant_index); }
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

mlc::String derive_hash_sum_operator_body(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{return variants.fold(type_gen::HashSumOperatorFoldState{0, mlc::String("size_t h = 1469598103934665603ULL;\n")}, [](type_gen::HashSumOperatorFoldState accumulated_hash_sum_state, std::shared_ptr<ast::TypeVariant> variant_under_hash_sum) mutable { return type_gen::HashSumOperatorFoldState{accumulated_hash_sum_state.next_discriminant_index + 1, accumulated_hash_sum_state.operator_body_text + derive_hash_sum_branch(variant_under_hash_sum, accumulated_hash_sum_state.next_discriminant_index)}; }).operator_body_text + mlc::String("return h;\n");}

mlc::String derive_hash_sum(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
mlc::String operator_body = derive_hash_sum_operator_body(variants);
return mlc::String("namespace std {\ntemplate<>\nstruct hash<") + type_name + mlc::String("> {\n  size_t operator()(const ") + type_name + mlc::String("& self) const noexcept {\n    ") + operator_body + mlc::String("  }\n};\n}\n");
}

bool variants_is_single_record(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{return variants.size() != 1 ? false : [&]() { if (std::holds_alternative<ast::VarRecord>((*variants[0]))) { auto _v_varrecord = std::get<ast::VarRecord>((*variants[0])); auto [_w0, _w1, _w2] = _v_varrecord; return true; } return false; }();}

mlc::Array<std::shared_ptr<ast::FieldDef>> derive_record_field_defs(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
mlc::Array<std::shared_ptr<ast::FieldDef>> empty = {};
return [&]() -> mlc::Array<std::shared_ptr<ast::FieldDef>> { if (std::holds_alternative<ast::VarRecord>((*variants[0]))) { auto _v_varrecord = std::get<ast::VarRecord>((*variants[0])); auto [_w0, field_definitions, _w1] = _v_varrecord; return field_definitions; } return empty; }();
}

mlc::String gen_derive_record_trait(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::String trait_name) noexcept{
mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions = derive_record_field_defs(variants);
return [&]() -> mlc::String { if (trait_name == mlc::String("Display")) { return derive_display_record(type_name, field_definitions); } if (trait_name == mlc::String("Eq")) { return derive_eq_record(type_name, field_definitions); } if (trait_name == mlc::String("Ord")) { return derive_ord_record(type_name, field_definitions); } if (trait_name == mlc::String("Hash")) { return derive_hash_record(type_name, field_definitions); } return mlc::String(""); }();
}

mlc::String gen_derive_sum_trait(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::String trait_name) noexcept{return [&]() -> mlc::String { if (trait_name == mlc::String("Display")) { return derive_display_sum(type_name, variants); } if (trait_name == mlc::String("Eq")) { return derive_eq_sum(type_name); } if (trait_name == mlc::String("Ord")) { return derive_ord_sum(type_name); } if (trait_name == mlc::String("Hash")) { return derive_hash_sum(type_name, variants); } return mlc::String(""); }();}

mlc::String gen_derive_methods(context::CodegenContext context, mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::Array<mlc::String> derive_traits) noexcept{
return derive_traits.size() == 0 ? mlc::String("") : [&]() -> mlc::String { 
  bool is_record = variants_is_single_record(variants);
  return derive_traits.map([is_record, type_name, variants](mlc::String trait_name) mutable { return is_record ? gen_derive_record_trait(type_name, variants, trait_name) : gen_derive_sum_trait(type_name, variants, trait_name); }).join(mlc::String(""));
 }();
}

mlc::String gen_type_decl_fwd_only(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{return gen_type_decl_fwd(context, type_name, type_parameters, variants);}

mlc::String gen_type_decl_body_only(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{return gen_type_decl_body(context, type_name, type_parameters, variants);}

mlc::String requires_clause_bound_part(mlc::Array<mlc::String> type_parameters, int type_parameter_index, mlc::String bound_trait_name) noexcept{return bound_trait_name == mlc::String("ExprVisitor") && type_parameters.size() >= 2 ? expr::concept_trait_constraint_with_result_and_implementor(cpp_naming::cpp_safe(bound_trait_name), type_parameters[0], type_parameters[type_parameter_index]) : expr::concept_trait_constraint_on_type_parameter(cpp_naming::cpp_safe(bound_trait_name), type_parameters[type_parameter_index]);}

mlc::String requires_clause(mlc::Array<mlc::String> type_parameters, mlc::Array<mlc::Array<mlc::String>> type_bounds) noexcept{
mlc::Array<mlc::String> parts = {};
int type_parameter_index = 0;
while (type_parameter_index < type_parameters.size()){
{
if (type_parameter_index < type_bounds.size()){
{
parts = parts.concat(type_bounds[type_parameter_index].map([type_parameters, type_parameter_index](mlc::String bound_trait_name) mutable { return requires_clause_bound_part(type_parameters, type_parameter_index, bound_trait_name); }));
}
}
type_parameter_index = type_parameter_index + 1;
}
}
return parts.size() > 0 ? expr::concept_requires_clause_line(parts.join(mlc::String(" && "))) : mlc::String("");
}

} // namespace type_gen
