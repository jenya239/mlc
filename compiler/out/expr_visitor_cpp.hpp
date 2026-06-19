#ifndef EXPR_VISITOR_CPP_HPP
#define EXPR_VISITOR_CPP_HPP

#include "mlc.hpp"
#include <variant>

#include "registry.hpp"
#include "semantic_ir.hpp"
#include "cpp_ast.hpp"
#include "cpp_ast.hpp"
#include "emit_helpers.hpp"
#include "semantic_type_structure.hpp"
#include "literals.hpp"
#include "identifiers.hpp"
#include "expression_support.hpp"
#include "match_analysis.hpp"
#include "match_gen.hpp"
#include "mut_actual_argument.hpp"
#include "method_gen.hpp"
#include "record_gen.hpp"
#include "cpp_naming.hpp"
#include "type_gen.hpp"
#include "context.hpp"
#include "statement_context.hpp"
#include "expr_visitor.hpp"

namespace expr_visitor_cpp {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

std::shared_ptr<cpp_ast::CppExpression> eval_expr_cpp(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_call_via_cpp_visitor(std::shared_ptr<semantic_ir::SemanticExpression> function_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> call_parameter_mutability_flags, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_method_via_cpp_visitor(std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> receiver_and_parameter_mutability_flags, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_if_via_cpp_visitor(std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> if_semantic_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_block_via_cpp_visitor(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_match_via_cpp_visitor(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms, std::shared_ptr<registry::Type> match_semantic_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_record_via_cpp_visitor(mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, std::shared_ptr<registry::Type> expression_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_array_via_cpp_visitor(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> element_container_semantic_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_lambda_via_cpp_visitor(mlc::Array<mlc::String> parameter_binding_names, std::shared_ptr<semantic_ir::SemanticExpression> body_expression_under_lambda, std::shared_ptr<registry::Type> semantic_function_type_for_lambda_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_field_via_cpp_visitor(std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String field_name, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_index_via_cpp_visitor(std::shared_ptr<semantic_ir::SemanticExpression> object, std::shared_ptr<semantic_ir::SemanticExpression> index_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_while_via_cpp_visitor(std::shared_ptr<semantic_ir::SemanticExpression> condition, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_for_via_cpp_visitor(mlc::String variable, std::shared_ptr<semantic_ir::SemanticExpression> iterator, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_record_update_via_cpp_visitor(mlc::String type_name, std::shared_ptr<semantic_ir::SemanticExpression> base_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_tuple_via_cpp_visitor(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_question_via_cpp_visitor(std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_with_via_cpp_visitor(std::shared_ptr<semantic_ir::SemanticExpression> resource, mlc::String binder, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_binary_via_cpp_visitor(mlc::String operation, std::shared_ptr<semantic_ir::SemanticExpression> left_expression, std::shared_ptr<semantic_ir::SemanticExpression> right_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> evaluate_expression) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_unary_via_cpp_visitor(mlc::String operation, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> evaluate_expression) noexcept;

std::shared_ptr<cpp_ast::CppExpression> eval_expr_with_cpp_visitor(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> evaluate_expression) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_expr_via_cpp_visitor(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context) noexcept;

} // namespace expr_visitor_cpp

#endif // EXPR_VISITOR_CPP_HPP
