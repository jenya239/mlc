#include "codegen.hpp"

#include "ast.hpp"

namespace codegen {

using namespace ast;
using namespace ast_tokens;

mlc::String path_to_module_base(mlc::String path) noexcept;

mlc::Array<mlc::String> decl_export_names(std::shared_ptr<ast::Decl> decl) noexcept;

mlc::HashMap<mlc::String, mlc::String> add_exports_to_qualified(mlc::HashMap<mlc::String, mlc::String> qualified, mlc::String imp_path, mlc::Array<codegen::LoadItem> all_items) noexcept;

mlc::HashMap<mlc::String, mlc::String> build_qualified(mlc::Array<mlc::String> import_paths, mlc::Array<codegen::LoadItem> all_items) noexcept;

mlc::String extract_method_name(mlc::String fn_name, mlc::String type_name) noexcept;

mlc::HashMap<mlc::String, mlc::String> add_method_owners_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls, mlc::HashMap<mlc::String, mlc::String> owners) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<codegen::LoadItem>> build_item_index(mlc::Array<codegen::LoadItem> all_items) noexcept;

mlc::HashMap<mlc::String, mlc::String> build_method_owners_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept;

mlc::HashMap<mlc::String, mlc::String> build_variant_types_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept;

codegen::CodegenContext create_codegen_context(ast::Program prog) noexcept;

mlc::String context_resolve(codegen::CodegenContext context, mlc::String name) noexcept;

mlc::Array<std::shared_ptr<codegen::FieldOrder>> build_field_orders(ast::Program prog) noexcept;

bool list_contains(mlc::Array<mlc::String> list, mlc::String item) noexcept;

mlc::Array<mlc::String> lookup_fields(mlc::Array<std::shared_ptr<codegen::FieldOrder>> orders, mlc::String name) noexcept;

bool cpp_keyword(mlc::String name) noexcept;

mlc::String cpp_safe(mlc::String name) noexcept;

mlc::String type_name_to_cpp(codegen::CodegenContext context, mlc::String type_name) noexcept;

mlc::String lower_first(mlc::String name) noexcept;

mlc::String type_to_cpp(codegen::CodegenContext context, std::shared_ptr<ast::TypeExpr> type_expr) noexcept;

mlc::String escape_str(mlc::String input) noexcept;

mlc::String gen_args(mlc::Array<std::shared_ptr<ast::Expr>> exprs, codegen::CodegenContext context) noexcept;

mlc::String map_method(mlc::String method_name) noexcept;

mlc::String map_builtin(mlc::String name) noexcept;

codegen::GenStmtsResult gen_stmts_str_with_try(mlc::Array<std::shared_ptr<ast::Stmt>> stmts, codegen::CodegenContext context, int try_counter) noexcept;

mlc::String gen_stmts_str(mlc::Array<std::shared_ptr<ast::Stmt>> stmts, codegen::CodegenContext context) noexcept;

codegen::CodegenContext stmts_final_ctx(mlc::Array<std::shared_ptr<ast::Stmt>> stmts, codegen::CodegenContext context) noexcept;

mlc::String gen_block_body(std::shared_ptr<ast::Expr> expr, codegen::CodegenContext context) noexcept;

mlc::String gen_expr(std::shared_ptr<ast::Expr> expr, codegen::CodegenContext context) noexcept;

mlc::String gen_record_ordered(mlc::Array<std::shared_ptr<ast::FieldVal>> field_vals, mlc::Array<mlc::String> field_order, codegen::CodegenContext context) noexcept;

mlc::String gen_record_unordered(mlc::Array<std::shared_ptr<ast::FieldVal>> field_vals, codegen::CodegenContext context) noexcept;

mlc::String gen_record_update_ordered(std::shared_ptr<ast::Expr> base, mlc::Array<std::shared_ptr<ast::FieldVal>> overrides, mlc::Array<mlc::String> field_order, codegen::CodegenContext context) noexcept;

std::shared_ptr<ast::Expr> find_field_val(mlc::Array<std::shared_ptr<ast::FieldVal>> field_vals, mlc::String name) noexcept;

bool subject_needs_deref(std::shared_ptr<ast::Expr> subject, codegen::CodegenContext context) noexcept;

bool first_arm_needs_deref(std::shared_ptr<ast::MatchArm> first_arm, std::shared_ptr<ast::Expr> subject, codegen::CodegenContext context) noexcept;

mlc::String gen_match(std::shared_ptr<ast::Expr> subject, mlc::Array<std::shared_ptr<ast::MatchArm>> arms, codegen::CodegenContext context) noexcept;

mlc::String gen_arm(std::shared_ptr<ast::MatchArm> arm, codegen::CodegenContext context) noexcept;

mlc::String pat_bind_names(mlc::Array<std::shared_ptr<ast::Pat>> sub_pats) noexcept;

mlc::String gen_try_unwrap(std::shared_ptr<ast::Expr> inner, codegen::CodegenContext context, mlc::String try_id, mlc::String success_line) noexcept;

codegen::GenStmtResult gen_stmt_with_try(std::shared_ptr<ast::Stmt> stmt, codegen::CodegenContext context, int try_counter) noexcept;

mlc::String gen_stmt(std::shared_ptr<ast::Stmt> stmt, codegen::CodegenContext context) noexcept;

mlc::String gen_stmt_expr(std::shared_ptr<ast::Expr> expr, codegen::CodegenContext context) noexcept;

mlc::String gen_return_body(std::shared_ptr<ast::Expr> expr, codegen::CodegenContext context) noexcept;

mlc::String gen_return_if_stmt(std::shared_ptr<ast::Expr> expr, codegen::CodegenContext context) noexcept;

mlc::String gen_fn_body(std::shared_ptr<ast::Expr> body, codegen::CodegenContext context) noexcept;

