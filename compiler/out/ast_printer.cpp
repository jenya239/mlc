#include "ast_printer.hpp"

#include "ast.hpp"
#include "ast.hpp"

namespace ast_printer {

using namespace ast;
using namespace ast;
using namespace ast_tokens;

mlc::Array<int> zero_based_indices(int count) noexcept;

mlc::String indent_text(int depth) noexcept;

mlc::String padding_spaces(int count) noexcept;

mlc::String escape_string_literal_content(mlc::String input) noexcept;

mlc::String print_string_literal(mlc::String value) noexcept;

mlc::String print_bool_literal(bool value) noexcept;

mlc::String print_unit_literal() noexcept;

mlc::String record_spread_text() noexcept;

mlc::String print_mlc_type(std::shared_ptr<ast::TypeExpr> type_expression) noexcept;

mlc::String print_mlc_parameter(std::shared_ptr<ast::Param> parameter) noexcept;

mlc::String print_mlc_parameters(mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept;

int max_field_name_length_from_values(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values) noexcept;

int max_field_name_length_from_definitions(mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept;

int record_fields_count(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values) noexcept;

int record_lit_part_field_count(ast::RecordLitPart part) noexcept;

int record_lit_part_max_name_length(ast::RecordLitPart part) noexcept;

int max_field_name_length_in_lit_parts(mlc::Array<ast::RecordLitPart> lit_parts) noexcept;

int record_lit_parts_field_value_count(mlc::Array<ast::RecordLitPart> lit_parts) noexcept;

mlc::String print_field_value_inline(std::shared_ptr<ast::FieldVal> field_value, int depth) noexcept;

mlc::String print_field_values_inline(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, int depth) noexcept;

mlc::String print_field_value_aligned(std::shared_ptr<ast::FieldVal> field_value, int name_width, int depth) noexcept;

mlc::Array<mlc::String> append_field_values_inline(mlc::Array<mlc::String> segments, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, int depth) noexcept;

mlc::Array<mlc::String> append_record_lit_part_inline(mlc::Array<mlc::String> segments, ast::RecordLitPart part, int depth) noexcept;

mlc::String print_record_lit_parts_inline(mlc::Array<ast::RecordLitPart> lit_parts, int depth) noexcept;

mlc::Array<mlc::String> append_field_values_multiline(mlc::Array<mlc::String> lines, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, int name_width, int depth) noexcept;

mlc::Array<mlc::String> append_record_lit_part_multiline(mlc::Array<mlc::String> lines, ast::RecordLitPart part, int name_width, int depth) noexcept;

mlc::String print_record_lit_parts_multiline(mlc::Array<ast::RecordLitPart> lit_parts, int name_width, int depth) noexcept;

mlc::String print_record_literal(mlc::String type_name, mlc::Array<ast::RecordLitPart> lit_parts, int depth) noexcept;

mlc::String print_record_update(mlc::String type_name, std::shared_ptr<ast::Expr> base, mlc::Array<std::shared_ptr<ast::FieldVal>> overrides, int depth) noexcept;

mlc::String print_type_fields_inline(mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept;

mlc::String print_type_fields_multiline(mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions, int name_width, int depth) noexcept;

mlc::String print_type_record_body(mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions, int depth) noexcept;

mlc::String format_do_end_block(mlc::Array<std::shared_ptr<ast::Stmt>> statements, std::shared_ptr<ast::Expr> result_expression, int depth) noexcept;

mlc::String print_mlc_stmt_expression_at_depth(std::shared_ptr<ast::Expr> expression, int depth) noexcept;

mlc::String print_mlc_block_lines(mlc::Array<std::shared_ptr<ast::Stmt>> statements, std::shared_ptr<ast::Expr> result_expression, int depth) noexcept;

mlc::String print_mlc_let_stmt(mlc::String name, bool is_mutable, std::shared_ptr<ast::Expr> value, int depth) noexcept;

mlc::String print_mlc_type_decl(mlc::String name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::String print_mlc_stmt_at_depth(std::shared_ptr<ast::Stmt> statement, int depth) noexcept;

mlc::String print_mlc_expr_at_depth(std::shared_ptr<ast::Expr> expression, int depth) noexcept;

mlc::String print_mlc_expr(std::shared_ptr<ast::Expr> expression) noexcept;

mlc::String print_mlc_function_body(std::shared_ptr<ast::Expr> body_expression, int depth) noexcept;

mlc::String print_mlc_decl(std::shared_ptr<ast::Decl> declaration) noexcept;

mlc::String print_mlc_program(ast::Program program) noexcept;

mlc::Array<int> zero_based_indices(int count) noexcept{return count <= 0 ? mlc::Array<int>{} : zero_based_indices(count - 1).concat(mlc::Array<int>{count - 1});}

mlc::String indent_text(int depth) noexcept{return zero_based_indices(depth).map([](int _) mutable { return mlc::String("  "); }).join(mlc::String(""));}

mlc::String padding_spaces(int count) noexcept{return count <= 0 ? mlc::String("") : zero_based_indices(count).map([](int _) mutable { return mlc::String(" "); }).join(mlc::String(""));}

mlc::String escape_string_literal_content(mlc::String input) noexcept{
mlc::Array<mlc::String> parts = {};
int index = 0;
while (index < input.length()){
{
mlc::String character = input.char_at(index);
if (character == mlc::String("\\")){
{
parts.push_back(mlc::String("\\\\"));
}
} else {
{
if (character == mlc::String("\"")){
parts.push_back(mlc::String("\\\""));
} else {
if (character == mlc::String("\n")){
parts.push_back(mlc::String("\\n"));
} else {
if (character == mlc::String("\r")){
parts.push_back(mlc::String("\\r"));
} else {
if (character == mlc::String("\t")){
parts.push_back(mlc::String("\\t"));
} else {
parts.push_back(character);
}
}
}
}
}
}
index = index + 1;
}
}
return parts.join(mlc::String(""));
}

mlc::String print_string_literal(mlc::String value) noexcept{return mlc::String("\"") + escape_string_literal_content(value) + mlc::String("\"");}

mlc::String print_bool_literal(bool value) noexcept{return value ? mlc::String("true") : mlc::String("false");}

mlc::String print_unit_literal() noexcept{return mlc::String("(") + mlc::String(")");}

mlc::String record_spread_text() noexcept{return mlc::String(".") + mlc::String(".") + mlc::String(".");}

mlc::String print_mlc_type(std::shared_ptr<ast::TypeExpr> type_expression) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::TyI32>((*type_expression))) {  return mlc::String("i32"); } if (std::holds_alternative<ast::TyString>((*type_expression))) {  return mlc::String("string"); } if (std::holds_alternative<ast::TyBool>((*type_expression))) {  return mlc::String("bool"); } if (std::holds_alternative<ast::TyUnit>((*type_expression))) {  return mlc::String("unit"); } if (std::holds_alternative<ast::TyNamed>((*type_expression))) { auto _v_tynamed = std::get<ast::TyNamed>((*type_expression)); auto [name] = _v_tynamed; return name; } return mlc::String("<unsupported-type>"); }();}

mlc::String print_mlc_parameter(std::shared_ptr<ast::Param> parameter) noexcept{return parameter->is_mut ? mlc::String("let mut ") + parameter->name + mlc::String(": ") + print_mlc_type(parameter->type_value) : parameter->name + mlc::String(": ") + print_mlc_type(parameter->type_value);}

mlc::String print_mlc_parameters(mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept{return parameters.map([](std::shared_ptr<ast::Param> parameter) mutable { return print_mlc_parameter(parameter); }).join(mlc::String(", "));}

int max_field_name_length_from_values(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values) noexcept{
int maximum = 0;
int index = 0;
while (index < field_values.size()){
{
if (field_values[index]->name.length() > maximum){
{
maximum = field_values[index]->name.length();
}
}
index = index + 1;
}
}
return maximum;
}

int max_field_name_length_from_definitions(mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept{
int maximum = 0;
int index = 0;
while (index < field_definitions.size()){
{
if (field_definitions[index]->name.length() > maximum){
{
maximum = field_definitions[index]->name.length();
}
}
index = index + 1;
}
}
return maximum;
}

int record_fields_count(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values) noexcept{
int count = 0;
int index = 0;
while (index < field_values.size()){
{
count = count + 1;
index = index + 1;
}
}
return count;
}

int record_lit_part_field_count(ast::RecordLitPart part) noexcept{return std::visit(overloaded{
  [&](const RecordLitFields& recordlitfields) -> int { auto [field_values] = recordlitfields; return record_fields_count(field_values); },
  [&](const RecordLitSpread& recordlitspread) -> int { auto [_w0] = recordlitspread; return 0; }
}, part);}

int record_lit_part_max_name_length(ast::RecordLitPart part) noexcept{return std::visit(overloaded{
  [&](const RecordLitFields& recordlitfields) -> int { auto [field_values] = recordlitfields; return max_field_name_length_from_values(field_values); },
  [&](const RecordLitSpread& recordlitspread) -> int { auto [_w0] = recordlitspread; return 0; }
}, part);}

int max_field_name_length_in_lit_parts(mlc::Array<ast::RecordLitPart> lit_parts) noexcept{
int maximum = 0;
int index = 0;
while (index < lit_parts.size()){
{
int part_maximum = record_lit_part_max_name_length(lit_parts[index]);
if (part_maximum > maximum){
{
maximum = part_maximum;
}
}
index = index + 1;
}
}
return maximum;
}

int record_lit_parts_field_value_count(mlc::Array<ast::RecordLitPart> lit_parts) noexcept{
int count = 0;
int index = 0;
while (index < lit_parts.size()){
{
count = count + record_lit_part_field_count(lit_parts[index]);
index = index + 1;
}
}
return count;
}

mlc::String print_field_value_inline(std::shared_ptr<ast::FieldVal> field_value, int depth) noexcept{return field_value->name + mlc::String(": ") + print_mlc_expr_at_depth(field_value->value, depth);}

mlc::String print_field_values_inline(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, int depth) noexcept{return field_values.map([depth](std::shared_ptr<ast::FieldVal> field_value) mutable { return print_field_value_inline(field_value, depth); }).join(mlc::String(", "));}

mlc::String print_field_value_aligned(std::shared_ptr<ast::FieldVal> field_value, int name_width, int depth) noexcept{return indent_text(depth) + field_value->name + mlc::String(": ") + padding_spaces(name_width - field_value->name.length()) + print_mlc_expr_at_depth(field_value->value, depth);}

mlc::Array<mlc::String> append_field_values_inline(mlc::Array<mlc::String> segments, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, int depth) noexcept{
mlc::Array<mlc::String> result = segments;
int field_index = 0;
while (field_index < field_values.size()){
{
result.push_back(print_field_value_inline(field_values[field_index], depth));
field_index = field_index + 1;
}
}
return result;
}

mlc::Array<mlc::String> append_record_lit_part_inline(mlc::Array<mlc::String> segments, ast::RecordLitPart part, int depth) noexcept{return std::visit(overloaded{
  [&](const RecordLitFields& recordlitfields) -> mlc::Array<mlc::String> { auto [field_values] = recordlitfields; return append_field_values_inline(segments, field_values, depth); },
  [&](const RecordLitSpread& recordlitspread) -> mlc::Array<mlc::String> { auto [expression] = recordlitspread; return segments.concat(mlc::Array<mlc::String>{record_spread_text() + print_mlc_expr_at_depth(expression, depth)}); }
}, part);}

mlc::String print_record_lit_parts_inline(mlc::Array<ast::RecordLitPart> lit_parts, int depth) noexcept{
mlc::Array<mlc::String> segments = {};
int index = 0;
while (index < lit_parts.size()){
{
segments = append_record_lit_part_inline(segments, lit_parts[index], depth);
index = index + 1;
}
}
return segments.join(mlc::String(", "));
}

mlc::Array<mlc::String> append_field_values_multiline(mlc::Array<mlc::String> lines, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, int name_width, int depth) noexcept{
mlc::Array<mlc::String> result = lines;
int field_index = 0;
while (field_index < field_values.size()){
{
result.push_back(print_field_value_aligned(field_values[field_index], name_width, depth) + mlc::String(","));
field_index = field_index + 1;
}
}
return result;
}

mlc::Array<mlc::String> append_record_lit_part_multiline(mlc::Array<mlc::String> lines, ast::RecordLitPart part, int name_width, int depth) noexcept{return std::visit(overloaded{
  [&](const RecordLitFields& recordlitfields) -> mlc::Array<mlc::String> { auto [field_values] = recordlitfields; return append_field_values_multiline(lines, field_values, name_width, depth); },
  [&](const RecordLitSpread& recordlitspread) -> mlc::Array<mlc::String> { auto [expression] = recordlitspread; return lines.concat(mlc::Array<mlc::String>{indent_text(depth) + record_spread_text() + print_mlc_expr_at_depth(expression, depth) + mlc::String(",")}); }
}, part);}

mlc::String print_record_lit_parts_multiline(mlc::Array<ast::RecordLitPart> lit_parts, int name_width, int depth) noexcept{
mlc::Array<mlc::String> lines = {};
int index = 0;
while (index < lit_parts.size()){
{
lines = append_record_lit_part_multiline(lines, lit_parts[index], name_width, depth);
index = index + 1;
}
}
return lines.join(mlc::String("\n"));
}

mlc::String print_record_literal(mlc::String type_name, mlc::Array<ast::RecordLitPart> lit_parts, int depth) noexcept{return record_lit_parts_field_value_count(lit_parts) <= 1 ? type_name + mlc::String(" { ") + print_record_lit_parts_inline(lit_parts, depth) + mlc::String(" }") : [&]() -> mlc::String { 
  int name_width = max_field_name_length_in_lit_parts(lit_parts);
  return type_name + mlc::String(" {") + mlc::String("\n") + print_record_lit_parts_multiline(lit_parts, name_width, depth + 1) + mlc::String("\n") + indent_text(depth) + mlc::String("}");
 }();}

mlc::String print_record_update(mlc::String type_name, std::shared_ptr<ast::Expr> base, mlc::Array<std::shared_ptr<ast::FieldVal>> overrides, int depth) noexcept{return overrides.size() <= 1 ? type_name + mlc::String(" { ") + record_spread_text() + print_mlc_expr_at_depth(base, depth) + mlc::String(", ") + print_field_values_inline(overrides, depth) + mlc::String(" }") : [&]() -> mlc::String { 
  int name_width = max_field_name_length_from_values(overrides);
  return type_name + mlc::String(" {") + mlc::String("\n") + indent_text(depth + 1) + record_spread_text() + print_mlc_expr_at_depth(base, depth + 1) + mlc::String(",") + mlc::String("\n") + overrides.map([name_width, depth](std::shared_ptr<ast::FieldVal> field_value) mutable { return print_field_value_aligned(field_value, name_width, depth + 1) + mlc::String(","); }).join(mlc::String("\n")) + mlc::String("\n") + indent_text(depth) + mlc::String("}");
 }();}

mlc::String print_type_fields_inline(mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept{return field_definitions.map([](std::shared_ptr<ast::FieldDef> field_definition) mutable { return field_definition->name + mlc::String(": ") + print_mlc_type(field_definition->type_value); }).join(mlc::String(", "));}

mlc::String print_type_fields_multiline(mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions, int name_width, int depth) noexcept{return field_definitions.map([depth, name_width](std::shared_ptr<ast::FieldDef> field_definition) mutable { return indent_text(depth) + field_definition->name + mlc::String(": ") + padding_spaces(name_width - field_definition->name.length()) + print_mlc_type(field_definition->type_value) + mlc::String(","); }).join(mlc::String("\n"));}

mlc::String print_type_record_body(mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions, int depth) noexcept{return field_definitions.size() <= 1 ? mlc::String("{ ") + print_type_fields_inline(field_definitions) + mlc::String(" }") : [&]() -> mlc::String { 
  int name_width = max_field_name_length_from_definitions(field_definitions);
  return mlc::String("{") + mlc::String("\n") + print_type_fields_multiline(field_definitions, name_width, depth + 1) + mlc::String("\n") + indent_text(depth) + mlc::String("}");
 }();}

mlc::String format_do_end_block(mlc::Array<std::shared_ptr<ast::Stmt>> statements, std::shared_ptr<ast::Expr> result_expression, int depth) noexcept{
mlc::String header = indent_text(depth) + mlc::String("do");
mlc::String body_lines = print_mlc_block_lines(statements, result_expression, depth + 1);
mlc::String footer = indent_text(depth) + mlc::String("end");
return body_lines == mlc::String("") ? header + mlc::String("\n") + footer : header + mlc::String("\n") + body_lines + mlc::String("\n") + footer;
}

mlc::String print_mlc_stmt_expression_at_depth(std::shared_ptr<ast::Expr> expression, int depth) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprBlock>((*expression)._)) { auto _v_exprblock = std::get<ast::ExprBlock>((*expression)._); auto [_w0, _w1, _w2] = _v_exprblock; return print_mlc_expr_at_depth(expression, depth); } return indent_text(depth) + print_mlc_expr_at_depth(expression, depth); }();}

mlc::String print_mlc_block_lines(mlc::Array<std::shared_ptr<ast::Stmt>> statements, std::shared_ptr<ast::Expr> result_expression, int depth) noexcept{
mlc::Array<mlc::String> parts = {};
int index = 0;
while (index < statements.size()){
{
parts.push_back(print_mlc_stmt_at_depth(statements[index], depth));
index = index + 1;
}
}
parts.push_back(print_mlc_stmt_expression_at_depth(result_expression, depth));
return parts.join(mlc::String("\n"));
}

mlc::String print_mlc_let_stmt(mlc::String name, bool is_mutable, std::shared_ptr<ast::Expr> value, int depth) noexcept{return is_mutable ? indent_text(depth) + mlc::String("let mut ") + name + mlc::String(" = ") + print_mlc_expr_at_depth(value, depth) : indent_text(depth) + mlc::String("let ") + name + mlc::String(" = ") + print_mlc_expr_at_depth(value, depth);}

mlc::String print_mlc_type_decl(mlc::String name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{return variants.size() == 1 ? [&]() -> mlc::String { if (std::holds_alternative<ast::VarRecord>((*variants[0]))) { auto _v_varrecord = std::get<ast::VarRecord>((*variants[0])); auto [_w0, field_definitions, _w1] = _v_varrecord; return mlc::String("type ") + name + mlc::String(" = ") + print_type_record_body(field_definitions, 0); } return mlc::String("<unsupported-type-decl>"); }() : mlc::String("<unsupported-type-decl>");}

mlc::String print_mlc_stmt_at_depth(std::shared_ptr<ast::Stmt> statement, int depth) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::StmtLet>((*statement)._)) { auto _v_stmtlet = std::get<ast::StmtLet>((*statement)._); auto [name, is_mutable, _w0, value, _w1] = _v_stmtlet; return print_mlc_let_stmt(name, is_mutable, value, depth); } if (std::holds_alternative<ast::StmtExpr>((*statement)._)) { auto _v_stmtexpr = std::get<ast::StmtExpr>((*statement)._); auto [expression, _w0] = _v_stmtexpr; return print_mlc_stmt_expression_at_depth(expression, depth); } if (std::holds_alternative<ast::StmtReturn>((*statement)._)) { auto _v_stmtreturn = std::get<ast::StmtReturn>((*statement)._); auto [expression, _w0] = _v_stmtreturn; return indent_text(depth) + mlc::String("return ") + print_mlc_expr_at_depth(expression, depth); } if (std::holds_alternative<ast::StmtBreak>((*statement)._)) { auto _v_stmtbreak = std::get<ast::StmtBreak>((*statement)._); auto [_w0] = _v_stmtbreak; return indent_text(depth) + mlc::String("break"); } if (std::holds_alternative<ast::StmtContinue>((*statement)._)) { auto _v_stmtcontinue = std::get<ast::StmtContinue>((*statement)._); auto [_w0] = _v_stmtcontinue; return indent_text(depth) + mlc::String("continue"); } return indent_text(depth) + mlc::String("<unsupported-stmt>"); }();}

