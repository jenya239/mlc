#include "type_index.hpp"

#include "ast.hpp"
#include "decl_index.hpp"

namespace type_index {

using namespace ast;
using namespace decl_index;
using namespace ast_tokens;

mlc::String extract_method_name(mlc::String fn_name, mlc::String type_name) noexcept;

mlc::String extract_owner_prefix_from_mangled(mlc::String fn_name) noexcept;

mlc::String trait_name_from_decl(std::shared_ptr<ast::Decl> declaration) noexcept;

mlc::String function_name_from_method_decl(std::shared_ptr<ast::Decl> method) noexcept;

bool extend_may_register_method_owner(mlc::HashMap<mlc::String, mlc::String>& owners, mlc::String unmangled, mlc::String type_name, mlc::HashMap<mlc::String, bool> trait_names) noexcept;

void register_trait_name_from_decl(mlc::HashMap<mlc::String, bool>& trait_names, std::shared_ptr<ast::Decl> declaration) noexcept;

mlc::HashMap<mlc::String, bool> build_trait_names_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept;

void register_function_method_owner(mlc::HashMap<mlc::String, mlc::String>& owners, mlc::String owner_name, std::shared_ptr<ast::Decl> method) noexcept;

void register_trait_method_owners(mlc::HashMap<mlc::String, mlc::String>& owners, mlc::String trait_name, mlc::Array<std::shared_ptr<ast::Decl>> methods) noexcept;

void register_extend_function_method_owner(mlc::HashMap<mlc::String, mlc::String>& owners, mlc::String type_name, std::shared_ptr<ast::Decl> method, mlc::HashMap<mlc::String, bool> trait_names) noexcept;

void register_extend_method_owners(mlc::HashMap<mlc::String, mlc::String>& owners, mlc::String type_name, mlc::Array<std::shared_ptr<ast::Decl>> methods, mlc::HashMap<mlc::String, bool> trait_names) noexcept;

mlc::String trait_name_and_methods_from_decl(std::shared_ptr<ast::Decl> declaration) noexcept;

mlc::String extend_type_name_and_methods_from_decl(std::shared_ptr<ast::Decl> declaration) noexcept;

mlc::Array<std::shared_ptr<ast::Decl>> methods_from_trait_decl(std::shared_ptr<ast::Decl> declaration) noexcept;

mlc::Array<std::shared_ptr<ast::Decl>> methods_from_extend_decl(std::shared_ptr<ast::Decl> declaration) noexcept;

void register_method_owners_from_decl(mlc::HashMap<mlc::String, mlc::String>& owners, std::shared_ptr<ast::Decl> declaration, mlc::HashMap<mlc::String, bool> trait_names) noexcept;

void add_method_owners_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> declarations, mlc::HashMap<mlc::String, mlc::String>& owners) noexcept;

mlc::HashMap<mlc::String, mlc::String> build_method_owners_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> declarations) noexcept;

mlc::String variant_name_from_type_variant(std::shared_ptr<ast::TypeVariant> variant) noexcept;

mlc::String generic_variant_entry_name(mlc::Array<mlc::String> type_parameters, std::shared_ptr<ast::TypeVariant> variant) noexcept;

mlc::Array<mlc::String> type_parameters_from_type_decl(std::shared_ptr<ast::Decl> declaration) noexcept;

mlc::Array<std::shared_ptr<ast::TypeVariant>> variants_from_type_decl(std::shared_ptr<ast::Decl> declaration) noexcept;

mlc::Array<mlc::String> append_generic_variant_names(mlc::Array<mlc::String> result, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::Array<mlc::String> generic_variants_from_decl(std::shared_ptr<ast::Decl> declaration) noexcept;

mlc::Array<mlc::String> build_generic_variants_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> declarations) noexcept;

mlc::String type_name_from_type_decl(std::shared_ptr<ast::Decl> declaration) noexcept;

