#define main mlc_user_main
#include "check.hpp"

namespace check {

bool param_defaults_in_tail(mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept{
auto optional_began = false;
auto i = 0;
while ((i < parameters.length())) {
if (parameters[i]->has_default) {
optional_began = true;
} else if (optional_began) {
return false;
}

}
i = (i + 1);
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
bool default_expr_mvp_ok(std::shared_ptr<ast::Expr> e) noexcept{
return std::visit(overloaded{[&](const ast::ExprInt& exprInt) { auto [__0, __1] = exprInt; return true; },
[&](const ast::ExprStr& exprStr) { auto [__0, __1] = exprStr; return true; },
[&](const ast::ExprBool& exprBool) { auto [__0, __1] = exprBool; return true; },
[&](const ast::ExprUnit& exprUnit) { auto [__0] = exprUnit; return true; },
[&](const auto& __v) { return false; }
}, (*e));
}
bool is_extern_body(std::shared_ptr<ast::Expr> e) noexcept{
return std::visit(overloaded{[&](const ast::ExprExtern& exprExtern) { auto [__0] = exprExtern; return true; },
[&](const auto& __v) { return false; }
}, (*e));
}
mlc::Array<ast::Diagnostic> param_default_diagnostics(mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<ast::Expr> body, registry::TypeRegistry registry) noexcept{
auto out = mlc::Array<ast::Diagnostic>{};
if ((!param_defaults_in_tail(parameters))) {
out.push_back(ast::diagnostic_error(mlc::String("parameter defaults must be trailing", 35), ast::expr_span(body)));
}
auto has_any_default = false;
auto j = 0;
while ((j < parameters.length())) {
if (parameters[j]->has_default) {
has_any_default = true;
}
j = (j + 1);
}
if (((type_parameters.length() > 0) && has_any_default)) {
out.push_back(ast::diagnostic_error(mlc::String("default parameters are not allowed on generic functions yet", 59), ast::expr_span(body)));
}
if ((is_extern_body(body) && has_any_default)) {
out.push_back(ast::diagnostic_error(mlc::String("default parameters are not allowed on extern functions", 54), ast::expr_span(body)));
}
auto empty_type_environment = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
auto default_infer_context = check_context::check_context_new(empty_type_environment, registry);
auto p = 0;
while ((p < parameters.length())) {
if (parameters[p]->has_default) {
auto param_default_expr = parameters[p]->default_;
if ((!default_expr_mvp_ok(param_default_expr))) {
out.push_back(ast::diagnostic_error(mlc::String("parameter default must be a literal in this version", 51), ast::expr_span(param_default_expr)));
} else {
auto default_infer = infer::infer_expr(param_default_expr, default_infer_context);
out = ast::diagnostics_append(out, default_infer.errors);
auto param_expected_type = registry::type_from_annotation(ast::param_typ(parameters[p]));
if (((type_is_checkable(param_expected_type, registry) && type_is_checkable(default_infer.inferred_type, registry)) && (!semantic_type_structure::types_structurally_equal(param_expected_type, default_infer.inferred_type)))) {
out.push_back(ast::diagnostic_error((((mlc::String("parameter default: expected ", 28) + semantic_type_structure::type_description(param_expected_type)) + mlc::String(", got ", 6)) + semantic_type_structure::type_description(default_infer.inferred_type)), ast::expr_span(param_default_expr)));
}
}
}
p = (p + 1);
}
return out;
}
mlc::HashMap<mlc::String, bool> collect_globals(ast::Program program) noexcept{
auto names = mlc::HashMap<mlc::String, bool>();
names.set(mlc::String("true", 4), true);
names.set(mlc::String("false", 5), true);
names.set(mlc::String("exit", 4), true);
names.set(mlc::String("print", 5), true);
names.set(mlc::String("println", 7), true);
names.set(mlc::String("args", 4), true);
names.set(mlc::String("File", 4), true);
names.set(mlc::String("Shared", 6), true);
names.set(mlc::String("Map", 3), true);
names.set(mlc::String("Ok", 2), true);
names.set(mlc::String("Err", 3), true);
names.set(mlc::String("Result", 6), true);
auto index = 0;
while ((index < program.decls.length())) {
std::visit(overloaded{[&](const ast::DeclFn& declFn) { auto [name, __1, __2, __3, __4, __5] = declFn; return [&]() {
names.set(name, true);
/* unit */;
return /* unit */;
}(); },
[&](const ast::DeclType& declType) { auto [name, __1, variants, __3] = declType; return [&]() {
names.set(name, true);
auto variant_index = 0;
while ((variant_index < variants.length())) {
std::visit(overloaded{[&](const ast::VarUnit& varUnit) { auto [variant_name] = varUnit; return [&]() {
names.set(variant_name, true);
/* unit */;
return /* unit */;
}(); },
[&](const ast::VarTuple& varTuple) { auto [variant_name, __1] = varTuple; return [&]() {
names.set(variant_name, true);
/* unit */;
return /* unit */;
}(); },
[&](const ast::VarRecord& varRecord) { auto [variant_name, __1] = varRecord; return [&]() {
names.set(variant_name, true);
/* unit */;
return /* unit */;
}(); }
}, (*variants[variant_index]));
variant_index = (variant_index + 1);
}
/* unit */;
return /* unit */;
}(); },
[&](const ast::DeclTrait& declTrait) { auto [name, __1, methods] = declTrait; return [&]() {
names.set(name, true);
auto method_index = 0;
while ((method_index < methods.length())) {
std::visit(overloaded{[&](const ast::DeclFn& declFn) { auto [function_name, __1, __2, __3, __4, __5] = declFn; return [&]() {
names.set(function_name, true);
/* unit */;
return /* unit */;
}(); },
[&](const auto& __v) { return [&]() {
/* unit */;
return /* unit */;
}(); }
}, (*methods[method_index]));
method_index = (method_index + 1);
}
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
[&](const ast::DeclExported& declExported) { auto [__0] = declExported; return [&]() {
/* unit */;
return /* unit */;
}(); }
}, (*ast::decl_inner(program.decls[index])));
index = (index + 1);
}
return names;
}
bool type_is_checkable(std::shared_ptr<registry::Type> type_value, registry::TypeRegistry registry) noexcept{
return std::visit(overloaded{[&](const registry::TI32& tI32) { return true; },
[&](const registry::TString& tString) { return true; },
[&](const registry::TBool& tBool) { return true; },
[&](const registry::TUnit& tUnit) { return true; },
[&](const registry::TArray& tArray) { auto [inner] = tArray; return type_is_checkable(inner, registry); },
[&](const registry::TPair& tPair) { auto [a, b] = tPair; return (type_is_checkable(a, registry) && type_is_checkable(b, registry)); },
[&](const registry::TTuple& tTuple) { auto [ts] = tTuple; return [&]() {
auto ok = true;
auto ti = 0;
while (((ti < ts.length()) && ok)) {
ok = type_is_checkable(ts[ti], registry);
ti = (ti + 1);
}
return ok;
}(); },
[&](const registry::TGeneric& tGeneric) { auto [__0, args] = tGeneric; return [&]() {
auto ok = true;
auto i = 0;
while (((i < mlc::io::args.length()) && ok)) {
ok = type_is_checkable(mlc::io::args[i], registry);
i = (i + 1);
}
return ok;
}(); },
[&](const registry::TFn& tFn) { auto [__0, __1] = tFn; return true; },
[&](const registry::TShared& tShared) { auto [__0] = tShared; return true; },
[&](const registry::TNamed& tNamed) { auto [name] = tNamed; return registry::TypeRegistry_has_fields(registry, name); },
[&](const auto& __v) { return false; }
}, (*type_value));
}
bool CheckOut_has_errors(CheckOut self) noexcept{
return (self.errors.length() > 0);
}
ast::Result<CheckOut, mlc::Array<mlc::String>> check_program_against_full(ast::Program entry, ast::Program full_program) noexcept{
auto globals = collect_globals(full_program);
auto registry = registry::build_registry(full_program);
auto all_diagnostics = mlc::Array<ast::Diagnostic>{};
auto declaration_index = 0;
while ((declaration_index < entry.decls.length())) {
std::visit(overloaded{[&](const ast::DeclFn& declFn) { auto [__0, type_parameters, __2, parameters, return_type_annotation, body] = declFn; return [&]() {
all_diagnostics = ast::diagnostics_append(all_diagnostics, param_default_diagnostics(type_parameters, parameters, body, registry));
auto locals = mlc::Array<mlc::String>{};
auto type_parameter_index = 0;
while ((type_parameter_index < type_parameters.length())) {
locals.push_back(type_parameters[type_parameter_index]);
globals.set(type_parameters[type_parameter_index], true);
type_parameter_index = (type_parameter_index + 1);
}
auto type_environment = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
auto parameter_index = 0;
while ((parameter_index < parameters.length())) {
locals.push_back(ast::param_name(parameters[parameter_index]));
type_environment.set(ast::param_name(parameters[parameter_index]), registry::type_from_annotation(ast::param_typ(parameters[parameter_index])));
parameter_index = (parameter_index + 1);
}
all_diagnostics = ast::diagnostics_append(all_diagnostics, names::check_names_expr(body, locals, globals));
all_diagnostics = ast::diagnostics_append(all_diagnostics, check_mutations::check_fn_body_mutations(parameters, body));
auto inference_context = check_context::check_context_new(type_environment, registry);
auto inference_result = infer::infer_expr(body, inference_context);
auto expected_type = registry::type_from_annotation(return_type_annotation);
auto actual_type = inference_result.inferred_type;
auto return_type_errors = (((type_is_checkable(expected_type, registry) && type_is_checkable(actual_type, registry)) && (!semantic_type_structure::types_structurally_equal(expected_type, actual_type))) ? mlc::Array{ast::diagnostic_error((((mlc::String("return type: expected ", 22) + semantic_type_structure::type_description(expected_type)) + mlc::String(", got ", 6)) + semantic_type_structure::type_description(actual_type)), ast::expr_span(body))} : {});
all_diagnostics = ast::diagnostics_append(ast::diagnostics_append(all_diagnostics, inference_result.errors), return_type_errors);
/* unit */;
return /* unit */;
}(); },
[&](const ast::DeclType& declType) { auto [__0, __1, __2, __3] = declType; return [&]() {
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
[&](const ast::DeclExported& declExported) { auto [__0] = declExported; return [&]() {
/* unit */;
return /* unit */;
}(); }
}, (*ast::decl_inner(entry.decls[declaration_index])));
declaration_index = (declaration_index + 1);
}
if ((all_diagnostics.length() > 0)) {
return ast::Err{ast::diagnostics_to_strings(all_diagnostics)};
} else {
return ast::Ok{CheckOut{{}, registry}};
}
}
ast::Result<CheckOut, mlc::Array<mlc::String>> check_with_context(ast::Program entry, ast::Program full) noexcept{
return check_program_against_full(entry, full);
}
ast::Result<CheckOut, mlc::Array<mlc::String>> check(ast::Program program) noexcept{
return check_program_against_full(program, program);
}

} // namespace check
