#include "hover.hpp"

#include "ast.hpp"
#include "lexer.hpp"
#include "decls.hpp"
#include "registry.hpp"
#include "param_destructure_expand.hpp"
#include "partial_application_desugar.hpp"
#include "check_context.hpp"
#include "infer.hpp"
#include "infer_expr_ident.hpp"
#include "semantic_type_structure.hpp"
#include "symbols.hpp"

namespace hover {

using namespace ast;
using namespace lexer;
using namespace decls;
using namespace registry;
using namespace param_destructure_expand;
using namespace partial_application_desugar;
using namespace check_context;
using namespace infer;
using namespace infer_expr_ident;
using namespace semantic_type_structure;
using namespace symbols;
using namespace ast_tokens;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> hover_function_parameter_environment(mlc::Array<std::shared_ptr<ast::Param>> parameters, registry::TypeRegistry registry) noexcept;

mlc::String hover_type_for_identifier_in_program(ast::Program program, registry::TypeRegistry registry, mlc::String identifier_name, int target_line_one_based) noexcept;

mlc::String resolve_hover_type_in_source(mlc::String source_text, mlc::String source_file, int line_zero_based, int column_zero_based) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> hover_function_parameter_environment(mlc::Array<std::shared_ptr<ast::Param>> parameters, registry::TypeRegistry registry) noexcept{
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
int index = 0;
while (index < parameters.size()){
{
std::shared_ptr<ast::Param> parameter = parameters[index];
type_environment.set(ast::param_name(parameter), registry::type_from_annotation_with_registry(ast::param_type_value(parameter), registry));
index = index + 1;
}
}
return type_environment;
}

mlc::String hover_type_for_identifier_in_program(ast::Program program, registry::TypeRegistry registry, mlc::String identifier_name, int target_line_one_based) noexcept{
return registry::TypeRegistry_has_fn(registry, identifier_name) ? semantic_type_structure::type_description(registry::TypeRegistry_fn_type(registry, identifier_name)) : registry::TypeRegistry_has_ctor(registry, identifier_name) ? semantic_type_structure::type_description(registry::TypeRegistry_ctor_type(registry, identifier_name)) : [&]() -> mlc::String { 
  int index = 0;
  mlc::String found_type = mlc::String("");
  while (index < program.decls.size() && found_type.length() == 0){
{
std::visit(overloaded{
  [&](const DeclFn& declfn) {
auto [_w0, _w1, _w2, parameters, _w3, function_body, _w4] = declfn;
{
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment = hover_function_parameter_environment(parameters, registry);
std::shared_ptr<ast::Expr> desugared_body = partial_application_desugar::partial_application_desugar_expr(function_body);
[&]() -> void { if (std::holds_alternative<ast::ExprBlock>((*desugared_body)._)) { auto _v_exprblock = std::get<ast::ExprBlock>((*desugared_body)._); auto [statements, _w0, _w1] = _v_exprblock; return [&]() { 
  int statement_index = 0;
  return [&]() { 
  while (statement_index < statements.size()){
{
[&]() -> void { if (std::holds_alternative<ast::StmtLet>((*statements[statement_index])._)) { auto _v_stmtlet = std::get<ast::StmtLet>((*statements[statement_index])._); auto [name, _w0, _w1, value, statement_span] = _v_stmtlet; return [&]() { 
  if (statement_span.line <= target_line_one_based){
{
check_context::CheckContext inference_context = check_context::check_context_new(type_environment, registry);
type_environment.set(name, infer::infer_expr(value, inference_context).inferred_type);
}
}
  return [&]() { 
  while (false){
{
}
}
 }();
 }(); } if (std::holds_alternative<ast::StmtExpr>((*statements[statement_index])._)) { auto _v_stmtexpr = std::get<ast::StmtExpr>((*statements[statement_index])._); auto [expression, _w0] = _v_stmtexpr; return [&]() { 
  if (ast::stmt_span(statements[statement_index]).line <= target_line_one_based){
{
infer::infer_expr(expression, check_context::check_context_new(type_environment, registry));
}
}
  return [&]() { 
  while (false){
{
}
}
 }();
 }(); } return; }();
statement_index = statement_index + 1;
}
}
 }();
 }(); } return; }();
check_context::CheckContext inference_context = check_context::check_context_new(type_environment, registry);
infer_result::InferResult inferred = infer_expr_ident::infer_expr_identifier(identifier_name, inference_context);
found_type = semantic_type_structure::type_description(inferred.inferred_type);
}
},
  [&](const auto& _unused) {
{
}
}
}, (*ast::decl_inner(program.decls[index])));
index = index + 1;
}
}
  return found_type;
 }();
}

mlc::String resolve_hover_type_in_source(mlc::String source_text, mlc::String source_file, int line_zero_based, int column_zero_based) noexcept{
ast::Program expanded_program = param_destructure_expand::expand_parameter_destructuring_in_program(decls::parse_program(lexer::tokenize(source_text).tokens));
registry::TypeRegistry registry = registry::build_registry(expanded_program);
mlc::String identifier_name = symbols::find_identifier_at_position(source_text, line_zero_based, column_zero_based);
return identifier_name.length() == 0 ? mlc::String("") : hover_type_for_identifier_in_program(expanded_program, registry, identifier_name, line_zero_based + 1);
}

} // namespace hover