mlc::String gen_params(codegen::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;

mlc::String gen_type_decl_fwd(codegen::CodegenContext context, mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::String gen_type_decl_body(codegen::CodegenContext context, mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::String gen_single_variant(codegen::CodegenContext context, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept;

mlc::String gen_adt_fwd(codegen::CodegenContext context, mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::String gen_adt_defs(codegen::CodegenContext context, mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::String variant_ctor_name(std::shared_ptr<ast::TypeVariant> variant) noexcept;

mlc::String gen_variant_struct(codegen::CodegenContext context, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept;

bool is_shared_type(std::shared_ptr<ast::TypeExpr> t) noexcept;

bool is_shared_array_type(std::shared_ptr<ast::TypeExpr> t) noexcept;

bool is_shared_map_type(std::shared_ptr<ast::TypeExpr> t) noexcept;

mlc::String array_elem_name(std::shared_ptr<ast::TypeExpr> t) noexcept;

mlc::Array<mlc::String> collect_shared_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;

mlc::Array<mlc::String> collect_shared_array_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;

mlc::HashMap<mlc::String, mlc::String> collect_array_elem_types(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;

mlc::Array<mlc::String> collect_shared_map_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;

mlc::Array<mlc::String> collect_value_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;

std::shared_ptr<codegen::CtorTypeInfo> ctor_type_info_for(mlc::String ctor_name, mlc::Array<std::shared_ptr<ast::TypeExpr>> types) noexcept;

std::shared_ptr<ast::TypeExpr> field_def_type(std::shared_ptr<ast::FieldDef> fd) noexcept;

mlc::Array<std::shared_ptr<codegen::CtorTypeInfo>> add_ctor_infos_from_variants(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::Array<std::shared_ptr<codegen::CtorTypeInfo>> build_ctor_type_infos_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept;

std::shared_ptr<codegen::CtorTypeInfo> lookup_ctor_type_info(mlc::Array<std::shared_ptr<codegen::CtorTypeInfo>> infos, mlc::String cti_name) noexcept;

codegen::CodegenContext make_body_context(codegen::CodegenContext base, mlc::Array<mlc::String> shared_params, mlc::Array<mlc::String> shared_array_params, mlc::HashMap<mlc::String, mlc::String> array_elem_types, mlc::Array<mlc::String> shared_map_params, mlc::String self_type, mlc::Array<mlc::String> value_params) noexcept;

codegen::CodegenContext context_add_shared(codegen::CodegenContext context, mlc::String name) noexcept;

codegen::CodegenContext context_add_shared_array(codegen::CodegenContext context, mlc::String name) noexcept;

codegen::CodegenContext context_add_value(codegen::CodegenContext context, mlc::String name) noexcept;

codegen::CodegenContext update_context_from_statement(std::shared_ptr<ast::Stmt> stmt, codegen::CodegenContext context) noexcept;

mlc::String template_prefix(mlc::Array<mlc::String> type_params) noexcept;

mlc::String requires_clause(mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds) noexcept;

mlc::String gen_fn_decl(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<ast::TypeExpr> return_type, std::shared_ptr<ast::Expr> body, codegen::CodegenContext context) noexcept;

mlc::String gen_fn_proto(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<ast::TypeExpr> return_type, codegen::CodegenContext context) noexcept;

mlc::String gen_trait_decl(codegen::CodegenContext context, mlc::String trait_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::Decl>> methods) noexcept;

mlc::String gen_type_decl(codegen::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::String gen_decl(std::shared_ptr<ast::Decl> decl, codegen::CodegenContext context) noexcept;

mlc::String gen_extend_wrapper_protos(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<ast::Decl>> methods, codegen::CodegenContext context) noexcept;

mlc::String gen_proto(std::shared_ptr<ast::Decl> decl, codegen::CodegenContext context) noexcept;

mlc::Array<mlc::String> collect_decl_parts(mlc::Array<std::shared_ptr<ast::Decl>> decls, codegen::CodegenContext context, int phase) noexcept;

mlc::String gen_program(ast::Program prog) noexcept;

mlc::String include_lines(mlc::Array<mlc::String> import_paths) noexcept;

codegen::PrecomputedCtx precompute(ast::Program prog, mlc::Array<codegen::LoadItem> all_items) noexcept;

codegen::GenModuleOut gen_module(codegen::LoadItem item, mlc::Array<codegen::LoadItem> all_items, ast::Program full_prog, codegen::PrecomputedCtx precomp) noexcept;

mlc::String path_to_module_base(mlc::String path) noexcept{
int last_slash = -1;
int last_dot = path.length();
int i = 0;
while (i < path.length()){
{
if (path.char_at(i) == mlc::String("/")){
{
last_slash = i;
}
} else {
{
if (path.char_at(i) == mlc::String(".") && i > last_slash){
last_dot = i;
}
}
}
i = i + 1;
}
}
return last_dot > last_slash ? path.substring(last_slash + 1, last_dot - last_slash - 1) : path.substring(last_slash + 1, path.length() - last_slash - 1);
}

mlc::Array<mlc::String> decl_export_names(std::shared_ptr<ast::Decl> decl) noexcept{
mlc::Array<mlc::String> names = {};
[&]() -> std::tuple<> { if (std::holds_alternative<ast::DeclExported>((*decl))) { auto _v_declexported = std::get<ast::DeclExported>((*decl)); auto [d] = _v_declexported; return [&]() -> std::tuple<> { 
  [&]() -> std::tuple<> { if (std::holds_alternative<ast::DeclFn>((*d))) { auto _v_declfn = std::get<ast::DeclFn>((*d)); auto [name, _w0, _w1, _w2, _w3, _w4] = _v_declfn; return [&]() -> std::tuple<> { 
  names.push_back(name);
  return std::make_tuple();
 }(); } if (std::holds_alternative<ast::DeclType>((*d))) { auto _v_decltype = std::get<ast::DeclType>((*d)); auto [type_name, _w0, variants] = _v_decltype; return [&]() -> std::tuple<> { 
  names.push_back(type_name);
  int variant_index = 0;
  while (variant_index < variants.size()){
{
std::visit(overloaded{
  [&](const VarRecord& varrecord) -> std::tuple<> { auto [variant_name, _w0] = varrecord; return [&]() -> std::tuple<> { 
  names.push_back(variant_name);
  return std::make_tuple();
 }(); },
  [&](const VarTuple& vartuple) -> std::tuple<> { auto [variant_name, _w0] = vartuple; return [&]() -> std::tuple<> { 
  names.push_back(variant_name);
  return std::make_tuple();
 }(); },
  [&](const VarUnit& varunit) -> std::tuple<> { auto [variant_name] = varunit; return [&]() -> std::tuple<> { 
  names.push_back(variant_name);
  return std::make_tuple();
 }(); }
}, (*variants[variant_index]));
variant_index = variant_index + 1;
}
}
  return std::make_tuple();
 }(); } if (std::holds_alternative<ast::DeclTrait>((*d))) { auto _v_decltrait = std::get<ast::DeclTrait>((*d)); auto [name, _w0, methods] = _v_decltrait; return [&]() -> std::tuple<> { 
  names.push_back(name);
  int mi = 0;
  while (mi < methods.size()){
{
[&]() -> std::tuple<> { if (std::holds_alternative<ast::DeclFn>((*methods[mi]))) { auto _v_declfn = std::get<ast::DeclFn>((*methods[mi])); auto [fn_name, _w0, _w1, _w2, _w3, _w4] = _v_declfn; return [&]() -> std::tuple<> { 
  names.push_back(fn_name);
  return std::make_tuple();
 }(); } return std::make_tuple(); }();
mi = mi + 1;
}
}
  return std::make_tuple();
 }(); } if (std::holds_alternative<ast::DeclExtend>((*d))) { auto _v_declextend = std::get<ast::DeclExtend>((*d)); auto [_w0, _w1, methods] = _v_declextend; return [&]() -> std::tuple<> { 
  int mi = 0;
  while (mi < methods.size()){
{
[&]() -> std::tuple<> { if (std::holds_alternative<ast::DeclFn>((*methods[mi]))) { auto _v_declfn = std::get<ast::DeclFn>((*methods[mi])); auto [fn_name, _w0, _w1, _w2, _w3, _w4] = _v_declfn; return [&]() -> std::tuple<> { 
  names.push_back(fn_name);
  return std::make_tuple();
 }(); } return std::make_tuple(); }();
mi = mi + 1;
}
}
  return std::make_tuple();
 }(); } return std::make_tuple(); }();
  return std::make_tuple();
 }(); } return std::make_tuple(); }();
return names;
}

mlc::HashMap<mlc::String, mlc::String> add_exports_to_qualified(mlc::HashMap<mlc::String, mlc::String> qualified, mlc::String imp_path, mlc::Array<codegen::LoadItem> all_items) noexcept{
mlc::String prefix = path_to_module_base(imp_path) + mlc::String("::");
int item_i = 0;
while (item_i < all_items.size()){
{
codegen::LoadItem cur_item = all_items[item_i];
if (cur_item.path == imp_path){
{
int decl_index = 0;
[&]() { 
  while (decl_index < cur_item.decls.size()){
{
int name_index = 0;
mlc::Array<mlc::String> exp_names = decl_export_names(cur_item.decls[decl_index]);
while (name_index < exp_names.size()){
{
if (!qualified.has(exp_names[name_index])){
{
qualified.set(exp_names[name_index], prefix);
}
}
name_index = name_index + 1;
}
}
decl_index = decl_index + 1;
}
}
 }();
}
}
item_i = item_i + 1;
}
}
return qualified;
}

mlc::HashMap<mlc::String, mlc::String> build_qualified(mlc::Array<mlc::String> import_paths, mlc::Array<codegen::LoadItem> all_items) noexcept{
mlc::HashMap<mlc::String, mlc::String> qualified = mlc::HashMap<mlc::String, mlc::String>();
int imp_i = 0;
while (imp_i < import_paths.size()){
{
mlc::String imp_path = import_paths[imp_i];
qualified = add_exports_to_qualified(qualified, imp_path, all_items);
int item_i = 0;
while (item_i < all_items.size()){
{
codegen::LoadItem cur_item = all_items[item_i];
if (cur_item.path == imp_path){
{
int trans_i = 0;
[&]() { 
  while (trans_i < cur_item.imports.size()){
{
qualified = add_exports_to_qualified(qualified, cur_item.imports[trans_i], all_items);
trans_i = trans_i + 1;
}
}
 }();
}
}
item_i = item_i + 1;
}
}
imp_i = imp_i + 1;
}
}
return qualified;
}

mlc::String extract_method_name(mlc::String fn_name, mlc::String type_name) noexcept{
mlc::String prefix = type_name + mlc::String("_");
return fn_name.length() > prefix.length() && fn_name.substring(0, prefix.length()) == prefix ? fn_name.substring(prefix.length(), fn_name.length() - prefix.length()) : fn_name;
}

mlc::HashMap<mlc::String, mlc::String> add_method_owners_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls, mlc::HashMap<mlc::String, mlc::String> owners) noexcept{
int i = 0;
while (i < decls.size()){
{
[&]() -> void { if (std::holds_alternative<ast::DeclTrait>((*ast::decl_inner(decls[i])))) { auto _v_decltrait = std::get<ast::DeclTrait>((*ast::decl_inner(decls[i]))); auto [trait_name, _w0, methods] = _v_decltrait; return [&]() { 
  int mi = 0;
  return [&]() { 
  while (mi < methods.size()){
{
[&]() -> void { if (std::holds_alternative<ast::DeclFn>((*methods[mi]))) { auto _v_declfn = std::get<ast::DeclFn>((*methods[mi])); auto [fn_name, _w0, _w1, _w2, _w3, _w4] = _v_declfn; return [&]() { 
  mlc::String unmangled = extract_method_name(fn_name, trait_name);
  if (!owners.has(unmangled)){
owners.set(unmangled, fn_name);
}
 }(); } return; }();
mi = mi + 1;
}
}
 }();
 }(); } if (std::holds_alternative<ast::DeclExtend>((*ast::decl_inner(decls[i])))) { auto _v_declextend = std::get<ast::DeclExtend>((*ast::decl_inner(decls[i]))); auto [type_name, _w0, methods] = _v_declextend; return [&]() { 
  int mi = 0;
  return [&]() { 
  while (mi < methods.size()){
{
[&]() -> void { if (std::holds_alternative<ast::DeclFn>((*methods[mi]))) { auto _v_declfn = std::get<ast::DeclFn>((*methods[mi])); auto [fn_name, _w0, _w1, _w2, _w3, _w4] = _v_declfn; return [&]() { 
  mlc::String unmangled = extract_method_name(fn_name, type_name);
  if (!owners.has(unmangled)){
owners.set(unmangled, fn_name);
}
 }(); } return; }();
mi = mi + 1;
}
}
 }();
 }(); } return; }();
i = i + 1;
}
}
return owners;
}

mlc::HashMap<mlc::String, std::shared_ptr<codegen::LoadItem>> build_item_index(mlc::Array<codegen::LoadItem> all_items) noexcept{
mlc::HashMap<mlc::String, std::shared_ptr<codegen::LoadItem>> item_index = mlc::HashMap<mlc::String, std::shared_ptr<codegen::LoadItem>>();
int i = 0;
while (i < all_items.size()){
{
item_index.set(all_items[i].path, std::make_shared<codegen::LoadItem>(all_items[i]));
i = i + 1;
}
}
return item_index;
}

mlc::HashMap<mlc::String, mlc::String> build_method_owners_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept{return add_method_owners_from_decls(decls, mlc::HashMap<mlc::String, mlc::String>());}

mlc::HashMap<mlc::String, mlc::String> build_variant_types_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept{
mlc::HashMap<mlc::String, mlc::String> variant_map = mlc::HashMap<mlc::String, mlc::String>();
int i = 0;
while (i < decls.size()){
{
std::shared_ptr<ast::Decl> inner_decl = ast::decl_inner(decls[i]);
[&]() -> void { if (std::holds_alternative<ast::DeclType>((*inner_decl))) { auto _v_decltype = std::get<ast::DeclType>((*inner_decl)); auto [type_name, _w0, variants] = _v_decltype; return [&]() { 
  int vi = 0;
  return [&]() { 
  while (vi < variants.size()){
{
std::visit(overloaded{
  [&](const VarRecord& varrecord) -> void { auto [variant_name, _w0] = varrecord; return variant_map.set(variant_name, type_name); },
  [&](const VarTuple& vartuple) -> void { auto [variant_name, _w0] = vartuple; return variant_map.set(variant_name, type_name); },
  [&](const VarUnit& varunit) -> void { auto [variant_name] = varunit; return variant_map.set(variant_name, type_name); }
}, (*variants[vi]));
vi = vi + 1;
}
}
 }();
 }(); } return; }();
i = i + 1;
}
}
return variant_map;
}

codegen::CodegenContext create_codegen_context(ast::Program prog) noexcept{return codegen::CodegenContext{build_field_orders(prog), mlc::String(""), mlc::HashMap<mlc::String, mlc::String>(), mlc::String(""), build_method_owners_from_decls(prog.decls), {}, {}, mlc::HashMap<mlc::String, mlc::String>(), {}, {}, build_variant_types_from_decls(prog.decls), {}};}

mlc::String context_resolve(codegen::CodegenContext context, mlc::String name) noexcept{return context.qualified.has(name) ? context.qualified.get(name) + cpp_safe(name) : context.namespace_prefix.length() > 0 ? context.namespace_prefix + cpp_safe(name) : cpp_safe(name);}

mlc::Array<std::shared_ptr<codegen::FieldOrder>> build_field_orders(ast::Program prog) noexcept{
mlc::Array<std::shared_ptr<codegen::FieldOrder>> orders = {};
int i = 0;
while (i < prog.decls.size()){
{
std::visit(overloaded{
  [&](const DeclExported& declexported) -> void { auto [inner] = declexported; return std::visit(overloaded{
  [&](const DeclType& decltype_) -> void { auto [type_name, _w0, variants] = decltype_; return [&]() { 
  int vi = 0;
  return [&]() { 
  while (vi < variants.size()){
{
std::visit(overloaded{
  [&](const VarRecord& varrecord) -> void { auto [variant_name, field_defs] = varrecord; return [&]() { 
  mlc::Array<mlc::String> field_names = {};
  int fi = 0;
  while (fi < field_defs.size()){
{
field_names.push_back(field_defs[fi]->name);
fi = fi + 1;
}
}
  orders.push_back(std::make_shared<codegen::FieldOrder>(codegen::FieldOrder{variant_name, field_names}));
  if (variants.size() == 1){
{
orders.push_back(std::make_shared<codegen::FieldOrder>(codegen::FieldOrder{type_name, field_names}));
}
}
  return;
 }(); },
  [&](const VarTuple& vartuple) -> void { auto [_w0, _w1] = vartuple; return ; },
  [&](const VarUnit& varunit) -> void { auto [_w0] = varunit; return ; }
}, (*variants[vi]));
vi = vi + 1;
}
}
 }();
 }(); },
  [&](const DeclFn& declfn) -> void { auto [_w0, _w1, _w2, _w3, _w4, _w5] = declfn; return ; },
  [&](const DeclTrait& decltrait) -> void { auto [_w0, _w1, _w2] = decltrait; return ; },
  [&](const DeclExtend& declextend) -> void { auto [_w0, _w1, _w2] = declextend; return ; },
  [&](const DeclImport& declimport) -> void { auto [_w0, _w1] = declimport; return ; },
  [&](const DeclExported& declexported) -> void { auto [_w0] = declexported; return ; }
}, (*ast::decl_inner(inner))); },
  [&](const DeclType& decltype_) -> void { auto [type_name, _w0, variants] = decltype_; return [&]() { 
  int vi = 0;
  return [&]() { 
  while (vi < variants.size()){
{
std::visit(overloaded{
  [&](const VarRecord& varrecord) -> void { auto [variant_name, field_defs] = varrecord; return [&]() { 
  mlc::Array<mlc::String> field_names = {};
  int fi = 0;
  while (fi < field_defs.size()){
{
field_names.push_back(field_defs[fi]->name);
fi = fi + 1;
}
}
  orders.push_back(std::make_shared<codegen::FieldOrder>(codegen::FieldOrder{variant_name, field_names}));
  if (variants.size() == 1){
{
orders.push_back(std::make_shared<codegen::FieldOrder>(codegen::FieldOrder{type_name, field_names}));
}
}
  return;
 }(); },
  [&](const VarTuple& vartuple) -> void { auto [_w0, _w1] = vartuple; return ; },
  [&](const VarUnit& varunit) -> void { auto [_w0] = varunit; return ; }
}, (*variants[vi]));
vi = vi + 1;
}
}
 }();
 }(); },
  [&](const DeclFn& declfn) -> void { auto [_w0, _w1, _w2, _w3, _w4, _w5] = declfn; return ; },
  [&](const DeclTrait& decltrait) -> void { auto [_w0, _w1, _w2] = decltrait; return ; },
  [&](const DeclExtend& declextend) -> void { auto [_w0, _w1, _w2] = declextend; return ; },
  [&](const DeclImport& declimport) -> void { auto [_w0, _w1] = declimport; return ; }
}, (*prog.decls[i]));
i = i + 1;
}
}
return orders;
}

bool list_contains(mlc::Array<mlc::String> list, mlc::String item) noexcept{
bool found = false;
int i = 0;
while (i < list.size() && !found){
{
if (list[i] == item){
{
found = true;
}
}
i = i + 1;
}
}
return found;
}

mlc::Array<mlc::String> lookup_fields(mlc::Array<std::shared_ptr<codegen::FieldOrder>> orders, mlc::String name) noexcept{
mlc::Array<mlc::String> result = {};
int i = 0;
bool found = false;
while (i < orders.size() && !found){
{
if (orders[i]->type_name == name){
{
result = orders[i]->fields;
found = true;
}
}
i = i + 1;
}
}
return result;
}

bool cpp_keyword(mlc::String name) noexcept{return name == mlc::String("export") || name == mlc::String("import") || name == mlc::String("module") || name == mlc::String("struct") || name == mlc::String("class") || name == mlc::String("template") || name == mlc::String("typename") || name == mlc::String("const") || name == mlc::String("return") || name == mlc::String("if") || name == mlc::String("else") || name == mlc::String("for") || name == mlc::String("while") || name == mlc::String("do") || name == mlc::String("switch") || name == mlc::String("case") || name == mlc::String("default") || name == mlc::String("break") || name == mlc::String("continue") || name == mlc::String("try") || name == mlc::String("catch") || name == mlc::String("throw") || name == mlc::String("new") || name == mlc::String("delete") || name == mlc::String("virtual") || name == mlc::String("override") || name == mlc::String("final") || name == mlc::String("public") || name == mlc::String("private") || name == mlc::String("protected") || name == mlc::String("friend") || name == mlc::String("static") || name == mlc::String("extern") || name == mlc::String("inline") || name == mlc::String("using") || name == mlc::String("namespace") || name == mlc::String("typedef") || name == mlc::String("enum") || name == mlc::String("union") || name == mlc::String("true") || name == mlc::String("false") || name == mlc::String("nullptr") || name == mlc::String("void") || name == mlc::String("bool") || name == mlc::String("int") || name == mlc::String("long") || name == mlc::String("short") || name == mlc::String("char") || name == mlc::String("float") || name == mlc::String("double") || name == mlc::String("signed") || name == mlc::String("unsigned") || name == mlc::String("auto") || name == mlc::String("typeid") || name == mlc::String("sizeof") || name == mlc::String("and") || name == mlc::String("or") || name == mlc::String("not") || name == mlc::String("xor") || name == mlc::String("and_eq") || name == mlc::String("or_eq") || name == mlc::String("not_eq") || name == mlc::String("xor_eq") || name == mlc::String("bitand") || name == mlc::String("bitor") || name == mlc::String("compl") || name == mlc::String("alignas") || name == mlc::String("alignof") || name == mlc::String("asm") || name == mlc::String("concept") || name == mlc::String("consteval") || name == mlc::String("constexpr") || name == mlc::String("constinit") || name == mlc::String("const_cast") || name == mlc::String("decltype") || name == mlc::String("dynamic_cast") || name == mlc::String("explicit") || name == mlc::String("mutable") || name == mlc::String("noexcept") || name == mlc::String("operator") || name == mlc::String("reinterpret_cast") || name == mlc::String("requires") || name == mlc::String("static_cast") || name == mlc::String("this") || name == mlc::String("thread_local") || name == mlc::String("volatile") || name == mlc::String("wchar_t");}

mlc::String cpp_safe(mlc::String name) noexcept{return cpp_keyword(name) ? name + mlc::String("_") : name;}

mlc::String type_name_to_cpp(codegen::CodegenContext context, mlc::String type_name) noexcept{return type_name == mlc::String("i32") ? mlc::String("int") : type_name == mlc::String("string") ? mlc::String("mlc::String") : type_name == mlc::String("bool") ? mlc::String("bool") : type_name == mlc::String("unit") ? mlc::String("void") : type_name == mlc::String("Self") || type_name == mlc::String("self") ? context.self_type.length() > 0 ? type_name_to_cpp(context, context.self_type) : mlc::String("void") : context_resolve(context, type_name);}

mlc::String lower_first(mlc::String name) noexcept{return name.length() == 0 ? name : name.char_at(0).to_lower() + name.substring(1, name.length());}

mlc::String type_to_cpp(codegen::CodegenContext context, std::shared_ptr<ast::TypeExpr> type_expr) noexcept{return std::visit(overloaded{
  [&](const TyI32& tyi32) -> mlc::String { return mlc::String("int"); },
  [&](const TyString& tystring) -> mlc::String { return mlc::String("mlc::String"); },
  [&](const TyBool& tybool) -> mlc::String { return mlc::String("bool"); },
  [&](const TyUnit& tyunit) -> mlc::String { return mlc::String("void"); },
  [&](const TyNamed& tynamed) -> mlc::String { auto [name] = tynamed; return type_name_to_cpp(context, name); },
  [&](const TyArray& tyarray) -> mlc::String { auto [inner] = tyarray; return mlc::String("mlc::Array<") + type_to_cpp(context, inner) + mlc::String(">"); },
  [&](const TyShared& tyshared) -> mlc::String { auto [inner] = tyshared; return mlc::String("std::shared_ptr<") + type_to_cpp(context, inner) + mlc::String(">"); },
  [&](const TyGeneric& tygeneric) -> mlc::String { auto [name, targs] = tygeneric; return name == mlc::String("ref") && targs.size() == 1 ? type_to_cpp(context, targs[0]) + mlc::String("&") : [&]() -> mlc::String { 
  mlc::String safe = name == mlc::String("Map") ? mlc::String("mlc::HashMap") : name == mlc::String("Shared") ? mlc::String("std::shared_ptr") : context_resolve(context, name);
  return targs.size() == 0 ? safe : targs.size() == 1 ? safe + mlc::String("<") + type_to_cpp(context, targs[0]) + mlc::String(">") : safe + mlc::String("<") + type_to_cpp(context, targs[0]) + mlc::String(", ") + type_to_cpp(context, targs[1]) + mlc::String(">");
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
  return mlc::String("std::function<") + type_to_cpp(context, ret) + mlc::String("(") + param_types.join(mlc::String(", ")) + mlc::String(")>");
 }(); }
}, (*type_expr));}

mlc::String escape_str(mlc::String input) noexcept{
mlc::Array<mlc::String> parts = {};
int i = 0;
while (i < input.length()){
{
mlc::String ch = input.char_at(i);
if (ch == mlc::String("\\")){
{
parts.push_back(mlc::String("\\\\"));
}
} else {
{
if (ch == mlc::String("\"")){
parts.push_back(mlc::String("\\\""));
} else {
if (ch == mlc::String("\n")){
parts.push_back(mlc::String("\\n"));
} else {
if (ch == mlc::String("\r")){
parts.push_back(mlc::String("\\r"));
} else {
if (ch == mlc::String("\t")){
parts.push_back(mlc::String("\\t"));
} else {
if (ch == mlc::String("\0", 1)){
parts.push_back(mlc::String("\\0"));
} else {
parts.push_back(ch);
}
}
}
}
}
}
}
i = i + 1;
}
}
return parts.join(mlc::String(""));
}

mlc::String gen_args(mlc::Array<std::shared_ptr<ast::Expr>> exprs, codegen::CodegenContext context) noexcept{
mlc::Array<mlc::String> parts = {};
int i = 0;
while (i < exprs.size()){
{
parts.push_back(gen_expr(exprs[i], context));
i = i + 1;
}
}
return parts.join(mlc::String(", "));
}

mlc::String map_method(mlc::String method_name) noexcept{return method_name == mlc::String("length") || method_name == mlc::String("len") ? mlc::String("length") : method_name == mlc::String("push") ? mlc::String("push_back") : method_name == mlc::String("to_string") ? mlc::String("to_string") : method_name == mlc::String("to_int") || method_name == mlc::String("to_i") ? mlc::String("to_i") : method_name == mlc::String("upper") || method_name == mlc::String("to_upper") ? mlc::String("upper") : method_name == mlc::String("lower") || method_name == mlc::String("to_lower") ? mlc::String("lower") : method_name;}

mlc::String map_builtin(mlc::String name) noexcept{return name == mlc::String("print") ? mlc::String("mlc::io::print") : name == mlc::String("println") ? mlc::String("mlc::io::println") : name == mlc::String("exit") ? mlc::String("mlc::io::exit") : name == mlc::String("args") ? mlc::String("mlc::io::args") : name;}

codegen::GenStmtsResult gen_stmts_str_with_try(mlc::Array<std::shared_ptr<ast::Stmt>> stmts, codegen::CodegenContext context, int try_counter) noexcept{
mlc::Array<mlc::String> parts = {};
int i = 0;
codegen::CodegenContext cur_ctx = std::move(context);
int counter = try_counter;
while (i < stmts.size()){
{
codegen::GenStmtResult result = gen_stmt_with_try(stmts[i], cur_ctx, counter);
parts.push_back(result.output);
counter = result.next_try;
cur_ctx = update_context_from_statement(stmts[i], cur_ctx);
i = i + 1;
}
}
return codegen::GenStmtsResult{parts.join(mlc::String("")), counter};
}

mlc::String gen_stmts_str(mlc::Array<std::shared_ptr<ast::Stmt>> stmts, codegen::CodegenContext context) noexcept{
codegen::GenStmtsResult result = gen_stmts_str_with_try(stmts, context, 0);
return result.code;
}

codegen::CodegenContext stmts_final_ctx(mlc::Array<std::shared_ptr<ast::Stmt>> stmts, codegen::CodegenContext context) noexcept{
codegen::CodegenContext cur_ctx = std::move(context);
int i = 0;
while (i < stmts.size()){
{
cur_ctx = update_context_from_statement(stmts[i], cur_ctx);
i = i + 1;
}
}
return cur_ctx;
}

mlc::String gen_block_body(std::shared_ptr<ast::Expr> expr, codegen::CodegenContext context) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprBlock>((*expr)._)) { auto _v_exprblock = std::get<ast::ExprBlock>((*expr)._); auto [stmts, result] = _v_exprblock; return [&]() -> mlc::String { 
  mlc::String stmts_code = gen_stmts_str(stmts, context);
  codegen::CodegenContext final_ctx = stmts_final_ctx(stmts, context);
  return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprIf>((*result)._)) { auto _v_exprif = std::get<ast::ExprIf>((*result)._); auto [_w0, _w1, _w2] = _v_exprif; return stmts_code + gen_stmt_expr(result, final_ctx); } if (std::holds_alternative<ast::ExprUnit>((*result)._)) {  return stmts_code; } return [&]() -> mlc::String { 
  mlc::String result_code = gen_expr(result, final_ctx);
  return result_code == mlc::String("/* unit */") ? stmts_code : stmts_code + result_code + mlc::String(";\n");
 }(); }();
 }(); } if (std::holds_alternative<ast::ExprIf>((*expr)._)) { auto _v_exprif = std::get<ast::ExprIf>((*expr)._); auto [_w0, _w1, _w2] = _v_exprif; return gen_stmt_expr(expr, context); } return gen_expr(expr, context) + mlc::String(";\n"); }();}

mlc::String gen_expr(std::shared_ptr<ast::Expr> expr, codegen::CodegenContext context) noexcept{return std::visit(overloaded{
  [&](const ExprInt& exprint) -> mlc::String { auto [n] = exprint; return mlc::to_string(n); },
  [&](const ExprStr& exprstr) -> mlc::String { auto [s] = exprstr; return mlc::String("mlc::String(\"") + escape_str(s) + mlc::String("\", ") + mlc::to_string(s.length()) + mlc::String(")"); },
  [&](const ExprBool& exprbool) -> mlc::String { auto [b] = exprbool; return b ? mlc::String("true") : mlc::String("false"); },
  [&](const ExprUnit& exprunit) -> mlc::String { return mlc::String("/* unit */"); },
  [&](const ExprExtern& exprextern) -> mlc::String { return mlc::String(""); },
  [&](const ExprIdent& exprident) -> mlc::String { auto [name] = exprident; return context.self_type.length() > 0 ? [&]() -> mlc::String { 
  mlc::Array<mlc::String> self_fields = lookup_fields(context.field_orders, context.self_type);
  bool likely_field = name == mlc::String("errors") || name == mlc::String("kind") || name == mlc::String("tokens") || name == mlc::String("line") || name == mlc::String("col") || name == mlc::String("inferred_type") || name == mlc::String("type_env");
  return list_contains(self_fields, name) || likely_field ? mlc::String("self.") + cpp_safe(name) : [&]() -> mlc::String { 
  mlc::String mapped = map_builtin(name);
  mlc::String safe = context_resolve(context, mapped);
  bool needs_brace = name.length() > 0 && name.char_at(0) >= mlc::String("A") && name.char_at(0) <= mlc::String("Z");
  return needs_brace ? safe + mlc::String("{}") : safe;
 }();
 }() : [&]() -> mlc::String { 
  mlc::String mapped = map_builtin(name);
  mlc::String safe = context_resolve(context, mapped);
  bool needs_brace = name.length() > 0 && name.char_at(0) >= mlc::String("A") && name.char_at(0) <= mlc::String("Z");
  return needs_brace ? safe + mlc::String("{}") : safe;
 }(); },
  [&](const ExprBin& exprbin) -> mlc::String { auto [op, left_expr, right_expr] = exprbin; return mlc::String("(") + gen_expr(left_expr, context) + mlc::String(" ") + op + mlc::String(" ") + gen_expr(right_expr, context) + mlc::String(")"); },
  [&](const ExprUn& exprun) -> mlc::String { auto [op, inner] = exprun; return mlc::String("(") + op + gen_expr(inner, context) + mlc::String(")"); },
  [&](const ExprCall& exprcall) -> mlc::String { auto [func, call_args] = exprcall; return [&]() -> mlc::String { 
  mlc::String fn_code = [&]() -> mlc::String { if (std::holds_alternative<ast::ExprIdent>((*func)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*func)._); auto [name] = _v_exprident; return context_resolve(context, map_builtin(name)); } return gen_expr(func, context); }();
  return fn_code + mlc::String("(") + gen_args(call_args, context) + mlc::String(")");
 }(); },
  [&](const ExprMethod& exprmethod) -> mlc::String { auto [obj, method_name, margs] = exprmethod; return [&]() -> mlc::String { 
  mlc::String obj_code = [&]() -> mlc::String { if (std::holds_alternative<ast::ExprIdent>((*obj)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*obj)._); auto [name] = _v_exprident; return context.self_type.length() > 0 ? [&]() -> mlc::String { 
  mlc::Array<mlc::String> sf = lookup_fields(context.field_orders, context.self_type);
  bool likely_field = name == mlc::String("errors") || name == mlc::String("kind") || name == mlc::String("tokens") || name == mlc::String("line") || name == mlc::String("col") || name == mlc::String("inferred_type") || name == mlc::String("type_env");
  return list_contains(sf, name) || likely_field ? mlc::String("self.") + cpp_safe(name) : context_resolve(context, map_builtin(name));
 }() : context_resolve(context, map_builtin(name)); } return gen_expr(obj, context); }();
  return obj_code == mlc::String("File") ? [&]() -> mlc::String { 
  mlc::String cpp_fn = method_name == mlc::String("read") ? mlc::String("mlc::file::read_to_string") : method_name == mlc::String("write") ? mlc::String("mlc::file::write_string") : mlc::String("mlc::file::") + method_name;
  return cpp_fn + mlc::String("(") + gen_args(margs, context) + mlc::String(")");
 }() : obj_code == mlc::String("Map") && method_name == mlc::String("new") ? mlc::String("{}") : obj_code == mlc::String("Shared") && method_name == mlc::String("new") && margs.size() == 1 ? [&]() -> mlc::String { 
  mlc::String arg_code = gen_expr(margs[0], context);
  mlc::String raw_name = [&]() -> mlc::String { if (std::holds_alternative<ast::ExprRecord>((*margs[0])._)) { auto _v_exprrecord = std::get<ast::ExprRecord>((*margs[0])._); auto [n, _w0] = _v_exprrecord; return n; } if (std::holds_alternative<ast::ExprCall>((*margs[0])._)) { auto _v_exprcall = std::get<ast::ExprCall>((*margs[0])._); auto [fn_expr, _w0] = _v_exprcall; return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprIdent>((*fn_expr)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*fn_expr)._); auto [n] = _v_exprident; return n; } return mlc::String(""); }(); } if (std::holds_alternative<ast::ExprIdent>((*margs[0])._)) { auto _v_exprident = std::get<ast::ExprIdent>((*margs[0])._); auto [n] = _v_exprident; return n; } if (std::holds_alternative<ast::ExprIndex>((*margs[0])._)) { auto _v_exprindex = std::get<ast::ExprIndex>((*margs[0])._); auto [arr, _w0] = _v_exprindex; return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprIdent>((*arr)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*arr)._); auto [n] = _v_exprident; return context.array_elem_types.has(n) ? context.array_elem_types.get(n) : mlc::String(""); } return mlc::String(""); }(); } return mlc::String(""); }();
  mlc::String parent = raw_name.length() > 0 && context.variant_types.has(raw_name) ? context.variant_types.get(raw_name) : raw_name;
  mlc::String type_name = parent.length() > 0 ? context_resolve(context, parent) : mlc::String("auto");
  return mlc::String("std::make_shared<") + type_name + mlc::String(">(") + arg_code + mlc::String(")");
 }() : method_name == mlc::String("to_string") && margs.size() == 0 ? mlc::String("mlc::to_string(") + obj_code + mlc::String(")") : context.method_owners.has(method_name) ? [&]() -> mlc::String { 
  mlc::String mangled = context.method_owners.get(method_name);
  mlc::String fn_name = context_resolve(context, mangled);
  return margs.size() == 0 ? fn_name + mlc::String("(") + obj_code + mlc::String(")") : fn_name + mlc::String("(") + obj_code + mlc::String(", ") + gen_args(margs, context) + mlc::String(")");
 }() : [&]() -> mlc::String { 
  mlc::String base = obj_code + mlc::String(".") + map_method(method_name) + mlc::String("(");
  return margs.size() == 0 ? base + mlc::String(")") : base + gen_args(margs, context) + mlc::String(")");
 }();
 }(); },
  [&](const ExprField& exprfield) -> mlc::String { auto [obj, field_name] = exprfield; return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprIndex>((*obj)._)) { auto _v_exprindex = std::get<ast::ExprIndex>((*obj)._); auto [inner, _w0] = _v_exprindex; return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprIdent>((*inner)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*inner)._); auto [n] = _v_exprident; return context.shared_array_params.contains(n) ? gen_expr(obj, context) + mlc::String("->") + cpp_safe(field_name) : gen_expr(obj, context) + mlc::String(".") + cpp_safe(field_name); } return gen_expr(obj, context) + mlc::String("->") + cpp_safe(field_name); }(); } if (std::holds_alternative<ast::ExprIdent>((*obj)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*obj)._); auto [n] = _v_exprident; return context.shared_params.contains(n) ? gen_expr(obj, context) + mlc::String("->") + cpp_safe(field_name) : gen_expr(obj, context) + mlc::String(".") + cpp_safe(field_name); } if (std::holds_alternative<ast::ExprMethod>((*obj)._)) { auto _v_exprmethod = std::get<ast::ExprMethod>((*obj)._); auto [m_obj, method_name, _w0] = _v_exprmethod; return method_name == mlc::String("get") ? [&]() -> mlc::String { if (std::holds_alternative<ast::ExprIdent>((*m_obj)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*m_obj)._); auto [m] = _v_exprident; return context.shared_map_params.contains(m) ? gen_expr(obj, context) + mlc::String("->") + cpp_safe(field_name) : gen_expr(obj, context) + mlc::String(".") + cpp_safe(field_name); } return gen_expr(obj, context) + mlc::String(".") + cpp_safe(field_name); }() : gen_expr(obj, context) + mlc::String(".") + cpp_safe(field_name); } return gen_expr(obj, context) + mlc::String(".") + cpp_safe(field_name); }(); },
  [&](const ExprIndex& exprindex) -> mlc::String { auto [obj, idx] = exprindex; return gen_expr(obj, context) + mlc::String("[") + gen_expr(idx, context) + mlc::String("]"); },
  [&](const ExprIf& exprif) -> mlc::String { auto [cond, then_expr, else_expr] = exprif; return mlc::String("(") + gen_expr(cond, context) + mlc::String(" ? ") + gen_expr(then_expr, context) + mlc::String(" : ") + gen_expr(else_expr, context) + mlc::String(")"); },
  [&](const ExprBlock& exprblock) -> mlc::String { auto [stmts, result] = exprblock; return stmts.size() == 0 ? gen_expr(result, context) : [&]() -> mlc::String { 
  mlc::String body = gen_stmts_str(stmts, context) + mlc::String("return ") + gen_expr(result, stmts_final_ctx(stmts, context)) + mlc::String(";\n");
  return mlc::String("[&]() {\n") + body + mlc::String("}()");
 }(); },
  [&](const ExprWhile& exprwhile) -> mlc::String { auto [cond, stmts] = exprwhile; return mlc::String("[&]() {\nwhile (") + gen_expr(cond, context) + mlc::String(") {\n") + gen_stmts_str(stmts, context) + mlc::String("}\n}()"); },
  [&](const ExprFor& exprfor) -> mlc::String { auto [var, iter, stmts] = exprfor; return mlc::String("[&]() {\nfor (auto ") + cpp_safe(var) + mlc::String(" : ") + gen_expr(iter, context) + mlc::String(") {\n") + gen_stmts_str(stmts, context) + mlc::String("}\n}()"); },
  [&](const ExprMatch& exprmatch) -> mlc::String { auto [subject, arms] = exprmatch; return gen_match(subject, arms, context); },
  [&](const ExprRecord& exprrecord) -> mlc::String { auto [name, field_vals] = exprrecord; return [&]() -> mlc::String { 
  mlc::Array<mlc::String> field_order = lookup_fields(context.field_orders, name);
  mlc::String vals = field_order.size() > 0 ? gen_record_ordered(field_vals, field_order, context) : gen_record_unordered(field_vals, context);
  return context_resolve(context, name) + mlc::String("{") + vals + mlc::String("}");
 }(); },
  [&](const ExprRecordUpdate& exprrecordupdate) -> mlc::String { auto [name, base, overrides] = exprrecordupdate; return [&]() -> mlc::String { 
  mlc::Array<mlc::String> field_order = lookup_fields(context.field_orders, name);
  mlc::String vals = gen_record_update_ordered(base, overrides, field_order, context);
  return context_resolve(context, name) + mlc::String("{") + vals + mlc::String("}");
 }(); },
  [&](const ExprArray& exprarray) -> mlc::String { auto [exprs] = exprarray; return exprs.size() == 0 ? mlc::String("{}") : mlc::String("mlc::Array{") + gen_args(exprs, context) + mlc::String("}"); },
  [&](const ExprQuestion& exprquestion) -> mlc::String { auto [inner] = exprquestion; return gen_expr(inner, context); },
  [&](const ExprLambda& exprlambda) -> mlc::String { auto [params, body] = exprlambda; return [&]() -> mlc::String { 
  mlc::String capture = params.size() == 0 ? mlc::String("[]") : mlc::String("[=]");
  mlc::String param_list = params.size() == 0 ? mlc::String("") : [&]() -> mlc::String { 
  mlc::Array<mlc::String> parts = {};
  int i = 0;
  while (i < params.size()){
{
parts.push_back(mlc::String("auto ") + cpp_safe(params[i]));
i = i + 1;
}
}
  return parts.join(mlc::String(", "));
 }();
  mlc::String body_code = gen_expr(body, context);
  return capture + mlc::String("(") + param_list + mlc::String(") { return ") + body_code + mlc::String("; }");
 }(); }
}, (*expr)._);}

