#ifndef EXPR_VISITOR_CPP_HPP
#define EXPR_VISITOR_CPP_HPP

#include "mlc.hpp"
#include <variant>

#include "registry.hpp"
#include "semantic_ir.hpp"
#include "cpp_ast.hpp"
#include "cpp_printer.hpp"
#include "emit_helpers.hpp"
#include "semantic_type_structure.hpp"
#include "literals.hpp"
#include "identifiers.hpp"
#include "expr_fragment_codegen.hpp"
#include "expression_support.hpp"
#include "cpp_naming.hpp"
#include "expr_visitor.hpp"
#include "context.hpp"

namespace expr_visitor_cpp {

struct RecordLitPart;
struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;
struct CppStmt;
struct CppExpr;

mlc::String eval_expr_cpp_as_string(std::shared_ptr<semantic_ir::SExpr> expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpr> eval_expr_cpp(std::shared_ptr<semantic_ir::SExpr> expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpr> gen_call_via_cpp_visitor(std::shared_ptr<semantic_ir::SExpr> function_expression, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, mlc::Array<int> call_parameter_mutability_flags, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpr> gen_method_via_cpp_visitor(std::shared_ptr<semantic_ir::SExpr> object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, mlc::Array<int> receiver_and_parameter_mutability_flags, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpr> gen_if_via_cpp_visitor(std::shared_ptr<semantic_ir::SExpr> condition, std::shared_ptr<semantic_ir::SExpr> then_expression, std::shared_ptr<semantic_ir::SExpr> else_expression, std::shared_ptr<registry::Type> if_semantic_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpr> gen_block_via_cpp_visitor(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, std::shared_ptr<semantic_ir::SExpr> result_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpr> gen_match_via_cpp_visitor(std::shared_ptr<semantic_ir::SExpr> subject, mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> arms, std::shared_ptr<registry::Type> match_semantic_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpr> gen_record_via_cpp_visitor(mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> field_values, std::shared_ptr<registry::Type> expression_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpr> gen_array_via_cpp_visitor(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> elements, std::shared_ptr<registry::Type> element_container_semantic_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpr> gen_lambda_via_cpp_visitor(mlc::Array<mlc::String> parameter_binding_names, std::shared_ptr<semantic_ir::SExpr> body_expression_under_lambda, std::shared_ptr<registry::Type> semantic_function_type_for_lambda_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpr> gen_field_via_cpp_visitor(std::shared_ptr<semantic_ir::SExpr> object, mlc::String field_name, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpr> gen_index_via_cpp_visitor(std::shared_ptr<semantic_ir::SExpr> object, std::shared_ptr<semantic_ir::SExpr> index_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpr> gen_while_via_cpp_visitor(std::shared_ptr<semantic_ir::SExpr> condition, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpr> gen_for_via_cpp_visitor(mlc::String variable, std::shared_ptr<semantic_ir::SExpr> iterator, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpr> gen_record_update_via_cpp_visitor(mlc::String type_name, std::shared_ptr<semantic_ir::SExpr> base_expression, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> overrides, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpr> gen_tuple_via_cpp_visitor(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> elements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpr> gen_question_via_cpp_visitor(std::shared_ptr<semantic_ir::SExpr> inner_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpr> gen_with_via_cpp_visitor(std::shared_ptr<semantic_ir::SExpr> resource, mlc::String binder, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

std::shared_ptr<cpp_ast::CppExpr> gen_binary_via_cpp_visitor(mlc::String operation, std::shared_ptr<semantic_ir::SExpr> left_expression, std::shared_ptr<semantic_ir::SExpr> right_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpr>(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

std::shared_ptr<cpp_ast::CppExpr> gen_unary_via_cpp_visitor(mlc::String operation, std::shared_ptr<semantic_ir::SExpr> inner_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpr>(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

std::shared_ptr<cpp_ast::CppExpr> eval_expr_with_cpp_visitor(std::shared_ptr<semantic_ir::SExpr> expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpr>(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

std::shared_ptr<cpp_ast::CppExpr> gen_expr_via_cpp_visitor(std::shared_ptr<semantic_ir::SExpr> expression, context::CodegenContext context) noexcept;

} // namespace expr_visitor_cpp

#endif // EXPR_VISITOR_CPP_HPP
