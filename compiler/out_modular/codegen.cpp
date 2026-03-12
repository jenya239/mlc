#include "codegen.hpp"

#include "ast.hpp"

namespace codegen {

using namespace ast;
using namespace ast_tokens;

struct FieldOrder;

struct CodegenContext;

codegen::CodegenContext create_codegen_context(ast::Program prog) noexcept;

mlc::Array<std::shared_ptr<codegen::FieldOrder>> build_field_orders(ast::Program prog) noexcept;

mlc::Array<mlc::String> lookup_fields(mlc::Array<std::shared_ptr<codegen::FieldOrder>> orders, mlc::String name) noexcept;

constexpr bool cpp_keyword(mlc::String name) noexcept;

mlc::String cpp_safe(mlc::String name) noexcept;

mlc::String lower_first(mlc::String name) noexcept;

mlc::String type_to_cpp(std::shared_ptr<ast::TypeExpr> type_expr) noexcept;

mlc::String escape_str(mlc::String input) noexcept;

mlc::String gen_args(mlc::Array<std::shared_ptr<ast::Expr>> args, codegen::CodegenContext ctx) noexcept;

mlc::String map_method(mlc::String method_name) noexcept;

mlc::String map_builtin(mlc::String name) noexcept;

mlc::String gen_stmts_str(mlc::Array<std::shared_ptr<ast::Stmt>> stmts, codegen::CodegenContext ctx) noexcept;

mlc::String gen_block_body(std::shared_ptr<ast::Expr> expr, codegen::CodegenContext ctx) noexcept;

mlc::String gen_expr(std::shared_ptr<ast::Expr> expr, codegen::CodegenContext ctx) noexcept;

mlc::String gen_record_ordered(mlc::Array<std::shared_ptr<ast::FieldVal>> field_vals, mlc::Array<mlc::String> field_order, codegen::CodegenContext ctx) noexcept;

mlc::String gen_record_unordered(mlc::Array<std::shared_ptr<ast::FieldVal>> field_vals, codegen::CodegenContext ctx) noexcept;

mlc::String gen_record_update_ordered(std::shared_ptr<ast::Expr> base, mlc::Array<std::shared_ptr<ast::FieldVal>> overrides, mlc::Array<mlc::String> field_order, codegen::CodegenContext ctx) noexcept;

std::shared_ptr<ast::Expr> find_field_val(mlc::Array<std::shared_ptr<ast::FieldVal>> field_vals, mlc::String name) noexcept;

mlc::String gen_match(std::shared_ptr<ast::Expr> subject, mlc::Array<std::shared_ptr<ast::MatchArm>> arms, codegen::CodegenContext ctx) noexcept;

mlc::String gen_arm(std::shared_ptr<ast::MatchArm> arm, codegen::CodegenContext ctx) noexcept;

mlc::String gen_pat_header(std::shared_ptr<ast::Pat> pat) noexcept;

mlc::String pat_bind_names(mlc::Array<std::shared_ptr<ast::Pat>> sub_pats) noexcept;

mlc::String gen_stmt(std::shared_ptr<ast::Stmt> stmt, codegen::CodegenContext ctx) noexcept;

mlc::String gen_stmt_expr(std::shared_ptr<ast::Expr> expr, codegen::CodegenContext ctx) noexcept;

mlc::String gen_return_body(std::shared_ptr<ast::Expr> expr, codegen::CodegenContext ctx) noexcept;

mlc::String gen_return_if_stmt(std::shared_ptr<ast::Expr> expr, codegen::CodegenContext ctx) noexcept;

mlc::String gen_fn_body(std::shared_ptr<ast::Expr> body, codegen::CodegenContext ctx) noexcept;

mlc::String gen_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;

mlc::String gen_type_decl_fwd(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::String gen_type_decl_body(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::String gen_single_variant(mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept;

mlc::String gen_adt_fwd(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::String gen_adt_defs(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::String variant_ctor_name(std::shared_ptr<ast::TypeVariant> variant) noexcept;

mlc::String gen_variant_struct(mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept;

mlc::String gen_fn_decl(mlc::String name, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<ast::TypeExpr> return_type, std::shared_ptr<ast::Expr> body, codegen::CodegenContext ctx) noexcept;

mlc::String gen_fn_proto(mlc::String name, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<ast::TypeExpr> return_type) noexcept;

mlc::String gen_type_decl(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::String gen_decl(std::shared_ptr<ast::Decl> decl, codegen::CodegenContext ctx) noexcept;

mlc::String gen_proto(std::shared_ptr<ast::Decl> decl) noexcept;

mlc::Array<mlc::String> collect_decl_parts(mlc::Array<std::shared_ptr<ast::Decl>> decls, codegen::CodegenContext ctx, int phase) noexcept;

mlc::String gen_program(ast::Program prog) noexcept;

struct FieldOrder {mlc::String type_name;mlc::Array<mlc::String> fields;};

struct CodegenContext {mlc::Array<std::shared_ptr<codegen::FieldOrder>> field_orders;};

codegen::CodegenContext create_codegen_context(ast::Program prog) noexcept{return codegen::CodegenContext{codegen::build_field_orders(prog)};}

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

constexpr bool cpp_keyword(mlc::String name) noexcept{return name == mlc::String("export") || name == mlc::String("import") || name == mlc::String("module") || name == mlc::String("struct") || name == mlc::String("class") || name == mlc::String("template") || name == mlc::String("typename") || name == mlc::String("const") || name == mlc::String("return") || name == mlc::String("if") || name == mlc::String("else") || name == mlc::String("for") || name == mlc::String("while") || name == mlc::String("do") || name == mlc::String("switch") || name == mlc::String("case") || name == mlc::String("default") || name == mlc::String("break") || name == mlc::String("continue") || name == mlc::String("try") || name == mlc::String("catch") || name == mlc::String("throw") || name == mlc::String("new") || name == mlc::String("delete") || name == mlc::String("virtual") || name == mlc::String("override") || name == mlc::String("final") || name == mlc::String("public") || name == mlc::String("private") || name == mlc::String("protected") || name == mlc::String("friend") || name == mlc::String("static") || name == mlc::String("extern") || name == mlc::String("inline") || name == mlc::String("using") || name == mlc::String("namespace") || name == mlc::String("typedef") || name == mlc::String("enum") || name == mlc::String("union") || name == mlc::String("true") || name == mlc::String("false") || name == mlc::String("nullptr") || name == mlc::String("void") || name == mlc::String("bool") || name == mlc::String("int") || name == mlc::String("long") || name == mlc::String("short") || name == mlc::String("char") || name == mlc::String("float") || name == mlc::String("double") || name == mlc::String("signed") || name == mlc::String("unsigned") || name == mlc::String("auto") || name == mlc::String("typeid") || name == mlc::String("sizeof") || name == mlc::String("and") || name == mlc::String("or") || name == mlc::String("not") || name == mlc::String("xor") || name == mlc::String("and_eq") || name == mlc::String("or_eq") || name == mlc::String("not_eq") || name == mlc::String("xor_eq") || name == mlc::String("bitand") || name == mlc::String("bitor") || name == mlc::String("compl") || name == mlc::String("alignas") || name == mlc::String("alignof") || name == mlc::String("asm") || name == mlc::String("concept") || name == mlc::String("consteval") || name == mlc::String("constexpr") || name == mlc::String("constinit") || name == mlc::String("const_cast") || name == mlc::String("decltype") || name == mlc::String("dynamic_cast") || name == mlc::String("explicit") || name == mlc::String("mutable") || name == mlc::String("noexcept") || name == mlc::String("operator") || name == mlc::String("reinterpret_cast") || name == mlc::String("requires") || name == mlc::String("static_cast") || name == mlc::String("this") || name == mlc::String("thread_local") || name == mlc::String("volatile") || name == mlc::String("wchar_t");}

mlc::String cpp_safe(mlc::String name) noexcept{return codegen::cpp_keyword(name) ? name + mlc::String("_") : name;}

mlc::String lower_first(mlc::String name) noexcept{return name.length() == 0 ? name : name.char_at(0).to_lower() + name.substring(1, name.length());}

mlc::String type_to_cpp(std::shared_ptr<ast::TypeExpr> type_expr) noexcept{return std::visit(overloaded{
  [&](const TyI32& tyi32) { return mlc::String("int"); },
  [&](const TyString& tystring) { return mlc::String("mlc::String"); },
  [&](const TyBool& tybool) { return mlc::String("bool"); },
  [&](const TyUnit& tyunit) { return mlc::String("void"); },
  [&](const TyNamed& tynamed) { auto [name] = tynamed; return codegen::cpp_safe(name); },
  [&](const TyArray& tyarray) { auto [inner] = tyarray; return mlc::String("mlc::Array<") + codegen::type_to_cpp(inner) + mlc::String(">"); },
  [&](const TyShared& tyshared) { auto [inner] = tyshared; return mlc::String("std::shared_ptr<") + codegen::type_to_cpp(inner) + mlc::String(">"); },
  [&](const TyGeneric& tygeneric) { auto [name, args] = tygeneric; return [&]() -> mlc::String { 
  mlc::String safe = codegen::cpp_safe(name);
  return args.size() == 0 ? safe : args.size() == 1 ? safe + mlc::String("<") + codegen::type_to_cpp(args[0]) + mlc::String(">") : safe + mlc::String("<") + codegen::type_to_cpp(args[0]) + mlc::String(", ") + codegen::type_to_cpp(args[1]) + mlc::String(">");
 }(); },
  [&](const TyFn& tyfn) { auto [params, ret] = tyfn; return [&]() -> mlc::String { 
  mlc::Array<mlc::String> param_types = {};
  int i = 0;
  while (i < params.size()){
{
param_types.push_back(codegen::type_to_cpp(params[i]));
i = i + 1;
}
}
  return mlc::String("std::function<") + codegen::type_to_cpp(ret) + mlc::String("(") + param_types.join(mlc::String(", ")) + mlc::String(")>");
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
if (c == mlc::String("\0")){
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

mlc::String gen_args(mlc::Array<std::shared_ptr<ast::Expr>> args, codegen::CodegenContext ctx) noexcept{
mlc::Array<mlc::String> parts = {};
int i = 0;
while (i < args.size()){
{
parts.push_back(codegen::gen_expr(args[i], ctx));
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
while (i < stmts.size()){
{
parts.push_back(codegen::gen_stmt(stmts[i], ctx));
i = i + 1;
}
}
return parts.join(mlc::String(""));
}

mlc::String gen_block_body(std::shared_ptr<ast::Expr> expr, codegen::CodegenContext ctx) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprBlock>((*expr)._)) { auto _v_exprblock = std::get<ast::ExprBlock>((*expr)._); auto [stmts, result] = _v_exprblock; return [&]() -> mlc::String { 
  mlc::String stmts_code = codegen::gen_stmts_str(stmts, ctx);
  mlc::String result_code = codegen::gen_expr(result, ctx);
  return result_code == mlc::String("/* unit */") ? stmts_code : stmts_code + result_code + mlc::String(";\n");
 }(); } if (std::holds_alternative<ast::ExprIf>((*expr)._)) { auto _v_exprif = std::get<ast::ExprIf>((*expr)._); auto [_w0, _w1, _w2] = _v_exprif; return codegen::gen_stmt_expr(expr, ctx); } return codegen::gen_expr(expr, ctx) + mlc::String(";\n"); }();}

mlc::String gen_expr(std::shared_ptr<ast::Expr> expr, codegen::CodegenContext ctx) noexcept{return std::visit(overloaded{
  [&](const ExprInt& exprint) { auto [n] = exprint; return mlc::to_string(n); },
  [&](const ExprStr& exprstr) { auto [s] = exprstr; return mlc::String("mlc::String(\"") + codegen::escape_str(s) + mlc::String("\")"); },
  [&](const ExprBool& exprbool) { auto [b] = exprbool; return b ? mlc::String("true") : mlc::String("false"); },
  [&](const ExprUnit& exprunit) { return mlc::String("/* unit */"); },
  [&](const ExprIdent& exprident) { auto [name] = exprident; return [&]() -> mlc::String { 
  mlc::String mapped = codegen::map_builtin(name);
  mlc::String safe = codegen::cpp_safe(mapped);
  mlc::String first = safe.char_at(0);
  return first >= mlc::String("A") && first <= mlc::String("Z") ? safe + mlc::String("{}") : safe;
 }(); },
  [&](const ExprBin& exprbin) { auto [op, left_expr, right_expr] = exprbin; return mlc::String("(") + codegen::gen_expr(left_expr, ctx) + mlc::String(" ") + op + mlc::String(" ") + codegen::gen_expr(right_expr, ctx) + mlc::String(")"); },
  [&](const ExprUn& exprun) { auto [op, inner] = exprun; return mlc::String("(") + op + codegen::gen_expr(inner, ctx) + mlc::String(")"); },
  [&](const ExprCall& exprcall) { auto [func, args] = exprcall; return [&]() -> mlc::String { 
  mlc::String fn_code = [&]() -> mlc::String { if (std::holds_alternative<ast::ExprIdent>((*func)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*func)._); auto [name] = _v_exprident; return codegen::cpp_safe(codegen::map_builtin(name)); } return codegen::gen_expr(func, ctx); }();
  return fn_code + mlc::String("(") + codegen::gen_args(args, ctx) + mlc::String(")");
 }(); },
  [&](const ExprMethod& exprmethod) { auto [obj, method_name, args] = exprmethod; return [&]() -> mlc::String { 
  mlc::String obj_code = [&]() -> mlc::String { if (std::holds_alternative<ast::ExprIdent>((*obj)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*obj)._); auto [name] = _v_exprident; return codegen::cpp_safe(codegen::map_builtin(name)); } return codegen::gen_expr(obj, ctx); }();
  return obj_code == mlc::String("File") ? [&]() -> mlc::String { 
  mlc::String cpp_fn = method_name == mlc::String("read") ? mlc::String("mlc::file::read_to_string") : mlc::String("mlc::file::") + method_name;
  return cpp_fn + mlc::String("(") + codegen::gen_args(args, ctx) + mlc::String(")");
 }() : [&]() -> mlc::String { 
  mlc::String base = obj_code + mlc::String(".") + codegen::map_method(method_name) + mlc::String("(");
  return args.size() == 0 ? base + mlc::String(")") : base + codegen::gen_args(args, ctx) + mlc::String(")");
 }();
 }(); },
  [&](const ExprField& exprfield) { auto [obj, field_name] = exprfield; return codegen::gen_expr(obj, ctx) + mlc::String(".") + codegen::cpp_safe(field_name); },
  [&](const ExprIndex& exprindex) { auto [obj, idx] = exprindex; return codegen::gen_expr(obj, ctx) + mlc::String("[") + codegen::gen_expr(idx, ctx) + mlc::String("]"); },
  [&](const ExprIf& exprif) { auto [cond, then_expr, else_expr] = exprif; return mlc::String("(") + codegen::gen_expr(cond, ctx) + mlc::String(" ? ") + codegen::gen_expr(then_expr, ctx) + mlc::String(" : ") + codegen::gen_expr(else_expr, ctx) + mlc::String(")"); },
  [&](const ExprBlock& exprblock) { auto [stmts, result] = exprblock; return stmts.size() == 0 ? codegen::gen_expr(result, ctx) : [&]() -> mlc::String { 
  mlc::String body = codegen::gen_stmts_str(stmts, ctx) + mlc::String("return ") + codegen::gen_expr(result, ctx) + mlc::String(";\n");
  return mlc::String("[&]() {\n") + body + mlc::String("}()");
 }(); },
  [&](const ExprWhile& exprwhile) { auto [cond, stmts] = exprwhile; return mlc::String("[&]() {\nwhile (") + codegen::gen_expr(cond, ctx) + mlc::String(") {\n") + codegen::gen_stmts_str(stmts, ctx) + mlc::String("}\n}()"); },
  [&](const ExprFor& exprfor) { auto [var, iter, stmts] = exprfor; return mlc::String("[&]() {\nfor (auto ") + codegen::cpp_safe(var) + mlc::String(" : ") + codegen::gen_expr(iter, ctx) + mlc::String(") {\n") + codegen::gen_stmts_str(stmts, ctx) + mlc::String("}\n}()"); },
  [&](const ExprMatch& exprmatch) { auto [subject, arms] = exprmatch; return codegen::gen_match(subject, arms, ctx); },
  [&](const ExprRecord& exprrecord) { auto [name, field_vals] = exprrecord; return [&]() -> mlc::String { 
  mlc::Array<mlc::String> field_order = codegen::lookup_fields(ctx.field_orders, name);
  mlc::String vals = field_order.size() > 0 ? codegen::gen_record_ordered(field_vals, field_order, ctx) : codegen::gen_record_unordered(field_vals, ctx);
  return codegen::cpp_safe(name) + mlc::String("{") + vals + mlc::String("}");
 }(); },
  [&](const ExprRecordUpdate& exprrecordupdate) { auto [name, base, overrides] = exprrecordupdate; return [&]() -> mlc::String { 
  mlc::Array<mlc::String> field_order = codegen::lookup_fields(ctx.field_orders, name);
  mlc::String vals = codegen::gen_record_update_ordered(base, overrides, field_order, ctx);
  return codegen::cpp_safe(name) + mlc::String("{") + vals + mlc::String("}");
 }(); },
  [&](const ExprArray& exprarray) { auto [exprs] = exprarray; return mlc::String("mlc::Array<auto>{") + codegen::gen_args(exprs, ctx) + mlc::String("}"); },
  [&](const ExprQuestion& exprquestion) { auto [inner] = exprquestion; return codegen::gen_expr(inner, ctx); },
  [&](const ExprLambda& exprlambda) { auto [params, body] = exprlambda; return [&]() -> mlc::String { 
  mlc::String capture = params.size() == 0 ? mlc::String("[]") : mlc::String("[=]");
  mlc::String param_list = params.size() == 0 ? mlc::String("") : [&]() -> mlc::String { 
  mlc::Array<mlc::String> parts = {};
  int i = 0;
  while (i < params.size()){
{
parts.push_back(mlc::String("auto ") + codegen::cpp_safe(params[i]));
i = i + 1;
}
}
  return parts.join(mlc::String(", "));
 }();
  mlc::String body_code = codegen::gen_expr(body, ctx);
  return capture + mlc::String("(") + param_list + mlc::String(") { return ") + body_code + mlc::String("; }");
 }(); }
}, (*expr)._);}

mlc::String gen_record_ordered(mlc::Array<std::shared_ptr<ast::FieldVal>> field_vals, mlc::Array<mlc::String> field_order, codegen::CodegenContext ctx) noexcept{
mlc::Array<mlc::String> parts = {};
int i = 0;
while (i < field_order.size()){
{
std::shared_ptr<ast::Expr> value = codegen::find_field_val(field_vals, field_order[i]);
parts.push_back(codegen::gen_expr(value, ctx));
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
parts.push_back(codegen::gen_expr(field_vals[i]->val, ctx));
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
std::shared_ptr<ast::Expr> override_val = codegen::find_field_val(overrides, field_name);
mlc::String val_str = [&]() -> mlc::String { if (std::holds_alternative<ast::ExprUnit>((*override_val)._)) {  return codegen::gen_expr(std::make_shared<ast::Expr>(ast::ExprField(base, field_name)), ctx); } return codegen::gen_expr(override_val, ctx); }();
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

mlc::String gen_match(std::shared_ptr<ast::Expr> subject, mlc::Array<std::shared_ptr<ast::MatchArm>> arms, codegen::CodegenContext ctx) noexcept{
mlc::Array<mlc::String> parts = {};
int i = 0;
while (i < arms.size()){
{
parts.push_back(codegen::gen_arm(arms[i], ctx));
i = i + 1;
}
}
return mlc::String("std::visit(overloaded{") + parts.join(mlc::String(",\n")) + mlc::String("\n}, ") + codegen::gen_expr(subject, ctx) + mlc::String(")");
}

mlc::String gen_arm(std::shared_ptr<ast::MatchArm> arm, codegen::CodegenContext ctx) noexcept{return codegen::gen_pat_header(arm->pat) + mlc::String("{ return ") + codegen::gen_expr(arm->body, ctx) + mlc::String("; }");}

mlc::String gen_pat_header(std::shared_ptr<ast::Pat> pat) noexcept{return std::visit(overloaded{
  [&](const PatWild& patwild) { return mlc::String("[&](auto& __v) "); },
  [&](const PatUnit& patunit) { return mlc::String("[&](auto& __v) "); },
  [&](const PatBool& patbool) { auto [_w0] = patbool; return mlc::String("[&](auto& __v) "); },
  [&](const PatInt& patint) { auto [_w0] = patint; return mlc::String("[&](auto& __v) "); },
  [&](const PatStr& patstr) { auto [_w0] = patstr; return mlc::String("[&](auto& __v) "); },
  [&](const PatIdent& patident) { auto [name] = patident; return mlc::String("[&](auto ") + codegen::cpp_safe(name) + mlc::String(") "); },
  [&](const PatCtor& patctor) { auto [name, sub_pats] = patctor; return [&]() -> mlc::String { 
  mlc::String lower_name = codegen::cpp_safe(codegen::lower_first(name));
  mlc::String binding = sub_pats.size() == 0 ? mlc::String("") : mlc::String("{ auto [") + codegen::pat_bind_names(sub_pats) + mlc::String("] = ") + lower_name + mlc::String("; } ");
  return mlc::String("[&](const ") + codegen::cpp_safe(name) + mlc::String("& ") + lower_name + mlc::String(") ") + binding;
 }(); }
}, (*pat));}

mlc::String pat_bind_names(mlc::Array<std::shared_ptr<ast::Pat>> sub_pats) noexcept{
mlc::Array<mlc::String> parts = {};
int i = 0;
while (i < sub_pats.size()){
{
mlc::String bind_name = [&]() -> mlc::String { if (std::holds_alternative<ast::PatIdent>((*sub_pats[i]))) { auto _v_patident = std::get<ast::PatIdent>((*sub_pats[i])); auto [name] = _v_patident; return codegen::cpp_safe(name); } return mlc::String("__") + mlc::to_string(i); }();
parts.push_back(bind_name);
i = i + 1;
}
}
return parts.join(mlc::String(", "));
}

mlc::String gen_stmt(std::shared_ptr<ast::Stmt> stmt, codegen::CodegenContext ctx) noexcept{return std::visit(overloaded{
  [&](const StmtLet& stmtlet) { auto [name, _w0, typ, value] = stmtlet; return [&]() -> mlc::String { 
  mlc::String val_code = [&]() -> mlc::String { if (std::holds_alternative<ast::ExprArray>((*value)._)) { auto _v_exprarray = std::get<ast::ExprArray>((*value)._); auto [elems] = _v_exprarray; return elems.size() == 0 ? [&]() -> mlc::String { if (std::holds_alternative<ast::TyArray>((*typ))) { auto _v_tyarray = std::get<ast::TyArray>((*typ)); auto [inner] = _v_tyarray; return mlc::String("mlc::Array<") + codegen::type_to_cpp(inner) + mlc::String(">{}"); } return mlc::String("mlc::Array<auto>{}"); }() : mlc::String("mlc::Array<auto>{") + codegen::gen_args(elems, ctx) + mlc::String("}"); } return codegen::gen_expr(value, ctx); }();
  return mlc::String("auto ") + codegen::cpp_safe(name) + mlc::String(" = ") + val_code + mlc::String(";\n");
 }(); },
  [&](const StmtExpr& stmtexpr) { auto [expr] = stmtexpr; return codegen::gen_stmt_expr(expr, ctx); },
  [&](const StmtBreak& stmtbreak) { return mlc::String("break;\n"); },
  [&](const StmtContinue& stmtcontinue) { return mlc::String("continue;\n"); },
  [&](const StmtReturn& stmtreturn) { auto [expr] = stmtreturn; return mlc::String("return ") + codegen::gen_expr(expr, ctx) + mlc::String(";\n"); }
}, (*stmt)._);}

mlc::String gen_stmt_expr(std::shared_ptr<ast::Expr> expr, codegen::CodegenContext ctx) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprBin>((*expr)._)) { auto _v_exprbin = std::get<ast::ExprBin>((*expr)._); auto [op, left_expr, right_expr] = _v_exprbin; return op == mlc::String("=") ? codegen::gen_expr(left_expr, ctx) + mlc::String(" = ") + codegen::gen_expr(right_expr, ctx) + mlc::String(";\n") : mlc::String("(") + codegen::gen_expr(left_expr, ctx) + mlc::String(" ") + op + mlc::String(" ") + codegen::gen_expr(right_expr, ctx) + mlc::String(");\n"); } if (std::holds_alternative<ast::ExprIf>((*expr)._)) { auto _v_exprif = std::get<ast::ExprIf>((*expr)._); auto [cond, then_expr, else_expr] = _v_exprif; return [&]() -> mlc::String { 
  mlc::String output = mlc::String("if (") + codegen::gen_expr(cond, ctx) + mlc::String(") {\n") + codegen::gen_block_body(then_expr, ctx) + mlc::String("}");
  mlc::String else_str = [&]() -> mlc::String { if (std::holds_alternative<ast::ExprUnit>((*else_expr)._)) {  return mlc::String(""); } if (std::holds_alternative<ast::ExprBlock>((*else_expr)._)) { auto _v_exprblock = std::get<ast::ExprBlock>((*else_expr)._); auto [stmts, result] = _v_exprblock; return stmts.size() == 0 ? [&]() -> mlc::String { if (std::holds_alternative<ast::ExprUnit>((*result)._)) {  return mlc::String(""); } return mlc::String(" else {\n") + codegen::gen_block_body(else_expr, ctx) + mlc::String("}"); }() : mlc::String(" else {\n") + codegen::gen_block_body(else_expr, ctx) + mlc::String("}"); } if (std::holds_alternative<ast::ExprIf>((*else_expr)._)) { auto _v_exprif = std::get<ast::ExprIf>((*else_expr)._); auto [_w0, _w1, _w2] = _v_exprif; return mlc::String(" else ") + codegen::gen_stmt_expr(else_expr, ctx); } return mlc::String(" else {\n") + codegen::gen_block_body(else_expr, ctx) + mlc::String("}"); }();
  if (else_str.length() > 0){
{
output = output + else_str;
}
}
  return output + mlc::String("\n");
 }(); } if (std::holds_alternative<ast::ExprWhile>((*expr)._)) { auto _v_exprwhile = std::get<ast::ExprWhile>((*expr)._); auto [cond, stmts] = _v_exprwhile; return mlc::String("while (") + codegen::gen_expr(cond, ctx) + mlc::String(") {\n") + codegen::gen_stmts_str(stmts, ctx) + mlc::String("}\n"); } if (std::holds_alternative<ast::ExprFor>((*expr)._)) { auto _v_exprfor = std::get<ast::ExprFor>((*expr)._); auto [var, iter, stmts] = _v_exprfor; return mlc::String("for (auto ") + codegen::cpp_safe(var) + mlc::String(" : ") + codegen::gen_expr(iter, ctx) + mlc::String(") {\n") + codegen::gen_stmts_str(stmts, ctx) + mlc::String("}\n"); } if (std::holds_alternative<ast::ExprBlock>((*expr)._)) { auto _v_exprblock = std::get<ast::ExprBlock>((*expr)._); auto [stmts, result] = _v_exprblock; return [&]() -> mlc::String { 
  mlc::String stmts_code = codegen::gen_stmts_str(stmts, ctx);
  mlc::String result_code = codegen::gen_expr(result, ctx);
  return result_code == mlc::String("/* unit */") ? stmts_code : stmts_code + result_code + mlc::String(";\n");
 }(); } return codegen::gen_expr(expr, ctx) + mlc::String(";\n"); }();}

mlc::String gen_return_body(std::shared_ptr<ast::Expr> expr, codegen::CodegenContext ctx) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprBlock>((*expr)._)) { auto _v_exprblock = std::get<ast::ExprBlock>((*expr)._); auto [stmts, result] = _v_exprblock; return [&]() -> mlc::String { 
  mlc::String stmts_code = codegen::gen_stmts_str(stmts, ctx);
  mlc::String result_code = codegen::gen_expr(result, ctx);
  return result_code == mlc::String("/* unit */") ? stmts_code : stmts_code + mlc::String("return ") + result_code + mlc::String(";\n");
 }(); } if (std::holds_alternative<ast::ExprIf>((*expr)._)) { auto _v_exprif = std::get<ast::ExprIf>((*expr)._); auto [cond, then_expr, else_expr] = _v_exprif; return [&]() -> mlc::String { 
  mlc::String output = mlc::String("if (") + codegen::gen_expr(cond, ctx) + mlc::String(") {\n") + codegen::gen_return_body(then_expr, ctx) + mlc::String("}");
  mlc::String else_str = [&]() -> mlc::String { if (std::holds_alternative<ast::ExprUnit>((*else_expr)._)) {  return mlc::String(""); } if (std::holds_alternative<ast::ExprIf>((*else_expr)._)) { auto _v_exprif = std::get<ast::ExprIf>((*else_expr)._); auto [_w0, _w1, _w2] = _v_exprif; return mlc::String(" else ") + codegen::gen_return_if_stmt(else_expr, ctx); } return mlc::String(" else {\n") + codegen::gen_return_body(else_expr, ctx) + mlc::String("}"); }();
  if (else_str.length() > 0){
{
output = output + else_str;
}
}
  return output + mlc::String("\n");
 }(); } return mlc::String("return ") + codegen::gen_expr(expr, ctx) + mlc::String(";\n"); }();}

mlc::String gen_return_if_stmt(std::shared_ptr<ast::Expr> expr, codegen::CodegenContext ctx) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprIf>((*expr)._)) { auto _v_exprif = std::get<ast::ExprIf>((*expr)._); auto [cond, then_expr, else_expr] = _v_exprif; return [&]() -> mlc::String { 
  mlc::String output = mlc::String("if (") + codegen::gen_expr(cond, ctx) + mlc::String(") {\n") + codegen::gen_return_body(then_expr, ctx) + mlc::String("}");
  mlc::String else_str = [&]() -> mlc::String { if (std::holds_alternative<ast::ExprUnit>((*else_expr)._)) {  return mlc::String(""); } if (std::holds_alternative<ast::ExprIf>((*else_expr)._)) { auto _v_exprif = std::get<ast::ExprIf>((*else_expr)._); auto [_w0, _w1, _w2] = _v_exprif; return mlc::String(" else ") + codegen::gen_return_if_stmt(else_expr, ctx); } return mlc::String(" else {\n") + codegen::gen_return_body(else_expr, ctx) + mlc::String("}"); }();
  if (else_str.length() > 0){
{
output = output + else_str;
}
}
  return output + mlc::String("\n");
 }(); } return mlc::String("if (true) {\n") + codegen::gen_return_body(expr, ctx) + mlc::String("}\n"); }();}

mlc::String gen_fn_body(std::shared_ptr<ast::Expr> body, codegen::CodegenContext ctx) noexcept{return codegen::gen_return_body(body, ctx);}

mlc::String gen_params(mlc::Array<std::shared_ptr<ast::Param>> params) noexcept{
mlc::Array<mlc::String> parts = {};
int i = 0;
while (i < params.size()){
{
parts.push_back(codegen::type_to_cpp(params[i]->typ) + mlc::String(" ") + codegen::cpp_safe(params[i]->name));
i = i + 1;
}
}
return parts.join(mlc::String(", "));
}

mlc::String gen_type_decl_fwd(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{return variants.size() == 1 ? mlc::String("") : codegen::gen_adt_fwd(type_name, variants);}

mlc::String gen_type_decl_body(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{return variants.size() == 1 ? codegen::gen_single_variant(type_name, variants[0]) : codegen::gen_adt_defs(type_name, variants);}

mlc::String gen_single_variant(mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept{return std::visit(overloaded{
  [&](const VarRecord& varrecord) { auto [_w0, field_defs] = varrecord; return [&]() -> mlc::String { 
  mlc::Array<mlc::String> parts = {};
  int i = 0;
  while (i < field_defs.size()){
{
parts.push_back(codegen::type_to_cpp(field_defs[i]->typ) + mlc::String(" ") + codegen::cpp_safe(field_defs[i]->name) + mlc::String(";"));
i = i + 1;
}
}
  return mlc::String("struct ") + codegen::cpp_safe(type_name) + mlc::String(" {") + parts.join(mlc::String("")) + mlc::String("};\n");
 }(); },
  [&](const VarTuple& vartuple) { auto [_w0, field_types] = vartuple; return [&]() -> mlc::String { 
  mlc::Array<mlc::String> parts = {};
  int i = 0;
  while (i < field_types.size()){
{
parts.push_back(codegen::type_to_cpp(field_types[i]) + mlc::String(" field") + mlc::to_string(i) + mlc::String(";"));
i = i + 1;
}
}
  return mlc::String("struct ") + codegen::cpp_safe(type_name) + mlc::String(" {") + parts.join(mlc::String("")) + mlc::String("};\n");
 }(); },
  [&](const VarUnit& varunit) { auto [_w0] = varunit; return mlc::String("struct ") + codegen::cpp_safe(type_name) + mlc::String(" {};\n"); }
}, (*variant));}

mlc::String gen_adt_fwd(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
mlc::Array<mlc::String> parts = {};
mlc::Array<mlc::String> variant_names = {};
int index = 0;
while (index < variants.size()){
{
mlc::String variant_name = codegen::cpp_safe(codegen::variant_ctor_name(variants[index]));
variant_names.push_back(variant_name);
parts.push_back(mlc::String("struct ") + variant_name + mlc::String(";\n"));
index = index + 1;
}
}
return parts.join(mlc::String("")) + mlc::String("using ") + codegen::cpp_safe(type_name) + mlc::String(" = std::variant<") + variant_names.join(mlc::String(", ")) + mlc::String(">;\n");
}

mlc::String gen_adt_defs(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
mlc::Array<mlc::String> parts = {};
int i = 0;
while (i < variants.size()){
{
parts.push_back(codegen::gen_variant_struct(type_name, variants[i]));
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

mlc::String gen_variant_struct(mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept{return std::visit(overloaded{
  [&](const VarUnit& varunit) { auto [name] = varunit; return mlc::String("struct ") + codegen::cpp_safe(name) + mlc::String(" {};\n"); },
  [&](const VarTuple& vartuple) { auto [name, field_types] = vartuple; return [&]() -> mlc::String { 
  mlc::Array<mlc::String> parts = {};
  int i = 0;
  while (i < field_types.size()){
{
parts.push_back(codegen::type_to_cpp(field_types[i]) + mlc::String(" field") + mlc::to_string(i) + mlc::String(";"));
i = i + 1;
}
}
  return mlc::String("struct ") + codegen::cpp_safe(name) + mlc::String(" {") + parts.join(mlc::String("")) + mlc::String("};\n");
 }(); },
  [&](const VarRecord& varrecord) { auto [name, field_defs] = varrecord; return [&]() -> mlc::String { 
  mlc::Array<mlc::String> parts = {};
  int i = 0;
  while (i < field_defs.size()){
{
parts.push_back(codegen::type_to_cpp(field_defs[i]->typ) + mlc::String(" ") + codegen::cpp_safe(field_defs[i]->name) + mlc::String(";"));
i = i + 1;
}
}
  return mlc::String("struct ") + codegen::cpp_safe(name) + mlc::String(" {") + parts.join(mlc::String("")) + mlc::String("};\n");
 }(); }
}, (*variant));}

mlc::String gen_fn_decl(mlc::String name, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<ast::TypeExpr> return_type, std::shared_ptr<ast::Expr> body, codegen::CodegenContext ctx) noexcept{
mlc::String safe_name = codegen::cpp_safe(name);
return name == mlc::String("main") && params.size() == 0 ? [&]() -> mlc::String { 
  mlc::String preamble = mlc::String("mlc::io::set_args(std::vector<mlc::String>(argv + 1, argv + argc));\n");
  return codegen::type_to_cpp(return_type) + mlc::String(" ") + safe_name + mlc::String("(int argc, char** argv) noexcept{\n") + preamble + codegen::gen_fn_body(body, ctx) + mlc::String("}\n");
 }() : codegen::type_to_cpp(return_type) + mlc::String(" ") + safe_name + mlc::String("(") + codegen::gen_params(params) + mlc::String(") noexcept{\n") + codegen::gen_fn_body(body, ctx) + mlc::String("}\n");
}

mlc::String gen_fn_proto(mlc::String name, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<ast::TypeExpr> return_type) noexcept{
mlc::String safe_name = codegen::cpp_safe(name);
return name == mlc::String("main") && params.size() == 0 ? codegen::type_to_cpp(return_type) + mlc::String(" ") + safe_name + mlc::String("(int argc, char** argv) noexcept;\n") : codegen::type_to_cpp(return_type) + mlc::String(" ") + safe_name + mlc::String("(") + codegen::gen_params(params) + mlc::String(") noexcept;\n");
}

mlc::String gen_type_decl(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{return codegen::gen_type_decl_fwd(type_name, variants) + codegen::gen_type_decl_body(type_name, variants);}

mlc::String gen_decl(std::shared_ptr<ast::Decl> decl, codegen::CodegenContext ctx) noexcept{return std::visit(overloaded{
  [&](const DeclType& decltype_) { auto [type_name, variants] = decltype_; return codegen::gen_type_decl(type_name, variants); },
  [&](const DeclFn& declfn) { auto [name, params, return_type, body] = declfn; return codegen::gen_fn_decl(name, params, return_type, body, ctx); },
  [&](const DeclExtend& declextend) { auto [_w0, methods] = declextend; return [&]() -> mlc::String { 
  mlc::Array<mlc::String> parts = {};
  int index = 0;
  while (index < methods.size()){
{
parts.push_back(codegen::gen_decl(methods[index], ctx));
index = index + 1;
}
}
  return parts.join(mlc::String(""));
 }(); },
  [&](const DeclImport& declimport) { auto [_w0, _w1] = declimport; return mlc::String(""); },
  [&](const DeclExported& declexported) { auto [d] = declexported; return codegen::gen_decl(d, ctx); }
}, (*decl));}

mlc::String gen_proto(std::shared_ptr<ast::Decl> decl) noexcept{return std::visit(overloaded{
  [&](const DeclType& decltype_) { auto [_w0, _w1] = decltype_; return mlc::String(""); },
  [&](const DeclFn& declfn) { auto [name, params, ret, _w0] = declfn; return codegen::gen_fn_proto(name, params, ret); },
  [&](const DeclExtend& declextend) { auto [_w0, methods] = declextend; return [&]() -> mlc::String { 
  mlc::Array<mlc::String> parts = {};
  int index = 0;
  while (index < methods.size()){
{
parts.push_back(codegen::gen_proto(methods[index]));
index = index + 1;
}
}
  return parts.join(mlc::String(""));
 }(); },
  [&](const DeclImport& declimport) { auto [_w0, _w1] = declimport; return mlc::String(""); },
  [&](const DeclExported& declexported) { auto [d] = declexported; return codegen::gen_proto(d); }
}, (*decl));}

mlc::Array<mlc::String> collect_decl_parts(mlc::Array<std::shared_ptr<ast::Decl>> decls, codegen::CodegenContext ctx, int phase) noexcept{
mlc::Array<mlc::String> parts = {};
int index = 0;
while (index < decls.size()){
{
mlc::String segment = std::visit(overloaded{
  [&](const DeclType& decltype_) { auto [type_name, variants] = decltype_; return phase == 0 ? codegen::gen_type_decl_fwd(type_name, variants) : phase == 1 ? codegen::gen_type_decl_body(type_name, variants) : mlc::String(""); },
  [&](const DeclFn& declfn) { auto [_w0, _w1, _w2, _w3] = declfn; return phase == 2 ? codegen::gen_proto(decls[index]) : phase == 3 ? codegen::gen_decl(decls[index], ctx) : mlc::String(""); },
  [&](const DeclExtend& declextend) { auto [_w0, _w1] = declextend; return phase == 2 ? codegen::gen_proto(decls[index]) : phase == 3 ? codegen::gen_decl(decls[index], ctx) : mlc::String(""); },
  [&](const DeclImport& declimport) { auto [_w0, _w1] = declimport; return mlc::String(""); },
  [&](const DeclExported& declexported) { auto [d] = declexported; return phase == 2 ? codegen::gen_proto(d) : phase == 3 ? codegen::gen_decl(d, ctx) : mlc::String(""); }
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
codegen::CodegenContext ctx = codegen::create_codegen_context(prog);
mlc::Array<mlc::String> preamble = mlc::Array<mlc::String>{mlc::String("#include \"mlc/core/string.hpp\"\n"), mlc::String("#include \"mlc/core/array.hpp\"\n"), mlc::String("#include \"mlc/core/hashmap.hpp\"\n"), mlc::String("#include \"mlc/core/match.hpp\"\n"), mlc::String("#include \"mlc/io/io.hpp\"\n"), mlc::String("#include \"mlc/io/file.hpp\"\n"), mlc::String("#include <functional>\n"), mlc::String("#include <memory>\n"), mlc::String("#include <variant>\n"), mlc::String("#include <vector>\n\n")};
mlc::Array<mlc::String> type_fwds = codegen::collect_decl_parts(prog.decls, ctx, 0);
mlc::Array<mlc::String> type_defs = codegen::collect_decl_parts(prog.decls, ctx, 1);
mlc::Array<mlc::String> fn_protos = codegen::collect_decl_parts(prog.decls, ctx, 2);
mlc::Array<mlc::String> fn_decls = codegen::collect_decl_parts(prog.decls, ctx, 3);
return ast::errs_append(ast::errs_append(ast::errs_append(ast::errs_append(preamble, type_fwds), type_defs), ast::errs_append(fn_protos, mlc::Array<mlc::String>{mlc::String("\n")})), fn_decls).join(mlc::String(""));
}

} // namespace codegen
