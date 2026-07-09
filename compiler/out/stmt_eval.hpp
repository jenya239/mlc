#ifndef STMT_EVAL_HPP
#define STMT_EVAL_HPP

#include "mlc.hpp"
#include <variant>
#include "semantic_ir.hpp"
#include "let_pat.hpp"
#include "registry.hpp"
#include "context.hpp"
#include "cpp_naming.hpp"
#include "expression_support.hpp"
#include "type_gen.hpp"
#include "literals.hpp"
#include "semantic_type_structure.hpp"
#include "expr.hpp"
namespace stmt_eval {

mlc::String constexpr_named_type_cpp(std::shared_ptr<registry::Type> binding_semantic_type, context::CodegenContext context) noexcept;
mlc::String constexpr_if_binding_value_code(std::shared_ptr<semantic_ir::SemanticExpression> if_value, std::shared_ptr<registry::Type> binding_semantic_type, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;
template<typename __F2>
mlc::String constexpr_binding_default_code(std::shared_ptr<semantic_ir::SemanticExpression> value, context::CodegenContext context, __F2 gen_expr_fn) noexcept;
bool semantic_expression_is_if(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept;
bool semantic_expression_is_block(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept;
bool semantic_expression_is_unit(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept;
bool semantic_expression_is_array(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept;
mlc::Array<std::shared_ptr<registry::Type>> map_generic_type_arguments(std::shared_ptr<registry::Type> value_type) noexcept;
mlc::String constexpr_block_binding_value_code(std::shared_ptr<semantic_ir::SemanticExpression> value, std::shared_ptr<registry::Type> binding_semantic_type, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;
mlc::String constexpr_binding_value_code(std::shared_ptr<semantic_ir::SemanticExpression> value, std::shared_ptr<registry::Type> binding_semantic_type, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;
template<typename __F4>
mlc::String eval_try_unwrap(std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, context::CodegenContext context, mlc::String try_identifier, mlc::String success_line, __F4 gen_expr_fn) noexcept;
bool empty_unit_block_expression(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept;
mlc::String block_result_trailing_code(mlc::String statements_code, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, context::CodegenContext final_context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;
template<typename __F3>
mlc::String block_result_expression_trailing_code(mlc::String statements_code, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, context::CodegenContext final_context, __F3 gen_expr_fn) noexcept;
mlc::String eval_stmt_if_nested(std::shared_ptr<semantic_ir::SemanticExpression> if_expression, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;
mlc::String eval_block_body(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;
mlc::String eval_semantic_block_body(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;
template<typename __F2>
mlc::String eval_stmt_expr_default_suffix(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, __F2 gen_expr_fn) noexcept;
mlc::String eval_stmt_expr_block(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;
mlc::String eval_stmt_expr(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;
template<typename __F2>
mlc::String eval_elements_code(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, context::CodegenContext context, __F2 gen_expr_fn) noexcept;
mlc::String gen_let_array_value_from_tarray(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> inner_type, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;
mlc::String gen_let_array_value_fallback(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;
mlc::String gen_let_array_value_code(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> array_type, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;
template<typename __F3>
mlc::String gen_let_map_new_value_code(std::shared_ptr<semantic_ir::SemanticExpression> value, std::shared_ptr<registry::Type> value_type, context::CodegenContext context, __F3 gen_expr_fn) noexcept;
mlc::String gen_let_array_value_from_expression(std::shared_ptr<semantic_ir::SemanticExpression> value, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;
mlc::String gen_let_value_code(std::shared_ptr<semantic_ir::SemanticExpression> value, std::shared_ptr<registry::Type> value_type, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;
mlc::String semantic_expression_ident_name(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept;
bool semantic_expression_is_method_map_new(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept;
context::GenStmtResult gen_let_stmt_result_default(mlc::String name, std::shared_ptr<semantic_ir::SemanticExpression> value, std::shared_ptr<registry::Type> value_type, context::CodegenContext context, int try_counter, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;
context::GenStmtResult gen_let_stmt_result(mlc::String name, std::shared_ptr<semantic_ir::SemanticExpression> value, std::shared_ptr<registry::Type> value_type, context::CodegenContext context, int try_counter, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;
context::GenStmtResult gen_expr_stmt_result_default(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, int try_counter, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;
context::GenStmtResult gen_expr_stmt_result(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, int try_counter, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;
template<typename __F3>
context::GenStmtResult gen_return_stmt_result_default(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, int try_counter, __F3 gen_expr_fn) noexcept;
context::GenStmtResult gen_return_stmt_result(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, int try_counter, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;
context::GenStmtResult eval_stmt_with_try(std::shared_ptr<semantic_ir::SemanticStatement> statement, context::CodegenContext context, int try_counter, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;
context::GenStmtsWithContext eval_stmts_str_with_try(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context, int try_counter, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;
mlc::String eval_stmts_str(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> gen_expr_fn) noexcept;
template<typename __F2>
mlc::String constexpr_binding_default_code(std::shared_ptr<semantic_ir::SemanticExpression> value, context::CodegenContext context, __F2 gen_expr_fn) noexcept{
  return gen_expr_fn(value, context);
}
template<typename __F4>
mlc::String eval_try_unwrap(std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, context::CodegenContext context, mlc::String try_identifier, mlc::String success_line, __F4 gen_expr_fn) noexcept{
  auto inner_code = gen_expr_fn(inner_expression, context);
  return expr::try_unwrap_result_block(try_identifier, inner_code, success_line, expression_support::question_from_converter_name(context, semantic_ir::sexpr_type(inner_expression)));
}
template<typename __F3>
mlc::String block_result_expression_trailing_code(mlc::String statements_code, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, context::CodegenContext final_context, __F3 gen_expr_fn) noexcept{
  auto result_code = gen_expr_fn(result_expression, final_context);
  if ((result_code == literals::gen_unit_literal()))   {
    return statements_code;
  } else   {
    return expr::append_trailing_expression_statement(statements_code, result_code);
  }
}
template<typename __F2>
mlc::String eval_stmt_expr_default_suffix(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, __F2 gen_expr_fn) noexcept{
  return expr::suffix_semicolon_newline(gen_expr_fn(expression, context));
}
template<typename __F2>
mlc::String eval_elements_code(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, context::CodegenContext context, __F2 gen_expr_fn) noexcept{
  return elements.map([=](std::shared_ptr<semantic_ir::SemanticExpression> element) mutable { return gen_expr_fn(element, context); }).join(mlc::String(", ", 2));
}
template<typename __F3>
mlc::String gen_let_map_new_value_code(std::shared_ptr<semantic_ir::SemanticExpression> value, std::shared_ptr<registry::Type> value_type, context::CodegenContext context, __F3 gen_expr_fn) noexcept{
  auto type_arguments = map_generic_type_arguments(value_type);
  if ((semantic_type_structure::receiver_type_is_map(value_type) && (type_arguments.length() == 2)))   {
    return expr::hash_map_empty_instantiation(type_gen::sem_type_to_cpp(context, type_arguments[0]), type_gen::sem_type_to_cpp(context, type_arguments[1]));
  } else   {
    return gen_expr_fn(value, context);
  }
}
template<typename __F3>
context::GenStmtResult gen_return_stmt_result_default(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, int try_counter, __F3 gen_expr_fn) noexcept{
  return context::GenStmtResult{expr::return_line(gen_expr_fn(expression, context)), try_counter, context};
}

} // namespace stmt_eval

#endif // STMT_EVAL_HPP
