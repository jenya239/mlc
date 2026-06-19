#include "names.hpp"

#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "expr_visitor.hpp"
#include "diagnostic_codes.hpp"

namespace names {

using namespace ast;
using namespace semantic_ir;
using namespace registry;
using namespace expr_visitor;
using namespace diagnostic_codes;
using namespace ast_tokens;

struct NamesPass {mlc::Array<mlc::String> locals;mlc::HashMap<mlc::String, bool> globals;int seed;};

bool scope_contains(mlc::Array<mlc::String> scope, mlc::String name) noexcept;

mlc::Array<mlc::String> pattern_bindings(std::shared_ptr<ast::Pattern> pattern) noexcept;

mlc::Array<mlc::String> collect_pattern_bindings(std::shared_ptr<ast::Pattern> pattern, mlc::Array<mlc::String> bindings_so_far) noexcept;

mlc::Array<ast::Diagnostic> check_names_identifier(mlc::String name, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals, ast::Span source_span) noexcept;

mlc::Array<ast::Diagnostic> check_names_binary_expression(std::shared_ptr<ast::Expr> left, std::shared_ptr<ast::Expr> right, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

mlc::Array<ast::Diagnostic> check_names_unary_expression(std::shared_ptr<ast::Expr> inner_expression, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

mlc::Array<ast::Diagnostic> check_names_call_expression(std::shared_ptr<ast::Expr> function, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

mlc::Array<ast::Diagnostic> check_names_method_expression(std::shared_ptr<ast::Expr> object, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

mlc::Array<ast::Diagnostic> check_names_field_expression(std::shared_ptr<ast::Expr> object, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

mlc::Array<ast::Diagnostic> check_names_index_expression(std::shared_ptr<ast::Expr> object, std::shared_ptr<ast::Expr> index_expression, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

mlc::Array<ast::Diagnostic> check_names_conditional_expression(std::shared_ptr<ast::Expr> condition, std::shared_ptr<ast::Expr> then_expression, std::shared_ptr<ast::Expr> else_expression, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

mlc::Array<ast::Diagnostic> check_names_block_expression(mlc::Array<std::shared_ptr<ast::Stmt>> statements, std::shared_ptr<ast::Expr> result, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

mlc::Array<ast::Diagnostic> check_names_while_expression(std::shared_ptr<ast::Expr> condition, mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

mlc::Array<mlc::String> scope_with_bindings(mlc::Array<mlc::String> scope, mlc::Array<mlc::String> binding_names) noexcept;

mlc::Array<ast::Diagnostic> check_names_for_expression(mlc::String variable_name, std::shared_ptr<ast::Expr> iterator, mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

mlc::Array<ast::Diagnostic> check_names_with_expression(std::shared_ptr<ast::Expr> resource, mlc::String binder, mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

mlc::Array<ast::Diagnostic> check_names_match_expression(std::shared_ptr<ast::Expr> subject, mlc::Array<std::shared_ptr<ast::MatchArm>> arms, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

mlc::Array<ast::Diagnostic> diagnostics_for_single_record_literal_name_part(ast::RecordLitPart literal_part, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

mlc::Array<ast::Diagnostic> check_names_record_lit_parts(mlc::Array<ast::RecordLitPart> lit_parts, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

mlc::Array<ast::Diagnostic> check_names_record_expression(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

mlc::Array<ast::Diagnostic> check_names_record_update_expression(std::shared_ptr<ast::Expr> base, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

mlc::Array<ast::Diagnostic> check_names_array_expression(mlc::Array<std::shared_ptr<ast::Expr>> elements, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

mlc::Array<ast::Diagnostic> check_names_question_expression(std::shared_ptr<ast::Expr> inner_expression, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

mlc::Array<ast::Diagnostic> check_names_lambda_expression(mlc::Array<mlc::String> parameter_names, std::shared_ptr<ast::Expr> body, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

mlc::Array<ast::Diagnostic> check_names_empty() noexcept;

mlc::Array<ast::Diagnostic> check_names_expr(std::shared_ptr<ast::Expr> expression, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

mlc::Array<ast::Diagnostic> NameCheckResult_append_expression_diagnostics(names::NameCheckResult self, mlc::Array<ast::Diagnostic> expression_diagnostics) noexcept;

names::NameCheckResult NameCheckResult_append_diagnostics(names::NameCheckResult self, mlc::Array<ast::Diagnostic> extra_diagnostics) noexcept;

names::NameCheckResult NameCheckResult_with_scope(names::NameCheckResult self, mlc::Array<mlc::String> new_scope) noexcept;

names::NameCheckResult NameCheckResult_merge(names::NameCheckResult self, names::NameCheckResult other) noexcept;

names::NameCheckResult check_names_statements(mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

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

bool scope_contains(mlc::Array<mlc::String> scope, mlc::String name) noexcept{return scope.any([name](mlc::String scope_entry) mutable { return scope_entry == name; });}

mlc::Array<mlc::String> pattern_bindings(std::shared_ptr<ast::Pattern> pattern) noexcept{
mlc::Array<mlc::String> empty_pattern_bindings = {};
return collect_pattern_bindings(pattern, empty_pattern_bindings);
}

mlc::Array<mlc::String> collect_pattern_bindings(std::shared_ptr<ast::Pattern> pattern, mlc::Array<mlc::String> bindings_so_far) noexcept{return [&]() -> mlc::Array<mlc::String> { if (std::holds_alternative<ast::PatternIdent>((*pattern))) { auto _v_patternident = std::get<ast::PatternIdent>((*pattern)); auto [name, _w0] = _v_patternident; return bindings_so_far.concat(mlc::Array<mlc::String>{name}); } if (std::holds_alternative<ast::PatternCtor>((*pattern))) { auto _v_patternctor = std::get<ast::PatternCtor>((*pattern)); auto [_w0, sub_patterns, _w1] = _v_patternctor; return sub_patterns.fold(bindings_so_far, [](mlc::Array<mlc::String> accumulator, std::shared_ptr<ast::Pattern> sub_pattern) mutable { return collect_pattern_bindings(sub_pattern, accumulator); }); } if (std::holds_alternative<ast::PatternRecord>((*pattern))) { auto _v_patternrecord = std::get<ast::PatternRecord>((*pattern)); auto [_w0, field_patterns, _w1] = _v_patternrecord; return field_patterns.fold(bindings_so_far, [](mlc::Array<mlc::String> accumulator, std::shared_ptr<ast::Pattern> field_pattern) mutable { return collect_pattern_bindings(field_pattern, accumulator); }); } if (std::holds_alternative<ast::PatternTuple>((*pattern))) { auto _v_patterntuple = std::get<ast::PatternTuple>((*pattern)); auto [sub_patterns, _w0] = _v_patterntuple; return sub_patterns.fold(bindings_so_far, [](mlc::Array<mlc::String> accumulator, std::shared_ptr<ast::Pattern> sub_pattern) mutable { return collect_pattern_bindings(sub_pattern, accumulator); }); } if (std::holds_alternative<ast::PatternArray>((*pattern))) { auto _v_patternarray = std::get<ast::PatternArray>((*pattern)); auto [sub_patterns, rest, _w0] = _v_patternarray; return [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> bindings_after_array_elements = sub_patterns.fold(bindings_so_far, [](mlc::Array<mlc::String> accumulator, std::shared_ptr<ast::Pattern> sub_pattern) mutable { return collect_pattern_bindings(sub_pattern, accumulator); });
  return rest != mlc::String("") && rest != mlc::String("_") ? bindings_after_array_elements.concat(mlc::Array<mlc::String>{rest}) : bindings_after_array_elements;
 }(); } if (std::holds_alternative<ast::PatternOr>((*pattern))) { auto _v_patternor = std::get<ast::PatternOr>((*pattern)); auto [alternatives, _w0] = _v_patternor; return alternatives.size() > 0 ? collect_pattern_bindings(alternatives[0], bindings_so_far) : bindings_so_far; } return bindings_so_far; }();}

mlc::Array<ast::Diagnostic> check_names_identifier(mlc::String name, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals, ast::Span source_span) noexcept{return !scope_contains(locals, name) && !globals.has(name) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("undefined: ") + name, source_span, diagnostic_codes::diagnostic_code_e001())} : [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> empty_ident_diagnostics = {};
  return empty_ident_diagnostics;
 }();}

mlc::Array<ast::Diagnostic> check_names_binary_expression(std::shared_ptr<ast::Expr> left, std::shared_ptr<ast::Expr> right, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{return ast::diagnostics_append(check_names_expr(left, locals, globals), check_names_expr(right, locals, globals));}

mlc::Array<ast::Diagnostic> check_names_unary_expression(std::shared_ptr<ast::Expr> inner_expression, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{return check_names_expr(inner_expression, locals, globals);}

mlc::Array<ast::Diagnostic> check_names_call_expression(std::shared_ptr<ast::Expr> function, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
mlc::Array<ast::Diagnostic> diagnostics_from_callable = check_names_expr(function, locals, globals);
return call_arguments.fold(diagnostics_from_callable, [locals, globals](mlc::Array<ast::Diagnostic> diagnostics_accumulator, std::shared_ptr<ast::Expr> argument_under_call) mutable { return ast::diagnostics_append(diagnostics_accumulator, check_names_expr(argument_under_call, locals, globals)); });
}

mlc::Array<ast::Diagnostic> check_names_method_expression(std::shared_ptr<ast::Expr> object, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
mlc::Array<ast::Diagnostic> diagnostics_from_receiver = check_names_expr(object, locals, globals);
return method_arguments.fold(diagnostics_from_receiver, [locals, globals](mlc::Array<ast::Diagnostic> diagnostics_accumulator, std::shared_ptr<ast::Expr> argument_under_method_call) mutable { return ast::diagnostics_append(diagnostics_accumulator, check_names_expr(argument_under_method_call, locals, globals)); });
}

mlc::Array<ast::Diagnostic> check_names_field_expression(std::shared_ptr<ast::Expr> object, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{return check_names_expr(object, locals, globals);}

mlc::Array<ast::Diagnostic> check_names_index_expression(std::shared_ptr<ast::Expr> object, std::shared_ptr<ast::Expr> index_expression, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{return ast::diagnostics_append(check_names_expr(object, locals, globals), check_names_expr(index_expression, locals, globals));}

mlc::Array<ast::Diagnostic> check_names_conditional_expression(std::shared_ptr<ast::Expr> condition, std::shared_ptr<ast::Expr> then_expression, std::shared_ptr<ast::Expr> else_expression, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{return ast::diagnostics_append(ast::diagnostics_append(check_names_expr(condition, locals, globals), check_names_expr(then_expression, locals, globals)), check_names_expr(else_expression, locals, globals));}

mlc::Array<ast::Diagnostic> check_names_block_expression(mlc::Array<std::shared_ptr<ast::Stmt>> statements, std::shared_ptr<ast::Expr> result, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
names::NameCheckResult statements_parsed = check_names_statements(statements, locals, globals);
return NameCheckResult_append_expression_diagnostics(statements_parsed, check_names_expr(result, statements_parsed.scope, globals));
}

mlc::Array<ast::Diagnostic> check_names_while_expression(std::shared_ptr<ast::Expr> condition, mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
names::NameCheckResult statement_check = check_names_statements(statements, locals, globals);
return ast::diagnostics_append(check_names_expr(condition, locals, globals), statement_check.diagnostics);
}

mlc::Array<mlc::String> scope_with_bindings(mlc::Array<mlc::String> scope, mlc::Array<mlc::String> binding_names) noexcept{return binding_names.fold(scope, [](mlc::Array<mlc::String> accumulated_scope, mlc::String binding_name) mutable { return accumulated_scope.concat(mlc::Array<mlc::String>{binding_name}); });}

mlc::Array<ast::Diagnostic> check_names_for_expression(mlc::String variable_name, std::shared_ptr<ast::Expr> iterator, mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
mlc::Array<mlc::String> inner_scope = locals.concat(mlc::Array<mlc::String>{variable_name});
names::NameCheckResult statement_check = check_names_statements(statements, inner_scope, globals);
return ast::diagnostics_append(check_names_expr(iterator, locals, globals), statement_check.diagnostics);
}

mlc::Array<ast::Diagnostic> check_names_with_expression(std::shared_ptr<ast::Expr> resource, mlc::String binder, mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
mlc::Array<mlc::String> inner_scope = locals.concat(mlc::Array<mlc::String>{binder});
names::NameCheckResult statements_check = check_names_statements(statements, inner_scope, globals);
return ast::diagnostics_append(check_names_expr(resource, locals, globals), statements_check.diagnostics);
}

mlc::Array<ast::Diagnostic> check_names_match_expression(std::shared_ptr<ast::Expr> subject, mlc::Array<std::shared_ptr<ast::MatchArm>> arms, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
mlc::Array<ast::Diagnostic> diagnostics_after_subject = check_names_expr(subject, locals, globals);
return arms.fold(diagnostics_after_subject, [locals, globals](mlc::Array<ast::Diagnostic> diagnostics_accumulator, std::shared_ptr<ast::MatchArm> arm_under_walk) mutable { return [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<mlc::String> arm_scope = scope_with_bindings(locals, pattern_bindings(arm_under_walk->pattern));
  mlc::Array<ast::Diagnostic> after_guard = arm_under_walk->has_guard ? ast::diagnostics_append(diagnostics_accumulator, check_names_expr(arm_under_walk->when_condition, arm_scope, globals)) : diagnostics_accumulator;
  return ast::diagnostics_append(after_guard, check_names_expr(arm_under_walk->body, arm_scope, globals));
 }(); });
}

mlc::Array<ast::Diagnostic> diagnostics_for_single_record_literal_name_part(ast::RecordLitPart literal_part, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{return std::visit(overloaded{
  [&](const RecordLitFields& recordlitfields) -> mlc::Array<ast::Diagnostic> { auto [field_values] = recordlitfields; return check_names_record_expression(field_values, locals, globals); },
  [&](const RecordLitSpread& recordlitspread) -> mlc::Array<ast::Diagnostic> { auto [spread_expression] = recordlitspread; return check_names_expr(spread_expression, locals, globals); }
}, literal_part);}

mlc::Array<ast::Diagnostic> check_names_record_lit_parts(mlc::Array<ast::RecordLitPart> lit_parts, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{return mlc::collections::flat_map(lit_parts, [locals, globals](ast::RecordLitPart literal_part_under_walk) mutable { return diagnostics_for_single_record_literal_name_part(literal_part_under_walk, locals, globals); });}

mlc::Array<ast::Diagnostic> check_names_record_expression(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{return field_values.fold([&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> starting_record_names = {};
  return starting_record_names;
 }(), [locals, globals](mlc::Array<ast::Diagnostic> diagnostics_accumulator, std::shared_ptr<ast::FieldVal> field_value_under_walk) mutable { return ast::diagnostics_append(diagnostics_accumulator, check_names_expr(field_value_under_walk->value, locals, globals)); });}

mlc::Array<ast::Diagnostic> check_names_record_update_expression(std::shared_ptr<ast::Expr> base, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{return field_values.fold(check_names_expr(base, locals, globals), [locals, globals](mlc::Array<ast::Diagnostic> diagnostics_accumulator, std::shared_ptr<ast::FieldVal> field_value_under_walk) mutable { return ast::diagnostics_append(diagnostics_accumulator, check_names_expr(field_value_under_walk->value, locals, globals)); });}

mlc::Array<ast::Diagnostic> check_names_array_expression(mlc::Array<std::shared_ptr<ast::Expr>> elements, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{return elements.fold([&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> starting_array_names = {};
  return starting_array_names;
 }(), [locals, globals](mlc::Array<ast::Diagnostic> diagnostics_accumulator, std::shared_ptr<ast::Expr> element_under_walk) mutable { return ast::diagnostics_append(diagnostics_accumulator, check_names_expr(element_under_walk, locals, globals)); });}

mlc::Array<ast::Diagnostic> check_names_question_expression(std::shared_ptr<ast::Expr> inner_expression, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{return check_names_expr(inner_expression, locals, globals);}

mlc::Array<ast::Diagnostic> check_names_lambda_expression(mlc::Array<mlc::String> parameter_names, std::shared_ptr<ast::Expr> body, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
mlc::Array<mlc::String> lambda_environment_names = parameter_names.fold(locals, [](mlc::Array<mlc::String> accumulated_scope_names, mlc::String lambda_parameter_name) mutable { return accumulated_scope_names.concat(mlc::Array<mlc::String>{lambda_parameter_name}); });
return check_names_expr(body, lambda_environment_names, globals);
}

mlc::Array<ast::Diagnostic> check_names_empty() noexcept{
mlc::Array<ast::Diagnostic> empty_list = {};
return empty_list;
}

mlc::Array<ast::Diagnostic> check_names_expr(std::shared_ptr<ast::Expr> expression, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{return [&]() -> mlc::Array<ast::Diagnostic> { if (std::holds_alternative<ast::ExprIdent>((*expression)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*expression)._); auto [name, source_span] = _v_exprident; return check_names_identifier(name, locals, globals, source_span); } if (std::holds_alternative<ast::ExprBin>((*expression)._)) { auto _v_exprbin = std::get<ast::ExprBin>((*expression)._); auto [_w0, left, right, _w1] = _v_exprbin; return check_names_binary_expression(left, right, locals, globals); } if (std::holds_alternative<ast::ExprUn>((*expression)._)) { auto _v_exprun = std::get<ast::ExprUn>((*expression)._); auto [_w0, inner_expression, _w1] = _v_exprun; return check_names_unary_expression(inner_expression, locals, globals); } if (std::holds_alternative<ast::ExprCall>((*expression)._)) { auto _v_exprcall = std::get<ast::ExprCall>((*expression)._); auto [function, call_arguments, _w0] = _v_exprcall; return check_names_call_expression(function, call_arguments, locals, globals); } if (std::holds_alternative<ast::ExprMethod>((*expression)._)) { auto _v_exprmethod = std::get<ast::ExprMethod>((*expression)._); auto [object, _w0, method_arguments, _w1] = _v_exprmethod; return check_names_method_expression(object, method_arguments, locals, globals); } if (std::holds_alternative<ast::ExprField>((*expression)._)) { auto _v_exprfield = std::get<ast::ExprField>((*expression)._); auto [object, _w0, _w1] = _v_exprfield; return check_names_field_expression(object, locals, globals); } if (std::holds_alternative<ast::ExprIndex>((*expression)._)) { auto _v_exprindex = std::get<ast::ExprIndex>((*expression)._); auto [object, index_expression, _w0] = _v_exprindex; return check_names_index_expression(object, index_expression, locals, globals); } if (std::holds_alternative<ast::ExprIf>((*expression)._)) { auto _v_exprif = std::get<ast::ExprIf>((*expression)._); auto [condition, then_expression, else_expression, _w0] = _v_exprif; return check_names_conditional_expression(condition, then_expression, else_expression, locals, globals); } if (std::holds_alternative<ast::ExprBlock>((*expression)._)) { auto _v_exprblock = std::get<ast::ExprBlock>((*expression)._); auto [statements, result, _w0] = _v_exprblock; return check_names_block_expression(statements, result, locals, globals); } if (std::holds_alternative<ast::ExprWhile>((*expression)._)) { auto _v_exprwhile = std::get<ast::ExprWhile>((*expression)._); auto [condition, statements, _w0] = _v_exprwhile; return check_names_while_expression(condition, statements, locals, globals); } if (std::holds_alternative<ast::ExprFor>((*expression)._)) { auto _v_exprfor = std::get<ast::ExprFor>((*expression)._); auto [variable_name, iterator, statements, _w0] = _v_exprfor; return check_names_for_expression(variable_name, iterator, statements, locals, globals); } if (std::holds_alternative<ast::ExprMatch>((*expression)._)) { auto _v_exprmatch = std::get<ast::ExprMatch>((*expression)._); auto [subject, arms, _w0] = _v_exprmatch; return check_names_match_expression(subject, arms, locals, globals); } if (std::holds_alternative<ast::ExprRecord>((*expression)._)) { auto _v_exprrecord = std::get<ast::ExprRecord>((*expression)._); auto [_w0, lit_parts, _w1] = _v_exprrecord; return check_names_record_lit_parts(lit_parts, locals, globals); } if (std::holds_alternative<ast::ExprRecordUpdate>((*expression)._)) { auto _v_exprrecordupdate = std::get<ast::ExprRecordUpdate>((*expression)._); auto [_w0, base, field_values, _w1] = _v_exprrecordupdate; return check_names_record_update_expression(base, field_values, locals, globals); } if (std::holds_alternative<ast::ExprArray>((*expression)._)) { auto _v_exprarray = std::get<ast::ExprArray>((*expression)._); auto [elements, _w0] = _v_exprarray; return check_names_array_expression(elements, locals, globals); } if (std::holds_alternative<ast::ExprTuple>((*expression)._)) { auto _v_exprtuple = std::get<ast::ExprTuple>((*expression)._); auto [elements, _w0] = _v_exprtuple; return check_names_array_expression(elements, locals, globals); } if (std::holds_alternative<ast::ExprQuestion>((*expression)._)) { auto _v_exprquestion = std::get<ast::ExprQuestion>((*expression)._); auto [inner_expression, _w0] = _v_exprquestion; return check_names_question_expression(inner_expression, locals, globals); } if (std::holds_alternative<ast::ExprLambda>((*expression)._)) { auto _v_exprlambda = std::get<ast::ExprLambda>((*expression)._); auto [parameter_names, body, _w0] = _v_exprlambda; return check_names_lambda_expression(parameter_names, body, locals, globals); } if (std::holds_alternative<ast::ExprNamedArg>((*expression)._)) { auto _v_exprnamedarg = std::get<ast::ExprNamedArg>((*expression)._); auto [_w0, inner_expression, _w1] = _v_exprnamedarg; return check_names_expr(inner_expression, locals, globals); } if (std::holds_alternative<ast::ExprWith>((*expression)._)) { auto _v_exprwith = std::get<ast::ExprWith>((*expression)._); auto [resource, binder, statements, _w0] = _v_exprwith; return check_names_with_expression(resource, binder, statements, locals, globals); } return check_names_empty(); }();}

mlc::Array<ast::Diagnostic> NameCheckResult_append_expression_diagnostics(names::NameCheckResult self, mlc::Array<ast::Diagnostic> expression_diagnostics) noexcept{return ast::diagnostics_append(self.diagnostics, expression_diagnostics);}

names::NameCheckResult NameCheckResult_append_diagnostics(names::NameCheckResult self, mlc::Array<ast::Diagnostic> extra_diagnostics) noexcept{return names::NameCheckResult{ast::diagnostics_append(self.diagnostics, extra_diagnostics), self.scope};}

names::NameCheckResult NameCheckResult_with_scope(names::NameCheckResult self, mlc::Array<mlc::String> new_scope) noexcept{return names::NameCheckResult{self.diagnostics, new_scope};}

names::NameCheckResult NameCheckResult_merge(names::NameCheckResult self, names::NameCheckResult other) noexcept{return names::NameCheckResult{ast::diagnostics_append(self.diagnostics, other.diagnostics), other.scope};}

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

names::NamesPass names_pass_new(mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{return names::NamesPass{locals, globals, 0};}

names::NameCheckResult name_check_ok(mlc::Array<mlc::String> scope) noexcept{return names::NameCheckResult{check_names_empty(), scope};}

names::NameCheckResult merge_name_check_results(names::NameCheckResult first, names::NameCheckResult second) noexcept{return NameCheckResult_merge(first, second);}

names::NameCheckResult dispatch_names_pass(names::NamesPass names_pass, std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{return [&]() -> names::NameCheckResult { if (std::holds_alternative<semantic_ir::SemanticExpressionInt>((*expression)._)) { auto _v_semanticexpressionint = std::get<semantic_ir::SemanticExpressionInt>((*expression)._); auto [integer_value, type_value, _w0] = _v_semanticexpressionint; return NamesPass_visit_int(names_pass, integer_value, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionStr>((*expression)._)) { auto _v_semanticexpressionstr = std::get<semantic_ir::SemanticExpressionStr>((*expression)._); auto [string_value, type_value, _w0] = _v_semanticexpressionstr; return NamesPass_visit_str(names_pass, string_value, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionBool>((*expression)._)) { auto _v_semanticexpressionbool = std::get<semantic_ir::SemanticExpressionBool>((*expression)._); auto [boolean_value, type_value, _w0] = _v_semanticexpressionbool; return NamesPass_visit_bool(names_pass, boolean_value, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionUnit>((*expression)._)) { auto _v_semanticexpressionunit = std::get<semantic_ir::SemanticExpressionUnit>((*expression)._); auto [type_value, _w0] = _v_semanticexpressionunit; return NamesPass_visit_unit(names_pass, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionFloat>((*expression)._)) { auto _v_semanticexpressionfloat = std::get<semantic_ir::SemanticExpressionFloat>((*expression)._); auto [float_value, type_value, _w0] = _v_semanticexpressionfloat; return NamesPass_visit_float(names_pass, float_value, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionI64>((*expression)._)) { auto _v_semanticexpressioni64 = std::get<semantic_ir::SemanticExpressionI64>((*expression)._); auto [integer_value, type_value, _w0] = _v_semanticexpressioni64; return NamesPass_visit_i64(names_pass, integer_value, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionU8>((*expression)._)) { auto _v_semanticexpressionu8 = std::get<semantic_ir::SemanticExpressionU8>((*expression)._); auto [byte_value, type_value, _w0] = _v_semanticexpressionu8; return NamesPass_visit_u8(names_pass, byte_value, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionUsize>((*expression)._)) { auto _v_semanticexpressionusize = std::get<semantic_ir::SemanticExpressionUsize>((*expression)._); auto [size_value, type_value, _w0] = _v_semanticexpressionusize; return NamesPass_visit_usize(names_pass, size_value, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionChar>((*expression)._)) { auto _v_semanticexpressionchar = std::get<semantic_ir::SemanticExpressionChar>((*expression)._); auto [character_value, type_value, _w0] = _v_semanticexpressionchar; return NamesPass_visit_char(names_pass, character_value, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionExtern>((*expression)._)) { auto _v_semanticexpressionextern = std::get<semantic_ir::SemanticExpressionExtern>((*expression)._); auto [type_value, _w0] = _v_semanticexpressionextern; return NamesPass_visit_extern(names_pass, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionIdent>((*expression)._)) { auto _v_semanticexpressionident = std::get<semantic_ir::SemanticExpressionIdent>((*expression)._); auto [name_value, type_value, _w0] = _v_semanticexpressionident; return NamesPass_visit_ident(names_pass, name_value, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionBin>((*expression)._)) { auto _v_semanticexpressionbin = std::get<semantic_ir::SemanticExpressionBin>((*expression)._); auto [operation, left_expression, right_expression, type_value, _w0] = _v_semanticexpressionbin; return NamesPass_visit_bin(names_pass, operation, left_expression, right_expression, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionUn>((*expression)._)) { auto _v_semanticexpressionun = std::get<semantic_ir::SemanticExpressionUn>((*expression)._); auto [operation, inner_expression, type_value, _w0] = _v_semanticexpressionun; return NamesPass_visit_un(names_pass, operation, inner_expression, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionCall>((*expression)._)) { auto _v_semanticexpressioncall = std::get<semantic_ir::SemanticExpressionCall>((*expression)._); auto [function_expression, arguments, mutability_flags, type_value, _w0] = _v_semanticexpressioncall; return NamesPass_visit_call(names_pass, function_expression, arguments, mutability_flags, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionMethod>((*expression)._)) { auto _v_semanticexpressionmethod = std::get<semantic_ir::SemanticExpressionMethod>((*expression)._); auto [object, method_name, arguments, mutability_flags, type_value, _w0] = _v_semanticexpressionmethod; return NamesPass_visit_method(names_pass, object, method_name, arguments, mutability_flags, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionField>((*expression)._)) { auto _v_semanticexpressionfield = std::get<semantic_ir::SemanticExpressionField>((*expression)._); auto [object, field_name, type_value, _w0] = _v_semanticexpressionfield; return NamesPass_visit_field(names_pass, object, field_name, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionIndex>((*expression)._)) { auto _v_semanticexpressionindex = std::get<semantic_ir::SemanticExpressionIndex>((*expression)._); auto [object, index_expression, type_value, _w0] = _v_semanticexpressionindex; return NamesPass_visit_index(names_pass, object, index_expression, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*expression)._)) { auto _v_semanticexpressionif = std::get<semantic_ir::SemanticExpressionIf>((*expression)._); auto [condition, then_expression, else_expression, type_value, _w0] = _v_semanticexpressionif; return NamesPass_visit_if(names_pass, condition, then_expression, else_expression, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionBlock>((*expression)._)) { auto _v_semanticexpressionblock = std::get<semantic_ir::SemanticExpressionBlock>((*expression)._); auto [statements, result_expression, type_value, _w0] = _v_semanticexpressionblock; return NamesPass_visit_block(names_pass, statements, result_expression, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionWhile>((*expression)._)) { auto _v_semanticexpressionwhile = std::get<semantic_ir::SemanticExpressionWhile>((*expression)._); auto [condition, statements, type_value, _w0] = _v_semanticexpressionwhile; return NamesPass_visit_while(names_pass, condition, statements, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionFor>((*expression)._)) { auto _v_semanticexpressionfor = std::get<semantic_ir::SemanticExpressionFor>((*expression)._); auto [variable, iterator, statements, type_value, _w0] = _v_semanticexpressionfor; return NamesPass_visit_for(names_pass, variable, iterator, statements, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionMatch>((*expression)._)) { auto _v_semanticexpressionmatch = std::get<semantic_ir::SemanticExpressionMatch>((*expression)._); auto [subject, arms, type_value, _w0] = _v_semanticexpressionmatch; return NamesPass_visit_match(names_pass, subject, arms, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionRecord>((*expression)._)) { auto _v_semanticexpressionrecord = std::get<semantic_ir::SemanticExpressionRecord>((*expression)._); auto [type_name, field_values, type_value, _w0] = _v_semanticexpressionrecord; return NamesPass_visit_record(names_pass, type_name, field_values, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionRecordUpdate>((*expression)._)) { auto _v_semanticexpressionrecordupdate = std::get<semantic_ir::SemanticExpressionRecordUpdate>((*expression)._); auto [type_name, base_expression, overrides, type_value, _w0] = _v_semanticexpressionrecordupdate; return NamesPass_visit_record_update(names_pass, type_name, base_expression, overrides, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionArray>((*expression)._)) { auto _v_semanticexpressionarray = std::get<semantic_ir::SemanticExpressionArray>((*expression)._); auto [elements, type_value, _w0] = _v_semanticexpressionarray; return NamesPass_visit_array(names_pass, elements, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionTuple>((*expression)._)) { auto _v_semanticexpressiontuple = std::get<semantic_ir::SemanticExpressionTuple>((*expression)._); auto [elements, type_value, _w0] = _v_semanticexpressiontuple; return NamesPass_visit_tuple(names_pass, elements, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionQuestion>((*expression)._)) { auto _v_semanticexpressionquestion = std::get<semantic_ir::SemanticExpressionQuestion>((*expression)._); auto [inner_expression, type_value, _w0] = _v_semanticexpressionquestion; return NamesPass_visit_question(names_pass, inner_expression, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionLambda>((*expression)._)) { auto _v_semanticexpressionlambda = std::get<semantic_ir::SemanticExpressionLambda>((*expression)._); auto [parameter_names, body_expression, type_value, _w0] = _v_semanticexpressionlambda; return NamesPass_visit_lambda(names_pass, parameter_names, body_expression, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionWith>((*expression)._)) { auto _v_semanticexpressionwith = std::get<semantic_ir::SemanticExpressionWith>((*expression)._); auto [resource, binder, statements, type_value, _w0] = _v_semanticexpressionwith; return NamesPass_visit_with(names_pass, resource, binder, statements, type_value); } return NamesPass_visit_unsupported(names_pass, expression); }();}

names::NameCheckResult check_names_semantic_field_values(mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, names::NamesPass names_pass) noexcept{return field_values.fold(name_check_ok(names_pass.locals), [names_pass](names::NameCheckResult accumulator, std::shared_ptr<semantic_ir::SemanticFieldVal> field_value_under_walk) mutable { return merge_name_check_results(accumulator, dispatch_names_pass(names_pass, field_value_under_walk->value)); });}

names::NameCheckResult check_names_semantic_elements(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, names::NamesPass names_pass) noexcept{return elements.fold(name_check_ok(names_pass.locals), [names_pass](names::NameCheckResult accumulator, std::shared_ptr<semantic_ir::SemanticExpression> element_under_walk) mutable { return merge_name_check_results(accumulator, dispatch_names_pass(names_pass, element_under_walk)); });}

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
mlc::Array<mlc::String> lambda_environment = parameter_names.fold(self.locals, [](mlc::Array<mlc::String> accumulated, mlc::String lambda_parameter_name) mutable { return accumulated.concat(mlc::Array<mlc::String>{lambda_parameter_name}); });
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

} // namespace names
