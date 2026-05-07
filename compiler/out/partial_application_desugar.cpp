#include "partial_application_desugar.hpp"

#include "ast.hpp"

namespace partial_application_desugar {

using namespace ast;
using namespace ast_tokens;

struct PartialPair {std::shared_ptr<ast::Expr> expression;int next_serial;};

struct StatementsPartialPair {mlc::Array<std::shared_ptr<ast::Stmt>> statements;int next_serial;};

struct ArgumentsPartialPair {mlc::Array<std::shared_ptr<ast::Expr>> arguments;int next_serial;};

struct RecordLitPartsPartialPair {mlc::Array<ast::RecordLitPart> parts;int next_serial;};

struct PartialNamesAllocation {mlc::Array<mlc::String> names;int next_serial;};

struct StmtPartialPair {std::shared_ptr<ast::Stmt> statement;int next_serial;};

struct MatchArmsPartialPair {mlc::Array<std::shared_ptr<ast::MatchArm>> arms;int next_serial;};

partial_application_desugar::PartialPair partial_application_leaf_partial_pair(std::shared_ptr<ast::Expr> leaf_expression, int serial) noexcept;

bool expr_is_placeholder(std::shared_ptr<ast::Expr> expression) noexcept;

mlc::Array<std::shared_ptr<ast::Expr>> partial_array_append_expression(mlc::Array<std::shared_ptr<ast::Expr>> elements, std::shared_ptr<ast::Expr> element) noexcept;

int partial_placeholder_delta_for_argument(std::shared_ptr<ast::Expr> argument) noexcept;

partial_application_desugar::PartialNamesAllocation partial_allocate_parameter_names(int hole_count, int serial_start) noexcept;

int partial_count_placeholder_arguments(mlc::Array<std::shared_ptr<ast::Expr>> arguments) noexcept;

mlc::Array<std::shared_ptr<ast::Expr>> partial_replace_call_arguments_inner(mlc::Array<std::shared_ptr<ast::Expr>> arguments, mlc::Array<mlc::String> parameter_names, ast::Span fallback_span, int argument_index, int name_index, mlc::Array<std::shared_ptr<ast::Expr>> output) noexcept;

mlc::Array<std::shared_ptr<ast::Expr>> partial_replace_call_arguments(mlc::Array<std::shared_ptr<ast::Expr>> arguments, mlc::Array<mlc::String> parameter_names, ast::Span fallback_span) noexcept;

partial_application_desugar::PartialPair partial_wrap_call_after_children(std::shared_ptr<ast::Expr> callee, mlc::Array<std::shared_ptr<ast::Expr>> mapped_arguments, ast::Span source_span, int serial) noexcept;

partial_application_desugar::PartialPair partial_wrap_method_after_children(std::shared_ptr<ast::Expr> object_expression, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> mapped_arguments, ast::Span source_span, int serial) noexcept;

partial_application_desugar::ArgumentsPartialPair partial_application_map_arguments_at(mlc::Array<std::shared_ptr<ast::Expr>> arguments, int index, int serial, mlc::Array<std::shared_ptr<ast::Expr>> mapped) noexcept;

partial_application_desugar::ArgumentsPartialPair partial_application_map_arguments(mlc::Array<std::shared_ptr<ast::Expr>> arguments, int serial) noexcept;

partial_application_desugar::RecordLitPartsPartialPair partial_application_desugar_record_lit_parts(mlc::Array<ast::RecordLitPart> parts, int serial) noexcept;

partial_application_desugar::StatementsPartialPair partial_application_desugar_statements_block(mlc::Array<std::shared_ptr<ast::Stmt>> statements, int serial) noexcept;

partial_application_desugar::StmtPartialPair partial_application_desugar_statement(std::shared_ptr<ast::Stmt> statement, int serial) noexcept;

partial_application_desugar::MatchArmsPartialPair partial_application_desugar_match_arms_list(mlc::Array<std::shared_ptr<ast::MatchArm>> arms, int serial) noexcept;

partial_application_desugar::PartialPair partial_application_desugar_inner(std::shared_ptr<ast::Expr> expression, int serial) noexcept;

std::shared_ptr<ast::Expr> partial_application_desugar_expr(std::shared_ptr<ast::Expr> expression) noexcept;

partial_application_desugar::PartialPair partial_application_leaf_partial_pair(std::shared_ptr<ast::Expr> leaf_expression, int serial) noexcept{return partial_application_desugar::PartialPair{leaf_expression, serial};}

bool expr_is_placeholder(std::shared_ptr<ast::Expr> expression) noexcept{return [&]() { if (std::holds_alternative<ast::ExprIdent>((*expression)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*expression)._); auto [name, _w0] = _v_exprident; return name == mlc::String("_"); } return false; }();}

