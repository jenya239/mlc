#define main mlc_user_main
#include "param_destructure_expand.hpp"

#include "ast.hpp"
#include "diagnostic_codes.hpp"

namespace param_destructure_expand {

using namespace ast;
using namespace diagnostic_codes;
using namespace ast_tokens;

mlc::String synthetic_destructure_parameter_name(int parameter_position_index) noexcept{
  return (mlc::String("__mlc_ds", 8) + mlc::to_string(parameter_position_index));
}
bool parameter_binding_is_plain_identifier(std::shared_ptr<ast::Param> parameter) noexcept{
  return [&]() -> bool {
auto __match_subject = parameter->param_pattern;
if (std::holds_alternative<ast::PatternUnit>((*__match_subject))) {
const ast::PatternUnit& patternUnit = std::get<ast::PatternUnit>((*__match_subject));
auto [__0] = patternUnit; return true;
}
return false;
std::abort();
}();
}
bool parameter_pattern_is_plain(std::shared_ptr<ast::Param> parameter) noexcept{
  return parameter_binding_is_plain_identifier(parameter);
}
std::shared_ptr<ast::Param> strip_parameter_to_synthetic_plain(std::shared_ptr<ast::Param> parameter, mlc::String synthetic_name) noexcept{
  return std::make_shared<ast::Param>(ast::Param{synthetic_name, false, parameter->type_value, parameter->has_default, parameter->default_, std::make_shared<ast::Pattern>(ast::PatternUnit{ast::pattern_span(parameter->param_pattern)})});
}
std::shared_ptr<ast::Stmt> prelude_statement_destructure(std::shared_ptr<ast::Param> parameter, mlc::String synthetic_name, ast::Span binding_span) noexcept{
  return std::make_shared<ast::Stmt>(ast::StmtLetPattern{parameter->param_pattern, parameter->is_mut, parameter->type_value, std::make_shared<ast::Expr>(ast::ExprIdent{synthetic_name, binding_span}), false, std::make_shared<ast::Expr>(ast::ExprBlock{{}, std::make_shared<ast::Expr>(ast::ExprUnit{binding_span}), binding_span}), binding_span});
}
std::shared_ptr<ast::Expr> wrap_expression_with_statement_prelude(mlc::Array<std::shared_ptr<ast::Stmt>> prelude_statements, std::shared_ptr<ast::Expr> body_expression) noexcept{
  return [&]() -> std::shared_ptr<ast::Expr> {
auto __match_subject = body_expression;
if (std::holds_alternative<ast::ExprBlock>((*__match_subject))) {
const ast::ExprBlock& exprBlock = std::get<ast::ExprBlock>((*__match_subject));
auto [inner_statements, result_expression, block_span] = exprBlock; return std::make_shared<ast::Expr>(ast::ExprBlock{prelude_statements.concat(inner_statements), result_expression, block_span});
}
return [&]() {
auto outer_span = ast::expr_span(body_expression);
return std::make_shared<ast::Expr>(ast::ExprBlock{prelude_statements, body_expression, outer_span});
}();
std::abort();
}();
}
Expand_fn_parameter_destructure_fold_state expand_decl_fn_parameter_destructure_fold_step(Expand_fn_parameter_destructure_fold_state state, std::shared_ptr<ast::Param> current_parameter) noexcept{
  if (parameter_pattern_is_plain(current_parameter))   {
    return Expand_fn_parameter_destructure_fold_state{state.prelude_statements, state.new_parameters.concat(mlc::Array<std::shared_ptr<ast::Param>>{current_parameter}), mlc::arith::checked_add(state.parameter_position_index, 1)};
  } else   {
    auto synthetic_name = synthetic_destructure_parameter_name(state.parameter_position_index);
    auto binding_span = ast::pattern_span(current_parameter->param_pattern);
    return Expand_fn_parameter_destructure_fold_state{state.prelude_statements.concat(mlc::Array<std::shared_ptr<ast::Stmt>>{prelude_statement_destructure(current_parameter, synthetic_name, binding_span)}), state.new_parameters.concat(mlc::Array<std::shared_ptr<ast::Param>>{strip_parameter_to_synthetic_plain(current_parameter, synthetic_name)}), mlc::arith::checked_add(state.parameter_position_index, 1)};
  }
}
std::shared_ptr<ast::Decl> expand_decl_fn_parameter_destructuring_inner(mlc::String function_name, mlc::Array<mlc::String> type_parameter_names, mlc::Array<mlc::Array<mlc::String>> trait_bounds_rows, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<ast::TypeExpr> return_type_expression, std::shared_ptr<ast::Expr> body_expression, mlc::Array<ast::WhereClauseBound> where_clause_bounds_entries) noexcept{
  auto fold_result = parameters.fold(Expand_fn_parameter_destructure_fold_state{[&]() {
auto empty_prelude = mlc::Array<std::shared_ptr<ast::Stmt>>{};
return empty_prelude;
}(), [&]() {
auto empty_parameters = mlc::Array<std::shared_ptr<ast::Param>>{};
return empty_parameters;
}(), 0}, [=](Expand_fn_parameter_destructure_fold_state state, std::shared_ptr<ast::Param> current_parameter) mutable { return expand_decl_fn_parameter_destructure_fold_step(state, current_parameter); });
  auto expanded_body = ((fold_result.prelude_statements.length() > 0) ? (wrap_expression_with_statement_prelude(fold_result.prelude_statements, body_expression)) : (body_expression));
  return std::make_shared<ast::Decl>(ast::DeclFn{function_name, type_parameter_names, trait_bounds_rows, fold_result.new_parameters, return_type_expression, expanded_body, where_clause_bounds_entries});
}
std::shared_ptr<ast::Decl> expand_parameter_destructuring_decl_shared(std::shared_ptr<ast::Decl> declaration) noexcept{
  return [&]() -> std::shared_ptr<ast::Decl> {
auto __match_subject = declaration;
if (std::holds_alternative<ast::DeclFn>((*__match_subject))) {
const ast::DeclFn& declFn = std::get<ast::DeclFn>((*__match_subject));
auto [function_name, type_parameter_names, trait_bounds_rows, parameters, return_type_expression, body_expression, where_clause_bounds_entries] = declFn; return expand_decl_fn_parameter_destructuring_inner(function_name, type_parameter_names, trait_bounds_rows, parameters, return_type_expression, body_expression, where_clause_bounds_entries);
}
if (std::holds_alternative<ast::DeclExported>((*__match_subject))) {
const ast::DeclExported& declExported = std::get<ast::DeclExported>((*__match_subject));
auto [inner_declaration] = declExported; return std::make_shared<ast::Decl>(ast::DeclExported{expand_parameter_destructuring_decl_shared(inner_declaration)});
}
if (std::holds_alternative<ast::DeclExtend>((*__match_subject))) {
const ast::DeclExtend& declExtend = std::get<ast::DeclExtend>((*__match_subject));
auto [extended_type_name, implemented_trait_name, methods, name_span] = declExtend; return std::make_shared<ast::Decl>(ast::DeclExtend{extended_type_name, implemented_trait_name, expand_extend_methods_parameter_destructuring(methods), name_span});
}
return declaration;
std::abort();
}();
}
mlc::Array<std::shared_ptr<ast::Decl>> expand_extend_methods_parameter_destructuring(mlc::Array<std::shared_ptr<ast::Decl>> methods) noexcept{
  return methods.fold([&]() {
auto empty_methods = mlc::Array<std::shared_ptr<ast::Decl>>{};
return empty_methods;
}(), [=](mlc::Array<std::shared_ptr<ast::Decl>> output_methods, std::shared_ptr<ast::Decl> method) mutable { return output_methods.concat(mlc::Array<std::shared_ptr<ast::Decl>>{expand_parameter_destructuring_decl_shared(method)}); });
}
ast::Program expand_parameter_destructuring_in_program(ast::Program program) noexcept{
  return ast::Program{program.decls.fold([&]() {
auto empty_declarations = mlc::Array<std::shared_ptr<ast::Decl>>{};
return empty_declarations;
}(), [=](mlc::Array<std::shared_ptr<ast::Decl>> output_declarations, std::shared_ptr<ast::Decl> declaration) mutable { return output_declarations.concat(mlc::Array<std::shared_ptr<ast::Decl>>{expand_parameter_destructuring_decl_shared(declaration)}); })};
}
mlc::Array<ast::Diagnostic> concatenate_diagnostic_arrays(mlc::Array<ast::Diagnostic> existing, mlc::Array<ast::Diagnostic> extra) noexcept{
  return existing.concat(extra);
}
mlc::Array<ast::Diagnostic> extern_parameter_destructure_errors_for_decl_shared(std::shared_ptr<ast::Decl> declaration) noexcept{
  return [&]() -> mlc::Array<ast::Diagnostic> {
auto __match_subject = declaration;
if (std::holds_alternative<ast::DeclFn>((*__match_subject))) {
const ast::DeclFn& declFn = std::get<ast::DeclFn>((*__match_subject));
auto [__0, __1, __2, parameters, __4, body_expression, __6] = declFn; return [&]() -> mlc::Array<ast::Diagnostic> {
auto __match_subject = body_expression;
if (std::holds_alternative<ast::ExprExtern>((*__match_subject))) {
const ast::ExprExtern& exprExtern = std::get<ast::ExprExtern>((*__match_subject));
auto [__0, __1, __2, error_span] = exprExtern; return parameters.fold([&]() {
auto empty_diagnostics = mlc::Array<ast::Diagnostic>{};
return empty_diagnostics;
}(), [=](mlc::Array<ast::Diagnostic> scan_errors, std::shared_ptr<ast::Param> parameter) mutable { return ((!parameter_pattern_is_plain(parameter)) ? (scan_errors.concat(mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("destructuring parameters are not supported on extern functions", 62), error_span, diagnostic_codes::diagnostic_code_e074())})) : (scan_errors)); });
}
return [&]() {
auto empty = mlc::Array<ast::Diagnostic>{};
return empty;
}();
std::abort();
}();
}
if (std::holds_alternative<ast::DeclExported>((*__match_subject))) {
const ast::DeclExported& declExported = std::get<ast::DeclExported>((*__match_subject));
auto [inner_declaration] = declExported; return extern_parameter_destructure_errors_for_decl_shared(inner_declaration);
}
if (std::holds_alternative<ast::DeclExtend>((*__match_subject))) {
const ast::DeclExtend& declExtend = std::get<ast::DeclExtend>((*__match_subject));
auto [__0, __1, methods, __3] = declExtend; return [&]() {
auto concatenated_errors = mlc::Array<ast::Diagnostic>{};
auto method_scan_index = 0;
while ((method_scan_index < methods.length())) {
  (concatenated_errors = concatenate_diagnostic_arrays(concatenated_errors, extern_parameter_destructure_errors_for_decl_shared(methods[method_scan_index])));
  (method_scan_index = mlc::arith::checked_add(method_scan_index, 1));
}
return concatenated_errors;
}();
}
return [&]() {
auto empty = mlc::Array<ast::Diagnostic>{};
return empty;
}();
std::abort();
}();
}
mlc::Array<ast::Diagnostic> extern_parameter_destructure_diagnostics(ast::Program program) noexcept{
  auto all_errors = mlc::Array<ast::Diagnostic>{};
  auto declaration_index = 0;
  while ((declaration_index < program.decls.length()))   {
    (all_errors = concatenate_diagnostic_arrays(all_errors, extern_parameter_destructure_errors_for_decl_shared(program.decls[declaration_index])));
    (declaration_index = mlc::arith::checked_add(declaration_index, 1));
  }
  return all_errors;
}

} // namespace param_destructure_expand
