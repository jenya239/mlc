#define main mlc_user_main
#include "infer.hpp"

namespace infer {

InferResult InferResult_with_type(InferResult self, std::shared_ptr<registry::Type> new_type) noexcept{
return InferResult{new_type, self.errors};
}
InferResult InferResult_absorb(InferResult self, InferResult other) noexcept{
return InferResult{self.inferred_type, ast::errs_append(self.errors, other.errors)};
}
InferResult InferResult_absorb_stmt(InferResult self, StmtInferResult stmt_result) noexcept{
return InferResult{self.inferred_type, ast::errs_append(stmt_result.errors, self.errors)};
}
InferResult infer_ok(std::shared_ptr<registry::Type> type_val) noexcept{
return InferResult{type_val, {}};
}
std::shared_ptr<registry::Type> binary_op_type(mlc::String op, std::shared_ptr<registry::Type> left_type) noexcept{
if ((((((op == mlc::String("+", 1)) || (op == mlc::String("-", 1))) || (op == mlc::String("*", 1))) || (op == mlc::String("/", 1))) || (op == mlc::String("%", 1)))) {
return std::visit(overloaded{[&](const registry::TString& tString) { return std::make_shared<registry::Type>(registry::TString{}); },
[&](const auto& __v) { return std::make_shared<registry::Type>(registry::TI32{}); }
}, (*left_type));
} else if ((op == mlc::String("=", 1))) {
return std::make_shared<registry::Type>(registry::TUnit{});
} else {
return std::make_shared<registry::Type>(registry::TBool{});
}

}
std::shared_ptr<registry::Type> method_return_type(mlc::String method_name) noexcept{
if ((((method_name == mlc::String("length", 6)) || (method_name == mlc::String("size", 4))) || (method_name == mlc::String("to_i", 4)))) {
return std::make_shared<registry::Type>(registry::TI32{});
} else if (((method_name == mlc::String("push", 4)) || (method_name == mlc::String("set", 3)))) {
return std::make_shared<registry::Type>(registry::TUnit{});
} else if ((((((method_name == mlc::String("char_at", 7)) || (method_name == mlc::String("join", 4))) || (method_name == mlc::String("to_string", 9))) || (method_name == mlc::String("substring", 9))) || (method_name == mlc::String("to_lower", 8)))) {
return std::make_shared<registry::Type>(registry::TString{});
} else if ((method_name == mlc::String("has", 3))) {
return std::make_shared<registry::Type>(registry::TBool{});
} else {
return std::make_shared<registry::Type>(registry::TUnknown{});
}



}
InferResult infer_args_errors(InferResult initial, mlc::Array<std::shared_ptr<ast::Expr>> exprs, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env, registry::TypeRegistry registry) noexcept{
auto result = initial;
auto index = 0;
while ((index < exprs.length())) {
result = InferResult_absorb(result, infer_expr(exprs[index], type_env, registry));
index = (index + 1);
}
return result;
}
InferResult infer_field_vals_errors(InferResult initial, mlc::Array<std::shared_ptr<ast::FieldVal>> field_vals, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env, registry::TypeRegistry registry) noexcept{
auto result = initial;
auto index = 0;
while ((index < field_vals.length())) {
result = InferResult_absorb(result, infer_expr(field_vals[index]->val, type_env, registry));
index = (index + 1);
}
return result;
}
std::shared_ptr<registry::Type> field_lookup_type(std::shared_ptr<registry::Type> obj_type, mlc::String field_name, registry::TypeRegistry registry) noexcept{
auto type_name = std::visit(overloaded{[&](const registry::TNamed& tNamed) { auto [name] = tNamed; return name; },
[&](const auto& __v) { return mlc::String("", 0); }
}, (*obj_type));
if (((type_name != mlc::String("", 0)) && registry::TypeRegistry_has_fields(registry, type_name))) {
auto field_map = registry::TypeRegistry_fields_for(registry, type_name);
if (field_map.has(field_name)) {
return field_map.get(field_name);
} else {
return std::make_shared<registry::Type>(registry::TUnknown{});
}
} else {
return std::make_shared<registry::Type>(registry::TUnknown{});
}
}
InferResult infer_expr(std::shared_ptr<ast::Expr> expr, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env, registry::TypeRegistry registry) noexcept{
return std::visit(overloaded{[&](const ast::ExprInt& exprInt) { auto [__0] = exprInt; return infer_ok(std::make_shared<registry::Type>(registry::TI32{})); },
[&](const ast::ExprStr& exprStr) { auto [__0] = exprStr; return infer_ok(std::make_shared<registry::Type>(registry::TString{})); },
[&](const ast::ExprBool& exprBool) { auto [__0] = exprBool; return infer_ok(std::make_shared<registry::Type>(registry::TBool{})); },
[&](const ast::ExprUnit& exprUnit) { return infer_ok(std::make_shared<registry::Type>(registry::TUnit{})); },
[&](const ast::ExprExtern& exprExtern) { return infer_ok(std::make_shared<registry::Type>(registry::TUnit{})); },
[&](const ast::ExprIdent& exprIdent) { auto [name] = exprIdent; return (type_env.has(name) ? infer_ok(type_env.get(name)) : (registry::TypeRegistry_has_fn(registry, name) ? infer_ok(registry::TypeRegistry_fn_type(registry, name)) : (registry::TypeRegistry_has_ctor(registry, name) ? infer_ok(registry::TypeRegistry_ctor_type(registry, name)) : infer_ok(std::make_shared<registry::Type>(registry::TUnknown{}))))); },
[&](const ast::ExprBin& exprBin) { auto [op, left, right] = exprBin; return [&]() {
auto left_result = infer_expr(left, type_env, registry);
auto right_result = infer_expr(right, type_env, registry);
return InferResult_with_type(InferResult_absorb(left_result, right_result), binary_op_type(op, left_result.inferred_type));
}(); },
[&](const ast::ExprUn& exprUn) { auto [op, inner] = exprUn; return [&]() {
auto inner_result = infer_expr(inner, type_env, registry);
auto result_type = ((op == mlc::String("!", 1)) ? std::make_shared<registry::Type>(registry::TBool{}) : inner_result.inferred_type);
return InferResult_with_type(inner_result, result_type);
}(); },
[&](const ast::ExprCall& exprCall) { auto [func, call_args] = exprCall; return [&]() {
auto func_result = infer_expr(func, type_env, registry);
auto with_args = infer_args_errors(func_result, call_args, type_env, registry);
auto return_type = std::visit(overloaded{[&](const registry::TFn& tFn) { auto [__0, ret] = tFn; return ret; },
[&](const auto& __v) { return std::make_shared<registry::Type>(registry::TUnknown{}); }
}, (*func_result.inferred_type));
return InferResult_with_type(with_args, return_type);
}(); },
[&](const ast::ExprMethod& exprMethod) { auto [obj, method_name, margs] = exprMethod; return [&]() {
auto obj_result = infer_expr(obj, type_env, registry);
return InferResult_with_type(infer_args_errors(obj_result, margs, type_env, registry), method_return_type(method_name));
}(); },
[&](const ast::ExprField& exprField) { auto [obj, field_name] = exprField; return [&]() {
auto obj_result = infer_expr(obj, type_env, registry);
return InferResult_with_type(obj_result, field_lookup_type(obj_result.inferred_type, field_name, registry));
}(); },
[&](const ast::ExprIndex& exprIndex) { auto [obj, idx] = exprIndex; return [&]() {
auto obj_result = infer_expr(obj, type_env, registry);
auto idx_result = infer_expr(idx, type_env, registry);
auto elem_type = std::visit(overloaded{[&](const registry::TArray& tArray) { auto [inner] = tArray; return inner; },
[&](const auto& __v) { return std::make_shared<registry::Type>(registry::TUnknown{}); }
}, (*obj_result.inferred_type));
return InferResult_with_type(InferResult_absorb(obj_result, idx_result), elem_type);
}(); },
[&](const ast::ExprIf& exprIf) { auto [cond, then_expr, else_expr] = exprIf; return [&]() {
auto cond_result = infer_expr(cond, type_env, registry);
auto then_result = infer_expr(then_expr, type_env, registry);
auto else_result = infer_expr(else_expr, type_env, registry);
return InferResult_absorb(InferResult_absorb(then_result, cond_result), else_result);
}(); },
[&](const ast::ExprBlock& exprBlock) { auto [stmts, result] = exprBlock; return [&]() {
auto stmts_result = infer_stmts(stmts, type_env, registry);
auto result_infer = infer_expr(result, stmts_result.type_env, registry);
return InferResult_absorb_stmt(result_infer, stmts_result);
}(); },
[&](const ast::ExprWhile& exprWhile) { auto [cond, stmts] = exprWhile; return [&]() {
auto cond_result = infer_expr(cond, type_env, registry);
auto stmts_result = infer_stmts(stmts, type_env, registry);
return InferResult_with_type(InferResult_absorb_stmt(cond_result, stmts_result), std::make_shared<registry::Type>(registry::TUnit{}));
}(); },
[&](const ast::ExprFor& exprFor) { auto [var, iter, stmts] = exprFor; return [&]() {
auto iter_result = infer_expr(iter, type_env, registry);
auto elem_type = std::visit(overloaded{[&](const registry::TArray& tArray) { auto [inner] = tArray; return inner; },
[&](const auto& __v) { return std::make_shared<registry::Type>(registry::TUnknown{}); }
}, (*iter_result.inferred_type));
auto inner_env = type_env;
inner_env.set(var, elem_type);
auto stmts_result = infer_stmts(stmts, inner_env, registry);
return InferResult_with_type(InferResult_absorb_stmt(iter_result, stmts_result), std::make_shared<registry::Type>(registry::TUnit{}));
}(); },
[&](const ast::ExprMatch& exprMatch) { auto [subject, arms] = exprMatch; return [&]() {
auto subject_result = infer_expr(subject, type_env, registry);
auto errors = subject_result.errors;
auto arm_type = std::make_shared<registry::Type>(registry::TUnknown{});
auto i = 0;
while ((i < arms.length())) {
auto arm_env = env_for_pat(type_env, arms[i]->pat, registry);
auto arm_result = infer_expr(arms[i]->body, arm_env, registry);
errors = ast::errs_append(errors, arm_result.errors);
if ((i == 0)) {
arm_type = arm_result.inferred_type;
}
i = (i + 1);
}
return InferResult{arm_type, errors};
}(); },
[&](const ast::ExprRecord& exprRecord) { auto [name, field_vals] = exprRecord; return infer_field_vals_errors(infer_ok(std::make_shared<registry::Type>(registry::TNamed{name})), field_vals, type_env, registry); },
[&](const ast::ExprRecordUpdate& exprRecordUpdate) { auto [name, base, field_vals] = exprRecordUpdate; return [&]() {
auto base_result = infer_expr(base, type_env, registry);
return infer_field_vals_errors(InferResult_with_type(base_result, std::make_shared<registry::Type>(registry::TNamed{name})), field_vals, type_env, registry);
}(); },
[&](const ast::ExprArray& exprArray) { auto [exprs] = exprArray; return [&]() {
auto first_type = ((exprs.length() > 0) ? infer_expr(exprs[0], type_env, registry).inferred_type : std::make_shared<registry::Type>(registry::TUnknown{}));
return infer_args_errors(infer_ok(std::make_shared<registry::Type>(registry::TArray{first_type})), exprs, type_env, registry);
}(); },
[&](const ast::ExprQuestion& exprQuestion) { auto [inner] = exprQuestion; return infer_expr(inner, type_env, registry); },
[&](const ast::ExprLambda& exprLambda) { auto [params, body] = exprLambda; return [&]() {
auto lambda_env = type_env;
auto i = 0;
while ((i < params.length())) {
lambda_env.set(params[i], std::make_shared<registry::Type>(registry::TUnknown{}));
i = (i + 1);
}
auto ret_result = infer_expr(body, lambda_env, registry);
return infer_ok(std::make_shared<registry::Type>(registry::TFn{{}, ret_result.inferred_type}));
}(); }
}, (*expr));
}
StmtInferResult infer_stmts(mlc::Array<std::shared_ptr<ast::Stmt>> stmts, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env, registry::TypeRegistry registry) noexcept{
auto errors = mlc::Array<mlc::String>{};
auto current_env = type_env;
auto i = 0;
while ((i < stmts.length())) {
std::visit(overloaded{[&](const ast::StmtLet& stmtLet) { auto [name, __1, __2, value] = stmtLet; return [&]() {
auto value_result = infer_expr(value, current_env, registry);
current_env.set(name, value_result.inferred_type);
errors = ast::errs_append(errors, value_result.errors);
/* unit */;
return /* unit */;
}(); },
[&](const ast::StmtExpr& stmtExpr) { auto [expr] = stmtExpr; return [&]() {
auto expr_res = infer_expr(expr, current_env, registry);
errors = ast::errs_append(errors, expr_res.errors);
/* unit */;
return /* unit */;
}(); },
[&](const ast::StmtReturn& stmtReturn) { auto [expr] = stmtReturn; return [&]() {
auto ret_res = infer_expr(expr, current_env, registry);
errors = ast::errs_append(errors, ret_res.errors);
/* unit */;
return /* unit */;
}(); },
[&](const ast::StmtBreak& stmtBreak) { return [&]() {
/* unit */;
return /* unit */;
}(); },
[&](const ast::StmtContinue& stmtContinue) { return [&]() {
/* unit */;
return /* unit */;
}(); }
}, (*stmts[i]));
i = (i + 1);
}
return StmtInferResult{current_env, errors};
}
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> env_for_pat(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env, std::shared_ptr<ast::Pat> pat, registry::TypeRegistry registry) noexcept{
return std::visit(overloaded{[&](const ast::PatIdent& patIdent) { auto [name] = patIdent; return [&]() {
auto env = type_env;
env.set(name, std::make_shared<registry::Type>(registry::TUnknown{}));
return env;
}(); },
[&](const ast::PatCtor& patCtor) { auto [ctor_name, sub_pats] = patCtor; return [&]() {
auto param_types = registry::TypeRegistry_ctor_params_for(registry, ctor_name);
auto current_env = type_env;
auto i = 0;
while ((i < sub_pats.length())) {
auto sub_type = ((i < param_types.length()) ? param_types[i] : std::make_shared<registry::Type>(registry::TUnknown{}));
current_env = env_for_pat_with_type(current_env, sub_pats[i], sub_type, registry);
i = (i + 1);
}
return current_env;
}(); },
[&](const auto& __v) { return type_env; }
}, (*pat));
}
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> env_for_pat_with_type(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env, std::shared_ptr<ast::Pat> pat, std::shared_ptr<registry::Type> type_val, registry::TypeRegistry registry) noexcept{
return std::visit(overloaded{[&](const ast::PatIdent& patIdent) { auto [name] = patIdent; return [&]() {
auto env = type_env;
env.set(name, type_val);
return env;
}(); },
[&](const auto& __v) { return env_for_pat(type_env, pat, registry); }
}, (*pat));
}
bool CheckOut_has_errors(CheckOut self) noexcept{
return (self.errors.length() > 0);
}
ast::Result<CheckOut, mlc::Array<mlc::String>> check_with_context(ast::Program entry, ast::Program full) noexcept{
auto globals = names::collect_globals(full);
auto registry = registry::build_registry(full);
auto all_errors = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < entry.decls.length())) {
std::visit(overloaded{[&](const ast::DeclFn& declFn) { auto [__0, type_params, __2, params, return_type, body] = declFn; return [&]() {
auto locals = mlc::Array<mlc::String>{};
auto tpi = 0;
while ((tpi < type_params.length())) {
locals.push_back(type_params[tpi]);
globals.set(type_params[tpi], true);
tpi = (tpi + 1);
}
auto type_env = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
auto pi = 0;
while ((pi < params.length())) {
locals.push_back(ast::param_name(params[pi]));
type_env.set(ast::param_name(params[pi]), registry::type_from_annotation(ast::param_typ(params[pi])));
pi = (pi + 1);
}
all_errors = ast::errs_append(all_errors, names::check_names_expr(body, locals, globals));
auto infer_result = infer_expr(body, type_env, registry);
all_errors = ast::errs_append(all_errors, infer_result.errors);
/* unit */;
return /* unit */;
}(); },
[&](const ast::DeclType& declType) { auto [__0, __1, __2] = declType; return [&]() {
/* unit */;
return /* unit */;
}(); },
[&](const ast::DeclTrait& declTrait) { auto [__0, __1, __2] = declTrait; return [&]() {
/* unit */;
return /* unit */;
}(); },
[&](const ast::DeclExtend& declExtend) { auto [__0, __1, __2] = declExtend; return [&]() {
/* unit */;
return /* unit */;
}(); },
[&](const ast::DeclImport& declImport) { auto [__0, __1] = declImport; return [&]() {
/* unit */;
return /* unit */;
}(); },
[&](const ast::DeclExported& declExported) { auto [d] = declExported; return [&]() {
/* unit */;
return /* unit */;
}(); }
}, (*ast::decl_inner(entry.decls[i])));
i = (i + 1);
}
if ((all_errors.length() > 0)) {
return ast::Err{all_errors};
} else {
return ast::Ok{CheckOut{{}, registry}};
}
}
ast::Result<CheckOut, mlc::Array<mlc::String>> check(ast::Program prog) noexcept{
auto globals = names::collect_globals(prog);
auto registry = registry::build_registry(prog);
auto all_errors = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < prog.decls.length())) {
std::visit(overloaded{[&](const ast::DeclFn& declFn) { auto [__0, type_params, __2, params, return_type, body] = declFn; return [&]() {
auto locals = mlc::Array<mlc::String>{};
auto tpi = 0;
while ((tpi < type_params.length())) {
locals.push_back(type_params[tpi]);
globals.set(type_params[tpi], true);
tpi = (tpi + 1);
}
auto type_env = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
auto pi = 0;
while ((pi < params.length())) {
locals.push_back(ast::param_name(params[pi]));
type_env.set(ast::param_name(params[pi]), registry::type_from_annotation(ast::param_typ(params[pi])));
pi = (pi + 1);
}
all_errors = ast::errs_append(all_errors, names::check_names_expr(body, locals, globals));
auto infer_result = infer_expr(body, type_env, registry);
all_errors = ast::errs_append(all_errors, infer_result.errors);
/* unit */;
return /* unit */;
}(); },
[&](const ast::DeclType& declType) { auto [__0, __1, __2] = declType; return [&]() {
/* unit */;
return /* unit */;
}(); },
[&](const ast::DeclTrait& declTrait) { auto [__0, __1, __2] = declTrait; return [&]() {
/* unit */;
return /* unit */;
}(); },
[&](const ast::DeclExtend& declExtend) { auto [__0, __1, __2] = declExtend; return [&]() {
/* unit */;
return /* unit */;
}(); },
[&](const ast::DeclImport& declImport) { auto [__0, __1] = declImport; return [&]() {
/* unit */;
return /* unit */;
}(); },
[&](const ast::DeclExported& declExported) { auto [d] = declExported; return [&]() {
/* unit */;
return /* unit */;
}(); }
}, (*ast::decl_inner(prog.decls[i])));
i = (i + 1);
}
if ((all_errors.length() > 0)) {
return ast::Err{all_errors};
} else {
return ast::Ok{CheckOut{{}, registry}};
}
}

} // namespace infer