mlc::Array<std::shared_ptr<ast::Expr>> partial_array_append_expression(mlc::Array<std::shared_ptr<ast::Expr>> elements, std::shared_ptr<ast::Expr> element) noexcept{
elements.push_back(element);
return elements;
}

int partial_placeholder_delta_for_argument(std::shared_ptr<ast::Expr> argument) noexcept{return [&]() { if (std::holds_alternative<ast::ExprNamedArg>((*argument)._)) { auto _v_exprnamedarg = std::get<ast::ExprNamedArg>((*argument)._); auto [_w0, inner, _w1] = _v_exprnamedarg; return expr_is_placeholder(inner) ? 1 : 0; } if (std::holds_alternative<ast::ExprIdent>((*argument)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*argument)._); auto [name, _w0] = _v_exprident; return name == mlc::String("_") ? 1 : 0; } return 0; }();}

partial_application_desugar::PartialNamesAllocation partial_allocate_parameter_names(int hole_count, int serial_start) noexcept{
mlc::Array<mlc::String> names = {};
int runner = serial_start;
int counter = 0;
while (counter < hole_count){
{
names.push_back(mlc::String("partial_application_parameter_") + mlc::to_string(runner));
runner = runner + 1;
counter = counter + 1;
}
}
return partial_application_desugar::PartialNamesAllocation{names, runner};
}

int partial_count_placeholder_arguments(mlc::Array<std::shared_ptr<ast::Expr>> arguments) noexcept{
int total = 0;
int argument_index = 0;
while (argument_index < arguments.size()){
{
total = total + partial_placeholder_delta_for_argument(arguments[argument_index]);
argument_index = argument_index + 1;
}
}
return total;
}

mlc::Array<std::shared_ptr<ast::Expr>> partial_replace_call_arguments_inner(mlc::Array<std::shared_ptr<ast::Expr>> arguments, mlc::Array<mlc::String> parameter_names, ast::Span fallback_span, int argument_index, int name_index, mlc::Array<std::shared_ptr<ast::Expr>> output) noexcept{return argument_index >= arguments.size() ? output : [&]() -> mlc::Array<std::shared_ptr<ast::Expr>> { if (std::holds_alternative<ast::ExprNamedArg>((*arguments[argument_index])._)) { auto _v_exprnamedarg = std::get<ast::ExprNamedArg>((*arguments[argument_index])._); auto [label, inner, span_named] = _v_exprnamedarg; return expr_is_placeholder(inner) ? partial_replace_call_arguments_inner(arguments, parameter_names, fallback_span, argument_index + 1, name_index + 1, partial_array_append_expression(output, std::make_shared<ast::Expr>(ast::ExprNamedArg(label, std::make_shared<ast::Expr>(ast::ExprIdent(parameter_names[name_index], fallback_span)), span_named)))) : partial_replace_call_arguments_inner(arguments, parameter_names, fallback_span, argument_index + 1, name_index, partial_array_append_expression(output, arguments[argument_index])); } if (std::holds_alternative<ast::ExprIdent>((*arguments[argument_index])._)) { auto _v_exprident = std::get<ast::ExprIdent>((*arguments[argument_index])._); auto [name, _w0] = _v_exprident; return name == mlc::String("_") ? partial_replace_call_arguments_inner(arguments, parameter_names, fallback_span, argument_index + 1, name_index + 1, partial_array_append_expression(output, std::make_shared<ast::Expr>(ast::ExprIdent(parameter_names[name_index], fallback_span)))) : partial_replace_call_arguments_inner(arguments, parameter_names, fallback_span, argument_index + 1, name_index, partial_array_append_expression(output, arguments[argument_index])); } return partial_replace_call_arguments_inner(arguments, parameter_names, fallback_span, argument_index + 1, name_index, partial_array_append_expression(output, arguments[argument_index])); }();}

