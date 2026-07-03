#define main mlc_user_main
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

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> hover_function_parameter_environment(mlc::Array<std::shared_ptr<ast::Param>> parameters, registry::TypeRegistry registry) noexcept{
  auto type_environment = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
  auto index = 0;
  while ((index < parameters.length()))   {
    auto parameter = parameters[index];
    type_environment.set(ast::param_name(parameter), registry::type_from_annotation_with_registry(ast::param_type_value(parameter), registry));
    (index = (index + 1));
  }
  return type_environment;
}
mlc::String hover_type_for_identifier_in_program(ast::Program program, registry::TypeRegistry registry, mlc::String identifier_name, int target_line_one_based) noexcept{
  if (registry::TypeRegistry_has_fn(registry, identifier_name))   {
    return semantic_type_structure::type_description(registry::TypeRegistry_fn_type(registry, identifier_name));
  } else if (registry::TypeRegistry_has_ctor(registry, identifier_name))   {
    return semantic_type_structure::type_description(registry::TypeRegistry_ctor_type(registry, identifier_name));
  } else   {
    auto index = 0;
    auto found_type = mlc::String("", 0);
    while (((index < program.decls.length()) && (found_type.length() == 0)))     {
      [&]() {
auto __match_subject = ast::decl_inner(program.decls[index]);
if (std::holds_alternative<ast::DeclFn>((*__match_subject))) {
const ast::DeclFn& declFn = std::get<ast::DeclFn>((*__match_subject));
auto [__0, __1, __2, parameters, __4, function_body, __6] = declFn; [&]() {
auto type_environment = hover_function_parameter_environment(parameters, registry);
auto desugared_body = partial_application_desugar::partial_application_desugar_expr(function_body);
[&]() {
auto __match_subject = desugared_body;
if (std::holds_alternative<ast::ExprBlock>((*__match_subject))) {
const ast::ExprBlock& exprBlock = std::get<ast::ExprBlock>((*__match_subject));
auto [statements, __1, __2] = exprBlock; [&]() {
auto statement_index = 0;
return [&]() {
while ((statement_index < statements.length())) {
[&]() {
auto __match_subject = statements[statement_index];
if (std::holds_alternative<ast::StmtLet>((*__match_subject))) {
const ast::StmtLet& stmtLet = std::get<ast::StmtLet>((*__match_subject));
auto [name, __1, __2, value, statement_span] = stmtLet; [&]() {
if ((statement_span.line <= target_line_one_based)) {
  auto inference_context = check_context::check_context_new(type_environment, registry);
  type_environment.set(name, infer::infer_expr(value, inference_context).inferred_type);
}
return [&]() {
while (false) {
}
}();
}();
return;
}
if (std::holds_alternative<ast::StmtExpr>((*__match_subject))) {
const ast::StmtExpr& stmtExpr = std::get<ast::StmtExpr>((*__match_subject));
auto [expression, __1] = stmtExpr; [&]() {
if ((ast::stmt_span(statements[statement_index]).line <= target_line_one_based)) {
  infer::infer_expr(expression, check_context::check_context_new(type_environment, registry));
}
return [&]() {
while (false) {
}
}();
}();
return;
}
std::make_tuple();
return;
std::abort();
}();
(statement_index = (statement_index + 1));
}
}();
}();
return;
}
std::make_tuple();
return;
std::abort();
}();
auto inference_context = check_context::check_context_new(type_environment, registry);
auto inferred = infer_expr_ident::infer_expr_identifier(identifier_name, inference_context);
(found_type = semantic_type_structure::type_description(inferred.inferred_type));
return std::make_tuple();
}();
return;
}
std::make_tuple();
return;
std::abort();
}();
      (index = (index + 1));
    }
    return found_type;
  }
}
mlc::String resolve_hover_type_in_source(mlc::String source_text, mlc::String source_file, int line_zero_based, int column_zero_based) noexcept{
  auto expanded_program = param_destructure_expand::expand_parameter_destructuring_in_program(decls::parse_program(lexer::tokenize(source_text).tokens));
  auto registry = registry::build_registry(expanded_program);
  auto identifier_name = symbols::find_identifier_at_position(source_text, line_zero_based, column_zero_based);
  if ((identifier_name.length() == 0))   {
    return mlc::String("", 0);
  } else   {
    return hover_type_for_identifier_in_program(expanded_program, registry, identifier_name, (line_zero_based + 1));
  }
}

} // namespace hover
