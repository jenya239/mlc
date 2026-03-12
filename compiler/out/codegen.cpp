#define main mlc_user_main
#include "codegen.hpp"

namespace codegen {

mlc::String path_to_module_base(mlc::String path) noexcept{
auto last_slash = (-1);
auto last_dot = path.length();
auto i = 0;
while ((i < path.length())) {
if ((path.char_at(i) == mlc::String("/", 1))) {
(last_slash = i);
} else if (((path.char_at(i) == mlc::String(".", 1)) && (i > last_slash))) {
(last_dot = i);
}

i = (i + 1);
}
return ((last_dot > last_slash) ? path.substring((last_slash + 1), ((last_dot - last_slash) - 1)) : path.substring((last_slash + 1), ((path.length() - last_slash) - 1)));
}
mlc::Array<mlc::String> decl_export_names(std::shared_ptr<ast::Decl> decl) noexcept{
auto names = mlc::Array<mlc::String>{};
std::visit(overloaded{[&](const ast::DeclExported& declExported) { auto [d] = declExported; return [&]() {
std::visit(overloaded{[&](const ast::DeclFn& declFn) { auto [name, __1, __2, __3] = declFn; return [&]() {
names.push_back(name);
/* unit */;
return /* unit */;
}(); },
[&](const ast::DeclType& declType) { auto [type_name, variants] = declType; return [&]() {
names.push_back(type_name);
auto j = 0;
while ((j < variants.length())) {
std::visit(overloaded{[&](const ast::VarRecord& varRecord) { auto [n, __1] = varRecord; return [&]() {
names.push_back(n);
/* unit */;
return /* unit */;
}(); },
[&](const ast::VarTuple& varTuple) { auto [n, __1] = varTuple; return [&]() {
names.push_back(n);
/* unit */;
return /* unit */;
}(); },
[&](const ast::VarUnit& varUnit) { auto [n] = varUnit; return [&]() {
names.push_back(n);
/* unit */;
return /* unit */;
}(); }
}, (*variants[j]));
j = (j + 1);
}
/* unit */;
return /* unit */;
}(); },
[&](const ast::DeclExtend& declExtend) { auto [__0, methods] = declExtend; return [&]() {
auto mi = 0;
while ((mi < methods.length())) {
std::visit(overloaded{[&](const ast::DeclFn& declFn) { auto [fn_name, __1, __2, __3] = declFn; return [&]() {
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
mlc::HashMap<mlc::String, mlc::String> add_exports_to_qualified(mlc::HashMap<mlc::String, mlc::String> q, mlc::String imp_path, mlc::Array<LoadItem> all_items) noexcept{
auto prefix = (path_to_module_base(imp_path) + mlc::String("::", 2));
auto item_i = 0;
while ((item_i < all_items.length())) {
auto cur_item = all_items[item_i];
if ((cur_item.path == imp_path)) {
auto d = 0;
[&]() {
while ((d < cur_item.decls.length())) {
auto n_i = 0;
auto exp_names = decl_export_names(cur_item.decls[d]);
while ((n_i < exp_names.length())) {
if ((!q.has(exp_names[n_i]))) {
q.set(exp_names[n_i], prefix);
}
n_i = (n_i + 1);
}
d = (d + 1);
}
}();
}
item_i = (item_i + 1);
}
return q;
}
mlc::HashMap<mlc::String, mlc::String> build_qualified(mlc::Array<mlc::String> import_paths, mlc::Array<LoadItem> all_items) noexcept{
auto q = mlc::HashMap<mlc::String, mlc::String>();
auto imp_i = 0;
while ((imp_i < import_paths.length())) {
auto imp_path = import_paths[imp_i];
q = add_exports_to_qualified(q, imp_path, all_items);
auto item_i = 0;
while ((item_i < all_items.length())) {
auto cur_item = all_items[item_i];
if ((cur_item.path == imp_path)) {
auto trans_i = 0;
[&]() {
while ((trans_i < cur_item.imports.length())) {
q = add_exports_to_qualified(q, cur_item.imports[trans_i], all_items);
trans_i = (trans_i + 1);
}
}();
}
item_i = (item_i + 1);
}
imp_i = (imp_i + 1);
}
return q;
}
mlc::String extract_method_name(mlc::String fn_name, mlc::String type_name) noexcept{
auto prefix = (type_name + mlc::String("_", 1));
return (((fn_name.length() > prefix.length()) && (fn_name.substring(0, prefix.length()) == prefix)) ? fn_name.substring(prefix.length(), (fn_name.length() - prefix.length())) : fn_name);
}
mlc::HashMap<mlc::String, mlc::String> add_method_owners_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls, mlc::HashMap<mlc::String, mlc::String> m) noexcept{
auto i = 0;
while ((i < decls.length())) {
std::visit(overloaded{[&](const ast::DeclExtend& declExtend) { auto [type_name, methods] = declExtend; return [&]() {
auto j = 0;
return [&]() {
while ((j < methods.length())) {
std::visit(overloaded{[&](const ast::DeclFn& declFn) { auto [fn_name, __1, __2, __3] = declFn; return [&]() {
auto unmangled = extract_method_name(fn_name, type_name);
if ((!m.has(unmangled))) {
m.set(unmangled, fn_name);
}
return /* unit */;
}(); },
[&](const auto& __v) { return /* unit */; }
}, (*methods[j]));
j = (j + 1);
}
}();
}(); },
[&](const auto& __v) { return /* unit */; }
}, (*ast::decl_inner(decls[i])));
i = (i + 1);
}
return m;
}
mlc::HashMap<mlc::String, mlc::String> build_method_owners_for(LoadItem item, mlc::Array<LoadItem> all_items) noexcept{
auto m = mlc::HashMap<mlc::String, mlc::String>();
m = add_method_owners_from_decls(item.decls, m);
auto imp_i = 0;
while ((imp_i < item.imports.length())) {
auto imp_path = item.imports[imp_i];
auto ai = 0;
while ((ai < all_items.length())) {
auto ai_item = all_items[ai];
if ((ai_item.path == imp_path)) {
m = add_method_owners_from_decls(ai_item.decls, m);
auto ti = 0;
[&]() {
while ((ti < ai_item.imports.length())) {
auto t_path = ai_item.imports[ti];
auto bi = 0;
while ((bi < all_items.length())) {
auto bi_item = all_items[bi];
if ((bi_item.path == t_path)) {
(m = add_method_owners_from_decls(bi_item.decls, m));
}
bi = (bi + 1);
}
ti = (ti + 1);
}
}();
}
ai = (ai + 1);
}
imp_i = (imp_i + 1);
}
return m;
}
mlc::HashMap<mlc::String, mlc::String> build_method_owners_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept{
return add_method_owners_from_decls(decls, {});
}
mlc::HashMap<mlc::String, mlc::String> build_variant_types_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept{
auto m = mlc::HashMap<mlc::String, mlc::String>();
auto i = 0;
while ((i < decls.length())) {
auto d = ast::decl_inner(decls[i]);
std::visit(overloaded{[&](const ast::DeclType& declType) { auto [type_name, variants] = declType; return [&]() {
auto j = 0;
return [&]() {
while ((j < variants.length())) {
std::visit(overloaded{[&](const ast::VarRecord& varRecord) { auto [vn, __1] = varRecord; return m.set(vn, type_name); },
[&](const ast::VarTuple& varTuple) { auto [vn, __1] = varTuple; return m.set(vn, type_name); },
[&](const ast::VarUnit& varUnit) { auto [vn] = varUnit; return m.set(vn, type_name); }
}, (*variants[j]));
j = (j + 1);
}
}();
}(); },
[&](const auto& __v) { return /* unit */; }
}, (*d));
i = (i + 1);
}
return m;
}
CodegenContext create_codegen_context(ast::Program prog) noexcept{
return CodegenContext{build_field_orders(prog), mlc::String("", 0), {}, mlc::String("", 0), build_method_owners_from_decls(prog.decls), {}, build_variant_types_from_decls(prog.decls)};
}
mlc::String ctx_resolve(CodegenContext ctx, mlc::String name) noexcept{
if (ctx.qualified.has(name)) {
return (ctx.qualified.get(name) + cpp_safe(name));
} else if ((ctx.ns_prefix.length() > 0)) {
return (ctx.ns_prefix + cpp_safe(name));
} else {
return cpp_safe(name);
}

}
mlc::Array<std::shared_ptr<FieldOrder>> build_field_orders(ast::Program prog) noexcept{
auto orders = mlc::Array<std::shared_ptr<FieldOrder>>{};
auto i = 0;
while ((i < prog.decls.length())) {
std::visit(overloaded{[&](const ast::DeclExported& declExported) { auto [d] = declExported; return std::visit(overloaded{[&](const ast::DeclType& declType) { auto [type_name, variants] = declType; return [&]() {
auto j = 0;
while ((j < variants.length())) {
std::visit(overloaded{[&](const ast::VarRecord& varRecord) { auto [variant_name, field_defs] = varRecord; return [&]() {
auto field_names = mlc::Array<mlc::String>{};
auto k = 0;
while ((k < field_defs.length())) {
field_names.push_back(field_defs[k]->name);
k = (k + 1);
}
orders.push_back(std::make_shared<FieldOrder>(FieldOrder{variant_name, field_names}));
if ((variants.length() == 1)) {
orders.push_back(std::make_shared<FieldOrder>(FieldOrder{type_name, field_names}));
}
auto _d1 = 0;
return /* unit */;
}(); },
[&](const ast::VarTuple& varTuple) { auto [__0, __1] = varTuple; return /* unit */; },
[&](const ast::VarUnit& varUnit) { auto [__0] = varUnit; return /* unit */; }
}, (*variants[j]));
j = (j + 1);
}
auto _d2 = 0;
return /* unit */;
}(); },
[&](const ast::DeclFn& declFn) { auto [__0, __1, __2, __3] = declFn; return /* unit */; },
[&](const ast::DeclExtend& declExtend) { auto [__0, __1] = declExtend; return /* unit */; },
[&](const ast::DeclImport& declImport) { auto [__0, __1] = declImport; return /* unit */; },
[&](const ast::DeclExported& declExported) { auto [__0] = declExported; return /* unit */; }
}, (*ast::decl_inner(d))); },
[&](const ast::DeclType& declType) { auto [type_name, variants] = declType; return [&]() {
auto j = 0;
while ((j < variants.length())) {
std::visit(overloaded{[&](const ast::VarRecord& varRecord) { auto [variant_name, field_defs] = varRecord; return [&]() {
auto field_names = mlc::Array<mlc::String>{};
auto k = 0;
while ((k < field_defs.length())) {
field_names.push_back(field_defs[k]->name);
k = (k + 1);
}
orders.push_back(std::make_shared<FieldOrder>(FieldOrder{variant_name, field_names}));
if ((variants.length() == 1)) {
orders.push_back(std::make_shared<FieldOrder>(FieldOrder{type_name, field_names}));
}
auto _d1 = 0;
return /* unit */;
}(); },
[&](const ast::VarTuple& varTuple) { auto [__0, __1] = varTuple; return /* unit */; },
[&](const ast::VarUnit& varUnit) { auto [__0] = varUnit; return /* unit */; }
}, (*variants[j]));
j = (j + 1);
}
auto _d2 = 0;
return /* unit */;
}(); },
[&](const ast::DeclFn& declFn) { auto [__0, __1, __2, __3] = declFn; return /* unit */; },
[&](const ast::DeclExtend& declExtend) { auto [__0, __1] = declExtend; return /* unit */; },
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
(found = true);
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
(found = true);
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
mlc::String lower_first(mlc::String name) noexcept{
if ((name.length() == 0)) {
return name;
} else {
return (name.char_at(0).lower() + name.substring(1, name.length()));
}
}
mlc::String type_to_cpp(CodegenContext ctx, std::shared_ptr<ast::TypeExpr> type_expr) noexcept{
return std::visit(overloaded{[&](const ast::TyI32& tyI32) { return mlc::String("int", 3); },
[&](const ast::TyString& tyString) { return mlc::String("mlc::String", 11); },
[&](const ast::TyBool& tyBool) { return mlc::String("bool", 4); },
[&](const ast::TyUnit& tyUnit) { return mlc::String("void", 4); },
[&](const ast::TyNamed& tyNamed) { auto [name] = tyNamed; return ctx_resolve(ctx, name); },
[&](const ast::TyArray& tyArray) { auto [inner] = tyArray; return ((mlc::String("mlc::Array<", 11) + type_to_cpp(ctx, inner)) + mlc::String(">", 1)); },
[&](const ast::TyShared& tyShared) { auto [inner] = tyShared; return ((mlc::String("std::shared_ptr<", 16) + type_to_cpp(ctx, inner)) + mlc::String(">", 1)); },
[&](const ast::TyGeneric& tyGeneric) { auto [name, targs] = tyGeneric; return (((name == mlc::String("ref", 3)) && (targs.length() == 1)) ? (type_to_cpp(ctx, targs[0]) + mlc::String("&", 1)) : [&]() {
auto safe = ((name == mlc::String("Map", 3)) ? mlc::String("mlc::HashMap", 12) : ((name == mlc::String("Shared", 6)) ? mlc::String("std::shared_ptr", 15) : ctx_resolve(ctx, name)));
return ((targs.length() == 0) ? safe : ((targs.length() == 1) ? (((safe + mlc::String("<", 1)) + type_to_cpp(ctx, targs[0])) + mlc::String(">", 1)) : (((((safe + mlc::String("<", 1)) + type_to_cpp(ctx, targs[0])) + mlc::String(", ", 2)) + type_to_cpp(ctx, targs[1])) + mlc::String(">", 1))));
}()); },
[&](const ast::TyFn& tyFn) { auto [params, ret] = tyFn; return [&]() {
auto param_types = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < params.length())) {
param_types.push_back(type_to_cpp(ctx, params[i]));
i = (i + 1);
}
return ((((mlc::String("std::function<", 14) + type_to_cpp(ctx, ret)) + mlc::String("(", 1)) + param_types.join(mlc::String(", ", 2))) + mlc::String(")>", 2));
}(); }
}, (*type_expr));
}
mlc::String escape_str(mlc::String input) noexcept{
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < input.length())) {
auto c = input.char_at(i);
if ((c == mlc::String("\\", 1))) {
parts.push_back(mlc::String("\\\\", 2));
} else if ((c == mlc::String("\"", 1))) {
parts.push_back(mlc::String("\\\"", 2));
} else if ((c == mlc::String("\n", 1))) {
parts.push_back(mlc::String("\\n", 2));
} else if ((c == mlc::String("\r", 1))) {
parts.push_back(mlc::String("\\r", 2));
} else if ((c == mlc::String("\t", 1))) {
parts.push_back(mlc::String("\\t", 2));
} else if ((c == mlc::String("\0", 1))) {
parts.push_back(mlc::String("\\0", 2));
} else {
parts.push_back(c);
}





i = (i + 1);
}
return parts.join(mlc::String("", 0));
}
mlc::String gen_args(mlc::Array<std::shared_ptr<ast::Expr>> exprs, CodegenContext ctx) noexcept{
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < exprs.length())) {
parts.push_back(gen_expr(exprs[i], ctx));
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
mlc::String gen_stmts_str(mlc::Array<std::shared_ptr<ast::Stmt>> stmts, CodegenContext ctx) noexcept{
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < stmts.length())) {
parts.push_back(gen_stmt(stmts[i], ctx));
i = (i + 1);
}
return parts.join(mlc::String("", 0));
}
mlc::String gen_block_body(std::shared_ptr<ast::Expr> expr, CodegenContext ctx) noexcept{
return std::visit(overloaded{[&](const ast::ExprBlock& exprBlock) { auto [stmts, result] = exprBlock; return [&]() {
auto stmts_code = gen_stmts_str(stmts, ctx);
auto result_code = gen_expr(result, ctx);
return ((result_code == mlc::String("/* unit */", 10)) ? stmts_code : ((stmts_code + result_code) + mlc::String(";\n", 2)));
}(); },
[&](const ast::ExprIf& exprIf) { auto [__0, __1, __2] = exprIf; return gen_stmt_expr(expr, ctx); },
[&](const auto& __v) { return (gen_expr(expr, ctx) + mlc::String(";\n", 2)); }
}, (*expr));
}
mlc::String gen_expr(std::shared_ptr<ast::Expr> expr, CodegenContext ctx) noexcept{
return std::visit(overloaded{[&](const ast::ExprInt& exprInt) { auto [n] = exprInt; return mlc::to_string(n); },
[&](const ast::ExprStr& exprStr) { auto [s] = exprStr; return ((((mlc::String("mlc::String(\"", 13) + escape_str(s)) + mlc::String("\", ", 3)) + mlc::to_string(s.length())) + mlc::String(")", 1)); },
[&](const ast::ExprBool& exprBool) { auto [b] = exprBool; return (b ? mlc::String("true", 4) : mlc::String("false", 5)); },
[&](const ast::ExprUnit& exprUnit) { return mlc::String("/* unit */", 10); },
[&](const ast::ExprExtern& exprExtern) { return mlc::String("", 0); },
[&](const ast::ExprIdent& exprIdent) { auto [name] = exprIdent; return ((ctx.self_type.length() > 0) ? [&]() {
auto self_fields = lookup_fields(ctx.field_orders, ctx.self_type);
return (list_contains(self_fields, name) ? (mlc::String("self.", 5) + cpp_safe(name)) : [&]() {
auto mapped = map_builtin(name);
auto safe = ctx_resolve(ctx, mapped);
auto needs_brace = (((name.length() > 0) && (name.char_at(0) >= mlc::String("A", 1))) && (name.char_at(0) <= mlc::String("Z", 1)));
return (needs_brace ? (safe + mlc::String("{}", 2)) : safe);
}());
}() : [&]() {
auto mapped = map_builtin(name);
auto safe = ctx_resolve(ctx, mapped);
auto needs_brace = (((name.length() > 0) && (name.char_at(0) >= mlc::String("A", 1))) && (name.char_at(0) <= mlc::String("Z", 1)));
return (needs_brace ? (safe + mlc::String("{}", 2)) : safe);
}()); },
[&](const ast::ExprBin& exprBin) { auto [op, left_expr, right_expr] = exprBin; return ((((((mlc::String("(", 1) + gen_expr(left_expr, ctx)) + mlc::String(" ", 1)) + op) + mlc::String(" ", 1)) + gen_expr(right_expr, ctx)) + mlc::String(")", 1)); },
[&](const ast::ExprUn& exprUn) { auto [op, inner] = exprUn; return (((mlc::String("(", 1) + op) + gen_expr(inner, ctx)) + mlc::String(")", 1)); },
[&](const ast::ExprCall& exprCall) { auto [func, call_args] = exprCall; return [&]() {
auto fn_code = std::visit(overloaded{[&](const ast::ExprIdent& exprIdent) { auto [name] = exprIdent; return ctx_resolve(ctx, map_builtin(name)); },
[&](const auto& __v) { return gen_expr(func, ctx); }
}, (*func));
return (((fn_code + mlc::String("(", 1)) + gen_args(call_args, ctx)) + mlc::String(")", 1));
}(); },
[&](const ast::ExprMethod& exprMethod) { auto [obj, method_name, margs] = exprMethod; return [&]() {
auto obj_code = std::visit(overloaded{[&](const ast::ExprIdent& exprIdent) { auto [name] = exprIdent; return (((ctx.self_type.length() > 0) && list_contains(lookup_fields(ctx.field_orders, ctx.self_type), name)) ? (mlc::String("self.", 5) + cpp_safe(name)) : ctx_resolve(ctx, map_builtin(name))); },
[&](const auto& __v) { return gen_expr(obj, ctx); }
}, (*obj));
return ((obj_code == mlc::String("File", 4)) ? [&]() {
auto cpp_fn = ((method_name == mlc::String("read", 4)) ? mlc::String("mlc::file::read_to_string", 25) : ((method_name == mlc::String("write", 5)) ? mlc::String("mlc::file::write_string", 23) : (mlc::String("mlc::file::", 11) + method_name)));
return (((cpp_fn + mlc::String("(", 1)) + gen_args(margs, ctx)) + mlc::String(")", 1));
}() : (((obj_code == mlc::String("Map", 3)) && (method_name == mlc::String("new", 3))) ? mlc::String("{}", 2) : ((((obj_code == mlc::String("Shared", 6)) && (method_name == mlc::String("new", 3))) && (margs.length() == 1)) ? [&]() {
auto arg_code = gen_expr(margs[0], ctx);
auto raw_name = std::visit(overloaded{[&](const ast::ExprRecord& exprRecord) { auto [n, __1] = exprRecord; return n; },
[&](const ast::ExprCall& exprCall) { auto [fn_expr, __1] = exprCall; return std::visit(overloaded{[&](const ast::ExprIdent& exprIdent) { auto [n] = exprIdent; return n; },
[&](const auto& __v) { return mlc::String("", 0); }
}, (*fn_expr)); },
[&](const ast::ExprIdent& exprIdent) { auto [n] = exprIdent; return n; },
[&](const auto& __v) { return mlc::String("", 0); }
}, (*margs[0]));
auto parent = (((raw_name.length() > 0) && ctx.variant_types.has(raw_name)) ? ctx.variant_types.get(raw_name) : raw_name);
auto type_name = ((parent.length() > 0) ? ctx_resolve(ctx, parent) : mlc::String("auto", 4));
return ((((mlc::String("std::make_shared<", 17) + type_name) + mlc::String(">(", 2)) + arg_code) + mlc::String(")", 1));
}() : (((method_name == mlc::String("to_string", 9)) && (margs.length() == 0)) ? ((mlc::String("mlc::to_string(", 15) + obj_code) + mlc::String(")", 1)) : (ctx.method_owners.has(method_name) ? [&]() {
auto mangled = ctx.method_owners.get(method_name);
auto fn_name = ctx_resolve(ctx, mangled);
return ((margs.length() == 0) ? (((fn_name + mlc::String("(", 1)) + obj_code) + mlc::String(")", 1)) : (((((fn_name + mlc::String("(", 1)) + obj_code) + mlc::String(", ", 2)) + gen_args(margs, ctx)) + mlc::String(")", 1)));
}() : [&]() {
auto base = (((obj_code + mlc::String(".", 1)) + map_method(method_name)) + mlc::String("(", 1));
return ((margs.length() == 0) ? (base + mlc::String(")", 1)) : ((base + gen_args(margs, ctx)) + mlc::String(")", 1)));
}())))));
}(); },
[&](const ast::ExprField& exprField) { auto [obj, field_name] = exprField; return std::visit(overloaded{[&](const ast::ExprIndex& exprIndex) { auto [__0, __1] = exprIndex; return ((gen_expr(obj, ctx) + mlc::String("->", 2)) + cpp_safe(field_name)); },
[&](const ast::ExprIdent& exprIdent) { auto [n] = exprIdent; return (ctx.shared_params.contains(n) ? ((gen_expr(obj, ctx) + mlc::String("->", 2)) + cpp_safe(field_name)) : ((gen_expr(obj, ctx) + mlc::String(".", 1)) + cpp_safe(field_name))); },
[&](const auto& __v) { return ((gen_expr(obj, ctx) + mlc::String(".", 1)) + cpp_safe(field_name)); }
}, (*obj)); },
[&](const ast::ExprIndex& exprIndex) { auto [obj, idx] = exprIndex; return (((gen_expr(obj, ctx) + mlc::String("[", 1)) + gen_expr(idx, ctx)) + mlc::String("]", 1)); },
[&](const ast::ExprIf& exprIf) { auto [cond, then_expr, else_expr] = exprIf; return ((((((mlc::String("(", 1) + gen_expr(cond, ctx)) + mlc::String(" ? ", 3)) + gen_expr(then_expr, ctx)) + mlc::String(" : ", 3)) + gen_expr(else_expr, ctx)) + mlc::String(")", 1)); },
[&](const ast::ExprBlock& exprBlock) { auto [stmts, result] = exprBlock; return ((stmts.length() == 0) ? gen_expr(result, ctx) : [&]() {
auto body = (((gen_stmts_str(stmts, ctx) + mlc::String("return ", 7)) + gen_expr(result, ctx)) + mlc::String(";\n", 2));
return ((mlc::String("[&]() {\n", 8) + body) + mlc::String("}()", 3));
}()); },
[&](const ast::ExprWhile& exprWhile) { auto [cond, stmts] = exprWhile; return ((((mlc::String("[&]() {\nwhile (", 15) + gen_expr(cond, ctx)) + mlc::String(") {\n", 4)) + gen_stmts_str(stmts, ctx)) + mlc::String("}\n}()", 5)); },
[&](const ast::ExprFor& exprFor) { auto [var, iter, stmts] = exprFor; return ((((((mlc::String("[&]() {\nfor (auto ", 18) + cpp_safe(var)) + mlc::String(" : ", 3)) + gen_expr(iter, ctx)) + mlc::String(") {\n", 4)) + gen_stmts_str(stmts, ctx)) + mlc::String("}\n}()", 5)); },
[&](const ast::ExprMatch& exprMatch) { auto [subject, arms] = exprMatch; return gen_match(subject, arms, ctx); },
[&](const ast::ExprRecord& exprRecord) { auto [name, field_vals] = exprRecord; return [&]() {
auto field_order = lookup_fields(ctx.field_orders, name);
auto vals = ((field_order.length() > 0) ? gen_record_ordered(field_vals, field_order, ctx) : gen_record_unordered(field_vals, ctx));
return (((ctx_resolve(ctx, name) + mlc::String("{", 1)) + vals) + mlc::String("}", 1));
}(); },
[&](const ast::ExprRecordUpdate& exprRecordUpdate) { auto [name, base, overrides] = exprRecordUpdate; return [&]() {
auto field_order = lookup_fields(ctx.field_orders, name);
auto vals = gen_record_update_ordered(base, overrides, field_order, ctx);
return (((ctx_resolve(ctx, name) + mlc::String("{", 1)) + vals) + mlc::String("}", 1));
}(); },
[&](const ast::ExprArray& exprArray) { auto [exprs] = exprArray; return ((exprs.length() == 0) ? mlc::String("{}", 2) : ((mlc::String("mlc::Array{", 11) + gen_args(exprs, ctx)) + mlc::String("}", 1))); },
[&](const ast::ExprQuestion& exprQuestion) { auto [inner] = exprQuestion; return gen_expr(inner, ctx); },
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
auto body_code = gen_expr(body, ctx);
return (((((capture + mlc::String("(", 1)) + param_list) + mlc::String(") { return ", 11)) + body_code) + mlc::String("; }", 3));
}(); }
}, (*expr));
}
mlc::String gen_record_ordered(mlc::Array<std::shared_ptr<ast::FieldVal>> field_vals, mlc::Array<mlc::String> field_order, CodegenContext ctx) noexcept{
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < field_order.length())) {
auto value = find_field_val(field_vals, field_order[i]);
parts.push_back(gen_expr(value, ctx));
i = (i + 1);
}
return parts.join(mlc::String(", ", 2));
}
mlc::String gen_record_unordered(mlc::Array<std::shared_ptr<ast::FieldVal>> field_vals, CodegenContext ctx) noexcept{
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < field_vals.length())) {
parts.push_back(gen_expr(field_vals[i]->val, ctx));
i = (i + 1);
}
return parts.join(mlc::String(", ", 2));
}
mlc::String gen_record_update_ordered(std::shared_ptr<ast::Expr> base, mlc::Array<std::shared_ptr<ast::FieldVal>> overrides, mlc::Array<mlc::String> field_order, CodegenContext ctx) noexcept{
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < field_order.length())) {
auto field_name = field_order[i];
auto override_val = find_field_val(overrides, field_name);
auto val_str = std::visit(overloaded{[&](const ast::ExprUnit& exprUnit) { return gen_expr(std::make_shared<ast::Expr>(ast::ExprField(base, field_name)), ctx); },
[&](const auto& __v) { return gen_expr(override_val, ctx); }
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
(result = field_vals[i]->val);
}
i = (i + 1);
}
return result;
}
bool first_arm_needs_deref(std::shared_ptr<ast::MatchArm> first_arm, std::shared_ptr<ast::Expr> subject, CodegenContext ctx) noexcept{
return std::visit(overloaded{[&](const ast::PatCtor& patCtor) { auto [name, __1] = patCtor; return [&]() {
auto resolved = ctx_resolve(ctx, name);
return (((resolved.length() >= 12) && (resolved.substring(0, 12) == mlc::String("ast_tokens::", 12))) ? false : true);
}(); },
[&](const auto& __v) { return std::visit(overloaded{[&](const ast::ExprMethod& exprMethod) { auto [__0, __1, __2] = exprMethod; return false; },
[&](const auto& __v) { return true; }
}, (*subject)); }
}, (*first_arm->pat));
}
mlc::String gen_match(std::shared_ptr<ast::Expr> subject, mlc::Array<std::shared_ptr<ast::MatchArm>> arms, CodegenContext ctx) noexcept{
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < arms.length())) {
parts.push_back(gen_arm(arms[i], ctx));
i = (i + 1);
}
auto subject_code = gen_expr(subject, ctx);
auto needs_deref = ((arms.length() > 0) ? first_arm_needs_deref(arms[0], subject, ctx) : true);
auto visit_subject = (needs_deref ? ((mlc::String("(*", 2) + subject_code) + mlc::String(")", 1)) : subject_code);
return ((((mlc::String("std::visit(overloaded{", 22) + parts.join(mlc::String(",\n", 2))) + mlc::String("\n}, ", 4)) + visit_subject) + mlc::String(")", 1));
}
mlc::String gen_arm(std::shared_ptr<ast::MatchArm> arm, CodegenContext ctx) noexcept{
return std::visit(overloaded{[&](const ast::PatWild& patWild) { return ((mlc::String("[&](const auto& __v) { return ", 30) + gen_expr(arm->body, ctx)) + mlc::String("; }", 3)); },
[&](const ast::PatUnit& patUnit) { return ((mlc::String("[&](const auto& __v) { return ", 30) + gen_expr(arm->body, ctx)) + mlc::String("; }", 3)); },
[&](const ast::PatBool& patBool) { auto [__0] = patBool; return ((mlc::String("[&](const auto& __v) { return ", 30) + gen_expr(arm->body, ctx)) + mlc::String("; }", 3)); },
[&](const ast::PatInt& patInt) { auto [__0] = patInt; return ((mlc::String("[&](const auto& __v) { return ", 30) + gen_expr(arm->body, ctx)) + mlc::String("; }", 3)); },
[&](const ast::PatStr& patStr) { auto [__0] = patStr; return ((mlc::String("[&](const auto& __v) { return ", 30) + gen_expr(arm->body, ctx)) + mlc::String("; }", 3)); },
[&](const ast::PatIdent& patIdent) { auto [name] = patIdent; return ((((mlc::String("[&](const auto& ", 16) + cpp_safe(name)) + mlc::String(") { return ", 11)) + gen_expr(arm->body, ctx)) + mlc::String("; }", 3)); },
[&](const ast::PatCtor& patCtor) { auto [name, sub_pats] = patCtor; return [&]() {
auto qual = ctx_resolve(ctx, name);
auto lower_name = cpp_safe(lower_first(name));
auto binding = ((sub_pats.length() == 0) ? mlc::String("", 0) : ((((mlc::String("auto [", 6) + pat_bind_names(sub_pats)) + mlc::String("] = ", 4)) + lower_name) + mlc::String("; ", 2)));
return ((((((((mlc::String("[&](const ", 10) + qual) + mlc::String("& ", 2)) + lower_name) + mlc::String(") { ", 4)) + binding) + mlc::String("return ", 7)) + gen_expr(arm->body, ctx)) + mlc::String("; }", 3));
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
mlc::String gen_stmt(std::shared_ptr<ast::Stmt> stmt, CodegenContext ctx) noexcept{
return std::visit(overloaded{[&](const ast::StmtLet& stmtLet) { auto [name, __1, typ, value] = stmtLet; return [&]() {
auto val_code = std::visit(overloaded{[&](const ast::ExprArray& exprArray) { auto [elems] = exprArray; return ((elems.length() == 0) ? std::visit(overloaded{[&](const ast::TyArray& tyArray) { auto [inner] = tyArray; return ((mlc::String("mlc::Array<", 11) + type_to_cpp(ctx, inner)) + mlc::String(">{}", 3)); },
[&](const auto& __v) { return mlc::String("mlc::Array<mlc::String>{}", 25); }
}, (*typ)) : ((mlc::String("mlc::Array{", 11) + gen_args(elems, ctx)) + mlc::String("}", 1))); },
[&](const ast::ExprMethod& exprMethod) { auto [map_obj, mn, __2] = exprMethod; return ((mn == mlc::String("new", 3)) ? std::visit(overloaded{[&](const ast::ExprIdent& exprIdent) { auto [on] = exprIdent; return ((on == mlc::String("Map", 3)) ? std::visit(overloaded{[&](const ast::TyGeneric& tyGeneric) { auto [tn, targs] = tyGeneric; return (((tn == mlc::String("Map", 3)) && (targs.length() == 2)) ? ((((mlc::String("mlc::HashMap<", 13) + type_to_cpp(ctx, targs[0])) + mlc::String(", ", 2)) + type_to_cpp(ctx, targs[1])) + mlc::String(">()", 3)) : gen_expr(value, ctx)); },
[&](const auto& __v) { return gen_expr(value, ctx); }
}, (*typ)) : gen_expr(value, ctx)); },
[&](const auto& __v) { return gen_expr(value, ctx); }
}, (*map_obj)) : gen_expr(value, ctx)); },
[&](const auto& __v) { return gen_expr(value, ctx); }
}, (*value));
return ((((mlc::String("auto ", 5) + cpp_safe(name)) + mlc::String(" = ", 3)) + val_code) + mlc::String(";\n", 2));
}(); },
[&](const ast::StmtExpr& stmtExpr) { auto [expr] = stmtExpr; return gen_stmt_expr(expr, ctx); },
[&](const ast::StmtBreak& stmtBreak) { return mlc::String("break;\n", 7); },
[&](const ast::StmtContinue& stmtContinue) { return mlc::String("continue;\n", 10); },
[&](const ast::StmtReturn& stmtReturn) { auto [expr] = stmtReturn; return ((mlc::String("return ", 7) + gen_expr(expr, ctx)) + mlc::String(";\n", 2)); }
}, (*stmt));
}
mlc::String gen_stmt_expr(std::shared_ptr<ast::Expr> expr, CodegenContext ctx) noexcept{
return std::visit(overloaded{[&](const ast::ExprBin& exprBin) { auto [op, left_expr, right_expr] = exprBin; return ((op == mlc::String("=", 1)) ? (((gen_expr(left_expr, ctx) + mlc::String(" = ", 3)) + gen_expr(right_expr, ctx)) + mlc::String(";\n", 2)) : ((((((mlc::String("(", 1) + gen_expr(left_expr, ctx)) + mlc::String(" ", 1)) + op) + mlc::String(" ", 1)) + gen_expr(right_expr, ctx)) + mlc::String(");\n", 3))); },
[&](const ast::ExprIf& exprIf) { auto [cond, then_expr, else_expr] = exprIf; return [&]() {
auto output = ((((mlc::String("if (", 4) + gen_expr(cond, ctx)) + mlc::String(") {\n", 4)) + gen_block_body(then_expr, ctx)) + mlc::String("}", 1));
auto else_str = std::visit(overloaded{[&](const ast::ExprUnit& exprUnit) { return mlc::String("", 0); },
[&](const ast::ExprBlock& exprBlock) { auto [stmts, result] = exprBlock; return ((stmts.length() == 0) ? std::visit(overloaded{[&](const ast::ExprUnit& exprUnit) { return mlc::String("", 0); },
[&](const auto& __v) { return ((mlc::String(" else {\n", 8) + gen_block_body(else_expr, ctx)) + mlc::String("}", 1)); }
}, (*result)) : ((mlc::String(" else {\n", 8) + gen_block_body(else_expr, ctx)) + mlc::String("}", 1))); },
[&](const ast::ExprIf& exprIf) { auto [__0, __1, __2] = exprIf; return (mlc::String(" else ", 6) + gen_stmt_expr(else_expr, ctx)); },
[&](const auto& __v) { return ((mlc::String(" else {\n", 8) + gen_block_body(else_expr, ctx)) + mlc::String("}", 1)); }
}, (*else_expr));
if ((else_str.length() > 0)) {
(output = (output + else_str));
}
return (output + mlc::String("\n", 1));
}(); },
[&](const ast::ExprWhile& exprWhile) { auto [cond, stmts] = exprWhile; return ((((mlc::String("while (", 7) + gen_expr(cond, ctx)) + mlc::String(") {\n", 4)) + gen_stmts_str(stmts, ctx)) + mlc::String("}\n", 2)); },
[&](const ast::ExprFor& exprFor) { auto [var, iter, stmts] = exprFor; return ((((((mlc::String("for (auto ", 10) + cpp_safe(var)) + mlc::String(" : ", 3)) + gen_expr(iter, ctx)) + mlc::String(") {\n", 4)) + gen_stmts_str(stmts, ctx)) + mlc::String("}\n", 2)); },
[&](const ast::ExprBlock& exprBlock) { auto [stmts, result] = exprBlock; return [&]() {
auto stmts_code = gen_stmts_str(stmts, ctx);
auto result_code = gen_expr(result, ctx);
return ((result_code == mlc::String("/* unit */", 10)) ? stmts_code : ((stmts_code + result_code) + mlc::String(";\n", 2)));
}(); },
[&](const auto& __v) { return (gen_expr(expr, ctx) + mlc::String(";\n", 2)); }
}, (*expr));
}
mlc::String gen_return_body(std::shared_ptr<ast::Expr> expr, CodegenContext ctx) noexcept{
return std::visit(overloaded{[&](const ast::ExprBlock& exprBlock) { auto [stmts, result] = exprBlock; return [&]() {
auto stmts_code = gen_stmts_str(stmts, ctx);
auto result_code = gen_expr(result, ctx);
return ((result_code == mlc::String("/* unit */", 10)) ? stmts_code : (((stmts_code + mlc::String("return ", 7)) + result_code) + mlc::String(";\n", 2)));
}(); },
[&](const ast::ExprIf& exprIf) { auto [cond, then_expr, else_expr] = exprIf; return [&]() {
auto output = ((((mlc::String("if (", 4) + gen_expr(cond, ctx)) + mlc::String(") {\n", 4)) + gen_return_body(then_expr, ctx)) + mlc::String("}", 1));
auto else_str = std::visit(overloaded{[&](const ast::ExprUnit& exprUnit) { return mlc::String("", 0); },
[&](const ast::ExprIf& exprIf) { auto [__0, __1, __2] = exprIf; return (mlc::String(" else ", 6) + gen_return_if_stmt(else_expr, ctx)); },
[&](const auto& __v) { return ((mlc::String(" else {\n", 8) + gen_return_body(else_expr, ctx)) + mlc::String("}", 1)); }
}, (*else_expr));
if ((else_str.length() > 0)) {
(output = (output + else_str));
}
return (output + mlc::String("\n", 1));
}(); },
[&](const auto& __v) { return ((mlc::String("return ", 7) + gen_expr(expr, ctx)) + mlc::String(";\n", 2)); }
}, (*expr));
}
mlc::String gen_return_if_stmt(std::shared_ptr<ast::Expr> expr, CodegenContext ctx) noexcept{
return std::visit(overloaded{[&](const ast::ExprIf& exprIf) { auto [cond, then_expr, else_expr] = exprIf; return [&]() {
auto output = ((((mlc::String("if (", 4) + gen_expr(cond, ctx)) + mlc::String(") {\n", 4)) + gen_return_body(then_expr, ctx)) + mlc::String("}", 1));
auto else_str = std::visit(overloaded{[&](const ast::ExprUnit& exprUnit) { return mlc::String("", 0); },
[&](const ast::ExprIf& exprIf) { auto [__0, __1, __2] = exprIf; return (mlc::String(" else ", 6) + gen_return_if_stmt(else_expr, ctx)); },
[&](const auto& __v) { return ((mlc::String(" else {\n", 8) + gen_return_body(else_expr, ctx)) + mlc::String("}", 1)); }
}, (*else_expr));
if ((else_str.length() > 0)) {
(output = (output + else_str));
}
return (output + mlc::String("\n", 1));
}(); },
[&](const auto& __v) { return ((mlc::String("if (true) {\n", 12) + gen_return_body(expr, ctx)) + mlc::String("}\n", 2)); }
}, (*expr));
}
mlc::String gen_fn_body(std::shared_ptr<ast::Expr> body, CodegenContext ctx) noexcept{
return gen_return_body(body, ctx);
}
mlc::String gen_params(CodegenContext ctx, mlc::Array<std::shared_ptr<ast::Param>> params) noexcept{
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < params.length())) {
parts.push_back(((type_to_cpp(ctx, params[i]->typ) + mlc::String(" ", 1)) + cpp_safe(params[i]->name)));
i = (i + 1);
}
return parts.join(mlc::String(", ", 2));
}
mlc::String gen_type_decl_fwd(CodegenContext ctx, mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
if ((variants.length() == 1)) {
return mlc::String("", 0);
} else {
return gen_adt_fwd(ctx, type_name, variants);
}
}
mlc::String gen_type_decl_body(CodegenContext ctx, mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
if ((variants.length() == 1)) {
return gen_single_variant(ctx, type_name, variants[0]);
} else {
return gen_adt_defs(ctx, type_name, variants);
}
}
mlc::String gen_single_variant(CodegenContext ctx, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept{
return std::visit(overloaded{[&](const ast::VarRecord& varRecord) { auto [__0, field_defs] = varRecord; return [&]() {
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < field_defs.length())) {
parts.push_back((((type_to_cpp(ctx, field_defs[i]->typ) + mlc::String(" ", 1)) + cpp_safe(field_defs[i]->name)) + mlc::String(";", 1)));
i = (i + 1);
}
return ((((mlc::String("struct ", 7) + ctx_resolve(ctx, type_name)) + mlc::String(" {", 2)) + parts.join(mlc::String("", 0))) + mlc::String("};\n", 3));
}(); },
[&](const ast::VarTuple& varTuple) { auto [__0, field_types] = varTuple; return [&]() {
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < field_types.length())) {
parts.push_back((((type_to_cpp(ctx, field_types[i]) + mlc::String(" field", 6)) + mlc::to_string(i)) + mlc::String(";", 1)));
i = (i + 1);
}
return ((((mlc::String("struct ", 7) + ctx_resolve(ctx, type_name)) + mlc::String(" {", 2)) + parts.join(mlc::String("", 0))) + mlc::String("};\n", 3));
}(); },
[&](const ast::VarUnit& varUnit) { auto [__0] = varUnit; return ((mlc::String("struct ", 7) + ctx_resolve(ctx, type_name)) + mlc::String(" {};\n", 5)); }
}, (*variant));
}
mlc::String gen_adt_fwd(CodegenContext ctx, mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
auto parts = mlc::Array<mlc::String>{};
auto variant_names = mlc::Array<mlc::String>{};
auto index = 0;
while ((index < variants.length())) {
auto variant_name = ctx_resolve(ctx, variant_ctor_name(variants[index]));
variant_names.push_back(variant_name);
parts.push_back(((mlc::String("struct ", 7) + variant_name) + mlc::String(";\n", 2)));
index = (index + 1);
}
return (((((parts.join(mlc::String("", 0)) + mlc::String("using ", 6)) + ctx_resolve(ctx, type_name)) + mlc::String(" = std::variant<", 16)) + variant_names.join(mlc::String(", ", 2))) + mlc::String(">;\n", 3));
}
mlc::String gen_adt_defs(CodegenContext ctx, mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < variants.length())) {
parts.push_back(gen_variant_struct(ctx, type_name, variants[i]));
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
mlc::String gen_variant_struct(CodegenContext ctx, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept{
return std::visit(overloaded{[&](const ast::VarUnit& varUnit) { auto [name] = varUnit; return ((mlc::String("struct ", 7) + ctx_resolve(ctx, name)) + mlc::String(" {};\n", 5)); },
[&](const ast::VarTuple& varTuple) { auto [name, field_types] = varTuple; return [&]() {
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < field_types.length())) {
parts.push_back((((type_to_cpp(ctx, field_types[i]) + mlc::String(" field", 6)) + mlc::to_string(i)) + mlc::String(";", 1)));
i = (i + 1);
}
return ((((mlc::String("struct ", 7) + ctx_resolve(ctx, name)) + mlc::String(" {", 2)) + parts.join(mlc::String("", 0))) + mlc::String("};\n", 3));
}(); },
[&](const ast::VarRecord& varRecord) { auto [name, field_defs] = varRecord; return [&]() {
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < field_defs.length())) {
parts.push_back((((type_to_cpp(ctx, field_defs[i]->typ) + mlc::String(" ", 1)) + cpp_safe(field_defs[i]->name)) + mlc::String(";", 1)));
i = (i + 1);
}
return ((((mlc::String("struct ", 7) + ctx_resolve(ctx, name)) + mlc::String(" {", 2)) + parts.join(mlc::String("", 0))) + mlc::String("};\n", 3));
}(); }
}, (*variant));
}
bool is_shared_type(std::shared_ptr<ast::TypeExpr> t) noexcept{
return std::visit(overloaded{[&](const ast::TyGeneric& tyGeneric) { auto [n, __1] = tyGeneric; return (n == mlc::String("Shared", 6)); },
[&](const ast::TyShared& tyShared) { auto [__0] = tyShared; return true; },
[&](const auto& __v) { return false; }
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
mlc::String gen_fn_decl(mlc::String name, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<ast::TypeExpr> return_type, std::shared_ptr<ast::Expr> body, CodegenContext ctx) noexcept{
auto safe_name = ctx_resolve(ctx, name);
auto sp = collect_shared_params(params);
auto body_ctx = (((params.length() > 0) && (params[0]->name == mlc::String("self", 4))) ? std::visit(overloaded{[&](const ast::TyNamed& tyNamed) { auto [t] = tyNamed; return CodegenContext{ctx.field_orders, ctx.ns_prefix, ctx.qualified, t, ctx.method_owners, sp, ctx.variant_types}; },
[&](const auto& __v) { return CodegenContext{ctx.field_orders, ctx.ns_prefix, ctx.qualified, ctx.self_type, ctx.method_owners, sp, ctx.variant_types}; }
}, (*params[0]->typ)) : CodegenContext{ctx.field_orders, ctx.ns_prefix, ctx.qualified, ctx.self_type, ctx.method_owners, sp, ctx.variant_types});
return (((name == mlc::String("main", 4)) && (params.length() == 0)) ? [&]() {
auto preamble = mlc::String("mlc::io::set_args(std::vector<mlc::String>(argv + 1, argv + argc));\n", 68);
return ((((((type_to_cpp(ctx, return_type) + mlc::String(" ", 1)) + safe_name) + mlc::String("(int argc, char** argv) noexcept{\n", 34)) + preamble) + gen_fn_body(body, body_ctx)) + mlc::String("}\n", 2));
}() : (((((((type_to_cpp(ctx, return_type) + mlc::String(" ", 1)) + safe_name) + mlc::String("(", 1)) + gen_params(ctx, params)) + mlc::String(") noexcept{\n", 12)) + gen_fn_body(body, body_ctx)) + mlc::String("}\n", 2)));
}
mlc::String gen_fn_proto(mlc::String name, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<ast::TypeExpr> return_type, CodegenContext ctx) noexcept{
auto safe_name = ctx_resolve(ctx, name);
return (((name == mlc::String("main", 4)) && (params.length() == 0)) ? (((type_to_cpp(ctx, return_type) + mlc::String(" ", 1)) + safe_name) + mlc::String("(int argc, char** argv) noexcept;\n", 34)) : (((((type_to_cpp(ctx, return_type) + mlc::String(" ", 1)) + safe_name) + mlc::String("(", 1)) + gen_params(ctx, params)) + mlc::String(") noexcept;\n", 12)));
}
mlc::String gen_type_decl(CodegenContext ctx, mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
return (gen_type_decl_fwd(ctx, type_name, variants) + gen_type_decl_body(ctx, type_name, variants));
}
mlc::String gen_decl(std::shared_ptr<ast::Decl> decl, CodegenContext ctx) noexcept{
return std::visit(overloaded{[&](const ast::DeclType& declType) { auto [type_name, variants] = declType; return gen_type_decl(ctx, type_name, variants); },
[&](const ast::DeclFn& declFn) { auto [name, params, return_type, body] = declFn; return std::visit(overloaded{[&](const ast::ExprExtern& exprExtern) { return mlc::String("", 0); },
[&](const auto& __v) { return gen_fn_decl(name, params, return_type, body, ctx); }
}, (*body)); },
[&](const ast::DeclExtend& declExtend) { auto [__0, methods] = declExtend; return [&]() {
auto parts = mlc::Array<mlc::String>{};
auto index = 0;
while ((index < methods.length())) {
parts.push_back(gen_decl(methods[index], ctx));
index = (index + 1);
}
return parts.join(mlc::String("", 0));
}(); },
[&](const ast::DeclImport& declImport) { auto [__0, __1] = declImport; return mlc::String("", 0); },
[&](const ast::DeclExported& declExported) { auto [d] = declExported; return gen_decl(d, ctx); }
}, (*decl));
}
mlc::String gen_proto(std::shared_ptr<ast::Decl> decl, CodegenContext ctx) noexcept{
return std::visit(overloaded{[&](const ast::DeclType& declType) { auto [__0, __1] = declType; return mlc::String("", 0); },
[&](const ast::DeclFn& declFn) { auto [name, params, ret, body] = declFn; return std::visit(overloaded{[&](const ast::ExprExtern& exprExtern) { return mlc::String("", 0); },
[&](const auto& __v) { return gen_fn_proto(name, params, ret, ctx); }
}, (*body)); },
[&](const ast::DeclExtend& declExtend) { auto [__0, methods] = declExtend; return [&]() {
auto parts = mlc::Array<mlc::String>{};
auto index = 0;
while ((index < methods.length())) {
parts.push_back(gen_proto(methods[index], ctx));
index = (index + 1);
}
return parts.join(mlc::String("", 0));
}(); },
[&](const ast::DeclImport& declImport) { auto [__0, __1] = declImport; return mlc::String("", 0); },
[&](const ast::DeclExported& declExported) { auto [d] = declExported; return gen_proto(d, ctx); }
}, (*decl));
}
mlc::Array<mlc::String> collect_decl_parts(mlc::Array<std::shared_ptr<ast::Decl>> decls, CodegenContext ctx, int phase) noexcept{
auto parts = mlc::Array<mlc::String>{};
auto index = 0;
while ((index < decls.length())) {
auto segment = std::visit(overloaded{[&](const ast::DeclType& declType) { auto [type_name, variants] = declType; return ((phase == 0) ? gen_type_decl_fwd(ctx, type_name, variants) : ((phase == 1) ? gen_type_decl_body(ctx, type_name, variants) : mlc::String("", 0))); },
[&](const ast::DeclFn& declFn) { auto [__0, __1, __2, __3] = declFn; return ((phase == 2) ? gen_proto(decls[index], ctx) : ((phase == 3) ? gen_decl(decls[index], ctx) : mlc::String("", 0))); },
[&](const ast::DeclExtend& declExtend) { auto [__0, __1] = declExtend; return ((phase == 2) ? gen_proto(decls[index], ctx) : ((phase == 3) ? gen_decl(decls[index], ctx) : mlc::String("", 0))); },
[&](const ast::DeclImport& declImport) { auto [__0, __1] = declImport; return mlc::String("", 0); },
[&](const ast::DeclExported& declExported) { auto [d] = declExported; return ((phase == 0) ? std::visit(overloaded{[&](const ast::DeclType& declType) { auto [type_name, variants] = declType; return gen_type_decl_fwd(ctx, type_name, variants); },
[&](const auto& __v) { return mlc::String("", 0); }
}, (*ast::decl_inner(d))) : ((phase == 1) ? std::visit(overloaded{[&](const ast::DeclType& declType) { auto [type_name, variants] = declType; return gen_type_decl_body(ctx, type_name, variants); },
[&](const auto& __v) { return mlc::String("", 0); }
}, (*ast::decl_inner(d))) : ((phase == 2) ? gen_proto(d, ctx) : ((phase == 3) ? std::visit(overloaded{[&](const ast::DeclType& declType) { auto [__0, __1] = declType; return mlc::String("", 0); },
[&](const auto& __v) { return gen_decl(d, ctx); }
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
auto ctx = create_codegen_context(prog);
auto preamble = mlc::Array{mlc::String("#include \"mlc/core/string.hpp\"\n", 31), mlc::String("#include \"mlc/core/array.hpp\"\n", 30), mlc::String("#include \"mlc/core/hashmap.hpp\"\n", 32), mlc::String("#include \"mlc/core/match.hpp\"\n", 30), mlc::String("#include \"mlc/io/io.hpp\"\n", 25), mlc::String("#include \"mlc/io/file.hpp\"\n", 27), mlc::String("#include <functional>\n", 22), mlc::String("#include <memory>\n", 18), mlc::String("#include <variant>\n", 19), mlc::String("#include <vector>\n\n", 19)};
auto type_fwds = collect_decl_parts(prog.decls, ctx, 0);
auto type_defs = collect_decl_parts(prog.decls, ctx, 1);
auto fn_protos = collect_decl_parts(prog.decls, ctx, 2);
auto fn_decls = collect_decl_parts(prog.decls, ctx, 3);
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
GenModuleOut gen_module(LoadItem item, mlc::Array<LoadItem> all_items, ast::Program full_prog) noexcept{
auto base = path_to_module_base(item.path);
auto qualified = build_qualified(item.imports, all_items);
auto ctx = CodegenContext{build_field_orders(full_prog), mlc::String("", 0), qualified, mlc::String("", 0), build_method_owners_for(item, all_items), {}, build_variant_types_from_decls(full_prog.decls)};
auto ns = ((base == mlc::String("main", 4)) ? mlc::String("mlc_main", 8) : base);
auto std_includes = ((mlc::String("#include \"mlc.hpp\"\n#include <variant>\n\n", 39) + include_lines(item.imports)) + mlc::String("\n", 1));
auto type_fwds = collect_decl_parts(item.decls, ctx, 0);
auto type_defs = collect_decl_parts(item.decls, ctx, 1);
auto fn_protos = collect_decl_parts(item.decls, ctx, 2);
auto fn_defs = collect_decl_parts(item.decls, ctx, 3);
auto guard = (base.upper() + mlc::String("_HPP", 4));
auto header = mlc::Array{((mlc::String("#ifndef ", 8) + guard) + mlc::String("\n", 1)), ((mlc::String("#define ", 8) + guard) + mlc::String("\n", 1)), mlc::String("\n", 1), std_includes, ((mlc::String("namespace ", 10) + ns) + mlc::String(" {\n", 3)), mlc::String("\n", 1), type_fwds.join(mlc::String("", 0)), type_defs.join(mlc::String("", 0)), fn_protos.join(mlc::String("", 0)), mlc::String("\n", 1), ((mlc::String("} // namespace ", 15) + ns) + mlc::String("\n", 1)), mlc::String("\n", 1), ((mlc::String("#endif // ", 10) + guard) + mlc::String("\n", 1))}.join(mlc::String("", 0));
auto cli_wrapper = ((base == mlc::String("main", 4)) ? mlc::String("\n#undef main\n\nstatic void mlc_cli_set_args(int argc, char** argv) {\n  std::vector<mlc::String> arguments;\n  arguments.reserve(argc > 0 ? argc - 1 : 0);\n  for (int i = 1; i < argc; ++i) { arguments.emplace_back(argv[i]); }\n  mlc::io::set_args(std::move(arguments));\n}\n\nint main(int argc, char** argv) {\n  mlc_cli_set_args(argc, argv);\n  return ::mlc_main::mlc_user_main(argc, argv);\n}\n", 384) : mlc::String("", 0));
auto impl = mlc::Array{mlc::String("#define main mlc_user_main\n", 27), ((mlc::String("#include \"", 10) + base) + mlc::String(".hpp\"\n", 6)), mlc::String("\n", 1), ((mlc::String("namespace ", 10) + ns) + mlc::String(" {\n", 3)), mlc::String("\n", 1), fn_defs.join(mlc::String("", 0)), mlc::String("\n", 1), ((mlc::String("} // namespace ", 15) + ns) + mlc::String("\n", 1)), cli_wrapper}.join(mlc::String("", 0));
return GenModuleOut{header, impl};
}

} // namespace codegen