mlc::Array<std::shared_ptr<ast::Expr>> partial_replace_call_arguments(mlc::Array<std::shared_ptr<ast::Expr>> arguments, mlc::Array<mlc::String> parameter_names, ast::Span fallback_span) noexcept{return partial_replace_call_arguments_inner(arguments, parameter_names, fallback_span, 0, 0, {});}

partial_application_desugar::PartialPair partial_wrap_call_after_children(std::shared_ptr<ast::Expr> callee, mlc::Array<std::shared_ptr<ast::Expr>> mapped_arguments, ast::Span source_span, int serial) noexcept{
int hole_count = partial_count_placeholder_arguments(mapped_arguments);
return hole_count == 0 ? partial_application_desugar::PartialPair{std::make_shared<ast::Expr>(ast::ExprCall(callee, mapped_arguments, source_span)), serial} : [&]() -> partial_application_desugar::PartialPair { 
  partial_application_desugar::PartialNamesAllocation allocation = partial_allocate_parameter_names(hole_count, serial);
  mlc::Array<std::shared_ptr<ast::Expr>> replaced = partial_replace_call_arguments(mapped_arguments, allocation.names, source_span);
  std::shared_ptr<ast::Expr> inner_call = std::make_shared<ast::Expr>(ast::ExprCall(callee, replaced, source_span));
  return partial_application_desugar::PartialPair{std::make_shared<ast::Expr>(ast::ExprLambda(allocation.names, inner_call, source_span)), allocation.next_serial};
 }();
}

partial_application_desugar::PartialPair partial_wrap_method_after_children(std::shared_ptr<ast::Expr> object_expression, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> mapped_arguments, ast::Span source_span, int serial) noexcept{
int hole_count = partial_count_placeholder_arguments(mapped_arguments);
return hole_count == 0 ? partial_application_desugar::PartialPair{std::make_shared<ast::Expr>(ast::ExprMethod(object_expression, method_name, mapped_arguments, source_span)), serial} : [&]() -> partial_application_desugar::PartialPair { 
  partial_application_desugar::PartialNamesAllocation allocation = partial_allocate_parameter_names(hole_count, serial);
  mlc::Array<std::shared_ptr<ast::Expr>> replaced = partial_replace_call_arguments(mapped_arguments, allocation.names, source_span);
  std::shared_ptr<ast::Expr> inner_call = std::make_shared<ast::Expr>(ast::ExprMethod(object_expression, method_name, replaced, source_span));
  return partial_application_desugar::PartialPair{std::make_shared<ast::Expr>(ast::ExprLambda(allocation.names, inner_call, source_span)), allocation.next_serial};
 }();
}

partial_application_desugar::ArgumentsPartialPair partial_application_map_arguments_at(mlc::Array<std::shared_ptr<ast::Expr>> arguments, int index, int serial, mlc::Array<std::shared_ptr<ast::Expr>> mapped) noexcept{return index >= arguments.size() ? partial_application_desugar::ArgumentsPartialPair{mapped, serial} : [&]() -> partial_application_desugar::ArgumentsPartialPair { if (std::holds_alternative<ast::ExprNamedArg>((*arguments[index])._)) { auto _v_exprnamedarg = std::get<ast::ExprNamedArg>((*arguments[index])._); auto [label, inner, span_named] = _v_exprnamedarg; return [&]() -> partial_application_desugar::ArgumentsPartialPair { 
  partial_application_desugar::PartialPair inner_pair = partial_application_desugar_inner(inner, serial);
  return partial_application_map_arguments_at(arguments, index + 1, inner_pair.next_serial, partial_array_append_expression(mapped, std::make_shared<ast::Expr>(ast::ExprNamedArg(label, inner_pair.expression, span_named))));
 }(); } return [&]() -> partial_application_desugar::ArgumentsPartialPair { 
  partial_application_desugar::PartialPair argument_pair = partial_application_desugar_inner(arguments[index], serial);
  return partial_application_map_arguments_at(arguments, index + 1, argument_pair.next_serial, partial_array_append_expression(mapped, argument_pair.expression));
 }(); }();}

partial_application_desugar::ArgumentsPartialPair partial_application_map_arguments(mlc::Array<std::shared_ptr<ast::Expr>> arguments, int serial) noexcept{return partial_application_map_arguments_at(arguments, 0, serial, {});}

