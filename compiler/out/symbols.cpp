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

ast::Span span_with_file(mlc::String source_file, ast::Span source_span) noexcept;

void symbol_table_set(mlc::HashMap<mlc::String, ast::Span>& table, mlc::String name, mlc::String source_file, ast::Span source_span) noexcept;

void symbol_table_set_pattern_rest_name(mlc::HashMap<mlc::String, ast::Span>& table, mlc::String rest_name, mlc::String source_file, std::shared_ptr<ast::Pattern> pattern) noexcept;

void collect_first_pattern_alternative(mlc::Array<std::shared_ptr<ast::Pattern>> alternatives, mlc::String source_file, mlc::HashMap<mlc::String, ast::Span>& table) noexcept;

void collect_pattern_definition_spans(std::shared_ptr<ast::Pattern> pattern, mlc::String source_file, mlc::HashMap<mlc::String, ast::Span>& table) noexcept;

void collect_statement_definition_spans(std::shared_ptr<ast::Stmt> statement, mlc::String source_file, mlc::HashMap<mlc::String, ast::Span>& table) noexcept;

void collect_statements_definition_spans(mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::String source_file, mlc::HashMap<mlc::String, ast::Span>& table) noexcept;

void collect_match_arm_definition_spans(std::shared_ptr<ast::MatchArm> arm, mlc::String source_file, mlc::HashMap<mlc::String, ast::Span>& table) noexcept;

void collect_expression_definition_spans(std::shared_ptr<ast::Expr> expression, mlc::String source_file, mlc::HashMap<mlc::String, ast::Span>& table) noexcept;

void collect_declaration_definition_spans(std::shared_ptr<ast::Decl> declaration, mlc::String source_file, mlc::HashMap<mlc::String, ast::Span>& table) noexcept;

mlc::HashMap<mlc::String, ast::Span> build_symbol_table(ast::Program program, mlc::String source_file) noexcept;

mlc::String token_identifier_name(ast_tokens::Token token) noexcept;

mlc::String find_identifier_at_position(mlc::String source_text, int line_zero_based, int column_zero_based) noexcept;

ast::Span resolve_definition_span(mlc::HashMap<mlc::String, ast::Span> symbol_table, mlc::String identifier_name) noexcept;

ast::Span resolve_definition_in_source(mlc::String source_text, mlc::String source_file, int line_zero_based, int column_zero_based) noexcept;

ast::Span span_with_file(mlc::String source_file, ast::Span source_span) noexcept{return ast::Span{source_file, source_span.line, source_span.column};}

void symbol_table_set(mlc::HashMap<mlc::String, ast::Span>& table, mlc::String name, mlc::String source_file, ast::Span source_span) noexcept{
if (name.length() > 0 && source_span.line > 0){
table.set(name, span_with_file(source_file, source_span));
}
}

void symbol_table_set_pattern_rest_name(mlc::HashMap<mlc::String, ast::Span>& table, mlc::String rest_name, mlc::String source_file, std::shared_ptr<ast::Pattern> pattern) noexcept{
if (rest_name.length() > 0 && rest_name != mlc::String("_")){
symbol_table_set(table, rest_name, source_file, ast::pattern_span(pattern));
}
}

void collect_first_pattern_alternative(mlc::Array<std::shared_ptr<ast::Pattern>> alternatives, mlc::String source_file, mlc::HashMap<mlc::String, ast::Span>& table) noexcept{
if (alternatives.size() > 0){
collect_pattern_definition_spans(alternatives[0], source_file, table);
}
}

