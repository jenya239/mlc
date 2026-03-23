#include "infer.hpp"

#include "ast.hpp"
#include "names.hpp"
#include "registry.hpp"

namespace infer {

using namespace ast;
using namespace names;
using namespace registry;
using namespace ast_tokens;

struct InferResult {std::shared_ptr<registry::Type> inferred_type;mlc::Array<mlc::String> errors;};

struct StmtInferResult {mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env;mlc::Array<mlc::String> errors;};

infer::InferResult InferResult_with_type(infer::InferResult self, std::shared_ptr<registry::Type> new_type) noexcept;

infer::InferResult InferResult_absorb(infer::InferResult self, infer::InferResult other) noexcept;

infer::InferResult InferResult_absorb_stmt(infer::InferResult self, infer::StmtInferResult stmt_result) noexcept;

infer::InferResult infer_ok(std::shared_ptr<registry::Type> type_val) noexcept;

std::shared_ptr<registry::Type> binary_op_type(mlc::String op, std::shared_ptr<registry::Type> left_type) noexcept;

std::shared_ptr<registry::Type> method_return_type(mlc::String method_name) noexcept;

infer::InferResult infer_args_errors(infer::InferResult initial, mlc::Array<std::shared_ptr<ast::Expr>> exprs, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env, registry::TypeRegistry registry) noexcept;

infer::InferResult infer_field_vals_errors(infer::InferResult initial, mlc::Array<std::shared_ptr<ast::FieldVal>> field_vals, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env, registry::TypeRegistry registry) noexcept;

std::shared_ptr<registry::Type> field_lookup_type(std::shared_ptr<registry::Type> obj_type, mlc::String field_name, registry::TypeRegistry registry) noexcept;

infer::InferResult infer_expr(std::shared_ptr<ast::Expr> expr, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env, registry::TypeRegistry registry) noexcept;

infer::StmtInferResult infer_stmts(mlc::Array<std::shared_ptr<ast::Stmt>> stmts, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env, registry::TypeRegistry registry) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> env_for_pat(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env, std::shared_ptr<ast::Pat> pat, registry::TypeRegistry registry) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> env_for_pat_with_type(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env, std::shared_ptr<ast::Pat> pat, std::shared_ptr<registry::Type> type_val, registry::TypeRegistry registry) noexcept;

bool CheckOut_has_errors(infer::CheckOut self) noexcept;

ast::Result<infer::CheckOut, mlc::Array<mlc::String>> check_with_context(ast::Program entry, ast::Program full) noexcept;

ast::Result<infer::CheckOut, mlc::Array<mlc::String>> check(ast::Program prog) noexcept;

infer::InferResult InferResult_with_type(infer::InferResult self, std::shared_ptr<registry::Type> new_type) noexcept{return infer::InferResult{new_type, self.errors};}

infer::InferResult InferResult_absorb(infer::InferResult self, infer::InferResult other) noexcept{return infer::InferResult{self.inferred_type, ast::errs_append(self.errors, other.errors)};}

infer::InferResult InferResult_absorb_stmt(infer::InferResult self, infer::StmtInferResult stmt_result) noexcept{return infer::InferResult{self.inferred_type, ast::errs_append(stmt_result.errors, self.errors)};}

infer::InferResult infer_ok(std::shared_ptr<registry::Type> type_val) noexcept{return infer::InferResult{type_val, {}};}

std::shared_ptr<registry::Type> binary_op_type(mlc::String op, std::shared_ptr<registry::Type> left_type) noexcept{return op == mlc::String("+") || op == mlc::String("-") || op == mlc::String("*") || op == mlc::String("/") || op == mlc::String("%") ? [&]() { if (std::holds_alternative<registry::TString>((*left_type))) {  return std::make_shared<registry::Type>((registry::TString{})); } return std::make_shared<registry::Type>((registry::TI32{})); }() : op == mlc::String("=") ? std::make_shared<registry::Type>((registry::TUnit{})) : std::make_shared<registry::Type>((registry::TBool{}));}

std::shared_ptr<registry::Type> method_return_type(mlc::String method_name) noexcept{return method_name == mlc::String("length") || method_name == mlc::String("size") || method_name == mlc::String("to_i") ? std::make_shared<registry::Type>((registry::TI32{})) : method_name == mlc::String("push") || method_name == mlc::String("set") ? std::make_shared<registry::Type>((registry::TUnit{})) : method_name == mlc::String("char_at") || method_name == mlc::String("join") || method_name == mlc::String("to_string") || method_name == mlc::String("substring") || method_name == mlc::String("to_lower") ? std::make_shared<registry::Type>((registry::TString{})) : method_name == mlc::String("has") ? std::make_shared<registry::Type>((registry::TBool{})) : std::make_shared<registry::Type>((registry::TUnknown{}));}

infer::InferResult infer_args_errors(infer::InferResult initial, mlc::Array<std::shared_ptr<ast::Expr>> exprs, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env, registry::TypeRegistry registry) noexcept{
infer::InferResult result = std::move(initial);
int index = 0;
while (index < exprs.size()){
{
result = InferResult_absorb(result, infer_expr(exprs[index], type_env, registry));
index = index + 1;
}
}
return result;
}

infer::InferResult infer_field_vals_errors(infer::InferResult initial, mlc::Array<std::shared_ptr<ast::FieldVal>> field_vals, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env, registry::TypeRegistry registry) noexcept{
infer::InferResult result = std::move(initial);
int index = 0;
while (index < field_vals.size()){
{
result = InferResult_absorb(result, infer_expr(field_vals[index]->val, type_env, registry));
index = index + 1;
}
}
return result;
}

std::shared_ptr<registry::Type> field_lookup_type(std::shared_ptr<registry::Type> obj_type, mlc::String field_name, registry::TypeRegistry registry) noexcept{
mlc::String type_name = [&]() -> mlc::String { if (std::holds_alternative<registry::TNamed>((*obj_type))) { auto _v_tnamed = std::get<registry::TNamed>((*obj_type)); auto [name] = _v_tnamed; return name; } return mlc::String(""); }();
return type_name != mlc::String("") && registry::TypeRegistry_has_fields(registry, type_name) ? [&]() -> std::shared_ptr<registry::Type> { 
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> field_map = registry::TypeRegistry_fields_for(registry, type_name);
  return field_map.has(field_name) ? field_map.get(field_name) : std::make_shared<registry::Type>((registry::TUnknown{}));
 }() : std::make_shared<registry::Type>((registry::TUnknown{}));
}

infer::InferResult infer_expr(std::shared_ptr<ast::Expr> expr, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env, registry::TypeRegistry registry) noexcept{
return std::visit(overloaded{
  [&](const ExprInt& exprint) -> infer::InferResult { auto [_w0] = exprint; return infer_ok(std::make_shared<registry::Type>((registry::TI32{}))); },
  [&](const ExprStr& exprstr) -> infer::InferResult { auto [_w0] = exprstr; return infer_ok(std::make_shared<registry::Type>((registry::TString{}))); },
  [&](const ExprBool& exprbool) -> infer::InferResult { auto [_w0] = exprbool; return infer_ok(std::make_shared<registry::Type>((registry::TBool{}))); },
  [&](const ExprUnit& exprunit) -> infer::InferResult { return infer_ok(std::make_shared<registry::Type>((registry::TUnit{}))); },
  [&](const ExprExtern& exprextern) -> infer::InferResult { return infer_ok(std::make_shared<registry::Type>((registry::TUnit{}))); },
  [&](const ExprIdent& exprident) -> infer::InferResult { auto [name] = exprident; return type_env.has(name) ? infer_ok(type_env.get(name)) : registry::TypeRegistry_has_fn(registry, name) ? infer_ok(registry::TypeRegistry_fn_type(registry, name)) : registry::TypeRegistry_has_ctor(registry, name) ? infer_ok(registry::TypeRegistry_ctor_type(registry, name)) : infer_ok(std::make_shared<registry::Type>((registry::TUnknown{}))); },
  [&](const ExprBin& exprbin) -> infer::InferResult { auto [op, left, right] = exprbin; return [&]() -> infer::InferResult { 
  infer::InferResult left_result = infer_expr(left, type_env, registry);
  infer::InferResult right_result = infer_expr(right, type_env, registry);
  return InferResult_with_type(InferResult_absorb(left_result, right_result), binary_op_type(op, left_result.inferred_type));
 }(); },
  [&](const ExprUn& exprun) -> infer::InferResult { auto [op, inner] = exprun; return [&]() -> infer::InferResult { 
  infer::InferResult inner_result = infer_expr(inner, type_env, registry);
  auto result_type = op == mlc::String("!") ? std::make_shared<registry::Type>((registry::TBool{})) : inner_result.inferred_type;
  return InferResult_with_type(inner_result, result_type);
 }(); },
  [&](const ExprCall& exprcall) -> infer::InferResult { auto [func, call_args] = exprcall; return [&]() -> infer::InferResult { 
  infer::InferResult func_result = infer_expr(func, type_env, registry);
  infer::InferResult with_args = infer_args_errors(func_result, call_args, type_env, registry);
  std::shared_ptr<registry::Type> return_type = [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TFn>((*func_result.inferred_type))) { auto _v_tfn = std::get<registry::TFn>((*func_result.inferred_type)); auto [_w0, ret] = _v_tfn; return ret; } return std::make_shared<registry::Type>((registry::TUnknown{})); }();
  return InferResult_with_type(with_args, return_type);
 }(); },
  [&](const ExprMethod& exprmethod) -> infer::InferResult { auto [obj, method_name, margs] = exprmethod; return [&]() -> infer::InferResult { 
  infer::InferResult obj_result = infer_expr(obj, type_env, registry);
  return InferResult_with_type(infer_args_errors(obj_result, margs, type_env, registry), method_return_type(method_name));
 }(); },
  [&](const ExprField& exprfield) -> infer::InferResult { auto [obj, field_name] = exprfield; return [&]() -> infer::InferResult { 
  infer::InferResult obj_result = infer_expr(obj, type_env, registry);
  return InferResult_with_type(obj_result, field_lookup_type(obj_result.inferred_type, field_name, registry));
 }(); },
  [&](const ExprIndex& exprindex) -> infer::InferResult { auto [obj, idx] = exprindex; return [&]() -> infer::InferResult { 
  infer::InferResult obj_result = infer_expr(obj, type_env, registry);
  infer::InferResult idx_result = infer_expr(idx, type_env, registry);
  std::shared_ptr<registry::Type> elem_type = [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TArray>((*obj_result.inferred_type))) { auto _v_tarray = std::get<registry::TArray>((*obj_result.inferred_type)); auto [inner] = _v_tarray; return inner; } return std::make_shared<registry::Type>((registry::TUnknown{})); }();
  return InferResult_with_type(InferResult_absorb(obj_result, idx_result), elem_type);
 }(); },
  [&](const ExprIf& exprif) -> infer::InferResult { auto [cond, then_expr, else_expr] = exprif; return [&]() -> infer::InferResult { 
  infer::InferResult cond_result = infer_expr(cond, type_env, registry);
  infer::InferResult then_result = infer_expr(then_expr, type_env, registry);
  infer::InferResult else_result = infer_expr(else_expr, type_env, registry);
  return InferResult_absorb(InferResult_absorb(then_result, cond_result), else_result);
 }(); },
  [&](const ExprBlock& exprblock) -> infer::InferResult { auto [stmts, result] = exprblock; return [&]() -> infer::InferResult { 
  infer::StmtInferResult stmts_result = infer_stmts(stmts, type_env, registry);
  infer::InferResult result_infer = infer_expr(result, stmts_result.type_env, registry);
  return InferResult_absorb_stmt(result_infer, stmts_result);
 }(); },
  [&](const ExprWhile& exprwhile) -> infer::InferResult { auto [cond, stmts] = exprwhile; return [&]() -> infer::InferResult { 
  infer::InferResult cond_result = infer_expr(cond, type_env, registry);
  infer::StmtInferResult stmts_result = infer_stmts(stmts, type_env, registry);
  return InferResult_with_type(InferResult_absorb_stmt(cond_result, stmts_result), std::make_shared<registry::Type>((registry::TUnit{})));
 }(); },
  [&](const ExprFor& exprfor) -> infer::InferResult { auto [var, iter, stmts] = exprfor; return [&]() -> infer::InferResult { 
  infer::InferResult iter_result = infer_expr(iter, type_env, registry);
  std::shared_ptr<registry::Type> elem_type = [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TArray>((*iter_result.inferred_type))) { auto _v_tarray = std::get<registry::TArray>((*iter_result.inferred_type)); auto [inner] = _v_tarray; return inner; } return std::make_shared<registry::Type>((registry::TUnknown{})); }();
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> inner_env = type_env;
  inner_env.set(var, elem_type);
  infer::StmtInferResult stmts_result = infer_stmts(stmts, inner_env, registry);
  return InferResult_with_type(InferResult_absorb_stmt(iter_result, stmts_result), std::make_shared<registry::Type>((registry::TUnit{})));
 }(); },
  [&](const ExprMatch& exprmatch) -> infer::InferResult { auto [subject, arms] = exprmatch; return [&]() -> infer::InferResult { 
  infer::InferResult subject_result = infer_expr(subject, type_env, registry);
  mlc::Array<mlc::String> errors = subject_result.errors;
  std::shared_ptr<registry::Type> arm_type = std::make_shared<registry::Type>((registry::TUnknown{}));
  int i = 0;
  while (i < arms.size()){
{
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> arm_env = env_for_pat(type_env, arms[i]->pat, registry);
infer::InferResult arm_result = infer_expr(arms[i]->body, arm_env, registry);
errors = ast::errs_append(errors, arm_result.errors);
if (i == 0){
{
arm_type = arm_result.inferred_type;
}
}
i = i + 1;
}
}
  return infer::InferResult{arm_type, errors};
 }(); },
  [&](const ExprRecord& exprrecord) -> infer::InferResult { auto [name, field_vals] = exprrecord; return infer_field_vals_errors(infer_ok(std::make_shared<registry::Type>(registry::TNamed(name))), field_vals, type_env, registry); },
  [&](const ExprRecordUpdate& exprrecordupdate) -> infer::InferResult { auto [name, base, field_vals] = exprrecordupdate; return [&]() -> infer::InferResult { 
  infer::InferResult base_result = infer_expr(base, type_env, registry);
  return infer_field_vals_errors(InferResult_with_type(base_result, std::make_shared<registry::Type>(registry::TNamed(name))), field_vals, type_env, registry);
 }(); },
  [&](const ExprArray& exprarray) -> infer::InferResult { auto [exprs] = exprarray; return [&]() -> infer::InferResult { 
  std::shared_ptr<registry::Type> first_type = exprs.size() > 0 ? infer_expr(exprs[0], type_env, registry).inferred_type : std::make_shared<registry::Type>((registry::TUnknown{}));
  return infer_args_errors(infer_ok(std::make_shared<registry::Type>(registry::TArray(first_type))), exprs, type_env, registry);
 }(); },
  [&](const ExprQuestion& exprquestion) -> infer::InferResult { auto [inner] = exprquestion; return infer_expr(inner, type_env, registry); },
  [&](const ExprLambda& exprlambda) -> infer::InferResult { auto [params, body] = exprlambda; return [&]() -> infer::InferResult { 
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> lambda_env = type_env;
  int i = 0;
  while (i < params.size()){
{
lambda_env.set(params[i], std::make_shared<registry::Type>((registry::TUnknown{})));
i = i + 1;
}
}
  infer::InferResult ret_result = infer_expr(body, lambda_env, registry);
  return infer_ok(std::make_shared<registry::Type>(registry::TFn({}, ret_result.inferred_type)));
 }(); }
}, (*expr)._);
}

