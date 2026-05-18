#define main mlc_user_main
#include "param_destructure_expand.hpp"

namespace param_destructure_expand {

mlc::String synthetic_destructure_parameter_name(int parameter_position_index) noexcept{
return (mlc::String("__mlc_ds", 8) + mlc::to_string(parameter_position_index));
}
bool parameter_binding_is_plain_identifier(std::shared_ptr<ast::Param> parameter) noexcept{
return std::visit(overloaded{[&](const ast::PatUnit& patUnit) { auto [__0] = patUnit; return true; },
[&](const auto& __v) { return false; }
}, (*parameter->param_pattern));
}
bool parameter_pattern_is_plain(std::shared_ptr<ast::Param> parameter) noexcept{
return parameter_binding_is_plain_identifier(parameter);
}
std::shared_ptr<ast::Param> strip_parameter_to_synthetic_plain(std::shared_ptr<ast::Param> parameter, mlc::String synthetic_name) noexcept{
return std::make_shared<ast::Param>(ast::Param{synthetic_name, false, parameter->typ, parameter->has_default, parameter->default_, std::make_shared<ast::Pat>(ast::PatUnit{ast::span_unknown()})});
}
std::shared_ptr<ast::Stmt> prelude_statement_destructure(std::shared_ptr<ast::Param> parameter, mlc::String synthetic_name, ast::Span binding_span) noexcept{
return std::make_shared<ast::Stmt>(ast::StmtLetPat{parameter->param_pattern, parameter->is_mut, parameter->typ, std::make_shared<ast::Expr>(ast::ExprIdent{synthetic_name, binding_span}), false, std::make_shared<ast::Expr>(ast::ExprBlock{{}, std::make_shared<ast::Expr>(ast::ExprUnit{binding_span}), binding_span}), binding_span});
}
std::shared_ptr<ast::Expr> wrap_expression_with_statement_prelude(mlc::Array<std::shared_ptr<ast::Stmt>> prelude_statements, std::shared_ptr<ast::Expr> body_expression) noexcept{
return std::visit(overloaded{[&](const ast::ExprBlock& exprBlock) { auto [inner_statements, result_expression, block_span] = exprBlock; return [&]() {
auto combined = mlc::Array<std::shared_ptr<ast::Stmt>>{};
auto prelude_index = 0;
while ((prelude_index < prelude_statements.length())) {
combined.push_back(prelude_statements[prelude_index]);
prelude_index = (prelude_index + 1);
}
auto inner_index = 0;
while ((inner_index < inner_statements.length())) {
combined.push_back(inner_statements[inner_index]);
inner_index = (inner_index + 1);
}
return std::make_shared<ast::Expr>(ast::ExprBlock{combined, result_expression, block_span});
}(); },
[&](const auto& __v) { return [&]() {
auto outer_span = ast::expr_span(body_expression);
return std::make_shared<ast::Expr>(ast::ExprBlock{prelude_statements, body_expression, outer_span});
}(); }
}, (*body_expression));
}
std::shared_ptr<ast::Decl> expand_decl_fn_parameter_destructuring_inner(mlc::String function_name, mlc::Array<mlc::String> type_parameter_names, mlc::Array<mlc::Array<mlc::String>> trait_bounds_rows, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<ast::TypeExpr> return_type_expression, std::shared_ptr<ast::Expr> body_expression, mlc::Array<ast::WhereClauseBound> where_clause_bounds_entries) noexcept{
auto prelude_statements = mlc::Array<std::shared_ptr<ast::Stmt>>{};
auto new_parameters = mlc::Array<std::shared_ptr<ast::Param>>{};
auto parameter_position_index = 0;
while ((parameter_position_index < parameters.length())) {
auto current_parameter = parameters[parameter_position_index];
if (parameter_pattern_is_plain(current_parameter)) {
new_parameters.push_back(current_parameter);
} else {
auto synthetic_name = synthetic_destructure_parameter_name(parameter_position_index);
auto binding_span = ast::pat_span(current_parameter->param_pattern);
prelude_statements.push_back(prelude_statement_destructure(current_parameter, synthetic_name, binding_span));
new_parameters.push_back(strip_parameter_to_synthetic_plain(current_parameter, synthetic_name));
}
parameter_position_index = (parameter_position_index + 1);
}
auto expanded_body = ((prelude_statements.length() > 0) ? (wrap_expression_with_statement_prelude(prelude_statements, body_expression)) : (body_expression));
return std::make_shared<ast::Decl>(ast::DeclFn{function_name, type_parameter_names, trait_bounds_rows, new_parameters, return_type_expression, expanded_body, where_clause_bounds_entries});
}
std::shared_ptr<ast::Decl> expand_parameter_destructuring_decl_shared(std::shared_ptr<ast::Decl> declaration) noexcept{
return std::visit(overloaded{[&](const ast::DeclFn& declFn) { auto [function_name, type_parameter_names, trait_bounds_rows, parameters, return_type_expression, body_expression, where_clause_bounds_entries] = declFn; return expand_decl_fn_parameter_destructuring_inner(function_name, type_parameter_names, trait_bounds_rows, parameters, return_type_expression, body_expression, where_clause_bounds_entries); },
[&](const ast::DeclExported& declExported) { auto [inner_declaration] = declExported; return std::make_shared<ast::Decl>(ast::DeclExported{expand_parameter_destructuring_decl_shared(inner_declaration)}); },
[&](const ast::DeclExtend& declExtend) { auto [extended_type_name, implemented_trait_name, methods] = declExtend; return std::make_shared<ast::Decl>(ast::DeclExtend{extended_type_name, implemented_trait_name, expand_extend_methods_parameter_destructuring(methods)}); },
[&](const auto& __v) { return declaration; }
}, (*declaration));
}
mlc::Array<std::shared_ptr<ast::Decl>> expand_extend_methods_parameter_destructuring(mlc::Array<std::shared_ptr<ast::Decl>> methods) noexcept{
auto output_methods = mlc::Array<std::shared_ptr<ast::Decl>>{};
auto method_index = 0;
while ((method_index < methods.length())) {
output_methods.push_back(expand_parameter_destructuring_decl_shared(methods[method_index]));
method_index = (method_index + 1);
}
return output_methods;
}
ast::Program expand_parameter_destructuring_in_program(ast::Program program) noexcept{
auto output_declarations = mlc::Array<std::shared_ptr<ast::Decl>>{};
auto declaration_index = 0;
while ((declaration_index < program.decls.length())) {
output_declarations.push_back(expand_parameter_destructuring_decl_shared(program.decls[declaration_index]));
declaration_index = (declaration_index + 1);
}
return ast::Program{output_declarations};
}
mlc::Array<ast::Diagnostic> concatenate_diagnostic_arrays(mlc::Array<ast::Diagnostic> existing, mlc::Array<ast::Diagnostic> extra) noexcept{
auto combined = existing;
auto extra_index = 0;
while ((extra_index < extra.length())) {
combined.push_back(extra[extra_index]);
extra_index = (extra_index + 1);
}
return combined;
}
mlc::Array<ast::Diagnostic> extern_parameter_destructure_errors_for_decl_shared(std::shared_ptr<ast::Decl> declaration) noexcept{
return std::visit(overloaded{[&](const ast::DeclFn& declFn) { auto [__0, __1, __2, parameters, __4, body_expression, __6] = declFn; return std::visit(overloaded{[&](const ast::ExprExtern& exprExtern) { auto [error_span] = exprExtern; return [&]() {
auto scan_errors = mlc::Array<ast::Diagnostic>{};
auto parameter_scan_index = 0;
while ((parameter_scan_index < parameters.length())) {
if ((!parameter_pattern_is_plain(parameters[parameter_scan_index]))) {
scan_errors.push_back(ast::diagnostic_error(mlc::String("destructuring parameters are not supported on extern functions", 62), error_span));
}
parameter_scan_index = (parameter_scan_index + 1);
}
return scan_errors;
}(); },
[&](const auto& __v) { return {}; }
}, (*body_expression)); },
[&](const ast::DeclExported& declExported) { auto [inner_declaration] = declExported; return extern_parameter_destructure_errors_for_decl_shared(inner_declaration); },
[&](const ast::DeclExtend& declExtend) { auto [__0, __1, methods] = declExtend; return [&]() {
auto concatenated_errors = mlc::Array<ast::Diagnostic>{};
auto method_scan_index = 0;
while ((method_scan_index < methods.length())) {
concatenated_errors = concatenate_diagnostic_arrays(concatenated_errors, extern_parameter_destructure_errors_for_decl_shared(methods[method_scan_index]));
method_scan_index = (method_scan_index + 1);
}
return concatenated_errors;
}(); },
[&](const auto& __v) { return {}; }
}, (*declaration));
}
mlc::Array<ast::Diagnostic> extern_parameter_destructure_diagnostics(ast::Program program) noexcept{
auto all_errors = mlc::Array<ast::Diagnostic>{};
auto declaration_index = 0;
while ((declaration_index < program.decls.length())) {
all_errors = concatenate_diagnostic_arrays(all_errors, extern_parameter_destructure_errors_for_decl_shared(program.decls[declaration_index]));
declaration_index = (declaration_index + 1);
}
return all_errors;
}

} // namespace param_destructure_expand