mlc::String gen_record_ordered(mlc::Array<std::shared_ptr<ast::FieldVal>> field_vals, mlc::Array<mlc::String> field_order, codegen::CodegenContext context) noexcept{
mlc::Array<mlc::String> parts = {};
int i = 0;
while (i < field_order.size()){
{
std::shared_ptr<ast::Expr> value = find_field_val(field_vals, field_order[i]);
parts.push_back(gen_expr(value, context));
i = i + 1;
}
}
return parts.join(mlc::String(", "));
}

mlc::String gen_record_unordered(mlc::Array<std::shared_ptr<ast::FieldVal>> field_vals, codegen::CodegenContext context) noexcept{
mlc::Array<mlc::String> parts = {};
int i = 0;
while (i < field_vals.size()){
{
parts.push_back(gen_expr(field_vals[i]->val, context));
i = i + 1;
}
}
return parts.join(mlc::String(", "));
}

mlc::String gen_record_update_ordered(std::shared_ptr<ast::Expr> base, mlc::Array<std::shared_ptr<ast::FieldVal>> overrides, mlc::Array<mlc::String> field_order, codegen::CodegenContext context) noexcept{
mlc::Array<mlc::String> parts = {};
int i = 0;
while (i < field_order.size()){
{
mlc::String field_name = field_order[i];
std::shared_ptr<ast::Expr> override_val = find_field_val(overrides, field_name);
mlc::String val_str = [&]() -> mlc::String { if (std::holds_alternative<ast::ExprUnit>((*override_val)._)) {  return gen_expr(std::make_shared<ast::Expr>(ast::ExprField(base, field_name)), context); } return gen_expr(override_val, context); }();
parts.push_back(val_str);
i = i + 1;
}
}
return parts.join(mlc::String(", "));
}

