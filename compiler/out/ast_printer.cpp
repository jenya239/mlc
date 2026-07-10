#define main mlc_user_main
#include "ast_printer.hpp"

#include "ast.hpp"
#include "ast.hpp"

namespace ast_printer {

using namespace ast;
using namespace ast;
using namespace ast_tokens;

mlc::Array<int> zero_based_indices(int count) noexcept{
  if ((count <= 0))   {
    return mlc::Array<int>{};
  } else   {
    return zero_based_indices(mlc::arith::checked_sub(count, 1)).concat(mlc::Array<int>{mlc::arith::checked_sub(count, 1)});
  }
}
mlc::String indent_text(int depth) noexcept{
  return zero_based_indices(depth).map([=](int _) mutable { return mlc::String("  ", 2); }).join(mlc::String("", 0));
}
mlc::String padding_spaces(int count) noexcept{
  if ((count <= 0))   {
    return mlc::String("", 0);
  } else   {
    return zero_based_indices(count).map([=](int _) mutable { return mlc::String(" ", 1); }).join(mlc::String("", 0));
  }
}
mlc::String escape_string_literal_content(mlc::String input) noexcept{
  auto parts = mlc::Array<mlc::String>{};
  auto index = 0;
  while ((index < input.length()))   {
    auto character = input.char_at(index);
    if ((character == mlc::String("\\", 1)))     {
      parts.push_back(mlc::String("\\\\", 2));
    } else if ((character == mlc::String("\"", 1)))     {
      parts.push_back(mlc::String("\\\"", 2));
    } else if ((character == mlc::String("\n", 1)))     {
      parts.push_back(mlc::String("\\n", 2));
    } else if ((character == mlc::String("\r", 1)))     {
      parts.push_back(mlc::String("\\r", 2));
    } else if ((character == mlc::String("\t", 1)))     {
      parts.push_back(mlc::String("\\t", 2));
    } else     {
      parts.push_back(character);
    }
    (index = mlc::arith::checked_add(index, 1));
  }
  return parts.join(mlc::String("", 0));
}
mlc::String print_string_literal(mlc::String value) noexcept{
  return ((mlc::String("\"", 1) + escape_string_literal_content(value)) + mlc::String("\"", 1));
}
mlc::String print_bool_literal(bool value) noexcept{
  if (value)   {
    return mlc::String("true", 4);
  } else   {
    return mlc::String("false", 5);
  }
}
mlc::String print_unit_literal() noexcept{
  return (mlc::String("(", 1) + mlc::String(")", 1));
}
mlc::String record_spread_text() noexcept{
  return ((mlc::String(".", 1) + mlc::String(".", 1)) + mlc::String(".", 1));
}
mlc::String print_mlc_type(std::shared_ptr<ast::TypeExpr> type_expression) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = type_expression;
if (std::holds_alternative<ast::TyI32>((*__match_subject))) {
const ast::TyI32& tyI32 = std::get<ast::TyI32>((*__match_subject));
return mlc::String("i32", 3);
}
if (std::holds_alternative<ast::TyString>((*__match_subject))) {
const ast::TyString& tyString = std::get<ast::TyString>((*__match_subject));
return mlc::String("string", 6);
}
if (std::holds_alternative<ast::TyBool>((*__match_subject))) {
const ast::TyBool& tyBool = std::get<ast::TyBool>((*__match_subject));
return mlc::String("bool", 4);
}
if (std::holds_alternative<ast::TyUnit>((*__match_subject))) {
const ast::TyUnit& tyUnit = std::get<ast::TyUnit>((*__match_subject));
return mlc::String("unit", 4);
}
if (std::holds_alternative<ast::TyNamed>((*__match_subject))) {
const ast::TyNamed& tyNamed = std::get<ast::TyNamed>((*__match_subject));
auto [name] = tyNamed; return name;
}
return mlc::String("<unsupported-type>", 18);
std::abort();
}();
}
mlc::String print_mlc_parameter(std::shared_ptr<ast::Param> parameter) noexcept{
  if (parameter->is_mut)   {
    return (((mlc::String("let mut ", 8) + parameter->name) + mlc::String(": ", 2)) + print_mlc_type(parameter->type_value));
  } else   {
    return ((parameter->name + mlc::String(": ", 2)) + print_mlc_type(parameter->type_value));
  }
}
mlc::String print_mlc_parameters(mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept{
  return parameters.map([=](std::shared_ptr<ast::Param> parameter) mutable { return print_mlc_parameter(parameter); }).join(mlc::String(", ", 2));
}
int max_field_name_length_from_values(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values) noexcept{
  auto maximum = 0;
  auto index = 0;
  while ((index < field_values.length()))   {
    if ((field_values[index]->name.length() > maximum))     {
      (maximum = field_values[index]->name.length());
    }
    (index = mlc::arith::checked_add(index, 1));
  }
  return maximum;
}
int max_field_name_length_from_definitions(mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept{
  auto maximum = 0;
  auto index = 0;
  while ((index < field_definitions.length()))   {
    if ((field_definitions[index]->name.length() > maximum))     {
      (maximum = field_definitions[index]->name.length());
    }
    (index = mlc::arith::checked_add(index, 1));
  }
  return maximum;
}
int record_fields_count(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values) noexcept{
  auto count = 0;
  auto index = 0;
  while ((index < field_values.length()))   {
    (count = mlc::arith::checked_add(count, 1));
    (index = mlc::arith::checked_add(index, 1));
  }
  return count;
}
int record_lit_part_field_count(ast::RecordLitPart part) noexcept{
  return std::visit(overloaded{[&](const ast::RecordLitFields& recordLitFields) { auto [field_values] = recordLitFields; return record_fields_count(field_values); },
[&](const ast::RecordLitSpread& recordLitSpread) { auto [__0] = recordLitSpread; return 0; }
}, part);
}
int record_lit_part_max_name_length(ast::RecordLitPart part) noexcept{
  return std::visit(overloaded{[&](const ast::RecordLitFields& recordLitFields) { auto [field_values] = recordLitFields; return max_field_name_length_from_values(field_values); },
[&](const ast::RecordLitSpread& recordLitSpread) { auto [__0] = recordLitSpread; return 0; }
}, part);
}
int max_field_name_length_in_lit_parts(mlc::Array<ast::RecordLitPart> lit_parts) noexcept{
  auto maximum = 0;
  auto index = 0;
  while ((index < lit_parts.length()))   {
    auto part_maximum = record_lit_part_max_name_length(lit_parts[index]);
    if ((part_maximum > maximum))     {
      (maximum = part_maximum);
    }
    (index = mlc::arith::checked_add(index, 1));
  }
  return maximum;
}
int record_lit_parts_field_value_count(mlc::Array<ast::RecordLitPart> lit_parts) noexcept{
  auto count = 0;
  auto index = 0;
  while ((index < lit_parts.length()))   {
    (count = mlc::arith::checked_add(count, record_lit_part_field_count(lit_parts[index])));
    (index = mlc::arith::checked_add(index, 1));
  }
  return count;
}
mlc::String print_field_value_inline(std::shared_ptr<ast::FieldVal> field_value, int depth) noexcept{
  return ((field_value->name + mlc::String(": ", 2)) + print_mlc_expr_at_depth(field_value->value, depth));
}
mlc::String print_field_values_inline(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, int depth) noexcept{
  return field_values.map([=](std::shared_ptr<ast::FieldVal> field_value) mutable { return print_field_value_inline(field_value, depth); }).join(mlc::String(", ", 2));
}
mlc::String print_field_value_aligned(std::shared_ptr<ast::FieldVal> field_value, int name_width, int depth) noexcept{
  return ((((indent_text(depth) + field_value->name) + mlc::String(": ", 2)) + padding_spaces(mlc::arith::checked_sub(name_width, field_value->name.length()))) + print_mlc_expr_at_depth(field_value->value, depth));
}
mlc::Array<mlc::String> append_field_values_inline(mlc::Array<mlc::String> segments, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, int depth) noexcept{
  auto result = segments;
  auto field_index = 0;
  while ((field_index < field_values.length()))   {
    result.push_back(print_field_value_inline(field_values[field_index], depth));
    (field_index = mlc::arith::checked_add(field_index, 1));
  }
  return result;
}
mlc::Array<mlc::String> append_record_lit_part_inline(mlc::Array<mlc::String> segments, ast::RecordLitPart part, int depth) noexcept{
  return std::visit(overloaded{[&](const ast::RecordLitFields& recordLitFields) { auto [field_values] = recordLitFields; return append_field_values_inline(segments, field_values, depth); },
[&](const ast::RecordLitSpread& recordLitSpread) { auto [expression] = recordLitSpread; return segments.concat(mlc::Array<mlc::String>{(record_spread_text() + print_mlc_expr_at_depth(expression, depth))}); }
}, part);
}
mlc::String print_record_lit_parts_inline(mlc::Array<ast::RecordLitPart> lit_parts, int depth) noexcept{
  auto segments = mlc::Array<mlc::String>{};
  auto index = 0;
  while ((index < lit_parts.length()))   {
    (segments = append_record_lit_part_inline(segments, lit_parts[index], depth));
    (index = mlc::arith::checked_add(index, 1));
  }
  return segments.join(mlc::String(", ", 2));
}
mlc::Array<mlc::String> append_field_values_multiline(mlc::Array<mlc::String> lines, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, int name_width, int depth) noexcept{
  auto result = lines;
  auto field_index = 0;
  while ((field_index < field_values.length()))   {
    result.push_back((print_field_value_aligned(field_values[field_index], name_width, depth) + mlc::String(",", 1)));
    (field_index = mlc::arith::checked_add(field_index, 1));
  }
  return result;
}
mlc::Array<mlc::String> append_record_lit_part_multiline(mlc::Array<mlc::String> lines, ast::RecordLitPart part, int name_width, int depth) noexcept{
  return std::visit(overloaded{[&](const ast::RecordLitFields& recordLitFields) { auto [field_values] = recordLitFields; return append_field_values_multiline(lines, field_values, name_width, depth); },
[&](const ast::RecordLitSpread& recordLitSpread) { auto [expression] = recordLitSpread; return lines.concat(mlc::Array<mlc::String>{(((indent_text(depth) + record_spread_text()) + print_mlc_expr_at_depth(expression, depth)) + mlc::String(",", 1))}); }
}, part);
}
mlc::String print_record_lit_parts_multiline(mlc::Array<ast::RecordLitPart> lit_parts, int name_width, int depth) noexcept{
  auto lines = mlc::Array<mlc::String>{};
  auto index = 0;
  while ((index < lit_parts.length()))   {
    (lines = append_record_lit_part_multiline(lines, lit_parts[index], name_width, depth));
    (index = mlc::arith::checked_add(index, 1));
  }
  return lines.join(mlc::String("\n", 1));
}
mlc::String print_record_literal(mlc::String type_name, mlc::Array<ast::RecordLitPart> lit_parts, int depth) noexcept{
  if ((record_lit_parts_field_value_count(lit_parts) <= 1))   {
    return (((type_name + mlc::String(" { ", 3)) + print_record_lit_parts_inline(lit_parts, depth)) + mlc::String(" }", 2));
  } else   {
    auto name_width = max_field_name_length_in_lit_parts(lit_parts);
    return ((((((type_name + mlc::String(" {", 2)) + mlc::String("\n", 1)) + print_record_lit_parts_multiline(lit_parts, name_width, mlc::arith::checked_add(depth, 1))) + mlc::String("\n", 1)) + indent_text(depth)) + mlc::String("}", 1));
  }
}
mlc::String print_record_update(mlc::String type_name, std::shared_ptr<ast::Expr> base, mlc::Array<std::shared_ptr<ast::FieldVal>> overrides, int depth) noexcept{
  if ((overrides.length() <= 1))   {
    return ((((((type_name + mlc::String(" { ", 3)) + record_spread_text()) + print_mlc_expr_at_depth(base, depth)) + mlc::String(", ", 2)) + print_field_values_inline(overrides, depth)) + mlc::String(" }", 2));
  } else   {
    auto name_width = max_field_name_length_from_values(overrides);
    return (((((((((((type_name + mlc::String(" {", 2)) + mlc::String("\n", 1)) + indent_text(mlc::arith::checked_add(depth, 1))) + record_spread_text()) + print_mlc_expr_at_depth(base, mlc::arith::checked_add(depth, 1))) + mlc::String(",", 1)) + mlc::String("\n", 1)) + overrides.map([=](std::shared_ptr<ast::FieldVal> field_value) mutable { return (print_field_value_aligned(field_value, name_width, mlc::arith::checked_add(depth, 1)) + mlc::String(",", 1)); }).join(mlc::String("\n", 1))) + mlc::String("\n", 1)) + indent_text(depth)) + mlc::String("}", 1));
  }
}
mlc::String print_type_fields_inline(mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept{
  return field_definitions.map([=](std::shared_ptr<ast::FieldDef> field_definition) mutable { return ((field_definition->name + mlc::String(": ", 2)) + print_mlc_type(field_definition->type_value)); }).join(mlc::String(", ", 2));
}
mlc::String print_type_fields_multiline(mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions, int name_width, int depth) noexcept{
  return field_definitions.map([=](std::shared_ptr<ast::FieldDef> field_definition) mutable { return (((((indent_text(depth) + field_definition->name) + mlc::String(": ", 2)) + padding_spaces(mlc::arith::checked_sub(name_width, field_definition->name.length()))) + print_mlc_type(field_definition->type_value)) + mlc::String(",", 1)); }).join(mlc::String("\n", 1));
}
mlc::String print_type_record_body(mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions, int depth) noexcept{
  if ((field_definitions.length() <= 1))   {
    return ((mlc::String("{ ", 2) + print_type_fields_inline(field_definitions)) + mlc::String(" }", 2));
  } else   {
    auto name_width = max_field_name_length_from_definitions(field_definitions);
    return (((((mlc::String("{", 1) + mlc::String("\n", 1)) + print_type_fields_multiline(field_definitions, name_width, mlc::arith::checked_add(depth, 1))) + mlc::String("\n", 1)) + indent_text(depth)) + mlc::String("}", 1));
  }
}
mlc::String format_do_end_block(mlc::Array<std::shared_ptr<ast::Stmt>> statements, std::shared_ptr<ast::Expr> result_expression, int depth) noexcept{
  auto header = (indent_text(depth) + mlc::String("do", 2));
  auto body_lines = print_mlc_block_lines(statements, result_expression, mlc::arith::checked_add(depth, 1));
  auto footer = (indent_text(depth) + mlc::String("end", 3));
  if ((body_lines == mlc::String("", 0)))   {
    return ((header + mlc::String("\n", 1)) + footer);
  } else   {
    return ((((header + mlc::String("\n", 1)) + body_lines) + mlc::String("\n", 1)) + footer);
  }
}
mlc::String print_mlc_stmt_expression_at_depth(std::shared_ptr<ast::Expr> expression, int depth) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = expression;
if (std::holds_alternative<ast::ExprBlock>((*__match_subject))) {
const ast::ExprBlock& exprBlock = std::get<ast::ExprBlock>((*__match_subject));
auto [__0, __1, __2] = exprBlock; return print_mlc_expr_at_depth(expression, depth);
}
return (indent_text(depth) + print_mlc_expr_at_depth(expression, depth));
std::abort();
}();
}
mlc::String print_mlc_block_lines(mlc::Array<std::shared_ptr<ast::Stmt>> statements, std::shared_ptr<ast::Expr> result_expression, int depth) noexcept{
  auto parts = mlc::Array<mlc::String>{};
  auto index = 0;
  while ((index < statements.length()))   {
    parts.push_back(print_mlc_stmt_at_depth(statements[index], depth));
    (index = mlc::arith::checked_add(index, 1));
  }
  parts.push_back(print_mlc_stmt_expression_at_depth(result_expression, depth));
  return parts.join(mlc::String("\n", 1));
}
mlc::String print_mlc_let_stmt(mlc::String name, bool is_mutable, std::shared_ptr<ast::Expr> value, int depth) noexcept{
  if (is_mutable)   {
    return ((((indent_text(depth) + mlc::String("let mut ", 8)) + name) + mlc::String(" = ", 3)) + print_mlc_expr_at_depth(value, depth));
  } else   {
    return ((((indent_text(depth) + mlc::String("let ", 4)) + name) + mlc::String(" = ", 3)) + print_mlc_expr_at_depth(value, depth));
  }
}
mlc::String print_mlc_type_decl(mlc::String name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept{
  if ((variants.length() == 1))   {
    return [&]() -> mlc::String {
auto __match_subject = variants[0];
if (std::holds_alternative<ast::VarRecord>((*__match_subject))) {
const ast::VarRecord& varRecord = std::get<ast::VarRecord>((*__match_subject));
auto [__0, field_definitions, __2] = varRecord; return (((mlc::String("type ", 5) + name) + mlc::String(" = ", 3)) + print_type_record_body(field_definitions, 0));
}
return mlc::String("<unsupported-type-decl>", 23);
std::abort();
}();
  } else   {
    return mlc::String("<unsupported-type-decl>", 23);
  }
}
mlc::String print_mlc_stmt_at_depth(std::shared_ptr<ast::Stmt> statement, int depth) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = statement;
if (std::holds_alternative<ast::StmtLet>((*__match_subject))) {
const ast::StmtLet& stmtLet = std::get<ast::StmtLet>((*__match_subject));
auto [name, is_mutable, __2, value, __4] = stmtLet; return print_mlc_let_stmt(name, is_mutable, value, depth);
}
if (std::holds_alternative<ast::StmtExpr>((*__match_subject))) {
const ast::StmtExpr& stmtExpr = std::get<ast::StmtExpr>((*__match_subject));
auto [expression, __1] = stmtExpr; return print_mlc_stmt_expression_at_depth(expression, depth);
}
if (std::holds_alternative<ast::StmtReturn>((*__match_subject))) {
const ast::StmtReturn& stmtReturn = std::get<ast::StmtReturn>((*__match_subject));
auto [expression, __1] = stmtReturn; return ((indent_text(depth) + mlc::String("return ", 7)) + print_mlc_expr_at_depth(expression, depth));
}
if (std::holds_alternative<ast::StmtBreak>((*__match_subject))) {
const ast::StmtBreak& stmtBreak = std::get<ast::StmtBreak>((*__match_subject));
auto [__0] = stmtBreak; return (indent_text(depth) + mlc::String("break", 5));
}
if (std::holds_alternative<ast::StmtContinue>((*__match_subject))) {
const ast::StmtContinue& stmtContinue = std::get<ast::StmtContinue>((*__match_subject));
auto [__0] = stmtContinue; return (indent_text(depth) + mlc::String("continue", 8));
}
return (indent_text(depth) + mlc::String("<unsupported-stmt>", 18));
std::abort();
}();
}
mlc::String print_mlc_expr_at_depth(std::shared_ptr<ast::Expr> expression, int depth) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = expression;
if (std::holds_alternative<ast::ExprInt>((*__match_subject))) {
const ast::ExprInt& exprInt = std::get<ast::ExprInt>((*__match_subject));
auto [value, __1] = exprInt; return mlc::to_string(value);
}
if (std::holds_alternative<ast::ExprStr>((*__match_subject))) {
const ast::ExprStr& exprStr = std::get<ast::ExprStr>((*__match_subject));
auto [value, __1] = exprStr; return print_string_literal(value);
}
if (std::holds_alternative<ast::ExprBool>((*__match_subject))) {
const ast::ExprBool& exprBool = std::get<ast::ExprBool>((*__match_subject));
auto [value, __1] = exprBool; return print_bool_literal(value);
}
if (std::holds_alternative<ast::ExprUnit>((*__match_subject))) {
const ast::ExprUnit& exprUnit = std::get<ast::ExprUnit>((*__match_subject));
auto [__0] = exprUnit; return print_unit_literal();
}
if (std::holds_alternative<ast::ExprIdent>((*__match_subject))) {
const ast::ExprIdent& exprIdent = std::get<ast::ExprIdent>((*__match_subject));
auto [name, __1] = exprIdent; return name;
}
if (std::holds_alternative<ast::ExprBlock>((*__match_subject))) {
const ast::ExprBlock& exprBlock = std::get<ast::ExprBlock>((*__match_subject));
auto [statements, result_expression, __2] = exprBlock; return format_do_end_block(statements, result_expression, depth);
}
if (std::holds_alternative<ast::ExprRecord>((*__match_subject))) {
const ast::ExprRecord& exprRecord = std::get<ast::ExprRecord>((*__match_subject));
auto [type_name, lit_parts, __2] = exprRecord; return print_record_literal(type_name, lit_parts, depth);
}
if (std::holds_alternative<ast::ExprRecordUpdate>((*__match_subject))) {
const ast::ExprRecordUpdate& exprRecordUpdate = std::get<ast::ExprRecordUpdate>((*__match_subject));
auto [type_name, base, overrides, __3] = exprRecordUpdate; return print_record_update(type_name, base, overrides, depth);
}
return mlc::String("<unsupported-expr>", 18);
std::abort();
}();
}
mlc::String print_mlc_expr(std::shared_ptr<ast::Expr> expression) noexcept{
  return print_mlc_expr_at_depth(expression, 0);
}
mlc::String print_mlc_function_body(std::shared_ptr<ast::Expr> body_expression, int depth) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = body_expression;
if (std::holds_alternative<ast::ExprBlock>((*__match_subject))) {
const ast::ExprBlock& exprBlock = std::get<ast::ExprBlock>((*__match_subject));
auto [statements, result_expression, __2] = exprBlock; return format_do_end_block(statements, result_expression, depth);
}
return print_mlc_expr_at_depth(body_expression, depth);
std::abort();
}();
}
mlc::String print_mlc_decl(std::shared_ptr<ast::Decl> declaration) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = declaration;
if (std::holds_alternative<ast::DeclFn>((*__match_subject))) {
const ast::DeclFn& declFn = std::get<ast::DeclFn>((*__match_subject));
auto [name, __1, __2, parameters, return_type, body, __6] = declFn; return (((((((mlc::String("fn ", 3) + name) + mlc::String("(", 1)) + print_mlc_parameters(parameters)) + mlc::String(") -> ", 5)) + print_mlc_type(return_type)) + mlc::String(" = ", 3)) + print_mlc_function_body(body, 0));
}
if (std::holds_alternative<ast::DeclType>((*__match_subject))) {
const ast::DeclType& declType = std::get<ast::DeclType>((*__match_subject));
auto [name, __1, variants, __3, __4] = declType; return print_mlc_type_decl(name, variants);
}
return mlc::String("<unsupported-decl>", 18);
std::abort();
}();
}
mlc::String print_mlc_program(ast::Program program) noexcept{
  auto parts = mlc::Array<mlc::String>{};
  auto index = 0;
  while ((index < program.decls.length()))   {
    parts.push_back(print_mlc_decl(program.decls[index]));
    (index = mlc::arith::checked_add(index, 1));
  }
  return parts.join(mlc::String("\n", 1));
}

} // namespace ast_printer
