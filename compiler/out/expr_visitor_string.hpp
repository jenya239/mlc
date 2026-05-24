#ifndef EXPR_VISITOR_STRING_HPP
#define EXPR_VISITOR_STRING_HPP

#include "mlc.hpp"
#include <variant>

#include "registry.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "semantic_type_structure.hpp"
#include "expr_visitor.hpp"
#include "context.hpp"
#include "literals.hpp"
#include "identifiers.hpp"
#include "cpp_naming.hpp"
#include "expression_support.hpp"
#include "mut_actual_argument.hpp"
#include "method_gen.hpp"
#include "match_gen.hpp"
#include "record_gen.hpp"
#include "type_gen.hpp"
#include "statement_context.hpp"
#include "expr.hpp"

namespace expr_visitor_string {

struct RecordLitPart;
struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

mlc::String gen_binary_via_visitor(mlc::String operation, std::shared_ptr<semantic_ir::SExpr> left_expression, std::shared_ptr<semantic_ir::SExpr> right_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_unary_via_visitor(mlc::String operation, std::shared_ptr<semantic_ir::SExpr> inner_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_call_via_visitor(std::shared_ptr<semantic_ir::SExpr> function_expression, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, mlc::Array<int> call_parameter_mutability_flags, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_method_via_visitor(std::shared_ptr<semantic_ir::SExpr> object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, mlc::Array<int> receiver_and_parameter_mutability_flags, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_if_via_visitor(std::shared_ptr<semantic_ir::SExpr> condition, std::shared_ptr<semantic_ir::SExpr> then_expression, std::shared_ptr<semantic_ir::SExpr> else_expression, std::shared_ptr<registry::Type> if_semantic_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_block_via_visitor(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, std::shared_ptr<semantic_ir::SExpr> result_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_match_via_visitor(std::shared_ptr<semantic_ir::SExpr> subject, mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> arms, std::shared_ptr<registry::Type> match_semantic_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_extern_via_visitor(std::shared_ptr<registry::Type> _semantic_type) noexcept;

mlc::String gen_field_via_visitor(std::shared_ptr<semantic_ir::SExpr> object, mlc::String field_name, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_index_via_visitor(std::shared_ptr<semantic_ir::SExpr> object, std::shared_ptr<semantic_ir::SExpr> index_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_while_via_visitor(std::shared_ptr<semantic_ir::SExpr> condition, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_for_via_visitor(mlc::String variable, std::shared_ptr<semantic_ir::SExpr> iterator, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_record_via_visitor(mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> field_values, std::shared_ptr<registry::Type> expression_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_record_update_via_visitor(mlc::String type_name, std::shared_ptr<semantic_ir::SExpr> base_expression, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> overrides, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_array_via_visitor(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> elements, std::shared_ptr<registry::Type> element_container_semantic_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_tuple_via_visitor(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> elements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_question_via_visitor(std::shared_ptr<semantic_ir::SExpr> inner_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_lambda_via_visitor(mlc::Array<mlc::String> parameter_binding_names, std::shared_ptr<semantic_ir::SExpr> body_expression_under_lambda, std::shared_ptr<registry::Type> semantic_function_type_for_lambda_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_with_via_visitor(std::shared_ptr<semantic_ir::SExpr> resource, mlc::String binder, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> evaluate_expression) noexcept;

mlc::String gen_expr_via_string_visitor(std::shared_ptr<semantic_ir::SExpr> expression, context::CodegenContext context) noexcept;

} // namespace expr_visitor_string

#endif // EXPR_VISITOR_STRING_HPP
