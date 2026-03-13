#include "codegen.hpp"

#include "ast.hpp"

namespace codegen {

using namespace ast;
using namespace ast_tokens;

mlc::String path_to_module_base(mlc::String path) noexcept;

mlc::Array<mlc::String> decl_export_names(std::shared_ptr<ast::Decl> decl) noexcept;

mlc::HashMap<mlc::String, mlc::String> add_exports_to_qualified(mlc::HashMap<mlc::String, mlc::String> q, mlc::String imp_path, mlc::Array<codegen::LoadItem> all_items) noexcept;

mlc::HashMap<mlc::String, mlc::String> build_qualified(mlc::Array<mlc::String> import_paths, mlc::Array<codegen::LoadItem> all_items) noexcept;

mlc::String extract_method_name(mlc::String fn_name, mlc::String type_name) noexcept;

mlc::HashMap<mlc::String, mlc::String> add_method_owners_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls, mlc::HashMap<mlc::String, mlc::String> m) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<codegen::LoadItem>> build_item_index(mlc::Array<codegen::LoadItem> all_items) noexcept;

mlc::HashMap<mlc::String, mlc::String> build_method_owners_with_index(codegen::LoadItem item, mlc::HashMap<mlc::String, std::shared_ptr<codegen::LoadItem>> idx) noexcept;

mlc::HashMap<mlc::String, mlc::String> build_method_owners_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept;

mlc::HashMap<mlc::String, mlc::String> build_variant_types_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept;

codegen::CodegenContext create_codegen_context(ast::Program prog) noexcept;

mlc::String ctx_resolve(codegen::CodegenContext ctx, mlc::String name) noexcept;

mlc::Array<std::shared_ptr<codegen::FieldOrder>> build_field_orders(ast::Program prog) noexcept;

bool list_contains(mlc::Array<mlc::String> list, mlc::String item) noexcept;

mlc::Array<mlc::String> lookup_fields(mlc::Array<std::shared_ptr<codegen::FieldOrder>> orders, mlc::String name) noexcept;

bool cpp_keyword(mlc::String name) noexcept;

mlc::String cpp_safe(mlc::String name) noexcept;

mlc::String lower_first(mlc::String name) noexcept;

mlc::String type_to_cpp(codegen::CodegenContext ctx, std::shared_ptr<ast::TypeExpr> type_expr) noexcept;

mlc::String escape_str(mlc::String input) noexcept;

mlc::String gen_args(mlc::Array<std::shared_ptr<ast::Expr>> exprs, codegen::CodegenContext ctx) noexcept;

mlc::String map_method(mlc::String method_name) noexcept;

mlc::String map_builtin(mlc::String name) noexcept;

mlc::String gen_stmts_str(mlc::Array<std::shared_ptr<ast::Stmt>> stmts, codegen::CodegenContext ctx) noexcept;

codegen::CodegenContext stmts_final_ctx(mlc::Array<std::shared_ptr<ast::Stmt>> stmts, codegen::CodegenContext ctx) noexcept;

mlc::String gen_block_body(std::shared_ptr<ast::Expr> expr, codegen::CodegenContext ctx) noexcept;

mlc::String gen_expr(std::shared_ptr<ast::Expr> expr, codegen::CodegenContext ctx) noexcept;

mlc::String gen_record_ordered(mlc::Array<std::shared_ptr<ast::FieldVal>> field_vals, mlc::Array<mlc::String> field_order, codegen::CodegenContext ctx) noexcept;

mlc::String gen_record_unordered(mlc::Array<std::shared_ptr<ast::FieldVal>> field_vals, codegen::CodegenContext ctx) noexcept;

mlc::String gen_record_update_ordered(std::shared_ptr<ast::Expr> base, mlc::Array<std::shared_ptr<ast::FieldVal>> overrides, mlc::Array<mlc::String> field_order, codegen::CodegenContext ctx) noexcept;

std::shared_ptr<ast::Expr> find_field_val(mlc::Array<std::shared_ptr<ast::FieldVal>> field_vals, mlc::String name) noexcept;

bool first_arm_needs_deref(std::shared_ptr<ast::MatchArm> first_arm, std::shared_ptr<ast::Expr> subject, codegen::CodegenContext ctx) noexcept;

mlc::String gen_match(std::shared_ptr<ast::Expr> subject, mlc::Array<std::shared_ptr<ast::MatchArm>> arms, codegen::CodegenContext ctx) noexcept;

mlc::String gen_arm(std::shared_ptr<ast::MatchArm> arm, codegen::CodegenContext ctx) noexcept;

mlc::String pat_bind_names(mlc::Array<std::shared_ptr<ast::Pat>> sub_pats) noexcept;

mlc::String gen_stmt(std::shared_ptr<ast::Stmt> stmt, codegen::CodegenContext ctx) noexcept;

mlc::String gen_stmt_expr(std::shared_ptr<ast::Expr> expr, codegen::CodegenContext ctx) noexcept;

mlc::String gen_return_body(std::shared_ptr<ast::Expr> expr, codegen::CodegenContext ctx) noexcept;

mlc::String gen_return_if_stmt(std::shared_ptr<ast::Expr> expr, codegen::CodegenContext ctx) noexcept;

mlc::String gen_fn_body(std::shared_ptr<ast::Expr> body, codegen::CodegenContext ctx) noexcept;

