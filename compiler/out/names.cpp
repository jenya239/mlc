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

struct NamesPass {mlc::Array<mlc::String> locals;mlc::HashMap<mlc::String, bool> globals;int seed;};

bool scope_contains(mlc::Array<mlc::String> scope, mlc::String name) noexcept;

mlc::Array<mlc::String> pattern_bindings(std::shared_ptr<ast::Pattern> pattern) noexcept;

mlc::Array<mlc::String> collect_pattern_bindings_fold_step(mlc::Array<mlc::String> accumulator, std::shared_ptr<ast::Pattern> sub_pattern) noexcept;

mlc::Array<mlc::String> collect_pattern_array_bindings(mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, mlc::String rest, mlc::Array<mlc::String> bindings_so_far) noexcept;

bool binding_name_sets_equal(mlc::Array<mlc::String> left, mlc::Array<mlc::String> right) noexcept;

mlc::Array<mlc::String> collect_pattern_or_bindings(mlc::Array<std::shared_ptr<ast::Pattern>> alternatives, mlc::Array<mlc::String> bindings_so_far) noexcept;

mlc::Array<ast::Diagnostic> check_or_pattern_alternatives_same_bindings(mlc::Array<std::shared_ptr<ast::Pattern>> alternatives, ast::Span source_span) noexcept;

mlc::Array<ast::Diagnostic> check_pattern_or_binding_consistency_fold_step(mlc::Array<ast::Diagnostic> accumulator, std::shared_ptr<ast::Pattern> sub_pattern) noexcept;

mlc::Array<ast::Diagnostic> check_pattern_or_binding_consistency(std::shared_ptr<ast::Pattern> pattern) noexcept;

mlc::Array<mlc::String> collect_pattern_bindings(std::shared_ptr<ast::Pattern> pattern, mlc::Array<mlc::String> bindings_so_far) noexcept;

mlc::Array<ast::Diagnostic> check_names_identifier(mlc::String name, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals, ast::Span source_span) noexcept;

mlc::Array<ast::Diagnostic> check_names_empty() noexcept;

mlc::Array<mlc::String> append_binding_name_to_scope(mlc::Array<mlc::String> accumulated_scope, mlc::String binding_name) noexcept;

mlc::Array<mlc::String> scope_with_bindings(mlc::Array<mlc::String> scope, mlc::Array<mlc::String> binding_names) noexcept;

mlc::Array<ast::Diagnostic> NameCheckResult_append_expression_diagnostics(names::NameCheckResult self, mlc::Array<ast::Diagnostic> expression_diagnostics) noexcept;

names::NameCheckResult NameCheckResult_append_diagnostics(names::NameCheckResult self, mlc::Array<ast::Diagnostic> extra_diagnostics) noexcept;

names::NameCheckResult NameCheckResult_with_scope(names::NameCheckResult self, mlc::Array<mlc::String> new_scope) noexcept;

names::NameCheckResult NameCheckResult_merge(names::NameCheckResult self, names::NameCheckResult other) noexcept;

