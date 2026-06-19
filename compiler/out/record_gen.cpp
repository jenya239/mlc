#include "record_gen.hpp"

#include "semantic_ir.hpp"
#include "registry.hpp"
#include "cpp_ast.hpp"
#include "cpp_ast.hpp"
#include "context.hpp"
#include "type_gen.hpp"
#include "expression_support.hpp"
#include "cpp_naming.hpp"
#include "expr.hpp"

namespace record_gen {

using namespace semantic_ir;
using namespace registry;
using namespace cpp_ast;
using namespace cpp_ast;
using namespace context;
using namespace type_gen;
using namespace expression_support;
using namespace cpp_naming;
using namespace expr;

mlc::String eval_field_update_value(std::shared_ptr<semantic_ir::SemanticExpression> base_expression, mlc::String field_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

mlc::String gen_record_ordered(mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, mlc::Array<mlc::String> field_order, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

mlc::String gen_record_unordered(mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

mlc::String gen_record_update_ordered(std::shared_ptr<semantic_ir::SemanticExpression> base_expr, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, mlc::Array<mlc::String> field_order, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

mlc::String gen_record_update_lazy(std::shared_ptr<semantic_ir::SemanticExpression> base_expr, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

mlc::String gen_record_expr(mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, std::shared_ptr<registry::Type> expr_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

mlc::String gen_record_update_expr(mlc::String type_name, std::shared_ptr<semantic_ir::SemanticExpression> base_expr, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

std::shared_ptr<cpp_ast::CppExpression> eval_field_update_value_cpp(std::shared_ptr<semantic_ir::SemanticExpression> base_expression, mlc::String field_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

std::shared_ptr<cpp_ast::CppExpression> gen_record_update_expr_cpp(mlc::String type_name, std::shared_ptr<semantic_ir::SemanticExpression> base_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

mlc::String eval_field_update_value(std::shared_ptr<semantic_ir::SemanticExpression> base_expression, mlc::String field_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
int override_index = 0;
while (override_index < overrides.size() && overrides[override_index]->name != field_name){
{
override_index = override_index + 1;
}
}
return override_index < overrides.size() ? eval_expr_fn(overrides[override_index]->value, context, gen_stmts) : eval_expr_fn(std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionField(base_expression, field_name, std::make_shared<registry::Type>((registry::TUnknown{})), semantic_ir::sexpr_span(base_expression))), context, gen_stmts);
}

mlc::String gen_record_ordered(mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, mlc::Array<mlc::String> field_order, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{return field_order.map([eval_expr_fn, field_values, context, gen_stmts](mlc::String field_name) mutable { return eval_expr_fn(expression_support::find_field_value(field_values, field_name), context, gen_stmts); }).join(mlc::String(", "));}

mlc::String gen_record_unordered(mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{return field_values.map([eval_expr_fn, context, gen_stmts](std::shared_ptr<semantic_ir::SemanticFieldVal> field_value) mutable { return eval_expr_fn(field_value->value, context, gen_stmts); }).join(mlc::String(", "));}

mlc::String gen_record_update_ordered(std::shared_ptr<semantic_ir::SemanticExpression> base_expr, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, mlc::Array<mlc::String> field_order, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{return field_order.map([base_expr, overrides, context, gen_stmts, eval_expr_fn](mlc::String field_name) mutable { return eval_field_update_value(base_expr, field_name, overrides, context, gen_stmts, eval_expr_fn); }).join(mlc::String(", "));}

mlc::String gen_record_update_lazy(std::shared_ptr<semantic_ir::SemanticExpression> base_expr, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{return expr::record_update_lazy_closure(expr::record_update_base_prefix(eval_expr_fn(base_expr, context, gen_stmts)) + overrides.map([eval_expr_fn, context, gen_stmts](std::shared_ptr<semantic_ir::SemanticFieldVal> override_field) mutable { return expr::mutable_member_assignment(mlc::String("__upd"), cpp_naming::cpp_safe(override_field->name), eval_expr_fn(override_field->value, context, gen_stmts)); }).join(mlc::String("")));}

mlc::String gen_record_expr(mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, std::shared_ptr<registry::Type> expr_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
mlc::String cpp_name = [&]() -> mlc::String { if (std::holds_alternative<registry::TGeneric>((*expr_type))) { auto _v_tgeneric = std::get<registry::TGeneric>((*expr_type)); auto [_w0, _w1] = _v_tgeneric; return type_gen::sem_type_to_cpp(context, expr_type); } return context::CodegenContext_resolve(context, type_name); }();
mlc::Array<mlc::String> field_order = context::lookup_fields_for_context(context, type_name);
mlc::String values_code = field_order.size() > 0 ? gen_record_ordered(field_values, field_order, context, gen_stmts, eval_expr_fn) : gen_record_unordered(field_values, context, gen_stmts, eval_expr_fn);
return expr::record_initializer(cpp_name, values_code);
}

mlc::String gen_record_update_expr(mlc::String type_name, std::shared_ptr<semantic_ir::SemanticExpression> base_expr, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
mlc::Array<mlc::String> field_order = context::lookup_fields_for_context(context, type_name);
return field_order.size() > 0 ? expr::record_initializer(context::CodegenContext_resolve(context, type_name), gen_record_update_ordered(base_expr, overrides, field_order, context, gen_stmts, eval_expr_fn)) : gen_record_update_lazy(base_expr, overrides, context, gen_stmts, eval_expr_fn);
}

std::shared_ptr<cpp_ast::CppExpression> eval_field_update_value_cpp(std::shared_ptr<semantic_ir::SemanticExpression> base_expression, mlc::String field_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
int override_index = 0;
while (override_index < overrides.size() && overrides[override_index]->name != field_name){
{
override_index = override_index + 1;
}
}
return override_index < overrides.size() ? eval_expr_fn(overrides[override_index]->value, context, gen_stmts) : eval_expr_fn(std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionField(base_expression, field_name, std::make_shared<registry::Type>((registry::TUnknown{})), semantic_ir::sexpr_span(base_expression))), context, gen_stmts);
}

std::shared_ptr<cpp_ast::CppExpression> gen_record_update_expr_cpp(mlc::String type_name, std::shared_ptr<semantic_ir::SemanticExpression> base_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
mlc::Array<mlc::String> field_order = context::lookup_fields_for_context(context, type_name);
return field_order.size() > 0 ? std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppAggregateInit(context::CodegenContext_resolve(context, type_name), field_order.map([base_expression, overrides, context, gen_stmts, eval_expr_fn](mlc::String field_name) mutable { return eval_field_update_value_cpp(base_expression, field_name, overrides, context, gen_stmts, eval_expr_fn); }))) : [&]() -> std::shared_ptr<cpp_ast::CppExpression> { 
  mlc::String body_fragment = mlc::String("auto __upd = ") + cpp_ast::print_expr(eval_expr_fn(base_expression, context, gen_stmts)) + mlc::String(";\n") + overrides.map([eval_expr_fn, context, gen_stmts](std::shared_ptr<semantic_ir::SemanticFieldVal> override_field) mutable { return mlc::String("__upd.") + cpp_naming::cpp_safe(override_field->name) + mlc::String(" = ") + cpp_ast::print_expr(eval_expr_fn(override_field->value, context, gen_stmts)) + mlc::String(";\n"); }).join(mlc::String("")) + mlc::String("return __upd;\n");
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppInvokedBlock(body_fragment));
 }();
}

} // namespace record_gen