mlc::String print_mlc_expr_at_depth(std::shared_ptr<ast::Expr> expression, int depth) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprInt>((*expression)._)) { auto _v_exprint = std::get<ast::ExprInt>((*expression)._); auto [value, _w0] = _v_exprint; return mlc::to_string(value); } if (std::holds_alternative<ast::ExprStr>((*expression)._)) { auto _v_exprstr = std::get<ast::ExprStr>((*expression)._); auto [value, _w0] = _v_exprstr; return print_string_literal(value); } if (std::holds_alternative<ast::ExprBool>((*expression)._)) { auto _v_exprbool = std::get<ast::ExprBool>((*expression)._); auto [value, _w0] = _v_exprbool; return print_bool_literal(value); } if (std::holds_alternative<ast::ExprUnit>((*expression)._)) { auto _v_exprunit = std::get<ast::ExprUnit>((*expression)._); auto [_w0] = _v_exprunit; return print_unit_literal(); } if (std::holds_alternative<ast::ExprIdent>((*expression)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*expression)._); auto [name, _w0] = _v_exprident; return name; } if (std::holds_alternative<ast::ExprBlock>((*expression)._)) { auto _v_exprblock = std::get<ast::ExprBlock>((*expression)._); auto [statements, result_expression, _w0] = _v_exprblock; return format_do_end_block(statements, result_expression, depth); } if (std::holds_alternative<ast::ExprRecord>((*expression)._)) { auto _v_exprrecord = std::get<ast::ExprRecord>((*expression)._); auto [type_name, lit_parts, _w0] = _v_exprrecord; return print_record_literal(type_name, lit_parts, depth); } if (std::holds_alternative<ast::ExprRecordUpdate>((*expression)._)) { auto _v_exprrecordupdate = std::get<ast::ExprRecordUpdate>((*expression)._); auto [type_name, base, overrides, _w0] = _v_exprrecordupdate; return print_record_update(type_name, base, overrides, depth); } return mlc::String("<unsupported-expr>"); }();}

