#include "let_pat.hpp"

#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "context.hpp"
#include "type_gen.hpp"
#include "cpp_naming.hpp"
#include "match_analysis.hpp"
#include "expr.hpp"

namespace let_pat {

using namespace ast;
using namespace semantic_ir;
using namespace registry;
using namespace context;
using namespace type_gen;
using namespace cpp_naming;
using namespace match_analysis;
using namespace expr;
using namespace ast_tokens;

mlc::String gen_let_array_rest_slice(mlc::String temp, int from_index, mlc::String element_cpp) noexcept;

mlc::String gen_let_pattern_statement(std::shared_ptr<ast::Pat> pat, std::shared_ptr<semantic_ir::SExpr> value, std::shared_ptr<registry::Type> value_type, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> eval_fn) noexcept;

mlc::String gen_let_array_rest_slice(mlc::String temp, int from_index, mlc::String element_cpp) noexcept{return mlc::String("mlc::Array<") + element_cpp + mlc::String(">(") + temp + mlc::String(".cbegin() + ") + mlc::to_string(from_index) + mlc::String(", ") + temp + mlc::String(".cend())");}

mlc::String gen_let_pattern_statement(std::shared_ptr<ast::Pat> pat, std::shared_ptr<semantic_ir::SExpr> value, std::shared_ptr<registry::Type> value_type, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> eval_fn) noexcept{
mlc::String rhs = eval_fn(value, context);
return [&]() -> mlc::String { if (std::holds_alternative<ast::PatIdent>((*pat))) { auto _v_patident = std::get<ast::PatIdent>((*pat)); auto [n, _w0] = _v_patident; return expr::auto_binding_statement(cpp_naming::cpp_safe(n), rhs); } if (std::holds_alternative<ast::PatTuple>((*pat))) { auto _v_pattuple = std::get<ast::PatTuple>((*pat)); auto [subs, _w0] = _v_pattuple; return [&]() -> mlc::String { 
  mlc::String names = match_analysis::pat_bind_names(subs);
  return mlc::String("{\nauto __lt = ") + rhs + mlc::String(";\n") + expr::tuple_destructure_binding(names, mlc::String("__lt")) + mlc::String("\n}\n");
 }(); } if (std::holds_alternative<ast::PatRecord>((*pat))) { auto _v_patrecord = std::get<ast::PatRecord>((*pat)); auto [_w0, field_pats, _w1] = _v_patrecord; return [&]() -> mlc::String { 
  mlc::String b = mlc::String("{\nauto __lt = ") + rhs + mlc::String(";\n");
  int i = 0;
  while (i < field_pats.size()){
{
std::visit(overloaded{
  [&](const PatIdent& patident) {
auto [fname, _w0] = patident;
{
b = b + expr::auto_binding_statement(cpp_naming::cpp_safe(fname), mlc::String("__lt.") + cpp_naming::cpp_safe(fname));
}
},
  [&](const auto& _unused) {
{
}
}
}, (*field_pats[i]));
i = i + 1;
}
}
  return b + mlc::String("}\n");
 }(); } if (std::holds_alternative<ast::PatArray>((*pat))) { auto _v_patarray = std::get<ast::PatArray>((*pat)); auto [subs, rest, _w0] = _v_patarray; return [&]() -> mlc::String { 
  mlc::String et = [&]() -> mlc::String { if (std::holds_alternative<registry::TArray>((*value_type))) { auto _v_tarray = std::get<registry::TArray>((*value_type)); auto [inn] = _v_tarray; return type_gen::sem_type_to_cpp(context, inn); } return mlc::String("auto"); }();
  mlc::String b = mlc::String("{\nauto __lt = ") + rhs + mlc::String(";\n");
  int j = 0;
  while (j < subs.size()){
{
std::visit(overloaded{
  [&](const PatIdent& patident) {
auto [n, _w0] = patident;
{
b = b + mlc::String("auto ") + cpp_naming::cpp_safe(n) + mlc::String(" = __lt[") + mlc::to_string(j) + mlc::String("];\n");
}
},
  [&](const auto& _unused) {
{
}
}
}, (*subs[j]));
j = j + 1;
}
}
  if (rest != mlc::String("")){
{
b = b + mlc::String("auto ") + cpp_naming::cpp_safe(rest) + mlc::String(" = ") + gen_let_array_rest_slice(mlc::String("__lt"), subs.size(), et) + mlc::String(";\n");
}
}
  return b + mlc::String("}\n");
 }(); } if (std::holds_alternative<ast::PatCtor>((*pat))) { auto _v_patctor = std::get<ast::PatCtor>((*pat)); auto [ctor_name, sub_patterns, _w0] = _v_patctor; return [&]() -> mlc::String { 
  mlc::String q = context::context_resolve(context, ctor_name);
  mlc::String bind = sub_patterns.size() == 0 ? mlc::String("") : expr::tuple_destructure_binding(match_analysis::pat_bind_names(sub_patterns), mlc::String("__b"));
  bool is_generic = decl_index::list_contains(context.generic_variants, ctor_name);
  mlc::String targs = is_generic ? mlc::String("<auto>") : mlc::String("");
  return mlc::String("{\n") + mlc::String("auto __lt = ") + rhs + mlc::String(";\n") + mlc::String("if (!std::holds_alternative<") + q + targs + mlc::String(">(__lt)) { std::abort(); }\n") + mlc::String("const ") + q + targs + mlc::String("& __b = std::get<") + q + targs + mlc::String(">(__lt);\n") + bind + mlc::String("\n}\n");
 }(); } return expr::suffix_semicolon_newline(rhs); }();
}

} // namespace let_pat
