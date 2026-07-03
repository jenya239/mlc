#define main mlc_user_main
#include "names.hpp"

#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "expr_visitor.hpp"
#include "diagnostic_codes.hpp"
#include "transform.hpp"
#include "transform_stmts.hpp"

namespace names {

using namespace ast;
using namespace semantic_ir;
using namespace registry;
using namespace expr_visitor;
using namespace diagnostic_codes;
using namespace transform;
using namespace transform_stmts;
using namespace ast_tokens;

mlc::Array<ast::Diagnostic> NameCheckResult_append_expression_diagnostics(NameCheckResult self, mlc::Array<ast::Diagnostic> expression_diagnostics) noexcept;
NameCheckResult NameCheckResult_append_diagnostics(NameCheckResult self, mlc::Array<ast::Diagnostic> extra_diagnostics) noexcept;
NameCheckResult NameCheckResult_with_scope(NameCheckResult self, mlc::Array<mlc::String> new_scope) noexcept;
NameCheckResult NameCheckResult_merge(NameCheckResult self, NameCheckResult other) noexcept;
NameCheckResult NamesPass_visit_int(NamesPass self, int _integer_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_str(NamesPass self, mlc::String _string_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_bool(NamesPass self, bool _boolean_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_unit(NamesPass self, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_float(NamesPass self, mlc::String _float_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_i64(NamesPass self, mlc::String _integer_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_u8(NamesPass self, mlc::String _byte_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_usize(NamesPass self, mlc::String _size_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_char(NamesPass self, mlc::String _character_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_extern(NamesPass self, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_ident(NamesPass self, mlc::String name, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_bin(NamesPass self, mlc::String _operation, std::shared_ptr<semantic_ir::SemanticExpression> left_expression, std::shared_ptr<semantic_ir::SemanticExpression> right_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_un(NamesPass self, mlc::String _operation, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_call(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> function_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> _mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_method(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String _method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> _mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_field(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String _field_name, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_index(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, std::shared_ptr<semantic_ir::SemanticExpression> index_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_if(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_block(NamesPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_while(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> condition, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_for(NamesPass self, mlc::String variable, std::shared_ptr<semantic_ir::SemanticExpression> iterator, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_match(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_record(NamesPass self, mlc::String _type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_record_update(NamesPass self, mlc::String _type_name, std::shared_ptr<semantic_ir::SemanticExpression> base_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_array(NamesPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_tuple(NamesPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_question(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_lambda(NamesPass self, mlc::Array<mlc::String> parameter_names, std::shared_ptr<semantic_ir::SemanticExpression> body_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_with(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> resource, mlc::String binder, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_unsupported(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> _expression) noexcept;
bool scope_contains(mlc::Array<mlc::String> scope, mlc::String name) noexcept{
  return scope.any([=](mlc::String scope_entry) mutable { return (scope_entry == name); });
}
mlc::Array<mlc::String> pattern_bindings(std::shared_ptr<ast::Pattern> pattern) noexcept{
  auto empty_pattern_bindings = mlc::Array<mlc::String>{};
  return collect_pattern_bindings(pattern, empty_pattern_bindings);
}
mlc::Array<mlc::String> collect_pattern_bindings_fold_step(mlc::Array<mlc::String> accumulator, std::shared_ptr<ast::Pattern> sub_pattern) noexcept{
  return collect_pattern_bindings(sub_pattern, accumulator);
}
mlc::Array<mlc::String> collect_pattern_array_bindings(mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, mlc::String rest, mlc::Array<mlc::String> bindings_so_far) noexcept{
  auto bindings_after_array_elements = sub_patterns.fold(bindings_so_far, collect_pattern_bindings_fold_step);
  if (((rest != mlc::String("", 0)) && (rest != mlc::String("_", 1))))   {
    return bindings_after_array_elements.concat(mlc::Array<mlc::String>{rest});
  } else   {
    return bindings_after_array_elements;
  }
}
mlc::Array<mlc::String> collect_pattern_or_bindings(mlc::Array<std::shared_ptr<ast::Pattern>> alternatives, mlc::Array<mlc::String> bindings_so_far) noexcept{
  if ((alternatives.length() > 0))   {
    return collect_pattern_bindings(alternatives[0], bindings_so_far);
  } else   {
    return bindings_so_far;
  }
}
mlc::Array<mlc::String> collect_pattern_bindings(std::shared_ptr<ast::Pattern> pattern, mlc::Array<mlc::String> bindings_so_far) noexcept{
  return std::visit(overloaded{[&](const ast::PatternWild& patternWild) { auto [__0] = patternWild; return bindings_so_far; },
[&](const ast::PatternIdent& patternIdent) { auto [name, __1] = patternIdent; return bindings_so_far.concat(mlc::Array<mlc::String>{name}); },
[&](const ast::PatternInt& patternInt) { auto [__0, __1] = patternInt; return bindings_so_far; },
[&](const ast::PatternStr& patternStr) { auto [__0, __1] = patternStr; return bindings_so_far; },
[&](const ast::PatternStringLit& patternStringLit) { auto [__0, __1] = patternStringLit; return bindings_so_far; },
[&](const ast::PatternBool& patternBool) { auto [__0, __1] = patternBool; return bindings_so_far; },
[&](const ast::PatternUnit& patternUnit) { auto [__0] = patternUnit; return bindings_so_far; },
[&](const ast::PatternCtor& patternCtor) { auto [__0, sub_patterns, __2] = patternCtor; return sub_patterns.fold(bindings_so_far, collect_pattern_bindings_fold_step); },
[&](const ast::PatternRecord& patternRecord) { auto [__0, field_patterns, __2] = patternRecord; return field_patterns.fold(bindings_so_far, collect_pattern_bindings_fold_step); },
[&](const ast::PatternTuple& patternTuple) { auto [sub_patterns, __1] = patternTuple; return sub_patterns.fold(bindings_so_far, collect_pattern_bindings_fold_step); },
[&](const ast::PatternArray& patternArray) { auto [sub_patterns, rest, __2] = patternArray; return collect_pattern_array_bindings(sub_patterns, rest, bindings_so_far); },
[&](const ast::PatternOr& patternOr) { auto [alternatives, __1] = patternOr; return collect_pattern_or_bindings(alternatives, bindings_so_far); }
}, (*pattern));
}
mlc::Array<ast::Diagnostic> check_names_identifier(mlc::String name, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals, ast::Span source_span) noexcept{
  if (((!scope_contains(locals, name)) && (!globals.has(name))))   {
    return mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(((mlc::String("undefined: ", 11) + mlc::to_string(name)) + mlc::String("", 0)), source_span, diagnostic_codes::diagnostic_code_e001())};
  } else   {
    auto empty_ident_diagnostics = mlc::Array<ast::Diagnostic>{};
    return empty_ident_diagnostics;
  }
}
mlc::Array<ast::Diagnostic> check_names_empty() noexcept{
  auto empty_list = mlc::Array<ast::Diagnostic>{};
  return empty_list;
}
mlc::Array<mlc::String> append_binding_name_to_scope(mlc::Array<mlc::String> accumulated_scope, mlc::String binding_name) noexcept{
  return accumulated_scope.concat(mlc::Array<mlc::String>{binding_name});
}
mlc::Array<mlc::String> scope_with_bindings(mlc::Array<mlc::String> scope, mlc::Array<mlc::String> binding_names) noexcept{
  return binding_names.fold(scope, append_binding_name_to_scope);
}
mlc::Array<ast::Diagnostic> NameCheckResult_append_expression_diagnostics(NameCheckResult self, mlc::Array<ast::Diagnostic> expression_diagnostics) noexcept;
NameCheckResult NameCheckResult_append_diagnostics(NameCheckResult self, mlc::Array<ast::Diagnostic> extra_diagnostics) noexcept;
NameCheckResult NameCheckResult_with_scope(NameCheckResult self, mlc::Array<mlc::String> new_scope) noexcept;
NameCheckResult NameCheckResult_merge(NameCheckResult self, NameCheckResult other) noexcept;
mlc::Array<ast::Diagnostic> NameCheckResult_append_expression_diagnostics(NameCheckResult self, mlc::Array<ast::Diagnostic> expression_diagnostics) noexcept{
return ast::diagnostics_append(self.diagnostics, expression_diagnostics);
}
NameCheckResult NameCheckResult_append_diagnostics(NameCheckResult self, mlc::Array<ast::Diagnostic> extra_diagnostics) noexcept{
return NameCheckResult{ast::diagnostics_append(self.diagnostics, extra_diagnostics), self.scope};
}
NameCheckResult NameCheckResult_with_scope(NameCheckResult self, mlc::Array<mlc::String> new_scope) noexcept{
return NameCheckResult{self.diagnostics, new_scope};
}
NameCheckResult NameCheckResult_merge(NameCheckResult self, NameCheckResult other) noexcept{
return NameCheckResult{ast::diagnostics_append(self.diagnostics, other.diagnostics), other.scope};
}
NamesPass names_pass_new(mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
  return NamesPass{locals, globals, 0};
}
NameCheckResult name_check_ok(mlc::Array<mlc::String> scope) noexcept{
  return NameCheckResult{check_names_empty(), scope};
}
NameCheckResult merge_name_check_results(NameCheckResult first, NameCheckResult second) noexcept{
  return NameCheckResult_merge(first, second);
}
NameCheckResult dispatch_names_pass(NamesPass names_pass, std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{
  return std::visit(overloaded{[&](const semantic_ir::SemanticExpressionInt& semanticExpressionInt) -> NameCheckResult { auto [integer_value, type_value, __2] = semanticExpressionInt; return NamesPass_visit_int(names_pass, integer_value, type_value); },
[&](const semantic_ir::SemanticExpressionStr& semanticExpressionStr) -> NameCheckResult { auto [string_value, type_value, __2] = semanticExpressionStr; return NamesPass_visit_str(names_pass, string_value, type_value); },
[&](const semantic_ir::SemanticExpressionBool& semanticExpressionBool) -> NameCheckResult { auto [boolean_value, type_value, __2] = semanticExpressionBool; return NamesPass_visit_bool(names_pass, boolean_value, type_value); },
[&](const semantic_ir::SemanticExpressionUnit& semanticExpressionUnit) -> NameCheckResult { auto [type_value, __1] = semanticExpressionUnit; return NamesPass_visit_unit(names_pass, type_value); },
[&](const semantic_ir::SemanticExpressionFloat& semanticExpressionFloat) -> NameCheckResult { auto [float_value, type_value, __2] = semanticExpressionFloat; return NamesPass_visit_float(names_pass, float_value, type_value); },
[&](const semantic_ir::SemanticExpressionI64& semanticExpressionI64) -> NameCheckResult { auto [integer_value, type_value, __2] = semanticExpressionI64; return NamesPass_visit_i64(names_pass, integer_value, type_value); },
[&](const semantic_ir::SemanticExpressionU8& semanticExpressionU8) -> NameCheckResult { auto [byte_value, type_value, __2] = semanticExpressionU8; return NamesPass_visit_u8(names_pass, byte_value, type_value); },
[&](const semantic_ir::SemanticExpressionUsize& semanticExpressionUsize) -> NameCheckResult { auto [size_value, type_value, __2] = semanticExpressionUsize; return NamesPass_visit_usize(names_pass, size_value, type_value); },
[&](const semantic_ir::SemanticExpressionChar& semanticExpressionChar) -> NameCheckResult { auto [character_value, type_value, __2] = semanticExpressionChar; return NamesPass_visit_char(names_pass, character_value, type_value); },
[&](const semantic_ir::SemanticExpressionExtern& semanticExpressionExtern) -> NameCheckResult { auto [type_value, __1] = semanticExpressionExtern; return NamesPass_visit_extern(names_pass, type_value); },
[&](const semantic_ir::SemanticExpressionIdent& semanticExpressionIdent) -> NameCheckResult { auto [name_value, type_value, source_span] = semanticExpressionIdent; return NameCheckResult{check_names_identifier(name_value, names_pass.locals, names_pass.globals, source_span), names_pass.locals}; },
[&](const semantic_ir::SemanticExpressionBin& semanticExpressionBin) -> NameCheckResult { auto [operation, left_expression, right_expression, type_value, __4] = semanticExpressionBin; return NamesPass_visit_bin(names_pass, operation, left_expression, right_expression, type_value); },
[&](const semantic_ir::SemanticExpressionUn& semanticExpressionUn) -> NameCheckResult { auto [operation, inner_expression, type_value, __3] = semanticExpressionUn; return NamesPass_visit_un(names_pass, operation, inner_expression, type_value); },
[&](const semantic_ir::SemanticExpressionCall& semanticExpressionCall) -> NameCheckResult { auto [function_expression, arguments, mutability_flags, type_value, __4] = semanticExpressionCall; return NamesPass_visit_call(names_pass, function_expression, arguments, mutability_flags, type_value); },
[&](const semantic_ir::SemanticExpressionMethod& semanticExpressionMethod) -> NameCheckResult { auto [object, method_name, arguments, mutability_flags, type_value, __5] = semanticExpressionMethod; return NamesPass_visit_method(names_pass, object, method_name, arguments, mutability_flags, type_value); },
[&](const semantic_ir::SemanticExpressionField& semanticExpressionField) -> NameCheckResult { auto [object, field_name, type_value, __3] = semanticExpressionField; return NamesPass_visit_field(names_pass, object, field_name, type_value); },
[&](const semantic_ir::SemanticExpressionIndex& semanticExpressionIndex) -> NameCheckResult { auto [object, index_expression, type_value, __3] = semanticExpressionIndex; return NamesPass_visit_index(names_pass, object, index_expression, type_value); },
[&](const semantic_ir::SemanticExpressionIf& semanticExpressionIf) -> NameCheckResult { auto [condition, then_expression, else_expression, type_value, __4] = semanticExpressionIf; return NamesPass_visit_if(names_pass, condition, then_expression, else_expression, type_value); },
[&](const semantic_ir::SemanticExpressionBlock& semanticExpressionBlock) -> NameCheckResult { auto [statements, result_expression, type_value, __3] = semanticExpressionBlock; return NamesPass_visit_block(names_pass, statements, result_expression, type_value); },
[&](const semantic_ir::SemanticExpressionWhile& semanticExpressionWhile) -> NameCheckResult { auto [condition, statements, type_value, __3] = semanticExpressionWhile; return NamesPass_visit_while(names_pass, condition, statements, type_value); },
[&](const semantic_ir::SemanticExpressionFor& semanticExpressionFor) -> NameCheckResult { auto [variable, iterator, statements, type_value, __4] = semanticExpressionFor; return NamesPass_visit_for(names_pass, variable, iterator, statements, type_value); },
[&](const semantic_ir::SemanticExpressionMatch& semanticExpressionMatch) -> NameCheckResult { auto [subject, arms, type_value, __3] = semanticExpressionMatch; return NamesPass_visit_match(names_pass, subject, arms, type_value); },
[&](const semantic_ir::SemanticExpressionRecord& semanticExpressionRecord) -> NameCheckResult { auto [type_name, field_values, type_value, __3] = semanticExpressionRecord; return NamesPass_visit_record(names_pass, type_name, field_values, type_value); },
[&](const semantic_ir::SemanticExpressionRecordUpdate& semanticExpressionRecordUpdate) -> NameCheckResult { auto [type_name, base_expression, overrides, type_value, __4] = semanticExpressionRecordUpdate; return NamesPass_visit_record_update(names_pass, type_name, base_expression, overrides, type_value); },
[&](const semantic_ir::SemanticExpressionArray& semanticExpressionArray) -> NameCheckResult { auto [elements, type_value, __2] = semanticExpressionArray; return NamesPass_visit_array(names_pass, elements, type_value); },
[&](const semantic_ir::SemanticExpressionTuple& semanticExpressionTuple) -> NameCheckResult { auto [elements, type_value, __2] = semanticExpressionTuple; return NamesPass_visit_tuple(names_pass, elements, type_value); },
[&](const semantic_ir::SemanticExpressionQuestion& semanticExpressionQuestion) -> NameCheckResult { auto [inner_expression, type_value, __2] = semanticExpressionQuestion; return NamesPass_visit_question(names_pass, inner_expression, type_value); },
[&](const semantic_ir::SemanticExpressionLambda& semanticExpressionLambda) -> NameCheckResult { auto [parameter_names, body_expression, type_value, __3] = semanticExpressionLambda; return NamesPass_visit_lambda(names_pass, parameter_names, body_expression, type_value); },
[&](const semantic_ir::SemanticExpressionWith& semanticExpressionWith) -> NameCheckResult { auto [resource, binder, statements, type_value, __4] = semanticExpressionWith; return NamesPass_visit_with(names_pass, resource, binder, statements, type_value); }
}, (*expression));
}
NameCheckResult check_names_semantic_field_values(mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, NamesPass names_pass) noexcept{
  auto accumulator = name_check_ok(names_pass.locals);
  auto index = 0;
  while ((index < field_values.length()))   {
    (accumulator = merge_name_check_results(accumulator, dispatch_names_pass(names_pass, field_values[index]->value)));
    (index = (index + 1));
  }
  return accumulator;
}
NameCheckResult check_names_semantic_elements(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, NamesPass names_pass) noexcept{
  auto accumulator = name_check_ok(names_pass.locals);
  auto index = 0;
  while ((index < elements.length()))   {
    (accumulator = merge_name_check_results(accumulator, dispatch_names_pass(names_pass, elements[index])));
    (index = (index + 1));
  }
  return accumulator;
}
NameCheckResult check_names_semantic_statements(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
  return statements.fold(NameCheckResult{{}, locals}, [=](NameCheckResult accumulator, std::shared_ptr<semantic_ir::SemanticStatement> statement_under_walk) mutable { return std::visit(overloaded{[&](const semantic_ir::SemanticStatementLet& semanticStatementLet) -> NameCheckResult { auto [name, __1, value, __3, __4] = semanticStatementLet; return NameCheckResult{ast::diagnostics_append(accumulator.diagnostics, check_names_semantic_expression(value, accumulator.scope, globals)), accumulator.scope.concat(mlc::Array<mlc::String>{name})}; },
[&](const semantic_ir::SemanticStatementLetPattern& semanticStatementLetPattern) -> NameCheckResult { auto [pattern, __1, value, __3, has_else, else_body, __6] = semanticStatementLetPattern; return [&]() {
auto scope_after_value = accumulator.scope;
auto diagnostics_after_pattern = ast::diagnostics_append(accumulator.diagnostics, check_names_semantic_expression(value, scope_after_value, globals));
auto scope_after_bindings = scope_with_bindings(scope_after_value, pattern_bindings(pattern));
if (has_else) {
  (diagnostics_after_pattern = ast::diagnostics_append(diagnostics_after_pattern, check_names_semantic_expression(else_body, scope_after_bindings, globals)));
}
return NameCheckResult{diagnostics_after_pattern, scope_after_bindings};
}(); },
[&](const semantic_ir::SemanticStatementLetConst& semanticStatementLetConst) -> NameCheckResult { auto [name, value, __2, __3] = semanticStatementLetConst; return NameCheckResult{ast::diagnostics_append(accumulator.diagnostics, check_names_semantic_expression(value, accumulator.scope, globals)), accumulator.scope.concat(mlc::Array<mlc::String>{name})}; },
[&](const semantic_ir::SemanticStatementExpr& semanticStatementExpr) -> NameCheckResult { auto [expression, __1] = semanticStatementExpr; return NameCheckResult{ast::diagnostics_append(accumulator.diagnostics, check_names_semantic_expression(expression, accumulator.scope, globals)), accumulator.scope}; },
[&](const semantic_ir::SemanticStatementReturn& semanticStatementReturn) -> NameCheckResult { auto [expression, __1] = semanticStatementReturn; return NameCheckResult{ast::diagnostics_append(accumulator.diagnostics, check_names_semantic_expression(expression, accumulator.scope, globals)), accumulator.scope}; },
[&](const semantic_ir::SemanticStatementBreak& semanticStatementBreak) -> NameCheckResult { auto [__0] = semanticStatementBreak; return accumulator; },
[&](const semantic_ir::SemanticStatementContinue& semanticStatementContinue) -> NameCheckResult { auto [__0] = semanticStatementContinue; return accumulator; }
}, (*statement_under_walk)); });
}
NameCheckResult visit_int(NamesPass self, int _integer_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult visit_str(NamesPass self, mlc::String _string_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult visit_bool(NamesPass self, bool _boolean_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult visit_unit(NamesPass self, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult visit_float(NamesPass self, mlc::String _float_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult visit_i64(NamesPass self, mlc::String _integer_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult visit_u8(NamesPass self, mlc::String _byte_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult visit_usize(NamesPass self, mlc::String _size_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult visit_char(NamesPass self, mlc::String _character_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult visit_extern(NamesPass self, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult visit_ident(NamesPass self, mlc::String name, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult visit_bin(NamesPass self, mlc::String _operation, std::shared_ptr<semantic_ir::SemanticExpression> left_expression, std::shared_ptr<semantic_ir::SemanticExpression> right_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult visit_un(NamesPass self, mlc::String _operation, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult visit_call(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> function_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> _mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult visit_method(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String _method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> _mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult visit_field(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String _field_name, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult visit_index(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, std::shared_ptr<semantic_ir::SemanticExpression> index_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult visit_if(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult visit_block(NamesPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult visit_while(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> condition, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult visit_for(NamesPass self, mlc::String variable, std::shared_ptr<semantic_ir::SemanticExpression> iterator, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult visit_match(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult visit_record(NamesPass self, mlc::String _type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult visit_record_update(NamesPass self, mlc::String _type_name, std::shared_ptr<semantic_ir::SemanticExpression> base_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult visit_array(NamesPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult visit_tuple(NamesPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult visit_question(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult visit_lambda(NamesPass self, mlc::Array<mlc::String> parameter_names, std::shared_ptr<semantic_ir::SemanticExpression> body_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult visit_with(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> resource, mlc::String binder, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult visit_unsupported(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> _expression) noexcept;
NameCheckResult NamesPass_visit_int(NamesPass self, int _integer_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_str(NamesPass self, mlc::String _string_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_bool(NamesPass self, bool _boolean_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_unit(NamesPass self, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_float(NamesPass self, mlc::String _float_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_i64(NamesPass self, mlc::String _integer_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_u8(NamesPass self, mlc::String _byte_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_usize(NamesPass self, mlc::String _size_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_char(NamesPass self, mlc::String _character_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_extern(NamesPass self, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_ident(NamesPass self, mlc::String name, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_bin(NamesPass self, mlc::String _operation, std::shared_ptr<semantic_ir::SemanticExpression> left_expression, std::shared_ptr<semantic_ir::SemanticExpression> right_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_un(NamesPass self, mlc::String _operation, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_call(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> function_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> _mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_method(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String _method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> _mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_field(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String _field_name, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_index(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, std::shared_ptr<semantic_ir::SemanticExpression> index_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_if(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_block(NamesPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_while(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> condition, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_for(NamesPass self, mlc::String variable, std::shared_ptr<semantic_ir::SemanticExpression> iterator, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_match(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_record(NamesPass self, mlc::String _type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_record_update(NamesPass self, mlc::String _type_name, std::shared_ptr<semantic_ir::SemanticExpression> base_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_array(NamesPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_tuple(NamesPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_question(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_lambda(NamesPass self, mlc::Array<mlc::String> parameter_names, std::shared_ptr<semantic_ir::SemanticExpression> body_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_with(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> resource, mlc::String binder, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept;
NameCheckResult NamesPass_visit_unsupported(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> _expression) noexcept;
NameCheckResult NamesPass_visit_int(NamesPass self, int _integer_value, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return name_check_ok(self.locals);
}
NameCheckResult NamesPass_visit_str(NamesPass self, mlc::String _string_value, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return name_check_ok(self.locals);
}
NameCheckResult NamesPass_visit_bool(NamesPass self, bool _boolean_value, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return name_check_ok(self.locals);
}
NameCheckResult NamesPass_visit_unit(NamesPass self, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return name_check_ok(self.locals);
}
NameCheckResult NamesPass_visit_float(NamesPass self, mlc::String _float_value, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return name_check_ok(self.locals);
}
NameCheckResult NamesPass_visit_i64(NamesPass self, mlc::String _integer_value, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return name_check_ok(self.locals);
}
NameCheckResult NamesPass_visit_u8(NamesPass self, mlc::String _byte_value, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return name_check_ok(self.locals);
}
NameCheckResult NamesPass_visit_usize(NamesPass self, mlc::String _size_value, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return name_check_ok(self.locals);
}
NameCheckResult NamesPass_visit_char(NamesPass self, mlc::String _character_value, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return name_check_ok(self.locals);
}
NameCheckResult NamesPass_visit_extern(NamesPass self, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return name_check_ok(self.locals);
}
NameCheckResult NamesPass_visit_ident(NamesPass self, mlc::String name, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return NameCheckResult{check_names_identifier(name, self.locals, self.globals, ast::span_unknown()), self.locals};
}
NameCheckResult NamesPass_visit_bin(NamesPass self, mlc::String _operation, std::shared_ptr<semantic_ir::SemanticExpression> left_expression, std::shared_ptr<semantic_ir::SemanticExpression> right_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return merge_name_check_results(dispatch_names_pass(self, left_expression), dispatch_names_pass(self, right_expression));
}
NameCheckResult NamesPass_visit_un(NamesPass self, mlc::String _operation, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return dispatch_names_pass(self, inner_expression);
}
NameCheckResult NamesPass_visit_call(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> function_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> _mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return merge_name_check_results(dispatch_names_pass(self, function_expression), check_names_semantic_elements(arguments, self));
}
NameCheckResult NamesPass_visit_method(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String _method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> _mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return merge_name_check_results(dispatch_names_pass(self, object), check_names_semantic_elements(arguments, self));
}
NameCheckResult NamesPass_visit_field(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String _field_name, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return dispatch_names_pass(self, object);
}
NameCheckResult NamesPass_visit_index(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, std::shared_ptr<semantic_ir::SemanticExpression> index_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return merge_name_check_results(dispatch_names_pass(self, object), dispatch_names_pass(self, index_expression));
}
NameCheckResult NamesPass_visit_if(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return merge_name_check_results(dispatch_names_pass(self, condition), merge_name_check_results(dispatch_names_pass(self, then_expression), dispatch_names_pass(self, else_expression)));
}
NameCheckResult NamesPass_visit_block(NamesPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{
auto statements_check = check_names_semantic_statements(statements, self.locals, self.globals);
auto result_check = dispatch_names_pass(names_pass_new(statements_check.scope, self.globals), result_expression);
return NameCheckResult_merge(statements_check, result_check);
}
NameCheckResult NamesPass_visit_while(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> condition, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept{
auto statement_check = check_names_semantic_statements(statements, self.locals, self.globals);
return NameCheckResult_with_scope(NameCheckResult_merge(dispatch_names_pass(self, condition), statement_check), self.locals);
}
NameCheckResult NamesPass_visit_for(NamesPass self, mlc::String variable, std::shared_ptr<semantic_ir::SemanticExpression> iterator, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept{
auto inner_scope = self.locals.concat(mlc::Array<mlc::String>{variable});
auto statement_check = check_names_semantic_statements(statements, inner_scope, self.globals);
return NameCheckResult_with_scope(NameCheckResult_merge(dispatch_names_pass(self, iterator), statement_check), self.locals);
}
NameCheckResult NamesPass_visit_match(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms, std::shared_ptr<registry::Type> _semantic_type) noexcept{
auto accumulator = dispatch_names_pass(self, subject);
auto arm_index = 0;
while ((arm_index < arms.length())) {
  auto arm_under_walk = arms[arm_index];
  auto arm_scope = scope_with_bindings(self.locals, pattern_bindings(arm_under_walk->pattern));
  auto arm_pass = names_pass_new(arm_scope, self.globals);
  if (arm_under_walk->has_guard)   {
    (accumulator = merge_name_check_results(accumulator, dispatch_names_pass(arm_pass, arm_under_walk->when_condition)));
  }
  (accumulator = merge_name_check_results(accumulator, dispatch_names_pass(arm_pass, arm_under_walk->body)));
  (arm_index = (arm_index + 1));
}
return NameCheckResult{accumulator.diagnostics, self.locals};
}
NameCheckResult NamesPass_visit_record(NamesPass self, mlc::String _type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return check_names_semantic_field_values(field_values, self);
}
NameCheckResult NamesPass_visit_record_update(NamesPass self, mlc::String _type_name, std::shared_ptr<semantic_ir::SemanticExpression> base_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return merge_name_check_results(dispatch_names_pass(self, base_expression), check_names_semantic_field_values(overrides, self));
}
NameCheckResult NamesPass_visit_array(NamesPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return check_names_semantic_elements(elements, self);
}
NameCheckResult NamesPass_visit_tuple(NamesPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return check_names_semantic_elements(elements, self);
}
NameCheckResult NamesPass_visit_question(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return dispatch_names_pass(self, inner_expression);
}
NameCheckResult NamesPass_visit_lambda(NamesPass self, mlc::Array<mlc::String> parameter_names, std::shared_ptr<semantic_ir::SemanticExpression> body_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{
auto lambda_environment = self.locals;
auto parameter_index = 0;
while ((parameter_index < parameter_names.length())) {
  (lambda_environment = lambda_environment.concat(mlc::Array<mlc::String>{parameter_names[parameter_index]}));
  (parameter_index = (parameter_index + 1));
}
return dispatch_names_pass(names_pass_new(lambda_environment, self.globals), body_expression);
}
NameCheckResult NamesPass_visit_with(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> resource, mlc::String binder, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept{
auto inner_scope = self.locals.concat(mlc::Array<mlc::String>{binder});
auto statements_check = check_names_semantic_statements(statements, inner_scope, self.globals);
return NameCheckResult{ast::diagnostics_append(dispatch_names_pass(self, resource).diagnostics, statements_check.diagnostics), self.locals};
}
NameCheckResult NamesPass_visit_unsupported(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> _expression) noexcept{
return name_check_ok(self.locals);
}
inline NameCheckResult visit_int(NamesPass self, int _integer_value, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_int(self, _integer_value, _semantic_type); }
inline NameCheckResult ExprVisitor_visit_int(NamesPass self, int _integer_value, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_int(self, _integer_value, _semantic_type); }
inline NameCheckResult visit_str(NamesPass self, mlc::String _string_value, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_str(self, _string_value, _semantic_type); }
inline NameCheckResult ExprVisitor_visit_str(NamesPass self, mlc::String _string_value, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_str(self, _string_value, _semantic_type); }
inline NameCheckResult visit_bool(NamesPass self, bool _boolean_value, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_bool(self, _boolean_value, _semantic_type); }
inline NameCheckResult ExprVisitor_visit_bool(NamesPass self, bool _boolean_value, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_bool(self, _boolean_value, _semantic_type); }
inline NameCheckResult visit_unit(NamesPass self, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_unit(self, _semantic_type); }
inline NameCheckResult ExprVisitor_visit_unit(NamesPass self, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_unit(self, _semantic_type); }
inline NameCheckResult visit_float(NamesPass self, mlc::String _float_value, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_float(self, _float_value, _semantic_type); }
inline NameCheckResult ExprVisitor_visit_float(NamesPass self, mlc::String _float_value, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_float(self, _float_value, _semantic_type); }
inline NameCheckResult visit_i64(NamesPass self, mlc::String _integer_value, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_i64(self, _integer_value, _semantic_type); }
inline NameCheckResult ExprVisitor_visit_i64(NamesPass self, mlc::String _integer_value, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_i64(self, _integer_value, _semantic_type); }
inline NameCheckResult visit_u8(NamesPass self, mlc::String _byte_value, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_u8(self, _byte_value, _semantic_type); }
inline NameCheckResult ExprVisitor_visit_u8(NamesPass self, mlc::String _byte_value, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_u8(self, _byte_value, _semantic_type); }
inline NameCheckResult visit_usize(NamesPass self, mlc::String _size_value, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_usize(self, _size_value, _semantic_type); }
inline NameCheckResult ExprVisitor_visit_usize(NamesPass self, mlc::String _size_value, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_usize(self, _size_value, _semantic_type); }
inline NameCheckResult visit_char(NamesPass self, mlc::String _character_value, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_char(self, _character_value, _semantic_type); }
inline NameCheckResult ExprVisitor_visit_char(NamesPass self, mlc::String _character_value, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_char(self, _character_value, _semantic_type); }
inline NameCheckResult visit_extern(NamesPass self, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_extern(self, _semantic_type); }
inline NameCheckResult ExprVisitor_visit_extern(NamesPass self, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_extern(self, _semantic_type); }
inline NameCheckResult visit_ident(NamesPass self, mlc::String name, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_ident(self, name, _semantic_type); }
inline NameCheckResult ExprVisitor_visit_ident(NamesPass self, mlc::String name, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_ident(self, name, _semantic_type); }
inline NameCheckResult visit_bin(NamesPass self, mlc::String _operation, std::shared_ptr<semantic_ir::SemanticExpression> left_expression, std::shared_ptr<semantic_ir::SemanticExpression> right_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_bin(self, _operation, left_expression, right_expression, _semantic_type); }
inline NameCheckResult ExprVisitor_visit_bin(NamesPass self, mlc::String _operation, std::shared_ptr<semantic_ir::SemanticExpression> left_expression, std::shared_ptr<semantic_ir::SemanticExpression> right_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_bin(self, _operation, left_expression, right_expression, _semantic_type); }
inline NameCheckResult visit_un(NamesPass self, mlc::String _operation, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_un(self, _operation, inner_expression, _semantic_type); }
inline NameCheckResult ExprVisitor_visit_un(NamesPass self, mlc::String _operation, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_un(self, _operation, inner_expression, _semantic_type); }
inline NameCheckResult visit_call(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> function_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> _mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_call(self, function_expression, arguments, _mutability_flags, _semantic_type); }
inline NameCheckResult ExprVisitor_visit_call(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> function_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> _mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_call(self, function_expression, arguments, _mutability_flags, _semantic_type); }
inline NameCheckResult visit_method(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String _method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> _mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_method(self, object, _method_name, arguments, _mutability_flags, _semantic_type); }
inline NameCheckResult ExprVisitor_visit_method(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String _method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> _mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_method(self, object, _method_name, arguments, _mutability_flags, _semantic_type); }
inline NameCheckResult visit_field(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String _field_name, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_field(self, object, _field_name, _semantic_type); }
inline NameCheckResult ExprVisitor_visit_field(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String _field_name, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_field(self, object, _field_name, _semantic_type); }
inline NameCheckResult visit_index(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, std::shared_ptr<semantic_ir::SemanticExpression> index_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_index(self, object, index_expression, _semantic_type); }
inline NameCheckResult ExprVisitor_visit_index(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, std::shared_ptr<semantic_ir::SemanticExpression> index_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_index(self, object, index_expression, _semantic_type); }
inline NameCheckResult visit_if(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_if(self, condition, then_expression, else_expression, _semantic_type); }
inline NameCheckResult ExprVisitor_visit_if(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_if(self, condition, then_expression, else_expression, _semantic_type); }
inline NameCheckResult visit_block(NamesPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_block(self, statements, result_expression, _semantic_type); }
inline NameCheckResult ExprVisitor_visit_block(NamesPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_block(self, statements, result_expression, _semantic_type); }
inline NameCheckResult visit_while(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> condition, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_while(self, condition, statements, _semantic_type); }
inline NameCheckResult ExprVisitor_visit_while(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> condition, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_while(self, condition, statements, _semantic_type); }
inline NameCheckResult visit_for(NamesPass self, mlc::String variable, std::shared_ptr<semantic_ir::SemanticExpression> iterator, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_for(self, variable, iterator, statements, _semantic_type); }
inline NameCheckResult ExprVisitor_visit_for(NamesPass self, mlc::String variable, std::shared_ptr<semantic_ir::SemanticExpression> iterator, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_for(self, variable, iterator, statements, _semantic_type); }
inline NameCheckResult visit_match(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_match(self, subject, arms, _semantic_type); }
inline NameCheckResult ExprVisitor_visit_match(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_match(self, subject, arms, _semantic_type); }
inline NameCheckResult visit_record(NamesPass self, mlc::String _type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_record(self, _type_name, field_values, _semantic_type); }
inline NameCheckResult ExprVisitor_visit_record(NamesPass self, mlc::String _type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_record(self, _type_name, field_values, _semantic_type); }
inline NameCheckResult visit_record_update(NamesPass self, mlc::String _type_name, std::shared_ptr<semantic_ir::SemanticExpression> base_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_record_update(self, _type_name, base_expression, overrides, _semantic_type); }
inline NameCheckResult ExprVisitor_visit_record_update(NamesPass self, mlc::String _type_name, std::shared_ptr<semantic_ir::SemanticExpression> base_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_record_update(self, _type_name, base_expression, overrides, _semantic_type); }
inline NameCheckResult visit_array(NamesPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_array(self, elements, _semantic_type); }
inline NameCheckResult ExprVisitor_visit_array(NamesPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_array(self, elements, _semantic_type); }
inline NameCheckResult visit_tuple(NamesPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_tuple(self, elements, _semantic_type); }
inline NameCheckResult ExprVisitor_visit_tuple(NamesPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_tuple(self, elements, _semantic_type); }
inline NameCheckResult visit_question(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_question(self, inner_expression, _semantic_type); }
inline NameCheckResult ExprVisitor_visit_question(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_question(self, inner_expression, _semantic_type); }
inline NameCheckResult visit_lambda(NamesPass self, mlc::Array<mlc::String> parameter_names, std::shared_ptr<semantic_ir::SemanticExpression> body_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_lambda(self, parameter_names, body_expression, _semantic_type); }
inline NameCheckResult ExprVisitor_visit_lambda(NamesPass self, mlc::Array<mlc::String> parameter_names, std::shared_ptr<semantic_ir::SemanticExpression> body_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_lambda(self, parameter_names, body_expression, _semantic_type); }
inline NameCheckResult visit_with(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> resource, mlc::String binder, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_with(self, resource, binder, statements, _semantic_type); }
inline NameCheckResult ExprVisitor_visit_with(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> resource, mlc::String binder, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept { return NamesPass_visit_with(self, resource, binder, statements, _semantic_type); }
inline NameCheckResult visit_unsupported(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> _expression) noexcept { return NamesPass_visit_unsupported(self, _expression); }
inline NameCheckResult ExprVisitor_visit_unsupported(NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> _expression) noexcept { return NamesPass_visit_unsupported(self, _expression); }
static_assert(ExprVisitor<NameCheckResult, NamesPass>, "NamesPass does not implement ExprVisitor");
mlc::Array<ast::Diagnostic> check_names_semantic_expression(std::shared_ptr<semantic_ir::SemanticExpression> expression, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
  auto check_result = dispatch_names_pass(names_pass_new(locals, globals), expression);
  return check_result.diagnostics;
}
std::shared_ptr<semantic_ir::SemanticExpression> transform_parser_expression_for_names(std::shared_ptr<ast::Expr> expression) noexcept{
  return transform::transform_expr(expression, transform::empty_transform_context(), [=](mlc::Array<std::shared_ptr<ast::Stmt>> statements, transform::TransformContext nested_transform_context) mutable { return transform_stmts::transform_stmts(statements, nested_transform_context); });
}
mlc::Array<ast::Diagnostic> check_names_expr(std::shared_ptr<ast::Expr> expression, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
  return check_names_semantic_expression(transform_parser_expression_for_names(expression), locals, globals);
}
NameCheckResult check_names_statements(mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
  return statements.fold(NameCheckResult{{}, locals}, [=](NameCheckResult accumulator, std::shared_ptr<ast::Stmt> statement_under_walk) mutable { return std::visit(overloaded{[&](const ast::StmtLet& stmtLet) -> NameCheckResult { auto [name, __1, __2, value, __4] = stmtLet; return NameCheckResult{ast::diagnostics_append(accumulator.diagnostics, check_names_expr(value, accumulator.scope, globals)), accumulator.scope.concat(mlc::Array<mlc::String>{name})}; },
[&](const ast::StmtLetPattern& stmtLetPattern) -> NameCheckResult { auto [pattern, __1, __2, value, has_else, else_body, __6] = stmtLetPattern; return [&]() {
auto scope_after_value = accumulator.scope;
auto diagnostics_after_pattern = ast::diagnostics_append(accumulator.diagnostics, check_names_expr(value, scope_after_value, globals));
auto scope_after_bindings = scope_with_bindings(scope_after_value, pattern_bindings(pattern));
if (has_else) {
  (diagnostics_after_pattern = ast::diagnostics_append(diagnostics_after_pattern, check_names_expr(else_body, scope_after_bindings, globals)));
}
return NameCheckResult{diagnostics_after_pattern, scope_after_bindings};
}(); },
[&](const ast::StmtLetConst& stmtLetConst) -> NameCheckResult { auto [name, __1, value, __3] = stmtLetConst; return NameCheckResult{ast::diagnostics_append(accumulator.diagnostics, check_names_expr(value, accumulator.scope, globals)), accumulator.scope.concat(mlc::Array<mlc::String>{name})}; },
[&](const ast::StmtExpr& stmtExpr) -> NameCheckResult { auto [expression, __1] = stmtExpr; return NameCheckResult{ast::diagnostics_append(accumulator.diagnostics, check_names_expr(expression, accumulator.scope, globals)), accumulator.scope}; },
[&](const ast::StmtReturn& stmtReturn) -> NameCheckResult { auto [expression, __1] = stmtReturn; return NameCheckResult{ast::diagnostics_append(accumulator.diagnostics, check_names_expr(expression, accumulator.scope, globals)), accumulator.scope}; },
[&](const ast::StmtBreak& stmtBreak) -> NameCheckResult { auto [__0] = stmtBreak; return accumulator; },
[&](const ast::StmtContinue& stmtContinue) -> NameCheckResult { auto [__0] = stmtContinue; return accumulator; }
}, (*statement_under_walk)); });
}

} // namespace names