std::shared_ptr<ast::Expr> find_field_val(mlc::Array<std::shared_ptr<ast::FieldVal>> field_vals, mlc::String name) noexcept{
std::shared_ptr<ast::Expr> result = std::make_shared<ast::Expr>((ast::ExprUnit{}));
int i = 0;
while (i < field_vals.size()){
{
if (field_vals[i]->name == name){
{
result = field_vals[i]->val;
}
}
i = i + 1;
}
}
return result;
}

bool subject_needs_deref(std::shared_ptr<ast::Expr> subject, codegen::CodegenContext context) noexcept{return [&]() { if (std::holds_alternative<ast::ExprIdent>((*subject)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*subject)._); auto [_w0] = _v_exprident; return true; } if (std::holds_alternative<ast::ExprIndex>((*subject)._)) { auto _v_exprindex = std::get<ast::ExprIndex>((*subject)._); auto [arr, _w0] = _v_exprindex; return [&]() { if (std::holds_alternative<ast::ExprIdent>((*arr)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*arr)._); auto [arr_name] = _v_exprident; return context.shared_array_params.contains(arr_name); } return true; }(); } if (std::holds_alternative<ast::ExprMethod>((*subject)._)) { auto _v_exprmethod = std::get<ast::ExprMethod>((*subject)._); auto [_w0, _w1, _w2] = _v_exprmethod; return false; } return true; }();}

bool first_arm_needs_deref(std::shared_ptr<ast::MatchArm> first_arm, std::shared_ptr<ast::Expr> subject, codegen::CodegenContext context) noexcept{return [&]() { if (std::holds_alternative<ast::PatCtor>((*first_arm->pat))) { auto _v_patctor = std::get<ast::PatCtor>((*first_arm->pat)); auto [name, _w0] = _v_patctor; return [&]() -> bool { 
  mlc::String resolved = context_resolve(context, name);
  return resolved.length() >= 12 && resolved.substring(0, 12) == mlc::String("ast_tokens::") ? false : subject_needs_deref(subject, context);
 }(); } return subject_needs_deref(subject, context); }();}

mlc::String gen_match(std::shared_ptr<ast::Expr> subject, mlc::Array<std::shared_ptr<ast::MatchArm>> arms, codegen::CodegenContext context) noexcept{
mlc::Array<mlc::String> parts = {};
int i = 0;
while (i < arms.size()){
{
parts.push_back(gen_arm(arms[i], context));
i = i + 1;
}
}
mlc::String subject_code = gen_expr(subject, context);
bool needs_deref = arms.size() > 0 ? first_arm_needs_deref(arms[0], subject, context) : true;
mlc::String visit_subject = needs_deref ? mlc::String("(*") + subject_code + mlc::String(")") : subject_code;
return mlc::String("std::visit(overloaded{") + parts.join(mlc::String(",\n")) + mlc::String("\n}, ") + visit_subject + mlc::String(")");
}

mlc::String gen_arm(std::shared_ptr<ast::MatchArm> arm, codegen::CodegenContext context) noexcept{return std::visit(overloaded{
  [&](const PatWild& patwild) -> mlc::String { return mlc::String("[&](const auto& __v) { return ") + gen_expr(arm->body, context) + mlc::String("; }"); },
  [&](const PatUnit& patunit) -> mlc::String { return mlc::String("[&](const auto& __v) { return ") + gen_expr(arm->body, context) + mlc::String("; }"); },
  [&](const PatBool& patbool) -> mlc::String { auto [_w0] = patbool; return mlc::String("[&](const auto& __v) { return ") + gen_expr(arm->body, context) + mlc::String("; }"); },
  [&](const PatInt& patint) -> mlc::String { auto [_w0] = patint; return mlc::String("[&](const auto& __v) { return ") + gen_expr(arm->body, context) + mlc::String("; }"); },
  [&](const PatStr& patstr) -> mlc::String { auto [_w0] = patstr; return mlc::String("[&](const auto& __v) { return ") + gen_expr(arm->body, context) + mlc::String("; }"); },
  [&](const PatIdent& patident) -> mlc::String { auto [name] = patident; return mlc::String("[&](const auto& ") + cpp_safe(name) + mlc::String(") { return ") + gen_expr(arm->body, context) + mlc::String("; }"); },
  [&](const PatCtor& patctor) -> mlc::String { auto [name, sub_pats] = patctor; return [&]() -> mlc::String { 
  mlc::String qualified_name = context_resolve(context, name);
  mlc::String lower_name = cpp_safe(lower_first(name));
  mlc::String binding = sub_pats.size() == 0 ? mlc::String("") : mlc::String("auto [") + pat_bind_names(sub_pats) + mlc::String("] = ") + lower_name + mlc::String("; ");
  std::shared_ptr<codegen::CtorTypeInfo> ctor_type_info = lookup_ctor_type_info(context.ctor_type_infos, name);
  codegen::CodegenContext arm_context = std::move(context);
  int index = 0;
  while (index < sub_pats.size()){
{
[&]() -> void { if (std::holds_alternative<ast::PatIdent>((*sub_pats[index]))) { auto _v_patident = std::get<ast::PatIdent>((*sub_pats[index])); auto [pat_name] = _v_patident; return [&]() { 
  if (ctor_type_info->shared_pos.contains(index)){
{
arm_context = context_add_shared(arm_context, pat_name);
}
}
  if (ctor_type_info->shared_arr_pos.contains(index)){
arm_context = context_add_shared_array(arm_context, pat_name);
}
 }(); } return; }();
index = index + 1;
}
}
  return mlc::String("[&](const ") + qualified_name + mlc::String("& ") + lower_name + mlc::String(") { ") + binding + mlc::String("return ") + gen_expr(arm->body, arm_context) + mlc::String("; }");
 }(); },
  [&](const PatRecord& patrecord) -> mlc::String { auto [name, field_pats] = patrecord; return [&]() -> mlc::String { 
  mlc::String qualified_name = context_resolve(context, name);
  mlc::String lower_name = cpp_safe(lower_first(name));
  mlc::String field_bindings = mlc::String("");
  int fi = 0;
  while (fi < field_pats.size()){
{
std::visit(overloaded{
  [&](const PatIdent& patident) {
auto [field_name] = patident;
{
field_bindings = field_bindings + mlc::String("const auto& ") + cpp_safe(field_name) + mlc::String(" = ") + lower_name + mlc::String(".") + cpp_safe(field_name) + mlc::String("; ");
}
},
  [&](const auto& _unused) {
{
}
}
}, (*field_pats[fi]));
fi = fi + 1;
}
}
  return mlc::String("[&](const ") + qualified_name + mlc::String("& ") + lower_name + mlc::String(") { ") + field_bindings + mlc::String("return ") + gen_expr(arm->body, context) + mlc::String("; }");
 }(); }
}, (*arm->pat));}