void register_variant_type_mapping(mlc::HashMap<mlc::String, mlc::String> variant_map, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept;

void register_variant_types_for_decl(mlc::HashMap<mlc::String, mlc::String> variant_map, std::shared_ptr<ast::Decl> declaration) noexcept;

mlc::HashMap<mlc::String, mlc::String> build_variant_types_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> declarations) noexcept;

mlc::Array<mlc::String> field_names_from_field_definitions(mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept;

mlc::Array<std::shared_ptr<decl_index::FieldOrder>> field_orders_for_record_variant(mlc::String type_name, mlc::String variant_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions, int variant_count) noexcept;

mlc::String record_variant_name(std::shared_ptr<ast::TypeVariant> variant) noexcept;

mlc::Array<std::shared_ptr<ast::FieldDef>> record_field_definitions(std::shared_ptr<ast::TypeVariant> variant) noexcept;

mlc::Array<std::shared_ptr<decl_index::FieldOrder>> field_orders_for_type_variants(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

std::shared_ptr<ast::Decl> exported_inner_decl(std::shared_ptr<ast::Decl> declaration) noexcept;

mlc::String type_name_from_top_level_decl(std::shared_ptr<ast::Decl> declaration) noexcept;

mlc::Array<std::shared_ptr<ast::TypeVariant>> variants_from_top_level_decl(std::shared_ptr<ast::Decl> declaration) noexcept;

mlc::Array<std::shared_ptr<decl_index::FieldOrder>> field_orders_from_decl(std::shared_ptr<ast::Decl> declaration) noexcept;

mlc::Array<std::shared_ptr<decl_index::FieldOrder>> build_field_orders(ast::Program program) noexcept;

bool type_param_in_typeexpr_for_index(mlc::String parameter_name, std::shared_ptr<ast::TypeExpr> type_expression) noexcept;

bool type_param_in_typeexpr_list_for_index(mlc::String parameter_name, mlc::Array<std::shared_ptr<ast::TypeExpr>> types) noexcept;

mlc::Array<std::shared_ptr<ast::TypeExpr>> variant_field_typeexprs_for_index(std::shared_ptr<ast::TypeVariant> variant) noexcept;

mlc::Array<mlc::String> variant_used_type_parameters_for_index(mlc::Array<mlc::String> type_parameters, std::shared_ptr<ast::TypeVariant> variant) noexcept;

void register_sum_type_parameter_names(mlc::HashMap<mlc::String, mlc::Array<mlc::String>> parameter_map, std::shared_ptr<ast::Decl> declaration) noexcept;

mlc::HashMap<mlc::String, mlc::Array<mlc::String>> build_sum_type_parameter_names_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> declarations) noexcept;

void register_variant_used_type_parameter_names(mlc::HashMap<mlc::String, mlc::Array<mlc::String>> used_map, mlc::String type_name, mlc::Array<mlc::String> type_parameters, std::shared_ptr<ast::TypeVariant> variant) noexcept;

void register_variant_used_type_parameter_names_for_decl(mlc::HashMap<mlc::String, mlc::Array<mlc::String>> used_map, std::shared_ptr<ast::Decl> declaration) noexcept;

mlc::HashMap<mlc::String, mlc::Array<mlc::String>> build_variant_used_type_parameter_names_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> declarations) noexcept;

mlc::String extract_method_name(mlc::String fn_name, mlc::String type_name) noexcept{
mlc::String prefix = type_name + mlc::String("_");
return fn_name.length() > prefix.length() && fn_name.substring(0, prefix.length()) == prefix ? fn_name.substring(prefix.length(), fn_name.length() - prefix.length()) : fn_name;
}

mlc::String extract_owner_prefix_from_mangled(mlc::String fn_name) noexcept{
int index = 0;
mlc::String result = fn_name;
while (index < fn_name.length()){
{
if (fn_name.substring(index, 1) == mlc::String("_")){
{
result = fn_name.substring(0, index);
index = fn_name.length();
}
} else {
{
index = index + 1;
}
}
}
}
return result;
}