void collect_pattern_definition_spans(std::shared_ptr<ast::Pattern> pattern, mlc::String source_file, mlc::HashMap<mlc::String, ast::Span>& table) noexcept{
return [&]() -> void { if (std::holds_alternative<ast::PatternIdent>((*pattern))) { auto _v_patternident = std::get<ast::PatternIdent>((*pattern)); auto [name, pattern_source_span] = _v_patternident; return symbol_table_set(table, name, source_file, pattern_source_span); } if (std::holds_alternative<ast::PatternCtor>((*pattern))) { auto _v_patternctor = std::get<ast::PatternCtor>((*pattern)); auto [_w0, sub_patterns, _w1] = _v_patternctor; return [&]() { 
  int index = 0;
  return [&]() { 
  while (index < sub_patterns.size()){
{
collect_pattern_definition_spans(sub_patterns[index], source_file, table);
index = index + 1;
}
}
 }();
 }(); } if (std::holds_alternative<ast::PatternRecord>((*pattern))) { auto _v_patternrecord = std::get<ast::PatternRecord>((*pattern)); auto [_w0, field_patterns, _w1] = _v_patternrecord; return [&]() { 
  int index = 0;
  return [&]() { 
  while (index < field_patterns.size()){
{
collect_pattern_definition_spans(field_patterns[index], source_file, table);
index = index + 1;
}
}
 }();
 }(); } if (std::holds_alternative<ast::PatternTuple>((*pattern))) { auto _v_patterntuple = std::get<ast::PatternTuple>((*pattern)); auto [sub_patterns, _w0] = _v_patterntuple; return [&]() { 
  int index = 0;
  return [&]() { 
  while (index < sub_patterns.size()){
{
collect_pattern_definition_spans(sub_patterns[index], source_file, table);
index = index + 1;
}
}
 }();
 }(); } if (std::holds_alternative<ast::PatternArray>((*pattern))) { auto _v_patternarray = std::get<ast::PatternArray>((*pattern)); auto [sub_patterns, rest_name, _w0] = _v_patternarray; return [&]() { 
  int index = 0;
  while (index < sub_patterns.size()){
{
collect_pattern_definition_spans(sub_patterns[index], source_file, table);
index = index + 1;
}
}
  return symbol_table_set_pattern_rest_name(table, rest_name, source_file, pattern);
 }(); } if (std::holds_alternative<ast::PatternOr>((*pattern))) { auto _v_patternor = std::get<ast::PatternOr>((*pattern)); auto [alternatives, _w0] = _v_patternor; return collect_first_pattern_alternative(alternatives, source_file, table); } return; }();
}

void collect_statement_definition_spans(std::shared_ptr<ast::Stmt> statement, mlc::String source_file, mlc::HashMap<mlc::String, ast::Span>& table) noexcept{
return std::visit(overloaded{
  [&](const StmtLet& stmtlet) -> void { auto [name, _w0, _w1, value, statement_span] = stmtlet; [&]() { 
  symbol_table_set(table, name, source_file, statement_span);
  return collect_expression_definition_spans(value, source_file, table);
 }(); },
  [&](const StmtLetPattern& stmtletpattern) -> void { auto [pattern, _w0, _w1, value, has_else, else_body, _w2] = stmtletpattern; [&]() { 
  collect_expression_definition_spans(value, source_file, table);
  collect_pattern_definition_spans(pattern, source_file, table);
  if (has_else){
collect_expression_definition_spans(else_body, source_file, table);
}
 }(); },
  [&](const StmtLetConst& stmtletconst) -> void { auto [name, _w0, value, statement_span] = stmtletconst; [&]() { 
  symbol_table_set(table, name, source_file, statement_span);
  return collect_expression_definition_spans(value, source_file, table);
 }(); },
  [&](const StmtExpr& stmtexpr) -> void { auto [expression, _w0] = stmtexpr; collect_expression_definition_spans(expression, source_file, table); },
  [&](const StmtReturn& stmtreturn) -> void { auto [expression, _w0] = stmtreturn; collect_expression_definition_spans(expression, source_file, table); },
  [&](const StmtBreak& stmtbreak) -> void { auto [_w0] = stmtbreak; std::make_tuple(); },
  [&](const StmtContinue& stmtcontinue) -> void { auto [_w0] = stmtcontinue; std::make_tuple(); }
}, (*statement)._);
}

void collect_statements_definition_spans(mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::String source_file, mlc::HashMap<mlc::String, ast::Span>& table) noexcept{
int index = 0;
return [&]() { 
  while (index < statements.size()){
{
collect_statement_definition_spans(statements[index], source_file, table);
index = index + 1;
}
}
 }();
}

void collect_match_arm_definition_spans(std::shared_ptr<ast::MatchArm> arm, mlc::String source_file, mlc::HashMap<mlc::String, ast::Span>& table) noexcept{
collect_pattern_definition_spans(arm->pattern, source_file, table);
return collect_expression_definition_spans(arm->body, source_file, table);
}

