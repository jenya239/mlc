#define main mlc_user_main
#include "symbols.hpp"

#include "ast.hpp"
#include "ast_tokens.hpp"
#include "lexer.hpp"
#include "decls.hpp"

namespace symbols {

using namespace ast;
using namespace ast_tokens;
using namespace lexer;
using namespace decls;
using namespace ast_tokens;

ast::Span span_with_file(mlc::String source_file, ast::Span source_span) noexcept{
  return ast::span_make(source_file, source_span.line, source_span.column);
}
void symbol_table_set(mlc::HashMap<mlc::String, ast::Span>& table, mlc::String name, mlc::String source_file, ast::Span source_span) noexcept{
  if (((name.length() > 0) && (source_span.line > 0)))   {
    table.set(name, span_with_file(source_file, source_span));
  }
}
void symbol_table_set_pattern_rest_name(mlc::HashMap<mlc::String, ast::Span>& table, mlc::String rest_name, mlc::String source_file, std::shared_ptr<ast::Pattern> pattern) noexcept{
  if (((rest_name.length() > 0) && (rest_name != mlc::String("_", 1))))   {
    symbol_table_set(table, rest_name, source_file, ast::pattern_span(pattern));
  }
}
void collect_first_pattern_alternative(mlc::Array<std::shared_ptr<ast::Pattern>> alternatives, mlc::String source_file, mlc::HashMap<mlc::String, ast::Span>& table) noexcept{
  if ((alternatives.length() > 0))   {
    collect_pattern_definition_spans(alternatives[0], source_file, table);
  }
}
void collect_pattern_list_definition_spans(mlc::Array<std::shared_ptr<ast::Pattern>> patterns, mlc::String source_file, mlc::HashMap<mlc::String, ast::Span>& table) noexcept{
  auto pattern_index = 0;
  return [&]() {
while ((pattern_index < patterns.length())) {
collect_pattern_definition_spans(patterns[pattern_index], source_file, table);
(pattern_index = mlc::arith::checked_add(pattern_index, 1));
}
}();
}
void collect_expression_list_definition_spans(mlc::Array<std::shared_ptr<ast::Expr>> expressions, mlc::String source_file, mlc::HashMap<mlc::String, ast::Span>& table) noexcept{
  auto expression_index = 0;
  return [&]() {
while ((expression_index < expressions.length())) {
collect_expression_definition_spans(expressions[expression_index], source_file, table);
(expression_index = mlc::arith::checked_add(expression_index, 1));
}
}();
}
void collect_declaration_list_definition_spans(mlc::Array<std::shared_ptr<ast::Decl>> declarations, mlc::String source_file, mlc::HashMap<mlc::String, ast::Span>& table) noexcept{
  auto declaration_index = 0;
  return [&]() {
while ((declaration_index < declarations.length())) {
collect_declaration_definition_spans(declarations[declaration_index], source_file, table);
(declaration_index = mlc::arith::checked_add(declaration_index, 1));
}
}();
}
void collect_record_parts_definition_spans(mlc::Array<ast::RecordLitPart> parts, mlc::String source_file, mlc::HashMap<mlc::String, ast::Span>& table) noexcept{
  auto part_index = 0;
  return [&]() {
while ((part_index < parts.length())) {
std::visit(overloaded{[&](const ast::RecordLitFields& recordLitFields) -> void { auto [field_values] = recordLitFields; [&]() {
auto field_index = 0;
return [&]() {
while ((field_index < field_values.length())) {
collect_expression_definition_spans(field_values[field_index]->value, source_file, table);
(field_index = mlc::arith::checked_add(field_index, 1));
}
}();
}(); },
[&](const ast::RecordLitSpread& recordLitSpread) -> void { auto [spread_expression] = recordLitSpread; collect_expression_definition_spans(spread_expression, source_file, table); }
}, parts[part_index]);
(part_index = mlc::arith::checked_add(part_index, 1));
}
}();
}
void collect_field_values_definition_spans(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, mlc::String source_file, mlc::HashMap<mlc::String, ast::Span>& table) noexcept{
  auto field_index = 0;
  return [&]() {
while ((field_index < field_values.length())) {
collect_expression_definition_spans(field_values[field_index]->value, source_file, table);
(field_index = mlc::arith::checked_add(field_index, 1));
}
}();
}
void collect_pattern_definition_spans(std::shared_ptr<ast::Pattern> pattern, mlc::String source_file, mlc::HashMap<mlc::String, ast::Span>& table) noexcept{
  return [&]() {
auto __match_subject = pattern;
if (std::holds_alternative<ast::PatternIdent>((*__match_subject))) {
const ast::PatternIdent& patternIdent = std::get<ast::PatternIdent>((*__match_subject));
auto [name, pattern_source_span] = patternIdent; symbol_table_set(table, name, source_file, pattern_source_span);
return;
}
if (std::holds_alternative<ast::PatternCtor>((*__match_subject))) {
const ast::PatternCtor& patternCtor = std::get<ast::PatternCtor>((*__match_subject));
auto [__0, sub_patterns, __2] = patternCtor; collect_pattern_list_definition_spans(sub_patterns, source_file, table);
return;
}
if (std::holds_alternative<ast::PatternRecord>((*__match_subject))) {
const ast::PatternRecord& patternRecord = std::get<ast::PatternRecord>((*__match_subject));
auto [__0, field_patterns, __2] = patternRecord; collect_pattern_list_definition_spans(field_patterns, source_file, table);
return;
}
if (std::holds_alternative<ast::PatternTuple>((*__match_subject))) {
const ast::PatternTuple& patternTuple = std::get<ast::PatternTuple>((*__match_subject));
auto [sub_patterns, __1] = patternTuple; collect_pattern_list_definition_spans(sub_patterns, source_file, table);
return;
}
if (std::holds_alternative<ast::PatternArray>((*__match_subject))) {
const ast::PatternArray& patternArray = std::get<ast::PatternArray>((*__match_subject));
auto [sub_patterns, rest_name, __2] = patternArray; [&]() {
collect_pattern_list_definition_spans(sub_patterns, source_file, table);
return symbol_table_set_pattern_rest_name(table, rest_name, source_file, pattern);
}();
return;
}
if (std::holds_alternative<ast::PatternOr>((*__match_subject))) {
const ast::PatternOr& patternOr = std::get<ast::PatternOr>((*__match_subject));
auto [alternatives, __1] = patternOr; collect_first_pattern_alternative(alternatives, source_file, table);
return;
}
std::make_tuple();
return;
std::abort();
}();
}
void collect_statement_definition_spans(std::shared_ptr<ast::Stmt> statement, mlc::String source_file, mlc::HashMap<mlc::String, ast::Span>& table) noexcept{
  return std::visit(overloaded{[&](const ast::StmtLet& stmtLet) -> void { auto [name, __1, __2, value, statement_span] = stmtLet; [&]() {
symbol_table_set(table, name, source_file, statement_span);
return collect_expression_definition_spans(value, source_file, table);
}(); },
[&](const ast::StmtLetPattern& stmtLetPattern) -> void { auto [pattern, __1, __2, value, has_else, else_body, __6] = stmtLetPattern; [&]() {
collect_expression_definition_spans(value, source_file, table);
collect_pattern_definition_spans(pattern, source_file, table);
if (has_else) {
  collect_expression_definition_spans(else_body, source_file, table);
}
return std::make_tuple();
}(); },
[&](const ast::StmtLetConst& stmtLetConst) -> void { auto [name, __1, value, statement_span] = stmtLetConst; [&]() {
symbol_table_set(table, name, source_file, statement_span);
return collect_expression_definition_spans(value, source_file, table);
}(); },
[&](const ast::StmtExpr& stmtExpr) -> void { auto [expression, __1] = stmtExpr; collect_expression_definition_spans(expression, source_file, table); },
[&](const ast::StmtReturn& stmtReturn) -> void { auto [expression, __1] = stmtReturn; collect_expression_definition_spans(expression, source_file, table); },
[&](const ast::StmtBreak& stmtBreak) -> void { auto [__0] = stmtBreak; std::make_tuple(); },
[&](const ast::StmtContinue& stmtContinue) -> void { auto [__0] = stmtContinue; std::make_tuple(); }
}, (*statement));
}
void collect_statements_definition_spans(mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::String source_file, mlc::HashMap<mlc::String, ast::Span>& table) noexcept{
  auto index = 0;
  return [&]() {
while ((index < statements.length())) {
collect_statement_definition_spans(statements[index], source_file, table);
(index = mlc::arith::checked_add(index, 1));
}
}();
}
void collect_match_arm_definition_spans(std::shared_ptr<ast::MatchArm> arm, mlc::String source_file, mlc::HashMap<mlc::String, ast::Span>& table) noexcept{
  collect_pattern_definition_spans(arm->pattern, source_file, table);
  return collect_expression_definition_spans(arm->body, source_file, table);
}
void collect_expression_definition_spans(std::shared_ptr<ast::Expr> expression, mlc::String source_file, mlc::HashMap<mlc::String, ast::Span>& table) noexcept{
  return [&]() {
auto __match_subject = expression;
if (std::holds_alternative<ast::ExprBlock>((*__match_subject))) {
const ast::ExprBlock& exprBlock = std::get<ast::ExprBlock>((*__match_subject));
auto [statements, result_expression, __2] = exprBlock; [&]() {
collect_statements_definition_spans(statements, source_file, table);
return collect_expression_definition_spans(result_expression, source_file, table);
}();
return;
}
if (std::holds_alternative<ast::ExprLambda>((*__match_subject))) {
const ast::ExprLambda& exprLambda = std::get<ast::ExprLambda>((*__match_subject));
auto [parameter_names, body_expression, lambda_span] = exprLambda; [&]() {
auto index = 0;
while ((index < parameter_names.length())) {
  symbol_table_set(table, parameter_names[index], source_file, lambda_span);
  (index = mlc::arith::checked_add(index, 1));
}
return collect_expression_definition_spans(body_expression, source_file, table);
}();
return;
}
if (std::holds_alternative<ast::ExprFor>((*__match_subject))) {
const ast::ExprFor& exprFor = std::get<ast::ExprFor>((*__match_subject));
auto [loop_variable, iterable, body_statements, loop_span] = exprFor; [&]() {
symbol_table_set(table, loop_variable, source_file, loop_span);
collect_expression_definition_spans(iterable, source_file, table);
return collect_statements_definition_spans(body_statements, source_file, table);
}();
return;
}
if (std::holds_alternative<ast::ExprWhile>((*__match_subject))) {
const ast::ExprWhile& exprWhile = std::get<ast::ExprWhile>((*__match_subject));
auto [condition, body_statements, __2] = exprWhile; [&]() {
collect_expression_definition_spans(condition, source_file, table);
return collect_statements_definition_spans(body_statements, source_file, table);
}();
return;
}
if (std::holds_alternative<ast::ExprSpawn>((*__match_subject))) {
const ast::ExprSpawn& exprSpawn = std::get<ast::ExprSpawn>((*__match_subject));
auto [body_statements, __1] = exprSpawn; collect_statements_definition_spans(body_statements, source_file, table);
return;
}
if (std::holds_alternative<ast::ExprIf>((*__match_subject))) {
const ast::ExprIf& exprIf = std::get<ast::ExprIf>((*__match_subject));
auto [condition, then_branch, else_branch, __3] = exprIf; [&]() {
collect_expression_definition_spans(condition, source_file, table);
collect_expression_definition_spans(then_branch, source_file, table);
return collect_expression_definition_spans(else_branch, source_file, table);
}();
return;
}
if (std::holds_alternative<ast::ExprMatch>((*__match_subject))) {
const ast::ExprMatch& exprMatch = std::get<ast::ExprMatch>((*__match_subject));
auto [scrutinee, arms, __2] = exprMatch; [&]() {
collect_expression_definition_spans(scrutinee, source_file, table);
auto index = 0;
return [&]() {
while ((index < arms.length())) {
collect_match_arm_definition_spans(arms[index], source_file, table);
(index = mlc::arith::checked_add(index, 1));
}
}();
}();
return;
}
if (std::holds_alternative<ast::ExprWith>((*__match_subject))) {
const ast::ExprWith& exprWith = std::get<ast::ExprWith>((*__match_subject));
auto [resource_expression, binder_name, body_statements, with_span] = exprWith; [&]() {
collect_expression_definition_spans(resource_expression, source_file, table);
symbol_table_set(table, binder_name, source_file, with_span);
return collect_statements_definition_spans(body_statements, source_file, table);
}();
return;
}
if (std::holds_alternative<ast::ExprBin>((*__match_subject))) {
const ast::ExprBin& exprBin = std::get<ast::ExprBin>((*__match_subject));
auto [__0, left_expression, right_expression, __3] = exprBin; [&]() {
collect_expression_definition_spans(left_expression, source_file, table);
return collect_expression_definition_spans(right_expression, source_file, table);
}();
return;
}
if (std::holds_alternative<ast::ExprUn>((*__match_subject))) {
const ast::ExprUn& exprUn = std::get<ast::ExprUn>((*__match_subject));
auto [__0, inner_expression, __2] = exprUn; collect_expression_definition_spans(inner_expression, source_file, table);
return;
}
if (std::holds_alternative<ast::ExprCall>((*__match_subject))) {
const ast::ExprCall& exprCall = std::get<ast::ExprCall>((*__match_subject));
auto [callee, arguments, __2] = exprCall; [&]() {
collect_expression_definition_spans(callee, source_file, table);
return collect_expression_list_definition_spans(arguments, source_file, table);
}();
return;
}
if (std::holds_alternative<ast::ExprMethod>((*__match_subject))) {
const ast::ExprMethod& exprMethod = std::get<ast::ExprMethod>((*__match_subject));
auto [receiver, __1, arguments, __3] = exprMethod; [&]() {
collect_expression_definition_spans(receiver, source_file, table);
return collect_expression_list_definition_spans(arguments, source_file, table);
}();
return;
}
if (std::holds_alternative<ast::ExprField>((*__match_subject))) {
const ast::ExprField& exprField = std::get<ast::ExprField>((*__match_subject));
auto [object, __1, __2] = exprField; collect_expression_definition_spans(object, source_file, table);
return;
}
if (std::holds_alternative<ast::ExprIndex>((*__match_subject))) {
const ast::ExprIndex& exprIndex = std::get<ast::ExprIndex>((*__match_subject));
auto [object, index_expression, __2] = exprIndex; [&]() {
collect_expression_definition_spans(object, source_file, table);
return collect_expression_definition_spans(index_expression, source_file, table);
}();
return;
}
if (std::holds_alternative<ast::ExprRecord>((*__match_subject))) {
const ast::ExprRecord& exprRecord = std::get<ast::ExprRecord>((*__match_subject));
auto [__0, parts, __2] = exprRecord; collect_record_parts_definition_spans(parts, source_file, table);
return;
}
if (std::holds_alternative<ast::ExprRecordUpdate>((*__match_subject))) {
const ast::ExprRecordUpdate& exprRecordUpdate = std::get<ast::ExprRecordUpdate>((*__match_subject));
auto [__0, base_record, field_values, __3] = exprRecordUpdate; [&]() {
collect_expression_definition_spans(base_record, source_file, table);
return collect_field_values_definition_spans(field_values, source_file, table);
}();
return;
}
if (std::holds_alternative<ast::ExprArray>((*__match_subject))) {
const ast::ExprArray& exprArray = std::get<ast::ExprArray>((*__match_subject));
auto [elements, __1] = exprArray; collect_expression_list_definition_spans(elements, source_file, table);
return;
}
if (std::holds_alternative<ast::ExprTuple>((*__match_subject))) {
const ast::ExprTuple& exprTuple = std::get<ast::ExprTuple>((*__match_subject));
auto [elements, __1] = exprTuple; collect_expression_list_definition_spans(elements, source_file, table);
return;
}
if (std::holds_alternative<ast::ExprQuestion>((*__match_subject))) {
const ast::ExprQuestion& exprQuestion = std::get<ast::ExprQuestion>((*__match_subject));
auto [inner_expression, __1] = exprQuestion; collect_expression_definition_spans(inner_expression, source_file, table);
return;
}
if (std::holds_alternative<ast::ExprNamedArg>((*__match_subject))) {
const ast::ExprNamedArg& exprNamedArg = std::get<ast::ExprNamedArg>((*__match_subject));
auto [__0, argument_expression, __2] = exprNamedArg; collect_expression_definition_spans(argument_expression, source_file, table);
return;
}
std::make_tuple();
return;
std::abort();
}();
}
void collect_declaration_definition_spans(std::shared_ptr<ast::Decl> declaration, mlc::String source_file, mlc::HashMap<mlc::String, ast::Span>& table) noexcept{
  return [&]() {
auto __match_subject = declaration;
if (std::holds_alternative<ast::DeclFn>((*__match_subject))) {
const ast::DeclFn& declFn = std::get<ast::DeclFn>((*__match_subject));
auto [function_name, __1, __2, parameters, __4, body_expression, __6] = declFn; [&]() {
symbol_table_set(table, function_name, source_file, ast::expr_span(body_expression));
auto index = 0;
while ((index < parameters.length())) {
  symbol_table_set(table, parameters[index]->name, source_file, ast::expr_span(body_expression));
  (index = mlc::arith::checked_add(index, 1));
}
return collect_expression_definition_spans(body_expression, source_file, table);
}();
return;
}
if (std::holds_alternative<ast::DeclType>((*__match_subject))) {
const ast::DeclType& declType = std::get<ast::DeclType>((*__match_subject));
auto [type_name, __1, __2, __3, name_span] = declType; symbol_table_set(table, type_name, source_file, name_span);
return;
}
if (std::holds_alternative<ast::DeclTypeAlias>((*__match_subject))) {
const ast::DeclTypeAlias& declTypeAlias = std::get<ast::DeclTypeAlias>((*__match_subject));
auto [alias_name, __1, __2, name_span] = declTypeAlias; symbol_table_set(table, alias_name, source_file, name_span);
return;
}
if (std::holds_alternative<ast::DeclTrait>((*__match_subject))) {
const ast::DeclTrait& declTrait = std::get<ast::DeclTrait>((*__match_subject));
auto [trait_name, __1, members, name_span] = declTrait; [&]() {
symbol_table_set(table, trait_name, source_file, name_span);
return collect_declaration_list_definition_spans(members, source_file, table);
}();
return;
}
if (std::holds_alternative<ast::DeclExtend>((*__match_subject))) {
const ast::DeclExtend& declExtend = std::get<ast::DeclExtend>((*__match_subject));
auto [__0, __1, members, __3] = declExtend; collect_declaration_list_definition_spans(members, source_file, table);
return;
}
if (std::holds_alternative<ast::DeclExported>((*__match_subject))) {
const ast::DeclExported& declExported = std::get<ast::DeclExported>((*__match_subject));
auto [inner_declaration] = declExported; collect_declaration_definition_spans(inner_declaration, source_file, table);
return;
}
std::make_tuple();
return;
std::abort();
}();
}
mlc::HashMap<mlc::String, ast::Span> build_symbol_table(ast::Program program, mlc::String source_file) noexcept{
  auto table = mlc::HashMap<mlc::String, ast::Span>();
  auto index = 0;
  while ((index < program.decls.length()))   {
    collect_declaration_definition_spans(program.decls[index], source_file, table);
    (index = mlc::arith::checked_add(index, 1));
  }
  return table;
}
mlc::String token_identifier_name(ast_tokens::Token token) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = ast_tokens::Token_kind_value(token);
if (std::holds_alternative<ast_tokens::Ident>(__match_subject)) {
const ast_tokens::Ident& ident = std::get<ast_tokens::Ident>(__match_subject);
auto [name] = ident; return name;
}
return mlc::String("", 0);
std::abort();
}();
}
mlc::String find_identifier_at_position(mlc::String source_text, int line_zero_based, int column_zero_based) noexcept{
  auto lex_output = lexer::tokenize(source_text);
  auto index = 0;
  auto found_name = mlc::String("", 0);
  auto target_line = mlc::arith::checked_add(line_zero_based, 1);
  auto target_column = mlc::arith::checked_add(column_zero_based, 1);
  while ((index < lex_output.tokens.length()))   {
    auto token = lex_output.tokens[index];
    auto identifier_name = token_identifier_name(token);
    if ((identifier_name.length() > 0))     {
      (found_name = ((((ast_tokens::Token_line_number(token) == target_line) && (ast_tokens::Token_column(token) <= target_column)) && (target_column < mlc::arith::checked_add(ast_tokens::Token_column(token), identifier_name.length()))) ? (identifier_name) : (found_name)));
    }
    (index = mlc::arith::checked_add(index, 1));
  }
  return found_name;
}
ast::Span resolve_definition_span(mlc::HashMap<mlc::String, ast::Span> symbol_table, mlc::String identifier_name) noexcept{
  if (((identifier_name.length() > 0) && symbol_table.has(identifier_name)))   {
    return symbol_table.get(identifier_name);
  } else   {
    return ast::span_unknown();
  }
}
ast::Span resolve_definition_in_source(mlc::String source_text, mlc::String source_file, int line_zero_based, int column_zero_based) noexcept{
  auto program = decls::parse_program(lexer::tokenize(source_text).tokens);
  auto symbol_table = build_symbol_table(program, source_file);
  auto identifier_name = find_identifier_at_position(source_text, line_zero_based, column_zero_based);
  return resolve_definition_span(symbol_table, identifier_name);
}

} // namespace symbols