partial_application_desugar::RecordLitPartsPartialPair partial_application_desugar_record_lit_parts(mlc::Array<ast::RecordLitPart> parts, int serial) noexcept{
mlc::Array<ast::RecordLitPart> output_parts = {};
int next_serial = serial;
int index = 0;
while (index < parts.size()){
{
std::visit(overloaded{
  [&](const RecordLitFields& recordlitfields) -> mlc::Array<ast::RecordLitPart> { auto [field_values] = recordlitfields; return [&]() -> mlc::Array<ast::RecordLitPart> { 
  mlc::Array<std::shared_ptr<ast::FieldVal>> mapped_fields = {};
  int field_index = 0;
  while (field_index < field_values.size()){
{
partial_application_desugar::PartialPair value_pair = partial_application_desugar_inner(field_values[field_index]->val, next_serial);
mapped_fields.push_back(std::make_shared<ast::FieldVal>(ast::FieldVal{field_values[field_index]->name, value_pair.expression}));
next_serial = value_pair.next_serial;
field_index = field_index + 1;
}
}
  output_parts.push_back(ast::RecordLitPart(ast::RecordLitFields(mapped_fields)));
  return output_parts;
 }(); },
  [&](const RecordLitSpread& recordlitspread) -> mlc::Array<ast::RecordLitPart> { auto [spread_expression] = recordlitspread; return [&]() -> mlc::Array<ast::RecordLitPart> { 
  partial_application_desugar::PartialPair spread_pair = partial_application_desugar_inner(spread_expression, next_serial);
  output_parts.push_back(ast::RecordLitPart(ast::RecordLitSpread(spread_pair.expression)));
  next_serial = spread_pair.next_serial;
  return output_parts;
 }(); }
}, parts[index]._);
index = index + 1;
}
}
return partial_application_desugar::RecordLitPartsPartialPair{output_parts, next_serial};
}

partial_application_desugar::StatementsPartialPair partial_application_desugar_statements_block(mlc::Array<std::shared_ptr<ast::Stmt>> statements, int serial) noexcept{
mlc::Array<std::shared_ptr<ast::Stmt>> output = {};
int next_serial = serial;
int index = 0;
while (index < statements.size()){
{
partial_application_desugar::StmtPartialPair statement_pair = partial_application_desugar_statement(statements[index], next_serial);
output.push_back(statement_pair.statement);
next_serial = statement_pair.next_serial;
index = index + 1;
}
}
return partial_application_desugar::StatementsPartialPair{output, next_serial};
}

partial_application_desugar::StmtPartialPair partial_application_desugar_statement(std::shared_ptr<ast::Stmt> statement, int serial) noexcept{return std::visit(overloaded{
  [&](const StmtLet& stmtlet) -> partial_application_desugar::StmtPartialPair { auto [name, is_mut, typ, value_expression, span_statement] = stmtlet; return [&]() -> partial_application_desugar::StmtPartialPair { 
  partial_application_desugar::PartialPair value_pair = partial_application_desugar_inner(value_expression, serial);
  return partial_application_desugar::StmtPartialPair{std::make_shared<ast::Stmt>(ast::StmtLet(name, is_mut, typ, value_pair.expression, span_statement)), value_pair.next_serial};
 }(); },
  [&](const StmtLetPat& stmtletpat) -> partial_application_desugar::StmtPartialPair { auto [pat, is_mut, typ, value_expression, has_else, else_body, span_statement] = stmtletpat; return [&]() -> partial_application_desugar::StmtPartialPair { 
  partial_application_desugar::PartialPair value_pair = partial_application_desugar_inner(value_expression, serial);
  partial_application_desugar::PartialPair else_pair = partial_application_desugar_inner(else_body, value_pair.next_serial);
  return partial_application_desugar::StmtPartialPair{std::make_shared<ast::Stmt>(ast::StmtLetPat(pat, is_mut, typ, value_pair.expression, has_else, else_pair.expression, span_statement)), else_pair.next_serial};
 }(); },
  [&](const StmtLetConst& stmtletconst) -> partial_application_desugar::StmtPartialPair { auto [name, typ, value_expression, span_statement] = stmtletconst; return [&]() -> partial_application_desugar::StmtPartialPair { 
  partial_application_desugar::PartialPair value_pair = partial_application_desugar_inner(value_expression, serial);
  return partial_application_desugar::StmtPartialPair{std::make_shared<ast::Stmt>(ast::StmtLetConst(name, typ, value_pair.expression, span_statement)), value_pair.next_serial};
 }(); },
  [&](const StmtExpr& stmtexpr) -> partial_application_desugar::StmtPartialPair { auto [inner_expression, span_statement] = stmtexpr; return [&]() -> partial_application_desugar::StmtPartialPair { 
  partial_application_desugar::PartialPair inner_pair = partial_application_desugar_inner(inner_expression, serial);
  return partial_application_desugar::StmtPartialPair{std::make_shared<ast::Stmt>(ast::StmtExpr(inner_pair.expression, span_statement)), inner_pair.next_serial};
 }(); },
  [&](const StmtReturn& stmtreturn) -> partial_application_desugar::StmtPartialPair { auto [inner_expression, span_statement] = stmtreturn; return [&]() -> partial_application_desugar::StmtPartialPair { 
  partial_application_desugar::PartialPair inner_pair = partial_application_desugar_inner(inner_expression, serial);
  return partial_application_desugar::StmtPartialPair{std::make_shared<ast::Stmt>(ast::StmtReturn(inner_pair.expression, span_statement)), inner_pair.next_serial};
 }(); },
  [&](const StmtBreak& stmtbreak) -> partial_application_desugar::StmtPartialPair { auto [_w0] = stmtbreak; return partial_application_desugar::StmtPartialPair{statement, serial}; },
  [&](const StmtContinue& stmtcontinue) -> partial_application_desugar::StmtPartialPair { auto [_w0] = stmtcontinue; return partial_application_desugar::StmtPartialPair{statement, serial}; }
}, (*statement)._);}