mlc::String trait_name_from_decl(std::shared_ptr<ast::Decl> declaration) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::DeclTrait>((*ast::decl_inner(declaration)))) { auto _v_decltrait = std::get<ast::DeclTrait>((*ast::decl_inner(declaration))); auto [trait_name, _w0, _w1, _w2] = _v_decltrait; return trait_name; } return mlc::String(""); }();}

mlc::String function_name_from_method_decl(std::shared_ptr<ast::Decl> method) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::DeclFn>((*method))) { auto _v_declfn = std::get<ast::DeclFn>((*method)); auto [function_name, _w0, _w1, _w2, _w3, _w4, _w5] = _v_declfn; return function_name; } return mlc::String(""); }();}

bool extend_may_register_method_owner(mlc::HashMap<mlc::String, mlc::String>& owners, mlc::String unmangled, mlc::String type_name, mlc::HashMap<mlc::String, bool> trait_names) noexcept{return !owners.has(unmangled) ? true : [&]() -> bool { 
  mlc::String existing_prefix = extract_owner_prefix_from_mangled(owners.get(unmangled));
  return existing_prefix == type_name || trait_names.has(existing_prefix);
 }();}

void register_trait_name_from_decl(mlc::HashMap<mlc::String, bool>& trait_names, std::shared_ptr<ast::Decl> declaration) noexcept{
mlc::String trait_name = trait_name_from_decl(declaration);
if (trait_name != mlc::String("")){
trait_names.set(trait_name, true);
}
}

mlc::HashMap<mlc::String, bool> build_trait_names_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept{
mlc::HashMap<mlc::String, bool> trait_names = mlc::HashMap<mlc::String, bool>();
int index = 0;
while (index < decls.size()){
{
register_trait_name_from_decl(trait_names, decls[index]);
index = index + 1;
}
}
return trait_names;
}

void register_function_method_owner(mlc::HashMap<mlc::String, mlc::String>& owners, mlc::String owner_name, std::shared_ptr<ast::Decl> method) noexcept{
mlc::String function_name = function_name_from_method_decl(method);
if (function_name != mlc::String("")){
mlc::String unmangled = extract_method_name(function_name, owner_name);
if (!owners.has(unmangled)){
owners.set(unmangled, function_name);
}
}
}

void register_trait_method_owners(mlc::HashMap<mlc::String, mlc::String>& owners, mlc::String trait_name, mlc::Array<std::shared_ptr<ast::Decl>> methods) noexcept{
int method_index = 0;
return [&]() { 
  while (method_index < methods.size()){
{
register_function_method_owner(owners, trait_name, methods[method_index]);
method_index = method_index + 1;
}
}
 }();
}

void register_extend_function_method_owner(mlc::HashMap<mlc::String, mlc::String>& owners, mlc::String type_name, std::shared_ptr<ast::Decl> method, mlc::HashMap<mlc::String, bool> trait_names) noexcept{
mlc::String function_name = function_name_from_method_decl(method);
if (function_name != mlc::String("")){
mlc::String unmangled = extract_method_name(function_name, type_name);
mlc::String type_owner_key = type_name + mlc::String("_") + unmangled;
owners.set(type_owner_key, function_name);
if (extend_may_register_method_owner(owners, unmangled, type_name, trait_names)){
owners.set(unmangled, function_name);
}
}
}

void register_extend_method_owners(mlc::HashMap<mlc::String, mlc::String>& owners, mlc::String type_name, mlc::Array<std::shared_ptr<ast::Decl>> methods, mlc::HashMap<mlc::String, bool> trait_names) noexcept{
int method_index = 0;
return [&]() { 
  while (method_index < methods.size()){
{
register_extend_function_method_owner(owners, type_name, methods[method_index], trait_names);
method_index = method_index + 1;
}
}
 }();
}

