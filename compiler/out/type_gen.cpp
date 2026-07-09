#define main mlc_user_main
#include "type_gen.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "record_field_default_validate.hpp"
#include "record_field_default_emit.hpp"
#include "context.hpp"
#include "decl_index.hpp"
#include "cpp_naming.hpp"
#include "expr.hpp"

namespace type_gen {

using namespace ast;
using namespace registry;
using namespace record_field_default_validate;
using namespace record_field_default_emit;
using namespace context;
using namespace decl_index;
using namespace cpp_naming;
using namespace expr;
using namespace ast_tokens;

mlc::String cpp_generic_base_name(context::CodegenContext context, mlc::String type_name) noexcept{
  if ((type_name == mlc::String("Map", 3)))   {
    return mlc::String("mlc::HashMap", 12);
  } else if ((type_name == mlc::String("Shared", 6)))   {
    return mlc::String("std::shared_ptr", 15);
  } else if ((type_name == mlc::String("Weak", 4)))   {
    return mlc::String("std::weak_ptr", 13);
  } else if ((type_name == mlc::String("Task", 4)))   {
    return mlc::String("mlc::Task", 9);
  } else if ((type_name == mlc::String("Future", 6)))   {
    return mlc::String("mlc::Task", 9);
  } else if ((type_name == mlc::String("Channel", 7)))   {
    return mlc::String("mlc::concurrency::Channel", 25);
  } else if ((type_name == mlc::String("Arc", 3)))   {
    return mlc::String("mlc::concurrency::Arc", 21);
  } else if ((type_name == mlc::String("Mutex", 5)))   {
    return mlc::String("mlc::concurrency::Mutex", 23);
  } else   {
    return context::CodegenContext_resolve(context, type_name);
  }
}
mlc::String sem_type_to_cpp(context::CodegenContext context, std::shared_ptr<registry::Type> semantic_type) noexcept{
  return std::visit(overloaded{[&](const registry::TI32& tI32) { return mlc::String("int", 3); },
[&](const registry::TString& tString) { return mlc::String("mlc::String", 11); },
[&](const registry::TBool& tBool) { return mlc::String("bool", 4); },
[&](const registry::TUnit& tUnit) { return mlc::String("void", 4); },
[&](const registry::TI64& tI64) { return mlc::String("int64_t", 7); },
[&](const registry::TF64& tF64) { return mlc::String("double", 6); },
[&](const registry::TU8& tU8) { return mlc::String("uint8_t", 7); },
[&](const registry::TUsize& tUsize) { return mlc::String("size_t", 6); },
[&](const registry::TChar& tChar) { return mlc::String("char32_t", 8); },
[&](const registry::TUnknown& tUnknown) { return mlc::String("auto", 4); },
[&](const registry::TArray& tArray) { auto [inner] = tArray; return expr::cpp_array_type_element(sem_type_to_cpp(context, inner)); },
[&](const registry::TShared& tShared) { auto [inner] = tShared; return expr::cpp_shared_pointer_type(sem_type_to_cpp(context, inner)); },
[&](const registry::TNamed& tNamed) { auto [type_name] = tNamed; return context::CodegenContext_resolve(context, type_name); },
[&](const registry::TTuple& tTuple) { auto [types] = tTuple; return ((mlc::String("std::tuple<", 11) + mlc::to_string(types.map([=](std::shared_ptr<registry::Type> type_value) mutable { return sem_type_to_cpp(context, type_value); }).join(mlc::String(", ", 2)))) + mlc::String(">", 1)); },
[&](const registry::TPair& tPair) { auto [left, right] = tPair; return ((((mlc::String("std::pair<", 10) + mlc::to_string(sem_type_to_cpp(context, left))) + mlc::String(", ", 2)) + mlc::to_string(sem_type_to_cpp(context, right))) + mlc::String(">", 1)); },
[&](const registry::TGeneric& tGeneric) { auto [type_name, type_arguments] = tGeneric; return (((type_name == mlc::String("Option", 6)) && (type_arguments.length() == 1)) ? (((mlc::String("std::optional<", 14) + mlc::to_string(sem_type_to_cpp(context, type_arguments[0]))) + mlc::String(">", 1))) : ([&]() -> mlc::String {
  if ((type_arguments.length() == 0))   {
    return cpp_generic_base_name(context, type_name);
  } else   {
    return ((type_arguments.length() == 1) ? (expr::cpp_template_single_type_argument(cpp_generic_base_name(context, type_name), sem_type_to_cpp(context, type_arguments[0]))) : (expr::cpp_template_two_type_arguments(cpp_generic_base_name(context, type_name), sem_type_to_cpp(context, type_arguments[0]), sem_type_to_cpp(context, type_arguments[1]))));
  }
}())); },
[&](const registry::TFn& tFn) { auto [param_types, return_type] = tFn; return expr::cpp_std_function_type(sem_type_to_cpp(context, return_type), param_types.map([=](std::shared_ptr<registry::Type> type_value) mutable { return sem_type_to_cpp(context, type_value); }).join(mlc::String(", ", 2))); },
[&](const registry::TAssoc& tAssoc) { auto [param, assoc] = tAssoc; return ((((mlc::String("typename ", 9) + mlc::to_string(cpp_naming::cpp_safe(param))) + mlc::String("::", 2)) + mlc::to_string(assoc)) + mlc::String("", 0)); }
}, (*semantic_type));
}
mlc::String type_alias_annotation_cpp(context::CodegenContext context, mlc::String alias_name) noexcept{
  if (context.type_alias_annotations.has(alias_name))   {
    return type_to_cpp(context, context.type_alias_annotations.get(alias_name));
  } else   {
    return mlc::String("", 0);
  }
}
mlc::String type_name_to_cpp_literal(mlc::String type_name) noexcept{
  if ((type_name == mlc::String("i32", 3)))   {
    return mlc::String("int", 3);
  } else if ((type_name == mlc::String("string", 6)))   {
    return mlc::String("mlc::String", 11);
  } else if ((type_name == mlc::String("bool", 4)))   {
    return mlc::String("bool", 4);
  } else if ((type_name == mlc::String("unit", 4)))   {
    return mlc::String("void", 4);
  } else if ((type_name == mlc::String("i64", 3)))   {
    return mlc::String("int64_t", 7);
  } else if ((type_name == mlc::String("f64", 3)))   {
    return mlc::String("double", 6);
  } else if ((type_name == mlc::String("u8", 2)))   {
    return mlc::String("uint8_t", 7);
  } else if ((type_name == mlc::String("usize", 5)))   {
    return mlc::String("size_t", 6);
  } else if ((type_name == mlc::String("char", 4)))   {
    return mlc::String("char32_t", 8);
  } else   {
    return mlc::String("", 0);
  }
}
mlc::String type_name_to_cpp_self(context::CodegenContext context) noexcept{
  if ((context.self_type.length() > 0))   {
    return type_name_to_cpp(context, context.self_type);
  } else   {
    return mlc::String("void", 4);
  }
}
mlc::String associated_type_name_cpp(context::CodegenContext context, mlc::String associated_type_name) noexcept{
  if (context::trait_has_associated_types(context, context.self_type))   {
    return ((mlc::String("typename Self::", 15) + mlc::to_string(cpp_naming::cpp_safe(associated_type_name))) + mlc::String("", 0));
  } else   {
    return associated_type_name;
  }
}
mlc::String resolved_type_name_cpp(context::CodegenContext context, mlc::String type_name) noexcept{
  return context::CodegenContext_resolve(context, type_name);
}
mlc::String type_name_to_cpp(context::CodegenContext context, mlc::String type_name) noexcept{
  if ((type_alias_annotation_cpp(context, type_name) != mlc::String("", 0)))   {
    return type_alias_annotation_cpp(context, type_name);
  } else if (((type_name == mlc::String("Self", 4)) || (type_name == mlc::String("self", 4))))   {
    return type_name_to_cpp_self(context);
  } else if ((context::trait_has_associated_types(context, context.self_type) && context.trait_associated_type_names.get(context.self_type).any([=](auto entry) mutable { return (entry == type_name); })))   {
    return associated_type_name_cpp(context, type_name);
  } else if ((type_name_to_cpp_literal(type_name) != mlc::String("", 0)))   {
    return type_name_to_cpp_literal(type_name);
  } else   {
    return resolved_type_name_cpp(context, type_name);
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
[&](const ast::TyGeneric& tyGeneric) { auto [name, type_arguments] = tyGeneric; return [&]() -> auto {
  if (((name == mlc::String("ref", 3)) && (type_arguments.length() == 1)))   {
    return expr::cpp_lvalue_reference_suffix(type_to_cpp(context, type_arguments[0]));
  } else   {
    return [&]() {
auto safe = cpp_generic_base_name(context, name);
return [&]() -> mlc::String {
  if ((type_arguments.length() == 0))   {
    return safe;
  } else   {
    return ((type_arguments.length() == 1) ? (expr::cpp_template_single_type_argument(safe, type_to_cpp(context, type_arguments[0]))) : (expr::cpp_template_two_type_arguments(safe, type_to_cpp(context, type_arguments[0]), type_to_cpp(context, type_arguments[1]))));
  }
}();
}();
  }
}(); },
[&](const ast::TyFn& tyFn) { auto [parameter_types, return_type_expression] = tyFn; return expr::cpp_std_function_type(type_to_cpp(context, return_type_expression), parameter_types.map([=](std::shared_ptr<ast::TypeExpr> parameter_type) mutable { return type_to_cpp(context, parameter_type); }).join(mlc::String(", ", 2))); },
[&](const ast::TyAssoc& tyAssoc) { auto [param, assoc] = tyAssoc; return ((((mlc::String("typename ", 9) + mlc::to_string(cpp_naming::cpp_safe(param))) + mlc::String("::", 2)) + mlc::to_string(assoc)) + mlc::String("", 0)); }
}, (*type_expr));
}
bool type_param_in_typeexpr(mlc::String param, std::shared_ptr<ast::TypeExpr> type_expression) noexcept{
  return [&]() -> bool {
auto __match_subject = type_expression;
if (std::holds_alternative<ast::TyNamed>((*__match_subject))) {
const ast::TyNamed& tyNamed = std::get<ast::TyNamed>((*__match_subject));
auto [name] = tyNamed; return (name == param);
}
if (std::holds_alternative<ast::TyArray>((*__match_subject))) {
const ast::TyArray& tyArray = std::get<ast::TyArray>((*__match_subject));
auto [inner] = tyArray; return type_param_in_typeexpr(param, inner);
}
if (std::holds_alternative<ast::TyShared>((*__match_subject))) {
const ast::TyShared& tyShared = std::get<ast::TyShared>((*__match_subject));
auto [inner] = tyShared; return type_param_in_typeexpr(param, inner);
}
if (std::holds_alternative<ast::TyGeneric>((*__match_subject))) {
const ast::TyGeneric& tyGeneric = std::get<ast::TyGeneric>((*__match_subject));
auto [__0, type_arguments] = tyGeneric; return type_param_in_typeexpr_list(param, type_arguments);
}
if (std::holds_alternative<ast::TyFn>((*__match_subject))) {
const ast::TyFn& tyFn = std::get<ast::TyFn>((*__match_subject));
auto [parameter_types, return_type_expression] = tyFn; return (type_param_in_typeexpr_list(param, parameter_types) || type_param_in_typeexpr(param, return_type_expression));
}
if (std::holds_alternative<ast::TyAssoc>((*__match_subject))) {
const ast::TyAssoc& tyAssoc = std::get<ast::TyAssoc>((*__match_subject));
auto [associated_parameter, __1] = tyAssoc; return (associated_parameter == param);
}
return false;
std::abort();
}();
}
bool type_param_in_typeexpr_list(mlc::String param, mlc::Array<std::shared_ptr<ast::TypeExpr>> types) noexcept{
  return types.any([=](std::shared_ptr<ast::TypeExpr> candidate) mutable { return type_param_in_typeexpr(param, candidate); });
}
mlc::Array<std::shared_ptr<ast::TypeExpr>> variant_field_typeexprs(std::shared_ptr<ast::TypeVariant> variant) noexcept{
  return std::visit(overloaded{[&](const ast::VarTuple& varTuple) { auto [__0, field_types, __2] = varTuple; return field_types; },
[&](const ast::VarRecord& varRecord) { auto [__0, field_definitions, __2] = varRecord; return field_definitions.map([=](std::shared_ptr<ast::FieldDef> field_definition) mutable { return field_definition->type_value; }); },
[&](const ast::VarUnit& varUnit) { auto [__0, __1] = varUnit; return [&]() {
auto empty = mlc::Array<std::shared_ptr<ast::TypeExpr>>{};
return empty;
}(); }
}, (*variant));
}
mlc::Array<mlc::String> variant_used_type_parameters(mlc::Array<mlc::String> type_parameters, std::shared_ptr<ast::TypeVariant> variant) noexcept{
  auto field_types = variant_field_typeexprs(variant);
  return type_parameters.filter([=](mlc::String type_parameter) mutable { return type_param_in_typeexpr_list(type_parameter, field_types); });
}
mlc::Array<mlc::String> union_string_lists(mlc::Array<mlc::String> first, mlc::Array<mlc::String> second) noexcept{
  return first.concat(second.filter([=](mlc::String element) mutable { return (!first.any([=](mlc::String existing) mutable { return (existing == element); })); }));
}
mlc::Array<mlc::String> type_phantom_params_for_variants(mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
  auto all_fields = variants.flat_map([=](std::shared_ptr<ast::TypeVariant> variant) mutable { return variant_field_typeexprs(variant); });
  return type_parameters.filter([=](mlc::String type_parameter) mutable { return (!type_param_in_typeexpr_list(type_parameter, all_fields)); });
}
mlc::String field_def_member_declaration(context::CodegenContext context, std::shared_ptr<ast::FieldDef> field_definition) noexcept{
  auto base = expr::struct_named_field_declaration(type_to_cpp(context, field_definition->type_value), cpp_naming::cpp_safe(field_definition->name));
  if ((field_definition->has_default_expression && record_field_default_validate::record_field_default_expression_is_static_initializer(field_definition->default_expression)))   {
    return ((base + mlc::String(" = ", 3)) + record_field_default_emit::record_field_default_expression_cpp_initializer(field_definition->default_expression, context));
  } else   {
    return base;
  }
}
mlc::String variant_record_struct_inline_member_declarations(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept{
  return field_definitions.map([=](std::shared_ptr<ast::FieldDef> field_definition) mutable { return field_def_member_declaration(context, field_definition); }).join(mlc::String("", 0));
}
mlc::String variant_ctor_name(std::shared_ptr<ast::TypeVariant> variant) noexcept{
  return std::visit(overloaded{[&](const ast::VarUnit& varUnit) { auto [name, __1] = varUnit; return name; },
[&](const ast::VarTuple& varTuple) { auto [name, __1, __2] = varTuple; return name; },
[&](const ast::VarRecord& varRecord) { auto [name, __1, __2] = varRecord; return name; }
}, (*variant));
}
mlc::String tuple_variant_struct_members_fragment(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::TypeExpr>> field_types) noexcept{
  return field_types.fold(TupleStructCodegenFoldState{0, mlc::String("", 0)}, [=](TupleStructCodegenFoldState accumulated_tuple_state, std::shared_ptr<ast::TypeExpr> field_type_expression_under_tuple) mutable { return TupleStructCodegenFoldState{(accumulated_tuple_state.field_slot_index + 1), (accumulated_tuple_state.declaration_text_so_far + expr::struct_tuple_field_declaration(type_to_cpp(context, field_type_expression_under_tuple), mlc::to_string(accumulated_tuple_state.field_slot_index)))}; }).declaration_text_so_far;
}
mlc::String gen_variant_struct(context::CodegenContext context, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept{
  return std::visit(overloaded{[&](const ast::VarUnit& varUnit) { auto [name, __1] = varUnit; return expr::struct_empty_definition(context::CodegenContext_resolve(context, name)); },
[&](const ast::VarTuple& varTuple) { auto [name, field_types, __2] = varTuple; return expr::struct_with_inline_members_definition(context::CodegenContext_resolve(context, name), tuple_variant_struct_members_fragment(context, field_types)); },
[&](const ast::VarRecord& varRecord) { auto [name, field_defs, __2] = varRecord; return expr::struct_with_inline_members_definition(context::CodegenContext_resolve(context, name), variant_record_struct_inline_member_declarations(context, field_defs)); }
}, (*variant));
}
mlc::String struct_extra_using(context::CodegenContext context, mlc::String type_name) noexcept{
  if (context.struct_using_lines.has(type_name))   {
    return context.struct_using_lines.get(type_name).join(mlc::String("", 0));
  } else   {
    return mlc::String("", 0);
  }
}
mlc::String gen_single_variant(context::CodegenContext context, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept{
  return std::visit(overloaded{[&](const ast::VarRecord& varRecord) { auto [__0, field_defs, __2] = varRecord; return expr::struct_with_inline_members_definition(context::CodegenContext_resolve(context, type_name), (variant_record_struct_inline_member_declarations(context, field_defs) + struct_extra_using(context, type_name))); },
[&](const ast::VarTuple& varTuple) { auto [__0, field_types, __2] = varTuple; return expr::struct_with_inline_members_definition(context::CodegenContext_resolve(context, type_name), (tuple_variant_struct_members_fragment(context, field_types) + struct_extra_using(context, type_name))); },
[&](const ast::VarUnit& varUnit) { auto [__0, __1] = varUnit; return expr::struct_empty_definition(context::CodegenContext_resolve(context, type_name)); }
}, (*variant));
}
mlc::String variant_forward_decl_line(context::CodegenContext context, mlc::Array<mlc::String> type_parameters, mlc::Array<mlc::String> phantom, std::shared_ptr<ast::TypeVariant> variant) noexcept{
  auto variant_name = context::CodegenContext_resolve(context, variant_ctor_name(variant));
  auto used = union_string_lists(variant_used_type_parameters(type_parameters, variant), phantom);
  return expr::struct_forward_declaration_line(cpp_naming::template_prefix(used), variant_name);
}
mlc::String variant_type_argument_suffix(mlc::Array<mlc::String> type_parameters, mlc::Array<mlc::String> phantom, std::shared_ptr<ast::TypeVariant> variant) noexcept{
  auto used = union_string_lists(variant_used_type_parameters(type_parameters, variant), phantom);
  if ((used.length() > 0))   {
    return ((mlc::String("<", 1) + mlc::to_string(used.join(mlc::String(", ", 2)))) + mlc::String(">", 1));
  } else   {
    return mlc::String("", 0);
  }
}
mlc::String variant_alias_part(context::CodegenContext context, mlc::Array<mlc::String> type_parameters, mlc::Array<mlc::String> phantom, std::shared_ptr<ast::TypeVariant> variant) noexcept{
  return (context::CodegenContext_resolve(context, variant_ctor_name(variant)) + variant_type_argument_suffix(type_parameters, phantom, variant));
}
mlc::String gen_adt_fwd(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
  auto full_prefix = cpp_naming::template_prefix(type_parameters);
  auto phantom = type_phantom_params_for_variants(type_parameters, variants);
  auto forward_decls = variants.map([=](std::shared_ptr<ast::TypeVariant> variant) mutable { return variant_forward_decl_line(context, type_parameters, phantom, variant); }).join(mlc::String("", 0));
  auto alias_parts = variants.map([=](std::shared_ptr<ast::TypeVariant> variant) mutable { return variant_alias_part(context, type_parameters, phantom, variant); }).join(mlc::String(", ", 2));
  return (forward_decls + expr::variant_using_alias_definition_line(full_prefix, context::CodegenContext_resolve(context, type_name), alias_parts));
}
mlc::String variant_def_with_prefix(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_parameters, mlc::Array<mlc::String> phantom, std::shared_ptr<ast::TypeVariant> variant) noexcept{
  auto used = union_string_lists(variant_used_type_parameters(type_parameters, variant), phantom);
  return (cpp_naming::template_prefix(used) + gen_variant_struct(context, type_name, variant));
}
mlc::String gen_adt_defs(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
  auto phantom = type_phantom_params_for_variants(type_parameters, variants);
  return variants.map([=](std::shared_ptr<ast::TypeVariant> variant) mutable { return variant_def_with_prefix(context, type_name, type_parameters, phantom, variant); }).join(mlc::String("", 0));
}
mlc::String gen_type_decl_fwd(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
  if ((variants.length() == 1))   {
    return mlc::String("", 0);
  } else   {
    return gen_adt_fwd(context, type_name, type_parameters, variants);
  }
}
mlc::String gen_type_decl_body(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
  if ((variants.length() == 1))   {
    return (cpp_naming::template_prefix(type_parameters) + gen_single_variant(context, type_name, variants[0]));
  } else   {
    return gen_adt_defs(context, type_name, type_parameters, variants);
  }
}
mlc::String gen_type_decl(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
  return (gen_type_decl_fwd(context, type_name, type_parameters, variants) + gen_type_decl_body(context, type_name, type_parameters, variants));
}
mlc::String derive_field_display(mlc::String field_name, std::shared_ptr<ast::TypeExpr> field_type) noexcept{
  return std::visit(overloaded{[&](const ast::TyString& tyString) { return ((mlc::String("self.", 5) + mlc::to_string(field_name)) + mlc::String("", 0)); },
[&](const ast::TyI32& tyI32) { return ((mlc::String("mlc::to_string(self.", 20) + mlc::to_string(field_name)) + mlc::String(")", 1)); },
[&](const ast::TyBool& tyBool) { return ((mlc::String("mlc::to_string(self.", 20) + mlc::to_string(field_name)) + mlc::String(")", 1)); },
[&](const ast::TyUnit& tyUnit) { return ((mlc::String("mlc::to_string(self.", 20) + mlc::to_string(field_name)) + mlc::String(")", 1)); },
[&](const ast::TyNamed& tyNamed) { auto [name] = tyNamed; return ((mlc::String("mlc::to_string(self.", 20) + mlc::to_string(field_name)) + mlc::String(")", 1)); },
[&](const ast::TyArray& tyArray) { auto [inner] = tyArray; return ((mlc::String("mlc::to_string(self.", 20) + mlc::to_string(field_name)) + mlc::String(")", 1)); },
[&](const ast::TyShared& tyShared) { auto [inner] = tyShared; return ((mlc::String("mlc::to_string(self.", 20) + mlc::to_string(field_name)) + mlc::String(")", 1)); },
[&](const ast::TyGeneric& tyGeneric) { auto [name, type_arguments] = tyGeneric; return ((mlc::String("mlc::to_string(self.", 20) + mlc::to_string(field_name)) + mlc::String(")", 1)); },
[&](const ast::TyFn& tyFn) { auto [parameter_types, return_type_expression] = tyFn; return ((mlc::String("mlc::to_string(self.", 20) + mlc::to_string(field_name)) + mlc::String(")", 1)); },
[&](const ast::TyAssoc& tyAssoc) { auto [param, assoc] = tyAssoc; return ((mlc::String("mlc::to_string(self.", 20) + mlc::to_string(field_name)) + mlc::String(")", 1)); }
}, (*field_type));
}
mlc::String derive_display_record_fields(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs) noexcept{
  return (((((((mlc::String("mlc::String(\"", 13) + type_name) + mlc::String(" { ", 3)) + field_defs[0]->name) + mlc::String(": \") + ", 7)) + derive_field_display(field_defs[0]->name, field_defs[0]->type_value)) + field_defs.drop(1).fold(mlc::String("", 0), [=](mlc::String accumulator, std::shared_ptr<ast::FieldDef> field_definition) mutable { return ((((accumulator + mlc::String(" + mlc::String(\", ", 18)) + field_definition->name) + mlc::String(": \") + ", 7)) + derive_field_display(field_definition->name, field_definition->type_value)); })) + mlc::String(" + mlc::String(\" }\")", 20));
}
mlc::String derive_display_record(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs) noexcept{
  auto signature = ((((mlc::String("mlc::String ", 12) + mlc::to_string(type_name)) + mlc::String("_to_string(const ", 17)) + mlc::to_string(type_name)) + mlc::String("& self) noexcept", 16));
  if ((field_defs.length() == 0))   {
    return (((signature + mlc::String(" { return mlc::String(\"", 23)) + type_name) + mlc::String(" {}\"); }\n", 9));
  } else   {
    return (((signature + mlc::String(" {\n  return ", 12)) + derive_display_record_fields(type_name, field_defs)) + mlc::String(";\n}\n", 4));
  }
}
mlc::String derive_display_variant_case(std::shared_ptr<ast::TypeVariant> variant) noexcept{
  return std::visit(overloaded{[&](const ast::VarUnit& varUnit) { auto [name, __1] = varUnit; return ((((mlc::String("  if (std::holds_alternative<", 29) + name) + mlc::String(">(self._)) return mlc::String(\"", 31)) + name) + mlc::String("\");\n", 4)); },
[&](const ast::VarTuple& varTuple) { auto [name, field_types, __2] = varTuple; return ((field_types.length() == 0) ? (((((mlc::String("  if (std::holds_alternative<", 29) + name) + mlc::String(">(self._)) return mlc::String(\"", 31)) + name) + mlc::String("\");\n", 4))) : (((((((mlc::String("  if (std::holds_alternative<", 29) + name) + mlc::String(">(self._)) return mlc::String(\"", 31)) + name) + mlc::String("(\") + mlc::to_string(std::get<", 30)) + name) + mlc::String(">(self._)._0) + mlc::String(\")\");\n", 34)))); },
[&](const ast::VarRecord& varRecord) { auto [name, field_definitions, __2] = varRecord; return derive_display_variant_record(name, field_definitions); }
}, (*variant));
}
mlc::String derive_display_variant_record(mlc::String name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept{
  if ((field_definitions.length() == 0))   {
    return ((((mlc::String("  if (std::holds_alternative<", 29) + name) + mlc::String(">(self._)) return mlc::String(\"", 31)) + name) + mlc::String("\");\n", 4));
  } else   {
    auto parts = (((((mlc::String("mlc::to_string(std::get<", 24) + name) + mlc::String(">(self._).", 10)) + field_definitions[0]->name) + mlc::String(")", 1)) + field_definitions.drop(1).fold(mlc::String("", 0), [=](mlc::String accumulator, std::shared_ptr<ast::FieldDef> field_definition) mutable { return (((((accumulator + mlc::String(" + mlc::String(\", \") + mlc::to_string(std::get<", 47)) + name) + mlc::String(">(self._).", 10)) + field_definition->name) + mlc::String(")", 1)); }));
    return ((((((mlc::String("  if (std::holds_alternative<", 29) + name) + mlc::String(">(self._)) return mlc::String(\"", 31)) + name) + mlc::String("(\") + ", 6)) + parts) + mlc::String(" + mlc::String(\")\");\n", 21));
  }
}
mlc::String derive_display_sum(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
  auto cases = variants.map([=](std::shared_ptr<ast::TypeVariant> variant) mutable { return derive_display_variant_case(variant); }).join(mlc::String("", 0));
  return (((((((((mlc::String("mlc::String ", 12) + mlc::to_string(type_name)) + mlc::String("_to_string(const ", 17)) + mlc::to_string(type_name)) + mlc::String("& self) noexcept {\n", 19)) + cases) + mlc::String("  return mlc::String(", 21)) + mlc::String("\"", 1)) + mlc::String("\"", 1)) + mlc::String(");\n}\n", 5));
}
mlc::String derive_eq_record(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs) noexcept{
  auto signature = ((((mlc::String("bool operator==(const ", 22) + mlc::to_string(type_name)) + mlc::String("& a, const ", 11)) + mlc::to_string(type_name)) + mlc::String("& b) noexcept", 13));
  if ((field_defs.length() == 0))   {
    return (signature + mlc::String(" { return true; }\n", 18));
  } else   {
    auto condition = (((((mlc::String("a.", 2) + mlc::to_string(field_defs[0]->name)) + mlc::String(" == b.", 6)) + mlc::to_string(field_defs[0]->name)) + mlc::String("", 0)) + field_defs.drop(1).fold(mlc::String("", 0), [=](mlc::String accumulator, std::shared_ptr<ast::FieldDef> field_definition) mutable { return (accumulator + ((((mlc::String(" && a.", 6) + mlc::to_string(field_definition->name)) + mlc::String(" == b.", 6)) + mlc::to_string(field_definition->name)) + mlc::String("", 0))); }));
    return (((signature + mlc::String(" { return ", 10)) + condition) + mlc::String("; }\n", 4));
  }
}
mlc::String derive_eq_sum(mlc::String type_name) noexcept{
  return ((((mlc::String("bool operator==(const ", 22) + mlc::to_string(type_name)) + mlc::String("& a, const ", 11)) + mlc::to_string(type_name)) + mlc::String("& b) noexcept { return a._ == b._; }\n", 37));
}
mlc::String derive_ord_build_cond(mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs) noexcept{
  auto condition = ((((mlc::String("a.", 2) + mlc::to_string(field_defs[0]->name)) + mlc::String(" < b.", 5)) + mlc::to_string(field_defs[0]->name)) + mlc::String("", 0));
  auto field_index = 1;
  while ((field_index < field_defs.length()))   {
    auto previous_equality = ((((mlc::String("a.", 2) + mlc::to_string(field_defs[0]->name)) + mlc::String(" == b.", 6)) + mlc::to_string(field_defs[0]->name)) + mlc::String("", 0));
    auto previous_field_index = 1;
    while ((previous_field_index < field_index))     {
      (previous_equality = (previous_equality + ((((mlc::String(" && a.", 6) + mlc::to_string(field_defs[previous_field_index]->name)) + mlc::String(" == b.", 6)) + mlc::to_string(field_defs[previous_field_index]->name)) + mlc::String("", 0))));
      (previous_field_index = (previous_field_index + 1));
    }
    (condition = (condition + ((((((mlc::String(" || (", 5) + mlc::to_string(previous_equality)) + mlc::String(" && a.", 6)) + mlc::to_string(field_defs[field_index]->name)) + mlc::String(" < b.", 5)) + mlc::to_string(field_defs[field_index]->name)) + mlc::String(")", 1))));
    (field_index = (field_index + 1));
  }
  return condition;
}
mlc::String derive_ord_record(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs) noexcept{
  auto signature = ((((mlc::String("bool operator<(const ", 21) + mlc::to_string(type_name)) + mlc::String("& a, const ", 11)) + mlc::to_string(type_name)) + mlc::String("& b) noexcept", 13));
  if ((field_defs.length() == 0))   {
    return (signature + mlc::String(" { return false; }\n", 19));
  } else   {
    return (((signature + mlc::String(" { return ", 10)) + derive_ord_build_cond(field_defs)) + mlc::String("; }\n", 4));
  }
}
mlc::String derive_ord_sum(mlc::String type_name) noexcept{
  return ((((mlc::String("bool operator<(const ", 21) + mlc::to_string(type_name)) + mlc::String("& a, const ", 11)) + mlc::to_string(type_name)) + mlc::String("& b) noexcept { return a._ < b._; }\n", 36));
}
mlc::String derive_hash_std_cpp_type(std::shared_ptr<ast::TypeExpr> field_type) noexcept{
  return std::visit(overloaded{[&](const ast::TyString& tyString) { return mlc::String("mlc::String", 11); },
[&](const ast::TyI32& tyI32) { return mlc::String("int", 3); },
[&](const ast::TyBool& tyBool) { return mlc::String("bool", 4); },
[&](const ast::TyUnit& tyUnit) { return mlc::String("", 0); },
[&](const ast::TyNamed& tyNamed) { auto [name] = tyNamed; return mlc::String("", 0); },
[&](const ast::TyArray& tyArray) { auto [inner] = tyArray; return mlc::String("", 0); },
[&](const ast::TyShared& tyShared) { auto [inner] = tyShared; return mlc::String("", 0); },
[&](const ast::TyGeneric& tyGeneric) { auto [name, type_arguments] = tyGeneric; return mlc::String("", 0); },
[&](const ast::TyFn& tyFn) { auto [parameter_types, return_type_expression] = tyFn; return mlc::String("", 0); },
[&](const ast::TyAssoc& tyAssoc) { auto [param, assoc] = tyAssoc; return mlc::String("", 0); }
}, (*field_type));
}
mlc::String derive_hash_combine_line(mlc::String cpp_type, mlc::String access_expr) noexcept{
  return ((((mlc::String("h ^= std::hash<", 15) + mlc::to_string(cpp_type)) + mlc::String(">{}(", 4)) + mlc::to_string(access_expr)) + mlc::String(") + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);\n", 49));
}
mlc::String derive_hash_record_operator_body(mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs) noexcept{
  if ((field_defs.length() == 0))   {
    return mlc::String("return static_cast<size_t>(1469598103934665603ULL);\n", 52);
  } else   {
    return (field_defs.fold(mlc::String("size_t h = 1469598103934665603ULL;\n", 35), [=](mlc::String accumulator, std::shared_ptr<ast::FieldDef> field_definition) mutable { return (accumulator + derive_hash_combine_line(derive_hash_std_cpp_type(field_definition->type_value), ((mlc::String("self.", 5) + mlc::to_string(field_definition->name)) + mlc::String("", 0)))); }) + mlc::String("return h;\n", 10));
  }
}
mlc::String derive_hash_record(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs) noexcept{
  auto operator_body = derive_hash_record_operator_body(field_defs);
  return ((((((mlc::String("namespace std {\ntemplate<>\nstruct hash<", 39) + mlc::to_string(type_name)) + mlc::String("> {\n  size_t operator()(const ", 30)) + mlc::to_string(type_name)) + mlc::String("& self) const noexcept {\n    ", 29)) + operator_body) + mlc::String("  }\n};\n}\n", 9));
}
mlc::String derive_hash_tuple_variant_inner(mlc::String variant_struct_name, mlc::Array<std::shared_ptr<ast::TypeExpr>> field_types, int discriminant_index) noexcept{
  return field_types.fold(TupleHashCodegenFoldState{0, derive_hash_combine_line(mlc::String("size_t", 6), mlc::to_string(discriminant_index))}, [=](TupleHashCodegenFoldState accumulated_hash_tuple_state, std::shared_ptr<ast::TypeExpr> field_type_expression_under_tuple) mutable { return TupleHashCodegenFoldState{(accumulated_hash_tuple_state.tuple_field_slot + 1), (accumulated_hash_tuple_state.concatenated_combine_lines + derive_hash_combine_line(derive_hash_std_cpp_type(field_type_expression_under_tuple), ((((mlc::String("std::get<", 9) + mlc::to_string(variant_struct_name)) + mlc::String(">(self._).field", 15)) + mlc::to_string(mlc::to_string(accumulated_hash_tuple_state.tuple_field_slot))) + mlc::String("", 0))))}; }).concatenated_combine_lines;
}
mlc::String derive_hash_record_variant_inner(mlc::String variant_struct_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs, int discriminant_index) noexcept{
  return field_defs.fold(derive_hash_combine_line(mlc::String("size_t", 6), mlc::to_string(discriminant_index)), [=](mlc::String accumulator, std::shared_ptr<ast::FieldDef> field_definition) mutable { return (accumulator + derive_hash_combine_line(derive_hash_std_cpp_type(field_definition->type_value), ((((mlc::String("std::get<", 9) + mlc::to_string(variant_struct_name)) + mlc::String(">(self._).", 10)) + mlc::to_string(field_definition->name)) + mlc::String("", 0)))); });
}
mlc::String derive_hash_sum_variant_inner(std::shared_ptr<ast::TypeVariant> variant, int discriminant_index) noexcept{
  return std::visit(overloaded{[&](const ast::VarUnit& varUnit) { auto [__0, __1] = varUnit; return derive_hash_combine_line(mlc::String("size_t", 6), mlc::to_string(discriminant_index)); },
[&](const ast::VarTuple& varTuple) { auto [variant_struct_name, field_types, __2] = varTuple; return derive_hash_tuple_variant_inner(variant_struct_name, field_types, discriminant_index); },
[&](const ast::VarRecord& varRecord) { auto [variant_struct_name, field_definitions, __2] = varRecord; return derive_hash_record_variant_inner(variant_struct_name, field_definitions, discriminant_index); }
}, (*variant));
}
mlc::String type_variant_constructor_name(std::shared_ptr<ast::TypeVariant> variant) noexcept{
  return std::visit(overloaded{[&](const ast::VarUnit& varUnit) { auto [name, __1] = varUnit; return name; },
[&](const ast::VarTuple& varTuple) { auto [name, __1, __2] = varTuple; return name; },
[&](const ast::VarRecord& varRecord) { auto [name, __1, __2] = varRecord; return name; }
}, (*variant));
}
mlc::String derive_hash_sum_branch(std::shared_ptr<ast::TypeVariant> variant, int discriminant_index) noexcept{
  auto constructor_name = type_variant_constructor_name(variant);
  auto variant_inner = derive_hash_sum_variant_inner(variant, discriminant_index);
  return ((((mlc::String("if (std::holds_alternative<", 27) + mlc::to_string(constructor_name)) + mlc::String(">(self._)) {\n    ", 17)) + variant_inner) + mlc::String("    return h;\n  }\n", 18));
}
mlc::String derive_hash_sum_operator_body(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
  return (variants.fold(HashSumOperatorFoldState{0, mlc::String("size_t h = 1469598103934665603ULL;\n", 35)}, [=](HashSumOperatorFoldState accumulated_hash_sum_state, std::shared_ptr<ast::TypeVariant> variant_under_hash_sum) mutable { return HashSumOperatorFoldState{(accumulated_hash_sum_state.next_discriminant_index + 1), (accumulated_hash_sum_state.operator_body_text + derive_hash_sum_branch(variant_under_hash_sum, accumulated_hash_sum_state.next_discriminant_index))}; }).operator_body_text + mlc::String("return h;\n", 10));
}
mlc::String derive_hash_sum(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
  auto operator_body = derive_hash_sum_operator_body(variants);
  return ((((((mlc::String("namespace std {\ntemplate<>\nstruct hash<", 39) + mlc::to_string(type_name)) + mlc::String("> {\n  size_t operator()(const ", 30)) + mlc::to_string(type_name)) + mlc::String("& self) const noexcept {\n    ", 29)) + operator_body) + mlc::String("  }\n};\n}\n", 9));
}
bool variants_is_single_record(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
  if ((variants.length() != 1))   {
    return false;
  } else   {
    return [&]() -> bool {
auto __match_subject = variants[0];
if (std::holds_alternative<ast::VarRecord>((*__match_subject))) {
const ast::VarRecord& varRecord = std::get<ast::VarRecord>((*__match_subject));
auto [__0, __1, __2] = varRecord; return true;
}
return false;
std::abort();
}();
  }
}
mlc::Array<std::shared_ptr<ast::FieldDef>> derive_record_field_defs(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
  auto empty = mlc::Array<std::shared_ptr<ast::FieldDef>>{};
  return [&]() -> mlc::Array<std::shared_ptr<ast::FieldDef>> {
auto __match_subject = variants[0];
if (std::holds_alternative<ast::VarRecord>((*__match_subject))) {
const ast::VarRecord& varRecord = std::get<ast::VarRecord>((*__match_subject));
auto [__0, field_definitions, __2] = varRecord; return field_definitions;
}
return empty;
std::abort();
}();
}
mlc::String gen_derive_record_trait(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::String trait_name) noexcept{
  auto field_definitions = derive_record_field_defs(variants);
  if ((trait_name == mlc::String("Display", 7)))   {
    return derive_display_record(type_name, field_definitions);
  } else if ((trait_name == mlc::String("Eq", 2)))   {
    return derive_eq_record(type_name, field_definitions);
  } else if ((trait_name == mlc::String("Ord", 3)))   {
    return derive_ord_record(type_name, field_definitions);
  } else if ((trait_name == mlc::String("Hash", 4)))   {
    return derive_hash_record(type_name, field_definitions);
  } else   {
    return mlc::String("", 0);
  }
}
mlc::String gen_derive_sum_trait(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::String trait_name) noexcept{
  if ((trait_name == mlc::String("Display", 7)))   {
    return derive_display_sum(type_name, variants);
  } else if ((trait_name == mlc::String("Eq", 2)))   {
    return derive_eq_sum(type_name);
  } else if ((trait_name == mlc::String("Ord", 3)))   {
    return derive_ord_sum(type_name);
  } else if ((trait_name == mlc::String("Hash", 4)))   {
    return derive_hash_sum(type_name, variants);
  } else   {
    return mlc::String("", 0);
  }
}
mlc::String gen_derive_methods(context::CodegenContext context, mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::Array<mlc::String> derive_traits) noexcept{
  if ((derive_traits.length() == 0))   {
    return mlc::String("", 0);
  } else   {
    auto is_record = variants_is_single_record(variants);
    return derive_traits.map([=](mlc::String trait_name) mutable { return (is_record ? (gen_derive_record_trait(type_name, variants, trait_name)) : (gen_derive_sum_trait(type_name, variants, trait_name))); }).join(mlc::String("", 0));
  }
}
mlc::String gen_type_decl_fwd_only(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
  return gen_type_decl_fwd(context, type_name, type_parameters, variants);
}
mlc::String gen_type_decl_body_only(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
  return gen_type_decl_body(context, type_name, type_parameters, variants);
}
mlc::String requires_clause_bound_part(mlc::Array<mlc::String> type_parameters, int type_parameter_index, mlc::String bound_trait_name) noexcept{
  if (((bound_trait_name == mlc::String("ExprVisitor", 11)) && (type_parameters.length() >= 2)))   {
    return expr::concept_trait_constraint_with_result_and_implementor(cpp_naming::cpp_safe(bound_trait_name), type_parameters[0], type_parameters[type_parameter_index]);
  } else   {
    return expr::concept_trait_constraint_on_type_parameter(cpp_naming::cpp_safe(bound_trait_name), type_parameters[type_parameter_index]);
  }
}
mlc::String requires_clause(mlc::Array<mlc::String> type_parameters, mlc::Array<mlc::Array<mlc::String>> type_bounds) noexcept{
  auto parts = mlc::Array<mlc::String>{};
  auto type_parameter_index = 0;
  while ((type_parameter_index < type_parameters.length()))   {
    if ((type_parameter_index < type_bounds.length()))     {
      (parts = parts.concat(type_bounds[type_parameter_index].map([=](mlc::String bound_trait_name) mutable { return requires_clause_bound_part(type_parameters, type_parameter_index, bound_trait_name); })));
    }
    (type_parameter_index = (type_parameter_index + 1));
  }
  if ((parts.length() > 0))   {
    return expr::concept_requires_clause_line(parts.join(mlc::String(" && ", 4)));
  } else   {
    return mlc::String("", 0);
  }
}

} // namespace type_gen
