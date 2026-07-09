#define main mlc_user_main
#include "record_gen.hpp"

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

using namespace semantic_ir;
using namespace registry;
using namespace cpp_ast;
using namespace print;
using namespace context;
using namespace type_gen;
using namespace expression_support;
using namespace cpp_naming;
using namespace expr;

mlc::String gen_record_update_ordered(std::shared_ptr<semantic_ir::SemanticExpression> base_expr, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, mlc::Array<mlc::String> field_order, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
  return field_order.map([=](mlc::String field_name) mutable { return eval_field_update_value(base_expr, field_name, overrides, context, gen_stmts, eval_expr_fn); }).join(mlc::String(", ", 2));
}
mlc::String gen_record_expr(mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, std::shared_ptr<registry::Type> expr_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
  auto cpp_name = [&]() -> mlc::String {
auto __match_subject = expr_type;
if (std::holds_alternative<registry::TGeneric>((*__match_subject))) {
const registry::TGeneric& tGeneric = std::get<registry::TGeneric>((*__match_subject));
auto [__0, __1] = tGeneric; return type_gen::sem_type_to_cpp(context, expr_type);
}
return context::CodegenContext_resolve(context, type_name);
std::abort();
}();
  auto field_order = context::lookup_fields_for_context(context, type_name);
  auto values_code = ((field_order.length() > 0) ? (gen_record_ordered(field_values, field_order, context, gen_stmts, eval_expr_fn)) : (gen_record_unordered(field_values, context, gen_stmts, eval_expr_fn)));
  return expr::record_initializer(cpp_name, values_code);
}
mlc::String gen_record_update_expr(mlc::String type_name, std::shared_ptr<semantic_ir::SemanticExpression> base_expr, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
  auto field_order = context::lookup_fields_for_context(context, type_name);
  if ((field_order.length() > 0))   {
    return expr::record_initializer(context::CodegenContext_resolve(context, type_name), gen_record_update_ordered(base_expr, overrides, field_order, context, gen_stmts, eval_expr_fn));
  } else   {
    return gen_record_update_lazy(base_expr, overrides, context, gen_stmts, eval_expr_fn);
  }
}
std::shared_ptr<cpp_ast::CppExpression> gen_record_update_expr_cpp(mlc::String type_name, std::shared_ptr<semantic_ir::SemanticExpression> base_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
  auto field_order = context::lookup_fields_for_context(context, type_name);
  if ((field_order.length() > 0))   {
    return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppAggregateInit{context::CodegenContext_resolve(context, type_name), field_order.map([=](mlc::String field_name) mutable { return eval_field_update_value_cpp(base_expression, field_name, overrides, context, gen_stmts, eval_expr_fn); })});
  } else   {
    auto body_fragment = ((((mlc::String("auto __upd = ", 13) + print::print_expr(eval_expr_fn(base_expression, context, gen_stmts))) + mlc::String(";\n", 2)) + overrides.map([=](std::shared_ptr<semantic_ir::SemanticFieldVal> override_field) mutable { return ((((mlc::String("__upd.", 6) + cpp_naming::cpp_safe(override_field->name)) + mlc::String(" = ", 3)) + print::print_expr(eval_expr_fn(override_field->value, context, gen_stmts))) + mlc::String(";\n", 2)); }).join(mlc::String("", 0))) + mlc::String("return __upd;\n", 14));
    return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppInvokedBlock{body_fragment});
  }
}

} // namespace record_gen