mlc::String trait_name_and_methods_from_decl(std::shared_ptr<ast::Decl> declaration) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::DeclTrait>((*ast::decl_inner(declaration)))) { auto _v_decltrait = std::get<ast::DeclTrait>((*ast::decl_inner(declaration))); auto [trait_name, _w0, _w1, _w2] = _v_decltrait; return trait_name; } return mlc::String(""); }();}

mlc::String extend_type_name_and_methods_from_decl(std::shared_ptr<ast::Decl> declaration) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::DeclExtend>((*ast::decl_inner(declaration)))) { auto _v_declextend = std::get<ast::DeclExtend>((*ast::decl_inner(declaration))); auto [type_name, _w0, _w1, _w2] = _v_declextend; return type_name; } return mlc::String(""); }();}

mlc::Array<std::shared_ptr<ast::Decl>> methods_from_trait_decl(std::shared_ptr<ast::Decl> declaration) noexcept{return [&]() -> mlc::Array<std::shared_ptr<ast::Decl>> { if (std::holds_alternative<ast::DeclTrait>((*ast::decl_inner(declaration)))) { auto _v_decltrait = std::get<ast::DeclTrait>((*ast::decl_inner(declaration))); auto [_w0, _w1, methods, _w2] = _v_decltrait; return methods; } return {}; }();}

mlc::Array<std::shared_ptr<ast::Decl>> methods_from_extend_decl(std::shared_ptr<ast::Decl> declaration) noexcept{return [&]() -> mlc::Array<std::shared_ptr<ast::Decl>> { if (std::holds_alternative<ast::DeclExtend>((*ast::decl_inner(declaration)))) { auto _v_declextend = std::get<ast::DeclExtend>((*ast::decl_inner(declaration))); auto [_w0, _w1, methods, _w2] = _v_declextend; return methods; } return {}; }();}

void register_method_owners_from_decl(mlc::HashMap<mlc::String, mlc::String>& owners, std::shared_ptr<ast::Decl> declaration, mlc::HashMap<mlc::String, bool> trait_names) noexcept{
mlc::String trait_name = trait_name_and_methods_from_decl(declaration);
if (trait_name != mlc::String("")){
{
register_trait_method_owners(owners, trait_name, methods_from_trait_decl(declaration));
}
}
mlc::String extend_type_name = extend_type_name_and_methods_from_decl(declaration);
if (extend_type_name != mlc::String("")){
register_extend_method_owners(owners, extend_type_name, methods_from_extend_decl(declaration), trait_names);
}
}

void add_method_owners_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> declarations, mlc::HashMap<mlc::String, mlc::String>& owners) noexcept{
mlc::HashMap<mlc::String, bool> trait_names = build_trait_names_from_decls(declarations);
int declaration_index = 0;
return [&]() { 
  while (declaration_index < declarations.size()){
{
register_method_owners_from_decl(owners, declarations[declaration_index], trait_names);
declaration_index = declaration_index + 1;
}
}
 }();
}

mlc::HashMap<mlc::String, mlc::String> build_method_owners_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> declarations) noexcept{
mlc::HashMap<mlc::String, mlc::String> owners = mlc::HashMap<mlc::String, mlc::String>();
add_method_owners_from_decls(declarations, owners);
return owners;
}

mlc::String variant_name_from_type_variant(std::shared_ptr<ast::TypeVariant> variant) noexcept{return std::visit(overloaded{
  [&](const VarRecord& varrecord) -> mlc::String { auto [name, _w0, _w1] = varrecord; return name; },
  [&](const VarTuple& vartuple) -> mlc::String { auto [name, _w0, _w1] = vartuple; return name; },
  [&](const VarUnit& varunit) -> mlc::String { auto [name, _w0] = varunit; return name; }
}, (*variant));}

mlc::String generic_variant_entry_name(mlc::Array<mlc::String> type_parameters, std::shared_ptr<ast::TypeVariant> variant) noexcept{return type_parameters.size() > 0 ? variant_name_from_type_variant(variant) : mlc::String("__skip__");}

