#include "param_destructure_expand.hpp"

#include "ast.hpp"
#include "diagnostic_codes.hpp"

namespace param_destructure_expand {

using namespace ast;
using namespace diagnostic_codes;
using namespace ast_tokens;

struct Expand_fn_parameter_destructure_fold_state {mlc::Array<std::shared_ptr<ast::Stmt>> prelude_statements;mlc::Array<std::shared_ptr<ast::Param>> new_parameters;int parameter_position_index;};

mlc::String synthetic_destructure_parameter_name(int parameter_position_index) noexcept;

bool parameter_binding_is_plain_identifier(std::shared_ptr<ast::Param> parameter) noexcept;

bool parameter_pattern_is_plain(std::shared_ptr<ast::Param> parameter) noexcept;

std::shared_ptr<ast::Param> strip_parameter_to_synthetic_plain(std::shared_ptr<ast::Param> parameter, mlc::String synthetic_name) noexcept;

std::shared_ptr<ast::Stmt> prelude_statement_destructure(std::shared_ptr<ast::Param> parameter, mlc::String synthetic_name, ast::Span binding_span) noexcept;

std::shared_ptr<ast::Expr> wrap_expression_with_statement_prelude(mlc::Array<std::shared_ptr<ast::Stmt>> prelude_statements, std::shared_ptr<ast::Expr> body_expression) noexcept;

param_destructure_expand::Expand_fn_parameter_destructure_fold_state expand_decl_fn_parameter_destructure_fold_step(param_destructure_expand::Expand_fn_parameter_destructure_fold_state state, std::shared_ptr<ast::Param> current_parameter) noexcept;

std::shared_ptr<ast::Decl> expand_decl_fn_parameter_destructuring_inner(mlc::String function_name, mlc::Array<mlc::String> type_parameter_names, mlc::Array<mlc::Array<mlc::String>> trait_bounds_rows, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<ast::TypeExpr> return_type_expression, std::shared_ptr<ast::Expr> body_expression, mlc::Array<ast::WhereClauseBound> where_clause_bounds_entries) noexcept;

std::shared_ptr<ast::Decl> expand_parameter_destructuring_decl_shared(std::shared_ptr<ast::Decl> declaration) noexcept;

mlc::Array<std::shared_ptr<ast::Decl>> expand_extend_methods_parameter_destructuring(mlc::Array<std::shared_ptr<ast::Decl>> methods) noexcept;

ast::Program expand_parameter_destructuring_in_program(ast::Program program) noexcept;

mlc::Array<ast::Diagnostic> concatenate_diagnostic_arrays(mlc::Array<ast::Diagnostic> existing, mlc::Array<ast::Diagnostic> extra) noexcept;

mlc::Array<ast::Diagnostic> extern_parameter_destructure_errors_for_decl_shared(std::shared_ptr<ast::Decl> declaration) noexcept;

mlc::Array<ast::Diagnostic> extern_parameter_destructure_diagnostics(ast::Program program) noexcept;

mlc::String synthetic_destructure_parameter_name(int parameter_position_index) noexcept{return mlc::String("__mlc_ds") + mlc::to_string(parameter_position_index);}

bool parameter_binding_is_plain_identifier(std::shared_ptr<ast::Param> parameter) noexcept{return [&]() { if (std::holds_alternative<ast::PatternUnit>((*parameter->param_pattern))) { auto _v_patternunit = std::get<ast::PatternUnit>((*parameter->param_pattern)); auto [_w0] = _v_patternunit; return true; } return false; }();}

bool parameter_pattern_is_plain(std::shared_ptr<ast::Param> parameter) noexcept{return parameter_binding_is_plain_identifier(parameter);}

std::shared_ptr<ast::Param> strip_parameter_to_synthetic_plain(std::shared_ptr<ast::Param> parameter, mlc::String synthetic_name) noexcept{return std::make_shared<ast::Param>(ast::Param{synthetic_name, false, parameter->type_value, parameter->has_default, parameter->default_, std::make_shared<ast::Pattern>(ast::PatternUnit(ast::pattern_span(parameter->param_pattern)))});}

std::shared_ptr<ast::Stmt> prelude_statement_destructure(std::shared_ptr<ast::Param> parameter, mlc::String synthetic_name, ast::Span binding_span) noexcept{return std::make_shared<ast::Stmt>(ast::StmtLetPattern(parameter->param_pattern, parameter->is_mut, parameter->type_value, std::make_shared<ast::Expr>(ast::ExprIdent(synthetic_name, binding_span)), false, std::make_shared<ast::Expr>(ast::ExprBlock({}, std::make_shared<ast::Expr>(ast::ExprUnit(binding_span)), binding_span)), binding_span));}

std::shared_ptr<ast::Expr> wrap_expression_with_statement_prelude(mlc::Array<std::shared_ptr<ast::Stmt>> prelude_statements, std::shared_ptr<ast::Expr> body_expression) noexcept{return [&]() -> std::shared_ptr<ast::Expr> { if (std::holds_alternative<ast::ExprBlock>((*body_expression)._)) { auto _v_exprblock = std::get<ast::ExprBlock>((*body_expression)._); auto [inner_statements, result_expression, block_span] = _v_exprblock; return std::make_shared<ast::Expr>(ast::ExprBlock(prelude_statements.concat(inner_statements), result_expression, block_span)); } return [&]() -> std::shared_ptr<ast::Expr> { 
  ast::Span outer_span = ast::expr_span(body_expression);
  return std::make_shared<ast::Expr>(ast::ExprBlock(prelude_statements, body_expression, outer_span));
 }(); }();}

param_destructure_expand::Expand_fn_parameter_destructure_fold_state expand_decl_fn_parameter_destructure_fold_step(param_destructure_expand::Expand_fn_parameter_destructure_fold_state state, std::shared_ptr<ast::Param> current_parameter) noexcept{return parameter_pattern_is_plain(current_parameter) ? param_destructure_expand::Expand_fn_parameter_destructure_fold_state{state.prelude_statements, state.new_parameters.concat(mlc::Array<std::shared_ptr<ast::Param>>{current_parameter}), state.parameter_position_index + 1} : [&]() -> param_destructure_expand::Expand_fn_parameter_destructure_fold_state { 
  mlc::String synthetic_name = synthetic_destructure_parameter_name(state.parameter_position_index);
  ast::Span binding_span = ast::pattern_span(current_parameter->param_pattern);
  return param_destructure_expand::Expand_fn_parameter_destructure_fold_state{state.prelude_statements.concat(mlc::Array<std::shared_ptr<ast::Stmt>>{prelude_statement_destructure(current_parameter, synthetic_name, binding_span)}), state.new_parameters.concat(mlc::Array<std::shared_ptr<ast::Param>>{strip_parameter_to_synthetic_plain(current_parameter, synthetic_name)}), state.parameter_position_index + 1};
 }();}

std::shared_ptr<ast::Decl> expand_decl_fn_parameter_destructuring_inner(mlc::String function_name, mlc::Array<mlc::String> type_parameter_names, mlc::Array<mlc::Array<mlc::String>> trait_bounds_rows, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<ast::TypeExpr> return_type_expression, std::shared_ptr<ast::Expr> body_expression, mlc::Array<ast::WhereClauseBound> where_clause_bounds_entries) noexcept{
param_destructure_expand::Expand_fn_parameter_destructure_fold_state fold_result = parameters.fold(param_destructure_expand::Expand_fn_parameter_destructure_fold_state{[&]() -> mlc::Array<std::shared_ptr<ast::Stmt>> { 
  mlc::Array<std::shared_ptr<ast::Stmt>> empty_prelude = {};
  return empty_prelude;
 }(), [&]() -> mlc::Array<std::shared_ptr<ast::Param>> { 
  mlc::Array<std::shared_ptr<ast::Param>> empty_parameters = {};
  return empty_parameters;
 }(), 0}, [](param_destructure_expand::Expand_fn_parameter_destructure_fold_state state, std::shared_ptr<ast::Param> current_parameter) mutable { return expand_decl_fn_parameter_destructure_fold_step(state, current_parameter); });
std::shared_ptr<ast::Expr> expanded_body = fold_result.prelude_statements.size() > 0 ? wrap_expression_with_statement_prelude(fold_result.prelude_statements, body_expression) : body_expression;
return std::make_shared<ast::Decl>(ast::DeclFn(function_name, type_parameter_names, trait_bounds_rows, fold_result.new_parameters, return_type_expression, expanded_body, where_clause_bounds_entries));
}

std::shared_ptr<ast::Decl> expand_parameter_destructuring_decl_shared(std::shared_ptr<ast::Decl> declaration) noexcept{return [&]() -> std::shared_ptr<ast::Decl> { if (std::holds_alternative<ast::DeclFn>((*declaration))) { auto _v_declfn = std::get<ast::DeclFn>((*declaration)); auto [function_name, type_parameter_names, trait_bounds_rows, parameters, return_type_expression, body_expression, where_clause_bounds_entries] = _v_declfn; return expand_decl_fn_parameter_destructuring_inner(function_name, type_parameter_names, trait_bounds_rows, parameters, return_type_expression, body_expression, where_clause_bounds_entries); } if (std::holds_alternative<ast::DeclExported>((*declaration))) { auto _v_declexported = std::get<ast::DeclExported>((*declaration)); auto [inner_declaration] = _v_declexported; return std::make_shared<ast::Decl>(ast::DeclExported(expand_parameter_destructuring_decl_shared(inner_declaration))); } if (std::holds_alternative<ast::DeclExtend>((*declaration))) { auto _v_declextend = std::get<ast::DeclExtend>((*declaration)); auto [extended_type_name, implemented_trait_name, methods, name_span] = _v_declextend; return std::make_shared<ast::Decl>(ast::DeclExtend(extended_type_name, implemented_trait_name, expand_extend_methods_parameter_destructuring(methods), name_span)); } return declaration; }();}

mlc::Array<std::shared_ptr<ast::Decl>> expand_extend_methods_parameter_destructuring(mlc::Array<std::shared_ptr<ast::Decl>> methods) noexcept{return methods.fold([&]() -> mlc::Array<std::shared_ptr<ast::Decl>> { 
  mlc::Array<std::shared_ptr<ast::Decl>> empty_methods = {};
  return empty_methods;
 }(), [](mlc::Array<std::shared_ptr<ast::Decl>> output_methods, std::shared_ptr<ast::Decl> method) mutable { return output_methods.concat(mlc::Array<std::shared_ptr<ast::Decl>>{expand_parameter_destructuring_decl_shared(method)}); });}

ast::Program expand_parameter_destructuring_in_program(ast::Program program) noexcept{return ast::Program{program.decls.fold([&]() -> mlc::Array<std::shared_ptr<ast::Decl>> { 
  mlc::Array<std::shared_ptr<ast::Decl>> empty_declarations = {};
  return empty_declarations;
 }(), [](mlc::Array<std::shared_ptr<ast::Decl>> output_declarations, std::shared_ptr<ast::Decl> declaration) mutable { return output_declarations.concat(mlc::Array<std::shared_ptr<ast::Decl>>{expand_parameter_destructuring_decl_shared(declaration)}); })};}

mlc::Array<ast::Diagnostic> concatenate_diagnostic_arrays(mlc::Array<ast::Diagnostic> existing, mlc::Array<ast::Diagnostic> extra) noexcept{return existing.concat(extra);}

mlc::Array<ast::Diagnostic> extern_parameter_destructure_errors_for_decl_shared(std::shared_ptr<ast::Decl> declaration) noexcept{return [&]() -> mlc::Array<ast::Diagnostic> { if (std::holds_alternative<ast::DeclFn>((*declaration))) { auto _v_declfn = std::get<ast::DeclFn>((*declaration)); auto [_w0, _w1, _w2, parameters, _w3, body_expression, _w4] = _v_declfn; return [&]() -> mlc::Array<ast::Diagnostic> { if (std::holds_alternative<ast::ExprExtern>((*body_expression)._)) { auto _v_exprextern = std::get<ast::ExprExtern>((*body_expression)._); auto [error_span] = _v_exprextern; return parameters.fold([&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> empty_diagnostics = {};
  return empty_diagnostics;
 }(), [error_span](mlc::Array<ast::Diagnostic> scan_errors, std::shared_ptr<ast::Param> parameter) mutable { return !parameter_pattern_is_plain(parameter) ? scan_errors.concat(mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("destructuring parameters are not supported on extern functions"), error_span, diagnostic_codes::diagnostic_code_e074())}) : scan_errors; }); } return [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> empty = {};
  return empty;
 }(); }(); } if (std::holds_alternative<ast::DeclExported>((*declaration))) { auto _v_declexported = std::get<ast::DeclExported>((*declaration)); auto [inner_declaration] = _v_declexported; return extern_parameter_destructure_errors_for_decl_shared(inner_declaration); } if (std::holds_alternative<ast::DeclExtend>((*declaration))) { auto _v_declextend = std::get<ast::DeclExtend>((*declaration)); auto [_w0, _w1, methods, _w2] = _v_declextend; return [&]() -> mlc::Array<ast::Diagnostic> { 
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
