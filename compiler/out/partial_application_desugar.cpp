#define main mlc_user_main
#include "partial_application_desugar.hpp"

#include "ast.hpp"

namespace partial_application_desugar {

using namespace ast;
using namespace ast_tokens;

PartialPair partial_application_leaf_partial_pair(std::shared_ptr<ast::Expr> leaf_expression, int serial) noexcept{
  return PartialPair{leaf_expression, serial};
}
bool expr_is_placeholder(std::shared_ptr<ast::Expr> expression) noexcept{
  return [&]() -> bool {
auto __match_subject = expression;
if (std::holds_alternative<ast::ExprIdent>((*__match_subject))) {
const ast::ExprIdent& exprIdent = std::get<ast::ExprIdent>((*__match_subject));
auto [name, __1] = exprIdent; return (name == mlc::String("_", 1));
}
return false;
std::abort();
}();
}
bool expr_is_named_arg_with_placeholder(std::shared_ptr<ast::Expr> argument) noexcept{
  return [&]() -> bool {
auto __match_subject = argument;
if (std::holds_alternative<ast::ExprNamedArg>((*__match_subject))) {
const ast::ExprNamedArg& exprNamedArg = std::get<ast::ExprNamedArg>((*__match_subject));
auto [__0, inner_expression, __2] = exprNamedArg; return expr_is_placeholder(inner_expression);
}
return false;
std::abort();
}();
}
int partial_placeholder_delta_for_argument(std::shared_ptr<ast::Expr> argument) noexcept{
  if (expr_is_named_arg_with_placeholder(argument))   {
    return 1;
  }
  if (expr_is_placeholder(argument))   {
    return 1;
  }
  return 0;
}
PartialNamesAllocation partial_allocate_parameter_names(int hole_count, int serial_start) noexcept{
  return partial_allocate_parameter_names_loop([&]() {
auto empty_names = mlc::Array<mlc::String>{};
return empty_names;
}(), serial_start, hole_count);
}
PartialNamesAllocation partial_allocate_parameter_names_loop(mlc::Array<mlc::String> names, int serial, int remaining) noexcept{
  if ((remaining <= 0))   {
    return PartialNamesAllocation{names, serial};
  } else   {
    return partial_allocate_parameter_names_loop(names.concat(mlc::Array<mlc::String>{(mlc::String("partial_application_parameter_", 30) + mlc::to_string(serial))}), (serial + 1), (remaining - 1));
  }
}
int partial_count_placeholder_arguments(mlc::Array<std::shared_ptr<ast::Expr>> arguments) noexcept{
  return arguments.fold(0, [=](int accumulated_total, std::shared_ptr<ast::Expr> argument_under_scan) mutable { return (accumulated_total + partial_placeholder_delta_for_argument(argument_under_scan)); });
}
ExpressionListAccumulator partial_application_desugar_expression_element_sequence(mlc::Array<std::shared_ptr<ast::Expr>> elements, int initial_serial) noexcept{
  return elements.fold(ExpressionListAccumulator{{}, initial_serial}, [=](ExpressionListAccumulator accumulated, std::shared_ptr<ast::Expr> element_under_sequence) mutable { return [&]() {
auto element_partial = partial_application_desugar_inner(element_under_sequence, accumulated.next_serial);
return ExpressionListAccumulator{accumulated.mapped_expressions.concat(mlc::Array<std::shared_ptr<ast::Expr>>{element_partial.expression}), element_partial.next_serial};
}(); });
}
FieldValuesAccumulator partial_application_desugar_field_values_sequence(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, int initial_serial) noexcept{
  return field_values.fold(FieldValuesAccumulator{{}, initial_serial}, [=](FieldValuesAccumulator accumulated, std::shared_ptr<ast::FieldVal> field_value_under_sequence) mutable { return [&]() {
auto value_partial = partial_application_desugar_inner(field_value_under_sequence->value, accumulated.next_serial);
return FieldValuesAccumulator{accumulated.mapped_field_values.concat(mlc::Array<std::shared_ptr<ast::FieldVal>>{std::make_shared<ast::FieldVal>(ast::FieldVal{field_value_under_sequence->name, value_partial.expression})}), value_partial.next_serial};
}(); });
}
mlc::Array<std::shared_ptr<ast::Expr>> partial_replace_call_arguments_inner(mlc::Array<std::shared_ptr<ast::Expr>> arguments, mlc::Array<mlc::String> parameter_names, ast::Span fallback_span, int argument_index, int name_index, mlc::Array<std::shared_ptr<ast::Expr>> output) noexcept{
  if ((argument_index >= arguments.length()))   {
    return output;
  } else   {
    return [&]() -> mlc::Array<std::shared_ptr<ast::Expr>> {
auto __match_subject = arguments[argument_index];
if (std::holds_alternative<ast::ExprNamedArg>((*__match_subject))) {
const ast::ExprNamedArg& exprNamedArg = std::get<ast::ExprNamedArg>((*__match_subject));
auto [label, inner_expression, span_named] = exprNamedArg; return (expr_is_placeholder(inner_expression) ? (partial_replace_call_arguments_inner(arguments, parameter_names, fallback_span, (argument_index + 1), (name_index + 1), output.concat(mlc::Array<std::shared_ptr<ast::Expr>>{std::make_shared<ast::Expr>(ast::ExprNamedArg{label, std::make_shared<ast::Expr>(ast::ExprIdent{parameter_names[name_index], fallback_span}), span_named})}))) : (partial_replace_call_arguments_inner(arguments, parameter_names, fallback_span, (argument_index + 1), name_index, output.concat(mlc::Array<std::shared_ptr<ast::Expr>>{arguments[argument_index]}))));
}
if (std::holds_alternative<ast::ExprIdent>((*__match_subject))) {
const ast::ExprIdent& exprIdent = std::get<ast::ExprIdent>((*__match_subject));
auto [name, __1] = exprIdent; return ((name == mlc::String("_", 1)) ? (partial_replace_call_arguments_inner(arguments, parameter_names, fallback_span, (argument_index + 1), (name_index + 1), output.concat(mlc::Array<std::shared_ptr<ast::Expr>>{std::make_shared<ast::Expr>(ast::ExprIdent{parameter_names[name_index], fallback_span})}))) : (partial_replace_call_arguments_inner(arguments, parameter_names, fallback_span, (argument_index + 1), name_index, output.concat(mlc::Array<std::shared_ptr<ast::Expr>>{arguments[argument_index]}))));
}
return partial_replace_call_arguments_inner(arguments, parameter_names, fallback_span, (argument_index + 1), name_index, output.concat(mlc::Array<std::shared_ptr<ast::Expr>>{arguments[argument_index]}));
std::abort();
}();
  }
}
mlc::Array<std::shared_ptr<ast::Expr>> partial_replace_call_arguments(mlc::Array<std::shared_ptr<ast::Expr>> arguments, mlc::Array<mlc::String> parameter_names, ast::Span fallback_span) noexcept{
  return partial_replace_call_arguments_inner(arguments, parameter_names, fallback_span, 0, 0, mlc::Array<std::shared_ptr<ast::Expr>>{});
}
PartialPair partial_wrap_call_after_children(std::shared_ptr<ast::Expr> callee, mlc::Array<std::shared_ptr<ast::Expr>> mapped_arguments, ast::Span source_span, int serial) noexcept{
  auto hole_count = partial_count_placeholder_arguments(mapped_arguments);
  if ((hole_count == 0))   {
    return PartialPair{std::make_shared<ast::Expr>(ast::ExprCall{callee, mapped_arguments, source_span}), serial};
  } else   {
    auto allocation = partial_allocate_parameter_names(hole_count, serial);
    auto replaced = partial_replace_call_arguments(mapped_arguments, allocation.names, source_span);
    auto inner_call = std::make_shared<ast::Expr>(ast::ExprCall{callee, replaced, source_span});
    return PartialPair{std::make_shared<ast::Expr>(ast::ExprLambda{allocation.names, inner_call, source_span}), allocation.next_serial};
  }
}
PartialPair partial_wrap_method_after_children(std::shared_ptr<ast::Expr> object_expression, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> mapped_arguments, ast::Span source_span, int serial) noexcept{
  auto hole_count = partial_count_placeholder_arguments(mapped_arguments);
  if ((hole_count == 0))   {
    return PartialPair{std::make_shared<ast::Expr>(ast::ExprMethod{object_expression, method_name, mapped_arguments, source_span}), serial};
  } else   {
    auto allocation = partial_allocate_parameter_names(hole_count, serial);
    auto replaced = partial_replace_call_arguments(mapped_arguments, allocation.names, source_span);
    auto inner_call = std::make_shared<ast::Expr>(ast::ExprMethod{object_expression, method_name, replaced, source_span});
    return PartialPair{std::make_shared<ast::Expr>(ast::ExprLambda{allocation.names, inner_call, source_span}), allocation.next_serial};
  }
}
ArgumentsPartialPair partial_application_map_arguments_at(mlc::Array<std::shared_ptr<ast::Expr>> arguments, int index, int serial, mlc::Array<std::shared_ptr<ast::Expr>> mapped) noexcept{
  if ((index >= arguments.length()))   {
    return ArgumentsPartialPair{mapped, serial};
  } else   {
    return [&]() -> ArgumentsPartialPair {
auto __match_subject = arguments[index];
if (std::holds_alternative<ast::ExprNamedArg>((*__match_subject))) {
const ast::ExprNamedArg& exprNamedArg = std::get<ast::ExprNamedArg>((*__match_subject));
auto [label, inner_expression, span_named] = exprNamedArg; return [&]() {
auto inner_pair = partial_application_desugar_inner(inner_expression, serial);
return partial_application_map_arguments_at(arguments, (index + 1), inner_pair.next_serial, mapped.concat(mlc::Array<std::shared_ptr<ast::Expr>>{std::make_shared<ast::Expr>(ast::ExprNamedArg{label, inner_pair.expression, span_named})}));
}();
}
return [&]() {
auto argument_pair = partial_application_desugar_inner(arguments[index], serial);
return partial_application_map_arguments_at(arguments, (index + 1), argument_pair.next_serial, mapped.concat(mlc::Array<std::shared_ptr<ast::Expr>>{argument_pair.expression}));
}();
std::abort();
}();
  }
}
ArgumentsPartialPair partial_application_map_arguments(mlc::Array<std::shared_ptr<ast::Expr>> arguments, int serial) noexcept{
  return partial_application_map_arguments_at(arguments, 0, serial, mlc::Array<std::shared_ptr<ast::Expr>>{});
}
RecordLitPartsFoldState partial_application_desugar_record_lit_part(RecordLitPartsFoldState state, ast::RecordLitPart part_under_scan) noexcept{
  return std::visit(overloaded{[&](const ast::RecordLitFields& recordLitFields) -> RecordLitPartsFoldState { auto [field_values_under_part] = recordLitFields; return [&]() {
auto fields_accumulator = partial_application_desugar_field_values_sequence(field_values_under_part, state.next_serial);
return RecordLitPartsFoldState{state.output_parts.concat(mlc::Array<ast::RecordLitPart>{ast::RecordLitFields{fields_accumulator.mapped_field_values}}), fields_accumulator.next_serial};
}(); },
[&](const ast::RecordLitSpread& recordLitSpread) -> RecordLitPartsFoldState { auto [spread_expression_under_part] = recordLitSpread; return [&]() {
auto spread_partial = partial_application_desugar_inner(spread_expression_under_part, state.next_serial);
return RecordLitPartsFoldState{state.output_parts.concat(mlc::Array<ast::RecordLitPart>{ast::RecordLitSpread{spread_partial.expression}}), spread_partial.next_serial};
}(); }
}, part_under_scan);
}
RecordLitPartsPartialPair partial_application_desugar_record_lit_parts(mlc::Array<ast::RecordLitPart> parts, int serial) noexcept{
  auto final_state = parts.fold(RecordLitPartsFoldState{[&]() {
auto empty_parts = mlc::Array<ast::RecordLitPart>{};
return empty_parts;
}(), serial}, [=](RecordLitPartsFoldState state, ast::RecordLitPart part_under_scan) mutable { return partial_application_desugar_record_lit_part(state, part_under_scan); });
  return RecordLitPartsPartialPair{final_state.output_parts, final_state.next_serial};
}
StatementsPartialPair partial_application_desugar_statements_block(mlc::Array<std::shared_ptr<ast::Stmt>> statements, int serial) noexcept{
  auto block_fold = statements.fold(StatementsBlockAccumulator{{}, serial}, [=](StatementsBlockAccumulator accumulated, std::shared_ptr<ast::Stmt> statement_under_block) mutable { return [&]() {
auto statement_partial = partial_application_desugar_statement(statement_under_block, accumulated.next_serial);
return StatementsBlockAccumulator{accumulated.accumulated_statements.concat(mlc::Array<std::shared_ptr<ast::Stmt>>{statement_partial.statement}), statement_partial.next_serial};
}(); });
  return StatementsPartialPair{block_fold.accumulated_statements, block_fold.next_serial};
}
StmtPartialPair partial_application_desugar_statement(std::shared_ptr<ast::Stmt> statement, int serial) noexcept{
  return std::visit(overloaded{[&](const ast::StmtLet& stmtLet) -> StmtPartialPair { auto [name, is_mut, type_expression, value_expression, span_statement] = stmtLet; return [&]() {
auto value_pair = partial_application_desugar_inner(value_expression, serial);
return StmtPartialPair{std::make_shared<ast::Stmt>(ast::StmtLet{name, is_mut, type_expression, value_pair.expression, span_statement}), value_pair.next_serial};
}(); },
[&](const ast::StmtLetPattern& stmtLetPattern) -> StmtPartialPair { auto [pattern, is_mut, type_expression, value_expression, has_else, else_body, span_statement] = stmtLetPattern; return [&]() {
auto value_pair = partial_application_desugar_inner(value_expression, serial);
auto else_pair = partial_application_desugar_inner(else_body, value_pair.next_serial);
return StmtPartialPair{std::make_shared<ast::Stmt>(ast::StmtLetPattern{pattern, is_mut, type_expression, value_pair.expression, has_else, else_pair.expression, span_statement}), else_pair.next_serial};
}(); },
[&](const ast::StmtLetConst& stmtLetConst) -> StmtPartialPair { auto [name, type_expression, value_expression, span_statement] = stmtLetConst; return [&]() {
auto value_pair = partial_application_desugar_inner(value_expression, serial);
return StmtPartialPair{std::make_shared<ast::Stmt>(ast::StmtLetConst{name, type_expression, value_pair.expression, span_statement}), value_pair.next_serial};
}(); },
[&](const ast::StmtExpr& stmtExpr) -> StmtPartialPair { auto [inner_expression, span_statement] = stmtExpr; return [&]() {
auto inner_pair = partial_application_desugar_inner(inner_expression, serial);
return StmtPartialPair{std::make_shared<ast::Stmt>(ast::StmtExpr{inner_pair.expression, span_statement}), inner_pair.next_serial};
}(); },
[&](const ast::StmtReturn& stmtReturn) -> StmtPartialPair { auto [inner_expression, span_statement] = stmtReturn; return [&]() {
auto inner_pair = partial_application_desugar_inner(inner_expression, serial);
return StmtPartialPair{std::make_shared<ast::Stmt>(ast::StmtReturn{inner_pair.expression, span_statement}), inner_pair.next_serial};
}(); },
[&](const ast::StmtBreak& stmtBreak) -> StmtPartialPair { auto [__0] = stmtBreak; return StmtPartialPair{statement, serial}; },
[&](const ast::StmtContinue& stmtContinue) -> StmtPartialPair { auto [__0] = stmtContinue; return StmtPartialPair{statement, serial}; }
}, (*statement));
}
MatchArmsPartialPair partial_application_desugar_match_arms_list(mlc::Array<std::shared_ptr<ast::MatchArm>> match_arms, int serial) noexcept{
  return match_arms.fold(MatchArmsPartialPair{{}, serial}, [=](MatchArmsPartialPair accumulated, std::shared_ptr<ast::MatchArm> arm_shared_under_match) mutable { return [&]() {
auto guard_pair = partial_application_desugar_inner(arm_shared_under_match->when_condition, accumulated.next_serial);
auto body_pair = partial_application_desugar_inner(arm_shared_under_match->body, guard_pair.next_serial);
return MatchArmsPartialPair{accumulated.match_arms.concat(mlc::Array<std::shared_ptr<ast::MatchArm>>{std::make_shared<ast::MatchArm>(ast::MatchArm{arm_shared_under_match->pattern, arm_shared_under_match->has_guard, guard_pair.expression, body_pair.expression})}), body_pair.next_serial};
}(); });
}
PartialPair partial_application_desugar_inner(std::shared_ptr<ast::Expr> expression, int serial) noexcept{
  return std::visit(overloaded{[&](const ast::ExprInt& exprInt) -> PartialPair { auto [__0, __1] = exprInt; return partial_application_leaf_partial_pair(expression, serial); },
[&](const ast::ExprStr& exprStr) -> PartialPair { auto [__0, __1] = exprStr; return partial_application_leaf_partial_pair(expression, serial); },
[&](const ast::ExprBool& exprBool) -> PartialPair { auto [__0, __1] = exprBool; return partial_application_leaf_partial_pair(expression, serial); },
[&](const ast::ExprUnit& exprUnit) -> PartialPair { auto [__0] = exprUnit; return partial_application_leaf_partial_pair(expression, serial); },
[&](const ast::ExprFloat& exprFloat) -> PartialPair { auto [__0, __1] = exprFloat; return partial_application_leaf_partial_pair(expression, serial); },
[&](const ast::ExprI64& exprI64) -> PartialPair { auto [__0, __1] = exprI64; return partial_application_leaf_partial_pair(expression, serial); },
[&](const ast::ExprU8& exprU8) -> PartialPair { auto [__0, __1] = exprU8; return partial_application_leaf_partial_pair(expression, serial); },
[&](const ast::ExprUsize& exprUsize) -> PartialPair { auto [__0, __1] = exprUsize; return partial_application_leaf_partial_pair(expression, serial); },
[&](const ast::ExprChar& exprChar) -> PartialPair { auto [__0, __1] = exprChar; return partial_application_leaf_partial_pair(expression, serial); },
[&](const ast::ExprExtern& exprExtern) -> PartialPair { auto [__0, __1, __2, __3] = exprExtern; return partial_application_leaf_partial_pair(expression, serial); },
[&](const ast::ExprIdent& exprIdent) -> PartialPair { auto [__0, __1] = exprIdent; return partial_application_leaf_partial_pair(expression, serial); },
[&](const ast::ExprBin& exprBin) -> PartialPair { auto [binary_operation, left, right, span_binary] = exprBin; return [&]() {
auto left_pair = partial_application_desugar_inner(left, serial);
auto right_pair = partial_application_desugar_inner(right, left_pair.next_serial);
return PartialPair{std::make_shared<ast::Expr>(ast::ExprBin{binary_operation, left_pair.expression, right_pair.expression, span_binary}), right_pair.next_serial};
}(); },
[&](const ast::ExprUn& exprUn) -> PartialPair { auto [unary_operation, inner_unary, span_unary] = exprUn; return [&]() {
auto inner_pair = partial_application_desugar_inner(inner_unary, serial);
return PartialPair{std::make_shared<ast::Expr>(ast::ExprUn{unary_operation, inner_pair.expression, span_unary}), inner_pair.next_serial};
}(); },
[&](const ast::ExprCall& exprCall) -> PartialPair { auto [function, call_arguments, span_call] = exprCall; return [&]() {
auto callee_pair = partial_application_desugar_inner(function, serial);
auto arguments_pair = partial_application_map_arguments(call_arguments, callee_pair.next_serial);
return partial_wrap_call_after_children(callee_pair.expression, arguments_pair.arguments, span_call, arguments_pair.next_serial);
}(); },
[&](const ast::ExprMethod& exprMethod) -> PartialPair { auto [object_expression, method_name, method_arguments, span_method] = exprMethod; return [&]() {
auto object_pair = partial_application_desugar_inner(object_expression, serial);
auto arguments_pair = partial_application_map_arguments(method_arguments, object_pair.next_serial);
return partial_wrap_method_after_children(object_pair.expression, method_name, arguments_pair.arguments, span_method, arguments_pair.next_serial);
}(); },
[&](const ast::ExprField& exprField) -> PartialPair { auto [object_expression, field_name, span_field] = exprField; return [&]() {
auto object_pair = partial_application_desugar_inner(object_expression, serial);
return PartialPair{std::make_shared<ast::Expr>(ast::ExprField{object_pair.expression, field_name, span_field}), object_pair.next_serial};
}(); },
[&](const ast::ExprIndex& exprIndex) -> PartialPair { auto [object_expression, index_expression, span_index] = exprIndex; return [&]() {
auto object_pair = partial_application_desugar_inner(object_expression, serial);
auto index_pair = partial_application_desugar_inner(index_expression, object_pair.next_serial);
return PartialPair{std::make_shared<ast::Expr>(ast::ExprIndex{object_pair.expression, index_pair.expression, span_index}), index_pair.next_serial};
}(); },
[&](const ast::ExprIf& exprIf) -> PartialPair { auto [condition, then_branch, else_branch, span_conditional] = exprIf; return [&]() {
auto condition_pair = partial_application_desugar_inner(condition, serial);
auto then_pair = partial_application_desugar_inner(then_branch, condition_pair.next_serial);
auto else_pair = partial_application_desugar_inner(else_branch, then_pair.next_serial);
return PartialPair{std::make_shared<ast::Expr>(ast::ExprIf{condition_pair.expression, then_pair.expression, else_pair.expression, span_conditional}), else_pair.next_serial};
}(); },
[&](const ast::ExprBlock& exprBlock) -> PartialPair { auto [statements, result_expression, span_block] = exprBlock; return [&]() {
auto stmts_pair = partial_application_desugar_statements_block(statements, serial);
auto result_pair = partial_application_desugar_inner(result_expression, stmts_pair.next_serial);
return PartialPair{std::make_shared<ast::Expr>(ast::ExprBlock{stmts_pair.statements, result_pair.expression, span_block}), result_pair.next_serial};
}(); },
[&](const ast::ExprWhile& exprWhile) -> PartialPair { auto [condition, statements, span_while] = exprWhile; return [&]() {
auto condition_pair = partial_application_desugar_inner(condition, serial);
auto stmts_pair = partial_application_desugar_statements_block(statements, condition_pair.next_serial);
return PartialPair{std::make_shared<ast::Expr>(ast::ExprWhile{condition_pair.expression, stmts_pair.statements, span_while}), stmts_pair.next_serial};
}(); },
[&](const ast::ExprSpawn& exprSpawn) -> PartialPair { auto [statements, span_spawn] = exprSpawn; return [&]() {
auto stmts_pair = partial_application_desugar_statements_block(statements, serial);
return PartialPair{std::make_shared<ast::Expr>(ast::ExprSpawn{stmts_pair.statements, span_spawn}), stmts_pair.next_serial};
}(); },
[&](const ast::ExprFor& exprFor) -> PartialPair { auto [variable_name, iterator_expression, statements, span_for] = exprFor; return [&]() {
auto iterator_pair = partial_application_desugar_inner(iterator_expression, serial);
auto stmts_pair = partial_application_desugar_statements_block(statements, iterator_pair.next_serial);
return PartialPair{std::make_shared<ast::Expr>(ast::ExprFor{variable_name, iterator_pair.expression, stmts_pair.statements, span_for}), stmts_pair.next_serial};
}(); },
[&](const ast::ExprMatch& exprMatch) -> PartialPair { auto [subject, match_arms, span_match] = exprMatch; return [&]() {
auto subject_pair = partial_application_desugar_inner(subject, serial);
auto arms_pair = partial_application_desugar_match_arms_list(match_arms, subject_pair.next_serial);
return PartialPair{std::make_shared<ast::Expr>(ast::ExprMatch{subject_pair.expression, arms_pair.match_arms, span_match}), arms_pair.next_serial};
}(); },
[&](const ast::ExprRecord& exprRecord) -> PartialPair { auto [type_name, lit_parts, span_record] = exprRecord; return [&]() {
auto parts_pair = partial_application_desugar_record_lit_parts(lit_parts, serial);
return PartialPair{std::make_shared<ast::Expr>(ast::ExprRecord{type_name, parts_pair.parts, span_record}), parts_pair.next_serial};
}(); },
[&](const ast::ExprRecordUpdate& exprRecordUpdate) -> PartialPair { auto [type_name, base_expression, field_values, span_update] = exprRecordUpdate; return [&]() {
auto base_pair = partial_application_desugar_inner(base_expression, serial);
auto fields_accumulator = partial_application_desugar_field_values_sequence(field_values, base_pair.next_serial);
return PartialPair{std::make_shared<ast::Expr>(ast::ExprRecordUpdate{type_name, base_pair.expression, fields_accumulator.mapped_field_values, span_update}), fields_accumulator.next_serial};
}(); },
[&](const ast::ExprArray& exprArray) -> PartialPair { auto [elements, span_array] = exprArray; return [&]() {
auto element_accumulator = partial_application_desugar_expression_element_sequence(elements, serial);
return PartialPair{std::make_shared<ast::Expr>(ast::ExprArray{element_accumulator.mapped_expressions, span_array}), element_accumulator.next_serial};
}(); },
[&](const ast::ExprTuple& exprTuple) -> PartialPair { auto [elements, span_tuple] = exprTuple; return [&]() {
auto element_accumulator = partial_application_desugar_expression_element_sequence(elements, serial);
return PartialPair{std::make_shared<ast::Expr>(ast::ExprTuple{element_accumulator.mapped_expressions, span_tuple}), element_accumulator.next_serial};
}(); },
[&](const ast::ExprQuestion& exprQuestion) -> PartialPair { auto [inner_question, span_question] = exprQuestion; return [&]() {
auto inner_pair = partial_application_desugar_inner(inner_question, serial);
return PartialPair{std::make_shared<ast::Expr>(ast::ExprQuestion{inner_pair.expression, span_question}), inner_pair.next_serial};
}(); },
[&](const ast::ExprLambda& exprLambda) -> PartialPair { auto [parameter_names, body, span_lambda] = exprLambda; return [&]() {
auto body_pair = partial_application_desugar_inner(body, serial);
return PartialPair{std::make_shared<ast::Expr>(ast::ExprLambda{parameter_names, body_pair.expression, span_lambda}), body_pair.next_serial};
}(); },
[&](const ast::ExprNamedArg& exprNamedArg) -> PartialPair { auto [label, inner_named, span_named_arg] = exprNamedArg; return [&]() {
auto inner_pair = partial_application_desugar_inner(inner_named, serial);
return PartialPair{std::make_shared<ast::Expr>(ast::ExprNamedArg{label, inner_pair.expression, span_named_arg}), inner_pair.next_serial};
}(); },
[&](const ast::ExprWith& exprWith) -> PartialPair { auto [resource_expression, binder_name, statements, span_with] = exprWith; return [&]() {
auto resource_pair = partial_application_desugar_inner(resource_expression, serial);
auto stmts_pair = partial_application_desugar_statements_block(statements, resource_pair.next_serial);
return PartialPair{std::make_shared<ast::Expr>(ast::ExprWith{resource_pair.expression, binder_name, stmts_pair.statements, span_with}), stmts_pair.next_serial};
}(); }
}, (*expression));
}
std::shared_ptr<ast::Expr> partial_application_desugar_expr(std::shared_ptr<ast::Expr> expression) noexcept{
  return partial_application_desugar_inner(expression, 0).expression;
}

} // namespace partial_application_desugar