mlc::Array<mlc::String> type_parameters_from_type_decl(std::shared_ptr<ast::Decl> declaration) noexcept{return [&]() -> mlc::Array<mlc::String> { if (std::holds_alternative<ast::DeclType>((*ast::decl_inner(declaration)))) { auto _v_decltype = std::get<ast::DeclType>((*ast::decl_inner(declaration))); auto [_w0, type_parameters, _w1, _w2, _w3] = _v_decltype; return type_parameters; } return {}; }();}

mlc::Array<std::shared_ptr<ast::TypeVariant>> variants_from_type_decl(std::shared_ptr<ast::Decl> declaration) noexcept{return [&]() -> mlc::Array<std::shared_ptr<ast::TypeVariant>> { if (std::holds_alternative<ast::DeclType>((*ast::decl_inner(declaration)))) { auto _v_decltype = std::get<ast::DeclType>((*ast::decl_inner(declaration))); auto [_w0, _w1, variants, _w2, _w3] = _v_decltype; return variants; } return {}; }();}

mlc::Array<mlc::String> append_generic_variant_names(mlc::Array<mlc::String> result, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
mlc::Array<mlc::String> accumulated = result;
int variant_index = 0;
while (variant_index < variants.size()){
{
accumulated.push_back(generic_variant_entry_name(type_parameters, variants[variant_index]));
variant_index = variant_index + 1;
}
}
return accumulated;
}

mlc::Array<mlc::String> generic_variants_from_decl(std::shared_ptr<ast::Decl> declaration) noexcept{
mlc::Array<mlc::String> type_parameters = type_parameters_from_type_decl(declaration);
mlc::Array<std::shared_ptr<ast::TypeVariant>> variants = variants_from_type_decl(declaration);
return variants.size() == 0 ? [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> empty = {};
  return empty;
 }() : append_generic_variant_names({}, type_parameters, variants);
}

mlc::Array<mlc::String> build_generic_variants_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> declarations) noexcept{
mlc::Array<mlc::String> result = {};
int declaration_index = 0;
while (declaration_index < declarations.size()){
{
mlc::Array<mlc::String> decl_variants = generic_variants_from_decl(declarations[declaration_index]);
int variant_index = 0;
while (variant_index < decl_variants.size()){
{
result.push_back(decl_variants[variant_index]);
variant_index = variant_index + 1;
}
}
declaration_index = declaration_index + 1;
}
}
return result;
}

mlc::String type_name_from_type_decl(std::shared_ptr<ast::Decl> declaration) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::DeclType>((*ast::decl_inner(declaration)))) { auto _v_decltype = std::get<ast::DeclType>((*ast::decl_inner(declaration))); auto [type_name, _w0, _w1, _w2, _w3] = _v_decltype; return type_name; } return mlc::String(""); }();}

void register_variant_type_mapping(mlc::HashMap<mlc::String, mlc::String> variant_map, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept{return variant_map.set(variant_name_from_type_variant(variant), type_name);}

void register_variant_types_for_decl(mlc::HashMap<mlc::String, mlc::String> variant_map, std::shared_ptr<ast::Decl> declaration) noexcept{
mlc::String type_name = type_name_from_type_decl(declaration);
mlc::Array<std::shared_ptr<ast::TypeVariant>> variants = variants_from_type_decl(declaration);
if (type_name != mlc::String("")){
int variant_index = 0;
[&]() { 
  while (variant_index < variants.size()){
{
register_variant_type_mapping(variant_map, type_name, variants[variant_index]);
variant_index = variant_index + 1;
}
}
 }();
}
}

mlc::HashMap<mlc::String, mlc::String> build_variant_types_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> declarations) noexcept{
mlc::HashMap<mlc::String, mlc::String> variant_map = mlc::HashMap<mlc::String, mlc::String>();
int declaration_index = 0;
while (declaration_index < declarations.size()){
{
register_variant_types_for_decl(variant_map, declarations[declaration_index]);
declaration_index = declaration_index + 1;
}
}
return variant_map;
}

