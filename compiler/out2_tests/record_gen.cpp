#define main mlc_user_main
#include "record_gen.hpp"

namespace record_gen {

void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
mlc::String gen_record_ordered(mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> field_values, mlc::Array<mlc::String> field_order, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, EvalExprFn eval_expr_fn) noexcept{
auto parts = mlc::Array<mlc::String>{};
auto index = 0;
while ((index < field_order.length())) {
auto value = expression_support::find_field_value(field_values, field_order[index]);
parts.push_back(eval_expr_fn(value, context, gen_stmts));
index = (index + 1);
}
return parts.join(mlc::String(", ", 2));
}
mlc::String gen_record_unordered(mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> field_values, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, EvalExprFn eval_expr_fn) noexcept{
auto parts = mlc::Array<mlc::String>{};
auto index = 0;
while ((index < field_values.length())) {
parts.push_back(eval_expr_fn(field_values[index]->val, context, gen_stmts));
index = (index + 1);
}
return parts.join(mlc::String(", ", 2));
}
mlc::String gen_record_update_ordered(std::shared_ptr<semantic_ir::SExpr> base_expr, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> overrides, mlc::Array<mlc::String> field_order, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, EvalExprFn eval_expr_fn) noexcept{
auto parts = mlc::Array<mlc::String>{};
auto index = 0;
while ((index < field_order.length())) {
auto field_name = field_order[index];
auto value_string = mlc::String("", 0);
auto override_found = false;
auto override_index = 0;
while ((override_index < overrides.length())) {
if ((overrides[override_index]->name == field_name)) {
value_string = eval_expr_fn(overrides[override_index]->val, context, gen_stmts);
override_found = true;
}
override_index = (override_index + 1);
}
if ((!override_found)) {
value_string = eval_expr_fn(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprField{base_expr, field_name, std::make_shared<registry::Type>(registry::TUnknown{}), ast::span_unknown()}), context, gen_stmts);
}
parts.push_back(value_string);
index = (index + 1);
}
return parts.join(mlc::String(", ", 2));
}
mlc::String gen_record_update_lazy(std::shared_ptr<semantic_ir::SExpr> base_expr, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> overrides, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, EvalExprFn eval_expr_fn) noexcept{
auto parts = mlc::Array{expr::record_update_base_prefix(eval_expr_fn(base_expr, context, gen_stmts))};
auto index = 0;
while ((index < overrides.length())) {
parts.push_back(expr::mutable_member_assignment(mlc::String("__upd", 5), cpp_naming::cpp_safe(overrides[index]->name), eval_expr_fn(overrides[index]->val, context, gen_stmts)));
index = (index + 1);
}
return expr::record_update_lazy_closure(parts.join(mlc::String("", 0)));
}
mlc::String gen_record_expr(mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> field_values, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, EvalExprFn eval_expr_fn) noexcept{
auto field_order = decl_index::lookup_fields(context.field_orders, type_name);
auto values_code = ((field_order.length() > 0) ? gen_record_ordered(field_values, field_order, context, gen_stmts, eval_expr_fn) : gen_record_unordered(field_values, context, gen_stmts, eval_expr_fn));
return expr::record_initializer(context::context_resolve(context, type_name), values_code);
}
mlc::String gen_record_update_expr(mlc::String type_name, std::shared_ptr<semantic_ir::SExpr> base_expr, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> overrides, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, EvalExprFn eval_expr_fn) noexcept{
auto field_order = decl_index::lookup_fields(context.field_orders, type_name);
if ((field_order.length() > 0)) {
return expr::record_initializer(context::context_resolve(context, type_name), gen_record_update_ordered(base_expr, overrides, field_order, context, gen_stmts, eval_expr_fn));
} else {
return gen_record_update_lazy(base_expr, overrides, context, gen_stmts, eval_expr_fn);
}
}

} // namespace record_gen
