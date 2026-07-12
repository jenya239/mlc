#define main mlc_user_main
#include "extern_dedup_lint.hpp"

#include "ast.hpp"
#include "diagnostic_codes.hpp"

namespace extern_dedup_lint {

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
mlc::String ffi_binding_key(mlc::String c_name, mlc::String header) noexcept{
  return ((c_name + mlc::String("#", 1)) + header);
}
mlc::String span_site_label(ast::Span source_span) noexcept{
  if ((source_span.file.length() > 0))   {
    return ((((source_span.file + mlc::String(":", 1)) + mlc::to_string(source_span.line)) + mlc::String(":", 1)) + mlc::to_string(source_span.column));
  } else   {
    return (((mlc::String("line ", 5) + mlc::to_string(source_span.line)) + mlc::String(":", 1)) + mlc::to_string(source_span.column));
  }
}
mlc::String encode_binding(int arity, mlc::String site_label) noexcept{
  return ((mlc::to_string(arity) + mlc::String("|", 1)) + site_label);
}
int decode_arity(mlc::String encoded) noexcept{
  auto index = 0;
  while (((index < encoded.length()) && (encoded.substring(index, 1) != mlc::String("|", 1))))   {
    (index = mlc::arith::checked_add(index, 1));
  }
  if ((index == 0))   {
    return 0;
  } else   {
    return encoded.substring(0, index).to_i();
  }
}
mlc::String decode_label(mlc::String encoded) noexcept{
  auto index = 0;
  while (((index < encoded.length()) && (encoded.substring(index, 1) != mlc::String("|", 1))))   {
    (index = mlc::arith::checked_add(index, 1));
  }
  if ((index >= encoded.length()))   {
    return mlc::String("", 0);
  } else   {
    return encoded.substring(mlc::arith::checked_add(index, 1), mlc::arith::checked_sub(mlc::arith::checked_sub(encoded.length(), index), 1));
  }
}
mlc::String mismatch_message(mlc::String c_name, mlc::String header, int later_arity, int earlier_arity, mlc::String earlier_label) noexcept{
  return (((((((((mlc::String("extern fn C symbol '", 20) + c_name) + mlc::String("' from \"", 8)) + header) + mlc::String("\" arity ", 8)) + mlc::to_string(later_arity)) + mlc::String(" conflicts with earlier arity ", 30)) + mlc::to_string(earlier_arity)) + mlc::String(" at ", 4)) + earlier_label);
}
mlc::HashMap<mlc::String, mlc::String> maybe_record_stub_from_extern_body(mlc::HashMap<mlc::String, mlc::String> binding_by_key, int parameters_length, std::shared_ptr<ast::Expr> body) noexcept{
  return [&]() -> mlc::HashMap<mlc::String, mlc::String> {
auto __match_subject = body;
if (std::holds_alternative<ast::ExprExtern>((*__match_subject))) {
const ast::ExprExtern& exprExtern = std::get<ast::ExprExtern>((*__match_subject));
auto [c_name, header, attributes, source_span] = exprExtern; return [&]() {
auto unused_attributes = attributes;
if ((((header.length() > 0) && (header != header_import_extern_marker())) && (c_name.length() > 0))) {
  auto key = ffi_binding_key(c_name, header);
  if ((!binding_by_key.has(key)))   {
    binding_by_key.set(key, encode_binding(parameters_length, span_site_label(source_span)));
  }
}
return binding_by_key;
}();
}
return binding_by_key;
std::abort();
}();
}
mlc::HashMap<mlc::String, mlc::String> record_declaration(mlc::HashMap<mlc::String, mlc::String> binding_by_key, std::shared_ptr<ast::Decl> declaration) noexcept{
  auto result = binding_by_key;
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
(result = record_declaration(result, inner));
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
mlc::Array<ast::Diagnostic> error_for_conflict(mlc::String c_name, mlc::String header, int later_arity, int earlier_arity, mlc::String earlier_label, ast::Span source_span) noexcept{
  return mlc::Array<ast::Diagnostic>{ast::diagnostic_new_with_code(mlc::String("error", 5), mismatch_message(c_name, header, later_arity, earlier_arity, earlier_label), source_span, diagnostic_codes::diagnostic_code_e090())};
}
mlc::Array<ast::Diagnostic> diagnostics_from_extern_body(int parameters_length, std::shared_ptr<ast::Expr> body, mlc::HashMap<mlc::String, mlc::String> binding_by_key) noexcept{
  return [&]() -> mlc::Array<ast::Diagnostic> {
auto __match_subject = body;
if (std::holds_alternative<ast::ExprExtern>((*__match_subject))) {
const ast::ExprExtern& exprExtern = std::get<ast::ExprExtern>((*__match_subject));
auto [c_name, header, attributes, source_span] = exprExtern; return [&]() {
auto unused_attributes = attributes;
return ((header.length() == 0) ? (empty_diagnostic_list()) : (((header == header_import_extern_marker()) ? (empty_diagnostic_list()) : (((c_name.length() == 0) ? (empty_diagnostic_list()) : ([&]() -> mlc::Array<ast::Diagnostic> {
  if (binding_by_key.has(ffi_binding_key(c_name, header)))   {
    return [&]() {
auto key = ffi_binding_key(c_name, header);
auto encoded = binding_by_key.get(key);
auto earlier_arity = decode_arity(encoded);
auto earlier_label = decode_label(encoded);
return ((parameters_length == earlier_arity) ? (empty_diagnostic_list()) : (((earlier_label == span_site_label(source_span)) ? (empty_diagnostic_list()) : (error_for_conflict(c_name, header, parameters_length, earlier_arity, earlier_label, source_span)))));
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
mlc::Array<ast::Diagnostic> diagnostics_for_declaration(std::shared_ptr<ast::Decl> declaration, mlc::HashMap<mlc::String, mlc::String> binding_by_key) noexcept{
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
return diagnostics_from_extern_body(parameters.length(), body, binding_by_key);
}();
}
if (std::holds_alternative<ast::DeclExported>((*__match_subject))) {
const ast::DeclExported& declExported = std::get<ast::DeclExported>((*__match_subject));
auto [inner] = declExported; return diagnostics_for_declaration(inner, binding_by_key);
}
return empty_diagnostic_list();
std::abort();
}();
}
mlc::Array<ast::Diagnostic> extern_dedup_diagnostics(ast::Program program) noexcept{
  auto binding_by_key = mlc::HashMap<mlc::String, mlc::String>();
  auto declaration_index = 0;
  while ((declaration_index < program.decls.length()))   {
    (binding_by_key = record_declaration(binding_by_key, program.decls[declaration_index]));
    (declaration_index = mlc::arith::checked_add(declaration_index, 1));
  }
  auto diagnostics = empty_diagnostic_list();
  (declaration_index = 0);
  while ((declaration_index < program.decls.length()))   {
    (diagnostics = ast::diagnostics_append(diagnostics, diagnostics_for_declaration(program.decls[declaration_index], binding_by_key)));
    (declaration_index = mlc::arith::checked_add(declaration_index, 1));
  }
  return diagnostics;
}

} // namespace extern_dedup_lint