void collect_expression_definition_spans(std::shared_ptr<ast::Expr> expression, mlc::String source_file, mlc::HashMap<mlc::String, ast::Span>& table) noexcept{
return [&]() -> void { if (std::holds_alternative<ast::ExprBlock>((*expression)._)) { auto _v_exprblock = std::get<ast::ExprBlock>((*expression)._); auto [statements, result_expression, _w0] = _v_exprblock; return [&]() { 
  collect_statements_definition_spans(statements, source_file, table);
  return collect_expression_definition_spans(result_expression, source_file, table);
 }(); } if (std::holds_alternative<ast::ExprLambda>((*expression)._)) { auto _v_exprlambda = std::get<ast::ExprLambda>((*expression)._); auto [parameter_names, body_expression, lambda_span] = _v_exprlambda; return [&]() { 
  int index = 0;
  while (index < parameter_names.size()){
{
symbol_table_set(table, parameter_names[index], source_file, lambda_span);
index = index + 1;
}
}
  return collect_expression_definition_spans(body_expression, source_file, table);
 }(); } if (std::holds_alternative<ast::ExprFor>((*expression)._)) { auto _v_exprfor = std::get<ast::ExprFor>((*expression)._); auto [loop_variable, iterable, body_statements, loop_span] = _v_exprfor; return [&]() { 
  symbol_table_set(table, loop_variable, source_file, loop_span);
  collect_expression_definition_spans(iterable, source_file, table);
  return collect_statements_definition_spans(body_statements, source_file, table);
 }(); } if (std::holds_alternative<ast::ExprWhile>((*expression)._)) { auto _v_exprwhile = std::get<ast::ExprWhile>((*expression)._); auto [condition, body_statements, _w0] = _v_exprwhile; return [&]() { 
  collect_expression_definition_spans(condition, source_file, table);
  return collect_statements_definition_spans(body_statements, source_file, table);
 }(); } if (std::holds_alternative<ast::ExprIf>((*expression)._)) { auto _v_exprif = std::get<ast::ExprIf>((*expression)._); auto [condition, then_branch, else_branch, _w0] = _v_exprif; return [&]() { 
  collect_expression_definition_spans(condition, source_file, table);
  collect_expression_definition_spans(then_branch, source_file, table);
  return collect_expression_definition_spans(else_branch, source_file, table);
 }(); } if (std::holds_alternative<ast::ExprMatch>((*expression)._)) { auto _v_exprmatch = std::get<ast::ExprMatch>((*expression)._); auto [scrutinee, arms, _w0] = _v_exprmatch; return [&]() { 
  collect_expression_definition_spans(scrutinee, source_file, table);
  int index = 0;
  return [&]() { 
  while (index < arms.size()){
{
collect_match_arm_definition_spans(arms[index], source_file, table);
index = index + 1;
}
}
 }();
 }(); } if (std::holds_alternative<ast::ExprWith>((*expression)._)) { auto _v_exprwith = std::get<ast::ExprWith>((*expression)._); auto [resource_expression, binder_name, body_statements, with_span] = _v_exprwith; return [&]() { 
  collect_expression_definition_spans(resource_expression, source_file, table);
  symbol_table_set(table, binder_name, source_file, with_span);
  return collect_statements_definition_spans(body_statements, source_file, table);
 }(); } if (std::holds_alternative<ast::ExprBin>((*expression)._)) { auto _v_exprbin = std::get<ast::ExprBin>((*expression)._); auto [_w0, left_expression, right_expression, _w1] = _v_exprbin; return [&]() { 
  collect_expression_definition_spans(left_expression, source_file, table);
  return collect_expression_definition_spans(right_expression, source_file, table);
 }(); } if (std::holds_alternative<ast::ExprUn>((*expression)._)) { auto _v_exprun = std::get<ast::ExprUn>((*expression)._); auto [_w0, inner_expression, _w1] = _v_exprun; return collect_expression_definition_spans(inner_expression, source_file, table); } if (std::holds_alternative<ast::ExprCall>((*expression)._)) { auto _v_exprcall = std::get<ast::ExprCall>((*expression)._); auto [callee, arguments, _w0] = _v_exprcall; return [&]() { 
  collect_expression_definition_spans(callee, source_file, table);
  int index = 0;
  return [&]() { 
  while (index < arguments.size()){
{
collect_expression_definition_spans(arguments[index], source_file, table);
index = index + 1;
}
}
 }();
 }(); } if (std::holds_alternative<ast::ExprMethod>((*expression)._)) { auto _v_exprmethod = std::get<ast::ExprMethod>((*expression)._); auto [receiver, _w0, arguments, _w1] = _v_exprmethod; return [&]() { 
  collect_expression_definition_spans(receiver, source_file, table);
  int index = 0;
  return [&]() { 
  while (index < arguments.size()){
{
collect_expression_definition_spans(arguments[index], source_file, table);
index = index + 1;
}
}
 }();
 }(); } if (std::holds_alternative<ast::ExprField>((*expression)._)) { auto _v_exprfield = std::get<ast::ExprField>((*expression)._); auto [object, _w0, _w1] = _v_exprfield; return collect_expression_definition_spans(object, source_file, table); } if (std::holds_alternative<ast::ExprIndex>((*expression)._)) { auto _v_exprindex = std::get<ast::ExprIndex>((*expression)._); auto [object, index_expression, _w0] = _v_exprindex; return [&]() { 
  collect_expression_definition_spans(object, source_file, table);
  return collect_expression_definition_spans(index_expression, source_file, table);
 }(); } if (std::holds_alternative<ast::ExprRecord>((*expression)._)) { auto _v_exprrecord = std::get<ast::ExprRecord>((*expression)._); auto [_w0, parts, _w1] = _v_exprrecord; return [&]() { 
  int index = 0;
  return [&]() { 
  while (index < parts.size()){
{
std::visit(overloaded{
  [&](const RecordLitFields& recordlitfields) -> void { auto [field_values] = recordlitfields; [&]() { 
  int field_index = 0;
  return [&]() { 
  while (field_index < field_values.size()){
{
collect_expression_definition_spans(field_values[field_index]->value, source_file, table);
field_index = field_index + 1;
}
}
 }();
 }(); },
  [&](const RecordLitSpread& recordlitspread) -> void { auto [spread_expression] = recordlitspread; collect_expression_definition_spans(spread_expression, source_file, table); }
}, parts[index]);
index = index + 1;
}
}
 }();
 }(); } if (std::holds_alternative<ast::ExprRecordUpdate>((*expression)._)) { auto _v_exprrecordupdate = std::get<ast::ExprRecordUpdate>((*expression)._); auto [_w0, base_record, field_values, _w1] = _v_exprrecordupdate; return [&]() { 
  collect_expression_definition_spans(base_record, source_file, table);
  int index = 0;
  return [&]() { 
  while (index < field_values.size()){
{
collect_expression_definition_spans(field_values[index]->value, source_file, table);
index = index + 1;
}
}
 }();
 }(); } if (std::holds_alternative<ast::ExprArray>((*expression)._)) { auto _v_exprarray = std::get<ast::ExprArray>((*expression)._); auto [elements, _w0] = _v_exprarray; return [&]() { 
  int index = 0;
  return [&]() { 
  while (index < elements.size()){
{
collect_expression_definition_spans(elements[index], source_file, table);
index = index + 1;
}
}
 }();
 }(); } if (std::holds_alternative<ast::ExprTuple>((*expression)._)) { auto _v_exprtuple = std::get<ast::ExprTuple>((*expression)._); auto [elements, _w0] = _v_exprtuple; return [&]() { 
  int index = 0;
  return [&]() { 
  while (index < elements.size()){
{
collect_expression_definition_spans(elements[index], source_file, table);
index = index + 1;
}
}
 }();
 }(); } if (std::holds_alternative<ast::ExprQuestion>((*expression)._)) { auto _v_exprquestion = std::get<ast::ExprQuestion>((*expression)._); auto [inner_expression, _w0] = _v_exprquestion; return collect_expression_definition_spans(inner_expression, source_file, table); } if (std::holds_alternative<ast::ExprNamedArg>((*expression)._)) { auto _v_exprnamedarg = std::get<ast::ExprNamedArg>((*expression)._); auto [_w0, argument_expression, _w1] = _v_exprnamedarg; return collect_expression_definition_spans(argument_expression, source_file, table); } return; }();
}

