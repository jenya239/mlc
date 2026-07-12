#define main mlc_user_main
#include "extern_header_arity_lint.hpp"

#include "ast.hpp"
#include "diagnostic_codes.hpp"

namespace extern_header_arity_lint {

using namespace ast;
using namespace diagnostic_codes;
using namespace ast_tokens;

mlc::String header_import_extern_marker() noexcept{
  return mlc::String("__mlc_header_import__", 21);
}
mlc::Array<ast::Diagnostic> empty_diagnostic_list() noexcept{
  auto empty = mlc::Array<ast::Diagnostic>{};
  return empty;
}
mlc::String arity_mismatch_message(mlc::String c_name, int user_arity, int header_arity) noexcept{
  return (((((mlc::String("extern fn C symbol '", 20) + c_name) + mlc::String("' arity ", 8)) + mlc::to_string(user_arity)) + mlc::String(" does not match imported header arity ", 38)) + mlc::to_string(header_arity));
}
mlc::Array<ast::Diagnostic> warning_for_arity_mismatch(mlc::String c_name, int user_arity, int header_arity, ast::Span source_span) noexcept{
  return mlc::Array<ast::Diagnostic>{ast::diagnostic_new_with_code(mlc::String("warning", 7), arity_mismatch_message(c_name, user_arity, header_arity), source_span, diagnostic_codes::diagnostic_code_w_extern_arity())};
}
mlc::HashMap<mlc::String, int> maybe_record_stub_from_extern_body(mlc::HashMap<mlc::String, int> arity_by_c_name, int parameters_length, std::shared_ptr<ast::Expr> body) noexcept{
  return [&]() -> mlc::HashMap<mlc::String, int> {
auto __match_subject = body;
if (std::holds_alternative<ast::ExprExtern>((*__match_subject))) {
const ast::ExprExtern& exprExtern = std::get<ast::ExprExtern>((*__match_subject));
auto [c_name, header, attributes, source_span] = exprExtern; return [&]() {
auto unused_attributes = attributes;
auto unused_span = source_span;
if ((header == header_import_extern_marker())) {
  if ((c_name.length() > 0))   {
    arity_by_c_name.set(c_name, parameters_length);
  }
}
return arity_by_c_name;
}();
}
return arity_by_c_name;
std::abort();
}();
}
mlc::HashMap<mlc::String, int> record_header_stub_arity(mlc::HashMap<mlc::String, int> arity_by_c_name, std::shared_ptr<ast::Decl> declaration) noexcept{
  auto result = arity_by_c_name;
  [&]() {
auto __match_subject = ast::decl_inner(declaration);
if (std::holds_alternative<ast::DeclFn>((*__match_subject))) {
const ast::DeclFn& declFn = std::get<ast::DeclFn>((*__match_subject));
auto [name, type_parameters, trait_bounds, parameters, return_type, body, where_clause] = declFn; [&]() {
auto unused_name = name;
auto unused_type_parameters = type_parameters;
auto unused_trait_bounds = trait_bounds;
auto unused_return_type = return_type;
auto unused_where_clause = where_clause;
(result = maybe_record_stub_from_extern_body(result, parameters.length(), body));
return std::make_tuple();
}();
return;
}
if (std::holds_alternative<ast::DeclExported>((*__match_subject))) {
const ast::DeclExported& declExported = std::get<ast::DeclExported>((*__match_subject));
auto [inner] = declExported; [&]() {
(result = record_header_stub_arity(result, inner));
return std::make_tuple();
}();
return;
}
std::make_tuple();
return;
std::abort();
}();
  return result;
}
mlc::HashMap<mlc::String, int> build_header_import_arity_index(ast::Program program) noexcept{
  auto arity_by_c_name = mlc::HashMap<mlc::String, int>();
  auto declaration_index = 0;
  while ((declaration_index < program.decls.length()))   {
    (arity_by_c_name = record_header_stub_arity(arity_by_c_name, program.decls[declaration_index]));
    (declaration_index = mlc::arith::checked_add(declaration_index, 1));
  }
  return arity_by_c_name;
}
mlc::Array<ast::Diagnostic> diagnostics_from_extern_body(int parameters_length, std::shared_ptr<ast::Expr> body, mlc::HashMap<mlc::String, int> arity_by_c_name) noexcept{
  return [&]() -> mlc::Array<ast::Diagnostic> {
auto __match_subject = body;
if (std::holds_alternative<ast::ExprExtern>((*__match_subject))) {
const ast::ExprExtern& exprExtern = std::get<ast::ExprExtern>((*__match_subject));
auto [c_name, header, attributes, source_span] = exprExtern; return [&]() {
auto unused_attributes = attributes;
return ((header.length() == 0) ? (empty_diagnostic_list()) : (((header == header_import_extern_marker()) ? (empty_diagnostic_list()) : (((c_name.length() == 0) ? (empty_diagnostic_list()) : ([&]() -> mlc::Array<ast::Diagnostic> {
  if (arity_by_c_name.has(c_name))   {
    return [&]() {
auto header_arity = arity_by_c_name.get(c_name);
return ((parameters_length == header_arity) ? (empty_diagnostic_list()) : (warning_for_arity_mismatch(c_name, parameters_length, header_arity, source_span)));
}();
  } else   {
    return empty_diagnostic_list();
  }
}()))))));
}();
}
return empty_diagnostic_list();
std::abort();
}();
}
mlc::Array<ast::Diagnostic> diagnostics_for_user_extern_against_header(std::shared_ptr<ast::Decl> declaration, mlc::HashMap<mlc::String, int> arity_by_c_name) noexcept{
  return [&]() -> mlc::Array<ast::Diagnostic> {
auto __match_subject = ast::decl_inner(declaration);
if (std::holds_alternative<ast::DeclFn>((*__match_subject))) {
const ast::DeclFn& declFn = std::get<ast::DeclFn>((*__match_subject));
auto [name, type_parameters, trait_bounds, parameters, return_type, body, where_clause] = declFn; return [&]() {
auto unused_name = name;
auto unused_type_parameters = type_parameters;
auto unused_trait_bounds = trait_bounds;
auto unused_return_type = return_type;
auto unused_where_clause = where_clause;
return diagnostics_from_extern_body(parameters.length(), body, arity_by_c_name);
}();
}
if (std::holds_alternative<ast::DeclExported>((*__match_subject))) {
const ast::DeclExported& declExported = std::get<ast::DeclExported>((*__match_subject));
auto [inner] = declExported; return diagnostics_for_user_extern_against_header(inner, arity_by_c_name);
}
return empty_diagnostic_list();
std::abort();
}();
}
mlc::Array<ast::Diagnostic> extern_header_arity_diagnostics(ast::Program program) noexcept{
  auto arity_by_c_name = build_header_import_arity_index(program);
  auto diagnostics = empty_diagnostic_list();
  auto declaration_index = 0;
  while ((declaration_index < program.decls.length()))   {
    (diagnostics = ast::diagnostics_append(diagnostics, diagnostics_for_user_extern_against_header(program.decls[declaration_index], arity_by_c_name)));
    (declaration_index = mlc::arith::checked_add(declaration_index, 1));
  }
  return diagnostics;
}

} // namespace extern_header_arity_lint
