#define main mlc_user_main
#include "check_mutations.hpp"

#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "expr_visitor.hpp"
#include "names.hpp"
#include "diagnostic_codes.hpp"
#include "transform.hpp"
#include "transform_stmts.hpp"

namespace check_mutations {

using namespace ast;
using namespace semantic_ir;
using namespace registry;
using namespace expr_visitor;
using namespace names;
using namespace diagnostic_codes;
using namespace transform;
using namespace transform_stmts;
using namespace ast_tokens;

MutationCheckResult MutationsPass_visit_int(MutationsPass self, int _integer_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_str(MutationsPass self, mlc::String _string_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_bool(MutationsPass self, bool _boolean_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_unit(MutationsPass self, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_float(MutationsPass self, mlc::String _float_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_i64(MutationsPass self, mlc::String _integer_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_u8(MutationsPass self, mlc::String _byte_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_usize(MutationsPass self, mlc::String _size_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_char(MutationsPass self, mlc::String _character_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_extern(MutationsPass self, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_ident(MutationsPass self, mlc::String _name, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_bin(MutationsPass self, mlc::String operation, std::shared_ptr<semantic_ir::SemanticExpression> left_expression, std::shared_ptr<semantic_ir::SemanticExpression> right_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_un(MutationsPass self, mlc::String _operation, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_call(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> function_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> _mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_method(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String _method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> _mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_field(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String _field_name, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_index(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, std::shared_ptr<semantic_ir::SemanticExpression> index_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_if(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_block(MutationsPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_while(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> condition, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_for(MutationsPass self, mlc::String _variable, std::shared_ptr<semantic_ir::SemanticExpression> iterator, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_match(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_record(MutationsPass self, mlc::String _type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_record_update(MutationsPass self, mlc::String _type_name, std::shared_ptr<semantic_ir::SemanticExpression> base_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_array(MutationsPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_tuple(MutationsPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_question(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_lambda(MutationsPass self, mlc::Array<mlc::String> _parameter_names, std::shared_ptr<semantic_ir::SemanticExpression> body_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_with(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> resource, mlc::String _binder, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_unsupported(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> _expression) noexcept;
bool scope_has(mlc::Array<mlc::String> scope, mlc::String sought_name) noexcept{
  return scope.any([=](mlc::String scope_entry) mutable { return (scope_entry == sought_name); });
}
mlc::Array<mlc::String> mutable_scope_with_binding(mlc::Array<mlc::String> scope, mlc::String binding_name) noexcept{
  return scope.concat(mlc::Array<mlc::String>{binding_name});
}
mlc::Array<mlc::String> mutable_scope_with_bindings(mlc::Array<mlc::String> scope, mlc::Array<mlc::String> binding_names) noexcept{
  return binding_names.fold(scope, [=](mlc::Array<mlc::String> accumulated_scope, mlc::String binding_name) mutable { return accumulated_scope.concat(mlc::Array<mlc::String>{binding_name}); });
}
mlc::Array<mlc::String> mutable_scope_after_let(mlc::Array<mlc::String> scope, mlc::String name, bool is_mut) noexcept{
  if (is_mut)   {
    return mutable_scope_with_binding(scope, name);
  } else   {
    return scope;
  }
}
mlc::Array<mlc::String> mutable_scope_after_pattern_let(mlc::Array<mlc::String> scope, std::shared_ptr<ast::Pattern> pattern, bool is_mut) noexcept{
  if (is_mut)   {
    return mutable_scope_with_bindings(scope, names::pattern_bindings(pattern));
  } else   {
    return scope;
  }
}
MutationsPass mutations_pass_new(mlc::Array<mlc::String> mutable_locals) noexcept{
  return MutationsPass{mutable_locals, 0};
}
mlc::Array<ast::Diagnostic> mutation_check_empty() noexcept{
  auto empty_diagnostics = mlc::Array<ast::Diagnostic>{};
  return empty_diagnostics;
}
mlc::Array<ast::Diagnostic> ident_assignment_mutation_errors(mlc::String binding_name, mlc::Array<mlc::String> mutable_locals, ast::Span source_span) noexcept{
  if ((!scope_has(mutable_locals, binding_name)))   {
    return mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code((mlc::String("cannot assign to immutable binding: ", 36) + binding_name), source_span, diagnostic_codes::diagnostic_code_e031())};
  } else   {
    return mutation_check_empty();
  }
}
mlc::Array<ast::Diagnostic> assignment_target_mutation_errors(std::shared_ptr<semantic_ir::SemanticExpression> left_expression, mlc::Array<mlc::String> mutable_locals) noexcept{
  return [&]() -> mlc::Array<ast::Diagnostic> {
auto __match_subject = left_expression;
if (std::holds_alternative<semantic_ir::SemanticExpressionIdent>((*__match_subject))) {
const semantic_ir::SemanticExpressionIdent& semanticExpressionIdent = std::get<semantic_ir::SemanticExpressionIdent>((*__match_subject));
auto [name, __1, __2] = semanticExpressionIdent; return ident_assignment_mutation_errors(name, mutable_locals, semantic_ir::sexpr_span(left_expression));
}
return mutation_check_empty();
std::abort();
}();
}
MutationCheckResult mutation_check_ok(mlc::Array<mlc::String> mutable_locals) noexcept{
  return MutationCheckResult{mutation_check_empty(), mutable_locals};
}
MutationCheckResult merge_mutation_check_results(MutationCheckResult first, MutationCheckResult second) noexcept{
  return MutationCheckResult{ast::diagnostics_append(first.diagnostics, second.diagnostics), second.mutable_locals};
}
MutationCheckResult dispatch_mutations_pass(MutationsPass mutations_pass, std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{
  return [&]() -> MutationCheckResult {
auto __match_subject = expression;
if (std::holds_alternative<semantic_ir::SemanticExpressionInt>((*__match_subject))) {
const semantic_ir::SemanticExpressionInt& semanticExpressionInt = std::get<semantic_ir::SemanticExpressionInt>((*__match_subject));
auto [integer_value, type_value, __2] = semanticExpressionInt; return MutationsPass_visit_int(mutations_pass, integer_value, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionStr>((*__match_subject))) {
const semantic_ir::SemanticExpressionStr& semanticExpressionStr = std::get<semantic_ir::SemanticExpressionStr>((*__match_subject));
auto [string_value, type_value, __2] = semanticExpressionStr; return MutationsPass_visit_str(mutations_pass, string_value, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionBool>((*__match_subject))) {
const semantic_ir::SemanticExpressionBool& semanticExpressionBool = std::get<semantic_ir::SemanticExpressionBool>((*__match_subject));
auto [boolean_value, type_value, __2] = semanticExpressionBool; return MutationsPass_visit_bool(mutations_pass, boolean_value, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionUnit>((*__match_subject))) {
const semantic_ir::SemanticExpressionUnit& semanticExpressionUnit = std::get<semantic_ir::SemanticExpressionUnit>((*__match_subject));
auto [type_value, __1] = semanticExpressionUnit; return MutationsPass_visit_unit(mutations_pass, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionFloat>((*__match_subject))) {
const semantic_ir::SemanticExpressionFloat& semanticExpressionFloat = std::get<semantic_ir::SemanticExpressionFloat>((*__match_subject));
auto [float_value, type_value, __2] = semanticExpressionFloat; return MutationsPass_visit_float(mutations_pass, float_value, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionI64>((*__match_subject))) {
const semantic_ir::SemanticExpressionI64& semanticExpressionI64 = std::get<semantic_ir::SemanticExpressionI64>((*__match_subject));
auto [integer_value, type_value, __2] = semanticExpressionI64; return MutationsPass_visit_i64(mutations_pass, integer_value, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionU8>((*__match_subject))) {
const semantic_ir::SemanticExpressionU8& semanticExpressionU8 = std::get<semantic_ir::SemanticExpressionU8>((*__match_subject));
auto [byte_value, type_value, __2] = semanticExpressionU8; return MutationsPass_visit_u8(mutations_pass, byte_value, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionUsize>((*__match_subject))) {
const semantic_ir::SemanticExpressionUsize& semanticExpressionUsize = std::get<semantic_ir::SemanticExpressionUsize>((*__match_subject));
auto [size_value, type_value, __2] = semanticExpressionUsize; return MutationsPass_visit_usize(mutations_pass, size_value, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionChar>((*__match_subject))) {
const semantic_ir::SemanticExpressionChar& semanticExpressionChar = std::get<semantic_ir::SemanticExpressionChar>((*__match_subject));
auto [character_value, type_value, __2] = semanticExpressionChar; return MutationsPass_visit_char(mutations_pass, character_value, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionExtern>((*__match_subject))) {
const semantic_ir::SemanticExpressionExtern& semanticExpressionExtern = std::get<semantic_ir::SemanticExpressionExtern>((*__match_subject));
auto [type_value, __1, __2, __3, __4] = semanticExpressionExtern; return MutationsPass_visit_extern(mutations_pass, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionIdent>((*__match_subject))) {
const semantic_ir::SemanticExpressionIdent& semanticExpressionIdent = std::get<semantic_ir::SemanticExpressionIdent>((*__match_subject));
auto [name_value, type_value, __2] = semanticExpressionIdent; return MutationsPass_visit_ident(mutations_pass, name_value, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionBin>((*__match_subject))) {
const semantic_ir::SemanticExpressionBin& semanticExpressionBin = std::get<semantic_ir::SemanticExpressionBin>((*__match_subject));
auto [operation, left_expression, right_expression, type_value, __4] = semanticExpressionBin; return MutationsPass_visit_bin(mutations_pass, operation, left_expression, right_expression, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionUn>((*__match_subject))) {
const semantic_ir::SemanticExpressionUn& semanticExpressionUn = std::get<semantic_ir::SemanticExpressionUn>((*__match_subject));
auto [operation, inner_expression, type_value, __3] = semanticExpressionUn; return MutationsPass_visit_un(mutations_pass, operation, inner_expression, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionCall>((*__match_subject))) {
const semantic_ir::SemanticExpressionCall& semanticExpressionCall = std::get<semantic_ir::SemanticExpressionCall>((*__match_subject));
auto [function_expression, arguments, mutability_flags, type_value, __4] = semanticExpressionCall; return MutationsPass_visit_call(mutations_pass, function_expression, arguments, mutability_flags, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionMethod>((*__match_subject))) {
const semantic_ir::SemanticExpressionMethod& semanticExpressionMethod = std::get<semantic_ir::SemanticExpressionMethod>((*__match_subject));
auto [object, method_name, arguments, mutability_flags, type_value, __5] = semanticExpressionMethod; return MutationsPass_visit_method(mutations_pass, object, method_name, arguments, mutability_flags, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionField>((*__match_subject))) {
const semantic_ir::SemanticExpressionField& semanticExpressionField = std::get<semantic_ir::SemanticExpressionField>((*__match_subject));
auto [object, field_name, type_value, __3] = semanticExpressionField; return MutationsPass_visit_field(mutations_pass, object, field_name, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionIndex>((*__match_subject))) {
const semantic_ir::SemanticExpressionIndex& semanticExpressionIndex = std::get<semantic_ir::SemanticExpressionIndex>((*__match_subject));
auto [object, index_expression, type_value, __3] = semanticExpressionIndex; return MutationsPass_visit_index(mutations_pass, object, index_expression, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*__match_subject))) {
const semantic_ir::SemanticExpressionIf& semanticExpressionIf = std::get<semantic_ir::SemanticExpressionIf>((*__match_subject));
auto [condition, then_expression, else_expression, type_value, __4] = semanticExpressionIf; return MutationsPass_visit_if(mutations_pass, condition, then_expression, else_expression, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionBlock>((*__match_subject))) {
const semantic_ir::SemanticExpressionBlock& semanticExpressionBlock = std::get<semantic_ir::SemanticExpressionBlock>((*__match_subject));
auto [statements, result_expression, type_value, __3] = semanticExpressionBlock; return MutationsPass_visit_block(mutations_pass, statements, result_expression, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionWhile>((*__match_subject))) {
const semantic_ir::SemanticExpressionWhile& semanticExpressionWhile = std::get<semantic_ir::SemanticExpressionWhile>((*__match_subject));
auto [condition, statements, type_value, __3] = semanticExpressionWhile; return MutationsPass_visit_while(mutations_pass, condition, statements, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionFor>((*__match_subject))) {
const semantic_ir::SemanticExpressionFor& semanticExpressionFor = std::get<semantic_ir::SemanticExpressionFor>((*__match_subject));
auto [variable, iterator, statements, type_value, __4] = semanticExpressionFor; return MutationsPass_visit_for(mutations_pass, variable, iterator, statements, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionMatch>((*__match_subject))) {
const semantic_ir::SemanticExpressionMatch& semanticExpressionMatch = std::get<semantic_ir::SemanticExpressionMatch>((*__match_subject));
auto [subject, arms, type_value, __3] = semanticExpressionMatch; return MutationsPass_visit_match(mutations_pass, subject, arms, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionRecord>((*__match_subject))) {
const semantic_ir::SemanticExpressionRecord& semanticExpressionRecord = std::get<semantic_ir::SemanticExpressionRecord>((*__match_subject));
auto [type_name, field_values, type_value, __3] = semanticExpressionRecord; return MutationsPass_visit_record(mutations_pass, type_name, field_values, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionRecordUpdate>((*__match_subject))) {
const semantic_ir::SemanticExpressionRecordUpdate& semanticExpressionRecordUpdate = std::get<semantic_ir::SemanticExpressionRecordUpdate>((*__match_subject));
auto [type_name, base_expression, overrides, type_value, __4] = semanticExpressionRecordUpdate; return MutationsPass_visit_record_update(mutations_pass, type_name, base_expression, overrides, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionArray>((*__match_subject))) {
const semantic_ir::SemanticExpressionArray& semanticExpressionArray = std::get<semantic_ir::SemanticExpressionArray>((*__match_subject));
auto [elements, type_value, __2] = semanticExpressionArray; return MutationsPass_visit_array(mutations_pass, elements, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionTuple>((*__match_subject))) {
const semantic_ir::SemanticExpressionTuple& semanticExpressionTuple = std::get<semantic_ir::SemanticExpressionTuple>((*__match_subject));
auto [elements, type_value, __2] = semanticExpressionTuple; return MutationsPass_visit_tuple(mutations_pass, elements, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionQuestion>((*__match_subject))) {
const semantic_ir::SemanticExpressionQuestion& semanticExpressionQuestion = std::get<semantic_ir::SemanticExpressionQuestion>((*__match_subject));
auto [inner_expression, type_value, __2] = semanticExpressionQuestion; return MutationsPass_visit_question(mutations_pass, inner_expression, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionLambda>((*__match_subject))) {
const semantic_ir::SemanticExpressionLambda& semanticExpressionLambda = std::get<semantic_ir::SemanticExpressionLambda>((*__match_subject));
auto [parameter_names, body_expression, type_value, __3] = semanticExpressionLambda; return MutationsPass_visit_lambda(mutations_pass, parameter_names, body_expression, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionWith>((*__match_subject))) {
const semantic_ir::SemanticExpressionWith& semanticExpressionWith = std::get<semantic_ir::SemanticExpressionWith>((*__match_subject));
auto [resource, binder, statements, type_value, __4] = semanticExpressionWith; return MutationsPass_visit_with(mutations_pass, resource, binder, statements, type_value);
}
return MutationsPass_visit_unsupported(mutations_pass, expression);
std::abort();
}();
}
MutationCheckResult check_mutation_semantic_field_values(mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, MutationsPass mutations_pass) noexcept{
  return field_values.fold(mutation_check_ok(mutations_pass.mutable_locals), [=](MutationCheckResult accumulator, std::shared_ptr<semantic_ir::SemanticFieldVal> field_value_under_walk) mutable { return merge_mutation_check_results(accumulator, dispatch_mutations_pass(mutations_pass, field_value_under_walk->value)); });
}
MutationCheckResult check_mutation_semantic_elements(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, MutationsPass mutations_pass) noexcept{
  return elements.fold(mutation_check_ok(mutations_pass.mutable_locals), [=](MutationCheckResult accumulator, std::shared_ptr<semantic_ir::SemanticExpression> element_under_walk) mutable { return merge_mutation_check_results(accumulator, dispatch_mutations_pass(mutations_pass, element_under_walk)); });
}
MutationCheckResult check_mutation_semantic_statements(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, mlc::Array<mlc::String> mutable_locals) noexcept{
  return statements.fold(MutationCheckResult{{}, mutable_locals}, [=](MutationCheckResult accumulator, std::shared_ptr<semantic_ir::SemanticStatement> statement_under_walk) mutable { return std::visit(overloaded{[&](const semantic_ir::SemanticStatementLet& semanticStatementLet) -> MutationCheckResult { auto [name, is_mut, value, __3, __4] = semanticStatementLet; return [&]() {
auto scope_after = mutable_scope_after_let(accumulator.mutable_locals, name, is_mut);
return MutationCheckResult{ast::diagnostics_append(accumulator.diagnostics, check_mutation_semantic_expression(value, accumulator.mutable_locals)), scope_after};
}(); },
[&](const semantic_ir::SemanticStatementLetPattern& semanticStatementLetPattern) -> MutationCheckResult { auto [pattern, is_mut, value, __3, __4, __5, __6] = semanticStatementLetPattern; return [&]() {
auto scope_after_value = accumulator.mutable_locals;
auto scope_after_bindings = mutable_scope_after_pattern_let(scope_after_value, pattern, is_mut);
return MutationCheckResult{ast::diagnostics_append(accumulator.diagnostics, check_mutation_semantic_expression(value, scope_after_value)), scope_after_bindings};
}(); },
[&](const semantic_ir::SemanticStatementLetConst& semanticStatementLetConst) -> MutationCheckResult { auto [name, value, __2, __3] = semanticStatementLetConst; return MutationCheckResult{ast::diagnostics_append(accumulator.diagnostics, check_mutation_semantic_expression(value, accumulator.mutable_locals)), accumulator.mutable_locals}; },
[&](const semantic_ir::SemanticStatementExpr& semanticStatementExpr) -> MutationCheckResult { auto [expression, __1] = semanticStatementExpr; return MutationCheckResult{ast::diagnostics_append(accumulator.diagnostics, check_mutation_semantic_expression(expression, accumulator.mutable_locals)), accumulator.mutable_locals}; },
[&](const semantic_ir::SemanticStatementReturn& semanticStatementReturn) -> MutationCheckResult { auto [expression, __1] = semanticStatementReturn; return MutationCheckResult{ast::diagnostics_append(accumulator.diagnostics, check_mutation_semantic_expression(expression, accumulator.mutable_locals)), accumulator.mutable_locals}; },
[&](const semantic_ir::SemanticStatementBreak& semanticStatementBreak) -> MutationCheckResult { auto [__0] = semanticStatementBreak; return accumulator; },
[&](const semantic_ir::SemanticStatementContinue& semanticStatementContinue) -> MutationCheckResult { auto [__0] = semanticStatementContinue; return accumulator; }
}, (*statement_under_walk)); });
}
MutationCheckResult visit_int(MutationsPass self, int _integer_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult visit_str(MutationsPass self, mlc::String _string_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult visit_bool(MutationsPass self, bool _boolean_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult visit_unit(MutationsPass self, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult visit_float(MutationsPass self, mlc::String _float_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult visit_i64(MutationsPass self, mlc::String _integer_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult visit_u8(MutationsPass self, mlc::String _byte_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult visit_usize(MutationsPass self, mlc::String _size_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult visit_char(MutationsPass self, mlc::String _character_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult visit_extern(MutationsPass self, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult visit_ident(MutationsPass self, mlc::String _name, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult visit_bin(MutationsPass self, mlc::String operation, std::shared_ptr<semantic_ir::SemanticExpression> left_expression, std::shared_ptr<semantic_ir::SemanticExpression> right_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult visit_un(MutationsPass self, mlc::String _operation, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult visit_call(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> function_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> _mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult visit_method(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String _method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> _mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult visit_field(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String _field_name, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult visit_index(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, std::shared_ptr<semantic_ir::SemanticExpression> index_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult visit_if(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult visit_block(MutationsPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult visit_while(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> condition, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult visit_for(MutationsPass self, mlc::String _variable, std::shared_ptr<semantic_ir::SemanticExpression> iterator, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult visit_match(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult visit_record(MutationsPass self, mlc::String _type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult visit_record_update(MutationsPass self, mlc::String _type_name, std::shared_ptr<semantic_ir::SemanticExpression> base_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult visit_array(MutationsPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult visit_tuple(MutationsPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult visit_question(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult visit_lambda(MutationsPass self, mlc::Array<mlc::String> _parameter_names, std::shared_ptr<semantic_ir::SemanticExpression> body_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult visit_with(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> resource, mlc::String _binder, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult visit_unsupported(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> _expression) noexcept;
MutationCheckResult MutationsPass_visit_int(MutationsPass self, int _integer_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_str(MutationsPass self, mlc::String _string_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_bool(MutationsPass self, bool _boolean_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_unit(MutationsPass self, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_float(MutationsPass self, mlc::String _float_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_i64(MutationsPass self, mlc::String _integer_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_u8(MutationsPass self, mlc::String _byte_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_usize(MutationsPass self, mlc::String _size_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_char(MutationsPass self, mlc::String _character_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_extern(MutationsPass self, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_ident(MutationsPass self, mlc::String _name, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_bin(MutationsPass self, mlc::String operation, std::shared_ptr<semantic_ir::SemanticExpression> left_expression, std::shared_ptr<semantic_ir::SemanticExpression> right_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_un(MutationsPass self, mlc::String _operation, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_call(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> function_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> _mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_method(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String _method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> _mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_field(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String _field_name, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_index(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, std::shared_ptr<semantic_ir::SemanticExpression> index_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_if(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_block(MutationsPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_while(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> condition, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_for(MutationsPass self, mlc::String _variable, std::shared_ptr<semantic_ir::SemanticExpression> iterator, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_match(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_record(MutationsPass self, mlc::String _type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_record_update(MutationsPass self, mlc::String _type_name, std::shared_ptr<semantic_ir::SemanticExpression> base_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_array(MutationsPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_tuple(MutationsPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_question(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_lambda(MutationsPass self, mlc::Array<mlc::String> _parameter_names, std::shared_ptr<semantic_ir::SemanticExpression> body_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_with(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> resource, mlc::String _binder, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept;
MutationCheckResult MutationsPass_visit_unsupported(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> _expression) noexcept;
MutationCheckResult MutationsPass_visit_int(MutationsPass self, int _integer_value, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return mutation_check_ok(self.mutable_locals);
}
MutationCheckResult MutationsPass_visit_str(MutationsPass self, mlc::String _string_value, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return mutation_check_ok(self.mutable_locals);
}
MutationCheckResult MutationsPass_visit_bool(MutationsPass self, bool _boolean_value, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return mutation_check_ok(self.mutable_locals);
}
MutationCheckResult MutationsPass_visit_unit(MutationsPass self, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return mutation_check_ok(self.mutable_locals);
}
MutationCheckResult MutationsPass_visit_float(MutationsPass self, mlc::String _float_value, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return mutation_check_ok(self.mutable_locals);
}
MutationCheckResult MutationsPass_visit_i64(MutationsPass self, mlc::String _integer_value, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return mutation_check_ok(self.mutable_locals);
}
MutationCheckResult MutationsPass_visit_u8(MutationsPass self, mlc::String _byte_value, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return mutation_check_ok(self.mutable_locals);
}
MutationCheckResult MutationsPass_visit_usize(MutationsPass self, mlc::String _size_value, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return mutation_check_ok(self.mutable_locals);
}
MutationCheckResult MutationsPass_visit_char(MutationsPass self, mlc::String _character_value, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return mutation_check_ok(self.mutable_locals);
}
MutationCheckResult MutationsPass_visit_extern(MutationsPass self, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return mutation_check_ok(self.mutable_locals);
}
MutationCheckResult MutationsPass_visit_ident(MutationsPass self, mlc::String _name, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return mutation_check_ok(self.mutable_locals);
}
MutationCheckResult MutationsPass_visit_bin(MutationsPass self, mlc::String operation, std::shared_ptr<semantic_ir::SemanticExpression> left_expression, std::shared_ptr<semantic_ir::SemanticExpression> right_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{
auto right_check = dispatch_mutations_pass(self, right_expression);
if ((operation == mlc::String("=", 1))) {
  auto assignment_errors = assignment_target_mutation_errors(left_expression, self.mutable_locals);
  return MutationCheckResult{ast::diagnostics_append(assignment_errors, right_check.diagnostics), self.mutable_locals};
} else {
  return merge_mutation_check_results(dispatch_mutations_pass(self, left_expression), right_check);
}
}
MutationCheckResult MutationsPass_visit_un(MutationsPass self, mlc::String _operation, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return dispatch_mutations_pass(self, inner_expression);
}
MutationCheckResult MutationsPass_visit_call(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> function_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> _mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return merge_mutation_check_results(dispatch_mutations_pass(self, function_expression), check_mutation_semantic_elements(arguments, self));
}
MutationCheckResult MutationsPass_visit_method(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String _method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> _mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return merge_mutation_check_results(dispatch_mutations_pass(self, object), check_mutation_semantic_elements(arguments, self));
}
MutationCheckResult MutationsPass_visit_field(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String _field_name, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return dispatch_mutations_pass(self, object);
}
MutationCheckResult MutationsPass_visit_index(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, std::shared_ptr<semantic_ir::SemanticExpression> index_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return merge_mutation_check_results(dispatch_mutations_pass(self, object), dispatch_mutations_pass(self, index_expression));
}
MutationCheckResult MutationsPass_visit_if(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return merge_mutation_check_results(dispatch_mutations_pass(self, condition), merge_mutation_check_results(dispatch_mutations_pass(self, then_expression), dispatch_mutations_pass(self, else_expression)));
}
MutationCheckResult MutationsPass_visit_block(MutationsPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{
auto statements_check = check_mutation_semantic_statements(statements, self.mutable_locals);
auto result_check = dispatch_mutations_pass(mutations_pass_new(statements_check.mutable_locals), result_expression);
return MutationCheckResult{ast::diagnostics_append(statements_check.diagnostics, result_check.diagnostics), self.mutable_locals};
}
MutationCheckResult MutationsPass_visit_while(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> condition, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept{
auto statements_check = check_mutation_semantic_statements(statements, self.mutable_locals);
return MutationCheckResult{ast::diagnostics_append(dispatch_mutations_pass(self, condition).diagnostics, statements_check.diagnostics), self.mutable_locals};
}
MutationCheckResult MutationsPass_visit_for(MutationsPass self, mlc::String _variable, std::shared_ptr<semantic_ir::SemanticExpression> iterator, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept{
auto statements_check = check_mutation_semantic_statements(statements, self.mutable_locals);
return MutationCheckResult{ast::diagnostics_append(dispatch_mutations_pass(self, iterator).diagnostics, statements_check.diagnostics), self.mutable_locals};
}
MutationCheckResult MutationsPass_visit_match(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms, std::shared_ptr<registry::Type> _semantic_type) noexcept{
auto accumulator = dispatch_mutations_pass(self, subject);
auto arm_index = 0;
while ((arm_index < arms.length())) {
  auto arm_under_walk = arms[arm_index];
  auto arm_scope = mutable_scope_with_bindings(self.mutable_locals, names::pattern_bindings(arm_under_walk->pattern));
  auto arm_pass = mutations_pass_new(arm_scope);
  if (arm_under_walk->has_guard)   {
    (accumulator = merge_mutation_check_results(accumulator, dispatch_mutations_pass(arm_pass, arm_under_walk->when_condition)));
  }
  (accumulator = merge_mutation_check_results(accumulator, dispatch_mutations_pass(arm_pass, arm_under_walk->body)));
  (arm_index = mlc::arith::checked_add(arm_index, 1));
}
return MutationCheckResult{accumulator.diagnostics, self.mutable_locals};
}
MutationCheckResult MutationsPass_visit_record(MutationsPass self, mlc::String _type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return check_mutation_semantic_field_values(field_values, self);
}
MutationCheckResult MutationsPass_visit_record_update(MutationsPass self, mlc::String _type_name, std::shared_ptr<semantic_ir::SemanticExpression> base_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return merge_mutation_check_results(dispatch_mutations_pass(self, base_expression), check_mutation_semantic_field_values(overrides, self));
}
MutationCheckResult MutationsPass_visit_array(MutationsPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return check_mutation_semantic_elements(elements, self);
}
MutationCheckResult MutationsPass_visit_tuple(MutationsPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return check_mutation_semantic_elements(elements, self);
}
MutationCheckResult MutationsPass_visit_question(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return dispatch_mutations_pass(self, inner_expression);
}
MutationCheckResult MutationsPass_visit_lambda(MutationsPass self, mlc::Array<mlc::String> _parameter_names, std::shared_ptr<semantic_ir::SemanticExpression> body_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{
auto empty_scope = mlc::Array<mlc::String>{};
return dispatch_mutations_pass(mutations_pass_new(empty_scope), body_expression);
}
MutationCheckResult MutationsPass_visit_with(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> resource, mlc::String _binder, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept{
auto statements_check = check_mutation_semantic_statements(statements, self.mutable_locals);
return MutationCheckResult{ast::diagnostics_append(dispatch_mutations_pass(self, resource).diagnostics, statements_check.diagnostics), self.mutable_locals};
}
MutationCheckResult MutationsPass_visit_unsupported(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> _expression) noexcept{
return mutation_check_ok(self.mutable_locals);
}
mlc::Array<ast::Diagnostic> check_mutation_semantic_expression(std::shared_ptr<semantic_ir::SemanticExpression> expression, mlc::Array<mlc::String> mutable_locals) noexcept{
  auto check_result = dispatch_mutations_pass(mutations_pass_new(mutable_locals), expression);
  return check_result.diagnostics;
}
mlc::Array<ast::Diagnostic> check_fn_body_semantic_mutations(mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<semantic_ir::SemanticExpression> body) noexcept{
  auto mutable_locals = parameters.fold([&]() {
auto empty_scope = mlc::Array<mlc::String>{};
return empty_scope;
}(), [=](mlc::Array<mlc::String> parameter_scope, std::shared_ptr<ast::Param> parameter_under_check) mutable { return mutable_scope_after_let(parameter_scope, parameter_under_check->name, parameter_under_check->is_mut); });
  return check_mutation_semantic_expression(body, mutable_locals);
}
std::shared_ptr<semantic_ir::SemanticExpression> transform_parser_expression_for_mutations(std::shared_ptr<ast::Expr> expression) noexcept{
  return transform::transform_expr(expression, transform::empty_transform_context(), [=](mlc::Array<std::shared_ptr<ast::Stmt>> statements, transform::TransformContext nested_transform_context) mutable { return transform_stmts::transform_stmts(statements, nested_transform_context); });
}
mlc::Array<ast::Diagnostic> check_fn_body_mutations(mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<ast::Expr> body) noexcept{
  return check_fn_body_semantic_mutations(parameters, transform_parser_expression_for_mutations(body));
}

} // namespace check_mutations