void collect_declaration_definition_spans(std::shared_ptr<ast::Decl> declaration, mlc::String source_file, mlc::HashMap<mlc::String, ast::Span>& table) noexcept{
return [&]() -> void { if (std::holds_alternative<ast::DeclFn>((*declaration))) { auto _v_declfn = std::get<ast::DeclFn>((*declaration)); auto [function_name, _w0, _w1, parameters, _w2, body_expression, _w3] = _v_declfn; return [&]() { 
  symbol_table_set(table, function_name, source_file, ast::expr_span(body_expression));
  int index = 0;
  while (index < parameters.size()){
{
symbol_table_set(table, parameters[index]->name, source_file, ast::expr_span(body_expression));
index = index + 1;
}
}
  return collect_expression_definition_spans(body_expression, source_file, table);
 }(); } if (std::holds_alternative<ast::DeclType>((*declaration))) { auto _v_decltype = std::get<ast::DeclType>((*declaration)); auto [type_name, _w0, _w1, _w2, name_span] = _v_decltype; return symbol_table_set(table, type_name, source_file, name_span); } if (std::holds_alternative<ast::DeclTypeAlias>((*declaration))) { auto _v_decltypealias = std::get<ast::DeclTypeAlias>((*declaration)); auto [alias_name, _w0, _w1, name_span] = _v_decltypealias; return symbol_table_set(table, alias_name, source_file, name_span); } if (std::holds_alternative<ast::DeclTrait>((*declaration))) { auto _v_decltrait = std::get<ast::DeclTrait>((*declaration)); auto [trait_name, _w0, members, name_span] = _v_decltrait; return [&]() { 
  symbol_table_set(table, trait_name, source_file, name_span);
  int index = 0;
  return [&]() { 
  while (index < members.size()){
{
collect_declaration_definition_spans(members[index], source_file, table);
index = index + 1;
}
}
 }();
 }(); } if (std::holds_alternative<ast::DeclExtend>((*declaration))) { auto _v_declextend = std::get<ast::DeclExtend>((*declaration)); auto [_w0, _w1, members, _w2] = _v_declextend; return [&]() { 
  int index = 0;
  return [&]() { 
  while (index < members.size()){
{
collect_declaration_definition_spans(members[index], source_file, table);
index = index + 1;
}
}
 }();
 }(); } if (std::holds_alternative<ast::DeclExported>((*declaration))) { auto _v_declexported = std::get<ast::DeclExported>((*declaration)); auto [inner_declaration] = _v_declexported; return collect_declaration_definition_spans(inner_declaration, source_file, table); } return; }();
}