mlc::String print_mlc_expr(std::shared_ptr<ast::Expr> expression) noexcept{return print_mlc_expr_at_depth(expression, 0);}

mlc::String print_mlc_function_body(std::shared_ptr<ast::Expr> body_expression, int depth) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprBlock>((*body_expression)._)) { auto _v_exprblock = std::get<ast::ExprBlock>((*body_expression)._); auto [statements, result_expression, _w0] = _v_exprblock; return format_do_end_block(statements, result_expression, depth); } return print_mlc_expr_at_depth(body_expression, depth); }();}

mlc::String print_mlc_decl(std::shared_ptr<ast::Decl> declaration) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::DeclFn>((*declaration))) { auto _v_declfn = std::get<ast::DeclFn>((*declaration)); auto [name, _w0, _w1, parameters, return_type, body, _w2] = _v_declfn; return mlc::String("fn ") + name + mlc::String("(") + print_mlc_parameters(parameters) + mlc::String(") -> ") + print_mlc_type(return_type) + mlc::String(" = ") + print_mlc_function_body(body, 0); } if (std::holds_alternative<ast::DeclType>((*declaration))) { auto _v_decltype = std::get<ast::DeclType>((*declaration)); auto [name, _w0, variants, _w1, _w2] = _v_decltype; return print_mlc_type_decl(name, variants); } return mlc::String("<unsupported-decl>"); }();}

mlc::String print_mlc_program(ast::Program program) noexcept{
mlc::Array<mlc::String> parts = {};
int index = 0;
while (index < program.decls.size()){
{
parts.push_back(print_mlc_decl(program.decls[index]));
index = index + 1;
}
}
return parts.join(mlc::String("\n"));
}

} // namespace ast_printer