mlc::String pat_bind_names(mlc::Array<std::shared_ptr<ast::Pat>> sub_pats) noexcept{
mlc::Array<mlc::String> parts = {};
int i = 0;
while (i < sub_pats.size()){
{
mlc::String bind_name = [&]() -> mlc::String { if (std::holds_alternative<ast::PatIdent>((*sub_pats[i]))) { auto _v_patident = std::get<ast::PatIdent>((*sub_pats[i])); auto [name] = _v_patident; return cpp_safe(name); } return mlc::String("__") + mlc::to_string(i); }();
parts.push_back(bind_name);
i = i + 1;
}
}
return parts.join(mlc::String(", "));
}

mlc::String gen_try_unwrap(std::shared_ptr<ast::Expr> inner, codegen::CodegenContext context, mlc::String try_id, mlc::String success_line) noexcept{
mlc::String inner_code = gen_expr(inner, context);
mlc::String ok_name = context_resolve(context, mlc::String("Ok"));
mlc::String err_name = context_resolve(context, mlc::String("Err"));
return mlc::String("auto ") + try_id + mlc::String(" = ") + inner_code + mlc::String(";\n") + mlc::String("if (std::holds_alternative<") + err_name + mlc::String(">(") + try_id + mlc::String(")) return std::get<") + err_name + mlc::String(">(") + try_id + mlc::String(");\n") + success_line;
}

codegen::GenStmtResult gen_stmt_with_try(std::shared_ptr<ast::Stmt> stmt, codegen::CodegenContext context, int try_counter) noexcept{return std::visit(overloaded{
  [&](const StmtLet& stmtlet) -> codegen::GenStmtResult { auto [name, _w0, typ, value] = stmtlet; return [&]() -> codegen::GenStmtResult { 
  mlc::String try_id = mlc::String("__try_") + mlc::to_string(try_counter);
  int next_counter = try_counter + 1;
  codegen::GenStmtResult result = [&]() -> codegen::GenStmtResult { if (std::holds_alternative<ast::ExprQuestion>((*value)._)) { auto _v_exprquestion = std::get<ast::ExprQuestion>((*value)._); auto [inner] = _v_exprquestion; return codegen::GenStmtResult{gen_try_unwrap(inner, context, try_id, mlc::String("auto ") + cpp_safe(name) + mlc::String(" = std::get<") + context_resolve(context, mlc::String("Ok")) + mlc::String(">(") + try_id + mlc::String(").field0;\n")), next_counter}; } if (std::holds_alternative<ast::ExprArray>((*value)._)) { auto _v_exprarray = std::get<ast::ExprArray>((*value)._); auto [elems] = _v_exprarray; return [&]() -> codegen::GenStmtResult { 
  mlc::String val_code = elems.size() == 0 ? [&]() -> mlc::String { if (std::holds_alternative<ast::TyArray>((*typ))) { auto _v_tyarray = std::get<ast::TyArray>((*typ)); auto [inner] = _v_tyarray; return mlc::String("mlc::Array<") + type_to_cpp(context, inner) + mlc::String(">{}"); } return mlc::String("mlc::Array<mlc::String>{}"); }() : mlc::String("mlc::Array{") + gen_args(elems, context) + mlc::String("}");
  return codegen::GenStmtResult{mlc::String("auto ") + cpp_safe(name) + mlc::String(" = ") + val_code + mlc::String(";\n"), try_counter};
 }(); } if (std::holds_alternative<ast::ExprMethod>((*value)._)) { auto _v_exprmethod = std::get<ast::ExprMethod>((*value)._); auto [map_obj, method_name, _w0] = _v_exprmethod; return [&]() -> codegen::GenStmtResult { 
  mlc::String val_code = method_name == mlc::String("new") ? [&]() -> mlc::String { if (std::holds_alternative<ast::ExprIdent>((*map_obj)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*map_obj)._); auto [on] = _v_exprident; return on == mlc::String("Map") ? [&]() -> mlc::String { if (std::holds_alternative<ast::TyGeneric>((*typ))) { auto _v_tygeneric = std::get<ast::TyGeneric>((*typ)); auto [tn, targs] = _v_tygeneric; return tn == mlc::String("Map") && targs.size() == 2 ? mlc::String("mlc::HashMap<") + type_to_cpp(context, targs[0]) + mlc::String(", ") + type_to_cpp(context, targs[1]) + mlc::String(">()") : gen_expr(value, context); } return gen_expr(value, context); }() : gen_expr(value, context); } return gen_expr(value, context); }() : gen_expr(value, context);
  return codegen::GenStmtResult{mlc::String("auto ") + cpp_safe(name) + mlc::String(" = ") + val_code + mlc::String(";\n"), try_counter};
 }(); } return codegen::GenStmtResult{mlc::String("auto ") + cpp_safe(name) + mlc::String(" = ") + gen_expr(value, context) + mlc::String(";\n"), try_counter}; }();
  return result;
 }(); },
  [&](const StmtExpr& stmtexpr) -> codegen::GenStmtResult { auto [expr] = stmtexpr; return [&]() -> codegen::GenStmtResult { if (std::holds_alternative<ast::ExprQuestion>((*expr)._)) { auto _v_exprquestion = std::get<ast::ExprQuestion>((*expr)._); auto [inner] = _v_exprquestion; return [&]() -> codegen::GenStmtResult { 
  mlc::String try_id = mlc::String("__try_") + mlc::to_string(try_counter);
  mlc::String ok_name = context_resolve(context, mlc::String("Ok"));
  return codegen::GenStmtResult{gen_try_unwrap(inner, context, try_id, mlc::String("/* discard */ (void)std::get<") + ok_name + mlc::String(">(") + try_id + mlc::String(").field0;\n")), try_counter + 1};
 }(); } return codegen::GenStmtResult{gen_stmt_expr(expr, context), try_counter}; }(); },
  [&](const StmtBreak& stmtbreak) -> codegen::GenStmtResult { return codegen::GenStmtResult{mlc::String("break;\n"), try_counter}; },
  [&](const StmtContinue& stmtcontinue) -> codegen::GenStmtResult { return codegen::GenStmtResult{mlc::String("continue;\n"), try_counter}; },
  [&](const StmtReturn& stmtreturn) -> codegen::GenStmtResult { auto [expr] = stmtreturn; return [&]() -> codegen::GenStmtResult { if (std::holds_alternative<ast::ExprQuestion>((*expr)._)) { auto _v_exprquestion = std::get<ast::ExprQuestion>((*expr)._); auto [inner] = _v_exprquestion; return [&]() -> codegen::GenStmtResult { 
  mlc::String try_id = mlc::String("__try_ret_") + mlc::to_string(try_counter);
  mlc::String ok_name = context_resolve(context, mlc::String("Ok"));
  return codegen::GenStmtResult{gen_try_unwrap(inner, context, try_id, mlc::String("return std::get<") + ok_name + mlc::String(">(") + try_id + mlc::String(").field0;\n")), try_counter + 1};
 }(); } return codegen::GenStmtResult{mlc::String("return ") + gen_expr(expr, context) + mlc::String(";\n"), try_counter}; }(); }
}, (*stmt)._);}

mlc::String gen_stmt(std::shared_ptr<ast::Stmt> stmt, codegen::CodegenContext context) noexcept{
codegen::GenStmtResult result = gen_stmt_with_try(stmt, context, 0);
return result.output;
}

mlc::String gen_stmt_expr(std::shared_ptr<ast::Expr> expr, codegen::CodegenContext context) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprBin>((*expr)._)) { auto _v_exprbin = std::get<ast::ExprBin>((*expr)._); auto [op, left_expr, right_expr] = _v_exprbin; return op == mlc::String("=") ? gen_expr(left_expr, context) + mlc::String(" = ") + gen_expr(right_expr, context) + mlc::String(";\n") : mlc::String("(") + gen_expr(left_expr, context) + mlc::String(" ") + op + mlc::String(" ") + gen_expr(right_expr, context) + mlc::String(");\n"); } if (std::holds_alternative<ast::ExprIf>((*expr)._)) { auto _v_exprif = std::get<ast::ExprIf>((*expr)._); auto [cond, then_expr, else_expr] = _v_exprif; return [&]() -> mlc::String { 
  mlc::String output = mlc::String("if (") + gen_expr(cond, context) + mlc::String(") {\n") + gen_block_body(then_expr, context) + mlc::String("}");
  mlc::String else_str = [&]() -> mlc::String { if (std::holds_alternative<ast::ExprUnit>((*else_expr)._)) {  return mlc::String(""); } if (std::holds_alternative<ast::ExprBlock>((*else_expr)._)) { auto _v_exprblock = std::get<ast::ExprBlock>((*else_expr)._); auto [stmts, result] = _v_exprblock; return stmts.size() == 0 ? [&]() -> mlc::String { if (std::holds_alternative<ast::ExprUnit>((*result)._)) {  return mlc::String(""); } return mlc::String(" else {\n") + gen_block_body(else_expr, context) + mlc::String("}"); }() : mlc::String(" else {\n") + gen_block_body(else_expr, context) + mlc::String("}"); } if (std::holds_alternative<ast::ExprIf>((*else_expr)._)) { auto _v_exprif = std::get<ast::ExprIf>((*else_expr)._); auto [_w0, _w1, _w2] = _v_exprif; return mlc::String(" else ") + gen_stmt_expr(else_expr, context); } return mlc::String(" else {\n") + gen_block_body(else_expr, context) + mlc::String("}"); }();
  if (else_str.length() > 0){
{
output = output + else_str;
}
}
  return output + mlc::String("\n");
 }(); } if (std::holds_alternative<ast::ExprWhile>((*expr)._)) { auto _v_exprwhile = std::get<ast::ExprWhile>((*expr)._); auto [cond, stmts] = _v_exprwhile; return mlc::String("while (") + gen_expr(cond, context) + mlc::String(") {\n") + gen_stmts_str(stmts, context) + mlc::String("}\n"); } if (std::holds_alternative<ast::ExprFor>((*expr)._)) { auto _v_exprfor = std::get<ast::ExprFor>((*expr)._); auto [var, iter, stmts] = _v_exprfor; return mlc::String("for (auto ") + cpp_safe(var) + mlc::String(" : ") + gen_expr(iter, context) + mlc::String(") {\n") + gen_stmts_str(stmts, context) + mlc::String("}\n"); } if (std::holds_alternative<ast::ExprBlock>((*expr)._)) { auto _v_exprblock = std::get<ast::ExprBlock>((*expr)._); auto [stmts, result] = _v_exprblock; return [&]() -> mlc::String { 
  mlc::String stmts_code = gen_stmts_str(stmts, context);
  mlc::String result_code = gen_expr(result, stmts_final_ctx(stmts, context));
  return result_code == mlc::String("/* unit */") ? stmts_code : stmts_code + result_code + mlc::String(";\n");
 }(); } return gen_expr(expr, context) + mlc::String(";\n"); }();}