mlc::Array<mlc::String> field_names_from_field_definitions(mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept{
mlc::Array<mlc::String> field_names = {};
int field_index = 0;
while (field_index < field_definitions.size()){
{
field_names.push_back(field_definitions[field_index]->name);
field_index = field_index + 1;
}
}
return field_names;
}

mlc::Array<std::shared_ptr<decl_index::FieldOrder>> field_orders_for_record_variant(mlc::String type_name, mlc::String variant_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions, int variant_count) noexcept{
mlc::Array<mlc::String> field_names = field_names_from_field_definitions(field_definitions);
mlc::Array<std::shared_ptr<decl_index::FieldOrder>> result = mlc::Array<std::shared_ptr<decl_index::FieldOrder>>{std::make_shared<decl_index::FieldOrder>(decl_index::FieldOrder{variant_name, field_names})};
if (variant_count == 1){
{
result.push_back(std::make_shared<decl_index::FieldOrder>(decl_index::FieldOrder{type_name, field_names}));
}
}
return result;
}

mlc::String record_variant_name(std::shared_ptr<ast::TypeVariant> variant) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::VarRecord>((*variant))) { auto _v_varrecord = std::get<ast::VarRecord>((*variant)); auto [name, _w0, _w1] = _v_varrecord; return name; } return mlc::String(""); }();}

mlc::Array<std::shared_ptr<ast::FieldDef>> record_field_definitions(std::shared_ptr<ast::TypeVariant> variant) noexcept{return [&]() -> mlc::Array<std::shared_ptr<ast::FieldDef>> { if (std::holds_alternative<ast::VarRecord>((*variant))) { auto _v_varrecord = std::get<ast::VarRecord>((*variant)); auto [_w0, field_definitions, _w1] = _v_varrecord; return field_definitions; } return {}; }();}

mlc::Array<std::shared_ptr<decl_index::FieldOrder>> field_orders_for_type_variants(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
mlc::Array<std::shared_ptr<decl_index::FieldOrder>> result = {};
int variant_index = 0;
while (variant_index < variants.size()){
{
std::shared_ptr<ast::TypeVariant> variant = variants[variant_index];
mlc::String variant_name = record_variant_name(variant);
if (variant_name != mlc::String("")){
{
mlc::Array<std::shared_ptr<decl_index::FieldOrder>> new_orders = field_orders_for_record_variant(type_name, variant_name, record_field_definitions(variant), variants.size());
int order_index = 0;
[&]() { 
  while (order_index < new_orders.size()){
{
result.push_back(new_orders[order_index]);
order_index = order_index + 1;
}
}
 }();
}
}
variant_index = variant_index + 1;
}
}
return result;
}

std::shared_ptr<ast::Decl> exported_inner_decl(std::shared_ptr<ast::Decl> declaration) noexcept{return [&]() -> std::shared_ptr<ast::Decl> { if (std::holds_alternative<ast::DeclExported>((*declaration))) { auto _v_declexported = std::get<ast::DeclExported>((*declaration)); auto [inner] = _v_declexported; return inner; } return declaration; }();}

mlc::String type_name_from_top_level_decl(std::shared_ptr<ast::Decl> declaration) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::DeclType>((*exported_inner_decl(declaration)))) { auto _v_decltype = std::get<ast::DeclType>((*exported_inner_decl(declaration))); auto [type_name, _w0, _w1, _w2, _w3] = _v_decltype; return type_name; } return mlc::String(""); }();}

mlc::Array<std::shared_ptr<ast::TypeVariant>> variants_from_top_level_decl(std::shared_ptr<ast::Decl> declaration) noexcept{return [&]() -> mlc::Array<std::shared_ptr<ast::TypeVariant>> { if (std::holds_alternative<ast::DeclType>((*exported_inner_decl(declaration)))) { auto _v_decltype = std::get<ast::DeclType>((*exported_inner_decl(declaration))); auto [_w0, _w1, variants, _w2, _w3] = _v_decltype; return variants; } return {}; }();}

