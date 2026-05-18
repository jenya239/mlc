#define main mlc_user_main
#include "record_gen.hpp"

namespace record_gen {

mlc::String eval_field_update_value(std::shared_ptr<semantic_ir::SExpr> base_expression, mlc::String field_name, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> overrides, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
auto override_index = 0;
while (((override_index < overrides.length()) && (overrides[override_index]->name != field_name))) {
override_index = (override_index + 1);
}
if ((override_index < overrides.length())) {
return eval_expr_fn(overrides[override_index]->val, context, gen_stmts);
} else {
return eval_expr_fn(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprField{base_expression, field_name, std::make_shared<registry::Type>(registry::TUnknown{}), ast::span_unknown()}), context, gen_stmts);
}
}
mlc::String gen_record_ordered(mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> field_values, mlc::Array<mlc::String> field_order, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
return field_order.map([=](mlc::String field_name) mutable { return eval_expr_fn(expression_support::find_field_value(field_values, field_name), context, gen_stmts); }).join(mlc::String(", ", 2));
}
mlc::String gen_record_unordered(mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> field_values, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
return field_values.map([=](std::shared_ptr<semantic_ir::SFieldVal> field_value) mutable { return eval_expr_fn(field_value->val, context, gen_stmts); }).join(mlc::String(", ", 2));
}
mlc::String gen_record_update_ordered(std::shared_ptr<semantic_ir::SExpr> base_expr, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> overrides, mlc::Array<mlc::String> field_order, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
return field_order.map([=](mlc::String field_name) mutable { return eval_field_update_value(base_expr, field_name, overrides, context, gen_stmts, eval_expr_fn); }).join(mlc::String(", ", 2));
}
mlc::String gen_record_update_lazy(std::shared_ptr<semantic_ir::SExpr> base_expr, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> overrides, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
return expr::record_update_lazy_closure((expr::record_update_base_prefix(eval_expr_fn(base_expr, context, gen_stmts)) + overrides.map([=](std::shared_ptr<semantic_ir::SFieldVal> override_field) mutable { return expr::mutable_member_assignment(mlc::String("__upd", 5), cpp_naming::cpp_safe(override_field->name), eval_expr_fn(override_field->val, context, gen_stmts)); }).join(mlc::String("", 0))));
}
mlc::String gen_record_expr(mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> field_values, std::shared_ptr<registry::Type> expr_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
auto cpp_name = std::visit(overloaded{[&](const registry::TGeneric& tGeneric) { auto [__0, __1] = tGeneric; return type_gen::sem_type_to_cpp(context, expr_type); },
[&](const auto& __v) { return CodegenContext_resolve(context, type_name); }
}, (*expr_type));
auto field_order = decl_index::lookup_fields(context.field_orders, type_name);
auto values_code = ((field_order.length() > 0) ? (gen_record_ordered(field_values, field_order, context, gen_stmts, eval_expr_fn)) : (gen_record_unordered(field_values, context, gen_stmts, eval_expr_fn)));
return expr::record_initializer(cpp_name, values_code);
}
mlc::String gen_record_update_expr(mlc::String type_name, std::shared_ptr<semantic_ir::SExpr> base_expr, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> overrides, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
auto field_order = decl_index::lookup_fields(context.field_orders, type_name);
if ((field_order.length() > 0)) {
return expr::record_initializer(CodegenContext_resolve(context, type_name), gen_record_update_ordered(base_expr, overrides, field_order, context, gen_stmts, eval_expr_fn));
} else {
return gen_record_update_lazy(base_expr, overrides, context, gen_stmts, eval_expr_fn);
}
}

} // namespace record_gen
