#ifndef CHECK_MUTATIONS_HPP
#define CHECK_MUTATIONS_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "expr_visitor.hpp"
#include "names.hpp"
#include "diagnostic_codes.hpp"
#include "transform.hpp"
#include "transform_stmts.hpp"
namespace check_mutations {

struct MutationCheckResult {
  mlc::Array<ast::Diagnostic> diagnostics;
  mlc::Array<mlc::String> mutable_locals;
};
struct MutationsPass {
  mlc::Array<mlc::String> mutable_locals;
  int seed;
};
bool scope_has(mlc::Array<mlc::String> binding_scope, mlc::String sought_name) noexcept;
mlc::Array<mlc::String> mutable_scope_with_binding(mlc::Array<mlc::String> binding_scope, mlc::String binding_name) noexcept;
mlc::Array<mlc::String> mutable_scope_with_bindings(mlc::Array<mlc::String> binding_scope, mlc::Array<mlc::String> binding_names) noexcept;
mlc::Array<mlc::String> mutable_scope_after_let(mlc::Array<mlc::String> binding_scope, mlc::String name, bool is_mut) noexcept;
mlc::Array<mlc::String> mutable_scope_after_pattern_let(mlc::Array<mlc::String> binding_scope, std::shared_ptr<ast::Pattern> pattern, bool is_mut) noexcept;
MutationsPass mutations_pass_new(mlc::Array<mlc::String> mutable_locals) noexcept;
mlc::Array<ast::Diagnostic> mutation_check_empty() noexcept;
mlc::Array<ast::Diagnostic> ident_assignment_mutation_errors(mlc::String binding_name, mlc::Array<mlc::String> mutable_locals, ast::Span source_span) noexcept;
mlc::Array<ast::Diagnostic> assignment_target_mutation_errors(std::shared_ptr<semantic_ir::SemanticExpression> left_expression, mlc::Array<mlc::String> mutable_locals) noexcept;
MutationCheckResult mutation_check_ok(mlc::Array<mlc::String> mutable_locals) noexcept;
MutationCheckResult merge_mutation_check_results(MutationCheckResult first, MutationCheckResult second) noexcept;
MutationCheckResult dispatch_mutations_pass(MutationsPass mutations_pass, std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept;
MutationCheckResult check_mutation_semantic_field_values(mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, MutationsPass mutations_pass) noexcept;
MutationCheckResult check_mutation_semantic_elements(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, MutationsPass mutations_pass) noexcept;
MutationCheckResult check_mutation_semantic_statements(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, mlc::Array<mlc::String> mutable_locals) noexcept;
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
mlc::Array<ast::Diagnostic> check_mutation_semantic_expression(std::shared_ptr<semantic_ir::SemanticExpression> expression, mlc::Array<mlc::String> mutable_locals) noexcept;
mlc::Array<ast::Diagnostic> check_fn_body_semantic_mutations(mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<semantic_ir::SemanticExpression> body) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> transform_parser_expression_for_mutations(std::shared_ptr<ast::Expr> expression) noexcept;
mlc::Array<ast::Diagnostic> check_fn_body_mutations(mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<ast::Expr> body) noexcept;
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
inline MutationCheckResult visit_int(MutationsPass self, int _integer_value, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_int(self, _integer_value, _semantic_type); }
inline MutationCheckResult ExprVisitor_visit_int(MutationsPass self, int _integer_value, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_int(self, _integer_value, _semantic_type); }
inline MutationCheckResult visit_str(MutationsPass self, mlc::String _string_value, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_str(self, _string_value, _semantic_type); }
inline MutationCheckResult ExprVisitor_visit_str(MutationsPass self, mlc::String _string_value, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_str(self, _string_value, _semantic_type); }
inline MutationCheckResult visit_bool(MutationsPass self, bool _boolean_value, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_bool(self, _boolean_value, _semantic_type); }
inline MutationCheckResult ExprVisitor_visit_bool(MutationsPass self, bool _boolean_value, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_bool(self, _boolean_value, _semantic_type); }
inline MutationCheckResult visit_unit(MutationsPass self, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_unit(self, _semantic_type); }
inline MutationCheckResult ExprVisitor_visit_unit(MutationsPass self, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_unit(self, _semantic_type); }
inline MutationCheckResult visit_float(MutationsPass self, mlc::String _float_value, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_float(self, _float_value, _semantic_type); }
inline MutationCheckResult ExprVisitor_visit_float(MutationsPass self, mlc::String _float_value, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_float(self, _float_value, _semantic_type); }
inline MutationCheckResult visit_i64(MutationsPass self, mlc::String _integer_value, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_i64(self, _integer_value, _semantic_type); }
inline MutationCheckResult ExprVisitor_visit_i64(MutationsPass self, mlc::String _integer_value, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_i64(self, _integer_value, _semantic_type); }
inline MutationCheckResult visit_u8(MutationsPass self, mlc::String _byte_value, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_u8(self, _byte_value, _semantic_type); }
inline MutationCheckResult ExprVisitor_visit_u8(MutationsPass self, mlc::String _byte_value, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_u8(self, _byte_value, _semantic_type); }
inline MutationCheckResult visit_usize(MutationsPass self, mlc::String _size_value, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_usize(self, _size_value, _semantic_type); }
inline MutationCheckResult ExprVisitor_visit_usize(MutationsPass self, mlc::String _size_value, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_usize(self, _size_value, _semantic_type); }
inline MutationCheckResult visit_char(MutationsPass self, mlc::String _character_value, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_char(self, _character_value, _semantic_type); }
inline MutationCheckResult ExprVisitor_visit_char(MutationsPass self, mlc::String _character_value, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_char(self, _character_value, _semantic_type); }
inline MutationCheckResult visit_extern(MutationsPass self, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_extern(self, _semantic_type); }
inline MutationCheckResult ExprVisitor_visit_extern(MutationsPass self, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_extern(self, _semantic_type); }
inline MutationCheckResult visit_ident(MutationsPass self, mlc::String _name, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_ident(self, _name, _semantic_type); }
inline MutationCheckResult ExprVisitor_visit_ident(MutationsPass self, mlc::String _name, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_ident(self, _name, _semantic_type); }
inline MutationCheckResult visit_bin(MutationsPass self, mlc::String operation, std::shared_ptr<semantic_ir::SemanticExpression> left_expression, std::shared_ptr<semantic_ir::SemanticExpression> right_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_bin(self, operation, left_expression, right_expression, _semantic_type); }
inline MutationCheckResult ExprVisitor_visit_bin(MutationsPass self, mlc::String operation, std::shared_ptr<semantic_ir::SemanticExpression> left_expression, std::shared_ptr<semantic_ir::SemanticExpression> right_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_bin(self, operation, left_expression, right_expression, _semantic_type); }
inline MutationCheckResult visit_un(MutationsPass self, mlc::String _operation, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_un(self, _operation, inner_expression, _semantic_type); }
inline MutationCheckResult ExprVisitor_visit_un(MutationsPass self, mlc::String _operation, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_un(self, _operation, inner_expression, _semantic_type); }
inline MutationCheckResult visit_call(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> function_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> _mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_call(self, function_expression, arguments, _mutability_flags, _semantic_type); }
inline MutationCheckResult ExprVisitor_visit_call(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> function_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> _mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_call(self, function_expression, arguments, _mutability_flags, _semantic_type); }
inline MutationCheckResult visit_method(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String _method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> _mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_method(self, object, _method_name, arguments, _mutability_flags, _semantic_type); }
inline MutationCheckResult ExprVisitor_visit_method(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String _method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> _mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_method(self, object, _method_name, arguments, _mutability_flags, _semantic_type); }
inline MutationCheckResult visit_field(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String _field_name, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_field(self, object, _field_name, _semantic_type); }
inline MutationCheckResult ExprVisitor_visit_field(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String _field_name, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_field(self, object, _field_name, _semantic_type); }
inline MutationCheckResult visit_index(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, std::shared_ptr<semantic_ir::SemanticExpression> index_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_index(self, object, index_expression, _semantic_type); }
inline MutationCheckResult ExprVisitor_visit_index(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, std::shared_ptr<semantic_ir::SemanticExpression> index_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_index(self, object, index_expression, _semantic_type); }
inline MutationCheckResult visit_if(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_if(self, condition, then_expression, else_expression, _semantic_type); }
inline MutationCheckResult ExprVisitor_visit_if(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_if(self, condition, then_expression, else_expression, _semantic_type); }
inline MutationCheckResult visit_block(MutationsPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_block(self, statements, result_expression, _semantic_type); }
inline MutationCheckResult ExprVisitor_visit_block(MutationsPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_block(self, statements, result_expression, _semantic_type); }
inline MutationCheckResult visit_while(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> condition, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_while(self, condition, statements, _semantic_type); }
inline MutationCheckResult ExprVisitor_visit_while(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> condition, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_while(self, condition, statements, _semantic_type); }
inline MutationCheckResult visit_for(MutationsPass self, mlc::String _variable, std::shared_ptr<semantic_ir::SemanticExpression> iterator, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_for(self, _variable, iterator, statements, _semantic_type); }
inline MutationCheckResult ExprVisitor_visit_for(MutationsPass self, mlc::String _variable, std::shared_ptr<semantic_ir::SemanticExpression> iterator, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_for(self, _variable, iterator, statements, _semantic_type); }
inline MutationCheckResult visit_match(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_match(self, subject, arms, _semantic_type); }
inline MutationCheckResult ExprVisitor_visit_match(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_match(self, subject, arms, _semantic_type); }
inline MutationCheckResult visit_record(MutationsPass self, mlc::String _type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_record(self, _type_name, field_values, _semantic_type); }
inline MutationCheckResult ExprVisitor_visit_record(MutationsPass self, mlc::String _type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_record(self, _type_name, field_values, _semantic_type); }
inline MutationCheckResult visit_record_update(MutationsPass self, mlc::String _type_name, std::shared_ptr<semantic_ir::SemanticExpression> base_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_record_update(self, _type_name, base_expression, overrides, _semantic_type); }
inline MutationCheckResult ExprVisitor_visit_record_update(MutationsPass self, mlc::String _type_name, std::shared_ptr<semantic_ir::SemanticExpression> base_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_record_update(self, _type_name, base_expression, overrides, _semantic_type); }
inline MutationCheckResult visit_array(MutationsPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_array(self, elements, _semantic_type); }
inline MutationCheckResult ExprVisitor_visit_array(MutationsPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_array(self, elements, _semantic_type); }
inline MutationCheckResult visit_tuple(MutationsPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_tuple(self, elements, _semantic_type); }
inline MutationCheckResult ExprVisitor_visit_tuple(MutationsPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_tuple(self, elements, _semantic_type); }
inline MutationCheckResult visit_question(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_question(self, inner_expression, _semantic_type); }
inline MutationCheckResult ExprVisitor_visit_question(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_question(self, inner_expression, _semantic_type); }
inline MutationCheckResult visit_lambda(MutationsPass self, mlc::Array<mlc::String> _parameter_names, std::shared_ptr<semantic_ir::SemanticExpression> body_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_lambda(self, _parameter_names, body_expression, _semantic_type); }
inline MutationCheckResult ExprVisitor_visit_lambda(MutationsPass self, mlc::Array<mlc::String> _parameter_names, std::shared_ptr<semantic_ir::SemanticExpression> body_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_lambda(self, _parameter_names, body_expression, _semantic_type); }
inline MutationCheckResult visit_with(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> resource, mlc::String _binder, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_with(self, resource, _binder, statements, _semantic_type); }
inline MutationCheckResult ExprVisitor_visit_with(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> resource, mlc::String _binder, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept { return MutationsPass_visit_with(self, resource, _binder, statements, _semantic_type); }
inline MutationCheckResult visit_unsupported(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> _expression) noexcept { return MutationsPass_visit_unsupported(self, _expression); }
inline MutationCheckResult ExprVisitor_visit_unsupported(MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> _expression) noexcept { return MutationsPass_visit_unsupported(self, _expression); }
static_assert(expr_visitor::ExprVisitor<MutationCheckResult, MutationsPass>, "MutationsPass does not implement ExprVisitor");

} // namespace check_mutations

#endif // CHECK_MUTATIONS_HPP
