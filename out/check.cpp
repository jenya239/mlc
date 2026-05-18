#define main mlc_user_main
#include "check.hpp"

namespace check {

bool param_defaults_in_tail(mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept{
auto optional_began = false;
auto i = 0;
while ((i < parameters.length())) {
auto has_default_now = parameters[i]->has_default;
if (((!has_default_now) && optional_began)) {
return false;
}
if (has_default_now) {
optional_began = true;
}
i = (i + 1);
}
return true;
}
bool default_expr_mvp_ok(std::shared_ptr<ast::Expr> e) noexcept{
return std::visit(overloaded{[&](const ast::ExprInt& exprInt) { auto [__0, __1] = exprInt; return true; },
[&](const ast::ExprStr& exprStr) { auto [__0, __1] = exprStr; return true; },
[&](const ast::ExprBool& exprBool) { auto [__0, __1] = exprBool; return true; },
[&](const ast::ExprUnit& exprUnit) { auto [__0] = exprUnit; return true; },
[&](const ast::ExprFloat& exprFloat) { auto [__0, __1] = exprFloat; return true; },
[&](const ast::ExprI64& exprI64) { auto [__0, __1] = exprI64; return true; },
[&](const ast::ExprU8& exprU8) { auto [__0, __1] = exprU8; return true; },
[&](const ast::ExprUsize& exprUsize) { auto [__0, __1] = exprUsize; return true; },
[&](const ast::ExprChar& exprChar) { auto [__0, __1] = exprChar; return true; },
[&](const auto& __v) { return false; }
}, (*e));
}
mlc::Array<ast::Diagnostic> accumulate_diagnostics_when_record_fields_have_defaults_under_generic_type_parameters(mlc::Array<ast::Diagnostic> diagnostics_accumulator_so_far, std::shared_ptr<ast::TypeVariant> variant_under_generic_scan) noexcept{
return std::visit(overloaded{[&](const ast::VarRecord& varRecord) { auto [__0, field_definitions_under_variant, __2] = varRecord; return field_definitions_under_variant.fold(diagnostics_accumulator_so_far, [=](mlc::Array<ast::Diagnostic> diagnostic_list_so_far_under_record, std::shared_ptr<ast::FieldDef> record_field_definition_under_scan) mutable { return [&]() {
if (record_field_definition_under_scan->has_default_expression) {
diagnostic_list_so_far_under_record.push_back(ast::diagnostic_error(mlc::String("record field default values are not supported when the type has generic parameters", 82), ast::span_unknown()));
}
return diagnostic_list_so_far_under_record;
}(); }); },
[&](const ast::VarTuple& varTuple) { auto [__0, __1, __2] = varTuple; return diagnostics_accumulator_so_far; },
[&](const ast::VarUnit& varUnit) { auto [__0, __1] = varUnit; return diagnostics_accumulator_so_far; }
}, (*variant_under_generic_scan));
}
mlc::Array<ast::Diagnostic> accumulate_diagnostics_after_literal_record_default_checks(mlc::Array<ast::Diagnostic> diagnostics_accumulator_so_far, std::shared_ptr<ast::TypeVariant> variant_under_literal_scan, check_context::CheckContext record_default_inference_context, registry::TypeRegistry registry) noexcept{
return std::visit(overloaded{[&](const ast::VarRecord& varRecord) { auto [__0, field_definitions_under_literal_scan, __2] = varRecord; return field_definitions_under_literal_scan.fold(diagnostics_accumulator_so_far, [=](mlc::Array<ast::Diagnostic> diagnostic_list_so_far_under_fields, std::shared_ptr<ast::FieldDef> record_field_under_literal_definition) mutable { return ((!record_field_under_literal_definition->has_default_expression) ? (diagnostic_list_so_far_under_fields) : ([&]() {
auto default_ast_expression = record_field_under_literal_definition->default_expression;
auto diagnostics_after_field = diagnostic_list_so_far_under_fields;
return ((!record_field_default_initializer::record_field_default_expression_acceptable_for_codegen(default_ast_expression)) ? ([&]() {
diagnostics_after_field.push_back(ast::diagnostic_error(mlc::String("record field default expression cannot be lowered to a C++ member initializer (literals, identifiers, + - * / %)", 112), ast::expr_span(default_ast_expression)));
return diagnostics_after_field;
}()) : ([&]() {
auto inferred_default_result = infer::infer_expr(default_ast_expression, record_default_inference_context);
diagnostics_after_field = ast::diagnostics_append(diagnostics_after_field, inferred_default_result.errors);
auto annotated_field_type = registry::type_from_annotation(record_field_under_literal_definition->typ);
if (((type_is_checkable(annotated_field_type, registry) && type_is_checkable(inferred_default_result.inferred_type, registry)) && (!semantic_type_structure::types_structurally_equal(annotated_field_type, inferred_default_result.inferred_type)))) {
diagnostics_after_field.push_back(ast::diagnostic_error(((mlc::String("record field default type mismatch for field \"", 46) + mlc::to_string(record_field_under_literal_definition->name)) + mlc::String("\"", 1)), ast::expr_span(default_ast_expression)));
}
return diagnostics_after_field;
}()));
}())); }); },
[&](const ast::VarTuple& varTuple) { auto [__0, __1, __2] = varTuple; return diagnostics_accumulator_so_far; },
[&](const ast::VarUnit& varUnit) { auto [__0, __1] = varUnit; return diagnostics_accumulator_so_far; }
}, (*variant_under_literal_scan));
}
mlc::Array<ast::Diagnostic> record_field_default_value_diagnostics(mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, registry::TypeRegistry registry) noexcept{
auto diagnostics_accumulator = mlc::Array<ast::Diagnostic>{};
if ((type_parameters.length() > 0)) {
diagnostics_accumulator = variants.fold(diagnostics_accumulator, diagnostics_so_far_across_variants);
/* unit */;
accumulate_diagnostics_when_record_fields_have_defaults_under_generic_type_parameters(diagnostics_so_far_across_variants, variant_shared_under_generic_pass);
/* unit */;
}
auto empty_record_default_environment = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
auto record_default_inference_context = check_context::check_context_new(empty_record_default_environment, registry);
diagnostics_accumulator = variants.fold(diagnostics_accumulator, diagnostics_so_far_across_variants);
/* unit */;
accumulate_diagnostics_after_literal_record_default_checks(diagnostics_so_far_across_variants, variant_shared_under_literal_pass, record_default_inference_context, registry);
/* unit */;
return diagnostics_accumulator;
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
auto has_any_default = parameters.any([=](std::shared_ptr<ast::Param> parameter_under_signature) mutable { return parameter_under_signature->has_default; });
if (((type_parameters.length() > 0) && has_any_default)) {
out.push_back(ast::diagnostic_error(mlc::String("default parameters are not allowed on generic functions yet", 59), ast::expr_span(body)));
}
if ((is_extern_body(body) && has_any_default)) {
out.push_back(ast::diagnostic_error(mlc::String("default parameters are not allowed on extern functions", 54), ast::expr_span(body)));
}
auto empty_type_environment = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
auto default_infer_context = check_context::check_context_new(empty_type_environment, registry);
return parameters.fold(out, [=](mlc::Array<ast::Diagnostic> diagnostic_list_so_far_under_parameters, std::shared_ptr<ast::Param> parameter_under_scan_for_defaults) mutable { return [&]() {
auto diagnostics_after_parameter = diagnostic_list_so_far_under_parameters;
if ((parameter_under_scan_for_defaults->has_default && (!param_destructure_expand::parameter_binding_is_plain_identifier(parameter_under_scan_for_defaults)))) {
diagnostics_after_parameter.push_back(ast::diagnostic_error(mlc::String("default values are not supported for destructuring parameters", 61), ast::expr_span(body)));
}
if (parameter_under_scan_for_defaults->has_default) {
auto param_default_expression = parameter_under_scan_for_defaults->default_;
if ((!default_expr_mvp_ok(param_default_expression))) {
diagnostics_after_parameter.push_back(ast::diagnostic_error(mlc::String("parameter default must be a literal in this version", 51), ast::expr_span(param_default_expression)));
} else {
auto default_infer = infer::infer_expr(param_default_expression, default_infer_context);
diagnostics_after_parameter = ast::diagnostics_append(diagnostics_after_parameter, default_infer.errors);
auto parameter_expected_type = registry::type_from_annotation(ast::param_typ(parameter_under_scan_for_defaults));
if (((type_is_checkable(parameter_expected_type, registry) && type_is_checkable(default_infer.inferred_type, registry)) && (!semantic_type_structure::types_structurally_equal(parameter_expected_type, default_infer.inferred_type)))) {
diagnostics_after_parameter.push_back(ast::diagnostic_error((((mlc::String("parameter default: expected ", 28) + semantic_type_structure::type_description(parameter_expected_type)) + mlc::String(", got ", 6)) + semantic_type_structure::type_description(default_infer.inferred_type)), ast::expr_span(param_default_expression)));
}
}
}
return diagnostics_after_parameter;
}(); });
}
bool type_parameter_name_known(mlc::Array<mlc::String> type_parameter_names, mlc::String candidate_name) noexcept{
return type_parameter_names.any([=](mlc::String type_parameter_name_under_scan) mutable { return (type_parameter_name_under_scan == candidate_name); });
}
mlc::Array<ast::Diagnostic> where_clause_unknown_parameter_diagnostics(mlc::Array<mlc::String> type_parameter_names, mlc::Array<ast::WhereClauseBound> where_entries, ast::Span source_span) noexcept{
auto initial_where_clause_diagnostics = mlc::Array<ast::Diagnostic>{};
return where_entries.fold(initial_where_clause_diagnostics, [=](mlc::Array<ast::Diagnostic> diagnostics_accumulated_so_far, ast::WhereClauseBound where_entry_under_scan) mutable { return (type_parameter_name_known(type_parameter_names, where_entry_under_scan.parameter_name) ? (diagnostics_accumulated_so_far) : ([&]() {
diagnostics_accumulated_so_far.push_back(ast::diagnostic_error(((mlc::String("where clause names unknown type parameter \"", 43) + mlc::to_string(where_entry_under_scan.parameter_name)) + mlc::String("\"", 1)), source_span));
return diagnostics_accumulated_so_far;
}())); });
}
mlc::HashMap<mlc::String, bool> global_names_merge_type_constructor_variant_into_map(mlc::HashMap<mlc::String, bool> global_names_so_far, std::shared_ptr<ast::TypeVariant> type_constructor_variant_under_scan) noexcept{
return std::visit(overloaded{[&](const ast::VarUnit& varUnit) { auto [variant_name, __1] = varUnit; return [&]() {
global_names_so_far.set(variant_name, true);
return global_names_so_far;
}(); },
[&](const ast::VarTuple& varTuple) { auto [variant_name, __1, __2] = varTuple; return [&]() {
global_names_so_far.set(variant_name, true);
return global_names_so_far;
}(); },
[&](const ast::VarRecord& varRecord) { auto [variant_name, __1, __2] = varRecord; return [&]() {
global_names_so_far.set(variant_name, true);
return global_names_so_far;
}(); }
}, (*type_constructor_variant_under_scan));
}
mlc::HashMap<mlc::String, bool> global_names_merge_trait_method_into_map(mlc::HashMap<mlc::String, bool> global_names_so_far, std::shared_ptr<ast::Decl> method_declaration_under_scan) noexcept{
return std::visit(overloaded{[&](const ast::DeclFn& declFn) { auto [function_name, __1, __2, __3, __4, __5, __6] = declFn; return [&]() {
global_names_so_far.set(function_name, true);
return global_names_so_far;
}(); },
[&](const auto& __v) { return global_names_so_far; }
}, (*method_declaration_under_scan));
}
mlc::HashMap<mlc::String, bool> global_names_merge_single_declaration_into_map(mlc::HashMap<mlc::String, bool> global_names_so_far, std::shared_ptr<ast::Decl> declaration_under_scan) noexcept{
return std::visit(overloaded{[&](const ast::DeclFn& declFn) { auto [name, __1, __2, __3, __4, __5, __6] = declFn; return [&]() {
global_names_so_far.set(name, true);
return global_names_so_far;
}(); },
[&](const ast::DeclType& declType) { auto [name, __1, variants, __3] = declType; return [&]() {
global_names_so_far.set(name, true);
return variants.fold(global_names_so_far, [=](mlc::HashMap<mlc::String, bool> names_after_type_definition, std::shared_ptr<ast::TypeVariant> variant_shared_under_scan) mutable { return global_names_merge_type_constructor_variant_into_map(names_after_type_definition, variant_shared_under_scan); });
}(); },
[&](const ast::DeclTrait& declTrait) { auto [name, __1, methods] = declTrait; return [&]() {
global_names_so_far.set(name, true);
return methods.fold(global_names_so_far, [=](mlc::HashMap<mlc::String, bool> names_after_trait_definition, std::shared_ptr<ast::Decl> method_declaration_shared_under_scan) mutable { return global_names_merge_trait_method_into_map(names_after_trait_definition, method_declaration_shared_under_scan); });
}(); },
[&](const ast::DeclExtend& declExtend) { auto [__0, __1, __2] = declExtend; return global_names_so_far; },
[&](const ast::DeclImport& declImport) { auto [__0, __1] = declImport; return global_names_so_far; },
[&](const ast::DeclExported& declExported) { auto [__0] = declExported; return global_names_so_far; },
[&](const ast::DeclAssocType& declAssocType) { auto [__0, __1] = declAssocType; return global_names_so_far; },
[&](const ast::DeclAssocBind& declAssocBind) { auto [__0, __1, __2] = declAssocBind; return global_names_so_far; }
}, (*ast::decl_inner(declaration_under_scan)));
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
return program.decls.fold(names, [=](mlc::HashMap<mlc::String, bool> global_names_so_far, std::shared_ptr<ast::Decl> declaration_shared_under_program) mutable { return global_names_merge_single_declaration_into_map(global_names_so_far, declaration_shared_under_program); });
}
bool type_is_checkable(std::shared_ptr<registry::Type> type_value, registry::TypeRegistry registry) noexcept{
return std::visit(overloaded{[&](const registry::TI32& tI32) { return true; },
[&](const registry::TString& tString) { return true; },
[&](const registry::TBool& tBool) { return true; },
[&](const registry::TUnit& tUnit) { return true; },
[&](const registry::TI64& tI64) { return true; },
[&](const registry::TF64& tF64) { return true; },
[&](const registry::TU8& tU8) { return true; },
[&](const registry::TUsize& tUsize) { return true; },
[&](const registry::TChar& tChar) { return true; },
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
while (((i < args.length()) && ok)) {
ok = type_is_checkable(args[i], registry);
i = (i + 1);
}
return ok;
}(); },
[&](const registry::TFn& tFn) { auto [__0, __1] = tFn; return true; },
[&](const registry::TShared& tShared) { auto [__0] = tShared; return true; },
[&](const registry::TNamed& tNamed) { auto [name] = tNamed; return registry::TypeRegistry_has_fields(registry, name); },
[&](const registry::TAssoc& tAssoc) { auto [__0, __1] = tAssoc; return true; },
[&](const auto& __v) { return false; }
}, (*type_value));
}
bool CheckOut_has_errors(CheckOut self) noexcept{
return (self.errors.length() > 0);
}
mlc::Array<ast::Diagnostic> accumulate_diagnostics_for_single_extend_method(mlc::String extend_type_name, std::shared_ptr<ast::Decl> method_declaration_shared_under_scan, registry::TypeRegistry registry) noexcept{
return std::visit(overloaded{[&](const ast::DeclFn& declFn) { auto [__0, type_params, __2, params, __4, method_body, where_entries] = declFn; return [&]() {
auto method_related_diagnostics = where_clause_unknown_parameter_diagnostics(type_params, where_entries, ast::expr_span(method_body));
auto method_environment = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
auto parameter_index = 0;
while ((parameter_index < params.length())) {
method_environment.set(ast::param_name(params[parameter_index]), registry::type_from_annotation(ast::param_typ(params[parameter_index])));
parameter_index = (parameter_index + 1);
}
auto extend_context = check_context::CheckContext{method_environment, registry, extend_type_name};
auto method_inference_result = infer::infer_expr(method_body, extend_context);
return ast::diagnostics_append(method_related_diagnostics, method_inference_result.errors);
}(); },
[&](const auto& __v) { return [&]() {
auto empty_extend_method_diagnostic_branch = mlc::Array<ast::Diagnostic>{};
return empty_extend_method_diagnostic_branch;
}(); }
}, (*method_declaration_shared_under_scan));
}
ast::Result<CheckOut, mlc::Array<mlc::String>> check_program_against_full(ast::Program entry, ast::Program full_program) noexcept{
auto all_diagnostics = trait_param_expand::trait_and_type_name_conflict_diagnostics(full_program);
auto destructured_full_program = param_destructure_expand::expand_parameter_destructuring_in_program(full_program);
all_diagnostics = ast::diagnostics_append(all_diagnostics, param_destructure_expand::extern_parameter_destructure_diagnostics(full_program));
auto expanded_entry_program = param_destructure_expand::expand_parameter_destructuring_in_program(trait_param_expand::expand_trait_as_param_entry_using_full(entry, destructured_full_program));
auto globals = collect_globals(destructured_full_program);
auto registry = registry::build_registry(destructured_full_program);
auto declaration_index = 0;
while ((declaration_index < expanded_entry_program.decls.length())) {
std::visit(overloaded{[&](const ast::DeclFn& declFn) { auto [name, type_parameters, trait_bounds, parameters, return_type_annotation, body, where_clause_bounds_entries] = declFn; return [&]() {
all_diagnostics = ast::diagnostics_append(all_diagnostics, where_clause_unknown_parameter_diagnostics(type_parameters, where_clause_bounds_entries, ast::expr_span(body)));
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
auto body_partial_application = partial_application_desugar::partial_application_desugar_expr(body);
all_diagnostics = ast::diagnostics_append(all_diagnostics, names::check_names_expr(body_partial_application, locals, globals));
all_diagnostics = ast::diagnostics_append(all_diagnostics, check_mutations::check_fn_body_mutations(parameters, body_partial_application));
auto inference_context = check_context::check_context_new(type_environment, registry);
auto inference_result = infer::infer_expr(body_partial_application, inference_context);
auto expected_type = registry::type_from_annotation(return_type_annotation);
auto actual_type = inference_result.inferred_type;
auto return_type_errors = (((type_is_checkable(expected_type, registry) && type_is_checkable(actual_type, registry)) && (!semantic_type_structure::types_structurally_equal(expected_type, actual_type))) ? (mlc::Array<ast::Diagnostic>{ast::diagnostic_error((((mlc::String("return type: expected ", 22) + semantic_type_structure::type_description(expected_type)) + mlc::String(", got ", 6)) + semantic_type_structure::type_description(actual_type)), ast::expr_span(body))}) : (mlc::Array<ast::Diagnostic>{}));
all_diagnostics = ast::diagnostics_append(ast::diagnostics_append(all_diagnostics, inference_result.errors), return_type_errors);
/* unit */;
return /* unit */;
}(); },
[&](const ast::DeclType& declType) { auto [__0, type_parameters, variants, derive_trait_names] = declType; return [&]() {
all_diagnostics = ast::diagnostics_append(all_diagnostics, derive_validation::derive_clause_diagnostics(type_parameters, variants, derive_trait_names, ast::span_unknown()));
all_diagnostics = ast::diagnostics_append(all_diagnostics, record_field_default_value_diagnostics(type_parameters, variants, registry));
/* unit */;
return /* unit */;
}(); },
[&](const ast::DeclTrait& declTrait) { auto [__0, __1, __2] = declTrait; return [&]() {
/* unit */;
return /* unit */;
}(); },
[&](const ast::DeclExtend& declExtend) { auto [extend_type_name, __1, methods] = declExtend; return [&]() {
all_diagnostics = methods.fold(all_diagnostics, [=](mlc::Array<ast::Diagnostic> diagnostics_so_far_across_extend_methods, std::shared_ptr<ast::Decl> method_shared_under_extend) mutable { return ast::diagnostics_append(diagnostics_so_far_across_extend_methods, accumulate_diagnostics_for_single_extend_method(extend_type_name, method_shared_under_extend, registry)); });
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
}(); },
[&](const ast::DeclAssocType& declAssocType) { auto [__0, __1] = declAssocType; return [&]() {
/* unit */;
return /* unit */;
}(); },
[&](const ast::DeclAssocBind& declAssocBind) { auto [__0, __1, __2] = declAssocBind; return [&]() {
/* unit */;
return /* unit */;
}(); }
}, (*ast::decl_inner(expanded_entry_program.decls[declaration_index])));
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