mlc::String gen_return_body(std::shared_ptr<ast::Expr> expr, codegen::CodegenContext context) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprBlock>((*expr)._)) { auto _v_exprblock = std::get<ast::ExprBlock>((*expr)._); auto [stmts, result] = _v_exprblock; return [&]() -> mlc::String { 
  codegen::CodegenContext final_ctx = stmts_final_ctx(stmts, context);
  mlc::String stmts_code = gen_stmts_str(stmts, context);
  return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprQuestion>((*result)._)) { auto _v_exprquestion = std::get<ast::ExprQuestion>((*result)._); auto [inner] = _v_exprquestion; return [&]() -> mlc::String { 
  mlc::String try_id = mlc::String("__try_ret");
  mlc::String ok_name = context_resolve(context, mlc::String("Ok"));
  return stmts_code + gen_try_unwrap(inner, final_ctx, try_id, mlc::String("return std::get<") + ok_name + mlc::String(">(") + try_id + mlc::String(").field0;\n"));
 }(); } if (std::holds_alternative<ast::ExprUnit>((*result)._)) {  return stmts_code; } return stmts_code + mlc::String("return ") + gen_expr(result, final_ctx) + mlc::String(";\n"); }();
 }(); } if (std::holds_alternative<ast::ExprIf>((*expr)._)) { auto _v_exprif = std::get<ast::ExprIf>((*expr)._); auto [cond, then_expr, else_expr] = _v_exprif; return [&]() -> mlc::String { 
  mlc::String output = mlc::String("if (") + gen_expr(cond, context) + mlc::String(") {\n") + gen_return_body(then_expr, context) + mlc::String("}");
  mlc::String else_str = [&]() -> mlc::String { if (std::holds_alternative<ast::ExprUnit>((*else_expr)._)) {  return mlc::String(""); } if (std::holds_alternative<ast::ExprIf>((*else_expr)._)) { auto _v_exprif = std::get<ast::ExprIf>((*else_expr)._); auto [_w0, _w1, _w2] = _v_exprif; return mlc::String(" else ") + gen_return_if_stmt(else_expr, context); } return mlc::String(" else {\n") + gen_return_body(else_expr, context) + mlc::String("}"); }();
  if (else_str.length() > 0){
{
output = output + else_str;
}
}
  return output + mlc::String("\n");
 }(); } if (std::holds_alternative<ast::ExprQuestion>((*expr)._)) { auto _v_exprquestion = std::get<ast::ExprQuestion>((*expr)._); auto [inner] = _v_exprquestion; return [&]() -> mlc::String { 
  mlc::String try_id = mlc::String("__try_ret");
  mlc::String ok_name = context_resolve(context, mlc::String("Ok"));
  return gen_try_unwrap(inner, context, try_id, mlc::String("return std::get<") + ok_name + mlc::String(">(") + try_id + mlc::String(").field0;\n"));
 }(); } return mlc::String("return ") + gen_expr(expr, context) + mlc::String(";\n"); }();}

mlc::String gen_return_if_stmt(std::shared_ptr<ast::Expr> expr, codegen::CodegenContext context) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprIf>((*expr)._)) { auto _v_exprif = std::get<ast::ExprIf>((*expr)._); auto [cond, then_expr, else_expr] = _v_exprif; return [&]() -> mlc::String { 
  mlc::String output = mlc::String("if (") + gen_expr(cond, context) + mlc::String(") {\n") + gen_return_body(then_expr, context) + mlc::String("}");
  mlc::String else_str = [&]() -> mlc::String { if (std::holds_alternative<ast::ExprUnit>((*else_expr)._)) {  return mlc::String(""); } if (std::holds_alternative<ast::ExprIf>((*else_expr)._)) { auto _v_exprif = std::get<ast::ExprIf>((*else_expr)._); auto [_w0, _w1, _w2] = _v_exprif; return mlc::String(" else ") + gen_return_if_stmt(else_expr, context); } return mlc::String(" else {\n") + gen_return_body(else_expr, context) + mlc::String("}"); }();
  if (else_str.length() > 0){
{
output = output + else_str;
}
}
  return output + mlc::String("\n");
 }(); } return mlc::String("if (true) {\n") + gen_return_body(expr, context) + mlc::String("}\n"); }();}

mlc::String gen_fn_body(std::shared_ptr<ast::Expr> body, codegen::CodegenContext context) noexcept{return gen_return_body(body, context);}

mlc::String gen_params(codegen::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> params) noexcept{
mlc::Array<mlc::String> parts = {};
int i = 0;
while (i < params.size()){
{
parts.push_back(type_to_cpp(context, params[i]->typ) + mlc::String(" ") + cpp_safe(params[i]->name));
i = i + 1;
}
}
return parts.join(mlc::String(", "));
}

mlc::String gen_type_decl_fwd(codegen::CodegenContext context, mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{return variants.size() == 1 ? mlc::String("") : gen_adt_fwd(context, type_name, variants);}

mlc::String gen_type_decl_body(codegen::CodegenContext context, mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{return variants.size() == 1 ? gen_single_variant(context, type_name, variants[0]) : gen_adt_defs(context, type_name, variants);}

mlc::String gen_single_variant(codegen::CodegenContext context, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept{return std::visit(overloaded{
  [&](const VarRecord& varrecord) -> mlc::String { auto [_w0, field_defs] = varrecord; return [&]() -> mlc::String { 
  mlc::Array<mlc::String> parts = {};
  int i = 0;
  while (i < field_defs.size()){
{
parts.push_back(type_to_cpp(context, field_defs[i]->typ) + mlc::String(" ") + cpp_safe(field_defs[i]->name) + mlc::String(";"));
i = i + 1;
}
}
  return mlc::String("struct ") + context_resolve(context, type_name) + mlc::String(" {") + parts.join(mlc::String("")) + mlc::String("};\n");
 }(); },
  [&](const VarTuple& vartuple) -> mlc::String { auto [_w0, field_types] = vartuple; return [&]() -> mlc::String { 
  mlc::Array<mlc::String> parts = {};
  int i = 0;
  while (i < field_types.size()){
{
parts.push_back(type_to_cpp(context, field_types[i]) + mlc::String(" field") + mlc::to_string(i) + mlc::String(";"));
i = i + 1;
}
}
  return mlc::String("struct ") + context_resolve(context, type_name) + mlc::String(" {") + parts.join(mlc::String("")) + mlc::String("};\n");
 }(); },
  [&](const VarUnit& varunit) -> mlc::String { auto [_w0] = varunit; return mlc::String("struct ") + context_resolve(context, type_name) + mlc::String(" {};\n"); }
}, (*variant));}

mlc::String gen_adt_fwd(codegen::CodegenContext context, mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
mlc::Array<mlc::String> parts = {};
mlc::Array<mlc::String> variant_names = {};
int index = 0;
while (index < variants.size()){
{
mlc::String variant_name = context_resolve(context, variant_ctor_name(variants[index]));
variant_names.push_back(variant_name);
parts.push_back(mlc::String("struct ") + variant_name + mlc::String(";\n"));
index = index + 1;
}
}
return parts.join(mlc::String("")) + mlc::String("using ") + context_resolve(context, type_name) + mlc::String(" = std::variant<") + variant_names.join(mlc::String(", ")) + mlc::String(">;\n");
}

mlc::String gen_adt_defs(codegen::CodegenContext context, mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
mlc::Array<mlc::String> parts = {};
int i = 0;
while (i < variants.size()){
{
parts.push_back(gen_variant_struct(context, type_name, variants[i]));
i = i + 1;
}
}
return parts.join(mlc::String(""));
}

mlc::String variant_ctor_name(std::shared_ptr<ast::TypeVariant> variant) noexcept{return std::visit(overloaded{
  [&](const VarUnit& varunit) -> mlc::String { auto [name] = varunit; return name; },
  [&](const VarTuple& vartuple) -> mlc::String { auto [name, _w0] = vartuple; return name; },
  [&](const VarRecord& varrecord) -> mlc::String { auto [name, _w0] = varrecord; return name; }
}, (*variant));}

mlc::String gen_variant_struct(codegen::CodegenContext context, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept{return std::visit(overloaded{
  [&](const VarUnit& varunit) -> mlc::String { auto [name] = varunit; return mlc::String("struct ") + context_resolve(context, name) + mlc::String(" {};\n"); },
  [&](const VarTuple& vartuple) -> mlc::String { auto [name, field_types] = vartuple; return [&]() -> mlc::String { 
  mlc::Array<mlc::String> parts = {};
  int i = 0;
  while (i < field_types.size()){
{
parts.push_back(type_to_cpp(context, field_types[i]) + mlc::String(" field") + mlc::to_string(i) + mlc::String(";"));
i = i + 1;
}
}
  return mlc::String("struct ") + context_resolve(context, name) + mlc::String(" {") + parts.join(mlc::String("")) + mlc::String("};\n");
 }(); },
  [&](const VarRecord& varrecord) -> mlc::String { auto [name, field_defs] = varrecord; return [&]() -> mlc::String { 
  mlc::Array<mlc::String> parts = {};
  int i = 0;
  while (i < field_defs.size()){
{
parts.push_back(type_to_cpp(context, field_defs[i]->typ) + mlc::String(" ") + cpp_safe(field_defs[i]->name) + mlc::String(";"));
i = i + 1;
}
}
  return mlc::String("struct ") + context_resolve(context, name) + mlc::String(" {") + parts.join(mlc::String("")) + mlc::String("};\n");
 }(); }
}, (*variant));}

bool is_shared_type(std::shared_ptr<ast::TypeExpr> t) noexcept{return [&]() { if (std::holds_alternative<ast::TyGeneric>((*t))) { auto _v_tygeneric = std::get<ast::TyGeneric>((*t)); auto [n, _w0] = _v_tygeneric; return n == mlc::String("Shared"); } if (std::holds_alternative<ast::TyShared>((*t))) { auto _v_tyshared = std::get<ast::TyShared>((*t)); auto [_w0] = _v_tyshared; return true; } return false; }();}

bool is_shared_array_type(std::shared_ptr<ast::TypeExpr> t) noexcept{return [&]() { if (std::holds_alternative<ast::TyArray>((*t))) { auto _v_tyarray = std::get<ast::TyArray>((*t)); auto [inner] = _v_tyarray; return is_shared_type(inner); } return false; }();}

bool is_shared_map_type(std::shared_ptr<ast::TypeExpr> t) noexcept{return [&]() { if (std::holds_alternative<ast::TyGeneric>((*t))) { auto _v_tygeneric = std::get<ast::TyGeneric>((*t)); auto [n, targs] = _v_tygeneric; return n == mlc::String("Map") && targs.size() >= 2 && is_shared_type(targs[1]); } return false; }();}

mlc::String array_elem_name(std::shared_ptr<ast::TypeExpr> t) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::TyArray>((*t))) { auto _v_tyarray = std::get<ast::TyArray>((*t)); auto [inner] = _v_tyarray; return [&]() -> mlc::String { if (std::holds_alternative<ast::TyNamed>((*inner))) { auto _v_tynamed = std::get<ast::TyNamed>((*inner)); auto [n] = _v_tynamed; return n; } return mlc::String(""); }(); } return mlc::String(""); }();}

mlc::Array<mlc::String> collect_shared_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept{
mlc::Array<mlc::String> result = {};
int i = 0;
while (i < params.size()){
{
if (is_shared_type(params[i]->typ)){
{
result.push_back(params[i]->name);
}
}
i = i + 1;
}
}
return result;
}

mlc::Array<mlc::String> collect_shared_array_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept{
mlc::Array<mlc::String> result = {};
int i = 0;
while (i < params.size()){
{
if (is_shared_array_type(params[i]->typ)){
{
result.push_back(params[i]->name);
}
}
i = i + 1;
}
}
return result;
}

mlc::HashMap<mlc::String, mlc::String> collect_array_elem_types(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept{
mlc::HashMap<mlc::String, mlc::String> result = mlc::HashMap<mlc::String, mlc::String>();
int i = 0;
while (i < params.size()){
{
mlc::String en = array_elem_name(params[i]->typ);
if (en.length() > 0){
{
result.set(params[i]->name, en);
}
}
i = i + 1;
}
}
return result;
}

mlc::Array<mlc::String> collect_shared_map_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept{
mlc::Array<mlc::String> result = {};
int i = 0;
while (i < params.size()){
{
if (is_shared_map_type(params[i]->typ)){
{
result.push_back(params[i]->name);
}
}
i = i + 1;
}
}
return result;
}

mlc::Array<mlc::String> collect_value_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept{
mlc::Array<mlc::String> result = {};
int i = 0;
while (i < params.size()){
{
if (!is_shared_type(params[i]->typ) && !is_shared_array_type(params[i]->typ)){
{
result.push_back(params[i]->name);
}
}
i = i + 1;
}
}
return result;
}

std::shared_ptr<codegen::CtorTypeInfo> ctor_type_info_for(mlc::String ctor_name, mlc::Array<std::shared_ptr<ast::TypeExpr>> types) noexcept{
mlc::Array<int> shared_positions = {};
mlc::Array<int> shared_array_positions = {};
int index = 0;
while (index < types.size()){
{
if (is_shared_type(types[index])){
{
shared_positions.push_back(index);
}
}
if (is_shared_array_type(types[index])){
{
shared_array_positions.push_back(index);
}
}
index = index + 1;
}
}
return std::make_shared<codegen::CtorTypeInfo>(codegen::CtorTypeInfo{ctor_name, shared_positions, shared_array_positions});
}

std::shared_ptr<ast::TypeExpr> field_def_type(std::shared_ptr<ast::FieldDef> fd) noexcept{return fd->typ;}

mlc::Array<std::shared_ptr<codegen::CtorTypeInfo>> add_ctor_infos_from_variants(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
mlc::Array<std::shared_ptr<codegen::CtorTypeInfo>> result = {};
int vi = 0;
while (vi < variants.size()){
{
std::visit(overloaded{
  [&](const VarTuple& vartuple) -> void { auto [variant_name, types] = vartuple; return result.push_back(ctor_type_info_for(variant_name, types)); },
  [&](const VarRecord& varrecord) -> void { auto [variant_name, field_defs] = varrecord; return [&]() { 
  mlc::Array<std::shared_ptr<ast::TypeExpr>> types = {};
  int fi = 0;
  while (fi < field_defs.size()){
{
types.push_back(field_def_type(field_defs[fi]));
fi = fi + 1;
}
}
  return result.push_back(ctor_type_info_for(variant_name, types));
 }(); },
  [&](const VarUnit& varunit) -> void { auto [_w0] = varunit; return ; }
}, (*variants[vi]));
vi = vi + 1;
}
}
return result;
}

mlc::Array<std::shared_ptr<codegen::CtorTypeInfo>> build_ctor_type_infos_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept{
mlc::Array<std::shared_ptr<codegen::CtorTypeInfo>> result = {};
int i = 0;
while (i < decls.size()){
{
[&]() -> void { if (std::holds_alternative<ast::DeclExported>((*decls[i]))) { auto _v_declexported = std::get<ast::DeclExported>((*decls[i])); auto [inner] = _v_declexported; return [&]() -> void { if (std::holds_alternative<ast::DeclType>((*ast::decl_inner(inner)))) { auto _v_decltype = std::get<ast::DeclType>((*ast::decl_inner(inner))); auto [_w0, _w1, variants] = _v_decltype; return [&]() { 
  mlc::Array<std::shared_ptr<codegen::CtorTypeInfo>> infos = add_ctor_infos_from_variants(variants);
  int ii = 0;
  return [&]() { 
  while (ii < infos.size()){
{
result.push_back(infos[ii]);
ii = ii + 1;
}
}
 }();
 }(); } return; }(); } if (std::holds_alternative<ast::DeclType>((*decls[i]))) { auto _v_decltype = std::get<ast::DeclType>((*decls[i])); auto [_w0, _w1, variants] = _v_decltype; return [&]() { 
  mlc::Array<std::shared_ptr<codegen::CtorTypeInfo>> infos = add_ctor_infos_from_variants(variants);
  int ii = 0;
  return [&]() { 
  while (ii < infos.size()){
{
result.push_back(infos[ii]);
ii = ii + 1;
}
}
 }();
 }(); } return; }();
i = i + 1;
}
}
return result;
}

std::shared_ptr<codegen::CtorTypeInfo> lookup_ctor_type_info(mlc::Array<std::shared_ptr<codegen::CtorTypeInfo>> infos, mlc::String cti_name) noexcept{
std::shared_ptr<codegen::CtorTypeInfo> result = std::make_shared<codegen::CtorTypeInfo>(codegen::CtorTypeInfo{mlc::String(""), {}, {}});
int i = 0;
while (i < infos.size()){
{
if (infos[i]->name == cti_name){
{
result = infos[i];
}
}
i = i + 1;
}
}
return result;
}

codegen::CodegenContext make_body_context(codegen::CodegenContext base, mlc::Array<mlc::String> shared_params, mlc::Array<mlc::String> shared_array_params, mlc::HashMap<mlc::String, mlc::String> array_elem_types, mlc::Array<mlc::String> shared_map_params, mlc::String self_type, mlc::Array<mlc::String> value_params) noexcept{return codegen::CodegenContext{base.field_orders, base.namespace_prefix, base.qualified, self_type, base.method_owners, shared_params, shared_array_params, array_elem_types, shared_map_params, base.ctor_type_infos, base.variant_types, value_params};}

codegen::CodegenContext context_add_shared(codegen::CodegenContext context, mlc::String name) noexcept{
mlc::Array<mlc::String> new_shared_params = context.shared_params;
new_shared_params.push_back(name);
return make_body_context(context, new_shared_params, context.shared_array_params, context.array_elem_types, context.shared_map_params, context.self_type, context.value_params);
}

codegen::CodegenContext context_add_shared_array(codegen::CodegenContext context, mlc::String name) noexcept{
mlc::Array<mlc::String> new_shared_array_params = context.shared_array_params;
new_shared_array_params.push_back(name);
return make_body_context(context, context.shared_params, new_shared_array_params, context.array_elem_types, context.shared_map_params, context.self_type, context.value_params);
}

codegen::CodegenContext context_add_value(codegen::CodegenContext context, mlc::String name) noexcept{
mlc::Array<mlc::String> new_value_params = context.value_params;
new_value_params.push_back(name);
return make_body_context(context, context.shared_params, context.shared_array_params, context.array_elem_types, context.shared_map_params, context.self_type, new_value_params);
}

codegen::CodegenContext update_context_from_statement(std::shared_ptr<ast::Stmt> stmt, codegen::CodegenContext context) noexcept{return [&]() -> codegen::CodegenContext { if (std::holds_alternative<ast::StmtLet>((*stmt)._)) { auto _v_stmtlet = std::get<ast::StmtLet>((*stmt)._); auto [binding_name, _w0, typ, value] = _v_stmtlet; return is_shared_type(typ) ? context_add_shared(context, binding_name) : is_shared_array_type(typ) ? context_add_shared_array(context, binding_name) : [&]() -> codegen::CodegenContext { if (std::holds_alternative<ast::ExprMethod>((*value)._)) { auto _v_exprmethod = std::get<ast::ExprMethod>((*value)._); auto [map_obj, method_name, _w0] = _v_exprmethod; return method_name == mlc::String("get") ? [&]() -> codegen::CodegenContext { if (std::holds_alternative<ast::ExprIdent>((*map_obj)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*map_obj)._); auto [map_name] = _v_exprident; return context.shared_map_params.contains(map_name) ? context_add_shared(context, binding_name) : context_add_value(context, binding_name); } return context_add_value(context, binding_name); }() : context_add_value(context, binding_name); } return context_add_value(context, binding_name); }(); } return context; }();}

mlc::String template_prefix(mlc::Array<mlc::String> type_params) noexcept{return type_params.size() > 0 ? mlc::String("template<typename ") + type_params.join(mlc::String(", typename ")) + mlc::String(">\n") : mlc::String("");}

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
parts.push_back(cpp_safe(type_bounds[i][k]) + mlc::String("<") + type_params[i] + mlc::String(">"));
k = k + 1;
}
}
 }();
}
}
i = i + 1;
}
}
return parts.size() > 0 ? mlc::String("requires ") + parts.join(mlc::String(" && ")) + mlc::String("\n") : mlc::String("");
}

