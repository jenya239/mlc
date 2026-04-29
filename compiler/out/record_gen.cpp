#include "record_gen.hpp"

#include "semantic_ir.hpp"
#include "registry.hpp"
#include "context.hpp"
#include "type_gen.hpp"
#include "expression_support.hpp"
#include "cpp_naming.hpp"
#include "ast.hpp"
#include "expr.hpp"

namespace record_gen {

using namespace semantic_ir;
using namespace registry;
using namespace context;
using namespace type_gen;
using namespace expression_support;
using namespace cpp_naming;
using namespace ast;
using namespace expr;
using namespace ast_tokens;

using EvalExprFn = std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)>;

mlc::String gen_record_ordered(mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> field_values, mlc::Array<mlc::String> field_order, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

mlc::String gen_record_unordered(mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> field_values, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

mlc::String gen_record_update_ordered(std::shared_ptr<semantic_ir::SExpr> base_expr, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> overrides, mlc::Array<mlc::String> field_order, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

mlc::String gen_record_update_lazy(std::shared_ptr<semantic_ir::SExpr> base_expr, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> overrides, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

mlc::String gen_record_expr(mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> field_values, std::shared_ptr<registry::Type> expr_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

mlc::String gen_record_update_expr(mlc::String type_name, std::shared_ptr<semantic_ir::SExpr> base_expr, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> overrides, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept;

mlc::String gen_record_ordered(mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> field_values, mlc::Array<mlc::String> field_order, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
mlc::Array<mlc::String> parts = {};
int index = 0;
while (index < field_order.size()){
{
std::shared_ptr<semantic_ir::SExpr> value = expression_support::find_field_value(field_values, field_order[index]);
parts.push_back(eval_expr_fn(value, context, gen_stmts));
index = index + 1;
}
}
return parts.join(mlc::String(", "));
}

mlc::String gen_record_unordered(mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> field_values, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
mlc::Array<mlc::String> parts = {};
int index = 0;
while (index < field_values.size()){
{
parts.push_back(eval_expr_fn(field_values[index]->val, context, gen_stmts));
index = index + 1;
}
}
return parts.join(mlc::String(", "));
}

mlc::String gen_record_update_ordered(std::shared_ptr<semantic_ir::SExpr> base_expr, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> overrides, mlc::Array<mlc::String> field_order, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
mlc::Array<mlc::String> parts = {};
int index = 0;
while (index < field_order.size()){
{
mlc::String field_name = field_order[index];
mlc::String value_string = mlc::String("");
bool override_found = false;
int override_index = 0;
while (override_index < overrides.size()){
{
if (overrides[override_index]->name == field_name){
{
value_string = eval_expr_fn(overrides[override_index]->val, context, gen_stmts);
override_found = true;
}
}
override_index = override_index + 1;
}
}
if (!override_found){
{
value_string = eval_expr_fn(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprField(base_expr, field_name, std::make_shared<registry::Type>((registry::TUnknown{})), ast::span_unknown())), context, gen_stmts);
}
}
parts.push_back(value_string);
index = index + 1;
}
}
return parts.join(mlc::String(", "));
}

mlc::String gen_record_update_lazy(std::shared_ptr<semantic_ir::SExpr> base_expr, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> overrides, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
mlc::Array<mlc::String> parts = mlc::Array<mlc::String>{expr::record_update_base_prefix(eval_expr_fn(base_expr, context, gen_stmts))};
int index = 0;
while (index < overrides.size()){
{
parts.push_back(expr::mutable_member_assignment(mlc::String("__upd"), cpp_naming::cpp_safe(overrides[index]->name), eval_expr_fn(overrides[index]->val, context, gen_stmts)));
index = index + 1;
}
}
return expr::record_update_lazy_closure(parts.join(mlc::String("")));
}

mlc::String gen_record_expr(mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> field_values, std::shared_ptr<registry::Type> expr_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
mlc::String cpp_name = [&]() -> mlc::String { if (std::holds_alternative<registry::TGeneric>((*expr_type))) { auto _v_tgeneric = std::get<registry::TGeneric>((*expr_type)); auto [_w0, _w1] = _v_tgeneric; return type_gen::sem_type_to_cpp(context, expr_type); } return context::context_resolve(context, type_name); }();
mlc::Array<mlc::String> field_order = decl_index::lookup_fields(context.field_orders, type_name);
mlc::String values_code = field_order.size() > 0 ? gen_record_ordered(field_values, field_order, context, gen_stmts, eval_expr_fn) : gen_record_unordered(field_values, context, gen_stmts, eval_expr_fn);
return expr::record_initializer(cpp_name, values_code);
}

mlc::String gen_record_update_expr(mlc::String type_name, std::shared_ptr<semantic_ir::SExpr> base_expr, mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> overrides, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)>)> eval_expr_fn) noexcept{
mlc::Array<mlc::String> field_order = decl_index::lookup_fields(context.field_orders, type_name);
return field_order.size() > 0 ? expr::record_initializer(context::context_resolve(context, type_name), gen_record_update_ordered(base_expr, overrides, field_order, context, gen_stmts, eval_expr_fn)) : gen_record_update_lazy(base_expr, overrides, context, gen_stmts, eval_expr_fn);
}

} // namespace record_gen
