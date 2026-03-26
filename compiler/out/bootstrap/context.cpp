#include "context.hpp"

#include "ast.hpp"

namespace context {

using namespace ast;
using namespace ast_tokens;

mlc::String path_to_module_base(mlc::String path) noexcept;

mlc::Array<mlc::String> decl_export_names(std::shared_ptr<ast::Decl> decl) noexcept;

mlc::HashMap<mlc::String, mlc::String> add_exports_to_qualified(mlc::HashMap<mlc::String, mlc::String> qualified, mlc::String imp_path, mlc::Array<context::LoadItem> all_items) noexcept;

mlc::HashMap<mlc::String, mlc::String> build_qualified(mlc::Array<mlc::String> import_paths, mlc::Array<context::LoadItem> all_items) noexcept;

bool cpp_keyword(mlc::String name) noexcept;

mlc::String cpp_safe(mlc::String name) noexcept;

mlc::String lower_first(mlc::String name) noexcept;

mlc::String map_method(mlc::String method_name) noexcept;

mlc::String map_builtin(mlc::String name) noexcept;

mlc::String escape_str(mlc::String input) noexcept;

mlc::String template_prefix(mlc::Array<mlc::String> type_params) noexcept;

mlc::String include_lines(mlc::Array<mlc::String> import_paths) noexcept;

mlc::String context_resolve(context::CodegenContext context, mlc::String name) noexcept;

mlc::Array<std::shared_ptr<context::FieldOrder>> build_field_orders(ast::Program prog) noexcept;

bool list_contains(mlc::Array<mlc::String> list, mlc::String item) noexcept;

mlc::Array<mlc::String> lookup_fields(mlc::Array<std::shared_ptr<context::FieldOrder>> orders, mlc::String name) noexcept;

mlc::String extract_method_name(mlc::String fn_name, mlc::String type_name) noexcept;

mlc::HashMap<mlc::String, mlc::String> add_method_owners_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls, mlc::HashMap<mlc::String, mlc::String> owners) noexcept;

mlc::HashMap<mlc::String, mlc::String> build_method_owners_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<context::LoadItem>> build_item_index(mlc::Array<context::LoadItem> all_items) noexcept;

mlc::Array<mlc::String> build_generic_variants_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept;

mlc::HashMap<mlc::String, mlc::String> build_variant_types_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept;

bool is_shared_type(std::shared_ptr<ast::TypeExpr> t) noexcept;

bool is_shared_array_type(std::shared_ptr<ast::TypeExpr> t) noexcept;

bool is_shared_map_type(std::shared_ptr<ast::TypeExpr> t) noexcept;

mlc::String array_elem_name(std::shared_ptr<ast::TypeExpr> t) noexcept;

mlc::Array<mlc::String> collect_shared_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;

mlc::Array<mlc::String> collect_shared_array_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;