partial_application_desugar::MatchArmsPartialPair partial_application_desugar_match_arms_list(mlc::Array<std::shared_ptr<ast::MatchArm>> arms, int serial) noexcept{
mlc::Array<std::shared_ptr<ast::MatchArm>> output_arms = {};
int next_serial = serial;
int index = 0;
while (index < arms.size()){
{
partial_application_desugar::PartialPair guard_pair = partial_application_desugar_inner(arms[index]->when_condition, next_serial);
partial_application_desugar::PartialPair body_pair = partial_application_desugar_inner(arms[index]->body, guard_pair.next_serial);
output_arms.push_back(std::make_shared<ast::MatchArm>(ast::MatchArm{arms[index]->pat, arms[index]->has_guard, guard_pair.expression, body_pair.expression}));
next_serial = body_pair.next_serial;
index = index + 1;
}
}
return partial_application_desugar::MatchArmsPartialPair{output_arms, next_serial};
}

partial_application_desugar::PartialPair partial_application_desugar_inner(std::shared_ptr<ast::Expr> expression, int serial) noexcept{return std::visit(overloaded{
  [&](const ExprInt& exprint) -> partial_application_desugar::PartialPair { auto [_w0, _w1] = exprint; return partial_application_leaf_partial_pair(expression, serial); },
  [&](const ExprStr& exprstr) -> partial_application_desugar::PartialPair { auto [_w0, _w1] = exprstr; return partial_application_leaf_partial_pair(expression, serial); },
  [&](const ExprBool& exprbool) -> partial_application_desugar::PartialPair { auto [_w0, _w1] = exprbool; return partial_application_leaf_partial_pair(expression, serial); },
  [&](const ExprUnit& exprunit) -> partial_application_desugar::PartialPair { auto [_w0] = exprunit; return partial_application_leaf_partial_pair(expression, serial); },
  [&](const ExprFloat& exprfloat) -> partial_application_desugar::PartialPair { auto [_w0, _w1] = exprfloat; return partial_application_leaf_partial_pair(expression, serial); },
  [&](const ExprI64& expri64) -> partial_application_desugar::PartialPair { auto [_w0, _w1] = expri64; return partial_application_leaf_partial_pair(expression, serial); },
  [&](const ExprU8& expru8) -> partial_application_desugar::PartialPair { auto [_w0, _w1] = expru8; return partial_application_leaf_partial_pair(expression, serial); },
  [&](const ExprUsize& exprusize) -> partial_application_desugar::PartialPair { auto [_w0, _w1] = exprusize; return partial_application_leaf_partial_pair(expression, serial); },
  [&](const ExprChar& exprchar) -> partial_application_desugar::PartialPair { auto [_w0, _w1] = exprchar; return partial_application_leaf_partial_pair(expression, serial); },
  [&](const ExprExtern& exprextern) -> partial_application_desugar::PartialPair { auto [_w0] = exprextern; return partial_application_leaf_partial_pair(expression, serial); },
  [&](const ExprIdent& exprident) -> partial_application_desugar::PartialPair { auto [_w0, _w1] = exprident; return partial_application_leaf_partial_pair(expression, serial); },
  [&](const ExprBin& exprbin) -> partial_application_desugar::PartialPair { auto [binary_operation, left, right, span_binary] = exprbin; return [&]() -> partial_application_desugar::PartialPair { 
  partial_application_desugar::PartialPair left_pair = partial_application_desugar_inner(left, serial);
  partial_application_desugar::PartialPair right_pair = partial_application_desugar_inner(right, left_pair.next_serial);
  return partial_application_desugar::PartialPair{std::make_shared<ast::Expr>(ast::ExprBin(binary_operation, left_pair.expression, right_pair.expression, span_binary)), right_pair.next_serial};
 }(); },
  [&](const ExprUn& exprun) -> partial_application_desugar::PartialPair { auto [unary_operation, inner_unary, span_unary] = exprun; return [&]() -> partial_application_desugar::PartialPair { 
  partial_application_desugar::PartialPair inner_pair = partial_application_desugar_inner(inner_unary, serial);
  return partial_application_desugar::PartialPair{std::make_shared<ast::Expr>(ast::ExprUn(unary_operation, inner_pair.expression, span_unary)), inner_pair.next_serial};
 }(); },
  [&](const ExprCall& exprcall) -> partial_application_desugar::PartialPair { auto [function, call_arguments, span_call] = exprcall; return [&]() -> partial_application_desugar::PartialPair { 
  partial_application_desugar::PartialPair callee_pair = partial_application_desugar_inner(function, serial);
  partial_application_desugar::ArgumentsPartialPair arguments_pair = partial_application_map_arguments(call_arguments, callee_pair.next_serial);
  return partial_wrap_call_after_children(callee_pair.expression, arguments_pair.arguments, span_call, arguments_pair.next_serial);
 }(); },
  [&](const ExprMethod& exprmethod) -> partial_application_desugar::PartialPair { auto [object_expression, method_name, method_arguments, span_method] = exprmethod; return [&]() -> partial_application_desugar::PartialPair { 
  partial_application_desugar::PartialPair object_pair = partial_application_desugar_inner(object_expression, serial);
  partial_application_desugar::ArgumentsPartialPair arguments_pair = partial_application_map_arguments(method_arguments, object_pair.next_serial);
  return partial_wrap_method_after_children(object_pair.expression, method_name, arguments_pair.arguments, span_method, arguments_pair.next_serial);
 }(); },
  [&](const ExprField& exprfield) -> partial_application_desugar::PartialPair { auto [object_expression, field_name, span_field] = exprfield; return [&]() -> partial_application_desugar::PartialPair { 
  partial_application_desugar::PartialPair object_pair = partial_application_desugar_inner(object_expression, serial);
  return partial_application_desugar::PartialPair{std::make_shared<ast::Expr>(ast::ExprField(object_pair.expression, field_name, span_field)), object_pair.next_serial};
 }(); },
  [&](const ExprIndex& exprindex) -> partial_application_desugar::PartialPair { auto [object_expression, index_expression, span_index] = exprindex; return [&]() -> partial_application_desugar::PartialPair { 
  partial_application_desugar::PartialPair object_pair = partial_application_desugar_inner(object_expression, serial);
  partial_application_desugar::PartialPair index_pair = partial_application_desugar_inner(index_expression, object_pair.next_serial);
  return partial_application_desugar::PartialPair{std::make_shared<ast::Expr>(ast::ExprIndex(object_pair.expression, index_pair.expression, span_index)), index_pair.next_serial};
 }(); },
  [&](const ExprIf& exprif) -> partial_application_desugar::PartialPair { auto [condition, then_branch, else_branch, span_conditional] = exprif; return [&]() -> partial_application_desugar::PartialPair { 
  partial_application_desugar::PartialPair condition_pair = partial_application_desugar_inner(condition, serial);
  partial_application_desugar::PartialPair then_pair = partial_application_desugar_inner(then_branch, condition_pair.next_serial);
  partial_application_desugar::PartialPair else_pair = partial_application_desugar_inner(else_branch, then_pair.next_serial);
  return partial_application_desugar::PartialPair{std::make_shared<ast::Expr>(ast::ExprIf(condition_pair.expression, then_pair.expression, else_pair.expression, span_conditional)), else_pair.next_serial};
 }(); },
  [&](const ExprBlock& exprblock) -> partial_application_desugar::PartialPair { auto [statements, result_expression, span_block] = exprblock; return [&]() -> partial_application_desugar::PartialPair { 
  partial_application_desugar::StatementsPartialPair stmts_pair = partial_application_desugar_statements_block(statements, serial);
  partial_application_desugar::PartialPair result_pair = partial_application_desugar_inner(result_expression, stmts_pair.next_serial);
  return partial_application_desugar::PartialPair{std::make_shared<ast::Expr>(ast::ExprBlock(stmts_pair.statements, result_pair.expression, span_block)), result_pair.next_serial};
 }(); },
  [&](const ExprWhile& exprwhile) -> partial_application_desugar::PartialPair { auto [condition, statements, span_while] = exprwhile; return [&]() -> partial_application_desugar::PartialPair { 
  partial_application_desugar::PartialPair condition_pair = partial_application_desugar_inner(condition, serial);
  partial_application_desugar::StatementsPartialPair stmts_pair = partial_application_desugar_statements_block(statements, condition_pair.next_serial);
  return partial_application_desugar::PartialPair{std::make_shared<ast::Expr>(ast::ExprWhile(condition_pair.expression, stmts_pair.statements, span_while)), stmts_pair.next_serial};
 }(); },
  [&](const ExprFor& exprfor) -> partial_application_desugar::PartialPair { auto [variable_name, iterator_expression, statements, span_for] = exprfor; return [&]() -> partial_application_desugar::PartialPair { 
  partial_application_desugar::PartialPair iterator_pair = partial_application_desugar_inner(iterator_expression, serial);
  partial_application_desugar::StatementsPartialPair stmts_pair = partial_application_desugar_statements_block(statements, iterator_pair.next_serial);
  return partial_application_desugar::PartialPair{std::make_shared<ast::Expr>(ast::ExprFor(variable_name, iterator_pair.expression, stmts_pair.statements, span_for)), stmts_pair.next_serial};
 }(); },
  [&](const ExprMatch& exprmatch) -> partial_application_desugar::PartialPair { auto [subject, arms, span_match] = exprmatch; return [&]() -> partial_application_desugar::PartialPair { 
  partial_application_desugar::PartialPair subject_pair = partial_application_desugar_inner(subject, serial);
  partial_application_desugar::MatchArmsPartialPair arms_pair = partial_application_desugar_match_arms_list(arms, subject_pair.next_serial);
  return partial_application_desugar::PartialPair{std::make_shared<ast::Expr>(ast::ExprMatch(subject_pair.expression, arms_pair.arms, span_match)), arms_pair.next_serial};
 }(); },
  [&](const ExprRecord& exprrecord) -> partial_application_desugar::PartialPair { auto [type_name, lit_parts, span_record] = exprrecord; return [&]() -> partial_application_desugar::PartialPair { 
  partial_application_desugar::RecordLitPartsPartialPair parts_pair = partial_application_desugar_record_lit_parts(lit_parts, serial);
  return partial_application_desugar::PartialPair{std::make_shared<ast::Expr>(ast::ExprRecord(type_name, parts_pair.parts, span_record)), parts_pair.next_serial};
 }(); },
  [&](const ExprRecordUpdate& exprrecordupdate) -> partial_application_desugar::PartialPair { auto [type_name, base_expression, field_values, span_update] = exprrecordupdate; return [&]() -> partial_application_desugar::PartialPair { 
  partial_application_desugar::PartialPair base_pair = partial_application_desugar_inner(base_expression, serial);
  int next_serial = base_pair.next_serial;
  mlc::Array<std::shared_ptr<ast::FieldVal>> mapped_fields = {};
  int field_index = 0;
  while (field_index < field_values.size()){
{
partial_application_desugar::PartialPair value_pair = partial_application_desugar_inner(field_values[field_index]->val, next_serial);
mapped_fields.push_back(std::make_shared<ast::FieldVal>(ast::FieldVal{field_values[field_index]->name, value_pair.expression}));
next_serial = value_pair.next_serial;
field_index = field_index + 1;
}
}
  return partial_application_desugar::PartialPair{std::make_shared<ast::Expr>(ast::ExprRecordUpdate(type_name, base_pair.expression, mapped_fields, span_update)), next_serial};
 }(); },
  [&](const ExprArray& exprarray) -> partial_application_desugar::PartialPair { auto [elements, span_array] = exprarray; return [&]() -> partial_application_desugar::PartialPair { 
  mlc::Array<std::shared_ptr<ast::Expr>> mapped_elements = {};
  int next_serial = serial;
  int element_index = 0;
  while (element_index < elements.size()){
{
partial_application_desugar::PartialPair element_pair = partial_application_desugar_inner(elements[element_index], next_serial);
mapped_elements.push_back(element_pair.expression);
next_serial = element_pair.next_serial;
element_index = element_index + 1;
}
}
  return partial_application_desugar::PartialPair{std::make_shared<ast::Expr>(ast::ExprArray(mapped_elements, span_array)), next_serial};
 }(); },
  [&](const ExprTuple& exprtuple) -> partial_application_desugar::PartialPair { auto [elements, span_tuple] = exprtuple; return [&]() -> partial_application_desugar::PartialPair { 
  mlc::Array<std::shared_ptr<ast::Expr>> mapped_elements = {};
  int next_serial = serial;
  int element_index = 0;
  while (element_index < elements.size()){
{
partial_application_desugar::PartialPair element_pair = partial_application_desugar_inner(elements[element_index], next_serial);
mapped_elements.push_back(element_pair.expression);
next_serial = element_pair.next_serial;
element_index = element_index + 1;
}
}
  return partial_application_desugar::PartialPair{std::make_shared<ast::Expr>(ast::ExprTuple(mapped_elements, span_tuple)), next_serial};
 }(); },
  [&](const ExprQuestion& exprquestion) -> partial_application_desugar::PartialPair { auto [inner_question, span_question] = exprquestion; return [&]() -> partial_application_desugar::PartialPair { 
  partial_application_desugar::PartialPair inner_pair = partial_application_desugar_inner(inner_question, serial);
  return partial_application_desugar::PartialPair{std::make_shared<ast::Expr>(ast::ExprQuestion(inner_pair.expression, span_question)), inner_pair.next_serial};
 }(); },
  [&](const ExprLambda& exprlambda) -> partial_application_desugar::PartialPair { auto [parameter_names, body, span_lambda] = exprlambda; return [&]() -> partial_application_desugar::PartialPair { 
  partial_application_desugar::PartialPair body_pair = partial_application_desugar_inner(body, serial);
  return partial_application_desugar::PartialPair{std::make_shared<ast::Expr>(ast::ExprLambda(parameter_names, body_pair.expression, span_lambda)), body_pair.next_serial};
 }(); },
  [&](const ExprNamedArg& exprnamedarg) -> partial_application_desugar::PartialPair { auto [label, inner_named, span_named_arg] = exprnamedarg; return [&]() -> partial_application_desugar::PartialPair { 
  partial_application_desugar::PartialPair inner_pair = partial_application_desugar_inner(inner_named, serial);
  return partial_application_desugar::PartialPair{std::make_shared<ast::Expr>(ast::ExprNamedArg(label, inner_pair.expression, span_named_arg)), inner_pair.next_serial};
 }(); },
  [&](const ExprWith& exprwith) -> partial_application_desugar::PartialPair { auto [resource_expression, binder_name, statements, span_with] = exprwith; return [&]() -> partial_application_desugar::PartialPair { 
  partial_application_desugar::PartialPair resource_pair = partial_application_desugar_inner(resource_expression, serial);
  partial_application_desugar::StatementsPartialPair stmts_pair = partial_application_desugar_statements_block(statements, resource_pair.next_serial);
  return partial_application_desugar::PartialPair{std::make_shared<ast::Expr>(ast::ExprWith(resource_pair.expression, binder_name, stmts_pair.statements, span_with)), stmts_pair.next_serial};
 }(); }
}, (*expression)._);}

std::shared_ptr<ast::Expr> partial_application_desugar_expr(std::shared_ptr<ast::Expr> expression) noexcept{return partial_application_desugar_inner(expression, 0).expression;}

} // namespace partial_application_desugar