mlc::String gen_params(codegen::CodegenContext ctx, mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;

mlc::String gen_type_decl_fwd(codegen::CodegenContext ctx, mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::String gen_type_decl_body(codegen::CodegenContext ctx, mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::String gen_single_variant(codegen::CodegenContext ctx, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept;

mlc::String gen_adt_fwd(codegen::CodegenContext ctx, mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::String gen_adt_defs(codegen::CodegenContext ctx, mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::String variant_ctor_name(std::shared_ptr<ast::TypeVariant> variant) noexcept;

mlc::String gen_variant_struct(codegen::CodegenContext ctx, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept;

bool is_shared_type(std::shared_ptr<ast::TypeExpr> t) noexcept;

bool is_shared_array_type(std::shared_ptr<ast::TypeExpr> t) noexcept;

bool is_shared_map_type(std::shared_ptr<ast::TypeExpr> t) noexcept;

mlc::String array_elem_name(std::shared_ptr<ast::TypeExpr> t) noexcept;

mlc::Array<mlc::String> collect_shared_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;

mlc::Array<mlc::String> collect_shared_array_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;

mlc::HashMap<mlc::String, mlc::String> collect_array_elem_types(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;

mlc::Array<mlc::String> collect_shared_map_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;

std::shared_ptr<codegen::CtorTypeInfo> ctor_type_info_for(mlc::String cti_name, mlc::Array<std::shared_ptr<ast::TypeExpr>> types) noexcept;

std::shared_ptr<ast::TypeExpr> field_def_type(std::shared_ptr<ast::FieldDef> fd) noexcept;

mlc::Array<std::shared_ptr<codegen::CtorTypeInfo>> add_ctor_infos_from_variants(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::Array<std::shared_ptr<codegen::CtorTypeInfo>> build_ctor_type_infos_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept;

std::shared_ptr<codegen::CtorTypeInfo> lookup_ctor_type_info(mlc::Array<std::shared_ptr<codegen::CtorTypeInfo>> infos, mlc::String cti_name) noexcept;

codegen::CodegenContext make_body_ctx(codegen::CodegenContext ctx, mlc::Array<mlc::String> sp, mlc::Array<mlc::String> sap, mlc::HashMap<mlc::String, mlc::String> aet, mlc::Array<mlc::String> smp, mlc::String self_t) noexcept;

codegen::CodegenContext ctx_add_shared(codegen::CodegenContext ctx, mlc::String n) noexcept;

codegen::CodegenContext ctx_add_shared_array(codegen::CodegenContext ctx, mlc::String n) noexcept;

codegen::CodegenContext update_ctx_from_stmt(std::shared_ptr<ast::Stmt> stmt, codegen::CodegenContext ctx) noexcept;

mlc::String gen_fn_decl(mlc::String name, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<ast::TypeExpr> return_type, std::shared_ptr<ast::Expr> body, codegen::CodegenContext ctx) noexcept;

mlc::String gen_fn_proto(mlc::String name, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<ast::TypeExpr> return_type, codegen::CodegenContext ctx) noexcept;

mlc::String gen_type_decl(codegen::CodegenContext ctx, mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::String gen_decl(std::shared_ptr<ast::Decl> decl, codegen::CodegenContext ctx) noexcept;

mlc::String gen_proto(std::shared_ptr<ast::Decl> decl, codegen::CodegenContext ctx) noexcept;

mlc::Array<mlc::String> collect_decl_parts(mlc::Array<std::shared_ptr<ast::Decl>> decls, codegen::CodegenContext ctx, int phase) noexcept;

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
  [&]() -> std::tuple<> { if (std::holds_alternative<ast::DeclFn>((*d))) { auto _v_declfn = std::get<ast::DeclFn>((*d)); auto [name, _w0, _w1, _w2] = _v_declfn; return [&]() -> std::tuple<> { 
  names.push_back(name);
  return std::make_tuple();
 }(); } if (std::holds_alternative<ast::DeclType>((*d))) { auto _v_decltype = std::get<ast::DeclType>((*d)); auto [type_name, variants] = _v_decltype; return [&]() -> std::tuple<> { 
  names.push_back(type_name);
  int j = 0;
  while (j < variants.size()){
{
std::visit(overloaded{
  [&](const VarRecord& varrecord) { auto [n, _w0] = varrecord; return [&]() -> std::tuple<> { 
  names.push_back(n);
  return std::make_tuple();
 }(); },
  [&](const VarTuple& vartuple) { auto [n, _w0] = vartuple; return [&]() -> std::tuple<> { 
  names.push_back(n);
  return std::make_tuple();
 }(); },
  [&](const VarUnit& varunit) { auto [n] = varunit; return [&]() -> std::tuple<> { 
  names.push_back(n);
  return std::make_tuple();
 }(); }
}, (*variants[j]));
j = j + 1;
}
}
  return std::make_tuple();
 }(); } if (std::holds_alternative<ast::DeclExtend>((*d))) { auto _v_declextend = std::get<ast::DeclExtend>((*d)); auto [_w0, methods] = _v_declextend; return [&]() -> std::tuple<> { 
  int mi = 0;
  while (mi < methods.size()){
{
[&]() -> std::tuple<> { if (std::holds_alternative<ast::DeclFn>((*methods[mi]))) { auto _v_declfn = std::get<ast::DeclFn>((*methods[mi])); auto [fn_name, _w0, _w1, _w2] = _v_declfn; return [&]() -> std::tuple<> { 
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

mlc::HashMap<mlc::String, mlc::String> add_exports_to_qualified(mlc::HashMap<mlc::String, mlc::String> q, mlc::String imp_path, mlc::Array<codegen::LoadItem> all_items) noexcept{
mlc::String prefix = path_to_module_base(imp_path) + mlc::String("::");
int item_i = 0;
while (item_i < all_items.size()){
{
codegen::LoadItem cur_item = all_items[item_i];
if (cur_item.path == imp_path){
{
int d = 0;
[&]() { 
  while (d < cur_item.decls.size()){
{
int n_i = 0;
mlc::Array<mlc::String> exp_names = decl_export_names(cur_item.decls[d]);
while (n_i < exp_names.size()){
{
if (!q.has(exp_names[n_i])){
{
q.set(exp_names[n_i], prefix);
}
}
n_i = n_i + 1;
}
}
d = d + 1;
}
}
 }();
}
}
item_i = item_i + 1;
}
}
return q;
}

mlc::HashMap<mlc::String, mlc::String> build_qualified(mlc::Array<mlc::String> import_paths, mlc::Array<codegen::LoadItem> all_items) noexcept{
mlc::HashMap<mlc::String, mlc::String> q = mlc::HashMap<mlc::String, mlc::String>();
int imp_i = 0;
while (imp_i < import_paths.size()){
{
mlc::String imp_path = import_paths[imp_i];
q = add_exports_to_qualified(q, imp_path, all_items);
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
q = add_exports_to_qualified(q, cur_item.imports[trans_i], all_items);
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
return q;
}

mlc::String extract_method_name(mlc::String fn_name, mlc::String type_name) noexcept{
mlc::String prefix = type_name + mlc::String("_");
return fn_name.length() > prefix.length() && fn_name.substring(0, prefix.length()) == prefix ? fn_name.substring(prefix.length(), fn_name.length() - prefix.length()) : fn_name;
}

mlc::HashMap<mlc::String, mlc::String> add_method_owners_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls, mlc::HashMap<mlc::String, mlc::String> m) noexcept{
int i = 0;
while (i < decls.size()){
{
[&]() -> void { if (std::holds_alternative<ast::DeclExtend>((*ast::decl_inner(decls[i])))) { auto _v_declextend = std::get<ast::DeclExtend>((*ast::decl_inner(decls[i]))); auto [type_name, methods] = _v_declextend; return [&]() { 
  int j = 0;
  return [&]() { 
  while (j < methods.size()){
{
[&]() -> void { if (std::holds_alternative<ast::DeclFn>((*methods[j]))) { auto _v_declfn = std::get<ast::DeclFn>((*methods[j])); auto [fn_name, _w0, _w1, _w2] = _v_declfn; return [&]() { 
  mlc::String unmangled = extract_method_name(fn_name, type_name);
  if (!m.has(unmangled)){
m.set(unmangled, fn_name);
}
 }(); } return; }();
j = j + 1;
}
}
 }();
 }(); } return; }();
i = i + 1;
}
}
return m;
}

mlc::HashMap<mlc::String, std::shared_ptr<codegen::LoadItem>> build_item_index(mlc::Array<codegen::LoadItem> all_items) noexcept{
mlc::HashMap<mlc::String, std::shared_ptr<codegen::LoadItem>> idx = mlc::HashMap<mlc::String, std::shared_ptr<codegen::LoadItem>>();
int i = 0;
while (i < all_items.size()){
{
idx.set(all_items[i].path, std::make_shared<codegen::LoadItem>(all_items[i]));
i = i + 1;
}
}
return idx;
}

mlc::HashMap<mlc::String, mlc::String> build_method_owners_with_index(codegen::LoadItem item, mlc::HashMap<mlc::String, std::shared_ptr<codegen::LoadItem>> idx) noexcept{
mlc::HashMap<mlc::String, mlc::String> m = mlc::HashMap<mlc::String, mlc::String>();
m = add_method_owners_from_decls(item.decls, m);
int imp_i = 0;
while (imp_i < item.imports.size()){
{
mlc::String imp_path = item.imports[imp_i];
if (idx.has(imp_path)){
{
std::shared_ptr<codegen::LoadItem> dep = idx.get(imp_path);
m = add_method_owners_from_decls(dep->decls, m);
int ti = 0;
[&]() { 
  while (ti < dep->imports.size()){
{
mlc::String t_path = dep->imports[ti];
if (idx.has(t_path)){
{
m = add_method_owners_from_decls(idx.get(t_path)->decls, m);
}
}
ti = ti + 1;
}
}
 }();
}
}
imp_i = imp_i + 1;
}
}
return m;
}

mlc::HashMap<mlc::String, mlc::String> build_method_owners_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept{return add_method_owners_from_decls(decls, mlc::HashMap<mlc::String, mlc::String>());}

mlc::HashMap<mlc::String, mlc::String> build_variant_types_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept{
mlc::HashMap<mlc::String, mlc::String> m = mlc::HashMap<mlc::String, mlc::String>();
int i = 0;
while (i < decls.size()){
{
std::shared_ptr<ast::Decl> d = ast::decl_inner(decls[i]);
[&]() -> void { if (std::holds_alternative<ast::DeclType>((*d))) { auto _v_decltype = std::get<ast::DeclType>((*d)); auto [type_name, variants] = _v_decltype; return [&]() { 
  int j = 0;
  return [&]() { 
  while (j < variants.size()){
{
std::visit(overloaded{
  [&](const VarRecord& varrecord) { auto [vn, _w0] = varrecord; return m.set(vn, type_name); },
  [&](const VarTuple& vartuple) { auto [vn, _w0] = vartuple; return m.set(vn, type_name); },
  [&](const VarUnit& varunit) { auto [vn] = varunit; return m.set(vn, type_name); }
}, (*variants[j]));
j = j + 1;
}
}
 }();
 }(); } return; }();
i = i + 1;
}
}
return m;
}

codegen::CodegenContext create_codegen_context(ast::Program prog) noexcept{return codegen::CodegenContext{build_field_orders(prog), mlc::String(""), mlc::HashMap<mlc::String, mlc::String>(), mlc::String(""), build_method_owners_from_decls(prog.decls), {}, {}, mlc::HashMap<mlc::String, mlc::String>(), {}, {}, build_variant_types_from_decls(prog.decls)};}

mlc::String ctx_resolve(codegen::CodegenContext ctx, mlc::String name) noexcept{return ctx.qualified.has(name) ? ctx.qualified.get(name) + cpp_safe(name) : ctx.ns_prefix.length() > 0 ? ctx.ns_prefix + cpp_safe(name) : cpp_safe(name);}

mlc::Array<std::shared_ptr<codegen::FieldOrder>> build_field_orders(ast::Program prog) noexcept{
mlc::Array<std::shared_ptr<codegen::FieldOrder>> orders = {};
int i = 0;
while (i < prog.decls.size()){
{
std::visit(overloaded{
  [&](const DeclExported& declexported) { auto [d] = declexported; return [&]() { 
  std::visit(overloaded{
  [&](const DeclType& decltype_) {
auto [type_name, variants] = decltype_;
{
int j = 0;
while (j < variants.size()){
{
std::visit(overloaded{
  [&](const VarRecord& varrecord) {
auto [variant_name, field_defs] = varrecord;
{
mlc::Array<mlc::String> field_names = {};
int k = 0;
while (k < field_defs.size()){
{
field_names.push_back(field_defs[k]->name);
k = k + 1;
}
}
orders.push_back(std::make_shared<codegen::FieldOrder>(codegen::FieldOrder{variant_name, field_names}));
if (variants.size() == 1){
{
orders.push_back(std::make_shared<codegen::FieldOrder>(codegen::FieldOrder{type_name, field_names}));
}
}
int _d1 = 0;
}
},
  [&](const VarTuple& vartuple) {
auto [_w0, _w1] = vartuple;
{
}
},
  [&](const VarUnit& varunit) {
auto [_w0] = varunit;
{
}
}
}, (*variants[j]));
j = j + 1;
}
}
int _d2 = 0;
}
},
  [&](const DeclFn& declfn) {
auto [_w0, _w1, _w2, _w3] = declfn;
{
}
},
  [&](const DeclExtend& declextend) {
auto [_w0, _w1] = declextend;
{
}
},
  [&](const DeclImport& declimport) {
auto [_w0, _w1] = declimport;
{
}
},
  [&](const DeclExported& declexported) {
auto [_w0] = declexported;
{
}
}
}, (*ast::decl_inner(d)));
 }(); },
  [&](const DeclType& decltype_) { auto [type_name, variants] = decltype_; return [&]() { 
  int j = 0;
  while (j < variants.size()){
{
std::visit(overloaded{
  [&](const VarRecord& varrecord) {
auto [variant_name, field_defs] = varrecord;
{
mlc::Array<mlc::String> field_names = {};
int k = 0;
while (k < field_defs.size()){
{
field_names.push_back(field_defs[k]->name);
k = k + 1;
}
}
orders.push_back(std::make_shared<codegen::FieldOrder>(codegen::FieldOrder{variant_name, field_names}));
if (variants.size() == 1){
{
orders.push_back(std::make_shared<codegen::FieldOrder>(codegen::FieldOrder{type_name, field_names}));
}
}
int _d1 = 0;
}
},
  [&](const VarTuple& vartuple) {
auto [_w0, _w1] = vartuple;
{
}
},
  [&](const VarUnit& varunit) {
auto [_w0] = varunit;
{
}
}
}, (*variants[j]));
j = j + 1;
}
}
  int _d2 = 0;
 }(); },
  [&](const DeclFn& declfn) { auto [_w0, _w1, _w2, _w3] = declfn; return ; },
  [&](const DeclExtend& declextend) { auto [_w0, _w1] = declextend; return ; },
  [&](const DeclImport& declimport) { auto [_w0, _w1] = declimport; return ; }
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

mlc::String lower_first(mlc::String name) noexcept{return name.length() == 0 ? name : name.char_at(0).to_lower() + name.substring(1, name.length());}

mlc::String type_to_cpp(codegen::CodegenContext ctx, std::shared_ptr<ast::TypeExpr> type_expr) noexcept{return std::visit(overloaded{
  [&](const TyI32& tyi32) { return mlc::String("int"); },
  [&](const TyString& tystring) { return mlc::String("mlc::String"); },
  [&](const TyBool& tybool) { return mlc::String("bool"); },
  [&](const TyUnit& tyunit) { return mlc::String("void"); },
  [&](const TyNamed& tynamed) { auto [name] = tynamed; return ctx_resolve(ctx, name); },
  [&](const TyArray& tyarray) { auto [inner] = tyarray; return mlc::String("mlc::Array<") + type_to_cpp(ctx, inner) + mlc::String(">"); },
  [&](const TyShared& tyshared) { auto [inner] = tyshared; return mlc::String("std::shared_ptr<") + type_to_cpp(ctx, inner) + mlc::String(">"); },
  [&](const TyGeneric& tygeneric) { auto [name, targs] = tygeneric; return name == mlc::String("ref") && targs.size() == 1 ? type_to_cpp(ctx, targs[0]) + mlc::String("&") : [&]() -> mlc::String { 
  mlc::String safe = name == mlc::String("Map") ? mlc::String("mlc::HashMap") : name == mlc::String("Shared") ? mlc::String("std::shared_ptr") : ctx_resolve(ctx, name);
  return targs.size() == 0 ? safe : targs.size() == 1 ? safe + mlc::String("<") + type_to_cpp(ctx, targs[0]) + mlc::String(">") : safe + mlc::String("<") + type_to_cpp(ctx, targs[0]) + mlc::String(", ") + type_to_cpp(ctx, targs[1]) + mlc::String(">");
 }(); },
  [&](const TyFn& tyfn) { auto [params, ret] = tyfn; return [&]() -> mlc::String { 
  mlc::Array<mlc::String> param_types = {};
  int i = 0;
  while (i < params.size()){
{
param_types.push_back(type_to_cpp(ctx, params[i]));
i = i + 1;
}
}
  return mlc::String("std::function<") + type_to_cpp(ctx, ret) + mlc::String("(") + param_types.join(mlc::String(", ")) + mlc::String(")>");
 }(); }
}, (*type_expr));}

mlc::String escape_str(mlc::String input) noexcept{
mlc::Array<mlc::String> parts = {};
int i = 0;
while (i < input.length()){
{
mlc::String c = input.char_at(i);
if (c == mlc::String("\\")){
{
parts.push_back(mlc::String("\\\\"));
}
} else {
{
if (c == mlc::String("\"")){
parts.push_back(mlc::String("\\\""));
} else {
if (c == mlc::String("\n")){
parts.push_back(mlc::String("\\n"));
} else {
if (c == mlc::String("\r")){
parts.push_back(mlc::String("\\r"));
} else {
if (c == mlc::String("\t")){
parts.push_back(mlc::String("\\t"));
} else {
if (c == mlc::String("\0", 1)){
parts.push_back(mlc::String("\\0"));
} else {
parts.push_back(c);
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

mlc::String gen_args(mlc::Array<std::shared_ptr<ast::Expr>> exprs, codegen::CodegenContext ctx) noexcept{
mlc::Array<mlc::String> parts = {};
int i = 0;
while (i < exprs.size()){
{
parts.push_back(gen_expr(exprs[i], ctx));
i = i + 1;
}
}
return parts.join(mlc::String(", "));
}

mlc::String map_method(mlc::String method_name) noexcept{return method_name == mlc::String("length") || method_name == mlc::String("len") ? mlc::String("length") : method_name == mlc::String("push") ? mlc::String("push_back") : method_name == mlc::String("to_string") ? mlc::String("to_string") : method_name == mlc::String("to_int") || method_name == mlc::String("to_i") ? mlc::String("to_i") : method_name == mlc::String("upper") || method_name == mlc::String("to_upper") ? mlc::String("upper") : method_name == mlc::String("lower") || method_name == mlc::String("to_lower") ? mlc::String("lower") : method_name;}

mlc::String map_builtin(mlc::String name) noexcept{return name == mlc::String("print") ? mlc::String("mlc::io::print") : name == mlc::String("println") ? mlc::String("mlc::io::println") : name == mlc::String("exit") ? mlc::String("mlc::io::exit") : name == mlc::String("args") ? mlc::String("mlc::io::args") : name;}

mlc::String gen_stmts_str(mlc::Array<std::shared_ptr<ast::Stmt>> stmts, codegen::CodegenContext ctx) noexcept{
mlc::Array<mlc::String> parts = {};
int i = 0;
codegen::CodegenContext cur_ctx = std::move(ctx);
while (i < stmts.size()){
{
parts.push_back(gen_stmt(stmts[i], cur_ctx));
cur_ctx = update_ctx_from_stmt(stmts[i], cur_ctx);
i = i + 1;
}
}
return parts.join(mlc::String(""));
}

codegen::CodegenContext stmts_final_ctx(mlc::Array<std::shared_ptr<ast::Stmt>> stmts, codegen::CodegenContext ctx) noexcept{
codegen::CodegenContext cur_ctx = std::move(ctx);
int i = 0;
while (i < stmts.size()){
{
cur_ctx = update_ctx_from_stmt(stmts[i], cur_ctx);
i = i + 1;
}
}
return cur_ctx;
}

mlc::String gen_block_body(std::shared_ptr<ast::Expr> expr, codegen::CodegenContext ctx) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprBlock>((*expr)._)) { auto _v_exprblock = std::get<ast::ExprBlock>((*expr)._); auto [stmts, result] = _v_exprblock; return [&]() -> mlc::String { 
  mlc::String stmts_code = gen_stmts_str(stmts, ctx);
  mlc::String result_code = gen_expr(result, stmts_final_ctx(stmts, ctx));
  return result_code == mlc::String("/* unit */") ? stmts_code : stmts_code + result_code + mlc::String(";\n");
 }(); } if (std::holds_alternative<ast::ExprIf>((*expr)._)) { auto _v_exprif = std::get<ast::ExprIf>((*expr)._); auto [_w0, _w1, _w2] = _v_exprif; return gen_stmt_expr(expr, ctx); } return gen_expr(expr, ctx) + mlc::String(";\n"); }();}

mlc::String gen_expr(std::shared_ptr<ast::Expr> expr, codegen::CodegenContext ctx) noexcept{return std::visit(overloaded{
  [&](const ExprInt& exprint) { auto [n] = exprint; return mlc::to_string(n); },
  [&](const ExprStr& exprstr) { auto [s] = exprstr; return mlc::String("mlc::String(\"") + escape_str(s) + mlc::String("\", ") + mlc::to_string(s.length()) + mlc::String(")"); },
  [&](const ExprBool& exprbool) { auto [b] = exprbool; return b ? mlc::String("true") : mlc::String("false"); },
  [&](const ExprUnit& exprunit) { return mlc::String("/* unit */"); },
  [&](const ExprExtern& exprextern) { return mlc::String(""); },
  [&](const ExprIdent& exprident) { auto [name] = exprident; return ctx.self_type.length() > 0 ? [&]() -> mlc::String { 
  mlc::Array<mlc::String> self_fields = lookup_fields(ctx.field_orders, ctx.self_type);
  return list_contains(self_fields, name) ? mlc::String("self.") + cpp_safe(name) : [&]() -> mlc::String { 
  mlc::String mapped = map_builtin(name);
  mlc::String safe = ctx_resolve(ctx, mapped);
  bool needs_brace = name.length() > 0 && name.char_at(0) >= mlc::String("A") && name.char_at(0) <= mlc::String("Z");
  return needs_brace ? safe + mlc::String("{}") : safe;
 }();
 }() : [&]() -> mlc::String { 
  mlc::String mapped = map_builtin(name);
  mlc::String safe = ctx_resolve(ctx, mapped);
  bool needs_brace = name.length() > 0 && name.char_at(0) >= mlc::String("A") && name.char_at(0) <= mlc::String("Z");
  return needs_brace ? safe + mlc::String("{}") : safe;
 }(); },
  [&](const ExprBin& exprbin) { auto [op, left_expr, right_expr] = exprbin; return mlc::String("(") + gen_expr(left_expr, ctx) + mlc::String(" ") + op + mlc::String(" ") + gen_expr(right_expr, ctx) + mlc::String(")"); },
  [&](const ExprUn& exprun) { auto [op, inner] = exprun; return mlc::String("(") + op + gen_expr(inner, ctx) + mlc::String(")"); },
  [&](const ExprCall& exprcall) { auto [func, call_args] = exprcall; return [&]() -> mlc::String { 
  mlc::String fn_code = [&]() -> mlc::String { if (std::holds_alternative<ast::ExprIdent>((*func)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*func)._); auto [name] = _v_exprident; return ctx_resolve(ctx, map_builtin(name)); } return gen_expr(func, ctx); }();
  return fn_code + mlc::String("(") + gen_args(call_args, ctx) + mlc::String(")");
 }(); },
  [&](const ExprMethod& exprmethod) { auto [obj, method_name, margs] = exprmethod; return [&]() -> mlc::String { 
  mlc::String obj_code = [&]() -> mlc::String { if (std::holds_alternative<ast::ExprIdent>((*obj)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*obj)._); auto [name] = _v_exprident; return ctx.self_type.length() > 0 && list_contains(lookup_fields(ctx.field_orders, ctx.self_type), name) ? mlc::String("self.") + cpp_safe(name) : ctx_resolve(ctx, map_builtin(name)); } return gen_expr(obj, ctx); }();
  return obj_code == mlc::String("File") ? [&]() -> mlc::String { 
  mlc::String cpp_fn = method_name == mlc::String("read") ? mlc::String("mlc::file::read_to_string") : method_name == mlc::String("write") ? mlc::String("mlc::file::write_string") : mlc::String("mlc::file::") + method_name;
  return cpp_fn + mlc::String("(") + gen_args(margs, ctx) + mlc::String(")");
 }() : obj_code == mlc::String("Map") && method_name == mlc::String("new") ? mlc::String("{}") : obj_code == mlc::String("Shared") && method_name == mlc::String("new") && margs.size() == 1 ? [&]() -> mlc::String { 
  mlc::String arg_code = gen_expr(margs[0], ctx);
  mlc::String raw_name = [&]() -> mlc::String { if (std::holds_alternative<ast::ExprRecord>((*margs[0])._)) { auto _v_exprrecord = std::get<ast::ExprRecord>((*margs[0])._); auto [n, _w0] = _v_exprrecord; return n; } if (std::holds_alternative<ast::ExprCall>((*margs[0])._)) { auto _v_exprcall = std::get<ast::ExprCall>((*margs[0])._); auto [fn_expr, _w0] = _v_exprcall; return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprIdent>((*fn_expr)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*fn_expr)._); auto [n] = _v_exprident; return n; } return mlc::String(""); }(); } if (std::holds_alternative<ast::ExprIdent>((*margs[0])._)) { auto _v_exprident = std::get<ast::ExprIdent>((*margs[0])._); auto [n] = _v_exprident; return n; } if (std::holds_alternative<ast::ExprIndex>((*margs[0])._)) { auto _v_exprindex = std::get<ast::ExprIndex>((*margs[0])._); auto [arr, _w0] = _v_exprindex; return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprIdent>((*arr)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*arr)._); auto [n] = _v_exprident; return ctx.array_elem_types.has(n) ? ctx.array_elem_types.get(n) : mlc::String(""); } return mlc::String(""); }(); } return mlc::String(""); }();
  mlc::String parent = raw_name.length() > 0 && ctx.variant_types.has(raw_name) ? ctx.variant_types.get(raw_name) : raw_name;
  mlc::String type_name = parent.length() > 0 ? ctx_resolve(ctx, parent) : mlc::String("auto");
  return mlc::String("std::make_shared<") + type_name + mlc::String(">(") + arg_code + mlc::String(")");
 }() : method_name == mlc::String("to_string") && margs.size() == 0 ? mlc::String("mlc::to_string(") + obj_code + mlc::String(")") : ctx.method_owners.has(method_name) ? [&]() -> mlc::String { 
  mlc::String mangled = ctx.method_owners.get(method_name);
  mlc::String fn_name = ctx_resolve(ctx, mangled);
  return margs.size() == 0 ? fn_name + mlc::String("(") + obj_code + mlc::String(")") : fn_name + mlc::String("(") + obj_code + mlc::String(", ") + gen_args(margs, ctx) + mlc::String(")");
 }() : [&]() -> mlc::String { 
  mlc::String base = obj_code + mlc::String(".") + map_method(method_name) + mlc::String("(");
  return margs.size() == 0 ? base + mlc::String(")") : base + gen_args(margs, ctx) + mlc::String(")");
 }();
 }(); },
  [&](const ExprField& exprfield) { auto [obj, field_name] = exprfield; return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprIndex>((*obj)._)) { auto _v_exprindex = std::get<ast::ExprIndex>((*obj)._); auto [inner, _w0] = _v_exprindex; return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprIdent>((*inner)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*inner)._); auto [n] = _v_exprident; return ctx.shared_array_params.contains(n) ? gen_expr(obj, ctx) + mlc::String("->") + cpp_safe(field_name) : gen_expr(obj, ctx) + mlc::String(".") + cpp_safe(field_name); } return gen_expr(obj, ctx) + mlc::String("->") + cpp_safe(field_name); }(); } if (std::holds_alternative<ast::ExprIdent>((*obj)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*obj)._); auto [n] = _v_exprident; return ctx.shared_params.contains(n) ? gen_expr(obj, ctx) + mlc::String("->") + cpp_safe(field_name) : gen_expr(obj, ctx) + mlc::String(".") + cpp_safe(field_name); } if (std::holds_alternative<ast::ExprMethod>((*obj)._)) { auto _v_exprmethod = std::get<ast::ExprMethod>((*obj)._); auto [m_obj, method_name, _w0] = _v_exprmethod; return method_name == mlc::String("get") ? [&]() -> mlc::String { if (std::holds_alternative<ast::ExprIdent>((*m_obj)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*m_obj)._); auto [m] = _v_exprident; return ctx.shared_map_params.contains(m) ? gen_expr(obj, ctx) + mlc::String("->") + cpp_safe(field_name) : gen_expr(obj, ctx) + mlc::String(".") + cpp_safe(field_name); } return gen_expr(obj, ctx) + mlc::String(".") + cpp_safe(field_name); }() : gen_expr(obj, ctx) + mlc::String(".") + cpp_safe(field_name); } return gen_expr(obj, ctx) + mlc::String(".") + cpp_safe(field_name); }(); },
  [&](const ExprIndex& exprindex) { auto [obj, idx] = exprindex; return gen_expr(obj, ctx) + mlc::String("[") + gen_expr(idx, ctx) + mlc::String("]"); },
  [&](const ExprIf& exprif) { auto [cond, then_expr, else_expr] = exprif; return mlc::String("(") + gen_expr(cond, ctx) + mlc::String(" ? ") + gen_expr(then_expr, ctx) + mlc::String(" : ") + gen_expr(else_expr, ctx) + mlc::String(")"); },
  [&](const ExprBlock& exprblock) { auto [stmts, result] = exprblock; return stmts.size() == 0 ? gen_expr(result, ctx) : [&]() -> mlc::String { 
  mlc::String body = gen_stmts_str(stmts, ctx) + mlc::String("return ") + gen_expr(result, stmts_final_ctx(stmts, ctx)) + mlc::String(";\n");
  return mlc::String("[&]() {\n") + body + mlc::String("}()");
 }(); },
  [&](const ExprWhile& exprwhile) { auto [cond, stmts] = exprwhile; return mlc::String("[&]() {\nwhile (") + gen_expr(cond, ctx) + mlc::String(") {\n") + gen_stmts_str(stmts, ctx) + mlc::String("}\n}()"); },
  [&](const ExprFor& exprfor) { auto [var, iter, stmts] = exprfor; return mlc::String("[&]() {\nfor (auto ") + cpp_safe(var) + mlc::String(" : ") + gen_expr(iter, ctx) + mlc::String(") {\n") + gen_stmts_str(stmts, ctx) + mlc::String("}\n}()"); },
  [&](const ExprMatch& exprmatch) { auto [subject, arms] = exprmatch; return gen_match(subject, arms, ctx); },
  [&](const ExprRecord& exprrecord) { auto [name, field_vals] = exprrecord; return [&]() -> mlc::String { 
  mlc::Array<mlc::String> field_order = lookup_fields(ctx.field_orders, name);
  mlc::String vals = field_order.size() > 0 ? gen_record_ordered(field_vals, field_order, ctx) : gen_record_unordered(field_vals, ctx);
  return ctx_resolve(ctx, name) + mlc::String("{") + vals + mlc::String("}");
 }(); },
  [&](const ExprRecordUpdate& exprrecordupdate) { auto [name, base, overrides] = exprrecordupdate; return [&]() -> mlc::String { 
  mlc::Array<mlc::String> field_order = lookup_fields(ctx.field_orders, name);
  mlc::String vals = gen_record_update_ordered(base, overrides, field_order, ctx);
  return ctx_resolve(ctx, name) + mlc::String("{") + vals + mlc::String("}");
 }(); },
  [&](const ExprArray& exprarray) { auto [exprs] = exprarray; return exprs.size() == 0 ? mlc::String("{}") : mlc::String("mlc::Array{") + gen_args(exprs, ctx) + mlc::String("}"); },
  [&](const ExprQuestion& exprquestion) { auto [inner] = exprquestion; return gen_expr(inner, ctx); },
  [&](const ExprLambda& exprlambda) { auto [params, body] = exprlambda; return [&]() -> mlc::String { 
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
  mlc::String body_code = gen_expr(body, ctx);
  return capture + mlc::String("(") + param_list + mlc::String(") { return ") + body_code + mlc::String("; }");
 }(); }
}, (*expr)._);}

mlc::String gen_record_ordered(mlc::Array<std::shared_ptr<ast::FieldVal>> field_vals, mlc::Array<mlc::String> field_order, codegen::CodegenContext ctx) noexcept{
mlc::Array<mlc::String> parts = {};
int i = 0;
while (i < field_order.size()){
{
std::shared_ptr<ast::Expr> value = find_field_val(field_vals, field_order[i]);
parts.push_back(gen_expr(value, ctx));
i = i + 1;
}
}
return parts.join(mlc::String(", "));
}

mlc::String gen_record_unordered(mlc::Array<std::shared_ptr<ast::FieldVal>> field_vals, codegen::CodegenContext ctx) noexcept{
mlc::Array<mlc::String> parts = {};
int i = 0;
while (i < field_vals.size()){
{
parts.push_back(gen_expr(field_vals[i]->val, ctx));
i = i + 1;
}
}
return parts.join(mlc::String(", "));
}

mlc::String gen_record_update_ordered(std::shared_ptr<ast::Expr> base, mlc::Array<std::shared_ptr<ast::FieldVal>> overrides, mlc::Array<mlc::String> field_order, codegen::CodegenContext ctx) noexcept{
mlc::Array<mlc::String> parts = {};
int i = 0;
while (i < field_order.size()){
{
mlc::String field_name = field_order[i];
std::shared_ptr<ast::Expr> override_val = find_field_val(overrides, field_name);
mlc::String val_str = [&]() -> mlc::String { if (std::holds_alternative<ast::ExprUnit>((*override_val)._)) {  return gen_expr(std::make_shared<ast::Expr>(ast::ExprField(base, field_name)), ctx); } return gen_expr(override_val, ctx); }();
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

bool first_arm_needs_deref(std::shared_ptr<ast::MatchArm> first_arm, std::shared_ptr<ast::Expr> subject, codegen::CodegenContext ctx) noexcept{return [&]() -> bool { if (std::holds_alternative<ast::PatCtor>((*first_arm->pat))) { auto _v_patctor = std::get<ast::PatCtor>((*first_arm->pat)); auto [name, _w0] = _v_patctor; return [&]() -> bool { 
  mlc::String resolved = ctx_resolve(ctx, name);
  return resolved.length() >= 12 && resolved.substring(0, 12) == mlc::String("ast_tokens::") ? false : true;
 }(); } return [&]() -> bool { if (std::holds_alternative<ast::ExprMethod>((*subject)._)) { auto _v_exprmethod = std::get<ast::ExprMethod>((*subject)._); auto [_w0, _w1, _w2] = _v_exprmethod; return false; } return true; }(); }();}

mlc::String gen_match(std::shared_ptr<ast::Expr> subject, mlc::Array<std::shared_ptr<ast::MatchArm>> arms, codegen::CodegenContext ctx) noexcept{
mlc::Array<mlc::String> parts = {};
int i = 0;
while (i < arms.size()){
{
parts.push_back(gen_arm(arms[i], ctx));
i = i + 1;
}
}
mlc::String subject_code = gen_expr(subject, ctx);
bool needs_deref = arms.size() > 0 ? first_arm_needs_deref(arms[0], subject, ctx) : true;
mlc::String visit_subject = needs_deref ? mlc::String("(*") + subject_code + mlc::String(")") : subject_code;
return mlc::String("std::visit(overloaded{") + parts.join(mlc::String(",\n")) + mlc::String("\n}, ") + visit_subject + mlc::String(")");
}

mlc::String gen_arm(std::shared_ptr<ast::MatchArm> arm, codegen::CodegenContext ctx) noexcept{return std::visit(overloaded{
  [&](const PatWild& patwild) { return mlc::String("[&](const auto& __v) { return ") + gen_expr(arm->body, ctx) + mlc::String("; }"); },
  [&](const PatUnit& patunit) { return mlc::String("[&](const auto& __v) { return ") + gen_expr(arm->body, ctx) + mlc::String("; }"); },
  [&](const PatBool& patbool) { auto [_w0] = patbool; return mlc::String("[&](const auto& __v) { return ") + gen_expr(arm->body, ctx) + mlc::String("; }"); },
  [&](const PatInt& patint) { auto [_w0] = patint; return mlc::String("[&](const auto& __v) { return ") + gen_expr(arm->body, ctx) + mlc::String("; }"); },
  [&](const PatStr& patstr) { auto [_w0] = patstr; return mlc::String("[&](const auto& __v) { return ") + gen_expr(arm->body, ctx) + mlc::String("; }"); },
  [&](const PatIdent& patident) { auto [name] = patident; return mlc::String("[&](const auto& ") + cpp_safe(name) + mlc::String(") { return ") + gen_expr(arm->body, ctx) + mlc::String("; }"); },
  [&](const PatCtor& patctor) { auto [name, sub_pats] = patctor; return [&]() -> mlc::String { 
  mlc::String qual = ctx_resolve(ctx, name);
  mlc::String lower_name = cpp_safe(lower_first(name));
  mlc::String binding = sub_pats.size() == 0 ? mlc::String("") : mlc::String("auto [") + pat_bind_names(sub_pats) + mlc::String("] = ") + lower_name + mlc::String("; ");
  std::shared_ptr<codegen::CtorTypeInfo> cti = lookup_ctor_type_info(ctx.ctor_type_infos, name);
  codegen::CodegenContext arm_ctx = std::move(ctx);
  int k = 0;
  while (k < sub_pats.size()){
{
[&]() -> void { if (std::holds_alternative<ast::PatIdent>((*sub_pats[k]))) { auto _v_patident = std::get<ast::PatIdent>((*sub_pats[k])); auto [pn] = _v_patident; return [&]() { 
  if (cti->shared_pos.contains(k)){
{
arm_ctx = ctx_add_shared(arm_ctx, pn);
}
}
  if (cti->shared_arr_pos.contains(k)){
arm_ctx = ctx_add_shared_array(arm_ctx, pn);
}
 }(); } return; }();
k = k + 1;
}
}
  return mlc::String("[&](const ") + qual + mlc::String("& ") + lower_name + mlc::String(") { ") + binding + mlc::String("return ") + gen_expr(arm->body, arm_ctx) + mlc::String("; }");
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

mlc::String gen_stmt(std::shared_ptr<ast::Stmt> stmt, codegen::CodegenContext ctx) noexcept{return std::visit(overloaded{
  [&](const StmtLet& stmtlet) { auto [name, _w0, typ, value] = stmtlet; return [&]() -> mlc::String { 
  mlc::String val_code = [&]() -> mlc::String { if (std::holds_alternative<ast::ExprArray>((*value)._)) { auto _v_exprarray = std::get<ast::ExprArray>((*value)._); auto [elems] = _v_exprarray; return elems.size() == 0 ? [&]() -> mlc::String { if (std::holds_alternative<ast::TyArray>((*typ))) { auto _v_tyarray = std::get<ast::TyArray>((*typ)); auto [inner] = _v_tyarray; return mlc::String("mlc::Array<") + type_to_cpp(ctx, inner) + mlc::String(">{}"); } return mlc::String("mlc::Array<mlc::String>{}"); }() : mlc::String("mlc::Array{") + gen_args(elems, ctx) + mlc::String("}"); } if (std::holds_alternative<ast::ExprMethod>((*value)._)) { auto _v_exprmethod = std::get<ast::ExprMethod>((*value)._); auto [map_obj, mn, _w0] = _v_exprmethod; return mn == mlc::String("new") ? [&]() -> mlc::String { if (std::holds_alternative<ast::ExprIdent>((*map_obj)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*map_obj)._); auto [on] = _v_exprident; return on == mlc::String("Map") ? [&]() -> mlc::String { if (std::holds_alternative<ast::TyGeneric>((*typ))) { auto _v_tygeneric = std::get<ast::TyGeneric>((*typ)); auto [tn, targs] = _v_tygeneric; return tn == mlc::String("Map") && targs.size() == 2 ? mlc::String("mlc::HashMap<") + type_to_cpp(ctx, targs[0]) + mlc::String(", ") + type_to_cpp(ctx, targs[1]) + mlc::String(">()") : gen_expr(value, ctx); } return gen_expr(value, ctx); }() : gen_expr(value, ctx); } return gen_expr(value, ctx); }() : gen_expr(value, ctx); } return gen_expr(value, ctx); }();
  return mlc::String("auto ") + cpp_safe(name) + mlc::String(" = ") + val_code + mlc::String(";\n");
 }(); },
  [&](const StmtExpr& stmtexpr) { auto [expr] = stmtexpr; return gen_stmt_expr(expr, ctx); },
  [&](const StmtBreak& stmtbreak) { return mlc::String("break;\n"); },
  [&](const StmtContinue& stmtcontinue) { return mlc::String("continue;\n"); },
  [&](const StmtReturn& stmtreturn) { auto [expr] = stmtreturn; return mlc::String("return ") + gen_expr(expr, ctx) + mlc::String(";\n"); }
}, (*stmt)._);}

mlc::String gen_stmt_expr(std::shared_ptr<ast::Expr> expr, codegen::CodegenContext ctx) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprBin>((*expr)._)) { auto _v_exprbin = std::get<ast::ExprBin>((*expr)._); auto [op, left_expr, right_expr] = _v_exprbin; return op == mlc::String("=") ? gen_expr(left_expr, ctx) + mlc::String(" = ") + gen_expr(right_expr, ctx) + mlc::String(";\n") : mlc::String("(") + gen_expr(left_expr, ctx) + mlc::String(" ") + op + mlc::String(" ") + gen_expr(right_expr, ctx) + mlc::String(");\n"); } if (std::holds_alternative<ast::ExprIf>((*expr)._)) { auto _v_exprif = std::get<ast::ExprIf>((*expr)._); auto [cond, then_expr, else_expr] = _v_exprif; return [&]() -> mlc::String { 
  mlc::String output = mlc::String("if (") + gen_expr(cond, ctx) + mlc::String(") {\n") + gen_block_body(then_expr, ctx) + mlc::String("}");
  mlc::String else_str = [&]() -> mlc::String { if (std::holds_alternative<ast::ExprUnit>((*else_expr)._)) {  return mlc::String(""); } if (std::holds_alternative<ast::ExprBlock>((*else_expr)._)) { auto _v_exprblock = std::get<ast::ExprBlock>((*else_expr)._); auto [stmts, result] = _v_exprblock; return stmts.size() == 0 ? [&]() -> mlc::String { if (std::holds_alternative<ast::ExprUnit>((*result)._)) {  return mlc::String(""); } return mlc::String(" else {\n") + gen_block_body(else_expr, ctx) + mlc::String("}"); }() : mlc::String(" else {\n") + gen_block_body(else_expr, ctx) + mlc::String("}"); } if (std::holds_alternative<ast::ExprIf>((*else_expr)._)) { auto _v_exprif = std::get<ast::ExprIf>((*else_expr)._); auto [_w0, _w1, _w2] = _v_exprif; return mlc::String(" else ") + gen_stmt_expr(else_expr, ctx); } return mlc::String(" else {\n") + gen_block_body(else_expr, ctx) + mlc::String("}"); }();
  if (else_str.length() > 0){
{
output = output + else_str;
}
}
  return output + mlc::String("\n");
 }(); } if (std::holds_alternative<ast::ExprWhile>((*expr)._)) { auto _v_exprwhile = std::get<ast::ExprWhile>((*expr)._); auto [cond, stmts] = _v_exprwhile; return mlc::String("while (") + gen_expr(cond, ctx) + mlc::String(") {\n") + gen_stmts_str(stmts, ctx) + mlc::String("}\n"); } if (std::holds_alternative<ast::ExprFor>((*expr)._)) { auto _v_exprfor = std::get<ast::ExprFor>((*expr)._); auto [var, iter, stmts] = _v_exprfor; return mlc::String("for (auto ") + cpp_safe(var) + mlc::String(" : ") + gen_expr(iter, ctx) + mlc::String(") {\n") + gen_stmts_str(stmts, ctx) + mlc::String("}\n"); } if (std::holds_alternative<ast::ExprBlock>((*expr)._)) { auto _v_exprblock = std::get<ast::ExprBlock>((*expr)._); auto [stmts, result] = _v_exprblock; return [&]() -> mlc::String { 
  mlc::String stmts_code = gen_stmts_str(stmts, ctx);
  mlc::String result_code = gen_expr(result, stmts_final_ctx(stmts, ctx));
  return result_code == mlc::String("/* unit */") ? stmts_code : stmts_code + result_code + mlc::String(";\n");
 }(); } return gen_expr(expr, ctx) + mlc::String(";\n"); }();}

mlc::String gen_return_body(std::shared_ptr<ast::Expr> expr, codegen::CodegenContext ctx) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprBlock>((*expr)._)) { auto _v_exprblock = std::get<ast::ExprBlock>((*expr)._); auto [stmts, result] = _v_exprblock; return [&]() -> mlc::String { 
  mlc::String stmts_code = gen_stmts_str(stmts, ctx);
  mlc::String result_code = gen_expr(result, stmts_final_ctx(stmts, ctx));
  return result_code == mlc::String("/* unit */") ? stmts_code : stmts_code + mlc::String("return ") + result_code + mlc::String(";\n");
 }(); } if (std::holds_alternative<ast::ExprIf>((*expr)._)) { auto _v_exprif = std::get<ast::ExprIf>((*expr)._); auto [cond, then_expr, else_expr] = _v_exprif; return [&]() -> mlc::String { 
  mlc::String output = mlc::String("if (") + gen_expr(cond, ctx) + mlc::String(") {\n") + gen_return_body(then_expr, ctx) + mlc::String("}");
  mlc::String else_str = [&]() -> mlc::String { if (std::holds_alternative<ast::ExprUnit>((*else_expr)._)) {  return mlc::String(""); } if (std::holds_alternative<ast::ExprIf>((*else_expr)._)) { auto _v_exprif = std::get<ast::ExprIf>((*else_expr)._); auto [_w0, _w1, _w2] = _v_exprif; return mlc::String(" else ") + gen_return_if_stmt(else_expr, ctx); } return mlc::String(" else {\n") + gen_return_body(else_expr, ctx) + mlc::String("}"); }();
  if (else_str.length() > 0){
{
output = output + else_str;
}
}
  return output + mlc::String("\n");
 }(); } return mlc::String("return ") + gen_expr(expr, ctx) + mlc::String(";\n"); }();}

mlc::String gen_return_if_stmt(std::shared_ptr<ast::Expr> expr, codegen::CodegenContext ctx) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprIf>((*expr)._)) { auto _v_exprif = std::get<ast::ExprIf>((*expr)._); auto [cond, then_expr, else_expr] = _v_exprif; return [&]() -> mlc::String { 
  mlc::String output = mlc::String("if (") + gen_expr(cond, ctx) + mlc::String(") {\n") + gen_return_body(then_expr, ctx) + mlc::String("}");
  mlc::String else_str = [&]() -> mlc::String { if (std::holds_alternative<ast::ExprUnit>((*else_expr)._)) {  return mlc::String(""); } if (std::holds_alternative<ast::ExprIf>((*else_expr)._)) { auto _v_exprif = std::get<ast::ExprIf>((*else_expr)._); auto [_w0, _w1, _w2] = _v_exprif; return mlc::String(" else ") + gen_return_if_stmt(else_expr, ctx); } return mlc::String(" else {\n") + gen_return_body(else_expr, ctx) + mlc::String("}"); }();
  if (else_str.length() > 0){
{
output = output + else_str;
}
}
  return output + mlc::String("\n");
 }(); } return mlc::String("if (true) {\n") + gen_return_body(expr, ctx) + mlc::String("}\n"); }();}

mlc::String gen_fn_body(std::shared_ptr<ast::Expr> body, codegen::CodegenContext ctx) noexcept{return gen_return_body(body, ctx);}

mlc::String gen_params(codegen::CodegenContext ctx, mlc::Array<std::shared_ptr<ast::Param>> params) noexcept{
mlc::Array<mlc::String> parts = {};
int i = 0;
while (i < params.size()){
{
parts.push_back(type_to_cpp(ctx, params[i]->typ) + mlc::String(" ") + cpp_safe(params[i]->name));
i = i + 1;
}
}
return parts.join(mlc::String(", "));
}

mlc::String gen_type_decl_fwd(codegen::CodegenContext ctx, mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{return variants.size() == 1 ? mlc::String("") : gen_adt_fwd(ctx, type_name, variants);}

mlc::String gen_type_decl_body(codegen::CodegenContext ctx, mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{return variants.size() == 1 ? gen_single_variant(ctx, type_name, variants[0]) : gen_adt_defs(ctx, type_name, variants);}

mlc::String gen_single_variant(codegen::CodegenContext ctx, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept{return std::visit(overloaded{
  [&](const VarRecord& varrecord) { auto [_w0, field_defs] = varrecord; return [&]() -> mlc::String { 
  mlc::Array<mlc::String> parts = {};
  int i = 0;
  while (i < field_defs.size()){
{
parts.push_back(type_to_cpp(ctx, field_defs[i]->typ) + mlc::String(" ") + cpp_safe(field_defs[i]->name) + mlc::String(";"));
i = i + 1;
}
}
  return mlc::String("struct ") + ctx_resolve(ctx, type_name) + mlc::String(" {") + parts.join(mlc::String("")) + mlc::String("};\n");
 }(); },
  [&](const VarTuple& vartuple) { auto [_w0, field_types] = vartuple; return [&]() -> mlc::String { 
  mlc::Array<mlc::String> parts = {};
  int i = 0;
  while (i < field_types.size()){
{
parts.push_back(type_to_cpp(ctx, field_types[i]) + mlc::String(" field") + mlc::to_string(i) + mlc::String(";"));
i = i + 1;
}
}
  return mlc::String("struct ") + ctx_resolve(ctx, type_name) + mlc::String(" {") + parts.join(mlc::String("")) + mlc::String("};\n");
 }(); },
  [&](const VarUnit& varunit) { auto [_w0] = varunit; return mlc::String("struct ") + ctx_resolve(ctx, type_name) + mlc::String(" {};\n"); }
}, (*variant));}

mlc::String gen_adt_fwd(codegen::CodegenContext ctx, mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
mlc::Array<mlc::String> parts = {};
mlc::Array<mlc::String> variant_names = {};
int index = 0;
while (index < variants.size()){
{
mlc::String variant_name = ctx_resolve(ctx, variant_ctor_name(variants[index]));
variant_names.push_back(variant_name);
parts.push_back(mlc::String("struct ") + variant_name + mlc::String(";\n"));
index = index + 1;
}
}
return parts.join(mlc::String("")) + mlc::String("using ") + ctx_resolve(ctx, type_name) + mlc::String(" = std::variant<") + variant_names.join(mlc::String(", ")) + mlc::String(">;\n");
}

mlc::String gen_adt_defs(codegen::CodegenContext ctx, mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
mlc::Array<mlc::String> parts = {};
int i = 0;
while (i < variants.size()){
{
parts.push_back(gen_variant_struct(ctx, type_name, variants[i]));
i = i + 1;
}
}
return parts.join(mlc::String(""));
}

mlc::String variant_ctor_name(std::shared_ptr<ast::TypeVariant> variant) noexcept{return std::visit(overloaded{
  [&](const VarUnit& varunit) { auto [name] = varunit; return name; },
  [&](const VarTuple& vartuple) { auto [name, _w0] = vartuple; return name; },
  [&](const VarRecord& varrecord) { auto [name, _w0] = varrecord; return name; }
}, (*variant));}

mlc::String gen_variant_struct(codegen::CodegenContext ctx, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept{return std::visit(overloaded{
  [&](const VarUnit& varunit) { auto [name] = varunit; return mlc::String("struct ") + ctx_resolve(ctx, name) + mlc::String(" {};\n"); },
  [&](const VarTuple& vartuple) { auto [name, field_types] = vartuple; return [&]() -> mlc::String { 
  mlc::Array<mlc::String> parts = {};
  int i = 0;
  while (i < field_types.size()){
{
parts.push_back(type_to_cpp(ctx, field_types[i]) + mlc::String(" field") + mlc::to_string(i) + mlc::String(";"));
i = i + 1;
}
}
  return mlc::String("struct ") + ctx_resolve(ctx, name) + mlc::String(" {") + parts.join(mlc::String("")) + mlc::String("};\n");
 }(); },
  [&](const VarRecord& varrecord) { auto [name, field_defs] = varrecord; return [&]() -> mlc::String { 
  mlc::Array<mlc::String> parts = {};
  int i = 0;
  while (i < field_defs.size()){
{
parts.push_back(type_to_cpp(ctx, field_defs[i]->typ) + mlc::String(" ") + cpp_safe(field_defs[i]->name) + mlc::String(";"));
i = i + 1;
}
}
  return mlc::String("struct ") + ctx_resolve(ctx, name) + mlc::String(" {") + parts.join(mlc::String("")) + mlc::String("};\n");
 }(); }
}, (*variant));}

bool is_shared_type(std::shared_ptr<ast::TypeExpr> t) noexcept{return [&]() -> bool { if (std::holds_alternative<ast::TyGeneric>((*t))) { auto _v_tygeneric = std::get<ast::TyGeneric>((*t)); auto [n, _w0] = _v_tygeneric; return n == mlc::String("Shared"); } if (std::holds_alternative<ast::TyShared>((*t))) { auto _v_tyshared = std::get<ast::TyShared>((*t)); auto [_w0] = _v_tyshared; return true; } return false; }();}

bool is_shared_array_type(std::shared_ptr<ast::TypeExpr> t) noexcept{return [&]() -> bool { if (std::holds_alternative<ast::TyArray>((*t))) { auto _v_tyarray = std::get<ast::TyArray>((*t)); auto [inner] = _v_tyarray; return is_shared_type(inner); } return false; }();}

bool is_shared_map_type(std::shared_ptr<ast::TypeExpr> t) noexcept{return [&]() -> bool { if (std::holds_alternative<ast::TyGeneric>((*t))) { auto _v_tygeneric = std::get<ast::TyGeneric>((*t)); auto [n, targs] = _v_tygeneric; return n == mlc::String("Map") && targs.size() >= 2 && is_shared_type(targs[1]); } return false; }();}

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

std::shared_ptr<codegen::CtorTypeInfo> ctor_type_info_for(mlc::String cti_name, mlc::Array<std::shared_ptr<ast::TypeExpr>> types) noexcept{
mlc::Array<int> sp = {};
mlc::Array<int> sap = {};
int k = 0;
while (k < types.size()){
{
if (is_shared_type(types[k])){
{
sp.push_back(k);
}
}
if (is_shared_array_type(types[k])){
{
sap.push_back(k);
}
}
k = k + 1;
}
}
return std::make_shared<codegen::CtorTypeInfo>(codegen::CtorTypeInfo{cti_name, sp, sap});
}

std::shared_ptr<ast::TypeExpr> field_def_type(std::shared_ptr<ast::FieldDef> fd) noexcept{return fd->typ;}

mlc::Array<std::shared_ptr<codegen::CtorTypeInfo>> add_ctor_infos_from_variants(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
mlc::Array<std::shared_ptr<codegen::CtorTypeInfo>> result = {};
int j = 0;
while (j < variants.size()){
{
std::visit(overloaded{
  [&](const VarTuple& vartuple) { auto [vn, types] = vartuple; return result.push_back(ctor_type_info_for(vn, types)); },
  [&](const VarRecord& varrecord) { auto [vn, fdefs] = varrecord; return [&]() { 
  mlc::Array<std::shared_ptr<ast::TypeExpr>> types = {};
  int k = 0;
  while (k < fdefs.size()){
{
types.push_back(field_def_type(fdefs[k]));
k = k + 1;
}
}
  return result.push_back(ctor_type_info_for(vn, types));
 }(); },
  [&](const VarUnit& varunit) { auto [_w0] = varunit; return ; }
}, (*variants[j]));
j = j + 1;
}
}
return result;
}

mlc::Array<std::shared_ptr<codegen::CtorTypeInfo>> build_ctor_type_infos_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept{
mlc::Array<std::shared_ptr<codegen::CtorTypeInfo>> result = {};
int i = 0;
while (i < decls.size()){
{
[&]() -> void { if (std::holds_alternative<ast::DeclExported>((*decls[i]))) { auto _v_declexported = std::get<ast::DeclExported>((*decls[i])); auto [d] = _v_declexported; return [&]() -> void { if (std::holds_alternative<ast::DeclType>((*ast::decl_inner(d)))) { auto _v_decltype = std::get<ast::DeclType>((*ast::decl_inner(d))); auto [_w0, variants] = _v_decltype; return [&]() { 
  mlc::Array<std::shared_ptr<codegen::CtorTypeInfo>> items = add_ctor_infos_from_variants(variants);
  int m = 0;
  return [&]() { 
  while (m < items.size()){
{
result.push_back(items[m]);
m = m + 1;
}
}
 }();
 }(); } return; }(); } if (std::holds_alternative<ast::DeclType>((*decls[i]))) { auto _v_decltype = std::get<ast::DeclType>((*decls[i])); auto [_w0, variants] = _v_decltype; return [&]() { 
  mlc::Array<std::shared_ptr<codegen::CtorTypeInfo>> items = add_ctor_infos_from_variants(variants);
  int m = 0;
  return [&]() { 
  while (m < items.size()){
{
result.push_back(items[m]);
m = m + 1;
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

codegen::CodegenContext make_body_ctx(codegen::CodegenContext ctx, mlc::Array<mlc::String> sp, mlc::Array<mlc::String> sap, mlc::HashMap<mlc::String, mlc::String> aet, mlc::Array<mlc::String> smp, mlc::String self_t) noexcept{return codegen::CodegenContext{ctx.field_orders, ctx.ns_prefix, ctx.qualified, self_t, ctx.method_owners, sp, sap, aet, smp, ctx.ctor_type_infos, ctx.variant_types};}

codegen::CodegenContext ctx_add_shared(codegen::CodegenContext ctx, mlc::String n) noexcept{
mlc::Array<mlc::String> sp = ctx.shared_params;
sp.push_back(n);
return make_body_ctx(ctx, sp, ctx.shared_array_params, ctx.array_elem_types, ctx.shared_map_params, ctx.self_type);
}

codegen::CodegenContext ctx_add_shared_array(codegen::CodegenContext ctx, mlc::String n) noexcept{
mlc::Array<mlc::String> sap = ctx.shared_array_params;
sap.push_back(n);
return make_body_ctx(ctx, ctx.shared_params, sap, ctx.array_elem_types, ctx.shared_map_params, ctx.self_type);
}

codegen::CodegenContext update_ctx_from_stmt(std::shared_ptr<ast::Stmt> stmt, codegen::CodegenContext ctx) noexcept{return [&]() -> codegen::CodegenContext { if (std::holds_alternative<ast::StmtLet>((*stmt)._)) { auto _v_stmtlet = std::get<ast::StmtLet>((*stmt)._); auto [n, _w0, typ, value] = _v_stmtlet; return is_shared_type(typ) ? ctx_add_shared(ctx, n) : is_shared_array_type(typ) ? ctx_add_shared_array(ctx, n) : [&]() -> codegen::CodegenContext { if (std::holds_alternative<ast::ExprMethod>((*value)._)) { auto _v_exprmethod = std::get<ast::ExprMethod>((*value)._); auto [m_obj, method_name, _w0] = _v_exprmethod; return method_name == mlc::String("get") ? [&]() -> codegen::CodegenContext { if (std::holds_alternative<ast::ExprIdent>((*m_obj)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*m_obj)._); auto [m] = _v_exprident; return ctx.shared_map_params.contains(m) ? ctx_add_shared(ctx, n) : ctx; } return ctx; }() : ctx; } return ctx; }(); } return ctx; }();}

mlc::String gen_fn_decl(mlc::String name, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<ast::TypeExpr> return_type, std::shared_ptr<ast::Expr> body, codegen::CodegenContext ctx) noexcept{
mlc::String safe_name = ctx_resolve(ctx, name);
mlc::Array<mlc::String> sp = collect_shared_params(params);
mlc::Array<mlc::String> sap = collect_shared_array_params(params);
mlc::HashMap<mlc::String, mlc::String> aet = collect_array_elem_types(params);
mlc::Array<mlc::String> smp = collect_shared_map_params(params);
codegen::CodegenContext body_ctx = params.size() > 0 && params[0]->name == mlc::String("self") ? [&]() -> codegen::CodegenContext { if (std::holds_alternative<ast::TyNamed>((*params[0]->typ))) { auto _v_tynamed = std::get<ast::TyNamed>((*params[0]->typ)); auto [t] = _v_tynamed; return make_body_ctx(ctx, sp, sap, aet, smp, t); } return make_body_ctx(ctx, sp, sap, aet, smp, ctx.self_type); }() : make_body_ctx(ctx, sp, sap, aet, smp, ctx.self_type);
return name == mlc::String("main") && params.size() == 0 ? [&]() -> mlc::String { 
  mlc::String preamble = mlc::String("mlc::io::set_args(std::vector<mlc::String>(argv + 1, argv + argc));\n");
  return type_to_cpp(ctx, return_type) + mlc::String(" ") + safe_name + mlc::String("(int argc, char** argv) noexcept{\n") + preamble + gen_fn_body(body, body_ctx) + mlc::String("}\n");
 }() : type_to_cpp(ctx, return_type) + mlc::String(" ") + safe_name + mlc::String("(") + gen_params(ctx, params) + mlc::String(") noexcept{\n") + gen_fn_body(body, body_ctx) + mlc::String("}\n");
}

mlc::String gen_fn_proto(mlc::String name, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<ast::TypeExpr> return_type, codegen::CodegenContext ctx) noexcept{
mlc::String safe_name = ctx_resolve(ctx, name);
return name == mlc::String("main") && params.size() == 0 ? type_to_cpp(ctx, return_type) + mlc::String(" ") + safe_name + mlc::String("(int argc, char** argv) noexcept;\n") : type_to_cpp(ctx, return_type) + mlc::String(" ") + safe_name + mlc::String("(") + gen_params(ctx, params) + mlc::String(") noexcept;\n");
}

mlc::String gen_type_decl(codegen::CodegenContext ctx, mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{return gen_type_decl_fwd(ctx, type_name, variants) + gen_type_decl_body(ctx, type_name, variants);}

mlc::String gen_decl(std::shared_ptr<ast::Decl> decl, codegen::CodegenContext ctx) noexcept{return std::visit(overloaded{
  [&](const DeclType& decltype_) { auto [type_name, variants] = decltype_; return gen_type_decl(ctx, type_name, variants); },
  [&](const DeclFn& declfn) { auto [name, params, return_type, body] = declfn; return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprExtern>((*body)._)) {  return mlc::String(""); } return gen_fn_decl(name, params, return_type, body, ctx); }(); },
  [&](const DeclExtend& declextend) { auto [_w0, methods] = declextend; return [&]() -> mlc::String { 
  mlc::Array<mlc::String> parts = {};
  int index = 0;
  while (index < methods.size()){
{
parts.push_back(gen_decl(methods[index], ctx));
index = index + 1;
}
}
  return parts.join(mlc::String(""));
 }(); },
  [&](const DeclImport& declimport) { auto [_w0, _w1] = declimport; return mlc::String(""); },
  [&](const DeclExported& declexported) { auto [d] = declexported; return gen_decl(d, ctx); }
}, (*decl));}

mlc::String gen_proto(std::shared_ptr<ast::Decl> decl, codegen::CodegenContext ctx) noexcept{return std::visit(overloaded{
  [&](const DeclType& decltype_) { auto [_w0, _w1] = decltype_; return mlc::String(""); },
  [&](const DeclFn& declfn) { auto [name, params, ret, body] = declfn; return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprExtern>((*body)._)) {  return mlc::String(""); } return gen_fn_proto(name, params, ret, ctx); }(); },
  [&](const DeclExtend& declextend) { auto [_w0, methods] = declextend; return [&]() -> mlc::String { 
  mlc::Array<mlc::String> parts = {};
  int index = 0;
  while (index < methods.size()){
{
parts.push_back(gen_proto(methods[index], ctx));
index = index + 1;
}
}
  return parts.join(mlc::String(""));
 }(); },
  [&](const DeclImport& declimport) { auto [_w0, _w1] = declimport; return mlc::String(""); },
  [&](const DeclExported& declexported) { auto [d] = declexported; return gen_proto(d, ctx); }
}, (*decl));}

mlc::Array<mlc::String> collect_decl_parts(mlc::Array<std::shared_ptr<ast::Decl>> decls, codegen::CodegenContext ctx, int phase) noexcept{
mlc::Array<mlc::String> parts = {};
int index = 0;
while (index < decls.size()){
{
mlc::String segment = std::visit(overloaded{
  [&](const DeclType& decltype_) { auto [type_name, variants] = decltype_; return phase == 0 ? gen_type_decl_fwd(ctx, type_name, variants) : phase == 1 ? gen_type_decl_body(ctx, type_name, variants) : mlc::String(""); },
  [&](const DeclFn& declfn) { auto [_w0, _w1, _w2, _w3] = declfn; return phase == 2 ? gen_proto(decls[index], ctx) : phase == 3 ? gen_decl(decls[index], ctx) : mlc::String(""); },
  [&](const DeclExtend& declextend) { auto [_w0, _w1] = declextend; return phase == 2 ? gen_proto(decls[index], ctx) : phase == 3 ? gen_decl(decls[index], ctx) : mlc::String(""); },
  [&](const DeclImport& declimport) { auto [_w0, _w1] = declimport; return mlc::String(""); },
  [&](const DeclExported& declexported) { auto [d] = declexported; return phase == 0 ? [&]() -> mlc::String { if (std::holds_alternative<ast::DeclType>((*ast::decl_inner(d)))) { auto _v_decltype = std::get<ast::DeclType>((*ast::decl_inner(d))); auto [type_name, variants] = _v_decltype; return gen_type_decl_fwd(ctx, type_name, variants); } return mlc::String(""); }() : phase == 1 ? [&]() -> mlc::String { if (std::holds_alternative<ast::DeclType>((*ast::decl_inner(d)))) { auto _v_decltype = std::get<ast::DeclType>((*ast::decl_inner(d))); auto [type_name, variants] = _v_decltype; return gen_type_decl_body(ctx, type_name, variants); } return mlc::String(""); }() : phase == 2 ? gen_proto(d, ctx) : phase == 3 ? [&]() -> mlc::String { if (std::holds_alternative<ast::DeclType>((*ast::decl_inner(d)))) { auto _v_decltype = std::get<ast::DeclType>((*ast::decl_inner(d))); auto [_w0, _w1] = _v_decltype; return mlc::String(""); } return gen_decl(d, ctx); }() : mlc::String(""); }
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
codegen::CodegenContext ctx = create_codegen_context(prog);
mlc::Array<mlc::String> preamble = mlc::Array<mlc::String>{mlc::String("#include \"mlc/core/string.hpp\"\n"), mlc::String("#include \"mlc/core/array.hpp\"\n"), mlc::String("#include \"mlc/core/hashmap.hpp\"\n"), mlc::String("#include \"mlc/core/match.hpp\"\n"), mlc::String("#include \"mlc/io/io.hpp\"\n"), mlc::String("#include \"mlc/io/file.hpp\"\n"), mlc::String("#include <functional>\n"), mlc::String("#include <memory>\n"), mlc::String("#include <variant>\n"), mlc::String("#include <vector>\n\n")};
mlc::Array<mlc::String> type_fwds = collect_decl_parts(prog.decls, ctx, 0);
mlc::Array<mlc::String> type_defs = collect_decl_parts(prog.decls, ctx, 1);
mlc::Array<mlc::String> fn_protos = collect_decl_parts(prog.decls, ctx, 2);
mlc::Array<mlc::String> fn_decls = collect_decl_parts(prog.decls, ctx, 3);
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
codegen::CodegenContext ctx = codegen::CodegenContext{precomp.field_orders, mlc::String(""), qualified, mlc::String(""), build_method_owners_with_index(item, precomp.item_index), {}, {}, mlc::HashMap<mlc::String, mlc::String>(), {}, precomp.ctor_type_infos, precomp.variant_types};
mlc::String ns = base == mlc::String("main") ? mlc::String("mlc_main") : base;
mlc::String std_includes = mlc::String("#include \"mlc.hpp\"\n#include <variant>\n\n") + include_lines(item.imports) + mlc::String("\n");
mlc::Array<mlc::String> type_fwds = collect_decl_parts(item.decls, ctx, 0);
mlc::Array<mlc::String> type_defs = collect_decl_parts(item.decls, ctx, 1);
mlc::Array<mlc::String> fn_protos = collect_decl_parts(item.decls, ctx, 2);
mlc::Array<mlc::String> fn_defs = collect_decl_parts(item.decls, ctx, 3);
mlc::String guard = base.upper() + mlc::String("_HPP");
mlc::String header = mlc::Array<mlc::String>{mlc::String("#ifndef ") + guard + mlc::String("\n"), mlc::String("#define ") + guard + mlc::String("\n"), mlc::String("\n"), std_includes, mlc::String("namespace ") + ns + mlc::String(" {\n"), mlc::String("\n"), type_fwds.join(mlc::String("")), type_defs.join(mlc::String("")), fn_protos.join(mlc::String("")), mlc::String("\n"), mlc::String("} // namespace ") + ns + mlc::String("\n"), mlc::String("\n"), mlc::String("#endif // ") + guard + mlc::String("\n")}.join(mlc::String(""));
mlc::String cli_wrapper = base == mlc::String("main") ? mlc::String("\n#undef main\n\nstatic void mlc_cli_set_args(int argc, char** argv) {\n  std::vector<mlc::String> arguments;\n  arguments.reserve(argc > 0 ? argc - 1 : 0);\n  for (int i = 1; i < argc; ++i) { arguments.emplace_back(argv[i]); }\n  mlc::io::set_args(std::move(arguments));\n}\n\nint main(int argc, char** argv) {\n  mlc_cli_set_args(argc, argv);\n  return ::mlc_main::mlc_user_main(argc, argv);\n}\n") : mlc::String("");
mlc::String impl = mlc::Array<mlc::String>{mlc::String("#define main mlc_user_main\n"), mlc::String("#include \"") + base + mlc::String(".hpp\"\n"), mlc::String("\n"), mlc::String("namespace ") + ns + mlc::String(" {\n"), mlc::String("\n"), fn_defs.join(mlc::String("")), mlc::String("\n"), mlc::String("} // namespace ") + ns + mlc::String("\n"), cli_wrapper}.join(mlc::String(""));
return codegen::GenModuleOut{header, impl};
}

} // namespace codegen