infer::StmtInferResult infer_stmts(mlc::Array<std::shared_ptr<ast::Stmt>> stmts, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env, registry::TypeRegistry registry) noexcept{
mlc::Array<mlc::String> errors = {};
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> current_env = type_env;
int i = 0;
while (i < stmts.size()){
{
std::visit(overloaded{
  [&](const StmtLet& stmtlet) -> std::tuple<> { auto [name, _w0, _w1, value] = stmtlet; return [&]() -> std::tuple<> { 
  infer::InferResult value_result = infer_expr(value, current_env, registry);
  current_env.set(name, value_result.inferred_type);
  errors = ast::errs_append(errors, value_result.errors);
  return std::make_tuple();
 }(); },
  [&](const StmtExpr& stmtexpr) -> std::tuple<> { auto [expr] = stmtexpr; return [&]() -> std::tuple<> { 
  infer::InferResult expr_res = infer_expr(expr, current_env, registry);
  errors = ast::errs_append(errors, expr_res.errors);
  return std::make_tuple();
 }(); },
  [&](const StmtReturn& stmtreturn) -> std::tuple<> { auto [expr] = stmtreturn; return [&]() -> std::tuple<> { 
  infer::InferResult ret_res = infer_expr(expr, current_env, registry);
  errors = ast::errs_append(errors, ret_res.errors);
  return std::make_tuple();
 }(); },
  [&](const StmtBreak& stmtbreak) -> std::tuple<> { return std::make_tuple(); },
  [&](const StmtContinue& stmtcontinue) -> std::tuple<> { return std::make_tuple(); }
}, (*stmts[i])._);
i = i + 1;
}
}
return infer::StmtInferResult{current_env, errors};
}

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> env_for_pat(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env, std::shared_ptr<ast::Pat> pat, registry::TypeRegistry registry) noexcept{
return [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { if (std::holds_alternative<ast::PatIdent>((*pat))) { auto _v_patident = std::get<ast::PatIdent>((*pat)); auto [name] = _v_patident; return [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { 
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> env = type_env;
  env.set(name, std::make_shared<registry::Type>((registry::TUnknown{})));
  return env;
 }(); } if (std::holds_alternative<ast::PatCtor>((*pat))) { auto _v_patctor = std::get<ast::PatCtor>((*pat)); auto [ctor_name, sub_pats] = _v_patctor; return [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> param_types = registry::TypeRegistry_ctor_params_for(registry, ctor_name);
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> current_env = type_env;
  int i = 0;
  while (i < sub_pats.size()){
{
std::shared_ptr<registry::Type> sub_type = i < param_types.size() ? param_types[i] : std::make_shared<registry::Type>((registry::TUnknown{}));
current_env = env_for_pat_with_type(current_env, sub_pats[i], sub_type, registry);
i = i + 1;
}
}
  return current_env;
 }(); } return type_env; }();
}

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> env_for_pat_with_type(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env, std::shared_ptr<ast::Pat> pat, std::shared_ptr<registry::Type> type_val, registry::TypeRegistry registry) noexcept{
return [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { if (std::holds_alternative<ast::PatIdent>((*pat))) { auto _v_patident = std::get<ast::PatIdent>((*pat)); auto [name] = _v_patident; return [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { 
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> env = type_env;
  env.set(name, type_val);
  return env;
 }(); } return env_for_pat(type_env, pat, registry); }();
}

bool CheckOut_has_errors(infer::CheckOut self) noexcept{return self.errors.size() > 0;}

ast::Result<infer::CheckOut, mlc::Array<mlc::String>> check_with_context(ast::Program entry, ast::Program full) noexcept{
mlc::HashMap<mlc::String, bool> globals = names::collect_globals(full);
registry::TypeRegistry registry = registry::build_registry(full);
mlc::Array<mlc::String> all_errors = {};
int i = 0;
while (i < entry.decls.size()){
{
std::visit(overloaded{
  [&](const DeclFn& declfn) -> std::tuple<> { auto [_w0, type_params, _w1, params, return_type, body] = declfn; return [&]() -> std::tuple<> { 
  mlc::Array<mlc::String> locals = {};
  int tpi = 0;
  while (tpi < type_params.size()){
{
locals.push_back(type_params[tpi]);
globals.set(type_params[tpi], true);
tpi = tpi + 1;
}
}
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
  int pi = 0;
  while (pi < params.size()){
{
locals.push_back(ast::param_name(params[pi]));
type_env.set(ast::param_name(params[pi]), registry::type_from_annotation(ast::param_typ(params[pi])));
pi = pi + 1;
}
}
  all_errors = ast::errs_append(all_errors, names::check_names_expr(body, locals, globals));
  infer::InferResult infer_result = infer_expr(body, type_env, registry);
  all_errors = ast::errs_append(all_errors, infer_result.errors);
  return std::make_tuple();
 }(); },
  [&](const DeclType& decltype_) -> std::tuple<> { auto [_w0, _w1, _w2] = decltype_; return std::make_tuple(); },
  [&](const DeclTrait& decltrait) -> std::tuple<> { auto [_w0, _w1, _w2] = decltrait; return std::make_tuple(); },
  [&](const DeclExtend& declextend) -> std::tuple<> { auto [_w0, _w1, _w2] = declextend; return std::make_tuple(); },
  [&](const DeclImport& declimport) -> std::tuple<> { auto [_w0, _w1] = declimport; return std::make_tuple(); },
  [&](const DeclExported& declexported) -> std::tuple<> { auto [d] = declexported; return std::make_tuple(); }
}, (*ast::decl_inner(entry.decls[i])));
i = i + 1;
}
}
return all_errors.size() > 0 ? ast::Result<infer::CheckOut, mlc::Array<mlc::String>>(ast::Err<mlc::Array<mlc::String>>(all_errors)) : ast::Result<infer::CheckOut, mlc::Array<mlc::String>>(ast::Ok<infer::CheckOut>(infer::CheckOut{{}, registry}));
}

ast::Result<infer::CheckOut, mlc::Array<mlc::String>> check(ast::Program prog) noexcept{
mlc::HashMap<mlc::String, bool> globals = names::collect_globals(prog);
registry::TypeRegistry registry = registry::build_registry(prog);
mlc::Array<mlc::String> all_errors = {};
int i = 0;
while (i < prog.decls.size()){
{
std::visit(overloaded{
  [&](const DeclFn& declfn) -> std::tuple<> { auto [_w0, type_params, _w1, params, return_type, body] = declfn; return [&]() -> std::tuple<> { 
  mlc::Array<mlc::String> locals = {};
  int tpi = 0;
  while (tpi < type_params.size()){
{
locals.push_back(type_params[tpi]);
globals.set(type_params[tpi], true);
tpi = tpi + 1;
}
}
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
  int pi = 0;
  while (pi < params.size()){
{
locals.push_back(ast::param_name(params[pi]));
type_env.set(ast::param_name(params[pi]), registry::type_from_annotation(ast::param_typ(params[pi])));
pi = pi + 1;
}
}
  all_errors = ast::errs_append(all_errors, names::check_names_expr(body, locals, globals));
  infer::InferResult infer_result = infer_expr(body, type_env, registry);
  all_errors = ast::errs_append(all_errors, infer_result.errors);
  return std::make_tuple();
 }(); },
  [&](const DeclType& decltype_) -> std::tuple<> { auto [_w0, _w1, _w2] = decltype_; return std::make_tuple(); },
  [&](const DeclTrait& decltrait) -> std::tuple<> { auto [_w0, _w1, _w2] = decltrait; return std::make_tuple(); },
  [&](const DeclExtend& declextend) -> std::tuple<> { auto [_w0, _w1, _w2] = declextend; return std::make_tuple(); },
  [&](const DeclImport& declimport) -> std::tuple<> { auto [_w0, _w1] = declimport; return std::make_tuple(); },
  [&](const DeclExported& declexported) -> std::tuple<> { auto [d] = declexported; return std::make_tuple(); }
}, (*ast::decl_inner(prog.decls[i])));
i = i + 1;
}
}
return all_errors.size() > 0 ? ast::Result<infer::CheckOut, mlc::Array<mlc::String>>(ast::Err<mlc::Array<mlc::String>>(all_errors)) : ast::Result<infer::CheckOut, mlc::Array<mlc::String>>(ast::Ok<infer::CheckOut>(infer::CheckOut{{}, registry}));
}

} // namespace infer
