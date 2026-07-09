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

template<typename __F5>
mlc::String eval_field_update_value(std::shared_ptr<semantic_ir::SemanticExpression> base_expression, mlc::String field_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, __F5 eval_expr_fn) noexcept;
template<typename __F4>
mlc::String gen_record_ordered(mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, mlc::Array<mlc::String> field_order, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, __F4 eval_expr_fn) noexcept;
template<typename __F3>
mlc::String gen_record_unordered(mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, __F3 eval_expr_fn) noexcept;
mlc::String gen_record_update_ordered(std::shared_ptr<semantic_ir::SemanticExpression> base_expr, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, mlc::Array<mlc::String> field_order, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;
template<typename __F4>
mlc::String gen_record_update_lazy(std::shared_ptr<semantic_ir::SemanticExpression> base_expr, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, __F4 eval_expr_fn) noexcept;
mlc::String gen_record_expr(mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, std::shared_ptr<registry::Type> expr_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;
mlc::String gen_record_update_expr(mlc::String type_name, std::shared_ptr<semantic_ir::SemanticExpression> base_expr, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;
template<typename __F5>
std::shared_ptr<cpp_ast::CppExpression> eval_field_update_value_cpp(std::shared_ptr<semantic_ir::SemanticExpression> base_expression, mlc::String field_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, __F5 eval_expr_fn) noexcept;
std::shared_ptr<cpp_ast::CppExpression> gen_record_update_expr_cpp(mlc::String type_name, std::shared_ptr<semantic_ir::SemanticExpression> base_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;
template<typename __F5>
mlc::String eval_field_update_value(std::shared_ptr<semantic_ir::SemanticExpression> base_expression, mlc::String field_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, __F5 eval_expr_fn) noexcept{
  auto override_index = 0;
  while (((override_index < overrides.length()) && (overrides[override_index]->name != field_name)))   {
    (override_index = (override_index + 1));
  }
  if ((override_index < overrides.length()))   {
    return eval_expr_fn(overrides[override_index]->value, context, gen_stmts);
  } else   {
    return eval_expr_fn(std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionField{base_expression, field_name, std::make_shared<registry::Type>(registry::TUnknown{}), semantic_ir::sexpr_span(base_expression)}), context, gen_stmts);
  }
}
template<typename __F4>
mlc::String gen_record_ordered(mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, mlc::Array<mlc::String> field_order, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, __F4 eval_expr_fn) noexcept{
  return field_order.map([=](mlc::String field_name) mutable { return eval_expr_fn(expression_support::find_field_value(field_values, field_name), context, gen_stmts); }).join(mlc::String(", ", 2));
}
template<typename __F3>
mlc::String gen_record_unordered(mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, __F3 eval_expr_fn) noexcept{
  return field_values.map([=](std::shared_ptr<semantic_ir::SemanticFieldVal> field_value) mutable { return eval_expr_fn(field_value->value, context, gen_stmts); }).join(mlc::String(", ", 2));
}
template<typename __F4>
mlc::String gen_record_update_lazy(std::shared_ptr<semantic_ir::SemanticExpression> base_expr, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, __F4 eval_expr_fn) noexcept{
  return expr::record_update_lazy_closure((expr::record_update_base_prefix(eval_expr_fn(base_expr, context, gen_stmts)) + overrides.map([=](std::shared_ptr<semantic_ir::SemanticFieldVal> override_field) mutable { return expr::mutable_member_assignment(mlc::String("__upd", 5), cpp_naming::cpp_safe(override_field->name), eval_expr_fn(override_field->value, context, gen_stmts)); }).join(mlc::String("", 0))));
}
template<typename __F5>
std::shared_ptr<cpp_ast::CppExpression> eval_field_update_value_cpp(std::shared_ptr<semantic_ir::SemanticExpression> base_expression, mlc::String field_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, __F5 eval_expr_fn) noexcept{
  auto override_index = 0;
  while (((override_index < overrides.length()) && (overrides[override_index]->name != field_name)))   {
    (override_index = (override_index + 1));
  }
  if ((override_index < overrides.length()))   {
    return eval_expr_fn(overrides[override_index]->value, context, gen_stmts);
  } else   {
    return eval_expr_fn(std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionField{base_expression, field_name, std::make_shared<registry::Type>(registry::TUnknown{}), semantic_ir::sexpr_span(base_expression)}), context, gen_stmts);
  }
}

} // namespace record_gen

#endif // RECORD_GEN_HPP