names::NamesPass names_pass_new(mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

names::NameCheckResult name_check_ok(mlc::Array<mlc::String> scope) noexcept;

names::NameCheckResult merge_name_check_results(names::NameCheckResult first, names::NameCheckResult second) noexcept;

names::NameCheckResult dispatch_names_pass(names::NamesPass names_pass, std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept;

names::NameCheckResult check_names_semantic_field_values(mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, names::NamesPass names_pass) noexcept;

names::NameCheckResult check_names_semantic_elements(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, names::NamesPass names_pass) noexcept;

names::NameCheckResult check_names_semantic_statements(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

names::NameCheckResult NamesPass_visit_int(names::NamesPass self, int _integer_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;

names::NameCheckResult NamesPass_visit_str(names::NamesPass self, mlc::String _string_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;

names::NameCheckResult NamesPass_visit_bool(names::NamesPass self, bool _boolean_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;

names::NameCheckResult NamesPass_visit_unit(names::NamesPass self, std::shared_ptr<registry::Type> _semantic_type) noexcept;

names::NameCheckResult NamesPass_visit_float(names::NamesPass self, mlc::String _float_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;

names::NameCheckResult NamesPass_visit_i64(names::NamesPass self, mlc::String _integer_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;

names::NameCheckResult NamesPass_visit_u8(names::NamesPass self, mlc::String _byte_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;

names::NameCheckResult NamesPass_visit_usize(names::NamesPass self, mlc::String _size_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;

names::NameCheckResult NamesPass_visit_char(names::NamesPass self, mlc::String _character_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;

names::NameCheckResult NamesPass_visit_extern(names::NamesPass self, std::shared_ptr<registry::Type> _semantic_type) noexcept;

names::NameCheckResult NamesPass_visit_ident(names::NamesPass self, mlc::String name, std::shared_ptr<registry::Type> _semantic_type) noexcept;

names::NameCheckResult NamesPass_visit_bin(names::NamesPass self, mlc::String _operation, std::shared_ptr<semantic_ir::SemanticExpression> left_expression, std::shared_ptr<semantic_ir::SemanticExpression> right_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;

names::NameCheckResult NamesPass_visit_un(names::NamesPass self, mlc::String _operation, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;

names::NameCheckResult NamesPass_visit_call(names::NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> function_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> _mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept;

names::NameCheckResult NamesPass_visit_method(names::NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String _method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> _mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept;

names::NameCheckResult NamesPass_visit_field(names::NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String _field_name, std::shared_ptr<registry::Type> _semantic_type) noexcept;

names::NameCheckResult NamesPass_visit_index(names::NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, std::shared_ptr<semantic_ir::SemanticExpression> index_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;

names::NameCheckResult NamesPass_visit_if(names::NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;

names::NameCheckResult NamesPass_visit_block(names::NamesPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;

names::NameCheckResult NamesPass_visit_while(names::NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> condition, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept;

names::NameCheckResult NamesPass_visit_for(names::NamesPass self, mlc::String variable, std::shared_ptr<semantic_ir::SemanticExpression> iterator, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept;

names::NameCheckResult NamesPass_visit_match(names::NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms, std::shared_ptr<registry::Type> _semantic_type) noexcept;

names::NameCheckResult NamesPass_visit_record(names::NamesPass self, mlc::String _type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, std::shared_ptr<registry::Type> _semantic_type) noexcept;

names::NameCheckResult NamesPass_visit_record_update(names::NamesPass self, mlc::String _type_name, std::shared_ptr<semantic_ir::SemanticExpression> base_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, std::shared_ptr<registry::Type> _semantic_type) noexcept;

names::NameCheckResult NamesPass_visit_array(names::NamesPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> _semantic_type) noexcept;

names::NameCheckResult NamesPass_visit_tuple(names::NamesPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> _semantic_type) noexcept;

names::NameCheckResult NamesPass_visit_question(names::NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;

names::NameCheckResult NamesPass_visit_lambda(names::NamesPass self, mlc::Array<mlc::String> parameter_names, std::shared_ptr<semantic_ir::SemanticExpression> body_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;

names::NameCheckResult NamesPass_visit_with(names::NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> resource, mlc::String binder, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept;

names::NameCheckResult NamesPass_visit_unsupported(names::NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> _expression) noexcept;

mlc::Array<ast::Diagnostic> check_names_semantic_expression(std::shared_ptr<semantic_ir::SemanticExpression> expression, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> transform_parser_expression_for_names(std::shared_ptr<ast::Expr> expression) noexcept;

mlc::Array<ast::Diagnostic> check_names_expr(std::shared_ptr<ast::Expr> expression, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

names::NameCheckResult check_names_statements(mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

bool scope_contains(mlc::Array<mlc::String> scope, mlc::String name) noexcept{return scope.any([name](mlc::String scope_entry) mutable { return scope_entry == name; });}

mlc::Array<mlc::String> pattern_bindings(std::shared_ptr<ast::Pattern> pattern) noexcept{
mlc::Array<mlc::String> empty_pattern_bindings = {};
return collect_pattern_bindings(pattern, empty_pattern_bindings);
}

mlc::Array<mlc::String> collect_pattern_bindings_fold_step(mlc::Array<mlc::String> accumulator, std::shared_ptr<ast::Pattern> sub_pattern) noexcept{return collect_pattern_bindings(sub_pattern, accumulator);}

mlc::Array<mlc::String> collect_pattern_array_bindings(mlc::Array<std::shared_ptr<ast::Pattern>> sub_patterns, mlc::String rest, mlc::Array<mlc::String> bindings_so_far) noexcept{
mlc::Array<mlc::String> bindings_after_array_elements = sub_patterns.fold(bindings_so_far, collect_pattern_bindings_fold_step);
return rest != mlc::String("") && rest != mlc::String("_") ? bindings_after_array_elements.concat(mlc::Array<mlc::String>{rest}) : bindings_after_array_elements;
}

bool binding_name_sets_equal(mlc::Array<mlc::String> left, mlc::Array<mlc::String> right) noexcept{return left.size() == right.size() && left.all([right](mlc::String left_name) mutable { return right.any([left_name](mlc::String right_name) mutable { return right_name == left_name; }); }) && right.all([left](mlc::String right_name) mutable { return left.any([right_name](mlc::String left_name) mutable { return left_name == right_name; }); });}

mlc::Array<mlc::String> collect_pattern_or_bindings(mlc::Array<std::shared_ptr<ast::Pattern>> alternatives, mlc::Array<mlc::String> bindings_so_far) noexcept{return alternatives.size() > 0 ? collect_pattern_bindings(alternatives[0], bindings_so_far) : bindings_so_far;}

mlc::Array<ast::Diagnostic> check_or_pattern_alternatives_same_bindings(mlc::Array<std::shared_ptr<ast::Pattern>> alternatives, ast::Span source_span) noexcept{
mlc::Array<ast::Diagnostic> empty_diagnostics = {};
return alternatives.size() == 0 ? empty_diagnostics : [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<mlc::String> empty_bindings = {};
  mlc::Array<mlc::String> first_bindings = collect_pattern_bindings(alternatives[0], empty_bindings);
  mlc::Array<ast::Diagnostic> diagnostics = empty_diagnostics;
  int alternative_index = 1;
  while (alternative_index < alternatives.size()){
{
mlc::Array<mlc::String> alternative_bindings = collect_pattern_bindings(alternatives[alternative_index], empty_bindings);
if (!binding_name_sets_equal(first_bindings, alternative_bindings)){
{
diagnostics = diagnostics.concat(mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("all alternatives of an or-pattern must bind the same names"), source_span, diagnostic_codes::diagnostic_code_e083())});
}
}
alternative_index = alternative_index + 1;
}
}
  return diagnostics;
 }();
}

mlc::Array<ast::Diagnostic> check_pattern_or_binding_consistency_fold_step(mlc::Array<ast::Diagnostic> accumulator, std::shared_ptr<ast::Pattern> sub_pattern) noexcept{return ast::diagnostics_append(accumulator, check_pattern_or_binding_consistency(sub_pattern));}

mlc::Array<ast::Diagnostic> check_pattern_or_binding_consistency(std::shared_ptr<ast::Pattern> pattern) noexcept{return std::visit(overloaded{
  [&](const PatternWild& patternwild) -> mlc::Array<ast::Diagnostic> { auto [_w0] = patternwild; return check_names_empty(); },
  [&](const PatternIdent& patternident) -> mlc::Array<ast::Diagnostic> { auto [_w0, _w1] = patternident; return check_names_empty(); },
  [&](const PatternInt& patternint) -> mlc::Array<ast::Diagnostic> { auto [_w0, _w1] = patternint; return check_names_empty(); },
  [&](const PatternStr& patternstr) -> mlc::Array<ast::Diagnostic> { auto [_w0, _w1] = patternstr; return check_names_empty(); },
  [&](const PatternStringLit& patternstringlit) -> mlc::Array<ast::Diagnostic> { auto [_w0, _w1] = patternstringlit; return check_names_empty(); },
  [&](const PatternBool& patternbool) -> mlc::Array<ast::Diagnostic> { auto [_w0, _w1] = patternbool; return check_names_empty(); },
  [&](const PatternUnit& patternunit) -> mlc::Array<ast::Diagnostic> { auto [_w0] = patternunit; return check_names_empty(); },
  [&](const PatternCtor& patternctor) -> mlc::Array<ast::Diagnostic> { auto [_w0, sub_patterns, _w1] = patternctor; return sub_patterns.fold(check_names_empty(), check_pattern_or_binding_consistency_fold_step); },
  [&](const PatternRecord& patternrecord) -> mlc::Array<ast::Diagnostic> { auto [_w0, field_patterns, _w1] = patternrecord; return field_patterns.fold(check_names_empty(), check_pattern_or_binding_consistency_fold_step); },
  [&](const PatternTuple& patterntuple) -> mlc::Array<ast::Diagnostic> { auto [sub_patterns, _w0] = patterntuple; return sub_patterns.fold(check_names_empty(), check_pattern_or_binding_consistency_fold_step); },
  [&](const PatternArray& patternarray) -> mlc::Array<ast::Diagnostic> { auto [sub_patterns, _w0, _w1] = patternarray; return sub_patterns.fold(check_names_empty(), check_pattern_or_binding_consistency_fold_step); },
  [&](const PatternOr& patternor) -> mlc::Array<ast::Diagnostic> { auto [alternatives, source_span] = patternor; return [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> mismatch_diagnostics = check_or_pattern_alternatives_same_bindings(alternatives, source_span);
  return alternatives.fold(mismatch_diagnostics, check_pattern_or_binding_consistency_fold_step);
 }(); }
}, (*pattern));}

mlc::Array<mlc::String> collect_pattern_bindings(std::shared_ptr<ast::Pattern> pattern, mlc::Array<mlc::String> bindings_so_far) noexcept{return std::visit(overloaded{
  [&](const PatternWild& patternwild) -> mlc::Array<mlc::String> { auto [_w0] = patternwild; return bindings_so_far; },
  [&](const PatternIdent& patternident) -> mlc::Array<mlc::String> { auto [name, _w0] = patternident; return bindings_so_far.concat(mlc::Array<mlc::String>{name}); },
  [&](const PatternInt& patternint) -> mlc::Array<mlc::String> { auto [_w0, _w1] = patternint; return bindings_so_far; },
  [&](const PatternStr& patternstr) -> mlc::Array<mlc::String> { auto [_w0, _w1] = patternstr; return bindings_so_far; },
  [&](const PatternStringLit& patternstringlit) -> mlc::Array<mlc::String> { auto [_w0, _w1] = patternstringlit; return bindings_so_far; },
  [&](const PatternBool& patternbool) -> mlc::Array<mlc::String> { auto [_w0, _w1] = patternbool; return bindings_so_far; },
  [&](const PatternUnit& patternunit) -> mlc::Array<mlc::String> { auto [_w0] = patternunit; return bindings_so_far; },
  [&](const PatternCtor& patternctor) -> mlc::Array<mlc::String> { auto [_w0, sub_patterns, _w1] = patternctor; return sub_patterns.fold(bindings_so_far, collect_pattern_bindings_fold_step); },
  [&](const PatternRecord& patternrecord) -> mlc::Array<mlc::String> { auto [_w0, field_patterns, _w1] = patternrecord; return field_patterns.fold(bindings_so_far, collect_pattern_bindings_fold_step); },
  [&](const PatternTuple& patterntuple) -> mlc::Array<mlc::String> { auto [sub_patterns, _w0] = patterntuple; return sub_patterns.fold(bindings_so_far, collect_pattern_bindings_fold_step); },
  [&](const PatternArray& patternarray) -> mlc::Array<mlc::String> { auto [sub_patterns, rest, _w0] = patternarray; return collect_pattern_array_bindings(sub_patterns, rest, bindings_so_far); },
  [&](const PatternOr& patternor) -> mlc::Array<mlc::String> { auto [alternatives, _w0] = patternor; return collect_pattern_or_bindings(alternatives, bindings_so_far); }
}, (*pattern));}

mlc::Array<ast::Diagnostic> check_names_identifier(mlc::String name, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals, ast::Span source_span) noexcept{return !scope_contains(locals, name) && !globals.has(name) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("undefined: ") + name, source_span, diagnostic_codes::diagnostic_code_e001())} : [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> empty_ident_diagnostics = {};
  return empty_ident_diagnostics;
 }();}

mlc::Array<ast::Diagnostic> check_names_empty() noexcept{
mlc::Array<ast::Diagnostic> empty_list = {};
return empty_list;
}

mlc::Array<mlc::String> append_binding_name_to_scope(mlc::Array<mlc::String> accumulated_scope, mlc::String binding_name) noexcept{return accumulated_scope.concat(mlc::Array<mlc::String>{binding_name});}

mlc::Array<mlc::String> scope_with_bindings(mlc::Array<mlc::String> scope, mlc::Array<mlc::String> binding_names) noexcept{return binding_names.fold(scope, append_binding_name_to_scope);}

mlc::Array<ast::Diagnostic> NameCheckResult_append_expression_diagnostics(names::NameCheckResult self, mlc::Array<ast::Diagnostic> expression_diagnostics) noexcept{return ast::diagnostics_append(self.diagnostics, expression_diagnostics);}

names::NameCheckResult NameCheckResult_append_diagnostics(names::NameCheckResult self, mlc::Array<ast::Diagnostic> extra_diagnostics) noexcept{return names::NameCheckResult{ast::diagnostics_append(self.diagnostics, extra_diagnostics), self.scope};}

names::NameCheckResult NameCheckResult_with_scope(names::NameCheckResult self, mlc::Array<mlc::String> new_scope) noexcept{return names::NameCheckResult{self.diagnostics, new_scope};}

names::NameCheckResult NameCheckResult_merge(names::NameCheckResult self, names::NameCheckResult other) noexcept{return names::NameCheckResult{ast::diagnostics_append(self.diagnostics, other.diagnostics), other.scope};}

names::NamesPass names_pass_new(mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{return names::NamesPass{locals, globals, 0};}

names::NameCheckResult name_check_ok(mlc::Array<mlc::String> scope) noexcept{return names::NameCheckResult{check_names_empty(), scope};}

names::NameCheckResult merge_name_check_results(names::NameCheckResult first, names::NameCheckResult second) noexcept{return NameCheckResult_merge(first, second);}

names::NameCheckResult dispatch_names_pass(names::NamesPass names_pass, std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{return std::visit(overloaded{
  [&](const SemanticExpressionInt& semanticexpressionint) -> names::NameCheckResult { auto [integer_value, type_value, _w0] = semanticexpressionint; return NamesPass_visit_int(names_pass, integer_value, type_value); },
  [&](const SemanticExpressionStr& semanticexpressionstr) -> names::NameCheckResult { auto [string_value, type_value, _w0] = semanticexpressionstr; return NamesPass_visit_str(names_pass, string_value, type_value); },
  [&](const SemanticExpressionBool& semanticexpressionbool) -> names::NameCheckResult { auto [boolean_value, type_value, _w0] = semanticexpressionbool; return NamesPass_visit_bool(names_pass, boolean_value, type_value); },
  [&](const SemanticExpressionUnit& semanticexpressionunit) -> names::NameCheckResult { auto [type_value, _w0] = semanticexpressionunit; return NamesPass_visit_unit(names_pass, type_value); },
  [&](const SemanticExpressionFloat& semanticexpressionfloat) -> names::NameCheckResult { auto [float_value, type_value, _w0] = semanticexpressionfloat; return NamesPass_visit_float(names_pass, float_value, type_value); },
  [&](const SemanticExpressionI64& semanticexpressioni64) -> names::NameCheckResult { auto [integer_value, type_value, _w0] = semanticexpressioni64; return NamesPass_visit_i64(names_pass, integer_value, type_value); },
  [&](const SemanticExpressionU8& semanticexpressionu8) -> names::NameCheckResult { auto [byte_value, type_value, _w0] = semanticexpressionu8; return NamesPass_visit_u8(names_pass, byte_value, type_value); },
  [&](const SemanticExpressionUsize& semanticexpressionusize) -> names::NameCheckResult { auto [size_value, type_value, _w0] = semanticexpressionusize; return NamesPass_visit_usize(names_pass, size_value, type_value); },
  [&](const SemanticExpressionChar& semanticexpressionchar) -> names::NameCheckResult { auto [character_value, type_value, _w0] = semanticexpressionchar; return NamesPass_visit_char(names_pass, character_value, type_value); },
  [&](const SemanticExpressionExtern& semanticexpressionextern) -> names::NameCheckResult { auto [type_value, _w0] = semanticexpressionextern; return NamesPass_visit_extern(names_pass, type_value); },
  [&](const SemanticExpressionIdent& semanticexpressionident) -> names::NameCheckResult { auto [name_value, type_value, source_span] = semanticexpressionident; return names::NameCheckResult{check_names_identifier(name_value, names_pass.locals, names_pass.globals, source_span), names_pass.locals}; },
  [&](const SemanticExpressionBin& semanticexpressionbin) -> names::NameCheckResult { auto [operation, left_expression, right_expression, type_value, _w0] = semanticexpressionbin; return NamesPass_visit_bin(names_pass, operation, left_expression, right_expression, type_value); },
  [&](const SemanticExpressionUn& semanticexpressionun) -> names::NameCheckResult { auto [operation, inner_expression, type_value, _w0] = semanticexpressionun; return NamesPass_visit_un(names_pass, operation, inner_expression, type_value); },
  [&](const SemanticExpressionCall& semanticexpressioncall) -> names::NameCheckResult { auto [function_expression, arguments, mutability_flags, type_value, _w0] = semanticexpressioncall; return NamesPass_visit_call(names_pass, function_expression, arguments, mutability_flags, type_value); },
  [&](const SemanticExpressionMethod& semanticexpressionmethod) -> names::NameCheckResult { auto [object, method_name, arguments, mutability_flags, type_value, _w0] = semanticexpressionmethod; return NamesPass_visit_method(names_pass, object, method_name, arguments, mutability_flags, type_value); },
  [&](const SemanticExpressionField& semanticexpressionfield) -> names::NameCheckResult { auto [object, field_name, type_value, _w0] = semanticexpressionfield; return NamesPass_visit_field(names_pass, object, field_name, type_value); },
  [&](const SemanticExpressionIndex& semanticexpressionindex) -> names::NameCheckResult { auto [object, index_expression, type_value, _w0] = semanticexpressionindex; return NamesPass_visit_index(names_pass, object, index_expression, type_value); },
  [&](const SemanticExpressionIf& semanticexpressionif) -> names::NameCheckResult { auto [condition, then_expression, else_expression, type_value, _w0] = semanticexpressionif; return NamesPass_visit_if(names_pass, condition, then_expression, else_expression, type_value); },
  [&](const SemanticExpressionBlock& semanticexpressionblock) -> names::NameCheckResult { auto [statements, result_expression, type_value, _w0] = semanticexpressionblock; return NamesPass_visit_block(names_pass, statements, result_expression, type_value); },
  [&](const SemanticExpressionWhile& semanticexpressionwhile) -> names::NameCheckResult { auto [condition, statements, type_value, _w0] = semanticexpressionwhile; return NamesPass_visit_while(names_pass, condition, statements, type_value); },
  [&](const SemanticExpressionFor& semanticexpressionfor) -> names::NameCheckResult { auto [variable, iterator, statements, type_value, _w0] = semanticexpressionfor; return NamesPass_visit_for(names_pass, variable, iterator, statements, type_value); },
  [&](const SemanticExpressionMatch& semanticexpressionmatch) -> names::NameCheckResult { auto [subject, arms, type_value, _w0] = semanticexpressionmatch; return NamesPass_visit_match(names_pass, subject, arms, type_value); },
  [&](const SemanticExpressionRecord& semanticexpressionrecord) -> names::NameCheckResult { auto [type_name, field_values, type_value, _w0] = semanticexpressionrecord; return NamesPass_visit_record(names_pass, type_name, field_values, type_value); },
  [&](const SemanticExpressionRecordUpdate& semanticexpressionrecordupdate) -> names::NameCheckResult { auto [type_name, base_expression, overrides, type_value, _w0] = semanticexpressionrecordupdate; return NamesPass_visit_record_update(names_pass, type_name, base_expression, overrides, type_value); },
  [&](const SemanticExpressionArray& semanticexpressionarray) -> names::NameCheckResult { auto [elements, type_value, _w0] = semanticexpressionarray; return NamesPass_visit_array(names_pass, elements, type_value); },
  [&](const SemanticExpressionTuple& semanticexpressiontuple) -> names::NameCheckResult { auto [elements, type_value, _w0] = semanticexpressiontuple; return NamesPass_visit_tuple(names_pass, elements, type_value); },
  [&](const SemanticExpressionQuestion& semanticexpressionquestion) -> names::NameCheckResult { auto [inner_expression, type_value, _w0] = semanticexpressionquestion; return NamesPass_visit_question(names_pass, inner_expression, type_value); },
  [&](const SemanticExpressionLambda& semanticexpressionlambda) -> names::NameCheckResult { auto [parameter_names, body_expression, type_value, _w0] = semanticexpressionlambda; return NamesPass_visit_lambda(names_pass, parameter_names, body_expression, type_value); },
  [&](const SemanticExpressionWith& semanticexpressionwith) -> names::NameCheckResult { auto [resource, binder, statements, type_value, _w0] = semanticexpressionwith; return NamesPass_visit_with(names_pass, resource, binder, statements, type_value); }
}, (*expression)._);}

names::NameCheckResult check_names_semantic_field_values(mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, names::NamesPass names_pass) noexcept{
names::NameCheckResult accumulator = name_check_ok(names_pass.locals);
int index = 0;
while (index < field_values.size()){
{
accumulator = merge_name_check_results(accumulator, dispatch_names_pass(names_pass, field_values[index]->value));
index = index + 1;
}
}
return accumulator;
}

names::NameCheckResult check_names_semantic_elements(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, names::NamesPass names_pass) noexcept{
names::NameCheckResult accumulator = name_check_ok(names_pass.locals);
int index = 0;
while (index < elements.size()){
{
accumulator = merge_name_check_results(accumulator, dispatch_names_pass(names_pass, elements[index]));
index = index + 1;
}
}
return accumulator;
}

names::NameCheckResult check_names_semantic_statements(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{return statements.fold(names::NameCheckResult{{}, locals}, [globals](names::NameCheckResult accumulator, std::shared_ptr<semantic_ir::SemanticStatement> statement_under_walk) mutable { return std::visit(overloaded{
  [&](const SemanticStatementLet& semanticstatementlet) -> names::NameCheckResult { auto [name, _w0, value, _w1, _w2] = semanticstatementlet; return names::NameCheckResult{ast::diagnostics_append(accumulator.diagnostics, check_names_semantic_expression(value, accumulator.scope, globals)), accumulator.scope.concat(mlc::Array<mlc::String>{name})}; },
  [&](const SemanticStatementLetPattern& semanticstatementletpattern) -> names::NameCheckResult { auto [pattern, _w0, value, _w1, has_else, else_body, _w2] = semanticstatementletpattern; return [&]() -> names::NameCheckResult { 
  mlc::Array<mlc::String> scope_after_value = accumulator.scope;
  mlc::Array<ast::Diagnostic> diagnostics_after_pattern = ast::diagnostics_append(accumulator.diagnostics, check_names_semantic_expression(value, scope_after_value, globals));
  mlc::Array<mlc::String> scope_after_bindings = scope_with_bindings(scope_after_value, pattern_bindings(pattern));
  if (has_else){
{
diagnostics_after_pattern = ast::diagnostics_append(diagnostics_after_pattern, check_names_semantic_expression(else_body, scope_after_bindings, globals));
}
}
  return names::NameCheckResult{diagnostics_after_pattern, scope_after_bindings};
 }(); },
  [&](const SemanticStatementLetConst& semanticstatementletconst) -> names::NameCheckResult { auto [name, value, _w0, _w1] = semanticstatementletconst; return names::NameCheckResult{ast::diagnostics_append(accumulator.diagnostics, check_names_semantic_expression(value, accumulator.scope, globals)), accumulator.scope.concat(mlc::Array<mlc::String>{name})}; },
  [&](const SemanticStatementExpr& semanticstatementexpr) -> names::NameCheckResult { auto [expression, _w0] = semanticstatementexpr; return names::NameCheckResult{ast::diagnostics_append(accumulator.diagnostics, check_names_semantic_expression(expression, accumulator.scope, globals)), accumulator.scope}; },
  [&](const SemanticStatementReturn& semanticstatementreturn) -> names::NameCheckResult { auto [expression, _w0] = semanticstatementreturn; return names::NameCheckResult{ast::diagnostics_append(accumulator.diagnostics, check_names_semantic_expression(expression, accumulator.scope, globals)), accumulator.scope}; },
  [&](const SemanticStatementBreak& semanticstatementbreak) -> names::NameCheckResult { auto [_w0] = semanticstatementbreak; return accumulator; },
  [&](const SemanticStatementContinue& semanticstatementcontinue) -> names::NameCheckResult { auto [_w0] = semanticstatementcontinue; return accumulator; }
}, (*statement_under_walk)._); });}

names::NameCheckResult NamesPass_visit_int(names::NamesPass self, int _integer_value, std::shared_ptr<registry::Type> _semantic_type) noexcept{return name_check_ok(self.locals);}

names::NameCheckResult NamesPass_visit_str(names::NamesPass self, mlc::String _string_value, std::shared_ptr<registry::Type> _semantic_type) noexcept{return name_check_ok(self.locals);}

names::NameCheckResult NamesPass_visit_bool(names::NamesPass self, bool _boolean_value, std::shared_ptr<registry::Type> _semantic_type) noexcept{return name_check_ok(self.locals);}

names::NameCheckResult NamesPass_visit_unit(names::NamesPass self, std::shared_ptr<registry::Type> _semantic_type) noexcept{return name_check_ok(self.locals);}

names::NameCheckResult NamesPass_visit_float(names::NamesPass self, mlc::String _float_value, std::shared_ptr<registry::Type> _semantic_type) noexcept{return name_check_ok(self.locals);}

names::NameCheckResult NamesPass_visit_i64(names::NamesPass self, mlc::String _integer_value, std::shared_ptr<registry::Type> _semantic_type) noexcept{return name_check_ok(self.locals);}

names::NameCheckResult NamesPass_visit_u8(names::NamesPass self, mlc::String _byte_value, std::shared_ptr<registry::Type> _semantic_type) noexcept{return name_check_ok(self.locals);}

names::NameCheckResult NamesPass_visit_usize(names::NamesPass self, mlc::String _size_value, std::shared_ptr<registry::Type> _semantic_type) noexcept{return name_check_ok(self.locals);}

names::NameCheckResult NamesPass_visit_char(names::NamesPass self, mlc::String _character_value, std::shared_ptr<registry::Type> _semantic_type) noexcept{return name_check_ok(self.locals);}

names::NameCheckResult NamesPass_visit_extern(names::NamesPass self, std::shared_ptr<registry::Type> _semantic_type) noexcept{return name_check_ok(self.locals);}

names::NameCheckResult NamesPass_visit_ident(names::NamesPass self, mlc::String name, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return names::NameCheckResult{check_names_identifier(name, self.locals, self.globals, ast::span_unknown()), self.locals};
}

names::NameCheckResult NamesPass_visit_bin(names::NamesPass self, mlc::String _operation, std::shared_ptr<semantic_ir::SemanticExpression> left_expression, std::shared_ptr<semantic_ir::SemanticExpression> right_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{return merge_name_check_results(dispatch_names_pass(self, left_expression), dispatch_names_pass(self, right_expression));}

names::NameCheckResult NamesPass_visit_un(names::NamesPass self, mlc::String _operation, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{return dispatch_names_pass(self, inner_expression);}

names::NameCheckResult NamesPass_visit_call(names::NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> function_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> _mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept{return merge_name_check_results(dispatch_names_pass(self, function_expression), check_names_semantic_elements(arguments, self));}

names::NameCheckResult NamesPass_visit_method(names::NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String _method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> _mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept{return merge_name_check_results(dispatch_names_pass(self, object), check_names_semantic_elements(arguments, self));}

names::NameCheckResult NamesPass_visit_field(names::NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String _field_name, std::shared_ptr<registry::Type> _semantic_type) noexcept{return dispatch_names_pass(self, object);}

names::NameCheckResult NamesPass_visit_index(names::NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, std::shared_ptr<semantic_ir::SemanticExpression> index_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{return merge_name_check_results(dispatch_names_pass(self, object), dispatch_names_pass(self, index_expression));}

names::NameCheckResult NamesPass_visit_if(names::NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{return merge_name_check_results(dispatch_names_pass(self, condition), merge_name_check_results(dispatch_names_pass(self, then_expression), dispatch_names_pass(self, else_expression)));}

names::NameCheckResult NamesPass_visit_block(names::NamesPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{
names::NameCheckResult statements_check = check_names_semantic_statements(statements, self.locals, self.globals);
names::NameCheckResult result_check = dispatch_names_pass(names_pass_new(statements_check.scope, self.globals), result_expression);
return NameCheckResult_merge(statements_check, result_check);
}

names::NameCheckResult NamesPass_visit_while(names::NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> condition, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept{
names::NameCheckResult statement_check = check_names_semantic_statements(statements, self.locals, self.globals);
return NameCheckResult_with_scope(NameCheckResult_merge(dispatch_names_pass(self, condition), statement_check), self.locals);
}

names::NameCheckResult NamesPass_visit_for(names::NamesPass self, mlc::String variable, std::shared_ptr<semantic_ir::SemanticExpression> iterator, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept{
mlc::Array<mlc::String> inner_scope = self.locals.concat(mlc::Array<mlc::String>{variable});
names::NameCheckResult statement_check = check_names_semantic_statements(statements, inner_scope, self.globals);
return NameCheckResult_with_scope(NameCheckResult_merge(dispatch_names_pass(self, iterator), statement_check), self.locals);
}

names::NameCheckResult NamesPass_visit_match(names::NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms, std::shared_ptr<registry::Type> _semantic_type) noexcept{
names::NameCheckResult accumulator = dispatch_names_pass(self, subject);
int arm_index = 0;
while (arm_index < arms.size()){
{
std::shared_ptr<semantic_ir::SemanticMatchArm> arm_under_walk = arms[arm_index];
accumulator = NameCheckResult_append_diagnostics(accumulator, check_pattern_or_binding_consistency(arm_under_walk->pattern));
mlc::Array<mlc::String> arm_scope = scope_with_bindings(self.locals, pattern_bindings(arm_under_walk->pattern));
names::NamesPass arm_pass = names_pass_new(arm_scope, self.globals);
if (arm_under_walk->has_guard){
{
accumulator = merge_name_check_results(accumulator, dispatch_names_pass(arm_pass, arm_under_walk->when_condition));
}
}
accumulator = merge_name_check_results(accumulator, dispatch_names_pass(arm_pass, arm_under_walk->body));
arm_index = arm_index + 1;
}
}
return names::NameCheckResult{accumulator.diagnostics, self.locals};
}

names::NameCheckResult NamesPass_visit_record(names::NamesPass self, mlc::String _type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, std::shared_ptr<registry::Type> _semantic_type) noexcept{return check_names_semantic_field_values(field_values, self);}

names::NameCheckResult NamesPass_visit_record_update(names::NamesPass self, mlc::String _type_name, std::shared_ptr<semantic_ir::SemanticExpression> base_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, std::shared_ptr<registry::Type> _semantic_type) noexcept{return merge_name_check_results(dispatch_names_pass(self, base_expression), check_names_semantic_field_values(overrides, self));}

names::NameCheckResult NamesPass_visit_array(names::NamesPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> _semantic_type) noexcept{return check_names_semantic_elements(elements, self);}

names::NameCheckResult NamesPass_visit_tuple(names::NamesPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> _semantic_type) noexcept{return check_names_semantic_elements(elements, self);}

names::NameCheckResult NamesPass_visit_question(names::NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{return dispatch_names_pass(self, inner_expression);}

names::NameCheckResult NamesPass_visit_lambda(names::NamesPass self, mlc::Array<mlc::String> parameter_names, std::shared_ptr<semantic_ir::SemanticExpression> body_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{
mlc::Array<mlc::String> lambda_environment = self.locals;
int parameter_index = 0;
while (parameter_index < parameter_names.size()){
{
lambda_environment = lambda_environment.concat(mlc::Array<mlc::String>{parameter_names[parameter_index]});
parameter_index = parameter_index + 1;
}
}
return dispatch_names_pass(names_pass_new(lambda_environment, self.globals), body_expression);
}

names::NameCheckResult NamesPass_visit_with(names::NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> resource, mlc::String binder, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept{
mlc::Array<mlc::String> inner_scope = self.locals.concat(mlc::Array<mlc::String>{binder});
names::NameCheckResult statements_check = check_names_semantic_statements(statements, inner_scope, self.globals);
return names::NameCheckResult{ast::diagnostics_append(dispatch_names_pass(self, resource).diagnostics, statements_check.diagnostics), self.locals};
}

names::NameCheckResult NamesPass_visit_unsupported(names::NamesPass self, std::shared_ptr<semantic_ir::SemanticExpression> _expression) noexcept{return name_check_ok(self.locals);}

mlc::Array<ast::Diagnostic> check_names_semantic_expression(std::shared_ptr<semantic_ir::SemanticExpression> expression, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
names::NameCheckResult check_result = dispatch_names_pass(names_pass_new(locals, globals), expression);
return check_result.diagnostics;
}

std::shared_ptr<semantic_ir::SemanticExpression> transform_parser_expression_for_names(std::shared_ptr<ast::Expr> expression) noexcept{return transform::transform_expr(expression, transform::empty_transform_context(), [](mlc::Array<std::shared_ptr<ast::Stmt>> statements, transform::TransformContext nested_transform_context) mutable { return transform_stmts::transform_stmts(statements, nested_transform_context); });}

mlc::Array<ast::Diagnostic> check_names_expr(std::shared_ptr<ast::Expr> expression, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{return check_names_semantic_expression(transform_parser_expression_for_names(expression), locals, globals);}

names::NameCheckResult check_names_statements(mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{return statements.fold(names::NameCheckResult{{}, locals}, [globals](names::NameCheckResult accumulator, std::shared_ptr<ast::Stmt> statement_under_walk) mutable { return std::visit(overloaded{
  [&](const StmtLet& stmtlet) -> names::NameCheckResult { auto [name, _w0, _w1, value, _w2] = stmtlet; return names::NameCheckResult{ast::diagnostics_append(accumulator.diagnostics, check_names_expr(value, accumulator.scope, globals)), accumulator.scope.concat(mlc::Array<mlc::String>{name})}; },
  [&](const StmtLetPattern& stmtletpattern) -> names::NameCheckResult { auto [pattern, _w0, _w1, value, has_else, else_body, _w2] = stmtletpattern; return [&]() -> names::NameCheckResult { 
  mlc::Array<mlc::String> scope_after_value = accumulator.scope;
  mlc::Array<ast::Diagnostic> diagnostics_after_pattern = ast::diagnostics_append(accumulator.diagnostics, check_names_expr(value, scope_after_value, globals));
  mlc::Array<mlc::String> scope_after_bindings = scope_with_bindings(scope_after_value, pattern_bindings(pattern));
  if (has_else){
{
diagnostics_after_pattern = ast::diagnostics_append(diagnostics_after_pattern, check_names_expr(else_body, scope_after_bindings, globals));
}
}
  return names::NameCheckResult{diagnostics_after_pattern, scope_after_bindings};
 }(); },
  [&](const StmtLetConst& stmtletconst) -> names::NameCheckResult { auto [name, _w0, value, _w1] = stmtletconst; return names::NameCheckResult{ast::diagnostics_append(accumulator.diagnostics, check_names_expr(value, accumulator.scope, globals)), accumulator.scope.concat(mlc::Array<mlc::String>{name})}; },
  [&](const StmtExpr& stmtexpr) -> names::NameCheckResult { auto [expression, _w0] = stmtexpr; return names::NameCheckResult{ast::diagnostics_append(accumulator.diagnostics, check_names_expr(expression, accumulator.scope, globals)), accumulator.scope}; },
  [&](const StmtReturn& stmtreturn) -> names::NameCheckResult { auto [expression, _w0] = stmtreturn; return names::NameCheckResult{ast::diagnostics_append(accumulator.diagnostics, check_names_expr(expression, accumulator.scope, globals)), accumulator.scope}; },
  [&](const StmtBreak& stmtbreak) -> names::NameCheckResult { auto [_w0] = stmtbreak; return accumulator; },
  [&](const StmtContinue& stmtcontinue) -> names::NameCheckResult { auto [_w0] = stmtcontinue; return accumulator; }
}, (*statement_under_walk)._); });}

} // namespace names
