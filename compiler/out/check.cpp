#include "check.hpp"

#include "ast.hpp"
#include "ast.hpp"
#include "names.hpp"
#include "check_mutations.hpp"
#include "registry.hpp"
#include "trait_param_expand.hpp"
#include "infer.hpp"
#include "check_context.hpp"
#include "semantic_type_structure.hpp"

namespace check {

using namespace ast;
using namespace ast;
using namespace names;
using namespace check_mutations;
using namespace registry;
using namespace trait_param_expand;
using namespace infer;
using namespace check_context;
using namespace semantic_type_structure;
using namespace ast_tokens;

bool param_defaults_in_tail(mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept;

bool default_expr_mvp_ok(std::shared_ptr<ast::Expr> e) noexcept;

bool is_extern_body(std::shared_ptr<ast::Expr> e) noexcept;

mlc::Array<ast::Diagnostic> param_default_diagnostics(mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<ast::Expr> body, registry::TypeRegistry registry) noexcept;

bool type_parameter_name_known(mlc::Array<mlc::String> type_parameter_names, mlc::String candidate_name) noexcept;

mlc::Array<ast::Diagnostic> where_clause_unknown_parameter_diagnostics(mlc::Array<mlc::String> type_parameter_names, mlc::Array<ast::WhereClauseBound> where_entries, ast::Span source_span) noexcept;

mlc::HashMap<mlc::String, bool> collect_globals(ast::Program program) noexcept;

bool type_is_checkable(std::shared_ptr<registry::Type> type_value, registry::TypeRegistry registry) noexcept;

bool CheckOut_has_errors(check::CheckOut self) noexcept;

ast::Result<check::CheckOut, mlc::Array<mlc::String>> check_program_against_full(ast::Program entry, ast::Program full_program) noexcept;

ast::Result<check::CheckOut, mlc::Array<mlc::String>> check_with_context(ast::Program entry, ast::Program full) noexcept;

ast::Result<check::CheckOut, mlc::Array<mlc::String>> check(ast::Program program) noexcept;

bool param_defaults_in_tail(mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept{
bool optional_began = false;
int i = 0;
while (i < parameters.size()){
{
if (parameters[i]->has_default){
{
optional_began = true;
}
} else {
{
if (optional_began){
return false;
}
}
}
i = i + 1;
}
}
return true;
}

bool default_expr_mvp_ok(std::shared_ptr<ast::Expr> e) noexcept{return [&]() { if (std::holds_alternative<ast::ExprInt>((*e)._)) { auto _v_exprint = std::get<ast::ExprInt>((*e)._); auto [_w0, _w1] = _v_exprint; return true; } if (std::holds_alternative<ast::ExprStr>((*e)._)) { auto _v_exprstr = std::get<ast::ExprStr>((*e)._); auto [_w0, _w1] = _v_exprstr; return true; } if (std::holds_alternative<ast::ExprBool>((*e)._)) { auto _v_exprbool = std::get<ast::ExprBool>((*e)._); auto [_w0, _w1] = _v_exprbool; return true; } if (std::holds_alternative<ast::ExprUnit>((*e)._)) { auto _v_exprunit = std::get<ast::ExprUnit>((*e)._); auto [_w0] = _v_exprunit; return true; } if (std::holds_alternative<ast::ExprFloat>((*e)._)) { auto _v_exprfloat = std::get<ast::ExprFloat>((*e)._); auto [_w0, _w1] = _v_exprfloat; return true; } if (std::holds_alternative<ast::ExprI64>((*e)._)) { auto _v_expri64 = std::get<ast::ExprI64>((*e)._); auto [_w0, _w1] = _v_expri64; return true; } if (std::holds_alternative<ast::ExprU8>((*e)._)) { auto _v_expru8 = std::get<ast::ExprU8>((*e)._); auto [_w0, _w1] = _v_expru8; return true; } if (std::holds_alternative<ast::ExprUsize>((*e)._)) { auto _v_exprusize = std::get<ast::ExprUsize>((*e)._); auto [_w0, _w1] = _v_exprusize; return true; } if (std::holds_alternative<ast::ExprChar>((*e)._)) { auto _v_exprchar = std::get<ast::ExprChar>((*e)._); auto [_w0, _w1] = _v_exprchar; return true; } return false; }();}

bool is_extern_body(std::shared_ptr<ast::Expr> e) noexcept{return [&]() { if (std::holds_alternative<ast::ExprExtern>((*e)._)) { auto _v_exprextern = std::get<ast::ExprExtern>((*e)._); auto [_w0] = _v_exprextern; return true; } return false; }();}

mlc::Array<ast::Diagnostic> param_default_diagnostics(mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<ast::Expr> body, registry::TypeRegistry registry) noexcept{
mlc::Array<ast::Diagnostic> out = {};
if (!param_defaults_in_tail(parameters)){
{
out.push_back(ast::diagnostic_error(mlc::String("parameter defaults must be trailing"), ast::expr_span(body)));
}
}
bool has_any_default = false;
int j = 0;
while (j < parameters.size()){
{
if (parameters[j]->has_default){
{
has_any_default = true;
}
}
j = j + 1;
}
}
if (type_parameters.size() > 0 && has_any_default){
{
out.push_back(ast::diagnostic_error(mlc::String("default parameters are not allowed on generic functions yet"), ast::expr_span(body)));
}
}
if (is_extern_body(body) && has_any_default){
{
out.push_back(ast::diagnostic_error(mlc::String("default parameters are not allowed on extern functions"), ast::expr_span(body)));
}
}
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> empty_type_environment = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
check_context::CheckContext default_infer_context = check_context::check_context_new(empty_type_environment, registry);
int p = 0;
while (p < parameters.size()){
{
if (parameters[p]->has_default){
{
std::shared_ptr<ast::Expr> param_default_expr = parameters[p]->default_;
if (!default_expr_mvp_ok(param_default_expr)){
out.push_back(ast::diagnostic_error(mlc::String("parameter default must be a literal in this version"), ast::expr_span(param_default_expr)));
} else {
infer_result::InferResult default_infer = infer::infer_expr(param_default_expr, default_infer_context);
out = ast::diagnostics_append(out, default_infer.errors);
std::shared_ptr<registry::Type> param_expected_type = registry::type_from_annotation(ast::param_typ(parameters[p]));
if (type_is_checkable(param_expected_type, registry) && type_is_checkable(default_infer.inferred_type, registry) && !semantic_type_structure::types_structurally_equal(param_expected_type, default_infer.inferred_type)){
out.push_back(ast::diagnostic_error(mlc::String("parameter default: expected ") + semantic_type_structure::type_description(param_expected_type) + mlc::String(", got ") + semantic_type_structure::type_description(default_infer.inferred_type), ast::expr_span(param_default_expr)));
}
}
}
}
p = p + 1;
}
}
return out;
}

bool type_parameter_name_known(mlc::Array<mlc::String> type_parameter_names, mlc::String candidate_name) noexcept{
int index = 0;
while (index < type_parameter_names.size()){
{
if (type_parameter_names[index] == candidate_name){
{
return true;
}
}
index = index + 1;
}
}
return false;
}

mlc::Array<ast::Diagnostic> where_clause_unknown_parameter_diagnostics(mlc::Array<mlc::String> type_parameter_names, mlc::Array<ast::WhereClauseBound> where_entries, ast::Span source_span) noexcept{
mlc::Array<ast::Diagnostic> collected = {};
int entry_index = 0;
while (entry_index < where_entries.size()){
{
ast::WhereClauseBound entry = where_entries[entry_index];
if (!type_parameter_name_known(type_parameter_names, entry.parameter_name)){
{
collected.push_back(ast::diagnostic_error(mlc::String("where clause names unknown type parameter \"") + entry.parameter_name + mlc::String("\""), source_span));
}
}
entry_index = entry_index + 1;
}
}
return collected;
}

mlc::HashMap<mlc::String, bool> collect_globals(ast::Program program) noexcept{
mlc::HashMap<mlc::String, bool> names = mlc::HashMap<mlc::String, bool>();
names.set(mlc::String("true"), true);
names.set(mlc::String("false"), true);
names.set(mlc::String("exit"), true);
names.set(mlc::String("print"), true);
names.set(mlc::String("println"), true);
names.set(mlc::String("args"), true);
names.set(mlc::String("File"), true);
names.set(mlc::String("Shared"), true);
names.set(mlc::String("Map"), true);
names.set(mlc::String("Ok"), true);
names.set(mlc::String("Err"), true);
names.set(mlc::String("Result"), true);
int index = 0;
while (index < program.decls.size()){
{
std::visit(overloaded{
  [&](const DeclFn& declfn) -> std::tuple<> { auto [name, _w0, _w1, _w2, _w3, _w4, _w5] = declfn; return [&]() -> std::tuple<> { 
  names.set(name, true);
  return std::make_tuple();
 }(); },
  [&](const DeclType& decltype_) -> std::tuple<> { auto [name, _w0, variants, _w1] = decltype_; return [&]() -> std::tuple<> { 
  names.set(name, true);
  int variant_index = 0;
  while (variant_index < variants.size()){
{
std::visit(overloaded{
  [&](const VarUnit& varunit) -> std::tuple<> { auto [variant_name, _w0] = varunit; return [&]() -> std::tuple<> { 
  names.set(variant_name, true);
  return std::make_tuple();
 }(); },
  [&](const VarTuple& vartuple) -> std::tuple<> { auto [variant_name, _w0, _w1] = vartuple; return [&]() -> std::tuple<> { 
  names.set(variant_name, true);
  return std::make_tuple();
 }(); },
  [&](const VarRecord& varrecord) -> std::tuple<> { auto [variant_name, _w0, _w1] = varrecord; return [&]() -> std::tuple<> { 
  names.set(variant_name, true);
  return std::make_tuple();
 }(); }
}, (*variants[variant_index]));
variant_index = variant_index + 1;
}
}
  return std::make_tuple();
 }(); },
  [&](const DeclTrait& decltrait) -> std::tuple<> { auto [name, _w0, methods] = decltrait; return [&]() -> std::tuple<> { 
  names.set(name, true);
  int method_index = 0;
  while (method_index < methods.size()){
{
[&]() -> std::tuple<> { if (std::holds_alternative<ast::DeclFn>((*methods[method_index]))) { auto _v_declfn = std::get<ast::DeclFn>((*methods[method_index])); auto [function_name, _w0, _w1, _w2, _w3, _w4, _w5] = _v_declfn; return [&]() -> std::tuple<> { 
  names.set(function_name, true);
  return std::make_tuple();
 }(); } return std::make_tuple(); }();
method_index = method_index + 1;
}
}
  return std::make_tuple();
 }(); },
  [&](const DeclExtend& declextend) -> std::tuple<> { auto [_w0, _w1, _w2] = declextend; return std::make_tuple(); },
  [&](const DeclImport& declimport) -> std::tuple<> { auto [_w0, _w1] = declimport; return std::make_tuple(); },
  [&](const DeclExported& declexported) -> std::tuple<> { auto [_w0] = declexported; return std::make_tuple(); },
  [&](const DeclAssocType& declassoctype) -> std::tuple<> { auto [_w0, _w1] = declassoctype; return std::make_tuple(); },
  [&](const DeclAssocBind& declassocbind) -> std::tuple<> { auto [_w0, _w1, _w2] = declassocbind; return std::make_tuple(); }
}, (*ast::decl_inner(program.decls[index])));
index = index + 1;
}
}
return names;
}

bool type_is_checkable(std::shared_ptr<registry::Type> type_value, registry::TypeRegistry registry) noexcept{return [&]() { if (std::holds_alternative<registry::TI32>((*type_value))) {  return true; } if (std::holds_alternative<registry::TString>((*type_value))) {  return true; } if (std::holds_alternative<registry::TBool>((*type_value))) {  return true; } if (std::holds_alternative<registry::TUnit>((*type_value))) {  return true; } if (std::holds_alternative<registry::TI64>((*type_value))) {  return true; } if (std::holds_alternative<registry::TF64>((*type_value))) {  return true; } if (std::holds_alternative<registry::TU8>((*type_value))) {  return true; } if (std::holds_alternative<registry::TUsize>((*type_value))) {  return true; } if (std::holds_alternative<registry::TChar>((*type_value))) {  return true; } if (std::holds_alternative<registry::TArray>((*type_value))) { auto _v_tarray = std::get<registry::TArray>((*type_value)); auto [inner] = _v_tarray; return type_is_checkable(inner, registry); } if (std::holds_alternative<registry::TPair>((*type_value))) { auto _v_tpair = std::get<registry::TPair>((*type_value)); auto [a, b] = _v_tpair; return type_is_checkable(a, registry) && type_is_checkable(b, registry); } if (std::holds_alternative<registry::TTuple>((*type_value))) { auto _v_ttuple = std::get<registry::TTuple>((*type_value)); auto [ts] = _v_ttuple; return [&]() -> bool { 
  bool ok = true;
  int ti = 0;
  while (ti < ts.size() && ok){
{
ok = type_is_checkable(ts[ti], registry);
ti = ti + 1;
}
}
  return ok;
 }(); } if (std::holds_alternative<registry::TGeneric>((*type_value))) { auto _v_tgeneric = std::get<registry::TGeneric>((*type_value)); auto [_w0, args] = _v_tgeneric; return [&]() -> bool { 
  bool ok = true;
  int i = 0;
  while (i < args.size() && ok){
{
ok = type_is_checkable(args[i], registry);
i = i + 1;
}
}
  return ok;
 }(); } if (std::holds_alternative<registry::TFn>((*type_value))) { auto _v_tfn = std::get<registry::TFn>((*type_value)); auto [_w0, _w1] = _v_tfn; return true; } if (std::holds_alternative<registry::TShared>((*type_value))) { auto _v_tshared = std::get<registry::TShared>((*type_value)); auto [_w0] = _v_tshared; return true; } if (std::holds_alternative<registry::TNamed>((*type_value))) { auto _v_tnamed = std::get<registry::TNamed>((*type_value)); auto [name] = _v_tnamed; return registry::TypeRegistry_has_fields(registry, name); } if (std::holds_alternative<registry::TAssoc>((*type_value))) { auto _v_tassoc = std::get<registry::TAssoc>((*type_value)); auto [_w0, _w1] = _v_tassoc; return true; } return false; }();}

bool CheckOut_has_errors(check::CheckOut self) noexcept{return self.errors.size() > 0;}

ast::Result<check::CheckOut, mlc::Array<mlc::String>> check_program_against_full(ast::Program entry, ast::Program full_program) noexcept{
mlc::Array<ast::Diagnostic> all_diagnostics = trait_param_expand::trait_and_type_name_conflict_diagnostics(full_program);
ast::Program expanded_entry_program = trait_param_expand::expand_trait_as_param_entry_using_full(entry, full_program);
mlc::HashMap<mlc::String, bool> globals = collect_globals(full_program);
registry::TypeRegistry registry = registry::build_registry(full_program);
int declaration_index = 0;
while (declaration_index < expanded_entry_program.decls.size()){
{
std::visit(overloaded{
  [&](const DeclFn& declfn) -> std::tuple<> { auto [name, type_parameters, trait_bounds, parameters, return_type_annotation, body, where_clause_bounds_entries] = declfn; return [&]() -> std::tuple<> { 
  all_diagnostics = ast::diagnostics_append(all_diagnostics, where_clause_unknown_parameter_diagnostics(type_parameters, where_clause_bounds_entries, ast::expr_span(body)));
  all_diagnostics = ast::diagnostics_append(all_diagnostics, param_default_diagnostics(type_parameters, parameters, body, registry));
  mlc::Array<mlc::String> locals = {};
  int type_parameter_index = 0;
  while (type_parameter_index < type_parameters.size()){
{
locals.push_back(type_parameters[type_parameter_index]);
globals.set(type_parameters[type_parameter_index], true);
type_parameter_index = type_parameter_index + 1;
}
}
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
  int parameter_index = 0;
  while (parameter_index < parameters.size()){
{
locals.push_back(ast::param_name(parameters[parameter_index]));
type_environment.set(ast::param_name(parameters[parameter_index]), registry::type_from_annotation(ast::param_typ(parameters[parameter_index])));
parameter_index = parameter_index + 1;
}
}
  all_diagnostics = ast::diagnostics_append(all_diagnostics, names::check_names_expr(body, locals, globals));
  all_diagnostics = ast::diagnostics_append(all_diagnostics, check_mutations::check_fn_body_mutations(parameters, body));
  check_context::CheckContext inference_context = check_context::check_context_new(type_environment, registry);
  infer_result::InferResult inference_result = infer::infer_expr(body, inference_context);
  std::shared_ptr<registry::Type> expected_type = registry::type_from_annotation(return_type_annotation);
  std::shared_ptr<registry::Type> actual_type = inference_result.inferred_type;
  mlc::Array<ast::Diagnostic> return_type_errors = type_is_checkable(expected_type, registry) && type_is_checkable(actual_type, registry) && !semantic_type_structure::types_structurally_equal(expected_type, actual_type) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("return type: expected ") + semantic_type_structure::type_description(expected_type) + mlc::String(", got ") + semantic_type_structure::type_description(actual_type), ast::expr_span(body))} : mlc::Array<ast::Diagnostic>{};
  all_diagnostics = ast::diagnostics_append(ast::diagnostics_append(all_diagnostics, inference_result.errors), return_type_errors);
  return std::make_tuple();
 }(); },
  [&](const DeclType& decltype_) -> std::tuple<> { auto [_w0, _w1, _w2, _w3] = decltype_; return std::make_tuple(); },
  [&](const DeclTrait& decltrait) -> std::tuple<> { auto [_w0, _w1, _w2] = decltrait; return std::make_tuple(); },
  [&](const DeclExtend& declextend) -> std::tuple<> { auto [extend_type_name, _w0, methods] = declextend; return [&]() -> std::tuple<> { 
  int method_index = 0;
  while (method_index < methods.size()){
{
[&]() -> std::tuple<> { if (std::holds_alternative<ast::DeclFn>((*methods[method_index]))) { auto _v_declfn = std::get<ast::DeclFn>((*methods[method_index])); auto [_w0, type_params, _w1, params, _w2, method_body, where_entries] = _v_declfn; return [&]() -> std::tuple<> { 
  all_diagnostics = ast::diagnostics_append(all_diagnostics, where_clause_unknown_parameter_diagnostics(type_params, where_entries, ast::expr_span(method_body)));
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> method_env = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
  int pi = 0;
  while (pi < params.size()){
{
method_env.set(ast::param_name(params[pi]), registry::type_from_annotation(ast::param_typ(params[pi])));
pi = pi + 1;
}
}
  check_context::CheckContext extend_context = check_context::CheckContext{method_env, registry, extend_type_name};
  infer_result::InferResult method_result = infer::infer_expr(method_body, extend_context);
  all_diagnostics = ast::diagnostics_append(all_diagnostics, method_result.errors);
  return std::make_tuple();
 }(); } return std::make_tuple(); }();
method_index = method_index + 1;
}
}
  return std::make_tuple();
 }(); },
  [&](const DeclImport& declimport) -> std::tuple<> { auto [_w0, _w1] = declimport; return std::make_tuple(); },
  [&](const DeclExported& declexported) -> std::tuple<> { auto [_w0] = declexported; return std::make_tuple(); },
  [&](const DeclAssocType& declassoctype) -> std::tuple<> { auto [_w0, _w1] = declassoctype; return std::make_tuple(); },
  [&](const DeclAssocBind& declassocbind) -> std::tuple<> { auto [_w0, _w1, _w2] = declassocbind; return std::make_tuple(); }
}, (*ast::decl_inner(expanded_entry_program.decls[declaration_index])));
declaration_index = declaration_index + 1;
}
}
return all_diagnostics.size() > 0 ? ast::Result<check::CheckOut, mlc::Array<mlc::String>>(ast::Err<mlc::Array<mlc::String>>(ast::diagnostics_to_strings(all_diagnostics))) : ast::Result<check::CheckOut, mlc::Array<mlc::String>>(ast::Ok<check::CheckOut>(check::CheckOut{{}, registry}));
}

ast::Result<check::CheckOut, mlc::Array<mlc::String>> check_with_context(ast::Program entry, ast::Program full) noexcept{return check_program_against_full(entry, full);}

ast::Result<check::CheckOut, mlc::Array<mlc::String>> check(ast::Program program) noexcept{return check_program_against_full(program, program);}

} // namespace check