mlc::HashMap<mlc::String, mlc::String> collect_array_elem_types(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;

mlc::Array<mlc::String> collect_shared_map_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;

mlc::Array<mlc::String> collect_value_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;

std::shared_ptr<ast::TypeExpr> field_def_type(std::shared_ptr<ast::FieldDef> fd) noexcept;

std::shared_ptr<context::CtorTypeInfo> ctor_type_info_for(mlc::String ctor_name, mlc::Array<std::shared_ptr<ast::TypeExpr>> types) noexcept;

mlc::Array<std::shared_ptr<context::CtorTypeInfo>> add_ctor_infos_from_variants(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::Array<std::shared_ptr<context::CtorTypeInfo>> build_ctor_type_infos_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept;

std::shared_ptr<context::CtorTypeInfo> lookup_ctor_type_info(mlc::Array<std::shared_ptr<context::CtorTypeInfo>> infos, mlc::String cti_name) noexcept;

context::CodegenContext make_body_context(context::CodegenContext base, mlc::Array<mlc::String> shared_params, mlc::Array<mlc::String> shared_array_params, mlc::HashMap<mlc::String, mlc::String> array_elem_types, mlc::Array<mlc::String> shared_map_params, mlc::String self_type, mlc::Array<mlc::String> value_params, mlc::Array<mlc::String> match_deref_params) noexcept;

context::CodegenContext context_add_shared(context::CodegenContext context, mlc::String name) noexcept;

context::CodegenContext context_add_shared_array(context::CodegenContext context, mlc::String name) noexcept;

context::CodegenContext context_add_value(context::CodegenContext context, mlc::String name) noexcept;

context::CodegenContext context_add_match_deref(context::CodegenContext context, mlc::String name) noexcept;

bool expr_returns_shared_sum_visit_ptr(std::shared_ptr<ast::Expr> e) noexcept;

context::CodegenContext update_context_from_statement(std::shared_ptr<ast::Stmt> stmt, context::CodegenContext context) noexcept;

context::CodegenContext create_codegen_context(ast::Program prog) noexcept;

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

mlc::HashMap<mlc::String, mlc::String> add_exports_to_qualified(mlc::HashMap<mlc::String, mlc::String> qualified, mlc::String imp_path, mlc::Array<context::LoadItem> all_items) noexcept{
mlc::String prefix = path_to_module_base(imp_path) + mlc::String("::");
int item_i = 0;
while (item_i < all_items.size()){
{
context::LoadItem cur_item = all_items[item_i];
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

mlc::HashMap<mlc::String, mlc::String> build_qualified(mlc::Array<mlc::String> import_paths, mlc::Array<context::LoadItem> all_items) noexcept{
mlc::HashMap<mlc::String, mlc::String> qualified = mlc::HashMap<mlc::String, mlc::String>();
int imp_i = 0;
while (imp_i < import_paths.size()){
{
mlc::String imp_path = import_paths[imp_i];
qualified = add_exports_to_qualified(qualified, imp_path, all_items);
int item_i = 0;
while (item_i < all_items.size()){
{
context::LoadItem cur_item = all_items[item_i];
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

bool cpp_keyword(mlc::String name) noexcept{return name == mlc::String("export") || name == mlc::String("import") || name == mlc::String("module") || name == mlc::String("struct") || name == mlc::String("class") || name == mlc::String("template") || name == mlc::String("typename") || name == mlc::String("const") || name == mlc::String("return") || name == mlc::String("if") || name == mlc::String("else") || name == mlc::String("for") || name == mlc::String("while") || name == mlc::String("do") || name == mlc::String("switch") || name == mlc::String("case") || name == mlc::String("default") || name == mlc::String("break") || name == mlc::String("continue") || name == mlc::String("try") || name == mlc::String("catch") || name == mlc::String("throw") || name == mlc::String("new") || name == mlc::String("delete") || name == mlc::String("virtual") || name == mlc::String("override") || name == mlc::String("final") || name == mlc::String("public") || name == mlc::String("private") || name == mlc::String("protected") || name == mlc::String("friend") || name == mlc::String("static") || name == mlc::String("extern") || name == mlc::String("inline") || name == mlc::String("using") || name == mlc::String("namespace") || name == mlc::String("typedef") || name == mlc::String("enum") || name == mlc::String("union") || name == mlc::String("true") || name == mlc::String("false") || name == mlc::String("nullptr") || name == mlc::String("void") || name == mlc::String("bool") || name == mlc::String("int") || name == mlc::String("long") || name == mlc::String("short") || name == mlc::String("char") || name == mlc::String("float") || name == mlc::String("double") || name == mlc::String("signed") || name == mlc::String("unsigned") || name == mlc::String("auto") || name == mlc::String("typeid") || name == mlc::String("sizeof") || name == mlc::String("and") || name == mlc::String("or") || name == mlc::String("not") || name == mlc::String("xor") || name == mlc::String("and_eq") || name == mlc::String("or_eq") || name == mlc::String("not_eq") || name == mlc::String("xor_eq") || name == mlc::String("bitand") || name == mlc::String("bitor") || name == mlc::String("compl") || name == mlc::String("alignas") || name == mlc::String("alignof") || name == mlc::String("asm") || name == mlc::String("concept") || name == mlc::String("consteval") || name == mlc::String("constexpr") || name == mlc::String("constinit") || name == mlc::String("const_cast") || name == mlc::String("decltype") || name == mlc::String("dynamic_cast") || name == mlc::String("explicit") || name == mlc::String("mutable") || name == mlc::String("noexcept") || name == mlc::String("operator") || name == mlc::String("reinterpret_cast") || name == mlc::String("requires") || name == mlc::String("static_cast") || name == mlc::String("this") || name == mlc::String("thread_local") || name == mlc::String("volatile") || name == mlc::String("wchar_t");}

mlc::String cpp_safe(mlc::String name) noexcept{return cpp_keyword(name) ? name + mlc::String("_") : name;}

mlc::String lower_first(mlc::String name) noexcept{return name.length() == 0 ? name : name.char_at(0).to_lower() + name.substring(1, name.length());}

mlc::String map_method(mlc::String method_name) noexcept{return method_name == mlc::String("length") || method_name == mlc::String("len") ? mlc::String("length") : method_name == mlc::String("push") ? mlc::String("push_back") : method_name == mlc::String("to_string") ? mlc::String("to_string") : method_name == mlc::String("to_int") || method_name == mlc::String("to_i") ? mlc::String("to_i") : method_name == mlc::String("upper") || method_name == mlc::String("to_upper") ? mlc::String("upper") : method_name == mlc::String("lower") || method_name == mlc::String("to_lower") ? mlc::String("lower") : method_name;}

mlc::String map_builtin(mlc::String name) noexcept{return name == mlc::String("print") ? mlc::String("mlc::io::print") : name == mlc::String("println") ? mlc::String("mlc::io::println") : name == mlc::String("exit") ? mlc::String("mlc::io::exit") : name == mlc::String("args") ? mlc::String("mlc::io::args") : name;}

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

mlc::String template_prefix(mlc::Array<mlc::String> type_params) noexcept{return type_params.size() > 0 ? mlc::String("template<typename ") + type_params.join(mlc::String(", typename ")) + mlc::String(">\n") : mlc::String("");}

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

mlc::String context_resolve(context::CodegenContext context, mlc::String name) noexcept{return context.qualified.has(name) ? context.qualified.get(name) + cpp_safe(name) : context.namespace_prefix.length() > 0 ? context.namespace_prefix + cpp_safe(name) : cpp_safe(name);}

mlc::Array<std::shared_ptr<context::FieldOrder>> build_field_orders(ast::Program prog) noexcept{
mlc::Array<std::shared_ptr<context::FieldOrder>> orders = {};
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
  orders.push_back(std::make_shared<context::FieldOrder>(context::FieldOrder{variant_name, field_names}));
  if (variants.size() == 1){
{
orders.push_back(std::make_shared<context::FieldOrder>(context::FieldOrder{type_name, field_names}));
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
  orders.push_back(std::make_shared<context::FieldOrder>(context::FieldOrder{variant_name, field_names}));
  if (variants.size() == 1){
{
orders.push_back(std::make_shared<context::FieldOrder>(context::FieldOrder{type_name, field_names}));
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

mlc::Array<mlc::String> lookup_fields(mlc::Array<std::shared_ptr<context::FieldOrder>> orders, mlc::String name) noexcept{
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

mlc::HashMap<mlc::String, mlc::String> build_method_owners_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept{return add_method_owners_from_decls(decls, mlc::HashMap<mlc::String, mlc::String>());}

mlc::HashMap<mlc::String, std::shared_ptr<context::LoadItem>> build_item_index(mlc::Array<context::LoadItem> all_items) noexcept{
mlc::HashMap<mlc::String, std::shared_ptr<context::LoadItem>> item_index = mlc::HashMap<mlc::String, std::shared_ptr<context::LoadItem>>();
int i = 0;
while (i < all_items.size()){
{
item_index.set(all_items[i].path, std::make_shared<context::LoadItem>(all_items[i]));
i = i + 1;
}
}
return item_index;
}

mlc::Array<mlc::String> build_generic_variants_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept{
mlc::Array<mlc::String> result = {};
int i = 0;
while (i < decls.size()){
{
std::shared_ptr<ast::Decl> inner_decl = ast::decl_inner(decls[i]);
[&]() -> void { if (std::holds_alternative<ast::DeclType>((*inner_decl))) { auto _v_decltype = std::get<ast::DeclType>((*inner_decl)); auto [_w0, type_params, variants] = _v_decltype; return [&]() { 
  int vi = 0;
  return [&]() { 
  while (vi < variants.size()){
{
bool should_add = type_params.size() > 0;
mlc::String vname = std::visit(overloaded{
  [&](const VarRecord& varrecord) -> mlc::String { auto [n, _w0] = varrecord; return n; },
  [&](const VarTuple& vartuple) -> mlc::String { auto [n, _w0] = vartuple; return n; },
  [&](const VarUnit& varunit) -> mlc::String { auto [n] = varunit; return n; }
}, (*variants[vi]));
if (should_add){
{
result.push_back(vname);
}
} else {
{
result.push_back(mlc::String("__skip__"));
}
}
vi = vi + 1;
}
}
 }();
 }(); } return; }();
i = i + 1;
}
}
return result;
}

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

std::shared_ptr<ast::TypeExpr> field_def_type(std::shared_ptr<ast::FieldDef> fd) noexcept{return fd->typ;}

std::shared_ptr<context::CtorTypeInfo> ctor_type_info_for(mlc::String ctor_name, mlc::Array<std::shared_ptr<ast::TypeExpr>> types) noexcept{
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
return std::make_shared<context::CtorTypeInfo>(context::CtorTypeInfo{ctor_name, shared_positions, shared_array_positions});
}

mlc::Array<std::shared_ptr<context::CtorTypeInfo>> add_ctor_infos_from_variants(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
mlc::Array<std::shared_ptr<context::CtorTypeInfo>> result = {};
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

mlc::Array<std::shared_ptr<context::CtorTypeInfo>> build_ctor_type_infos_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept{
mlc::Array<std::shared_ptr<context::CtorTypeInfo>> result = {};
int i = 0;
while (i < decls.size()){
{
[&]() -> void { if (std::holds_alternative<ast::DeclExported>((*decls[i]))) { auto _v_declexported = std::get<ast::DeclExported>((*decls[i])); auto [inner] = _v_declexported; return [&]() -> void { if (std::holds_alternative<ast::DeclType>((*ast::decl_inner(inner)))) { auto _v_decltype = std::get<ast::DeclType>((*ast::decl_inner(inner))); auto [_w0, _w1, variants] = _v_decltype; return [&]() { 
  mlc::Array<std::shared_ptr<context::CtorTypeInfo>> infos = add_ctor_infos_from_variants(variants);
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
  mlc::Array<std::shared_ptr<context::CtorTypeInfo>> infos = add_ctor_infos_from_variants(variants);
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

std::shared_ptr<context::CtorTypeInfo> lookup_ctor_type_info(mlc::Array<std::shared_ptr<context::CtorTypeInfo>> infos, mlc::String cti_name) noexcept{
std::shared_ptr<context::CtorTypeInfo> result = std::make_shared<context::CtorTypeInfo>(context::CtorTypeInfo{mlc::String(""), {}, {}});
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

context::CodegenContext make_body_context(context::CodegenContext base, mlc::Array<mlc::String> shared_params, mlc::Array<mlc::String> shared_array_params, mlc::HashMap<mlc::String, mlc::String> array_elem_types, mlc::Array<mlc::String> shared_map_params, mlc::String self_type, mlc::Array<mlc::String> value_params, mlc::Array<mlc::String> match_deref_params) noexcept{return context::CodegenContext{base.field_orders, base.namespace_prefix, base.qualified, self_type, base.method_owners, shared_params, shared_array_params, array_elem_types, shared_map_params, base.ctor_type_infos, base.variant_types, value_params, match_deref_params, base.generic_variants};}

context::CodegenContext context_add_shared(context::CodegenContext context, mlc::String name) noexcept{
mlc::Array<mlc::String> new_shared_params = context.shared_params;
new_shared_params.push_back(name);
return make_body_context(context, new_shared_params, context.shared_array_params, context.array_elem_types, context.shared_map_params, context.self_type, context.value_params, context.match_deref_params);
}

context::CodegenContext context_add_shared_array(context::CodegenContext context, mlc::String name) noexcept{
mlc::Array<mlc::String> new_shared_array_params = context.shared_array_params;
new_shared_array_params.push_back(name);
return make_body_context(context, context.shared_params, new_shared_array_params, context.array_elem_types, context.shared_map_params, context.self_type, context.value_params, context.match_deref_params);
}

context::CodegenContext context_add_value(context::CodegenContext context, mlc::String name) noexcept{
mlc::Array<mlc::String> new_value_params = context.value_params;
new_value_params.push_back(name);
return make_body_context(context, context.shared_params, context.shared_array_params, context.array_elem_types, context.shared_map_params, context.self_type, new_value_params, context.match_deref_params);
}

context::CodegenContext context_add_match_deref(context::CodegenContext context, mlc::String name) noexcept{
mlc::Array<mlc::String> md = context.match_deref_params;
md.push_back(name);
return make_body_context(context, context.shared_params, context.shared_array_params, context.array_elem_types, context.shared_map_params, context.self_type, context.value_params, md);
}

bool expr_returns_shared_sum_visit_ptr(std::shared_ptr<ast::Expr> e) noexcept{return [&]() { if (std::holds_alternative<ast::ExprCall>((*e)._)) { auto _v_exprcall = std::get<ast::ExprCall>((*e)._); auto [func, _w0] = _v_exprcall; return [&]() { if (std::holds_alternative<ast::ExprIdent>((*func)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*func)._); auto [n] = _v_exprident; return n == mlc::String("decl_inner") || n == mlc::String("find_field_val") || n == mlc::String("param_typ"); } return false; }(); } return false; }();}

context::CodegenContext update_context_from_statement(std::shared_ptr<ast::Stmt> stmt, context::CodegenContext context) noexcept{return [&]() -> context::CodegenContext { if (std::holds_alternative<ast::StmtLet>((*stmt)._)) { auto _v_stmtlet = std::get<ast::StmtLet>((*stmt)._); auto [binding_name, _w0, typ, value] = _v_stmtlet; return is_shared_type(typ) ? context_add_shared(context, binding_name) : is_shared_array_type(typ) ? context_add_shared_array(context, binding_name) : [&]() -> context::CodegenContext { if (std::holds_alternative<ast::ExprMethod>((*value)._)) { auto _v_exprmethod = std::get<ast::ExprMethod>((*value)._); auto [map_obj, method_name, _w0] = _v_exprmethod; return method_name == mlc::String("get") ? [&]() -> context::CodegenContext { if (std::holds_alternative<ast::ExprIdent>((*map_obj)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*map_obj)._); auto [map_name] = _v_exprident; return context.shared_map_params.contains(map_name) ? context_add_shared(context, binding_name) : context_add_value(context, binding_name); } return context_add_value(context, binding_name); }() : [&]() -> context::CodegenContext { 
  context::CodegenContext vctx = context_add_value(context, binding_name);
  return expr_returns_shared_sum_visit_ptr(value) ? context_add_match_deref(vctx, binding_name) : vctx;
 }(); } return [&]() -> context::CodegenContext { 
  context::CodegenContext vctx = context_add_value(context, binding_name);
  return expr_returns_shared_sum_visit_ptr(value) ? context_add_match_deref(vctx, binding_name) : vctx;
 }(); }(); } return context; }();}

context::CodegenContext create_codegen_context(ast::Program prog) noexcept{return context::CodegenContext{build_field_orders(prog), mlc::String(""), mlc::HashMap<mlc::String, mlc::String>(), mlc::String(""), build_method_owners_from_decls(prog.decls), {}, {}, mlc::HashMap<mlc::String, mlc::String>(), {}, {}, build_variant_types_from_decls(prog.decls), {}, {}, build_generic_variants_from_decls(prog.decls)};}

} // namespace context