mlc::Array<std::shared_ptr<decl_index::FieldOrder>> field_orders_from_decl(std::shared_ptr<ast::Decl> declaration) noexcept{
mlc::String type_name = type_name_from_top_level_decl(declaration);
mlc::Array<std::shared_ptr<ast::TypeVariant>> variants = variants_from_top_level_decl(declaration);
return type_name != mlc::String("") ? field_orders_for_type_variants(type_name, variants) : mlc::Array<std::shared_ptr<decl_index::FieldOrder>>{};
}

mlc::Array<std::shared_ptr<decl_index::FieldOrder>> build_field_orders(ast::Program program) noexcept{
mlc::Array<std::shared_ptr<decl_index::FieldOrder>> orders = {};
int declaration_index = 0;
while (declaration_index < program.decls.size()){
{
mlc::Array<std::shared_ptr<decl_index::FieldOrder>> decl_orders = field_orders_from_decl(program.decls[declaration_index]);
int order_index = 0;
while (order_index < decl_orders.size()){
{
orders.push_back(decl_orders[order_index]);
order_index = order_index + 1;
}
}
declaration_index = declaration_index + 1;
}
}
return orders;
}

bool type_param_in_typeexpr_for_index(mlc::String parameter_name, std::shared_ptr<ast::TypeExpr> type_expression) noexcept{return [&]() { if (std::holds_alternative<ast::TyNamed>((*type_expression))) { auto _v_tynamed = std::get<ast::TyNamed>((*type_expression)); auto [name] = _v_tynamed; return name == parameter_name; } if (std::holds_alternative<ast::TyArray>((*type_expression))) { auto _v_tyarray = std::get<ast::TyArray>((*type_expression)); auto [inner] = _v_tyarray; return type_param_in_typeexpr_for_index(parameter_name, inner); } if (std::holds_alternative<ast::TyShared>((*type_expression))) { auto _v_tyshared = std::get<ast::TyShared>((*type_expression)); auto [inner] = _v_tyshared; return type_param_in_typeexpr_for_index(parameter_name, inner); } if (std::holds_alternative<ast::TyGeneric>((*type_expression))) { auto _v_tygeneric = std::get<ast::TyGeneric>((*type_expression)); auto [_w0, type_arguments] = _v_tygeneric; return type_param_in_typeexpr_list_for_index(parameter_name, type_arguments); } if (std::holds_alternative<ast::TyFn>((*type_expression))) { auto _v_tyfn = std::get<ast::TyFn>((*type_expression)); auto [parameter_types, return_type_expression] = _v_tyfn; return type_param_in_typeexpr_list_for_index(parameter_name, parameter_types) || type_param_in_typeexpr_for_index(parameter_name, return_type_expression); } if (std::holds_alternative<ast::TyAssoc>((*type_expression))) { auto _v_tyassoc = std::get<ast::TyAssoc>((*type_expression)); auto [associated_parameter, _w0] = _v_tyassoc; return associated_parameter == parameter_name; } return false; }();}

bool type_param_in_typeexpr_list_for_index(mlc::String parameter_name, mlc::Array<std::shared_ptr<ast::TypeExpr>> types) noexcept{return types.any([parameter_name](std::shared_ptr<ast::TypeExpr> candidate) mutable { return type_param_in_typeexpr_for_index(parameter_name, candidate); });}

