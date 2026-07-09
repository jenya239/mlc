#ifndef NAMES_HPP
#define NAMES_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "expr_visitor.hpp"
#include "diagnostic_codes.hpp"
#include "transform.hpp"
#include "transform_stmts.hpp"
namespace names {

struct NameCheckResult {
  mlc::Array<ast::Diagnostic> diagnostics;
  mlc::Array<mlc::String> scope;
};
struct NamesPass {
  mlc::Array<mlc::String> locals;
  mlc::HashMap<mlc::String, bool> globals;
  int seed;
};
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
mlc::Array<ast::Diagnostic> NameCheckResult_append_expression_diagnostics(NameCheckResult self, mlc::Array<ast::Diagnostic> expression_diagnostics) noexcept;
NameCheckResult NameCheckResult_append_diagnostics(NameCheckResult self, mlc::Array<ast::Diagnostic> extra_diagnostics) noexcept;
NameCheckResult NameCheckResult_with_scope(NameCheckResult self, mlc::Array<mlc::String> new_scope) noexcept;
NameCheckResult NameCheckResult_merge(NameCheckResult self, NameCheckResult other) noexcept;
NamesPass names_pass_new(mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;
NameCheckResult name_check_ok(mlc::Array<mlc::String> scope) noexcept;
NameCheckResult merge_name_check_results(NameCheckResult first, NameCheckResult second) noexcept;
NameCheckResult dispatch_names_pass(NamesPass names_pass, std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept;
NameCheckResult check_names_semantic_field_values(mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, NamesPass names_pass) noexcept;
NameCheckResult check_names_semantic_elements(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, NamesPass names_pass) noexcept;
NameCheckResult check_names_semantic_statements(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;
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
mlc::Array<ast::Diagnostic> check_names_semantic_expression(std::shared_ptr<semantic_ir::SemanticExpression> expression, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> transform_parser_expression_for_names(std::shared_ptr<ast::Expr> expression) noexcept;
mlc::Array<ast::Diagnostic> check_names_expr(std::shared_ptr<ast::Expr> expression, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;
NameCheckResult check_names_statements(mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;
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
static_assert(expr_visitor::ExprVisitor<NameCheckResult, NamesPass>, "NamesPass does not implement ExprVisitor");

} // namespace names

#endif // NAMES_HPP
