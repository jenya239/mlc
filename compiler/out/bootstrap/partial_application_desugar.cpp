#define main mlc_user_main
#include "partial_application_desugar.hpp"

namespace partial_application_desugar {

PartialPair partial_application_leaf_partial_pair(std::shared_ptr<ast::Expr> leaf_expression, int serial) noexcept{
return PartialPair{leaf_expression, serial};
}
bool expr_is_placeholder(std::shared_ptr<ast::Expr> expression) noexcept{
return std::visit(overloaded{[&](const ast::ExprIdent& exprIdent) { auto [name, __1] = exprIdent; return (name == mlc::String("_", 1)); },
[&](const auto& __v) { return false; }
}, (*expression));
}
int partial_placeholder_delta_for_argument(std::shared_ptr<ast::Expr> argument) noexcept{
return std::visit(overloaded{[&](const ast::ExprNamedArg& exprNamedArg) { auto [__0, inner, __2] = exprNamedArg; return (expr_is_placeholder(inner) ? (1) : (0)); },
[&](const ast::ExprIdent& exprIdent) { auto [name, __1] = exprIdent; return ((name == mlc::String("_", 1)) ? (1) : (0)); },
[&](const auto& __v) { return 0; }
}, (*argument));
}
PartialNamesAllocation partial_allocate_parameter_names(int hole_count, int serial_start) noexcept{
if ((hole_count <= 0)) {
return PartialNamesAllocation{{}, serial_start};
} else {
auto tail_allocation = partial_allocate_parameter_names((hole_count - 1), (serial_start + 1));
return PartialNamesAllocation{mlc::Array<mlc::String>{(mlc::String("partial_application_parameter_", 30) + mlc::to_string(serial_start))}.concat(tail_allocation.names), tail_allocation.next_serial};
}
}
int partial_count_placeholder_arguments(mlc::Array<std::shared_ptr<ast::Expr>> arguments) noexcept{
return arguments.fold(0, [=](int accumulated_total, std::shared_ptr<ast::Expr> argument_under_scan) mutable { return (accumulated_total + partial_placeholder_delta_for_argument(argument_under_scan)); });
}
ExpressionListAccumulator partial_application_desugar_expression_element_sequence(mlc::Array<std::shared_ptr<ast::Expr>> elements, int initial_serial) noexcept{
return elements.fold(ExpressionListAccumulator{{}, initial_serial}, [=](ExpressionListAccumulator accumulated, std::shared_ptr<ast::Expr> element_under_sequence) mutable { return [&]() {
auto element_partial = partial_application_desugar_inner(element_under_sequence, accumulated.next_serial);
return ExpressionListAccumulator{accumulated.mapped_expressions.concat(mlc::Array{element_partial.expression}), element_partial.next_serial};
}(); });
}
FieldValuesAccumulator partial_application_desugar_field_values_sequence(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, int initial_serial) noexcept{
return field_values.fold(FieldValuesAccumulator{{}, initial_serial}, [=](FieldValuesAccumulator accumulated, std::shared_ptr<ast::FieldVal> field_value_under_sequence) mutable { return [&]() {
auto value_partial = partial_application_desugar_inner(field_value_under_sequence->val, accumulated.next_serial);
return FieldValuesAccumulator{accumulated.mapped_field_values.concat(mlc::Array{std::make_shared<ast::FieldVal>(ast::FieldVal{field_value_under_sequence->name, value_partial.expression})}), value_partial.next_serial};
}(); });
}
mlc::Array<std::shared_ptr<ast::Expr>> partial_replace_call_arguments_inner(mlc::Array<std::shared_ptr<ast::Expr>> arguments, mlc::Array<mlc::String> parameter_names, ast::Span fallback_span, int argument_index, int name_index, mlc::Array<std::shared_ptr<ast::Expr>> output) noexcept{
if ((argument_index >= arguments.length())) {
return output;
} else {
std::visit(overloaded{[&](const ast::ExprNamedArg& exprNamedArg) { auto [label, inner, span_named] = exprNamedArg; return (expr_is_placeholder(inner) ? (partial_replace_call_arguments_inner(arguments, parameter_names, fallback_span, (argument_index + 1), (name_index + 1), output.concat(mlc::Array{std::make_shared<ast::Expr>(ast::ExprNamedArg{label, std::make_shared<ast::Expr>(ast::ExprIdent{parameter_names[name_index], fallback_span}), span_named})}))) : (partial_replace_call_arguments_inner(arguments, parameter_names, fallback_span, (argument_index + 1), name_index, output.concat(mlc::Array<std::shared_ptr<ast::Expr>>{arguments[argument_index]})))); },
[&](const ast::ExprIdent& exprIdent) { auto [name, __1] = exprIdent; return ((name == mlc::String("_", 1)) ? (partial_replace_call_arguments_inner(arguments, parameter_names, fallback_span, (argument_index + 1), (name_index + 1), output.concat(mlc::Array{std::make_shared<ast::Expr>(ast::ExprIdent{parameter_names[name_index], fallback_span})}))) : (partial_replace_call_arguments_inner(arguments, parameter_names, fallback_span, (argument_index + 1), name_index, output.concat(mlc::Array<std::shared_ptr<ast::Expr>>{arguments[argument_index]})))); },
[&](const auto& __v) { return partial_replace_call_arguments_inner(arguments, parameter_names, fallback_span, (argument_index + 1), name_index, output.concat(mlc::Array<std::shared_ptr<ast::Expr>>{arguments[argument_index]})); }
}, (*arguments[argument_index]));
/* unit */;
partial_replace_call_arguments(mlc::Array<bool>{((Shared{} < ast::Expr{}) > /* unit */), parameter_names}[string], ast::Span{});
/* unit */[((Shared{} < ast::Expr{}) > /* unit */)];
partial_replace_call_arguments_inner(arguments, parameter_names, fallback_span, 0, 0, mlc::Array<std::shared_ptr<ast::Expr>>{});
/* unit */;
partial_wrap_call_after_children(((Shared{} < ast::Expr{}) > /* unit */), mlc::Array<bool>{((Shared{} < ast::Expr{}) > /* unit */), source_span}, ast::Span{}, i32);
/* unit */;
PartialPair{} = [&]() {
auto hole_count = partial_count_placeholder_arguments(mapped_arguments);
return [&]() -> PartialPair {
if ((hole_count == 0)) {
return PartialPair{std::make_shared<ast::Expr>(ast::ExprCall{callee, mapped_arguments, source_span}), serial};
} else {
return [&]() {
auto allocation = partial_allocate_parameter_names(hole_count, serial);
auto replaced = partial_replace_call_arguments(mapped_arguments, allocation.names, source_span);
auto inner_call = std::make_shared<ast::Expr>(ast::ExprCall{callee, replaced, source_span});
return PartialPair{std::make_shared<ast::Expr>(ast::ExprLambda{allocation.names, inner_call, source_span}), allocation.next_serial};
}();
}
}();
}();
/* unit */;
partial_wrap_method_after_children(((Shared{} < ast::Expr{}) > /* unit */), string, mlc::Array<bool>{((Shared{} < ast::Expr{}) > /* unit */), source_span}, ast::Span{}, i32);
/* unit */;
PartialPair{} = [&]() {
auto hole_count = partial_count_placeholder_arguments(mapped_arguments);
return [&]() -> PartialPair {
if ((hole_count == 0)) {
return PartialPair{std::make_shared<ast::Expr>(ast::ExprMethod{object_expression, method_name, mapped_arguments, source_span}), serial};
} else {
return [&]() {
auto allocation = partial_allocate_parameter_names(hole_count, serial);
auto replaced = partial_replace_call_arguments(mapped_arguments, allocation.names, source_span);
auto inner_call = std::make_shared<ast::Expr>(ast::ExprMethod{object_expression, method_name, replaced, source_span});
return PartialPair{std::make_shared<ast::Expr>(ast::ExprLambda{allocation.names, inner_call, source_span}), allocation.next_serial};
}();
}
}();
}();
/* unit */;
return partial_application_map_arguments_at(mlc::Array<bool>{((Shared{} < ast::Expr{}) > /* unit */), index}, i32, i32, mlc::Array<bool>{((Shared{} < ast::Expr{}) > /* unit */)}, /* unit */, ArgumentsPartialPair{}, /* unit */, [&]() -> ArgumentsPartialPair {
if ((index >= arguments.length())) {
return ArgumentsPartialPair{mapped, serial};
} else {
return [&]() {
std::visit(overloaded{[&](const ast::ExprNamedArg& exprNamedArg) { auto [label, inner, span_named] = exprNamedArg; return [&]() {
auto inner_pair = partial_application_desugar_inner(inner, serial);
return partial_application_map_arguments_at(arguments, (index + 1), inner_pair.next_serial, mapped.concat(mlc::Array{std::make_shared<ast::Expr>(ast::ExprNamedArg{label, inner_pair.expression, span_named})}));
}(); },
[&](const auto& __v) { return [&]() {
auto argument_pair = partial_application_desugar_inner(arguments[index], serial);
return partial_application_map_arguments_at(arguments, (index + 1), argument_pair.next_serial, mapped.concat(mlc::Array{argument_pair.expression}));
}(); }
}, (*arguments[index]));
/* unit */;
partial_application_map_arguments(mlc::Array<bool>{((Shared{} < ast::Expr{}) > /* unit */), serial}, i32);
/* unit */;
ArgumentsPartialPair{} = partial_application_map_arguments_at(arguments, 0, serial, {});
/* unit */;
partial_application_desugar_record_lit_parts(mlc::Array{ast::RecordLitPart{}}, i32);
/* unit */;
RecordLitPartsPartialPair{} = [&]() {
auto output_parts = mlc::Array<ast::RecordLitPart>{};
auto next_serial_after_fold = parts.fold(serial, [=](auto next_serial_under_fold, auto part_under_scan) mutable { return std::visit(overloaded{[&](const ast::RecordLitFields& recordLitFields) { auto [field_values_under_part] = recordLitFields; return [&]() {
auto fields_accumulator = partial_application_desugar_field_values_sequence(field_values_under_part, next_serial_under_fold);
output_parts.push_back(ast::RecordLitFields{fields_accumulator.mapped_field_values});
return fields_accumulator.next_serial;
}(); },
[&](const ast::RecordLitSpread& recordLitSpread) { auto [spread_expression_under_part] = recordLitSpread; return [&]() {
auto spread_partial = partial_application_desugar_inner(spread_expression_under_part, next_serial_under_fold);
output_parts.push_back(ast::RecordLitSpread{spread_partial.expression});
return spread_partial.next_serial;
}(); }
}, part_under_scan); });
return RecordLitPartsPartialPair{output_parts, next_serial_after_fold};
}();
/* unit */;
partial_application_desugar_statements_block(mlc::Array<bool>{((Shared{} < ast::Stmt{}) > /* unit */), serial}, i32);
/* unit */;
StatementsPartialPair{} = [&]() {
auto block_fold = statements.fold(StatementsBlockAccumulator{{}, serial}, [=](auto accumulated, auto statement_under_block) mutable { return [&]() {
auto statement_partial = partial_application_desugar_statement(statement_under_block, accumulated.next_serial);
return StatementsBlockAccumulator{accumulated.accumulated_statements.concat(mlc::Array{statement_partial.statement}), statement_partial.next_serial};
}(); });
return StatementsPartialPair{block_fold.accumulated_statements, block_fold.next_serial};
}();
/* unit */;
StmtPartialPair{} = StmtPartialPair{((Shared{} < ast::Stmt{}) > /* unit */), i32};
/* unit */;
partial_application_desugar_statement(((Shared{} < ast::Stmt{}) > /* unit */), i32);
/* unit */;
StmtPartialPair{} = std::visit(overloaded{[&](const ast::StmtLet& stmtLet) { auto [name, is_mut, typ, value_expression, span_statement] = stmtLet; return [&]() {
auto value_pair = partial_application_desugar_inner(value_expression, serial);
return StmtPartialPair{std::make_shared<ast::Stmt>(ast::StmtLet{name, is_mut, typ, value_pair.expression, span_statement}), value_pair.next_serial};
}(); },
[&](const ast::StmtLetPat& stmtLetPat) { auto [pat, is_mut, typ, value_expression, has_else, else_body, span_statement] = stmtLetPat; return [&]() {
auto value_pair = partial_application_desugar_inner(value_expression, serial);
auto else_pair = partial_application_desugar_inner(else_body, value_pair.next_serial);
return StmtPartialPair{std::make_shared<ast::Stmt>(ast::StmtLetPat{pat, is_mut, typ, value_pair.expression, has_else, else_pair.expression, span_statement}), else_pair.next_serial};
}(); },
[&](const ast::StmtLetConst& stmtLetConst) { auto [name, typ, value_expression, span_statement] = stmtLetConst; return [&]() {
auto value_pair = partial_application_desugar_inner(value_expression, serial);
return StmtPartialPair{std::make_shared<ast::Stmt>(ast::StmtLetConst{name, typ, value_pair.expression, span_statement}), value_pair.next_serial};
}(); },
[&](const ast::StmtExpr& stmtExpr) { auto [inner_expression, span_statement] = stmtExpr; return [&]() {
auto inner_pair = partial_application_desugar_inner(inner_expression, serial);
return StmtPartialPair{std::make_shared<ast::Stmt>(ast::StmtExpr{inner_pair.expression, span_statement}), inner_pair.next_serial};
}(); },
[&](const ast::StmtReturn& stmtReturn) { auto [inner_expression, span_statement] = stmtReturn; return [&]() {
auto inner_pair = partial_application_desugar_inner(inner_expression, serial);
return StmtPartialPair{std::make_shared<ast::Stmt>(ast::StmtReturn{inner_pair.expression, span_statement}), inner_pair.next_serial};
}(); },
[&](const ast::StmtBreak& stmtBreak) { auto [__0] = stmtBreak; return StmtPartialPair{statement, serial}; },
[&](const ast::StmtContinue& stmtContinue) { auto [__0] = stmtContinue; return StmtPartialPair{statement, serial}; }
}, statement);
/* unit */;
MatchArmsPartialPair{} = MatchArmsPartialPair{mlc::Array<bool>{((Shared{} < ast::MatchArm{}) > /* unit */), next_serial}, i32};
/* unit */;
partial_application_desugar_match_arms_list(mlc::Array<bool>{((Shared{} < ast::MatchArm{}) > /* unit */), serial}, i32);
/* unit */;
MatchArmsPartialPair{} = arms.fold(MatchArmsPartialPair{{}, serial}, [=](auto accumulated, auto arm_shared_under_match) mutable { return [&]() {
auto guard_pair = partial_application_desugar_inner(arm_shared_under_match.when_condition, accumulated.next_serial);
auto body_pair = partial_application_desugar_inner(arm_shared_under_match.body, guard_pair.next_serial);
return MatchArmsPartialPair{accumulated.arms.concat(mlc::Array{std::make_shared<ast::MatchArm>(ast::MatchArm{arm_shared_under_match.pat, arm_shared_under_match.has_guard, guard_pair.expression, body_pair.expression})}), body_pair.next_serial};
}(); });
/* unit */;
partial_application_desugar_inner(((Shared{} < ast::Expr{}) > /* unit */), i32);
/* unit */;
PartialPair{} = std::visit(overloaded{[&](const ast::ExprInt& exprInt) { auto [__0, __1] = exprInt; return partial_application_leaf_partial_pair(expression, serial); },
[&](const ast::ExprStr& exprStr) { auto [__0, __1] = exprStr; return partial_application_leaf_partial_pair(expression, serial); },
[&](const ast::ExprBool& exprBool) { auto [__0, __1] = exprBool; return partial_application_leaf_partial_pair(expression, serial); },
[&](const ast::ExprUnit& exprUnit) { auto [__0] = exprUnit; return partial_application_leaf_partial_pair(expression, serial); },
[&](const ast::ExprFloat& exprFloat) { auto [__0, __1] = exprFloat; return partial_application_leaf_partial_pair(expression, serial); },
[&](const ast::ExprI64& exprI64) { auto [__0, __1] = exprI64; return partial_application_leaf_partial_pair(expression, serial); },
[&](const ast::ExprU8& exprU8) { auto [__0, __1] = exprU8; return partial_application_leaf_partial_pair(expression, serial); },
[&](const ast::ExprUsize& exprUsize) { auto [__0, __1] = exprUsize; return partial_application_leaf_partial_pair(expression, serial); },
[&](const ast::ExprChar& exprChar) { auto [__0, __1] = exprChar; return partial_application_leaf_partial_pair(expression, serial); },
[&](const ast::ExprExtern& exprExtern) { auto [__0] = exprExtern; return partial_application_leaf_partial_pair(expression, serial); },
[&](const ast::ExprIdent& exprIdent) { auto [__0, __1] = exprIdent; return partial_application_leaf_partial_pair(expression, serial); },
[&](const ast::ExprBin& exprBin) { auto [binary_operation, left, right, span_binary] = exprBin; return [&]() {
auto left_pair = partial_application_desugar_inner(left, serial);
auto right_pair = partial_application_desugar_inner(right, left_pair.next_serial);
return PartialPair{std::make_shared<ast::Expr>(ast::ExprBin{binary_operation, left_pair.expression, right_pair.expression, span_binary}), right_pair.next_serial};
}(); },
[&](const ast::ExprUn& exprUn) { auto [unary_operation, inner_unary, span_unary] = exprUn; return [&]() {
auto inner_pair = partial_application_desugar_inner(inner_unary, serial);
return PartialPair{std::make_shared<ast::Expr>(ast::ExprUn{unary_operation, inner_pair.expression, span_unary}), inner_pair.next_serial};
}(); },
[&](const ast::ExprCall& exprCall) { auto [function, call_arguments, span_call] = exprCall; return [&]() {
auto callee_pair = partial_application_desugar_inner(function, serial);
auto arguments_pair = partial_application_map_arguments(call_arguments, callee_pair.next_serial);
return partial_wrap_call_after_children(callee_pair.expression, arguments_pair.arguments, span_call, arguments_pair.next_serial);
}(); },
[&](const ast::ExprMethod& exprMethod) { auto [object_expression, method_name, method_arguments, span_method] = exprMethod; return [&]() {
auto object_pair = partial_application_desugar_inner(object_expression, serial);
auto arguments_pair = partial_application_map_arguments(method_arguments, object_pair.next_serial);
return partial_wrap_method_after_children(object_pair.expression, method_name, arguments_pair.arguments, span_method, arguments_pair.next_serial);
}(); },
[&](const ast::ExprField& exprField) { auto [object_expression, field_name, span_field] = exprField; return [&]() {
auto object_pair = partial_application_desugar_inner(object_expression, serial);
return PartialPair{std::make_shared<ast::Expr>(ast::ExprField{object_pair.expression, field_name, span_field}), object_pair.next_serial};
}(); },
[&](const ast::ExprIndex& exprIndex) { auto [object_expression, index_expression, span_index] = exprIndex; return [&]() {
auto object_pair = partial_application_desugar_inner(object_expression, serial);
auto index_pair = partial_application_desugar_inner(index_expression, object_pair.next_serial);
return PartialPair{std::make_shared<ast::Expr>(ast::ExprIndex{object_pair.expression, index_pair.expression, span_index}), index_pair.next_serial};
}(); },
[&](const ast::ExprIf& exprIf) { auto [condition, then_branch, else_branch, span_conditional] = exprIf; return [&]() {
auto condition_pair = partial_application_desugar_inner(condition, serial);
auto then_pair = partial_application_desugar_inner(then_branch, condition_pair.next_serial);
auto else_pair = partial_application_desugar_inner(else_branch, then_pair.next_serial);
return PartialPair{std::make_shared<ast::Expr>(ast::ExprIf{condition_pair.expression, then_pair.expression, else_pair.expression, span_conditional}), else_pair.next_serial};
}(); },
[&](const ast::ExprBlock& exprBlock) { auto [statements, result_expression, span_block] = exprBlock; return [&]() {
auto stmts_pair = partial_application_desugar_statements_block(statements, serial);
auto result_pair = partial_application_desugar_inner(result_expression, stmts_pair.next_serial);
return PartialPair{std::make_shared<ast::Expr>(ast::ExprBlock{stmts_pair.statements, result_pair.expression, span_block}), result_pair.next_serial};
}(); },
[&](const ast::ExprWhile& exprWhile) { auto [condition, statements, span_while] = exprWhile; return [&]() {
auto condition_pair = partial_application_desugar_inner(condition, serial);
auto stmts_pair = partial_application_desugar_statements_block(statements, condition_pair.next_serial);
return PartialPair{std::make_shared<ast::Expr>(ast::ExprWhile{condition_pair.expression, stmts_pair.statements, span_while}), stmts_pair.next_serial};
}(); },
[&](const ast::ExprFor& exprFor) { auto [variable_name, iterator_expression, statements, span_for] = exprFor; return [&]() {
auto iterator_pair = partial_application_desugar_inner(iterator_expression, serial);
auto stmts_pair = partial_application_desugar_statements_block(statements, iterator_pair.next_serial);
return PartialPair{std::make_shared<ast::Expr>(ast::ExprFor{variable_name, iterator_pair.expression, stmts_pair.statements, span_for}), stmts_pair.next_serial};
}(); },
[&](const ast::ExprMatch& exprMatch) { auto [subject, arms, span_match] = exprMatch; return [&]() {
auto subject_pair = partial_application_desugar_inner(subject, serial);
auto arms_pair = partial_application_desugar_match_arms_list(arms, subject_pair.next_serial);
return PartialPair{std::make_shared<ast::Expr>(ast::ExprMatch{subject_pair.expression, arms_pair.arms, span_match}), arms_pair.next_serial};
}(); },
[&](const ast::ExprRecord& exprRecord) { auto [type_name, lit_parts, span_record] = exprRecord; return [&]() {
auto parts_pair = partial_application_desugar_record_lit_parts(lit_parts, serial);
return PartialPair{std::make_shared<ast::Expr>(ast::ExprRecord{type_name, parts_pair.parts, span_record}), parts_pair.next_serial};
}(); },
[&](const ast::ExprRecordUpdate& exprRecordUpdate) { auto [type_name, base_expression, field_values, span_update] = exprRecordUpdate; return [&]() {
auto base_pair = partial_application_desugar_inner(base_expression, serial);
auto fields_accumulator = partial_application_desugar_field_values_sequence(field_values, base_pair.next_serial);
return PartialPair{std::make_shared<ast::Expr>(ast::ExprRecordUpdate{type_name, base_pair.expression, fields_accumulator.mapped_field_values, span_update}), fields_accumulator.next_serial};
}(); },
[&](const ast::ExprArray& exprArray) { auto [elements, span_array] = exprArray; return [&]() {
auto element_accumulator = partial_application_desugar_expression_element_sequence(elements, serial);
return PartialPair{std::make_shared<ast::Expr>(ast::ExprArray{element_accumulator.mapped_expressions, span_array}), element_accumulator.next_serial};
}(); },
[&](const ast::ExprTuple& exprTuple) { auto [elements, span_tuple] = exprTuple; return [&]() {
auto element_accumulator = partial_application_desugar_expression_element_sequence(elements, serial);
return PartialPair{std::make_shared<ast::Expr>(ast::ExprTuple{element_accumulator.mapped_expressions, span_tuple}), element_accumulator.next_serial};
}(); },
[&](const ast::ExprQuestion& exprQuestion) { auto [inner_question, span_question] = exprQuestion; return [&]() {
auto inner_pair = partial_application_desugar_inner(inner_question, serial);
return PartialPair{std::make_shared<ast::Expr>(ast::ExprQuestion{inner_pair.expression, span_question}), inner_pair.next_serial};
}(); },
[&](const ast::ExprLambda& exprLambda) { auto [parameter_names, body, span_lambda] = exprLambda; return [&]() {
auto body_pair = partial_application_desugar_inner(body, serial);
return PartialPair{std::make_shared<ast::Expr>(ast::ExprLambda{parameter_names, body_pair.expression, span_lambda}), body_pair.next_serial};
}(); },
[&](const ast::ExprNamedArg& exprNamedArg) { auto [label, inner_named, span_named_arg] = exprNamedArg; return [&]() {
auto inner_pair = partial_application_desugar_inner(inner_named, serial);
return PartialPair{std::make_shared<ast::Expr>(ast::ExprNamedArg{label, inner_pair.expression, span_named_arg}), inner_pair.next_serial};
}(); },
[&](const ast::ExprWith& exprWith) { auto [resource_expression, binder_name, statements, span_with] = exprWith; return [&]() {
auto resource_pair = partial_application_desugar_inner(resource_expression, serial);
auto stmts_pair = partial_application_desugar_statements_block(statements, resource_pair.next_serial);
return PartialPair{std::make_shared<ast::Expr>(ast::ExprWith{resource_pair.expression, binder_name, stmts_pair.statements, span_with}), stmts_pair.next_serial};
}(); }
}, expression);
export_;
/* unit */;
return partial_application_desugar_expr(((Shared{} < ast::Expr{}) > /* unit */), /* unit */, ((Shared{} < ast::Expr{}) > /* unit */), partial_application_desugar_inner(expression, 0).expression);
}();
}
}());
}
}

} // namespace partial_application_desugar