mlc::HashMap<mlc::String, ast::Span> build_symbol_table(ast::Program program, mlc::String source_file) noexcept{
mlc::HashMap<mlc::String, ast::Span> table = mlc::HashMap<mlc::String, ast::Span>();
int index = 0;
while (index < program.decls.size()){
{
collect_declaration_definition_spans(program.decls[index], source_file, table);
index = index + 1;
}
}
return table;
}

mlc::String token_identifier_name(ast_tokens::Token token) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast_tokens::Ident>(ast_tokens::Token_kind_value(token))) { auto _v_ident = std::get<ast_tokens::Ident>(ast_tokens::Token_kind_value(token)); auto [name] = _v_ident; return name; } return mlc::String(""); }();}

mlc::String find_identifier_at_position(mlc::String source_text, int line_zero_based, int column_zero_based) noexcept{
ast_tokens::LexOut lex_output = lexer::tokenize(source_text);
int index = 0;
mlc::String found_name = mlc::String("");
int target_line = line_zero_based + 1;
int target_column = column_zero_based + 1;
while (index < lex_output.tokens.size()){
{
ast_tokens::Token token = lex_output.tokens[index];
mlc::String identifier_name = token_identifier_name(token);
if (identifier_name.length() > 0){
{
found_name = ast_tokens::Token_line_number(token) == target_line && ast_tokens::Token_column(token) <= target_column && target_column < ast_tokens::Token_column(token) + identifier_name.length() ? identifier_name : found_name;
}
}
index = index + 1;
}
}
return found_name;
}

ast::Span resolve_definition_span(mlc::HashMap<mlc::String, ast::Span> symbol_table, mlc::String identifier_name) noexcept{return identifier_name.length() > 0 && symbol_table.has(identifier_name) ? symbol_table.get(identifier_name) : ast::span_unknown();}

ast::Span resolve_definition_in_source(mlc::String source_text, mlc::String source_file, int line_zero_based, int column_zero_based) noexcept{
ast::Program program = decls::parse_program(lexer::tokenize(source_text).tokens);
mlc::HashMap<mlc::String, ast::Span> symbol_table = build_symbol_table(program, source_file);
mlc::String identifier_name = find_identifier_at_position(source_text, line_zero_based, column_zero_based);
return resolve_definition_span(symbol_table, identifier_name);
}

} // namespace symbols