mlc::String gen_fn_decl(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<ast::TypeExpr> return_type, std::shared_ptr<ast::Expr> body, codegen::CodegenContext context) noexcept{
mlc::String prefix = template_prefix(type_params) + requires_clause(type_params, type_bounds);
mlc::String safe_name = context_resolve(context, name);
mlc::Array<mlc::String> fn_shared_params = collect_shared_params(params);
mlc::Array<mlc::String> fn_shared_array_params = collect_shared_array_params(params);
mlc::HashMap<mlc::String, mlc::String> fn_array_elem_types = collect_array_elem_types(params);
mlc::Array<mlc::String> fn_shared_map_params = collect_shared_map_params(params);
mlc::Array<mlc::String> fn_value_params = collect_value_params(params);
codegen::CodegenContext body_context = params.size() > 0 && params[0]->name == mlc::String("self") ? [&]() -> codegen::CodegenContext { if (std::holds_alternative<ast::TyNamed>((*params[0]->typ))) { auto _v_tynamed = std::get<ast::TyNamed>((*params[0]->typ)); auto [self_type_name] = _v_tynamed; return [&]() -> codegen::CodegenContext { 
  mlc::String st = self_type_name == mlc::String("Self") || self_type_name == mlc::String("self") ? context.self_type : self_type_name;
  return make_body_context(context, fn_shared_params, fn_shared_array_params, fn_array_elem_types, fn_shared_map_params, st, fn_value_params);
 }(); } return make_body_context(context, fn_shared_params, fn_shared_array_params, fn_array_elem_types, fn_shared_map_params, context.self_type, fn_value_params); }() : make_body_context(context, fn_shared_params, fn_shared_array_params, fn_array_elem_types, fn_shared_map_params, context.self_type, fn_value_params);
codegen::CodegenContext ret_ctx = params.size() > 0 && params[0]->name == mlc::String("self") ? body_context : context;
return name == mlc::String("main") && params.size() == 0 ? [&]() -> mlc::String { 
  mlc::String preamble = mlc::String("mlc::io::set_args(std::vector<mlc::String>(argv + 1, argv + argc));\n");
  return prefix + type_to_cpp(ret_ctx, return_type) + mlc::String(" ") + safe_name + mlc::String("(int argc, char** argv) noexcept{\n") + preamble + gen_fn_body(body, body_context) + mlc::String("}\n");
 }() : prefix + type_to_cpp(ret_ctx, return_type) + mlc::String(" ") + safe_name + mlc::String("(") + gen_params(ret_ctx, params) + mlc::String(") noexcept{\n") + gen_fn_body(body, body_context) + mlc::String("}\n");
}

mlc::String gen_fn_proto(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<ast::TypeExpr> return_type, codegen::CodegenContext context) noexcept{
mlc::String prefix = template_prefix(type_params) + requires_clause(type_params, type_bounds);
mlc::String safe_name = context_resolve(context, name);
codegen::CodegenContext ret_ctx = params.size() > 0 && params[0]->name == mlc::String("self") ? [&]() -> codegen::CodegenContext { if (std::holds_alternative<ast::TyNamed>((*params[0]->typ))) { auto _v_tynamed = std::get<ast::TyNamed>((*params[0]->typ)); auto [tn] = _v_tynamed; return [&]() -> codegen::CodegenContext { 
  mlc::String st = tn == mlc::String("Self") || tn == mlc::String("self") ? context.self_type : tn;
  return make_body_context(context, {}, {}, mlc::HashMap<mlc::String, mlc::String>(), {}, st, {});
 }(); } return context; }() : context;
return name == mlc::String("main") && params.size() == 0 ? prefix + type_to_cpp(ret_ctx, return_type) + mlc::String(" ") + safe_name + mlc::String("(int argc, char** argv) noexcept;\n") : prefix + type_to_cpp(ret_ctx, return_type) + mlc::String(" ") + safe_name + mlc::String("(") + gen_params(ret_ctx, params) + mlc::String(") noexcept;\n");
}

mlc::String gen_trait_decl(codegen::CodegenContext context, mlc::String trait_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::Decl>> methods) noexcept{
mlc::String self_param = type_params.size() > 0 ? type_params[0] : mlc::String("T");
mlc::String tparam = type_params.size() > 0 ? type_params.join(mlc::String(", ")) : mlc::String("T");
mlc::String prefix = mlc::String("template<typename ") + tparam + mlc::String(">\n");
mlc::Array<mlc::String> req_parts = {};
int mi = 0;
while (mi < methods.size()){
{
[&]() -> void { if (std::holds_alternative<ast::DeclFn>((*methods[mi]))) { auto _v_declfn = std::get<ast::DeclFn>((*methods[mi])); auto [mangled, _w0, _w1, params, ret_type, _w2] = _v_declfn; return [&]() { 
  mlc::String method_name = extract_method_name(mangled, trait_name);
  mlc::String ret_cpp = type_to_cpp(context, ret_type);
  return req_parts.push_back(mlc::String("{ ") + method_name + mlc::String("(self) } -> std::convertible_to<") + ret_cpp + mlc::String(">"));
 }(); } return; }();
mi = mi + 1;
}
}
mlc::String req_body = req_parts.join(mlc::String("; "));
return prefix + mlc::String("concept ") + cpp_safe(trait_name) + mlc::String(" = requires(const ") + self_param + mlc::String("& self) { ") + req_body + mlc::String("; };\n");
}

mlc::String gen_type_decl(codegen::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
mlc::String prefix = template_prefix(type_params);
return prefix + gen_type_decl_fwd(context, type_name, variants) + gen_type_decl_body(context, type_name, variants);
}

mlc::String gen_decl(std::shared_ptr<ast::Decl> decl, codegen::CodegenContext context) noexcept{return std::visit(overloaded{
  [&](const DeclType& decltype_) -> mlc::String { auto [type_name, type_params, variants] = decltype_; return gen_type_decl(context, type_name, type_params, variants); },
  [&](const DeclTrait& decltrait) -> mlc::String { auto [name, type_params, methods] = decltrait; return gen_trait_decl(context, name, type_params, methods); },
  [&](const DeclFn& declfn) -> mlc::String { auto [name, type_params, type_bounds, params, return_type, body] = declfn; return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprExtern>((*body)._)) {  return mlc::String(""); } return gen_fn_decl(name, type_params, type_bounds, params, return_type, body, context); }(); },
  [&](const DeclExtend& declextend) -> mlc::String { auto [type_name, trait_name, methods] = declextend; return [&]() -> mlc::String { 
  codegen::CodegenContext ext_ctx = make_body_context(context, {}, {}, mlc::HashMap<mlc::String, mlc::String>(), {}, type_name, {});
  mlc::Array<mlc::String> parts = {};
  int index = 0;
  while (index < methods.size()){
{
[&]() -> void { if (std::holds_alternative<ast::DeclFn>((*methods[index]))) { auto _v_declfn = std::get<ast::DeclFn>((*methods[index])); auto [mangled, _w0, _w1, params, ret_type, body] = _v_declfn; return [&]() -> void { if (std::holds_alternative<ast::ExprExtern>((*body)._)) {  return [&]() { 
  mlc::String method_name = trait_name.length() > 0 ? extract_method_name(mangled, type_name) : mlc::String("");
  if (type_name == mlc::String("i32") && method_name == mlc::String("to_string") && params.size() > 0){
mlc::String ret_cpp = type_to_cpp(context, ret_type);
mlc::String params_str = gen_params(context, params);
parts.push_back(ret_cpp + mlc::String(" ") + context_resolve(context, mangled) + mlc::String("(") + params_str + mlc::String(") noexcept { return mlc::to_string(") + cpp_safe(params[0]->name) + mlc::String("); }\n"));
} else {
std::make_tuple();
}
 }(); } return parts.push_back(gen_decl(methods[index], ext_ctx)); }(); } return parts.push_back(gen_decl(methods[index], ext_ctx)); }();
index = index + 1;
}
}
  mlc::String wrappers = mlc::String("");
  if (trait_name.length() > 0){
{
int mi = 0;
[&]() { 
  while (mi < methods.size()){
{
std::visit(overloaded{
  [&](const DeclFn& declfn) {
auto [mangled, _w0, _w1, params, ret_type, _w2] = declfn;
{
codegen::CodegenContext wrapper_ctx = make_body_context(context, {}, {}, mlc::HashMap<mlc::String, mlc::String>(), {}, type_name, {});
mlc::String method_name = extract_method_name(mangled, type_name);
mlc::String ret_cpp = type_to_cpp(wrapper_ctx, ret_type);
mlc::String fn_resolved = context_resolve(context, mangled);
mlc::String params_str = gen_params(wrapper_ctx, params);
mlc::Array<mlc::String> call_args = {};
int pi = 0;
while (pi < params.size()){
{
call_args.push_back(cpp_safe(params[pi]->name));
pi = pi + 1;
}
}
wrappers = wrappers + mlc::String("inline ") + ret_cpp + mlc::String(" ") + cpp_safe(method_name) + mlc::String("(") + params_str + mlc::String(") noexcept { return ") + fn_resolved + mlc::String("(") + call_args.join(mlc::String(", ")) + mlc::String("); }\n");
}
},
  [&](const auto& _unused) {
{
}
}
}, (*methods[mi]));
mi = mi + 1;
}
}
 }();
}
}
  mlc::String methods_str = parts.join(mlc::String(""));
  return trait_name.length() > 0 ? [&]() -> mlc::String { 
  mlc::String cpp_type = type_name_to_cpp(context, type_name);
  mlc::String trait_safe = cpp_safe(trait_name);
  return methods_str + wrappers + mlc::String("static_assert(") + trait_safe + mlc::String("<") + cpp_type + mlc::String(">, \"") + type_name + mlc::String(" does not implement ") + trait_name + mlc::String("\");\n");
 }() : methods_str;
 }(); },
  [&](const DeclImport& declimport) -> mlc::String { auto [_w0, _w1] = declimport; return mlc::String(""); },
  [&](const DeclExported& declexported) -> mlc::String { auto [d] = declexported; return gen_decl(d, context); }
}, (*decl));}

