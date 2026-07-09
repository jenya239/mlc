#define main mlc_user_main
#include "type_index.hpp"

#include "ast.hpp"
#include "decl_index.hpp"

namespace type_index {

using namespace ast;
using namespace decl_index;
using namespace ast_tokens;

mlc::String extract_method_name(mlc::String fn_name, mlc::String type_name) noexcept{
  auto prefix = (type_name + mlc::String("_", 1));
  if (((fn_name.length() > prefix.length()) && (fn_name.substring(0, prefix.length()) == prefix)))   {
    return fn_name.substring(prefix.length(), (fn_name.length() - prefix.length()));
  } else   {
    return fn_name;
  }
}
mlc::String extract_owner_prefix_from_mangled(mlc::String fn_name) noexcept{
  auto index = 0;
  auto result = fn_name;
  while ((index < fn_name.length()))   {
    if ((fn_name.substring(index, 1) == mlc::String("_", 1)))     {
      (result = fn_name.substring(0, index));
      (index = fn_name.length());
    } else     {
      (index = (index + 1));
    }
  }
  return result;
}
mlc::String trait_name_from_decl(std::shared_ptr<ast::Decl> declaration) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = ast::decl_inner(declaration);
if (std::holds_alternative<ast::DeclTrait>((*__match_subject))) {
const ast::DeclTrait& declTrait = std::get<ast::DeclTrait>((*__match_subject));
auto [trait_name, __1, __2, __3] = declTrait; return trait_name;
}
return mlc::String("", 0);
std::abort();
}();
}
mlc::String function_name_from_method_decl(std::shared_ptr<ast::Decl> method) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = method;
if (std::holds_alternative<ast::DeclFn>((*__match_subject))) {
const ast::DeclFn& declFn = std::get<ast::DeclFn>((*__match_subject));
auto [function_name, __1, __2, __3, __4, __5, __6] = declFn; return function_name;
}
return mlc::String("", 0);
std::abort();
}();
}
bool extend_may_register_method_owner(mlc::HashMap<mlc::String, mlc::String>& owners, mlc::String unmangled, mlc::String type_name, mlc::HashMap<mlc::String, bool> trait_names) noexcept{
  if ((!owners.has(unmangled)))   {
    return true;
  } else   {
    auto existing_prefix = extract_owner_prefix_from_mangled(owners.get(unmangled));
    return ((existing_prefix == type_name) || trait_names.has(existing_prefix));
  }
}
void register_trait_name_from_decl(mlc::HashMap<mlc::String, bool>& trait_names, std::shared_ptr<ast::Decl> declaration) noexcept{
  auto trait_name = trait_name_from_decl(declaration);
  if ((trait_name != mlc::String("", 0)))   {
    trait_names.set(trait_name, true);
  }
}
mlc::HashMap<mlc::String, bool> build_trait_names_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept{
  auto trait_names = mlc::HashMap<mlc::String, bool>();
  auto index = 0;
  while ((index < decls.length()))   {
    register_trait_name_from_decl(trait_names, decls[index]);
    (index = (index + 1));
  }
  return trait_names;
}
void register_function_method_owner(mlc::HashMap<mlc::String, mlc::String>& owners, mlc::String owner_name, std::shared_ptr<ast::Decl> method) noexcept{
  auto function_name = function_name_from_method_decl(method);
  if ((function_name != mlc::String("", 0)))   {
    auto unmangled = extract_method_name(function_name, owner_name);
    if ((!owners.has(unmangled)))     {
      owners.set(unmangled, function_name);
    }
  }
}
void register_trait_method_owners(mlc::HashMap<mlc::String, mlc::String>& owners, mlc::String trait_name, mlc::Array<std::shared_ptr<ast::Decl>> methods) noexcept{
  auto method_index = 0;
  return [&]() {
while ((method_index < methods.length())) {
register_function_method_owner(owners, trait_name, methods[method_index]);
(method_index = (method_index + 1));
}
}();
}
void register_extend_function_method_owner(mlc::HashMap<mlc::String, mlc::String>& owners, mlc::String type_name, std::shared_ptr<ast::Decl> method, mlc::HashMap<mlc::String, bool> trait_names) noexcept{
  auto function_name = function_name_from_method_decl(method);
  if ((function_name != mlc::String("", 0)))   {
    auto unmangled = extract_method_name(function_name, type_name);
    auto type_owner_key = ((type_name + mlc::String("_", 1)) + unmangled);
    owners.set(type_owner_key, function_name);
    if (extend_may_register_method_owner(owners, unmangled, type_name, trait_names))     {
      owners.set(unmangled, function_name);
    }
  }
}
void register_extend_method_owners(mlc::HashMap<mlc::String, mlc::String>& owners, mlc::String type_name, mlc::Array<std::shared_ptr<ast::Decl>> methods, mlc::HashMap<mlc::String, bool> trait_names) noexcept{
  auto method_index = 0;
  return [&]() {
while ((method_index < methods.length())) {
register_extend_function_method_owner(owners, type_name, methods[method_index], trait_names);
(method_index = (method_index + 1));
}
}();
}
mlc::String trait_name_and_methods_from_decl(std::shared_ptr<ast::Decl> declaration) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = ast::decl_inner(declaration);
if (std::holds_alternative<ast::DeclTrait>((*__match_subject))) {
const ast::DeclTrait& declTrait = std::get<ast::DeclTrait>((*__match_subject));
auto [trait_name, __1, __2, __3] = declTrait; return trait_name;
}
return mlc::String("", 0);
std::abort();
}();
}
mlc::String extend_type_name_and_methods_from_decl(std::shared_ptr<ast::Decl> declaration) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = ast::decl_inner(declaration);
if (std::holds_alternative<ast::DeclExtend>((*__match_subject))) {
const ast::DeclExtend& declExtend = std::get<ast::DeclExtend>((*__match_subject));
auto [type_name, __1, __2, __3] = declExtend; return type_name;
}
return mlc::String("", 0);
std::abort();
}();
}
mlc::Array<std::shared_ptr<ast::Decl>> methods_from_trait_decl(std::shared_ptr<ast::Decl> declaration) noexcept{
  return [&]() -> mlc::Array<std::shared_ptr<ast::Decl>> {
auto __match_subject = ast::decl_inner(declaration);
if (std::holds_alternative<ast::DeclTrait>((*__match_subject))) {
const ast::DeclTrait& declTrait = std::get<ast::DeclTrait>((*__match_subject));
auto [__0, __1, methods, __3] = declTrait; return methods;
}
return mlc::Array<std::shared_ptr<ast::Decl>>{};
std::abort();
}();
}
mlc::Array<std::shared_ptr<ast::Decl>> methods_from_extend_decl(std::shared_ptr<ast::Decl> declaration) noexcept{
  return [&]() -> mlc::Array<std::shared_ptr<ast::Decl>> {
auto __match_subject = ast::decl_inner(declaration);
if (std::holds_alternative<ast::DeclExtend>((*__match_subject))) {
const ast::DeclExtend& declExtend = std::get<ast::DeclExtend>((*__match_subject));
auto [__0, __1, methods, __3] = declExtend; return methods;
}
return mlc::Array<std::shared_ptr<ast::Decl>>{};
std::abort();
}();
}
void register_method_owners_from_decl(mlc::HashMap<mlc::String, mlc::String>& owners, std::shared_ptr<ast::Decl> declaration, mlc::HashMap<mlc::String, bool> trait_names) noexcept{
  auto trait_name = trait_name_and_methods_from_decl(declaration);
  if ((trait_name != mlc::String("", 0)))   {
    register_trait_method_owners(owners, trait_name, methods_from_trait_decl(declaration));
  }
  auto extend_type_name = extend_type_name_and_methods_from_decl(declaration);
  if ((extend_type_name != mlc::String("", 0)))   {
    register_extend_method_owners(owners, extend_type_name, methods_from_extend_decl(declaration), trait_names);
  }
}
void add_method_owners_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> declarations, mlc::HashMap<mlc::String, mlc::String>& owners) noexcept{
  auto trait_names = build_trait_names_from_decls(declarations);
  auto declaration_index = 0;
  return [&]() {
while ((declaration_index < declarations.length())) {
register_method_owners_from_decl(owners, declarations[declaration_index], trait_names);
(declaration_index = (declaration_index + 1));
}
}();
}
mlc::HashMap<mlc::String, mlc::String> build_method_owners_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> declarations) noexcept{
  auto owners = mlc::HashMap<mlc::String, mlc::String>();
  add_method_owners_from_decls(declarations, owners);
  return owners;
}
mlc::String variant_name_from_type_variant(std::shared_ptr<ast::TypeVariant> variant) noexcept{
  return std::visit(overloaded{[&](const ast::VarRecord& varRecord) { auto [name, __1, __2] = varRecord; return name; },
[&](const ast::VarTuple& varTuple) { auto [name, __1, __2] = varTuple; return name; },
[&](const ast::VarUnit& varUnit) { auto [name, __1] = varUnit; return name; }
}, (*variant));
}
mlc::String generic_variant_entry_name(mlc::Array<mlc::String> type_parameters, std::shared_ptr<ast::TypeVariant> variant) noexcept{
  if ((type_parameters.length() > 0))   {
    return variant_name_from_type_variant(variant);
  } else   {
    return mlc::String("__skip__", 8);
  }
}
mlc::Array<mlc::String> type_parameters_from_type_decl(std::shared_ptr<ast::Decl> declaration) noexcept{
  return [&]() -> mlc::Array<mlc::String> {
auto __match_subject = ast::decl_inner(declaration);
if (std::holds_alternative<ast::DeclType>((*__match_subject))) {
const ast::DeclType& declType = std::get<ast::DeclType>((*__match_subject));
auto [__0, type_parameters, __2, __3, __4] = declType; return type_parameters;
}
return mlc::Array<mlc::String>{};
std::abort();
}();
}
mlc::Array<std::shared_ptr<ast::TypeVariant>> variants_from_type_decl(std::shared_ptr<ast::Decl> declaration) noexcept{
  return [&]() -> mlc::Array<std::shared_ptr<ast::TypeVariant>> {
auto __match_subject = ast::decl_inner(declaration);
if (std::holds_alternative<ast::DeclType>((*__match_subject))) {
const ast::DeclType& declType = std::get<ast::DeclType>((*__match_subject));
auto [__0, __1, variants, __3, __4] = declType; return variants;
}
return mlc::Array<std::shared_ptr<ast::TypeVariant>>{};
std::abort();
}();
}
mlc::Array<mlc::String> append_generic_variant_names(mlc::Array<mlc::String> result, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
  auto accumulated = result;
  auto variant_index = 0;
  while ((variant_index < variants.length()))   {
    accumulated.push_back(generic_variant_entry_name(type_parameters, variants[variant_index]));
    (variant_index = (variant_index + 1));
  }
  return accumulated;
}
mlc::Array<mlc::String> generic_variants_from_decl(std::shared_ptr<ast::Decl> declaration) noexcept{
  auto type_parameters = type_parameters_from_type_decl(declaration);
  auto variants = variants_from_type_decl(declaration);
  if ((variants.length() == 0))   {
    auto empty = mlc::Array<mlc::String>{};
    return empty;
  } else   {
    return append_generic_variant_names(mlc::Array<mlc::String>{}, type_parameters, variants);
  }
}
mlc::Array<mlc::String> build_generic_variants_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> declarations) noexcept{
  auto result = mlc::Array<mlc::String>{};
  auto declaration_index = 0;
  while ((declaration_index < declarations.length()))   {
    auto decl_variants = generic_variants_from_decl(declarations[declaration_index]);
    auto variant_index = 0;
    while ((variant_index < decl_variants.length()))     {
      result.push_back(decl_variants[variant_index]);
      (variant_index = (variant_index + 1));
    }
    (declaration_index = (declaration_index + 1));
  }
  return result;
}
mlc::String type_name_from_type_decl(std::shared_ptr<ast::Decl> declaration) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = ast::decl_inner(declaration);
if (std::holds_alternative<ast::DeclType>((*__match_subject))) {
const ast::DeclType& declType = std::get<ast::DeclType>((*__match_subject));
auto [type_name, __1, __2, __3, __4] = declType; return type_name;
}
return mlc::String("", 0);
std::abort();
}();
}
void register_variant_type_mapping(mlc::HashMap<mlc::String, mlc::String> variant_map, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept{
  return variant_map.set(variant_name_from_type_variant(variant), type_name);
}
void register_variant_types_for_decl(mlc::HashMap<mlc::String, mlc::String> variant_map, std::shared_ptr<ast::Decl> declaration) noexcept{
  auto type_name = type_name_from_type_decl(declaration);
  auto variants = variants_from_type_decl(declaration);
  if ((type_name != mlc::String("", 0)))   {
    auto variant_index = 0;
    [&]() {
while ((variant_index < variants.length())) {
register_variant_type_mapping(variant_map, type_name, variants[variant_index]);
(variant_index = (variant_index + 1));
}
}();
  }
}
mlc::HashMap<mlc::String, mlc::String> build_variant_types_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> declarations) noexcept{
  auto variant_map = mlc::HashMap<mlc::String, mlc::String>();
  auto declaration_index = 0;
  while ((declaration_index < declarations.length()))   {
    register_variant_types_for_decl(variant_map, declarations[declaration_index]);
    (declaration_index = (declaration_index + 1));
  }
  return variant_map;
}
mlc::Array<mlc::String> field_names_from_field_definitions(mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept{
  auto field_names = mlc::Array<mlc::String>{};
  auto field_index = 0;
  while ((field_index < field_definitions.length()))   {
    field_names.push_back(field_definitions[field_index]->name);
    (field_index = (field_index + 1));
  }
  return field_names;
}
mlc::Array<std::shared_ptr<decl_index::FieldOrder>> field_orders_for_record_variant(mlc::String type_name, mlc::String variant_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions, int variant_count) noexcept{
  auto field_names = field_names_from_field_definitions(field_definitions);
  auto result = mlc::Array<std::shared_ptr<decl_index::FieldOrder>>{std::make_shared<decl_index::FieldOrder>(decl_index::FieldOrder{variant_name, field_names})};
  if ((variant_count == 1))   {
    result.push_back(std::make_shared<decl_index::FieldOrder>(decl_index::FieldOrder{type_name, field_names}));
  }
  return result;
}
mlc::String record_variant_name(std::shared_ptr<ast::TypeVariant> variant) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = variant;
if (std::holds_alternative<ast::VarRecord>((*__match_subject))) {
const ast::VarRecord& varRecord = std::get<ast::VarRecord>((*__match_subject));
auto [name, __1, __2] = varRecord; return name;
}
return mlc::String("", 0);
std::abort();
}();
}
mlc::Array<std::shared_ptr<ast::FieldDef>> record_field_definitions(std::shared_ptr<ast::TypeVariant> variant) noexcept{
  return [&]() -> mlc::Array<std::shared_ptr<ast::FieldDef>> {
auto __match_subject = variant;
if (std::holds_alternative<ast::VarRecord>((*__match_subject))) {
const ast::VarRecord& varRecord = std::get<ast::VarRecord>((*__match_subject));
auto [__0, field_definitions, __2] = varRecord; return field_definitions;
}
return mlc::Array<std::shared_ptr<ast::FieldDef>>{};
std::abort();
}();
}
mlc::Array<std::shared_ptr<decl_index::FieldOrder>> field_orders_for_type_variants(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
  auto result = mlc::Array<std::shared_ptr<decl_index::FieldOrder>>{};
  auto variant_index = 0;
  while ((variant_index < variants.length()))   {
    auto variant = variants[variant_index];
    auto variant_name = record_variant_name(variant);
    if ((variant_name != mlc::String("", 0)))     {
      auto new_orders = field_orders_for_record_variant(type_name, variant_name, record_field_definitions(variant), variants.length());
      auto order_index = 0;
      [&]() {
while ((order_index < new_orders.length())) {
result.push_back(new_orders[order_index]);
(order_index = (order_index + 1));
}
}();
    }
    (variant_index = (variant_index + 1));
  }
  return result;
}
std::shared_ptr<ast::Decl> exported_inner_decl(std::shared_ptr<ast::Decl> declaration) noexcept{
  return [&]() -> std::shared_ptr<ast::Decl> {
auto __match_subject = declaration;
if (std::holds_alternative<ast::DeclExported>((*__match_subject))) {
const ast::DeclExported& declExported = std::get<ast::DeclExported>((*__match_subject));
auto [inner] = declExported; return inner;
}
return declaration;
std::abort();
}();
}
mlc::String type_name_from_top_level_decl(std::shared_ptr<ast::Decl> declaration) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = exported_inner_decl(declaration);
if (std::holds_alternative<ast::DeclType>((*__match_subject))) {
const ast::DeclType& declType = std::get<ast::DeclType>((*__match_subject));
auto [type_name, __1, __2, __3, __4] = declType; return type_name;
}
return mlc::String("", 0);
std::abort();
}();
}
mlc::Array<std::shared_ptr<ast::TypeVariant>> variants_from_top_level_decl(std::shared_ptr<ast::Decl> declaration) noexcept{
  return [&]() -> mlc::Array<std::shared_ptr<ast::TypeVariant>> {
auto __match_subject = exported_inner_decl(declaration);
if (std::holds_alternative<ast::DeclType>((*__match_subject))) {
const ast::DeclType& declType = std::get<ast::DeclType>((*__match_subject));
auto [__0, __1, variants, __3, __4] = declType; return variants;
}
return mlc::Array<std::shared_ptr<ast::TypeVariant>>{};
std::abort();
}();
}
mlc::Array<std::shared_ptr<decl_index::FieldOrder>> field_orders_from_decl(std::shared_ptr<ast::Decl> declaration) noexcept{
  auto type_name = type_name_from_top_level_decl(declaration);
  auto variants = variants_from_top_level_decl(declaration);
  if ((type_name != mlc::String("", 0)))   {
    return field_orders_for_type_variants(type_name, variants);
  } else   {
    return mlc::Array<std::shared_ptr<decl_index::FieldOrder>>{};
  }
}
mlc::Array<std::shared_ptr<decl_index::FieldOrder>> build_field_orders(ast::Program program) noexcept{
  auto orders = mlc::Array<std::shared_ptr<decl_index::FieldOrder>>{};
  auto declaration_index = 0;
  while ((declaration_index < program.decls.length()))   {
    auto decl_orders = field_orders_from_decl(program.decls[declaration_index]);
    auto order_index = 0;
    while ((order_index < decl_orders.length()))     {
      orders.push_back(decl_orders[order_index]);
      (order_index = (order_index + 1));
    }
    (declaration_index = (declaration_index + 1));
  }
  return orders;
}
bool type_param_in_typeexpr_for_index(mlc::String parameter_name, std::shared_ptr<ast::TypeExpr> type_expression) noexcept{
  return [&]() -> bool {
auto __match_subject = type_expression;
if (std::holds_alternative<ast::TyNamed>((*__match_subject))) {
const ast::TyNamed& tyNamed = std::get<ast::TyNamed>((*__match_subject));
auto [name] = tyNamed; return (name == parameter_name);
}
if (std::holds_alternative<ast::TyArray>((*__match_subject))) {
const ast::TyArray& tyArray = std::get<ast::TyArray>((*__match_subject));
auto [inner] = tyArray; return type_param_in_typeexpr_for_index(parameter_name, inner);
}
if (std::holds_alternative<ast::TyShared>((*__match_subject))) {
const ast::TyShared& tyShared = std::get<ast::TyShared>((*__match_subject));
auto [inner] = tyShared; return type_param_in_typeexpr_for_index(parameter_name, inner);
}
if (std::holds_alternative<ast::TyGeneric>((*__match_subject))) {
const ast::TyGeneric& tyGeneric = std::get<ast::TyGeneric>((*__match_subject));
auto [__0, type_arguments] = tyGeneric; return type_param_in_typeexpr_list_for_index(parameter_name, type_arguments);
}
if (std::holds_alternative<ast::TyFn>((*__match_subject))) {
const ast::TyFn& tyFn = std::get<ast::TyFn>((*__match_subject));
auto [parameter_types, return_type_expression] = tyFn; return (type_param_in_typeexpr_list_for_index(parameter_name, parameter_types) || type_param_in_typeexpr_for_index(parameter_name, return_type_expression));
}
if (std::holds_alternative<ast::TyAssoc>((*__match_subject))) {
const ast::TyAssoc& tyAssoc = std::get<ast::TyAssoc>((*__match_subject));
auto [associated_parameter, __1] = tyAssoc; return (associated_parameter == parameter_name);
}
return false;
std::abort();
}();
}
bool type_param_in_typeexpr_list_for_index(mlc::String parameter_name, mlc::Array<std::shared_ptr<ast::TypeExpr>> types) noexcept{
  return types.any([=](std::shared_ptr<ast::TypeExpr> candidate) mutable { return type_param_in_typeexpr_for_index(parameter_name, candidate); });
}
mlc::Array<std::shared_ptr<ast::TypeExpr>> variant_field_typeexprs_for_index(std::shared_ptr<ast::TypeVariant> variant) noexcept{
  return std::visit(overloaded{[&](const ast::VarTuple& varTuple) { auto [__0, field_types, __2] = varTuple; return field_types; },
[&](const ast::VarRecord& varRecord) { auto [__0, field_definitions, __2] = varRecord; return field_definitions.map([=](std::shared_ptr<ast::FieldDef> field_definition) mutable { return field_definition->type_value; }); },
[&](const ast::VarUnit& varUnit) { auto [__0, __1] = varUnit; return [&]() {
auto empty = mlc::Array<std::shared_ptr<ast::TypeExpr>>{};
return empty;
}(); }
}, (*variant));
}
mlc::Array<mlc::String> variant_used_type_parameters_for_index(mlc::Array<mlc::String> type_parameters, std::shared_ptr<ast::TypeVariant> variant) noexcept{
  auto field_types = variant_field_typeexprs_for_index(variant);
  return type_parameters.filter([=](mlc::String type_parameter) mutable { return type_param_in_typeexpr_list_for_index(type_parameter, field_types); });
}
void register_sum_type_parameter_names(mlc::HashMap<mlc::String, mlc::Array<mlc::String>> parameter_map, std::shared_ptr<ast::Decl> declaration) noexcept{
  auto type_name = type_name_from_type_decl(declaration);
  auto type_parameters = type_parameters_from_type_decl(declaration);
  if (((type_name != mlc::String("", 0)) && (type_parameters.length() > 0)))   {
    parameter_map.set(type_name, type_parameters);
  }
}
mlc::HashMap<mlc::String, mlc::Array<mlc::String>> build_sum_type_parameter_names_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> declarations) noexcept{
  auto parameter_map = mlc::HashMap<mlc::String, mlc::Array<mlc::String>>();
  auto declaration_index = 0;
  while ((declaration_index < declarations.length()))   {
    register_sum_type_parameter_names(parameter_map, declarations[declaration_index]);
    (declaration_index = (declaration_index + 1));
  }
  return parameter_map;
}
void register_variant_used_type_parameter_names(mlc::HashMap<mlc::String, mlc::Array<mlc::String>> used_map, mlc::String type_name, mlc::Array<mlc::String> type_parameters, std::shared_ptr<ast::TypeVariant> variant) noexcept{
  if ((type_parameters.length() > 0))   {
    used_map.set(variant_name_from_type_variant(variant), variant_used_type_parameters_for_index(type_parameters, variant));
  }
}
void register_variant_used_type_parameter_names_for_decl(mlc::HashMap<mlc::String, mlc::Array<mlc::String>> used_map, std::shared_ptr<ast::Decl> declaration) noexcept{
  auto type_name = type_name_from_type_decl(declaration);
  auto type_parameters = type_parameters_from_type_decl(declaration);
  auto variants = variants_from_type_decl(declaration);
  if (((type_name != mlc::String("", 0)) && (type_parameters.length() > 0)))   {
    auto variant_index = 0;
    [&]() {
while ((variant_index < variants.length())) {
register_variant_used_type_parameter_names(used_map, type_name, type_parameters, variants[variant_index]);
(variant_index = (variant_index + 1));
}
}();
  }
}
mlc::HashMap<mlc::String, mlc::Array<mlc::String>> build_variant_used_type_parameter_names_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> declarations) noexcept{
  auto used_map = mlc::HashMap<mlc::String, mlc::Array<mlc::String>>();
  auto declaration_index = 0;
  while ((declaration_index < declarations.length()))   {
    register_variant_used_type_parameter_names_for_decl(used_map, declarations[declaration_index]);
    (declaration_index = (declaration_index + 1));
  }
  return used_map;
}

} // namespace type_index
