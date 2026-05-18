#include "param_destructure_expand.hpp"

#include "ast.hpp"

namespace param_destructure_expand {

using namespace ast;
using namespace ast_tokens;

mlc::String synthetic_destructure_parameter_name(int parameter_position_index) noexcept;

bool parameter_binding_is_plain_identifier(std::shared_ptr<ast::Param> parameter) noexcept;

bool parameter_pattern_is_plain(std::shared_ptr<ast::Param> parameter) noexcept;

std::shared_ptr<ast::Param> strip_parameter_to_synthetic_plain(std::shared_ptr<ast::Param> parameter, mlc::String synthetic_name) noexcept;

std::shared_ptr<ast::Stmt> prelude_statement_destructure(std::shared_ptr<ast::Param> parameter, mlc::String synthetic_name, ast::Span binding_span) noexcept;

std::shared_ptr<ast::Expr> wrap_expression_with_statement_prelude(mlc::Array<std::shared_ptr<ast::Stmt>> prelude_statements, std::shared_ptr<ast::Expr> body_expression) noexcept;

std::shared_ptr<ast::Decl> expand_decl_fn_parameter_destructuring_inner(mlc::String function_name, mlc::Array<mlc::String> type_parameter_names, mlc::Array<mlc::Array<mlc::String>> trait_bounds_rows, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<ast::TypeExpr> return_type_expression, std::shared_ptr<ast::Expr> body_expression, mlc::Array<ast::WhereClauseBound> where_clause_bounds_entries) noexcept;

std::shared_ptr<ast::Decl> expand_parameter_destructuring_decl_shared(std::shared_ptr<ast::Decl> declaration) noexcept;

mlc::Array<std::shared_ptr<ast::Decl>> expand_extend_methods_parameter_destructuring(mlc::Array<std::shared_ptr<ast::Decl>> methods) noexcept;

ast::Program expand_parameter_destructuring_in_program(ast::Program program) noexcept;

mlc::Array<ast::Diagnostic> concatenate_diagnostic_arrays(mlc::Array<ast::Diagnostic> existing, mlc::Array<ast::Diagnostic> extra) noexcept;

mlc::Array<ast::Diagnostic> extern_parameter_destructure_errors_for_decl_shared(std::shared_ptr<ast::Decl> declaration) noexcept;

mlc::Array<ast::Diagnostic> extern_parameter_destructure_diagnostics(ast::Program program) noexcept;

mlc::String synthetic_destructure_parameter_name(int parameter_position_index) noexcept{return mlc::String("__mlc_ds") + mlc::to_string(parameter_position_index);}

bool parameter_binding_is_plain_identifier(std::shared_ptr<ast::Param> parameter) noexcept{return [&]() { if (std::holds_alternative<ast::PatUnit>((*parameter->param_pattern))) { auto _v_patunit = std::get<ast::PatUnit>((*parameter->param_pattern)); auto [_w0] = _v_patunit; return true; } return false; }();}

bool parameter_pattern_is_plain(std::shared_ptr<ast::Param> parameter) noexcept{return parameter_binding_is_plain_identifier(parameter);}

std::shared_ptr<ast::Param> strip_parameter_to_synthetic_plain(std::shared_ptr<ast::Param> parameter, mlc::String synthetic_name) noexcept{return std::make_shared<ast::Param>(ast::Param{synthetic_name, false, parameter->typ, parameter->has_default, parameter->default_, std::make_shared<ast::Pat>(ast::PatUnit(ast::span_unknown()))});}

std::shared_ptr<ast::Stmt> prelude_statement_destructure(std::shared_ptr<ast::Param> parameter, mlc::String synthetic_name, ast::Span binding_span) noexcept{return std::make_shared<ast::Stmt>(ast::StmtLetPat(parameter->param_pattern, parameter->is_mut, parameter->typ, std::make_shared<ast::Expr>(ast::ExprIdent(synthetic_name, binding_span)), false, std::make_shared<ast::Expr>(ast::ExprBlock({}, std::make_shared<ast::Expr>(ast::ExprUnit(binding_span)), binding_span)), binding_span));}

std::shared_ptr<ast::Expr> wrap_expression_with_statement_prelude(mlc::Array<std::shared_ptr<ast::Stmt>> prelude_statements, std::shared_ptr<ast::Expr> body_expression) noexcept{
return [&]() -> std::shared_ptr<ast::Expr> { if (std::holds_alternative<ast::ExprBlock>((*body_expression)._)) { auto _v_exprblock = std::get<ast::ExprBlock>((*body_expression)._); auto [inner_statements, result_expression, block_span] = _v_exprblock; return [&]() -> std::shared_ptr<ast::Expr> { 
  mlc::Array<std::shared_ptr<ast::Stmt>> combined = {};
  int prelude_index = 0;
  while (prelude_index < prelude_statements.size()){
{
combined.push_back(prelude_statements[prelude_index]);
prelude_index = prelude_index + 1;
}
}
  int inner_index = 0;
  while (inner_index < inner_statements.size()){
{
combined.push_back(inner_statements[inner_index]);
inner_index = inner_index + 1;
}
}
  return std::make_shared<ast::Expr>(ast::ExprBlock(combined, result_expression, block_span));
 }(); } return [&]() -> std::shared_ptr<ast::Expr> { 
  ast::Span outer_span = ast::expr_span(body_expression);
  return std::make_shared<ast::Expr>(ast::ExprBlock(prelude_statements, body_expression, outer_span));
 }(); }();
}

std::shared_ptr<ast::Decl> expand_decl_fn_parameter_destructuring_inner(mlc::String function_name, mlc::Array<mlc::String> type_parameter_names, mlc::Array<mlc::Array<mlc::String>> trait_bounds_rows, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<ast::TypeExpr> return_type_expression, std::shared_ptr<ast::Expr> body_expression, mlc::Array<ast::WhereClauseBound> where_clause_bounds_entries) noexcept{
mlc::Array<std::shared_ptr<ast::Stmt>> prelude_statements = {};
mlc::Array<std::shared_ptr<ast::Param>> new_parameters = {};
int parameter_position_index = 0;
while (parameter_position_index < parameters.size()){
{
std::shared_ptr<ast::Param> current_parameter = parameters[parameter_position_index];
if (parameter_pattern_is_plain(current_parameter)){
{
new_parameters.push_back(current_parameter);
}
} else {
{
mlc::String synthetic_name = synthetic_destructure_parameter_name(parameter_position_index);
ast::Span binding_span = ast::pat_span(current_parameter->param_pattern);
prelude_statements.push_back(prelude_statement_destructure(current_parameter, synthetic_name, binding_span));
new_parameters.push_back(strip_parameter_to_synthetic_plain(current_parameter, synthetic_name));
}
}
parameter_position_index = parameter_position_index + 1;
}
}
std::shared_ptr<ast::Expr> expanded_body = prelude_statements.size() > 0 ? wrap_expression_with_statement_prelude(prelude_statements, body_expression) : body_expression;
return std::make_shared<ast::Decl>(ast::DeclFn(function_name, type_parameter_names, trait_bounds_rows, new_parameters, return_type_expression, expanded_body, where_clause_bounds_entries));
}

std::shared_ptr<ast::Decl> expand_parameter_destructuring_decl_shared(std::shared_ptr<ast::Decl> declaration) noexcept{return [&]() -> std::shared_ptr<ast::Decl> { if (std::holds_alternative<ast::DeclFn>((*declaration))) { auto _v_declfn = std::get<ast::DeclFn>((*declaration)); auto [function_name, type_parameter_names, trait_bounds_rows, parameters, return_type_expression, body_expression, where_clause_bounds_entries] = _v_declfn; return expand_decl_fn_parameter_destructuring_inner(function_name, type_parameter_names, trait_bounds_rows, parameters, return_type_expression, body_expression, where_clause_bounds_entries); } if (std::holds_alternative<ast::DeclExported>((*declaration))) { auto _v_declexported = std::get<ast::DeclExported>((*declaration)); auto [inner_declaration] = _v_declexported; return std::make_shared<ast::Decl>(ast::DeclExported(expand_parameter_destructuring_decl_shared(inner_declaration))); } if (std::holds_alternative<ast::DeclExtend>((*declaration))) { auto _v_declextend = std::get<ast::DeclExtend>((*declaration)); auto [extended_type_name, implemented_trait_name, methods] = _v_declextend; return std::make_shared<ast::Decl>(ast::DeclExtend(extended_type_name, implemented_trait_name, expand_extend_methods_parameter_destructuring(methods))); } return declaration; }();}

mlc::Array<std::shared_ptr<ast::Decl>> expand_extend_methods_parameter_destructuring(mlc::Array<std::shared_ptr<ast::Decl>> methods) noexcept{
mlc::Array<std::shared_ptr<ast::Decl>> output_methods = {};
int method_index = 0;
while (method_index < methods.size()){
{
output_methods.push_back(expand_parameter_destructuring_decl_shared(methods[method_index]));
method_index = method_index + 1;
}
}
return output_methods;
}

ast::Program expand_parameter_destructuring_in_program(ast::Program program) noexcept{
mlc::Array<std::shared_ptr<ast::Decl>> output_declarations = {};
int declaration_index = 0;
while (declaration_index < program.decls.size()){
{
output_declarations.push_back(expand_parameter_destructuring_decl_shared(program.decls[declaration_index]));
declaration_index = declaration_index + 1;
}
}
return ast::Program{output_declarations};
}

mlc::Array<ast::Diagnostic> concatenate_diagnostic_arrays(mlc::Array<ast::Diagnostic> existing, mlc::Array<ast::Diagnostic> extra) noexcept{
mlc::Array<ast::Diagnostic> combined = existing;
int extra_index = 0;
while (extra_index < extra.size()){
{
combined.push_back(extra[extra_index]);
extra_index = extra_index + 1;
}
}
return combined;
}

mlc::Array<ast::Diagnostic> extern_parameter_destructure_errors_for_decl_shared(std::shared_ptr<ast::Decl> declaration) noexcept{return [&]() -> mlc::Array<ast::Diagnostic> { if (std::holds_alternative<ast::DeclFn>((*declaration))) { auto _v_declfn = std::get<ast::DeclFn>((*declaration)); auto [_w0, _w1, _w2, parameters, _w3, body_expression, _w4] = _v_declfn; return [&]() -> mlc::Array<ast::Diagnostic> { if (std::holds_alternative<ast::ExprExtern>((*body_expression)._)) { auto _v_exprextern = std::get<ast::ExprExtern>((*body_expression)._); auto [error_span] = _v_exprextern; return [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> scan_errors = {};
  int parameter_scan_index = 0;
  while (parameter_scan_index < parameters.size()){
{
if (!parameter_pattern_is_plain(parameters[parameter_scan_index])){
{
scan_errors.push_back(ast::diagnostic_error(mlc::String("destructuring parameters are not supported on extern functions"), error_span));
}
}
parameter_scan_index = parameter_scan_index + 1;
}
}
  return scan_errors;
 }(); } return [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> empty = {};
  return empty;
 }(); }(); } if (std::holds_alternative<ast::DeclExported>((*declaration))) { auto _v_declexported = std::get<ast::DeclExported>((*declaration)); auto [inner_declaration] = _v_declexported; return extern_parameter_destructure_errors_for_decl_shared(inner_declaration); } if (std::holds_alternative<ast::DeclExtend>((*declaration))) { auto _v_declextend = std::get<ast::DeclExtend>((*declaration)); auto [_w0, _w1, methods] = _v_declextend; return [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> concatenated_errors = {};
  int method_scan_index = 0;
  while (method_scan_index < methods.size()){
{
concatenated_errors = concatenate_diagnostic_arrays(concatenated_errors, extern_parameter_destructure_errors_for_decl_shared(methods[method_scan_index]));
method_scan_index = method_scan_index + 1;
}
}
  return concatenated_errors;
 }(); } return [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> empty = {};
  return empty;
 }(); }();}

mlc::Array<ast::Diagnostic> extern_parameter_destructure_diagnostics(ast::Program program) noexcept{
mlc::Array<ast::Diagnostic> all_errors = {};
int declaration_index = 0;
while (declaration_index < program.decls.size()){
{
all_errors = concatenate_diagnostic_arrays(all_errors, extern_parameter_destructure_errors_for_decl_shared(program.decls[declaration_index]));
declaration_index = declaration_index + 1;
}
}
return all_errors;
}

} // namespace param_destructure_expand
