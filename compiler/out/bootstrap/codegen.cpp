#define main mlc_user_main
#include "codegen.hpp"

namespace codegen {

mlc::String path_to_module_base(mlc::String path) noexcept{
auto last_slash = (-1);
auto last_dot = path.length();
auto i = 0;
while ((i < path.length())) {
if ((path.char_at(i) == mlc::String("/", 1))) {
last_slash = i;
} else if (((path.char_at(i) == mlc::String(".", 1)) && (i > last_slash))) {
last_dot = i;
}

i = (i + 1);
}
return ((last_dot > last_slash) ? path.substring((last_slash + 1), ((last_dot - last_slash) - 1)) : path.substring((last_slash + 1), ((path.length() - last_slash) - 1)));
}
mlc::Array<mlc::String> decl_export_names(std::shared_ptr<ast::Decl> decl) noexcept{
auto names = mlc::Array<mlc::String>{};
std::visit(overloaded{[&](const ast::DeclExported& declExported) { auto [d] = declExported; return [&]() {
std::visit(overloaded{[&](const ast::DeclFn& declFn) { auto [name, __1, __2, __3, __4, __5] = declFn; return [&]() {
names.push_back(name);
/* unit */;
return /* unit */;
}(); },
[&](const ast::DeclType& declType) { auto [type_name, __1, variants] = declType; return [&]() {
names.push_back(type_name);
auto variant_index = 0;
while ((variant_index < variants.length())) {
std::visit(overloaded{[&](const ast::VarRecord& varRecord) { auto [variant_name, __1] = varRecord; return [&]() {
names.push_back(variant_name);
/* unit */;
return /* unit */;
}(); },
[&](const ast::VarTuple& varTuple) { auto [variant_name, __1] = varTuple; return [&]() {
names.push_back(variant_name);
/* unit */;
return /* unit */;
}(); },
[&](const ast::VarUnit& varUnit) { auto [variant_name] = varUnit; return [&]() {
names.push_back(variant_name);
/* unit */;
return /* unit */;
}(); }
}, (*variants[variant_index]));
variant_index = (variant_index + 1);
}
/* unit */;
return /* unit */;
}(); },
[&](const ast::DeclTrait& declTrait) { auto [name, __1, methods] = declTrait; return [&]() {
names.push_back(name);
auto mi = 0;
while ((mi < methods.length())) {
std::visit(overloaded{[&](const ast::DeclFn& declFn) { auto [fn_name, __1, __2, __3, __4, __5] = declFn; return [&]() {
names.push_back(fn_name);
/* unit */;
return /* unit */;
}(); },
[&](const auto& __v) { return [&]() {
/* unit */;
return /* unit */;
}(); }
}, (*methods[mi]));
mi = (mi + 1);
}
/* unit */;
return /* unit */;
}(); },
[&](const ast::DeclExtend& declExtend) { auto [__0, __1, methods] = declExtend; return [&]() {
auto mi = 0;
while ((mi < methods.length())) {
std::visit(overloaded{[&](const ast::DeclFn& declFn) { auto [fn_name, __1, __2, __3, __4, __5] = declFn; return [&]() {
names.push_back(fn_name);
/* unit */;
return /* unit */;
}(); },
[&](const auto& __v) { return [&]() {
/* unit */;
return /* unit */;
}(); }
}, (*methods[mi]));
mi = (mi + 1);
}
/* unit */;
return /* unit */;
}(); },
[&](const auto& __v) { return [&]() {
/* unit */;
return /* unit */;
}(); }
}, (*d));
/* unit */;
return /* unit */;
}(); },
[&](const auto& __v) { return [&]() {
/* unit */;
return /* unit */;
}(); }
}, (*decl));
return names;
}
mlc::HashMap<mlc::String, mlc::String> add_exports_to_qualified(mlc::HashMap<mlc::String, mlc::String> qualified, mlc::String imp_path, mlc::Array<LoadItem> all_items) noexcept{
auto prefix = (path_to_module_base(imp_path) + mlc::String("::", 2));
auto item_i = 0;
while ((item_i < all_items.length())) {
auto cur_item = all_items[item_i];
if ((cur_item.path == imp_path)) {
auto decl_index = 0;
[&]() {
while ((decl_index < cur_item.decls.length())) {
auto name_index = 0;
auto exp_names = decl_export_names(cur_item.decls[decl_index]);
while ((name_index < exp_names.length())) {
if ((!qualified.has(exp_names[name_index]))) {
qualified.set(exp_names[name_index], prefix);
}
name_index = (name_index + 1);
}
decl_index = (decl_index + 1);
}
}();
}
item_i = (item_i + 1);
}
return qualified;
}
mlc::HashMap<mlc::String, mlc::String> build_qualified(mlc::Array<mlc::String> import_paths, mlc::Array<LoadItem> all_items) noexcept{
auto qualified = mlc::HashMap<mlc::String, mlc::String>();
auto imp_i = 0;
while ((imp_i < import_paths.length())) {
auto imp_path = import_paths[imp_i];
qualified = add_exports_to_qualified(qualified, imp_path, all_items);
auto item_i = 0;
while ((item_i < all_items.length())) {
auto cur_item = all_items[item_i];
if ((cur_item.path == imp_path)) {
auto trans_i = 0;
[&]() {
while ((trans_i < cur_item.imports.length())) {
qualified = add_exports_to_qualified(qualified, cur_item.imports[trans_i], all_items);
trans_i = (trans_i + 1);
}
}();
}
item_i = (item_i + 1);
}
imp_i = (imp_i + 1);
}
return qualified;
}
mlc::String extract_method_name(mlc::String fn_name, mlc::String type_name) noexcept{
auto prefix = (type_name + mlc::String("_", 1));
return (((fn_name.length() > prefix.length()) && (fn_name.substring(0, prefix.length()) == prefix)) ? fn_name.substring(prefix.length(), (fn_name.length() - prefix.length())) : fn_name);
}
mlc::HashMap<mlc::String, mlc::String> add_method_owners_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls, mlc::HashMap<mlc::String, mlc::String> owners) noexcept{
auto i = 0;
while ((i < decls.length())) {
std::visit(overloaded{[&](const ast::DeclTrait& declTrait) { auto [trait_name, __1, methods] = declTrait; return [&]() {
auto mi = 0;
return [&]() {
while ((mi < methods.length())) {
std::visit(overloaded{[&](const ast::DeclFn& declFn) { auto [fn_name, __1, __2, __3, __4, __5] = declFn; return [&]() {
auto unmangled = extract_method_name(fn_name, trait_name);
if ((!owners.has(unmangled))) {
owners.set(unmangled, fn_name);
}
return /* unit */;
}(); },
[&](const auto& __v) { return /* unit */; }
}, (*methods[mi]));
mi = (mi + 1);
}
}();
}(); },
[&](const ast::DeclExtend& declExtend) { auto [type_name, __1, methods] = declExtend; return [&]() {
auto mi = 0;
return [&]() {
while ((mi < methods.length())) {
std::visit(overloaded{[&](const ast::DeclFn& declFn) { auto [fn_name, __1, __2, __3, __4, __5] = declFn; return [&]() {
auto unmangled = extract_method_name(fn_name, type_name);
if ((!owners.has(unmangled))) {
owners.set(unmangled, fn_name);
}
return /* unit */;
}(); },
[&](const auto& __v) { return /* unit */; }
}, (*methods[mi]));
mi = (mi + 1);
}
}();
}(); },
[&](const auto& __v) { return /* unit */; }
}, (*ast::decl_inner(decls[i])));
i = (i + 1);
}
return owners;
}
mlc::HashMap<mlc::String, std::shared_ptr<LoadItem>> build_item_index(mlc::Array<LoadItem> all_items) noexcept{
auto item_index = mlc::HashMap<mlc::String, std::shared_ptr<LoadItem>>();
auto i = 0;
while ((i < all_items.length())) {
item_index.set(all_items[i].path, std::make_shared<LoadItem>(all_items[i]));
i = (i + 1);
}
return item_index;
}
mlc::HashMap<mlc::String, mlc::String> build_method_owners_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept{
return add_method_owners_from_decls(decls, {});
}
mlc::HashMap<mlc::String, mlc::String> build_variant_types_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept{
auto variant_map = mlc::HashMap<mlc::String, mlc::String>();
auto i = 0;
while ((i < decls.length())) {
auto inner_decl = ast::decl_inner(decls[i]);
std::visit(overloaded{[&](const ast::DeclType& declType) { auto [type_name, __1, variants] = declType; return [&]() {
auto vi = 0;
return [&]() {
while ((vi < variants.length())) {
std::visit(overloaded{[&](const ast::VarRecord& varRecord) { auto [variant_name, __1] = varRecord; return variant_map.set(variant_name, type_name); },
[&](const ast::VarTuple& varTuple) { auto [variant_name, __1] = varTuple; return variant_map.set(variant_name, type_name); },
[&](const ast::VarUnit& varUnit) { auto [variant_name] = varUnit; return variant_map.set(variant_name, type_name); }
}, (*variants[vi]));
vi = (vi + 1);
}
}();
}(); },
[&](const auto& __v) { return /* unit */; }
}, (*inner_decl));
i = (i + 1);
}
return variant_map;
}
CodegenContext create_codegen_context(ast::Program prog) noexcept{
return CodegenContext{build_field_orders(prog), mlc::String("", 0), {}, mlc::String("", 0), build_method_owners_from_decls(prog.decls), {}, {}, {}, {}, {}, build_variant_types_from_decls(prog.decls), {}};
}
mlc::String context_resolve(CodegenContext context, mlc::String name) noexcept{
if (context.qualified.has(name)) {
return (context.qualified.get(name) + cpp_safe(name));
} else if ((context.namespace_prefix.length() > 0)) {
return (context.namespace_prefix + cpp_safe(name));
} else {
return cpp_safe(name);
}

}
mlc::Array<std::shared_ptr<FieldOrder>> build_field_orders(ast::Program prog) noexcept{
auto orders = mlc::Array<std::shared_ptr<FieldOrder>>{};
auto i = 0;
while ((i < prog.decls.length())) {
std::visit(overloaded{[&](const ast::DeclExported& declExported) { auto [inner] = declExported; return std::visit(overloaded{[&](const ast::DeclType& declType) { auto [type_name, __1, variants] = declType; return [&]() {
auto vi = 0;
return [&]() {
while ((vi < variants.length())) {
std::visit(overloaded{[&](const ast::VarRecord& varRecord) { auto [variant_name, field_defs] = varRecord; return [&]() {
auto field_names = mlc::Array<mlc::String>{};
auto fi = 0;
while ((fi < field_defs.length())) {
field_names.push_back(field_defs[fi]->name);
fi = (fi + 1);
}
orders.push_back(std::make_shared<FieldOrder>(FieldOrder{variant_name, field_names}));
if ((variants.length() == 1)) {
orders.push_back(std::make_shared<FieldOrder>(FieldOrder{type_name, field_names}));
}
return /* unit */;
}(); },
[&](const ast::VarTuple& varTuple) { auto [__0, __1] = varTuple; return /* unit */; },
[&](const ast::VarUnit& varUnit) { auto [__0] = varUnit; return /* unit */; }
}, (*variants[vi]));
vi = (vi + 1);
}
}();
}(); },
[&](const ast::DeclFn& declFn) { auto [__0, __1, __2, __3, __4, __5] = declFn; return /* unit */; },
[&](const ast::DeclTrait& declTrait) { auto [__0, __1, __2] = declTrait; return /* unit */; },
[&](const ast::DeclExtend& declExtend) { auto [__0, __1, __2] = declExtend; return /* unit */; },
[&](const ast::DeclImport& declImport) { auto [__0, __1] = declImport; return /* unit */; },
[&](const ast::DeclExported& declExported) { auto [__0] = declExported; return /* unit */; }
}, (*ast::decl_inner(inner))); },
[&](const ast::DeclType& declType) { auto [type_name, __1, variants] = declType; return [&]() {
auto vi = 0;
return [&]() {
while ((vi < variants.length())) {
std::visit(overloaded{[&](const ast::VarRecord& varRecord) { auto [variant_name, field_defs] = varRecord; return [&]() {
auto field_names = mlc::Array<mlc::String>{};
auto fi = 0;
while ((fi < field_defs.length())) {
field_names.push_back(field_defs[fi]->name);
fi = (fi + 1);
}
orders.push_back(std::make_shared<FieldOrder>(FieldOrder{variant_name, field_names}));
if ((variants.length() == 1)) {
orders.push_back(std::make_shared<FieldOrder>(FieldOrder{type_name, field_names}));
}
return /* unit */;
}(); },
[&](const ast::VarTuple& varTuple) { auto [__0, __1] = varTuple; return /* unit */; },
[&](const ast::VarUnit& varUnit) { auto [__0] = varUnit; return /* unit */; }
}, (*variants[vi]));
vi = (vi + 1);
}
}();
}(); },
[&](const ast::DeclFn& declFn) { auto [__0, __1, __2, __3, __4, __5] = declFn; return /* unit */; },
[&](const ast::DeclTrait& declTrait) { auto [__0, __1, __2] = declTrait; return /* unit */; },
[&](const ast::DeclExtend& declExtend) { auto [__0, __1, __2] = declExtend; return /* unit */; },
[&](const ast::DeclImport& declImport) { auto [__0, __1] = declImport; return /* unit */; }
}, (*prog.decls[i]));
i = (i + 1);
}
return orders;
}
bool list_contains(mlc::Array<mlc::String> list, mlc::String item) noexcept{
auto found = false;
auto i = 0;
while (((i < list.length()) && (!found))) {
if ((list[i] == item)) {
found = true;
}
i = (i + 1);
}
return found;
}
mlc::Array<mlc::String> lookup_fields(mlc::Array<std::shared_ptr<FieldOrder>> orders, mlc::String name) noexcept{
auto result = mlc::Array<mlc::String>{};
auto i = 0;
auto found = false;
while (((i < orders.length()) && (!found))) {
if ((orders[i]->type_name == name)) {
result = orders[i]->fields;
found = true;
}
i = (i + 1);
}
return result;
}
bool cpp_keyword(mlc::String name) noexcept{
return (((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((name == mlc::String("export", 6)) || (name == mlc::String("import", 6))) || (name == mlc::String("module", 6))) || (name == mlc::String("struct", 6))) || (name == mlc::String("class", 5))) || (name == mlc::String("template", 8))) || (name == mlc::String("typename", 8))) || (name == mlc::String("const", 5))) || (name == mlc::String("return", 6))) || (name == mlc::String("if", 2))) || (name == mlc::String("else", 4))) || (name == mlc::String("for", 3))) || (name == mlc::String("while", 5))) || (name == mlc::String("do", 2))) || (name == mlc::String("switch", 6))) || (name == mlc::String("case", 4))) || (name == mlc::String("default", 7))) || (name == mlc::String("break", 5))) || (name == mlc::String("continue", 8))) || (name == mlc::String("try", 3))) || (name == mlc::String("catch", 5))) || (name == mlc::String("throw", 5))) || (name == mlc::String("new", 3))) || (name == mlc::String("delete", 6))) || (name == mlc::String("virtual", 7))) || (name == mlc::String("override", 8))) || (name == mlc::String("final", 5))) || (name == mlc::String("public", 6))) || (name == mlc::String("private", 7))) || (name == mlc::String("protected", 9))) || (name == mlc::String("friend", 6))) || (name == mlc::String("static", 6))) || (name == mlc::String("extern", 6))) || (name == mlc::String("inline", 6))) || (name == mlc::String("using", 5))) || (name == mlc::String("namespace", 9))) || (name == mlc::String("typedef", 7))) || (name == mlc::String("enum", 4))) || (name == mlc::String("union", 5))) || (name == mlc::String("true", 4))) || (name == mlc::String("false", 5))) || (name == mlc::String("nullptr", 7))) || (name == mlc::String("void", 4))) || (name == mlc::String("bool", 4))) || (name == mlc::String("int", 3))) || (name == mlc::String("long", 4))) || (name == mlc::String("short", 5))) || (name == mlc::String("char", 4))) || (name == mlc::String("float", 5))) || (name == mlc::String("double", 6))) || (name == mlc::String("signed", 6))) || (name == mlc::String("unsigned", 8))) || (name == mlc::String("auto", 4))) || (name == mlc::String("typeid", 6))) || (name == mlc::String("sizeof", 6))) || (name == mlc::String("and", 3))) || (name == mlc::String("or", 2))) || (name == mlc::String("not", 3))) || (name == mlc::String("xor", 3))) || (name == mlc::String("and_eq", 6))) || (name == mlc::String("or_eq", 5))) || (name == mlc::String("not_eq", 6))) || (name == mlc::String("xor_eq", 6))) || (name == mlc::String("bitand", 6))) || (name == mlc::String("bitor", 5))) || (name == mlc::String("compl", 5))) || (name == mlc::String("alignas", 7))) || (name == mlc::String("alignof", 7))) || (name == mlc::String("asm", 3))) || (name == mlc::String("concept", 7))) || (name == mlc::String("consteval", 9))) || (name == mlc::String("constexpr", 9))) || (name == mlc::String("constinit", 9))) || (name == mlc::String("const_cast", 10))) || (name == mlc::String("decltype", 8))) || (name == mlc::String("dynamic_cast", 12))) || (name == mlc::String("explicit", 8))) || (name == mlc::String("mutable", 7))) || (name == mlc::String("noexcept", 8))) || (name == mlc::String("operator", 8))) || (name == mlc::String("reinterpret_cast", 16))) || (name == mlc::String("requires", 8))) || (name == mlc::String("static_cast", 11))) || (name == mlc::String("this", 4))) || (name == mlc::String("thread_local", 12))) || (name == mlc::String("volatile", 8))) || (name == mlc::String("wchar_t", 7)));
}
mlc::String cpp_safe(mlc::String name) noexcept{
if (cpp_keyword(name)) {
return (name + mlc::String("_", 1));
} else {
return name;
}
}
mlc::String type_name_to_cpp(CodegenContext context, mlc::String type_name) noexcept{
if ((type_name == mlc::String("i32", 3))) {
return mlc::String("int", 3);
} else if ((type_name == mlc::String("string", 6))) {
return mlc::String("mlc::String", 11);
} else if ((type_name == mlc::String("bool", 4))) {
return mlc::String("bool", 4);
} else if ((type_name == mlc::String("unit", 4))) {
return mlc::String("void", 4);
} else if (((type_name == mlc::String("Self", 4)) || (type_name == mlc::String("self", 4)))) {
return ((context.self_type.length() > 0) ? type_name_to_cpp(context, context.self_type) : mlc::String("void", 4));
} else {
return context_resolve(context, type_name);
}




}
mlc::String lower_first(mlc::String name) noexcept{
if ((name.length() == 0)) {
return name;
} else {
return (name.char_at(0).lower() + name.substring(1, name.length()));
}
}
mlc::String type_to_cpp(CodegenContext context, std::shared_ptr<ast::TypeExpr> type_expr) noexcept{
return std::visit(overloaded{[&](const ast::TyI32& tyI32) { return mlc::String("int", 3); },
[&](const ast::TyString& tyString) { return mlc::String("mlc::String", 11); },
[&](const ast::TyBool& tyBool) { return mlc::String("bool", 4); },
[&](const ast::TyUnit& tyUnit) { return mlc::String("void", 4); },
[&](const ast::TyNamed& tyNamed) { auto [name] = tyNamed; return type_name_to_cpp(context, name); },
[&](const ast::TyArray& tyArray) { auto [inner] = tyArray; return ((mlc::String("mlc::Array<", 11) + type_to_cpp(context, inner)) + mlc::String(">", 1)); },
[&](const ast::TyShared& tyShared) { auto [inner] = tyShared; return ((mlc::String("std::shared_ptr<", 16) + type_to_cpp(context, inner)) + mlc::String(">", 1)); },
[&](const ast::TyGeneric& tyGeneric) { auto [name, targs] = tyGeneric; return (((name == mlc::String("ref", 3)) && (targs.length() == 1)) ? (type_to_cpp(context, targs[0]) + mlc::String("&", 1)) : [&]() {
auto safe = ((name == mlc::String("Map", 3)) ? mlc::String("mlc::HashMap", 12) : ((name == mlc::String("Shared", 6)) ? mlc::String("std::shared_ptr", 15) : context_resolve(context, name)));
return ((targs.length() == 0) ? safe : ((targs.length() == 1) ? (((safe + mlc::String("<", 1)) + type_to_cpp(context, targs[0])) + mlc::String(">", 1)) : (((((safe + mlc::String("<", 1)) + type_to_cpp(context, targs[0])) + mlc::String(", ", 2)) + type_to_cpp(context, targs[1])) + mlc::String(">", 1))));
}()); },
[&](const ast::TyFn& tyFn) { auto [params, ret] = tyFn; return [&]() {
auto param_types = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < params.length())) {
param_types.push_back(type_to_cpp(context, params[i]));
i = (i + 1);
}
return ((((mlc::String("std::function<", 14) + type_to_cpp(context, ret)) + mlc::String("(", 1)) + param_types.join(mlc::String(", ", 2))) + mlc::String(")>", 2));
}(); }
}, (*type_expr));
}
mlc::String escape_str(mlc::String input) noexcept{
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < input.length())) {
auto ch = input.char_at(i);
if ((ch == mlc::String("\\", 1))) {
parts.push_back(mlc::String("\\\\", 2));
} else if ((ch == mlc::String("\"", 1))) {
parts.push_back(mlc::String("\\\"", 2));
} else if ((ch == mlc::String("\n", 1))) {
parts.push_back(mlc::String("\\n", 2));
} else if ((ch == mlc::String("\r", 1))) {
parts.push_back(mlc::String("\\r", 2));
} else if ((ch == mlc::String("\t", 1))) {
parts.push_back(mlc::String("\\t", 2));
} else if ((ch == mlc::String("\0", 1))) {
parts.push_back(mlc::String("\\0", 2));
} else {
parts.push_back(ch);
}





i = (i + 1);
}
return parts.join(mlc::String("", 0));
}
mlc::String gen_args(mlc::Array<std::shared_ptr<ast::Expr>> exprs, CodegenContext context) noexcept{
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < exprs.length())) {
parts.push_back(gen_expr(exprs[i], context));
i = (i + 1);
}
return parts.join(mlc::String(", ", 2));
}
mlc::String map_method(mlc::String method_name) noexcept{
if (((method_name == mlc::String("length", 6)) || (method_name == mlc::String("len", 3)))) {
return mlc::String("length", 6);
} else if ((method_name == mlc::String("push", 4))) {
return mlc::String("push_back", 9);
} else if ((method_name == mlc::String("to_string", 9))) {
return mlc::String("to_string", 9);
} else if (((method_name == mlc::String("to_int", 6)) || (method_name == mlc::String("to_i", 4)))) {
return mlc::String("to_i", 4);
} else if (((method_name == mlc::String("upper", 5)) || (method_name == mlc::String("to_upper", 8)))) {
return mlc::String("upper", 5);
} else if (((method_name == mlc::String("lower", 5)) || (method_name == mlc::String("to_lower", 8)))) {
return mlc::String("lower", 5);
} else {
return method_name;
}





}
mlc::String map_builtin(mlc::String name) noexcept{
if ((name == mlc::String("print", 5))) {
return mlc::String("mlc::io::print", 14);
} else if ((name == mlc::String("println", 7))) {
return mlc::String("mlc::io::println", 16);
} else if ((name == mlc::String("exit", 4))) {
return mlc::String("mlc::io::exit", 13);
} else if ((name == mlc::String("args", 4))) {
return mlc::String("mlc::io::args", 13);
} else {
return name;
}



}
GenStmtsResult gen_stmts_str_with_try(mlc::Array<std::shared_ptr<ast::Stmt>> stmts, CodegenContext context, int try_counter) noexcept{
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
auto cur_ctx = context;
auto counter = try_counter;
while ((i < stmts.length())) {
auto result = gen_stmt_with_try(stmts[i], cur_ctx, counter);
parts.push_back(result.output);
counter = result.next_try;
cur_ctx = update_context_from_statement(stmts[i], cur_ctx);
i = (i + 1);
}
return GenStmtsResult{parts.join(mlc::String("", 0)), counter};
}
mlc::String gen_stmts_str(mlc::Array<std::shared_ptr<ast::Stmt>> stmts, CodegenContext context) noexcept{
auto result = gen_stmts_str_with_try(stmts, context, 0);
return result.code;
}
CodegenContext stmts_final_ctx(mlc::Array<std::shared_ptr<ast::Stmt>> stmts, CodegenContext context) noexcept{
auto cur_ctx = context;
auto i = 0;
while ((i < stmts.length())) {
cur_ctx = update_context_from_statement(stmts[i], cur_ctx);
i = (i + 1);
}
return cur_ctx;
}
mlc::String gen_block_body(std::shared_ptr<ast::Expr> expr, CodegenContext context) noexcept{
return std::visit(overloaded{[&](const ast::ExprBlock& exprBlock) { auto [stmts, result] = exprBlock; return [&]() {
auto stmts_code = gen_stmts_str(stmts, context);
auto final_ctx = stmts_final_ctx(stmts, context);
return std::visit(overloaded{[&](const ast::ExprIf& exprIf) { auto [__0, __1, __2] = exprIf; return (stmts_code + gen_stmt_expr(result, final_ctx)); },
[&](const ast::ExprUnit& exprUnit) { return stmts_code; },
[&](const auto& __v) { return [&]() {
auto result_code = gen_expr(result, final_ctx);
return ((result_code == mlc::String("/* unit */", 10)) ? stmts_code : ((stmts_code + result_code) + mlc::String(";\n", 2)));
}(); }
}, (*result));
}(); },
[&](const ast::ExprIf& exprIf) { auto [__0, __1, __2] = exprIf; return gen_stmt_expr(expr, context); },
[&](const auto& __v) { return (gen_expr(expr, context) + mlc::String(";\n", 2)); }
}, (*expr));
}
mlc::String gen_expr(std::shared_ptr<ast::Expr> expr, CodegenContext context) noexcept{
return std::visit(overloaded{[&](const ast::ExprInt& exprInt) { auto [n] = exprInt; return mlc::to_string(n); },
[&](const ast::ExprStr& exprStr) { auto [s] = exprStr; return ((((mlc::String("mlc::String(\"", 13) + escape_str(s)) + mlc::String("\", ", 3)) + mlc::to_string(s.length())) + mlc::String(")", 1)); },
[&](const ast::ExprBool& exprBool) { auto [b] = exprBool; return (b ? mlc::String("true", 4) : mlc::String("false", 5)); },
[&](const ast::ExprUnit& exprUnit) { return mlc::String("/* unit */", 10); },
[&](const ast::ExprExtern& exprExtern) { return mlc::String("", 0); },
[&](const ast::ExprIdent& exprIdent) { auto [name] = exprIdent; return ((context.self_type.length() > 0) ? [&]() {
auto self_fields = lookup_fields(context.field_orders, context.self_type);
auto likely_field = (((((((name == mlc::String("errors", 6)) || (name == mlc::String("kind", 4))) || (name == mlc::String("tokens", 6))) || (name == mlc::String("line", 4))) || (name == mlc::String("col", 3))) || (name == mlc::String("inferred_type", 13))) || (name == mlc::String("type_env", 8)));
return ((list_contains(self_fields, name) || likely_field) ? (mlc::String("self.", 5) + cpp_safe(name)) : [&]() {
auto mapped = map_builtin(name);
auto safe = context_resolve(context, mapped);
auto needs_brace = (((name.length() > 0) && (name.char_at(0) >= mlc::String("A", 1))) && (name.char_at(0) <= mlc::String("Z", 1)));
return (needs_brace ? (safe + mlc::String("{}", 2)) : safe);
}());
}() : [&]() {
auto mapped = map_builtin(name);
auto safe = context_resolve(context, mapped);
auto needs_brace = (((name.length() > 0) && (name.char_at(0) >= mlc::String("A", 1))) && (name.char_at(0) <= mlc::String("Z", 1)));
return (needs_brace ? (safe + mlc::String("{}", 2)) : safe);
}()); },
[&](const ast::ExprBin& exprBin) { auto [op, left_expr, right_expr] = exprBin; return ((((((mlc::String("(", 1) + gen_expr(left_expr, context)) + mlc::String(" ", 1)) + op) + mlc::String(" ", 1)) + gen_expr(right_expr, context)) + mlc::String(")", 1)); },
[&](const ast::ExprUn& exprUn) { auto [op, inner] = exprUn; return (((mlc::String("(", 1) + op) + gen_expr(inner, context)) + mlc::String(")", 1)); },
[&](const ast::ExprCall& exprCall) { auto [func, call_args] = exprCall; return [&]() {
auto fn_code = std::visit(overloaded{[&](const ast::ExprIdent& exprIdent) { auto [name] = exprIdent; return context_resolve(context, map_builtin(name)); },
[&](const auto& __v) { return gen_expr(func, context); }
}, (*func));
return (((fn_code + mlc::String("(", 1)) + gen_args(call_args, context)) + mlc::String(")", 1));
}(); },
[&](const ast::ExprMethod& exprMethod) { auto [obj, method_name, margs] = exprMethod; return [&]() {
auto obj_code = std::visit(overloaded{[&](const ast::ExprIdent& exprIdent) { auto [name] = exprIdent; return ((context.self_type.length() > 0) ? [&]() {
auto sf = lookup_fields(context.field_orders, context.self_type);
auto likely_field = (((((((name == mlc::String("errors", 6)) || (name == mlc::String("kind", 4))) || (name == mlc::String("tokens", 6))) || (name == mlc::String("line", 4))) || (name == mlc::String("col", 3))) || (name == mlc::String("inferred_type", 13))) || (name == mlc::String("type_env", 8)));
return ((list_contains(sf, name) || likely_field) ? (mlc::String("self.", 5) + cpp_safe(name)) : context_resolve(context, map_builtin(name)));
}() : context_resolve(context, map_builtin(name))); },
[&](const auto& __v) { return gen_expr(obj, context); }
}, (*obj));
return ((obj_code == mlc::String("File", 4)) ? [&]() {
auto cpp_fn = ((method_name == mlc::String("read", 4)) ? mlc::String("mlc::file::read_to_string", 25) : ((method_name == mlc::String("write", 5)) ? mlc::String("mlc::file::write_string", 23) : (mlc::String("mlc::file::", 11) + method_name)));
return (((cpp_fn + mlc::String("(", 1)) + gen_args(margs, context)) + mlc::String(")", 1));
}() : (((obj_code == mlc::String("Map", 3)) && (method_name == mlc::String("new", 3))) ? mlc::String("{}", 2) : ((((obj_code == mlc::String("Shared", 6)) && (method_name == mlc::String("new", 3))) && (margs.length() == 1)) ? [&]() {
auto arg_code = gen_expr(margs[0], context);
auto raw_name = std::visit(overloaded{[&](const ast::ExprRecord& exprRecord) { auto [n, __1] = exprRecord; return n; },
[&](const ast::ExprCall& exprCall) { auto [fn_expr, __1] = exprCall; return std::visit(overloaded{[&](const ast::ExprIdent& exprIdent) { auto [n] = exprIdent; return n; },
[&](const auto& __v) { return mlc::String("", 0); }
}, (*fn_expr)); },
[&](const ast::ExprIdent& exprIdent) { auto [n] = exprIdent; return n; },
[&](const ast::ExprIndex& exprIndex) { auto [arr, __1] = exprIndex; return std::visit(overloaded{[&](const ast::ExprIdent& exprIdent) { auto [n] = exprIdent; return (context.array_elem_types.has(n) ? context.array_elem_types.get(n) : mlc::String("", 0)); },
[&](const auto& __v) { return mlc::String("", 0); }
}, (*arr)); },
[&](const auto& __v) { return mlc::String("", 0); }
}, (*margs[0]));
auto parent = (((raw_name.length() > 0) && context.variant_types.has(raw_name)) ? context.variant_types.get(raw_name) : raw_name);
auto type_name = ((parent.length() > 0) ? context_resolve(context, parent) : mlc::String("auto", 4));
return ((((mlc::String("std::make_shared<", 17) + type_name) + mlc::String(">(", 2)) + arg_code) + mlc::String(")", 1));
}() : (((method_name == mlc::String("to_string", 9)) && (margs.length() == 0)) ? ((mlc::String("mlc::to_string(", 15) + obj_code) + mlc::String(")", 1)) : (context.method_owners.has(method_name) ? [&]() {
auto mangled = context.method_owners.get(method_name);
auto fn_name = context_resolve(context, mangled);
return ((margs.length() == 0) ? (((fn_name + mlc::String("(", 1)) + obj_code) + mlc::String(")", 1)) : (((((fn_name + mlc::String("(", 1)) + obj_code) + mlc::String(", ", 2)) + gen_args(margs, context)) + mlc::String(")", 1)));
}() : [&]() {
auto base = (((obj_code + mlc::String(".", 1)) + map_method(method_name)) + mlc::String("(", 1));
return ((margs.length() == 0) ? (base + mlc::String(")", 1)) : ((base + gen_args(margs, context)) + mlc::String(")", 1)));
}())))));
}(); },
[&](const ast::ExprField& exprField) { auto [obj, field_name] = exprField; return std::visit(overloaded{[&](const ast::ExprIndex& exprIndex) { auto [inner, __1] = exprIndex; return std::visit(overloaded{[&](const ast::ExprIdent& exprIdent) { auto [n] = exprIdent; return (context.shared_array_params.contains(n) ? ((gen_expr(obj, context) + mlc::String("->", 2)) + cpp_safe(field_name)) : ((gen_expr(obj, context) + mlc::String(".", 1)) + cpp_safe(field_name))); },
[&](const auto& __v) { return ((gen_expr(obj, context) + mlc::String("->", 2)) + cpp_safe(field_name)); }
}, (*inner)); },
[&](const ast::ExprIdent& exprIdent) { auto [n] = exprIdent; return (context.shared_params.contains(n) ? ((gen_expr(obj, context) + mlc::String("->", 2)) + cpp_safe(field_name)) : ((gen_expr(obj, context) + mlc::String(".", 1)) + cpp_safe(field_name))); },
[&](const ast::ExprMethod& exprMethod) { auto [m_obj, method_name, __2] = exprMethod; return ((method_name == mlc::String("get", 3)) ? std::visit(overloaded{[&](const ast::ExprIdent& exprIdent) { auto [m] = exprIdent; return (context.shared_map_params.contains(m) ? ((gen_expr(obj, context) + mlc::String("->", 2)) + cpp_safe(field_name)) : ((gen_expr(obj, context) + mlc::String(".", 1)) + cpp_safe(field_name))); },
[&](const auto& __v) { return ((gen_expr(obj, context) + mlc::String(".", 1)) + cpp_safe(field_name)); }
}, (*m_obj)) : ((gen_expr(obj, context) + mlc::String(".", 1)) + cpp_safe(field_name))); },
[&](const auto& __v) { return ((gen_expr(obj, context) + mlc::String(".", 1)) + cpp_safe(field_name)); }
}, (*obj)); },
[&](const ast::ExprIndex& exprIndex) { auto [obj, idx] = exprIndex; return (((gen_expr(obj, context) + mlc::String("[", 1)) + gen_expr(idx, context)) + mlc::String("]", 1)); },
[&](const ast::ExprIf& exprIf) { auto [cond, then_expr, else_expr] = exprIf; return ((((((mlc::String("(", 1) + gen_expr(cond, context)) + mlc::String(" ? ", 3)) + gen_expr(then_expr, context)) + mlc::String(" : ", 3)) + gen_expr(else_expr, context)) + mlc::String(")", 1)); },
[&](const ast::ExprBlock& exprBlock) { auto [stmts, result] = exprBlock; return ((stmts.length() == 0) ? gen_expr(result, context) : [&]() {
auto body = (((gen_stmts_str(stmts, context) + mlc::String("return ", 7)) + gen_expr(result, stmts_final_ctx(stmts, context))) + mlc::String(";\n", 2));
return ((mlc::String("[&]() {\n", 8) + body) + mlc::String("}()", 3));
}()); },
[&](const ast::ExprWhile& exprWhile) { auto [cond, stmts] = exprWhile; return ((((mlc::String("[&]() {\nwhile (", 15) + gen_expr(cond, context)) + mlc::String(") {\n", 4)) + gen_stmts_str(stmts, context)) + mlc::String("}\n}()", 5)); },
[&](const ast::ExprFor& exprFor) { auto [var, iter, stmts] = exprFor; return ((((((mlc::String("[&]() {\nfor (auto ", 18) + cpp_safe(var)) + mlc::String(" : ", 3)) + gen_expr(iter, context)) + mlc::String(") {\n", 4)) + gen_stmts_str(stmts, context)) + mlc::String("}\n}()", 5)); },
[&](const ast::ExprMatch& exprMatch) { auto [subject, arms] = exprMatch; return gen_match(subject, arms, context); },
[&](const ast::ExprRecord& exprRecord) { auto [name, field_vals] = exprRecord; return [&]() {
auto field_order = lookup_fields(context.field_orders, name);
auto vals = ((field_order.length() > 0) ? gen_record_ordered(field_vals, field_order, context) : gen_record_unordered(field_vals, context));
return (((context_resolve(context, name) + mlc::String("{", 1)) + vals) + mlc::String("}", 1));
}(); },
[&](const ast::ExprRecordUpdate& exprRecordUpdate) { auto [name, base, overrides] = exprRecordUpdate; return [&]() {
auto field_order = lookup_fields(context.field_orders, name);
auto vals = gen_record_update_ordered(base, overrides, field_order, context);
return (((context_resolve(context, name) + mlc::String("{", 1)) + vals) + mlc::String("}", 1));
}(); },
[&](const ast::ExprArray& exprArray) { auto [exprs] = exprArray; return ((exprs.length() == 0) ? mlc::String("{}", 2) : ((mlc::String("mlc::Array{", 11) + gen_args(exprs, context)) + mlc::String("}", 1))); },
[&](const ast::ExprQuestion& exprQuestion) { auto [inner] = exprQuestion; return gen_expr(inner, context); },
[&](const ast::ExprLambda& exprLambda) { auto [params, body] = exprLambda; return [&]() {
auto capture = ((params.length() == 0) ? mlc::String("[]", 2) : mlc::String("[=]", 3));
auto param_list = ((params.length() == 0) ? mlc::String("", 0) : [&]() {
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < params.length())) {
parts.push_back((mlc::String("auto ", 5) + cpp_safe(params[i])));
i = (i + 1);
}
return parts.join(mlc::String(", ", 2));
}());
auto body_code = gen_expr(body, context);
return (((((capture + mlc::String("(", 1)) + param_list) + mlc::String(") { return ", 11)) + body_code) + mlc::String("; }", 3));
}(); }
}, (*expr));
}
mlc::String gen_record_ordered(mlc::Array<std::shared_ptr<ast::FieldVal>> field_vals, mlc::Array<mlc::String> field_order, CodegenContext context) noexcept{
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < field_order.length())) {
auto value = find_field_val(field_vals, field_order[i]);
parts.push_back(gen_expr(value, context));
i = (i + 1);
}
return parts.join(mlc::String(", ", 2));
}
mlc::String gen_record_unordered(mlc::Array<std::shared_ptr<ast::FieldVal>> field_vals, CodegenContext context) noexcept{
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < field_vals.length())) {
parts.push_back(gen_expr(field_vals[i]->val, context));
i = (i + 1);
}
return parts.join(mlc::String(", ", 2));
}
mlc::String gen_record_update_ordered(std::shared_ptr<ast::Expr> base, mlc::Array<std::shared_ptr<ast::FieldVal>> overrides, mlc::Array<mlc::String> field_order, CodegenContext context) noexcept{
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < field_order.length())) {
auto field_name = field_order[i];
auto override_val = find_field_val(overrides, field_name);
auto val_str = std::visit(overloaded{[&](const ast::ExprUnit& exprUnit) { return gen_expr(std::make_shared<ast::Expr>(ast::ExprField(base, field_name)), context); },
[&](const auto& __v) { return gen_expr(override_val, context); }
}, (*override_val));
parts.push_back(val_str);
i = (i + 1);
}
return parts.join(mlc::String(", ", 2));
}
std::shared_ptr<ast::Expr> find_field_val(mlc::Array<std::shared_ptr<ast::FieldVal>> field_vals, mlc::String name) noexcept{
auto result = std::make_shared<ast::Expr>(ast::ExprUnit{});
auto i = 0;
while ((i < field_vals.length())) {
if ((field_vals[i]->name == name)) {
result = field_vals[i]->val;
}
i = (i + 1);
}
return result;
}
bool subject_needs_deref(std::shared_ptr<ast::Expr> subject, CodegenContext context) noexcept{
return std::visit(overloaded{[&](const ast::ExprIdent& exprIdent) { auto [__0] = exprIdent; return true; },
[&](const ast::ExprIndex& exprIndex) { auto [arr, __1] = exprIndex; return std::visit(overloaded{[&](const ast::ExprIdent& exprIdent) { auto [arr_name] = exprIdent; return context.shared_array_params.contains(arr_name); },
[&](const auto& __v) { return true; }
}, (*arr)); },
[&](const ast::ExprMethod& exprMethod) { auto [__0, __1, __2] = exprMethod; return false; },
[&](const auto& __v) { return true; }
}, (*subject));
}
bool first_arm_needs_deref(std::shared_ptr<ast::MatchArm> first_arm, std::shared_ptr<ast::Expr> subject, CodegenContext context) noexcept{
return std::visit(overloaded{[&](const ast::PatCtor& patCtor) { auto [name, __1] = patCtor; return [&]() {
auto resolved = context_resolve(context, name);
return (((resolved.length() >= 12) && (resolved.substring(0, 12) == mlc::String("ast_tokens::", 12))) ? false : subject_needs_deref(subject, context));
}(); },
[&](const auto& __v) { return subject_needs_deref(subject, context); }
}, (*first_arm->pat));
}
mlc::String gen_match(std::shared_ptr<ast::Expr> subject, mlc::Array<std::shared_ptr<ast::MatchArm>> arms, CodegenContext context) noexcept{
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < arms.length())) {
parts.push_back(gen_arm(arms[i], context));
i = (i + 1);
}
auto subject_code = gen_expr(subject, context);
auto needs_deref = ((arms.length() > 0) ? first_arm_needs_deref(arms[0], subject, context) : true);
auto visit_subject = (needs_deref ? ((mlc::String("(*", 2) + subject_code) + mlc::String(")", 1)) : subject_code);
return ((((mlc::String("std::visit(overloaded{", 22) + parts.join(mlc::String(",\n", 2))) + mlc::String("\n}, ", 4)) + visit_subject) + mlc::String(")", 1));
}
mlc::String gen_arm(std::shared_ptr<ast::MatchArm> arm, CodegenContext context) noexcept{
return std::visit(overloaded{[&](const ast::PatWild& patWild) { return ((mlc::String("[&](const auto& __v) { return ", 30) + gen_expr(arm->body, context)) + mlc::String("; }", 3)); },
[&](const ast::PatUnit& patUnit) { return ((mlc::String("[&](const auto& __v) { return ", 30) + gen_expr(arm->body, context)) + mlc::String("; }", 3)); },
[&](const ast::PatBool& patBool) { auto [__0] = patBool; return ((mlc::String("[&](const auto& __v) { return ", 30) + gen_expr(arm->body, context)) + mlc::String("; }", 3)); },
[&](const ast::PatInt& patInt) { auto [__0] = patInt; return ((mlc::String("[&](const auto& __v) { return ", 30) + gen_expr(arm->body, context)) + mlc::String("; }", 3)); },
[&](const ast::PatStr& patStr) { auto [__0] = patStr; return ((mlc::String("[&](const auto& __v) { return ", 30) + gen_expr(arm->body, context)) + mlc::String("; }", 3)); },
[&](const ast::PatIdent& patIdent) { auto [name] = patIdent; return ((((mlc::String("[&](const auto& ", 16) + cpp_safe(name)) + mlc::String(") { return ", 11)) + gen_expr(arm->body, context)) + mlc::String("; }", 3)); },
[&](const ast::PatCtor& patCtor) { auto [name, sub_pats] = patCtor; return [&]() {
auto qualified_name = context_resolve(context, name);
auto lower_name = cpp_safe(lower_first(name));
auto binding = ((sub_pats.length() == 0) ? mlc::String("", 0) : ((((mlc::String("auto [", 6) + pat_bind_names(sub_pats)) + mlc::String("] = ", 4)) + lower_name) + mlc::String("; ", 2)));
auto ctor_type_info = lookup_ctor_type_info(context.ctor_type_infos, name);
auto arm_context = context;
auto index = 0;
while ((index < sub_pats.length())) {
std::visit(overloaded{[&](const ast::PatIdent& patIdent) { auto [pat_name] = patIdent; return [&]() {
if (ctor_type_info->shared_pos.contains(index)) {
arm_context = context_add_shared(arm_context, pat_name);
}
if (ctor_type_info->shared_arr_pos.contains(index)) {
arm_context = context_add_shared_array(arm_context, pat_name);
}
return /* unit */;
}(); },
[&](const auto& __v) { return /* unit */; }
}, (*sub_pats[index]));
index = (index + 1);
}
return ((((((((mlc::String("[&](const ", 10) + qualified_name) + mlc::String("& ", 2)) + lower_name) + mlc::String(") { ", 4)) + binding) + mlc::String("return ", 7)) + gen_expr(arm->body, arm_context)) + mlc::String("; }", 3));
}(); },
[&](const ast::PatRecord& patRecord) { auto [name, field_pats] = patRecord; return [&]() {
auto qualified_name = context_resolve(context, name);
auto lower_name = cpp_safe(lower_first(name));
auto field_bindings = mlc::String("", 0);
auto fi = 0;
while ((fi < field_pats.length())) {
std::visit(overloaded{[&](const ast::PatIdent& patIdent) { auto [field_name] = patIdent; return [&]() {
field_bindings = (((((((field_bindings + mlc::String("const auto& ", 12)) + cpp_safe(field_name)) + mlc::String(" = ", 3)) + lower_name) + mlc::String(".", 1)) + cpp_safe(field_name)) + mlc::String("; ", 2));
return /* unit */;
}(); },
[&](const auto& __v) { return /* unit */; }
}, (*field_pats[fi]));
fi = (fi + 1);
}
return ((((((((mlc::String("[&](const ", 10) + qualified_name) + mlc::String("& ", 2)) + lower_name) + mlc::String(") { ", 4)) + field_bindings) + mlc::String("return ", 7)) + gen_expr(arm->body, context)) + mlc::String("; }", 3));
}(); }
}, (*arm->pat));
}
mlc::String pat_bind_names(mlc::Array<std::shared_ptr<ast::Pat>> sub_pats) noexcept{
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < sub_pats.length())) {
auto bind_name = std::visit(overloaded{[&](const ast::PatIdent& patIdent) { auto [name] = patIdent; return cpp_safe(name); },
[&](const auto& __v) { return (mlc::String("__", 2) + mlc::to_string(i)); }
}, (*sub_pats[i]));
parts.push_back(bind_name);
i = (i + 1);
}
return parts.join(mlc::String(", ", 2));
}
mlc::String gen_try_unwrap(std::shared_ptr<ast::Expr> inner, CodegenContext context, mlc::String try_id, mlc::String success_line) noexcept{
auto inner_code = gen_expr(inner, context);
auto ok_name = context_resolve(context, mlc::String("Ok", 2));
auto err_name = context_resolve(context, mlc::String("Err", 3));
return ((((((((((((((mlc::String("auto ", 5) + try_id) + mlc::String(" = ", 3)) + inner_code) + mlc::String(";\n", 2)) + mlc::String("if (std::holds_alternative<", 27)) + err_name) + mlc::String(">(", 2)) + try_id) + mlc::String(")) return std::get<", 19)) + err_name) + mlc::String(">(", 2)) + try_id) + mlc::String(");\n", 3)) + success_line);
}
GenStmtResult gen_stmt_with_try(std::shared_ptr<ast::Stmt> stmt, CodegenContext context, int try_counter) noexcept{
return std::visit(overloaded{[&](const ast::StmtLet& stmtLet) { auto [name, __1, typ, value] = stmtLet; return [&]() {
auto try_id = (mlc::String("__try_", 6) + mlc::to_string(try_counter));
auto next_counter = (try_counter + 1);
auto result = std::visit(overloaded{[&](const ast::ExprQuestion& exprQuestion) { auto [inner] = exprQuestion; return GenStmtResult{gen_try_unwrap(inner, context, try_id, ((((((mlc::String("auto ", 5) + cpp_safe(name)) + mlc::String(" = std::get<", 12)) + context_resolve(context, mlc::String("Ok", 2))) + mlc::String(">(", 2)) + try_id) + mlc::String(").field0;\n", 10))), next_counter}; },
[&](const ast::ExprArray& exprArray) { auto [elems] = exprArray; return [&]() {
auto val_code = ((elems.length() == 0) ? std::visit(overloaded{[&](const ast::TyArray& tyArray) { auto [inner] = tyArray; return ((mlc::String("mlc::Array<", 11) + type_to_cpp(context, inner)) + mlc::String(">{}", 3)); },
[&](const auto& __v) { return mlc::String("mlc::Array<mlc::String>{}", 25); }
}, (*typ)) : ((mlc::String("mlc::Array{", 11) + gen_args(elems, context)) + mlc::String("}", 1)));
return GenStmtResult{((((mlc::String("auto ", 5) + cpp_safe(name)) + mlc::String(" = ", 3)) + val_code) + mlc::String(";\n", 2)), try_counter};
}(); },
[&](const ast::ExprMethod& exprMethod) { auto [map_obj, method_name, __2] = exprMethod; return [&]() {
auto val_code = ((method_name == mlc::String("new", 3)) ? std::visit(overloaded{[&](const ast::ExprIdent& exprIdent) { auto [on] = exprIdent; return ((on == mlc::String("Map", 3)) ? std::visit(overloaded{[&](const ast::TyGeneric& tyGeneric) { auto [tn, targs] = tyGeneric; return (((tn == mlc::String("Map", 3)) && (targs.length() == 2)) ? ((((mlc::String("mlc::HashMap<", 13) + type_to_cpp(context, targs[0])) + mlc::String(", ", 2)) + type_to_cpp(context, targs[1])) + mlc::String(">()", 3)) : gen_expr(value, context)); },
[&](const auto& __v) { return gen_expr(value, context); }
}, (*typ)) : gen_expr(value, context)); },
[&](const auto& __v) { return gen_expr(value, context); }
}, (*map_obj)) : gen_expr(value, context));
return GenStmtResult{((((mlc::String("auto ", 5) + cpp_safe(name)) + mlc::String(" = ", 3)) + val_code) + mlc::String(";\n", 2)), try_counter};
}(); },
[&](const auto& __v) { return GenStmtResult{((((mlc::String("auto ", 5) + cpp_safe(name)) + mlc::String(" = ", 3)) + gen_expr(value, context)) + mlc::String(";\n", 2)), try_counter}; }
}, (*value));
return result;
}(); },
[&](const ast::StmtExpr& stmtExpr) { auto [expr] = stmtExpr; return std::visit(overloaded{[&](const ast::ExprQuestion& exprQuestion) { auto [inner] = exprQuestion; return [&]() {
auto try_id = (mlc::String("__try_", 6) + mlc::to_string(try_counter));
auto ok_name = context_resolve(context, mlc::String("Ok", 2));
return GenStmtResult{gen_try_unwrap(inner, context, try_id, ((((mlc::String("/* discard */ (void)std::get<", 29) + ok_name) + mlc::String(">(", 2)) + try_id) + mlc::String(").field0;\n", 10))), (try_counter + 1)};
}(); },
[&](const auto& __v) { return GenStmtResult{gen_stmt_expr(expr, context), try_counter}; }
}, (*expr)); },
[&](const ast::StmtBreak& stmtBreak) { return GenStmtResult{mlc::String("break;\n", 7), try_counter}; },
[&](const ast::StmtContinue& stmtContinue) { return GenStmtResult{mlc::String("continue;\n", 10), try_counter}; },
[&](const ast::StmtReturn& stmtReturn) { auto [expr] = stmtReturn; return std::visit(overloaded{[&](const ast::ExprQuestion& exprQuestion) { auto [inner] = exprQuestion; return [&]() {
auto try_id = (mlc::String("__try_ret_", 10) + mlc::to_string(try_counter));
auto ok_name = context_resolve(context, mlc::String("Ok", 2));
return GenStmtResult{gen_try_unwrap(inner, context, try_id, ((((mlc::String("return std::get<", 16) + ok_name) + mlc::String(">(", 2)) + try_id) + mlc::String(").field0;\n", 10))), (try_counter + 1)};
}(); },
[&](const auto& __v) { return GenStmtResult{((mlc::String("return ", 7) + gen_expr(expr, context)) + mlc::String(";\n", 2)), try_counter}; }
}, (*expr)); }
}, (*stmt));
}
mlc::String gen_stmt(std::shared_ptr<ast::Stmt> stmt, CodegenContext context) noexcept{
auto result = gen_stmt_with_try(stmt, context, 0);
return result.output;
}
mlc::String gen_stmt_expr(std::shared_ptr<ast::Expr> expr, CodegenContext context) noexcept{
return std::visit(overloaded{[&](const ast::ExprBin& exprBin) { auto [op, left_expr, right_expr] = exprBin; return ((op == mlc::String("=", 1)) ? (((gen_expr(left_expr, context) + mlc::String(" = ", 3)) + gen_expr(right_expr, context)) + mlc::String(";\n", 2)) : ((((((mlc::String("(", 1) + gen_expr(left_expr, context)) + mlc::String(" ", 1)) + op) + mlc::String(" ", 1)) + gen_expr(right_expr, context)) + mlc::String(");\n", 3))); },
[&](const ast::ExprIf& exprIf) { auto [cond, then_expr, else_expr] = exprIf; return [&]() {
auto output = ((((mlc::String("if (", 4) + gen_expr(cond, context)) + mlc::String(") {\n", 4)) + gen_block_body(then_expr, context)) + mlc::String("}", 1));
auto else_str = std::visit(overloaded{[&](const ast::ExprUnit& exprUnit) { return mlc::String("", 0); },
[&](const ast::ExprBlock& exprBlock) { auto [stmts, result] = exprBlock; return ((stmts.length() == 0) ? std::visit(overloaded{[&](const ast::ExprUnit& exprUnit) { return mlc::String("", 0); },
[&](const auto& __v) { return ((mlc::String(" else {\n", 8) + gen_block_body(else_expr, context)) + mlc::String("}", 1)); }
}, (*result)) : ((mlc::String(" else {\n", 8) + gen_block_body(else_expr, context)) + mlc::String("}", 1))); },
[&](const ast::ExprIf& exprIf) { auto [__0, __1, __2] = exprIf; return (mlc::String(" else ", 6) + gen_stmt_expr(else_expr, context)); },
[&](const auto& __v) { return ((mlc::String(" else {\n", 8) + gen_block_body(else_expr, context)) + mlc::String("}", 1)); }
}, (*else_expr));
if ((else_str.length() > 0)) {
output = (output + else_str);
}
return (output + mlc::String("\n", 1));
}(); },
[&](const ast::ExprWhile& exprWhile) { auto [cond, stmts] = exprWhile; return ((((mlc::String("while (", 7) + gen_expr(cond, context)) + mlc::String(") {\n", 4)) + gen_stmts_str(stmts, context)) + mlc::String("}\n", 2)); },
[&](const ast::ExprFor& exprFor) { auto [var, iter, stmts] = exprFor; return ((((((mlc::String("for (auto ", 10) + cpp_safe(var)) + mlc::String(" : ", 3)) + gen_expr(iter, context)) + mlc::String(") {\n", 4)) + gen_stmts_str(stmts, context)) + mlc::String("}\n", 2)); },
[&](const ast::ExprBlock& exprBlock) { auto [stmts, result] = exprBlock; return [&]() {
auto stmts_code = gen_stmts_str(stmts, context);
auto result_code = gen_expr(result, stmts_final_ctx(stmts, context));
return ((result_code == mlc::String("/* unit */", 10)) ? stmts_code : ((stmts_code + result_code) + mlc::String(";\n", 2)));
}(); },
[&](const auto& __v) { return (gen_expr(expr, context) + mlc::String(";\n", 2)); }
}, (*expr));
}
mlc::String gen_return_body(std::shared_ptr<ast::Expr> expr, CodegenContext context) noexcept{
return std::visit(overloaded{[&](const ast::ExprBlock& exprBlock) { auto [stmts, result] = exprBlock; return [&]() {
auto final_ctx = stmts_final_ctx(stmts, context);
auto stmts_code = gen_stmts_str(stmts, context);
return std::visit(overloaded{[&](const ast::ExprQuestion& exprQuestion) { auto [inner] = exprQuestion; return [&]() {
auto try_id = mlc::String("__try_ret", 9);
auto ok_name = context_resolve(context, mlc::String("Ok", 2));
return (stmts_code + gen_try_unwrap(inner, final_ctx, try_id, ((((mlc::String("return std::get<", 16) + ok_name) + mlc::String(">(", 2)) + try_id) + mlc::String(").field0;\n", 10))));
}(); },
[&](const ast::ExprUnit& exprUnit) { return stmts_code; },
[&](const auto& __v) { return (((stmts_code + mlc::String("return ", 7)) + gen_expr(result, final_ctx)) + mlc::String(";\n", 2)); }
}, (*result));
}(); },
[&](const ast::ExprIf& exprIf) { auto [cond, then_expr, else_expr] = exprIf; return [&]() {
auto output = ((((mlc::String("if (", 4) + gen_expr(cond, context)) + mlc::String(") {\n", 4)) + gen_return_body(then_expr, context)) + mlc::String("}", 1));
auto else_str = std::visit(overloaded{[&](const ast::ExprUnit& exprUnit) { return mlc::String("", 0); },
[&](const ast::ExprIf& exprIf) { auto [__0, __1, __2] = exprIf; return (mlc::String(" else ", 6) + gen_return_if_stmt(else_expr, context)); },
[&](const auto& __v) { return ((mlc::String(" else {\n", 8) + gen_return_body(else_expr, context)) + mlc::String("}", 1)); }
}, (*else_expr));
if ((else_str.length() > 0)) {
output = (output + else_str);
}
return (output + mlc::String("\n", 1));
}(); },
[&](const ast::ExprQuestion& exprQuestion) { auto [inner] = exprQuestion; return [&]() {
auto try_id = mlc::String("__try_ret", 9);
auto ok_name = context_resolve(context, mlc::String("Ok", 2));
return gen_try_unwrap(inner, context, try_id, ((((mlc::String("return std::get<", 16) + ok_name) + mlc::String(">(", 2)) + try_id) + mlc::String(").field0;\n", 10)));
}(); },
[&](const auto& __v) { return ((mlc::String("return ", 7) + gen_expr(expr, context)) + mlc::String(";\n", 2)); }
}, (*expr));
}
mlc::String gen_return_if_stmt(std::shared_ptr<ast::Expr> expr, CodegenContext context) noexcept{
return std::visit(overloaded{[&](const ast::ExprIf& exprIf) { auto [cond, then_expr, else_expr] = exprIf; return [&]() {
auto output = ((((mlc::String("if (", 4) + gen_expr(cond, context)) + mlc::String(") {\n", 4)) + gen_return_body(then_expr, context)) + mlc::String("}", 1));
auto else_str = std::visit(overloaded{[&](const ast::ExprUnit& exprUnit) { return mlc::String("", 0); },
[&](const ast::ExprIf& exprIf) { auto [__0, __1, __2] = exprIf; return (mlc::String(" else ", 6) + gen_return_if_stmt(else_expr, context)); },
[&](const auto& __v) { return ((mlc::String(" else {\n", 8) + gen_return_body(else_expr, context)) + mlc::String("}", 1)); }
}, (*else_expr));
if ((else_str.length() > 0)) {
output = (output + else_str);
}
return (output + mlc::String("\n", 1));
}(); },
[&](const auto& __v) { return ((mlc::String("if (true) {\n", 12) + gen_return_body(expr, context)) + mlc::String("}\n", 2)); }
}, (*expr));
}
mlc::String gen_fn_body(std::shared_ptr<ast::Expr> body, CodegenContext context) noexcept{
return gen_return_body(body, context);
}
mlc::String gen_params(CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> params) noexcept{
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < params.length())) {
parts.push_back(((type_to_cpp(context, params[i]->typ) + mlc::String(" ", 1)) + cpp_safe(params[i]->name)));
i = (i + 1);
}
return parts.join(mlc::String(", ", 2));
}
mlc::String gen_type_decl_fwd(CodegenContext context, mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
if ((variants.length() == 1)) {
return mlc::String("", 0);
} else {
return gen_adt_fwd(context, type_name, variants);
}
}
mlc::String gen_type_decl_body(CodegenContext context, mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
if ((variants.length() == 1)) {
return gen_single_variant(context, type_name, variants[0]);
} else {
return gen_adt_defs(context, type_name, variants);
}
}
mlc::String gen_single_variant(CodegenContext context, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept{
return std::visit(overloaded{[&](const ast::VarRecord& varRecord) { auto [__0, field_defs] = varRecord; return [&]() {
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < field_defs.length())) {
parts.push_back((((type_to_cpp(context, field_defs[i]->typ) + mlc::String(" ", 1)) + cpp_safe(field_defs[i]->name)) + mlc::String(";", 1)));
i = (i + 1);
}
return ((((mlc::String("struct ", 7) + context_resolve(context, type_name)) + mlc::String(" {", 2)) + parts.join(mlc::String("", 0))) + mlc::String("};\n", 3));
}(); },
[&](const ast::VarTuple& varTuple) { auto [__0, field_types] = varTuple; return [&]() {
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < field_types.length())) {
parts.push_back((((type_to_cpp(context, field_types[i]) + mlc::String(" field", 6)) + mlc::to_string(i)) + mlc::String(";", 1)));
i = (i + 1);
}
return ((((mlc::String("struct ", 7) + context_resolve(context, type_name)) + mlc::String(" {", 2)) + parts.join(mlc::String("", 0))) + mlc::String("};\n", 3));
}(); },
[&](const ast::VarUnit& varUnit) { auto [__0] = varUnit; return ((mlc::String("struct ", 7) + context_resolve(context, type_name)) + mlc::String(" {};\n", 5)); }
}, (*variant));
}
mlc::String gen_adt_fwd(CodegenContext context, mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
auto parts = mlc::Array<mlc::String>{};
auto variant_names = mlc::Array<mlc::String>{};
auto index = 0;
while ((index < variants.length())) {
auto variant_name = context_resolve(context, variant_ctor_name(variants[index]));
variant_names.push_back(variant_name);
parts.push_back(((mlc::String("struct ", 7) + variant_name) + mlc::String(";\n", 2)));
index = (index + 1);
}
return (((((parts.join(mlc::String("", 0)) + mlc::String("using ", 6)) + context_resolve(context, type_name)) + mlc::String(" = std::variant<", 16)) + variant_names.join(mlc::String(", ", 2))) + mlc::String(">;\n", 3));
}
mlc::String gen_adt_defs(CodegenContext context, mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < variants.length())) {
parts.push_back(gen_variant_struct(context, type_name, variants[i]));
i = (i + 1);
}
return parts.join(mlc::String("", 0));
}
mlc::String variant_ctor_name(std::shared_ptr<ast::TypeVariant> variant) noexcept{
return std::visit(overloaded{[&](const ast::VarUnit& varUnit) { auto [name] = varUnit; return name; },
[&](const ast::VarTuple& varTuple) { auto [name, __1] = varTuple; return name; },
[&](const ast::VarRecord& varRecord) { auto [name, __1] = varRecord; return name; }
}, (*variant));
}
mlc::String gen_variant_struct(CodegenContext context, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept{
return std::visit(overloaded{[&](const ast::VarUnit& varUnit) { auto [name] = varUnit; return ((mlc::String("struct ", 7) + context_resolve(context, name)) + mlc::String(" {};\n", 5)); },
[&](const ast::VarTuple& varTuple) { auto [name, field_types] = varTuple; return [&]() {
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < field_types.length())) {
parts.push_back((((type_to_cpp(context, field_types[i]) + mlc::String(" field", 6)) + mlc::to_string(i)) + mlc::String(";", 1)));
i = (i + 1);
}
return ((((mlc::String("struct ", 7) + context_resolve(context, name)) + mlc::String(" {", 2)) + parts.join(mlc::String("", 0))) + mlc::String("};\n", 3));
}(); },
[&](const ast::VarRecord& varRecord) { auto [name, field_defs] = varRecord; return [&]() {
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < field_defs.length())) {
parts.push_back((((type_to_cpp(context, field_defs[i]->typ) + mlc::String(" ", 1)) + cpp_safe(field_defs[i]->name)) + mlc::String(";", 1)));
i = (i + 1);
}
return ((((mlc::String("struct ", 7) + context_resolve(context, name)) + mlc::String(" {", 2)) + parts.join(mlc::String("", 0))) + mlc::String("};\n", 3));
}(); }
}, (*variant));
}
bool is_shared_type(std::shared_ptr<ast::TypeExpr> t) noexcept{
return std::visit(overloaded{[&](const ast::TyGeneric& tyGeneric) { auto [n, __1] = tyGeneric; return (n == mlc::String("Shared", 6)); },
[&](const ast::TyShared& tyShared) { auto [__0] = tyShared; return true; },
[&](const auto& __v) { return false; }
}, (*t));
}
bool is_shared_array_type(std::shared_ptr<ast::TypeExpr> t) noexcept{
return std::visit(overloaded{[&](const ast::TyArray& tyArray) { auto [inner] = tyArray; return is_shared_type(inner); },
[&](const auto& __v) { return false; }
}, (*t));
}
bool is_shared_map_type(std::shared_ptr<ast::TypeExpr> t) noexcept{
return std::visit(overloaded{[&](const ast::TyGeneric& tyGeneric) { auto [n, targs] = tyGeneric; return (((n == mlc::String("Map", 3)) && (targs.length() >= 2)) && is_shared_type(targs[1])); },
[&](const auto& __v) { return false; }
}, (*t));
}
mlc::String array_elem_name(std::shared_ptr<ast::TypeExpr> t) noexcept{
return std::visit(overloaded{[&](const ast::TyArray& tyArray) { auto [inner] = tyArray; return std::visit(overloaded{[&](const ast::TyNamed& tyNamed) { auto [n] = tyNamed; return n; },
[&](const auto& __v) { return mlc::String("", 0); }
}, (*inner)); },
[&](const auto& __v) { return mlc::String("", 0); }
}, (*t));
}
mlc::Array<mlc::String> collect_shared_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept{
auto result = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < params.length())) {
if (is_shared_type(params[i]->typ)) {
result.push_back(params[i]->name);
}
i = (i + 1);
}
return result;
}
mlc::Array<mlc::String> collect_shared_array_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept{
auto result = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < params.length())) {
if (is_shared_array_type(params[i]->typ)) {
result.push_back(params[i]->name);
}
i = (i + 1);
}
return result;
}
mlc::HashMap<mlc::String, mlc::String> collect_array_elem_types(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept{
auto result = mlc::HashMap<mlc::String, mlc::String>();
auto i = 0;
while ((i < params.length())) {
auto en = array_elem_name(params[i]->typ);
if ((en.length() > 0)) {
result.set(params[i]->name, en);
}
i = (i + 1);
}
return result;
}
mlc::Array<mlc::String> collect_shared_map_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept{
auto result = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < params.length())) {
if (is_shared_map_type(params[i]->typ)) {
result.push_back(params[i]->name);
}
i = (i + 1);
}
return result;
}
mlc::Array<mlc::String> collect_value_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept{
auto result = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < params.length())) {
if (((!is_shared_type(params[i]->typ)) && (!is_shared_array_type(params[i]->typ)))) {
result.push_back(params[i]->name);
}
i = (i + 1);
}
return result;
}
std::shared_ptr<CtorTypeInfo> ctor_type_info_for(mlc::String ctor_name, mlc::Array<std::shared_ptr<ast::TypeExpr>> types) noexcept{
auto shared_positions = mlc::Array<int>{};
auto shared_array_positions = mlc::Array<int>{};
auto index = 0;
while ((index < types.length())) {
if (is_shared_type(types[index])) {
shared_positions.push_back(index);
}
if (is_shared_array_type(types[index])) {
shared_array_positions.push_back(index);
}
index = (index + 1);
}
return std::make_shared<CtorTypeInfo>(CtorTypeInfo{ctor_name, shared_positions, shared_array_positions});
}
std::shared_ptr<ast::TypeExpr> field_def_type(std::shared_ptr<ast::FieldDef> fd) noexcept{
return fd->typ;
}
mlc::Array<std::shared_ptr<CtorTypeInfo>> add_ctor_infos_from_variants(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
auto result = mlc::Array<std::shared_ptr<CtorTypeInfo>>{};
auto vi = 0;
while ((vi < variants.length())) {
std::visit(overloaded{[&](const ast::VarTuple& varTuple) { auto [variant_name, types] = varTuple; return result.push_back(ctor_type_info_for(variant_name, types)); },
[&](const ast::VarRecord& varRecord) { auto [variant_name, field_defs] = varRecord; return [&]() {
auto types = mlc::Array<std::shared_ptr<ast::TypeExpr>>{};
auto fi = 0;
while ((fi < field_defs.length())) {
types.push_back(field_def_type(field_defs[fi]));
fi = (fi + 1);
}
return result.push_back(ctor_type_info_for(variant_name, types));
}(); },
[&](const ast::VarUnit& varUnit) { auto [__0] = varUnit; return /* unit */; }
}, (*variants[vi]));
vi = (vi + 1);
}
return result;
}
mlc::Array<std::shared_ptr<CtorTypeInfo>> build_ctor_type_infos_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept{
auto result = mlc::Array<std::shared_ptr<CtorTypeInfo>>{};
auto i = 0;
while ((i < decls.length())) {
std::visit(overloaded{[&](const ast::DeclExported& declExported) { auto [inner] = declExported; return std::visit(overloaded{[&](const ast::DeclType& declType) { auto [__0, __1, variants] = declType; return [&]() {
auto infos = add_ctor_infos_from_variants(variants);
auto ii = 0;
return [&]() {
while ((ii < infos.length())) {
result.push_back(infos[ii]);
ii = (ii + 1);
}
}();
}(); },
[&](const auto& __v) { return /* unit */; }
}, (*ast::decl_inner(inner))); },
[&](const ast::DeclType& declType) { auto [__0, __1, variants] = declType; return [&]() {
auto infos = add_ctor_infos_from_variants(variants);
auto ii = 0;
return [&]() {
while ((ii < infos.length())) {
result.push_back(infos[ii]);
ii = (ii + 1);
}
}();
}(); },
[&](const auto& __v) { return /* unit */; }
}, (*decls[i]));
i = (i + 1);
}
return result;
}
std::shared_ptr<CtorTypeInfo> lookup_ctor_type_info(mlc::Array<std::shared_ptr<CtorTypeInfo>> infos, mlc::String cti_name) noexcept{
auto result = std::make_shared<CtorTypeInfo>(CtorTypeInfo{mlc::String("", 0), {}, {}});
auto i = 0;
while ((i < infos.length())) {
if ((infos[i]->name == cti_name)) {
result = infos[i];
}
i = (i + 1);
}
return result;
}
CodegenContext make_body_context(CodegenContext base, mlc::Array<mlc::String> shared_params, mlc::Array<mlc::String> shared_array_params, mlc::HashMap<mlc::String, mlc::String> array_elem_types, mlc::Array<mlc::String> shared_map_params, mlc::String self_type, mlc::Array<mlc::String> value_params) noexcept{
return CodegenContext{base.field_orders, base.namespace_prefix, base.qualified, self_type, base.method_owners, shared_params, shared_array_params, array_elem_types, shared_map_params, base.ctor_type_infos, base.variant_types, value_params};
}
CodegenContext context_add_shared(CodegenContext context, mlc::String name) noexcept{
auto new_shared_params = context.shared_params;
new_shared_params.push_back(name);
return make_body_context(context, new_shared_params, context.shared_array_params, context.array_elem_types, context.shared_map_params, context.self_type, context.value_params);
}
CodegenContext context_add_shared_array(CodegenContext context, mlc::String name) noexcept{
auto new_shared_array_params = context.shared_array_params;
new_shared_array_params.push_back(name);
return make_body_context(context, context.shared_params, new_shared_array_params, context.array_elem_types, context.shared_map_params, context.self_type, context.value_params);
}
CodegenContext context_add_value(CodegenContext context, mlc::String name) noexcept{
auto new_value_params = context.value_params;
new_value_params.push_back(name);
return make_body_context(context, context.shared_params, context.shared_array_params, context.array_elem_types, context.shared_map_params, context.self_type, new_value_params);
}
CodegenContext update_context_from_statement(std::shared_ptr<ast::Stmt> stmt, CodegenContext context) noexcept{
return std::visit(overloaded{[&](const ast::StmtLet& stmtLet) { auto [binding_name, __1, typ, value] = stmtLet; return (is_shared_type(typ) ? context_add_shared(context, binding_name) : (is_shared_array_type(typ) ? context_add_shared_array(context, binding_name) : std::visit(overloaded{[&](const ast::ExprMethod& exprMethod) { auto [map_obj, method_name, __2] = exprMethod; return ((method_name == mlc::String("get", 3)) ? std::visit(overloaded{[&](const ast::ExprIdent& exprIdent) { auto [map_name] = exprIdent; return (context.shared_map_params.contains(map_name) ? context_add_shared(context, binding_name) : context_add_value(context, binding_name)); },
[&](const auto& __v) { return context_add_value(context, binding_name); }
}, (*map_obj)) : context_add_value(context, binding_name)); },
[&](const auto& __v) { return context_add_value(context, binding_name); }
}, (*value)))); },
[&](const auto& __v) { return context; }
}, (*stmt));
}
mlc::String template_prefix(mlc::Array<mlc::String> type_params) noexcept{
if ((type_params.length() > 0)) {
return ((mlc::String("template<typename ", 18) + type_params.join(mlc::String(", typename ", 11))) + mlc::String(">\n", 2));
} else {
return mlc::String("", 0);
}
}
mlc::String requires_clause(mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds) noexcept{
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < type_params.length())) {
if ((i < type_bounds.length())) {
auto k = 0;
[&]() {
while ((k < type_bounds[i].length())) {
parts.push_back((((cpp_safe(type_bounds[i][k]) + mlc::String("<", 1)) + type_params[i]) + mlc::String(">", 1)));
k = (k + 1);
}
}();
}
i = (i + 1);
}
return ((parts.length() > 0) ? ((mlc::String("requires ", 9) + parts.join(mlc::String(" && ", 4))) + mlc::String("\n", 1)) : mlc::String("", 0));
}
mlc::String gen_fn_decl(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<ast::TypeExpr> return_type, std::shared_ptr<ast::Expr> body, CodegenContext context) noexcept{
auto prefix = (template_prefix(type_params) + requires_clause(type_params, type_bounds));
auto safe_name = context_resolve(context, name);
auto fn_shared_params = collect_shared_params(params);
auto fn_shared_array_params = collect_shared_array_params(params);
auto fn_array_elem_types = collect_array_elem_types(params);
auto fn_shared_map_params = collect_shared_map_params(params);
auto fn_value_params = collect_value_params(params);
auto body_context = (((params.length() > 0) && (params[0]->name == mlc::String("self", 4))) ? std::visit(overloaded{[&](const ast::TyNamed& tyNamed) { auto [self_type_name] = tyNamed; return [&]() {
auto st = (((self_type_name == mlc::String("Self", 4)) || (self_type_name == mlc::String("self", 4))) ? context.self_type : self_type_name);
return make_body_context(context, fn_shared_params, fn_shared_array_params, fn_array_elem_types, fn_shared_map_params, st, fn_value_params);
}(); },
[&](const auto& __v) { return make_body_context(context, fn_shared_params, fn_shared_array_params, fn_array_elem_types, fn_shared_map_params, context.self_type, fn_value_params); }
}, (*params[0]->typ)) : make_body_context(context, fn_shared_params, fn_shared_array_params, fn_array_elem_types, fn_shared_map_params, context.self_type, fn_value_params));
auto ret_ctx = (((params.length() > 0) && (params[0]->name == mlc::String("self", 4))) ? body_context : context);
return (((name == mlc::String("main", 4)) && (params.length() == 0)) ? [&]() {
auto preamble = mlc::String("mlc::io::set_args(std::vector<mlc::String>(argv + 1, argv + argc));\n", 68);
return (((((((prefix + type_to_cpp(ret_ctx, return_type)) + mlc::String(" ", 1)) + safe_name) + mlc::String("(int argc, char** argv) noexcept{\n", 34)) + preamble) + gen_fn_body(body, body_context)) + mlc::String("}\n", 2));
}() : ((((((((prefix + type_to_cpp(ret_ctx, return_type)) + mlc::String(" ", 1)) + safe_name) + mlc::String("(", 1)) + gen_params(ret_ctx, params)) + mlc::String(") noexcept{\n", 12)) + gen_fn_body(body, body_context)) + mlc::String("}\n", 2)));
}
mlc::String gen_fn_proto(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<ast::TypeExpr> return_type, CodegenContext context) noexcept{
auto prefix = (template_prefix(type_params) + requires_clause(type_params, type_bounds));
auto safe_name = context_resolve(context, name);
auto ret_ctx = (((params.length() > 0) && (params[0]->name == mlc::String("self", 4))) ? std::visit(overloaded{[&](const ast::TyNamed& tyNamed) { auto [tn] = tyNamed; return [&]() {
auto st = (((tn == mlc::String("Self", 4)) || (tn == mlc::String("self", 4))) ? context.self_type : tn);
return make_body_context(context, {}, {}, {}, {}, st, {});
}(); },
[&](const auto& __v) { return context; }
}, (*params[0]->typ)) : context);
return (((name == mlc::String("main", 4)) && (params.length() == 0)) ? ((((prefix + type_to_cpp(ret_ctx, return_type)) + mlc::String(" ", 1)) + safe_name) + mlc::String("(int argc, char** argv) noexcept;\n", 34)) : ((((((prefix + type_to_cpp(ret_ctx, return_type)) + mlc::String(" ", 1)) + safe_name) + mlc::String("(", 1)) + gen_params(ret_ctx, params)) + mlc::String(") noexcept;\n", 12)));
}
mlc::String gen_trait_decl(CodegenContext context, mlc::String trait_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::Decl>> methods) noexcept{
auto self_param = ((type_params.length() > 0) ? type_params[0] : mlc::String("T", 1));
auto tparam = ((type_params.length() > 0) ? type_params.join(mlc::String(", ", 2)) : mlc::String("T", 1));
auto prefix = ((mlc::String("template<typename ", 18) + tparam) + mlc::String(">\n", 2));
auto req_parts = mlc::Array<mlc::String>{};
auto mi = 0;
while ((mi < methods.length())) {
std::visit(overloaded{[&](const ast::DeclFn& declFn) { auto [mangled, __1, __2, params, ret_type, __5] = declFn; return [&]() {
auto method_name = extract_method_name(mangled, trait_name);
auto ret_cpp = type_to_cpp(context, ret_type);
return req_parts.push_back(((((mlc::String("{ ", 2) + method_name) + mlc::String("(self) } -> std::convertible_to<", 32)) + ret_cpp) + mlc::String(">", 1)));
}(); },
[&](const auto& __v) { return /* unit */; }
}, (*methods[mi]));
mi = (mi + 1);
}
auto req_body = req_parts.join(mlc::String("; ", 2));
return (((((((prefix + mlc::String("concept ", 8)) + cpp_safe(trait_name)) + mlc::String(" = requires(const ", 18)) + self_param) + mlc::String("& self) { ", 10)) + req_body) + mlc::String("; };\n", 5));
}
mlc::String gen_type_decl(CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
auto prefix = template_prefix(type_params);
return ((prefix + gen_type_decl_fwd(context, type_name, variants)) + gen_type_decl_body(context, type_name, variants));
}
mlc::String gen_decl(std::shared_ptr<ast::Decl> decl, CodegenContext context) noexcept{
return std::visit(overloaded{[&](const ast::DeclType& declType) { auto [type_name, type_params, variants] = declType; return gen_type_decl(context, type_name, type_params, variants); },
[&](const ast::DeclTrait& declTrait) { auto [name, type_params, methods] = declTrait; return gen_trait_decl(context, name, type_params, methods); },
[&](const ast::DeclFn& declFn) { auto [name, type_params, type_bounds, params, return_type, body] = declFn; return std::visit(overloaded{[&](const ast::ExprExtern& exprExtern) { return mlc::String("", 0); },
[&](const auto& __v) { return gen_fn_decl(name, type_params, type_bounds, params, return_type, body, context); }
}, (*body)); },
[&](const ast::DeclExtend& declExtend) { auto [type_name, trait_name, methods] = declExtend; return [&]() {
auto ext_ctx = make_body_context(context, {}, {}, {}, {}, type_name, {});
auto parts = mlc::Array<mlc::String>{};
auto index = 0;
while ((index < methods.length())) {
std::visit(overloaded{[&](const ast::DeclFn& declFn) { auto [mangled, __1, __2, params, ret_type, body] = declFn; return std::visit(overloaded{[&](const ast::ExprExtern& exprExtern) { return [&]() {
auto method_name = ((trait_name.length() > 0) ? extract_method_name(mangled, type_name) : mlc::String("", 0));
if ((((type_name == mlc::String("i32", 3)) && (method_name == mlc::String("to_string", 9))) && (params.length() > 0))) {
auto ret_cpp = type_to_cpp(context, ret_type);
auto params_str = gen_params(context, params);
parts.push_back((((((((ret_cpp + mlc::String(" ", 1)) + context_resolve(context, mangled)) + mlc::String("(", 1)) + params_str) + mlc::String(") noexcept { return mlc::to_string(", 35)) + cpp_safe(params[0]->name)) + mlc::String("); }\n", 5)));
} else {
/* unit */;
}
return /* unit */;
}(); },
[&](const auto& __v) { return parts.push_back(gen_decl(methods[index], ext_ctx)); }
}, (*body)); },
[&](const auto& __v) { return parts.push_back(gen_decl(methods[index], ext_ctx)); }
}, (*methods[index]));
index = (index + 1);
}
auto wrappers = mlc::String("", 0);
if ((trait_name.length() > 0)) {
auto mi = 0;
[&]() {
while ((mi < methods.length())) {
std::visit(overloaded{[&](const ast::DeclFn& declFn) { auto [mangled, __1, __2, params, ret_type, __5] = declFn; return [&]() {
auto wrapper_ctx = make_body_context(context, {}, {}, {}, {}, type_name, {});
auto method_name = extract_method_name(mangled, type_name);
auto ret_cpp = type_to_cpp(wrapper_ctx, ret_type);
auto fn_resolved = context_resolve(context, mangled);
auto params_str = gen_params(wrapper_ctx, params);
auto call_args = mlc::Array<mlc::String>{};
auto pi = 0;
while ((pi < params.length())) {
call_args.push_back(cpp_safe(params[pi]->name));
pi = (pi + 1);
}
wrappers = (((((((((((wrappers + mlc::String("inline ", 7)) + ret_cpp) + mlc::String(" ", 1)) + cpp_safe(method_name)) + mlc::String("(", 1)) + params_str) + mlc::String(") noexcept { return ", 20)) + fn_resolved) + mlc::String("(", 1)) + call_args.join(mlc::String(", ", 2))) + mlc::String("); }\n", 5));
return /* unit */;
}(); },
[&](const auto& __v) { return /* unit */; }
}, (*methods[mi]));
mi = (mi + 1);
}
}();
}
auto methods_str = parts.join(mlc::String("", 0));
return ((trait_name.length() > 0) ? [&]() {
auto cpp_type = type_name_to_cpp(context, type_name);
auto trait_safe = cpp_safe(trait_name);
return ((((((((((methods_str + wrappers) + mlc::String("static_assert(", 14)) + trait_safe) + mlc::String("<", 1)) + cpp_type) + mlc::String(">, \"", 4)) + type_name) + mlc::String(" does not implement ", 20)) + trait_name) + mlc::String("\");\n", 4));
}() : methods_str);
}(); },
[&](const ast::DeclImport& declImport) { auto [__0, __1] = declImport; return mlc::String("", 0); },
[&](const ast::DeclExported& declExported) { auto [d] = declExported; return gen_decl(d, context); }
}, (*decl));
}
mlc::String gen_extend_wrapper_protos(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<ast::Decl>> methods, CodegenContext context) noexcept{
if ((trait_name.length() == 0)) {
return mlc::String("", 0);
} else {
auto wrappers = mlc::String("", 0);
auto mi = 0;
while ((mi < methods.length())) {
std::visit(overloaded{[&](const ast::DeclFn& declFn) { auto [mangled, __1, __2, params, ret_type, __5] = declFn; return [&]() {
auto wrapper_ctx = make_body_context(context, {}, {}, {}, {}, type_name, {});
auto method_name = extract_method_name(mangled, type_name);
auto ret_cpp = type_to_cpp(wrapper_ctx, ret_type);
auto params_str = gen_params(wrapper_ctx, params);
wrappers = ((((((wrappers + ret_cpp) + mlc::String(" ", 1)) + cpp_safe(method_name)) + mlc::String("(", 1)) + params_str) + mlc::String(") noexcept;\n", 12));
return /* unit */;
}(); },
[&](const auto& __v) { return /* unit */; }
}, (*methods[mi]));
mi = (mi + 1);
}
return wrappers;
}
}
mlc::String gen_proto(std::shared_ptr<ast::Decl> decl, CodegenContext context) noexcept{
return std::visit(overloaded{[&](const ast::DeclType& declType) { auto [__0, __1, __2] = declType; return mlc::String("", 0); },
[&](const ast::DeclTrait& declTrait) { auto [__0, __1, __2] = declTrait; return mlc::String("", 0); },
[&](const ast::DeclFn& declFn) { auto [name, type_params, type_bounds, params, ret, body] = declFn; return std::visit(overloaded{[&](const ast::ExprExtern& exprExtern) { return mlc::String("", 0); },
[&](const auto& __v) { return gen_fn_proto(name, type_params, type_bounds, params, ret, context); }
}, (*body)); },
[&](const ast::DeclExtend& declExtend) { auto [type_name, trait_name, methods] = declExtend; return [&]() {
auto ext_ctx = make_body_context(context, {}, {}, {}, {}, type_name, {});
auto parts = mlc::Array<mlc::String>{};
auto index = 0;
while ((index < methods.length())) {
parts.push_back(gen_proto(methods[index], ext_ctx));
index = (index + 1);
}
return parts.join(mlc::String("", 0));
}(); },
[&](const ast::DeclImport& declImport) { auto [__0, __1] = declImport; return mlc::String("", 0); },
[&](const ast::DeclExported& declExported) { auto [d] = declExported; return gen_proto(d, context); }
}, (*decl));
}
mlc::Array<mlc::String> collect_decl_parts(mlc::Array<std::shared_ptr<ast::Decl>> decls, CodegenContext context, int phase) noexcept{
auto parts = mlc::Array<mlc::String>{};
if ((phase == 0)) {
auto i = 0;
[&]() {
while ((i < decls.length())) {
auto w = std::visit(overloaded{[&](const ast::DeclExtend& declExtend) { auto [tn, tr, m] = declExtend; return gen_extend_wrapper_protos(tn, tr, m, context); },
[&](const ast::DeclExported& declExported) { auto [d] = declExported; return std::visit(overloaded{[&](const ast::DeclExtend& declExtend) { auto [tn, tr, m] = declExtend; return gen_extend_wrapper_protos(tn, tr, m, context); },
[&](const auto& __v) { return mlc::String("", 0); }
}, (*ast::decl_inner(d))); },
[&](const auto& __v) { return mlc::String("", 0); }
}, (*decls[i]));
if ((w.length() > 0)) {
parts.push_back(w);
}
i = (i + 1);
}
}();
}
auto index = 0;
while ((index < decls.length())) {
auto segment = std::visit(overloaded{[&](const ast::DeclType& declType) { auto [type_name, __1, variants] = declType; return ((phase == 0) ? gen_type_decl_fwd(context, type_name, variants) : ((phase == 1) ? gen_type_decl_body(context, type_name, variants) : mlc::String("", 0))); },
[&](const ast::DeclTrait& declTrait) { auto [name, type_params, methods] = declTrait; return ((phase == 0) ? gen_trait_decl(context, name, type_params, methods) : mlc::String("", 0)); },
[&](const ast::DeclFn& declFn) { auto [__0, __1, __2, __3, __4, __5] = declFn; return ((phase == 2) ? gen_proto(decls[index], context) : ((phase == 3) ? gen_decl(decls[index], context) : mlc::String("", 0))); },
[&](const ast::DeclExtend& declExtend) { auto [__0, __1, __2] = declExtend; return ((phase == 2) ? gen_proto(decls[index], context) : ((phase == 3) ? gen_decl(decls[index], context) : mlc::String("", 0))); },
[&](const ast::DeclImport& declImport) { auto [__0, __1] = declImport; return mlc::String("", 0); },
[&](const ast::DeclExported& declExported) { auto [d] = declExported; return ((phase == 0) ? std::visit(overloaded{[&](const ast::DeclType& declType) { auto [type_name, __1, variants] = declType; return gen_type_decl_fwd(context, type_name, variants); },
[&](const ast::DeclTrait& declTrait) { auto [name, type_params, methods] = declTrait; return gen_trait_decl(context, name, type_params, methods); },
[&](const auto& __v) { return mlc::String("", 0); }
}, (*ast::decl_inner(d))) : ((phase == 1) ? std::visit(overloaded{[&](const ast::DeclType& declType) { auto [type_name, __1, variants] = declType; return gen_type_decl_body(context, type_name, variants); },
[&](const auto& __v) { return mlc::String("", 0); }
}, (*ast::decl_inner(d))) : ((phase == 2) ? gen_proto(d, context) : ((phase == 3) ? std::visit(overloaded{[&](const ast::DeclType& declType) { auto [__0, __1, __2] = declType; return mlc::String("", 0); },
[&](const ast::DeclTrait& declTrait) { auto [__0, __1, __2] = declTrait; return mlc::String("", 0); },
[&](const auto& __v) { return gen_decl(d, context); }
}, (*ast::decl_inner(d))) : mlc::String("", 0))))); }
}, (*decls[index]));
if ((segment.length() > 0)) {
parts.push_back(segment);
}
index = (index + 1);
}
return parts;
}
mlc::String gen_program(ast::Program prog) noexcept{
auto context = create_codegen_context(prog);
auto preamble = mlc::Array{mlc::String("#include \"mlc/core/string.hpp\"\n", 31), mlc::String("#include \"mlc/core/array.hpp\"\n", 30), mlc::String("#include \"mlc/core/hashmap.hpp\"\n", 32), mlc::String("#include \"mlc/core/match.hpp\"\n", 30), mlc::String("#include \"mlc/io/io.hpp\"\n", 25), mlc::String("#include \"mlc/io/file.hpp\"\n", 27), mlc::String("#include <functional>\n", 22), mlc::String("#include <memory>\n", 18), mlc::String("#include <variant>\n", 19), mlc::String("#include <vector>\n\n", 19)};
auto type_fwds = collect_decl_parts(prog.decls, context, 0);
auto type_defs = collect_decl_parts(prog.decls, context, 1);
auto fn_protos = collect_decl_parts(prog.decls, context, 2);
auto fn_decls = collect_decl_parts(prog.decls, context, 3);
return ast::errs_append(ast::errs_append(ast::errs_append(ast::errs_append(preamble, type_fwds), type_defs), ast::errs_append(fn_protos, mlc::Array{mlc::String("\n", 1)})), fn_decls).join(mlc::String("", 0));
}
mlc::String include_lines(mlc::Array<mlc::String> import_paths) noexcept{
auto lines = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < import_paths.length())) {
lines.push_back(((mlc::String("#include \"", 10) + path_to_module_base(import_paths[i])) + mlc::String(".hpp\"\n", 6)));
i = (i + 1);
}
return lines.join(mlc::String("", 0));
}
PrecomputedCtx precompute(ast::Program prog, mlc::Array<LoadItem> all_items) noexcept{
return PrecomputedCtx{build_field_orders(prog), build_variant_types_from_decls(prog.decls), build_item_index(all_items), build_ctor_type_infos_from_decls(prog.decls)};
}
GenModuleOut gen_module(LoadItem item, mlc::Array<LoadItem> all_items, ast::Program full_prog, PrecomputedCtx precomp) noexcept{
auto base = path_to_module_base(item.path);
auto qualified = build_qualified(item.imports, all_items);
auto context = CodegenContext{precomp.field_orders, mlc::String("", 0), qualified, mlc::String("", 0), build_method_owners_from_decls(full_prog.decls), {}, {}, {}, {}, precomp.ctor_type_infos, precomp.variant_types, {}};
auto module_namespace = ((base == mlc::String("main", 4)) ? mlc::String("mlc_main", 8) : base);
auto std_includes = ((mlc::String("#include \"mlc.hpp\"\n#include <variant>\n\n", 39) + include_lines(item.imports)) + mlc::String("\n", 1));
auto type_fwds = collect_decl_parts(item.decls, context, 0);
auto type_defs = collect_decl_parts(item.decls, context, 1);
auto fn_protos = collect_decl_parts(item.decls, context, 2);
auto fn_defs = collect_decl_parts(item.decls, context, 3);
auto guard = (base.upper() + mlc::String("_HPP", 4));
auto header = mlc::Array{((mlc::String("#ifndef ", 8) + guard) + mlc::String("\n", 1)), ((mlc::String("#define ", 8) + guard) + mlc::String("\n", 1)), mlc::String("\n", 1), std_includes, ((mlc::String("namespace ", 10) + module_namespace) + mlc::String(" {\n", 3)), mlc::String("\n", 1), type_fwds.join(mlc::String("", 0)), type_defs.join(mlc::String("", 0)), fn_protos.join(mlc::String("", 0)), mlc::String("\n", 1), ((mlc::String("} // namespace ", 15) + module_namespace) + mlc::String("\n", 1)), mlc::String("\n", 1), ((mlc::String("#endif // ", 10) + guard) + mlc::String("\n", 1))}.join(mlc::String("", 0));
auto cli_wrapper = ((base == mlc::String("main", 4)) ? mlc::String("\n#undef main\n\nstatic void mlc_cli_set_args(int argc, char** argv) {\n  std::vector<mlc::String> arguments;\n  arguments.reserve(argc > 0 ? argc - 1 : 0);\n  for (int i = 1; i < argc; ++i) { arguments.emplace_back(argv[i]); }\n  mlc::io::set_args(std::move(arguments));\n}\n\nint main(int argc, char** argv) {\n  mlc_cli_set_args(argc, argv);\n  return ::mlc_main::mlc_user_main(argc, argv);\n}\n", 384) : mlc::String("", 0));
auto impl = mlc::Array{mlc::String("#define main mlc_user_main\n", 27), ((mlc::String("#include \"", 10) + base) + mlc::String(".hpp\"\n", 6)), mlc::String("\n", 1), ((mlc::String("namespace ", 10) + module_namespace) + mlc::String(" {\n", 3)), mlc::String("\n", 1), fn_defs.join(mlc::String("", 0)), mlc::String("\n", 1), ((mlc::String("} // namespace ", 15) + module_namespace) + mlc::String("\n", 1)), cli_wrapper}.join(mlc::String("", 0));
return GenModuleOut{header, impl};
}

} // namespace codegen
