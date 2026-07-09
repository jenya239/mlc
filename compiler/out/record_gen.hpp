#ifndef RECORD_GEN_HPP
#define RECORD_GEN_HPP

#include "mlc.hpp"
#include <variant>
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "cpp_ast.hpp"
#include "print.hpp"
#include "context.hpp"
#include "type_gen.hpp"
#include "expression_support.hpp"
#include "cpp_naming.hpp"
#include "expr.hpp"
namespace record_gen {

mlc::String eval_field_update_value(std::shared_ptr<semantic_ir::SemanticExpression> base_expression, mlc::String field_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;
mlc::String gen_record_ordered(mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, mlc::Array<mlc::String> field_order, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;
mlc::String gen_record_unordered(mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;
mlc::String gen_record_update_ordered(std::shared_ptr<semantic_ir::SemanticExpression> base_expr, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, mlc::Array<mlc::String> field_order, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;
mlc::String gen_record_update_lazy(std::shared_ptr<semantic_ir::SemanticExpression> base_expr, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;
mlc::String gen_record_expr(mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, std::shared_ptr<registry::Type> expr_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;
mlc::String gen_record_update_expr(mlc::String type_name, std::shared_ptr<semantic_ir::SemanticExpression> base_expr, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;
std::shared_ptr<cpp_ast::CppExpression> eval_field_update_value_cpp(std::shared_ptr<semantic_ir::SemanticExpression> base_expression, mlc::String field_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;
std::shared_ptr<cpp_ast::CppExpression> gen_record_update_expr_cpp(mlc::String type_name, std::shared_ptr<semantic_ir::SemanticExpression> base_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

} // namespace record_gen

#endif // RECORD_GEN_HPP