mlc::String gen_extend_wrapper_protos(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<ast::Decl>> methods, codegen::CodegenContext context) noexcept{return trait_name.length() == 0 ? mlc::String("") : [&]() -> mlc::String { 
  mlc::String wrappers = mlc::String("");
  int mi = 0;
  while (mi < methods.size()){
{
std::visit(overloaded{
  [&](const DeclFn& declfn) {
auto [mangled, _w0, _w1, params, ret_type, _w2] = declfn;
{
codegen::CodegenContext wrapper_ctx = make_body_context(context, {}, {}, mlc::HashMap<mlc::String, mlc::String>(), {}, type_name, {});
mlc::String method_name = extract_method_name(mangled, type_name);
mlc::String ret_cpp = type_to_cpp(wrapper_ctx, ret_type);
mlc::String params_str = gen_params(wrapper_ctx, params);
wrappers = wrappers + ret_cpp + mlc::String(" ") + cpp_safe(method_name) + mlc::String("(") + params_str + mlc::String(") noexcept;\n");
}
},
  [&](const auto& _unused) {
{
}
}
}, (*methods[mi]));
mi = mi + 1;
}
}
  return wrappers;
 }();}

mlc::String gen_proto(std::shared_ptr<ast::Decl> decl, codegen::CodegenContext context) noexcept{return std::visit(overloaded{
  [&](const DeclType& decltype_) -> mlc::String { auto [_w0, _w1, _w2] = decltype_; return mlc::String(""); },
  [&](const DeclTrait& decltrait) -> mlc::String { auto [_w0, _w1, _w2] = decltrait; return mlc::String(""); },
  [&](const DeclFn& declfn) -> mlc::String { auto [name, type_params, type_bounds, params, ret, body] = declfn; return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprExtern>((*body)._)) {  return mlc::String(""); } return gen_fn_proto(name, type_params, type_bounds, params, ret, context); }(); },
  [&](const DeclExtend& declextend) -> mlc::String { auto [type_name, trait_name, methods] = declextend; return [&]() -> mlc::String { 
  codegen::CodegenContext ext_ctx = make_body_context(context, {}, {}, mlc::HashMap<mlc::String, mlc::String>(), {}, type_name, {});
  mlc::Array<mlc::String> parts = {};
  int index = 0;
  while (index < methods.size()){
{
parts.push_back(gen_proto(methods[index], ext_ctx));
index = index + 1;
}
}
  return parts.join(mlc::String(""));
 }(); },
  [&](const DeclImport& declimport) -> mlc::String { auto [_w0, _w1] = declimport; return mlc::String(""); },
  [&](const DeclExported& declexported) -> mlc::String { auto [d] = declexported; return gen_proto(d, context); }
}, (*decl));}

mlc::Array<mlc::String> collect_decl_parts(mlc::Array<std::shared_ptr<ast::Decl>> decls, codegen::CodegenContext context, int phase) noexcept{
mlc::Array<mlc::String> parts = {};
if (phase == 0){
{
int i = 0;
[&]() { 
  while (i < decls.size()){
{
mlc::String w = [&]() -> mlc::String { if (std::holds_alternative<ast::DeclExtend>((*decls[i]))) { auto _v_declextend = std::get<ast::DeclExtend>((*decls[i])); auto [tn, tr, m] = _v_declextend; return gen_extend_wrapper_protos(tn, tr, m, context); } if (std::holds_alternative<ast::DeclExported>((*decls[i]))) { auto _v_declexported = std::get<ast::DeclExported>((*decls[i])); auto [d] = _v_declexported; return [&]() -> mlc::String { if (std::holds_alternative<ast::DeclExtend>((*ast::decl_inner(d)))) { auto _v_declextend = std::get<ast::DeclExtend>((*ast::decl_inner(d))); auto [tn, tr, m] = _v_declextend; return gen_extend_wrapper_protos(tn, tr, m, context); } return mlc::String(""); }(); } return mlc::String(""); }();
if (w.length() > 0){
{
parts.push_back(w);
}
}
i = i + 1;
}
}
 }();
}
}
int index = 0;
while (index < decls.size()){
{
mlc::String segment = std::visit(overloaded{
  [&](const DeclType& decltype_) -> mlc::String { auto [type_name, _w0, variants] = decltype_; return phase == 0 ? gen_type_decl_fwd(context, type_name, variants) : phase == 1 ? gen_type_decl_body(context, type_name, variants) : mlc::String(""); },
  [&](const DeclTrait& decltrait) -> mlc::String { auto [name, type_params, methods] = decltrait; return phase == 0 ? gen_trait_decl(context, name, type_params, methods) : mlc::String(""); },
  [&](const DeclFn& declfn) -> mlc::String { auto [_w0, _w1, _w2, _w3, _w4, _w5] = declfn; return phase == 2 ? gen_proto(decls[index], context) : phase == 3 ? gen_decl(decls[index], context) : mlc::String(""); },
  [&](const DeclExtend& declextend) -> mlc::String { auto [_w0, _w1, _w2] = declextend; return phase == 2 ? gen_proto(decls[index], context) : phase == 3 ? gen_decl(decls[index], context) : mlc::String(""); },
  [&](const DeclImport& declimport) -> mlc::String { auto [_w0, _w1] = declimport; return mlc::String(""); },
  [&](const DeclExported& declexported) -> mlc::String { auto [d] = declexported; return phase == 0 ? [&]() -> mlc::String { if (std::holds_alternative<ast::DeclType>((*ast::decl_inner(d)))) { auto _v_decltype = std::get<ast::DeclType>((*ast::decl_inner(d))); auto [type_name, _w0, variants] = _v_decltype; return gen_type_decl_fwd(context, type_name, variants); } if (std::holds_alternative<ast::DeclTrait>((*ast::decl_inner(d)))) { auto _v_decltrait = std::get<ast::DeclTrait>((*ast::decl_inner(d))); auto [name, type_params, methods] = _v_decltrait; return gen_trait_decl(context, name, type_params, methods); } return mlc::String(""); }() : phase == 1 ? [&]() -> mlc::String { if (std::holds_alternative<ast::DeclType>((*ast::decl_inner(d)))) { auto _v_decltype = std::get<ast::DeclType>((*ast::decl_inner(d))); auto [type_name, _w0, variants] = _v_decltype; return gen_type_decl_body(context, type_name, variants); } return mlc::String(""); }() : phase == 2 ? gen_proto(d, context) : phase == 3 ? [&]() -> mlc::String { if (std::holds_alternative<ast::DeclType>((*ast::decl_inner(d)))) { auto _v_decltype = std::get<ast::DeclType>((*ast::decl_inner(d))); auto [_w0, _w1, _w2] = _v_decltype; return mlc::String(""); } if (std::holds_alternative<ast::DeclTrait>((*ast::decl_inner(d)))) { auto _v_decltrait = std::get<ast::DeclTrait>((*ast::decl_inner(d))); auto [_w0, _w1, _w2] = _v_decltrait; return mlc::String(""); } return gen_decl(d, context); }() : mlc::String(""); }
}, (*decls[index]));
if (segment.length() > 0){
{
parts.push_back(segment);
}
}
index = index + 1;
}
}
return parts;
}

mlc::String gen_program(ast::Program prog) noexcept{
codegen::CodegenContext context = create_codegen_context(prog);
mlc::Array<mlc::String> preamble = mlc::Array<mlc::String>{mlc::String("#include \"mlc/core/string.hpp\"\n"), mlc::String("#include \"mlc/core/array.hpp\"\n"), mlc::String("#include \"mlc/core/hashmap.hpp\"\n"), mlc::String("#include \"mlc/core/match.hpp\"\n"), mlc::String("#include \"mlc/io/io.hpp\"\n"), mlc::String("#include \"mlc/io/file.hpp\"\n"), mlc::String("#include <functional>\n"), mlc::String("#include <memory>\n"), mlc::String("#include <variant>\n"), mlc::String("#include <vector>\n\n")};
mlc::Array<mlc::String> type_fwds = collect_decl_parts(prog.decls, context, 0);
mlc::Array<mlc::String> type_defs = collect_decl_parts(prog.decls, context, 1);
mlc::Array<mlc::String> fn_protos = collect_decl_parts(prog.decls, context, 2);
mlc::Array<mlc::String> fn_decls = collect_decl_parts(prog.decls, context, 3);
return ast::errs_append(ast::errs_append(ast::errs_append(ast::errs_append(preamble, type_fwds), type_defs), ast::errs_append(fn_protos, mlc::Array<mlc::String>{mlc::String("\n")})), fn_decls).join(mlc::String(""));
}

mlc::String include_lines(mlc::Array<mlc::String> import_paths) noexcept{
mlc::Array<mlc::String> lines = {};
int i = 0;
while (i < import_paths.size()){
{
lines.push_back(mlc::String("#include \"") + path_to_module_base(import_paths[i]) + mlc::String(".hpp\"\n"));
i = i + 1;
}
}
return lines.join(mlc::String(""));
}

codegen::PrecomputedCtx precompute(ast::Program prog, mlc::Array<codegen::LoadItem> all_items) noexcept{return codegen::PrecomputedCtx{build_field_orders(prog), build_variant_types_from_decls(prog.decls), build_item_index(all_items), build_ctor_type_infos_from_decls(prog.decls)};}

codegen::GenModuleOut gen_module(codegen::LoadItem item, mlc::Array<codegen::LoadItem> all_items, ast::Program full_prog, codegen::PrecomputedCtx precomp) noexcept{
mlc::String base = path_to_module_base(item.path);
mlc::HashMap<mlc::String, mlc::String> qualified = build_qualified(item.imports, all_items);
codegen::CodegenContext context = codegen::CodegenContext{precomp.field_orders, mlc::String(""), qualified, mlc::String(""), build_method_owners_from_decls(full_prog.decls), {}, {}, mlc::HashMap<mlc::String, mlc::String>(), {}, precomp.ctor_type_infos, precomp.variant_types, {}};
mlc::String module_namespace = base == mlc::String("main") ? mlc::String("mlc_main") : base;
mlc::String std_includes = mlc::String("#include \"mlc.hpp\"\n#include <variant>\n\n") + include_lines(item.imports) + mlc::String("\n");
mlc::Array<mlc::String> type_fwds = collect_decl_parts(item.decls, context, 0);
mlc::Array<mlc::String> type_defs = collect_decl_parts(item.decls, context, 1);
mlc::Array<mlc::String> fn_protos = collect_decl_parts(item.decls, context, 2);
mlc::Array<mlc::String> fn_defs = collect_decl_parts(item.decls, context, 3);
mlc::String guard = base.upper() + mlc::String("_HPP");
mlc::String header = mlc::Array<mlc::String>{mlc::String("#ifndef ") + guard + mlc::String("\n"), mlc::String("#define ") + guard + mlc::String("\n"), mlc::String("\n"), std_includes, mlc::String("namespace ") + module_namespace + mlc::String(" {\n"), mlc::String("\n"), type_fwds.join(mlc::String("")), type_defs.join(mlc::String("")), fn_protos.join(mlc::String("")), mlc::String("\n"), mlc::String("} // namespace ") + module_namespace + mlc::String("\n"), mlc::String("\n"), mlc::String("#endif // ") + guard + mlc::String("\n")}.join(mlc::String(""));
mlc::String cli_wrapper = base == mlc::String("main") ? mlc::String("\n#undef main\n\nstatic void mlc_cli_set_args(int argc, char** argv) {\n  std::vector<mlc::String> arguments;\n  arguments.reserve(argc > 0 ? argc - 1 : 0);\n  for (int i = 1; i < argc; ++i) { arguments.emplace_back(argv[i]); }\n  mlc::io::set_args(std::move(arguments));\n}\n\nint main(int argc, char** argv) {\n  mlc_cli_set_args(argc, argv);\n  return ::mlc_main::mlc_user_main(argc, argv);\n}\n") : mlc::String("");
mlc::String impl = mlc::Array<mlc::String>{mlc::String("#define main mlc_user_main\n"), mlc::String("#include \"") + base + mlc::String(".hpp\"\n"), mlc::String("\n"), mlc::String("namespace ") + module_namespace + mlc::String(" {\n"), mlc::String("\n"), fn_defs.join(mlc::String("")), mlc::String("\n"), mlc::String("} // namespace ") + module_namespace + mlc::String("\n"), cli_wrapper}.join(mlc::String(""));
return codegen::GenModuleOut{header, impl};
}

} // namespace codegen