mlc::Array<std::shared_ptr<ast::TypeExpr>> variant_field_typeexprs_for_index(std::shared_ptr<ast::TypeVariant> variant) noexcept{return std::visit(overloaded{
  [&](const VarTuple& vartuple) -> mlc::Array<std::shared_ptr<ast::TypeExpr>> { auto [_w0, field_types, _w1] = vartuple; return field_types; },
  [&](const VarRecord& varrecord) -> mlc::Array<std::shared_ptr<ast::TypeExpr>> { auto [_w0, field_definitions, _w1] = varrecord; return field_definitions.map([](std::shared_ptr<ast::FieldDef> field_definition) mutable { return field_definition->type_value; }); },
  [&](const VarUnit& varunit) -> mlc::Array<std::shared_ptr<ast::TypeExpr>> { auto [_w0, _w1] = varunit; return [&]() -> mlc::Array<std::shared_ptr<ast::TypeExpr>> { 
  mlc::Array<std::shared_ptr<ast::TypeExpr>> empty = {};
  return empty;
 }(); }
}, (*variant));}

mlc::Array<mlc::String> variant_used_type_parameters_for_index(mlc::Array<mlc::String> type_parameters, std::shared_ptr<ast::TypeVariant> variant) noexcept{
mlc::Array<std::shared_ptr<ast::TypeExpr>> field_types = variant_field_typeexprs_for_index(variant);
return type_parameters.filter([field_types](mlc::String type_parameter) mutable { return type_param_in_typeexpr_list_for_index(type_parameter, field_types); });
}

void register_sum_type_parameter_names(mlc::HashMap<mlc::String, mlc::Array<mlc::String>> parameter_map, std::shared_ptr<ast::Decl> declaration) noexcept{
mlc::String type_name = type_name_from_type_decl(declaration);
mlc::Array<mlc::String> type_parameters = type_parameters_from_type_decl(declaration);
if (type_name != mlc::String("") && type_parameters.size() > 0){
parameter_map.set(type_name, type_parameters);
}
}

mlc::HashMap<mlc::String, mlc::Array<mlc::String>> build_sum_type_parameter_names_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> declarations) noexcept{
mlc::HashMap<mlc::String, mlc::Array<mlc::String>> parameter_map = mlc::HashMap<mlc::String, mlc::Array<mlc::String>>();
int declaration_index = 0;
while (declaration_index < declarations.size()){
{
register_sum_type_parameter_names(parameter_map, declarations[declaration_index]);
declaration_index = declaration_index + 1;
}
}
return parameter_map;
}

void register_variant_used_type_parameter_names(mlc::HashMap<mlc::String, mlc::Array<mlc::String>> used_map, mlc::String type_name, mlc::Array<mlc::String> type_parameters, std::shared_ptr<ast::TypeVariant> variant) noexcept{
if (type_parameters.size() > 0){
used_map.set(variant_name_from_type_variant(variant), variant_used_type_parameters_for_index(type_parameters, variant));
}
}

void register_variant_used_type_parameter_names_for_decl(mlc::HashMap<mlc::String, mlc::Array<mlc::String>> used_map, std::shared_ptr<ast::Decl> declaration) noexcept{
mlc::String type_name = type_name_from_type_decl(declaration);
mlc::Array<mlc::String> type_parameters = type_parameters_from_type_decl(declaration);
mlc::Array<std::shared_ptr<ast::TypeVariant>> variants = variants_from_type_decl(declaration);
if (type_name != mlc::String("") && type_parameters.size() > 0){
int variant_index = 0;
[&]() { 
  while (variant_index < variants.size()){
{
register_variant_used_type_parameter_names(used_map, type_name, type_parameters, variants[variant_index]);
variant_index = variant_index + 1;
}
}
 }();
}
}

mlc::HashMap<mlc::String, mlc::Array<mlc::String>> build_variant_used_type_parameter_names_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> declarations) noexcept{
mlc::HashMap<mlc::String, mlc::Array<mlc::String>> used_map = mlc::HashMap<mlc::String, mlc::Array<mlc::String>>();
int declaration_index = 0;
while (declaration_index < declarations.size()){
{
register_variant_used_type_parameter_names_for_decl(used_map, declarations[declaration_index]);
declaration_index = declaration_index + 1;
}
}
return used_map;
}

} // namespace type_index
