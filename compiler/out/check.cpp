#define main mlc_user_main
#include "check.hpp"

#include "ast.hpp"
#include "ast.hpp"
#include "names.hpp"
#include "check_mutations.hpp"
#include "registry.hpp"
#include "trait_param_expand.hpp"
#include "param_destructure_expand.hpp"
#include "derive_validation.hpp"
#include "infer.hpp"
#include "check_context.hpp"
#include "semantic_type_structure.hpp"
#include "diagnostic_codes.hpp"
#include "record_field_default_validate.hpp"
#include "partial_application_desugar.hpp"
#include "cycle_lint.hpp"

namespace check {

using namespace ast;
using namespace ast;
using namespace names;
using namespace check_mutations;
using namespace registry;
using namespace trait_param_expand;
using namespace param_destructure_expand;
using namespace derive_validation;
using namespace infer;
using namespace check_context;
using namespace semantic_type_structure;
using namespace diagnostic_codes;
using namespace record_field_default_validate;
using namespace partial_application_desugar;
using namespace cycle_lint;
using namespace ast_tokens;

bool CheckOut_has_errors(CheckOut self) noexcept;
bool param_defaults_in_tail(mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept{
  auto optional_began = false;
  auto parameter_index = 0;
  while ((parameter_index < parameters.length()))   {
    auto has_default_now = parameters[parameter_index]->has_default;
    if (((!has_default_now) && optional_began))     {
      return false;
    }
    if (has_default_now)     {
      (optional_began = true);
    }
    (parameter_index = (parameter_index + 1));
  }
  return true;
}
bool default_expr_mvp_ok(std::shared_ptr<ast::Expr> expression) noexcept{
  return [&]() -> bool {
auto __match_subject = expression;
if (std::holds_alternative<ast::ExprInt>((*__match_subject))) {
const ast::ExprInt& exprInt = std::get<ast::ExprInt>((*__match_subject));
auto [__0, __1] = exprInt; return true;
}
if (std::holds_alternative<ast::ExprStr>((*__match_subject))) {
const ast::ExprStr& exprStr = std::get<ast::ExprStr>((*__match_subject));
auto [__0, __1] = exprStr; return true;
}
if (std::holds_alternative<ast::ExprBool>((*__match_subject))) {
const ast::ExprBool& exprBool = std::get<ast::ExprBool>((*__match_subject));
auto [__0, __1] = exprBool; return true;
}
if (std::holds_alternative<ast::ExprUnit>((*__match_subject))) {
const ast::ExprUnit& exprUnit = std::get<ast::ExprUnit>((*__match_subject));
auto [__0] = exprUnit; return true;
}
if (std::holds_alternative<ast::ExprFloat>((*__match_subject))) {
const ast::ExprFloat& exprFloat = std::get<ast::ExprFloat>((*__match_subject));
auto [__0, __1] = exprFloat; return true;
}
if (std::holds_alternative<ast::ExprI64>((*__match_subject))) {
const ast::ExprI64& exprI64 = std::get<ast::ExprI64>((*__match_subject));
auto [__0, __1] = exprI64; return true;
}
if (std::holds_alternative<ast::ExprU8>((*__match_subject))) {
const ast::ExprU8& exprU8 = std::get<ast::ExprU8>((*__match_subject));
auto [__0, __1] = exprU8; return true;
}
if (std::holds_alternative<ast::ExprUsize>((*__match_subject))) {
const ast::ExprUsize& exprUsize = std::get<ast::ExprUsize>((*__match_subject));
auto [__0, __1] = exprUsize; return true;
}
if (std::holds_alternative<ast::ExprChar>((*__match_subject))) {
const ast::ExprChar& exprChar = std::get<ast::ExprChar>((*__match_subject));
auto [__0, __1] = exprChar; return true;
}
return false;
std::abort();
}();
}
mlc::Array<ast::Diagnostic> accumulate_diagnostics_when_record_fields_have_defaults_under_generic_type_parameters(mlc::Array<ast::Diagnostic> diagnostics_accumulator_so_far, std::shared_ptr<ast::TypeVariant> variant_under_generic_scan) noexcept{
  return std::visit(overloaded{[&](const ast::VarRecord& varRecord) { auto [__0, field_definitions_under_variant, __2] = varRecord; return field_definitions_under_variant.fold(diagnostics_accumulator_so_far, [=](mlc::Array<ast::Diagnostic> diagnostic_list_so_far_under_record, std::shared_ptr<ast::FieldDef> record_field_definition_under_scan) mutable { return (record_field_definition_under_scan->has_default_expression ? (diagnostic_list_so_far_under_record.concat(mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("record field default values are not supported when the type has generic parameters", 82), ast::expr_span(record_field_definition_under_scan->default_expression), diagnostic_codes::diagnostic_code_e042())})) : (diagnostic_list_so_far_under_record)); }); },
[&](const ast::VarTuple& varTuple) { auto [__0, __1, __2] = varTuple; return diagnostics_accumulator_so_far; },
[&](const ast::VarUnit& varUnit) { auto [__0, __1] = varUnit; return diagnostics_accumulator_so_far; }
}, (*variant_under_generic_scan));
}
mlc::Array<ast::Diagnostic> accumulate_diagnostics_after_literal_record_default_checks(mlc::Array<ast::Diagnostic> diagnostics_accumulator_so_far, std::shared_ptr<ast::TypeVariant> variant_under_literal_scan, check_context::CheckContext record_default_inference_context, registry::TypeRegistry registry) noexcept{
  return std::visit(overloaded{[&](const ast::VarRecord& varRecord) { auto [__0, field_definitions_under_literal_scan, __2] = varRecord; return field_definitions_under_literal_scan.fold(diagnostics_accumulator_so_far, [=](mlc::Array<ast::Diagnostic> diagnostic_list_so_far_under_fields, std::shared_ptr<ast::FieldDef> record_field_under_literal_definition) mutable { return [&]() -> mlc::Array<ast::Diagnostic> {
  if ((!record_field_under_literal_definition->has_default_expression))   {
    return diagnostic_list_so_far_under_fields;
  } else   {
    return [&]() -> auto {
  if ((!record_field_default_validate::record_field_default_expression_is_static_initializer(record_field_under_literal_definition->default_expression)))   {
    return diagnostic_list_so_far_under_fields.concat(mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("record field default expression cannot be lowered to a C++ member initializer (literals, identifiers, + - * / %)", 112), ast::expr_span(record_field_under_literal_definition->default_expression), diagnostic_codes::diagnostic_code_e043())});
  } else   {
    return [&]() {
auto default_ast_expression = record_field_under_literal_definition->default_expression;
auto default_parsed = infer::infer_expr(default_ast_expression, record_default_inference_context);
auto with_infer_errors = ast::diagnostics_append(diagnostic_list_so_far_under_fields, default_parsed.errors);
auto annotated_field_type = registry::type_from_annotation_with_registry(record_field_under_literal_definition->type_value, registry);
return (((type_is_checkable(annotated_field_type, registry) && type_is_checkable(default_parsed.inferred_type, registry)) && (!semantic_type_structure::types_structurally_equal(annotated_field_type, default_parsed.inferred_type))) ? (with_infer_errors.concat(mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(((mlc::String("record field default type mismatch for field \"", 46) + record_field_under_literal_definition->name) + mlc::String("\"", 1)), ast::expr_span(default_ast_expression), diagnostic_codes::diagnostic_code_e044())})) : (with_infer_errors));
}();
  }
}();
  }
}(); }); },
[&](const ast::VarTuple& varTuple) { auto [__0, __1, __2] = varTuple; return diagnostics_accumulator_so_far; },
[&](const ast::VarUnit& varUnit) { auto [__0, __1] = varUnit; return diagnostics_accumulator_so_far; }
}, (*variant_under_literal_scan));
}
mlc::Array<ast::Diagnostic> record_field_default_value_diagnostics(mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, registry::TypeRegistry registry) noexcept{
  auto diagnostics_accumulator = mlc::Array<ast::Diagnostic>{};
  if ((type_parameters.length() > 0))   {
    (diagnostics_accumulator = variants.fold(diagnostics_accumulator, [=](mlc::Array<ast::Diagnostic> diagnostics_so_far_across_variants, std::shared_ptr<ast::TypeVariant> variant_shared_under_generic_pass) mutable { return accumulate_diagnostics_when_record_fields_have_defaults_under_generic_type_parameters(diagnostics_so_far_across_variants, variant_shared_under_generic_pass); }));
  }
  auto empty_record_default_environment = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
  auto record_default_inference_context = check_context::check_context_new(empty_record_default_environment, registry);
  (diagnostics_accumulator = variants.fold(diagnostics_accumulator, [=](mlc::Array<ast::Diagnostic> diagnostics_so_far_across_variants, std::shared_ptr<ast::TypeVariant> variant_shared_under_literal_pass) mutable { return accumulate_diagnostics_after_literal_record_default_checks(diagnostics_so_far_across_variants, variant_shared_under_literal_pass, record_default_inference_context, registry); }));
  return diagnostics_accumulator;
}
bool is_extern_body(std::shared_ptr<ast::Expr> expression) noexcept{
  return [&]() -> bool {
auto __match_subject = expression;
if (std::holds_alternative<ast::ExprExtern>((*__match_subject))) {
const ast::ExprExtern& exprExtern = std::get<ast::ExprExtern>((*__match_subject));
auto [__0] = exprExtern; return true;
}
return false;
std::abort();
}();
}
mlc::Array<ast::Diagnostic> param_default_diagnostics(mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<ast::Expr> body, registry::TypeRegistry registry) noexcept{
  auto trailing_default_diagnostics = [&]() -> mlc::Array<ast::Diagnostic> {
  if ((!param_defaults_in_tail(parameters)))   {
    return mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("parameter defaults must be trailing", 35), ast::expr_span(body), diagnostic_codes::diagnostic_code_e036())};
  } else   {
    return [&]() {
auto empty_diagnostics = mlc::Array<ast::Diagnostic>{};
return empty_diagnostics;
}();
  }
}();
  auto has_any_default = parameters.any([=](std::shared_ptr<ast::Param> parameter_under_signature) mutable { return parameter_under_signature->has_default; });
  auto generic_default_diagnostics = (((type_parameters.length() > 0) && has_any_default) ? (trailing_default_diagnostics.concat(mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("default parameters are not allowed on generic functions yet", 59), ast::expr_span(body), diagnostic_codes::diagnostic_code_e037())})) : (trailing_default_diagnostics));
  auto initial_default_diagnostics = ((is_extern_body(body) && has_any_default) ? (generic_default_diagnostics.concat(mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("default parameters are not allowed on extern functions", 54), ast::expr_span(body), diagnostic_codes::diagnostic_code_e038())})) : (generic_default_diagnostics));
  auto empty_type_environment = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
  auto default_infer_context = check_context::check_context_new(empty_type_environment, registry);
  return parameters.fold(initial_default_diagnostics, [=](auto diagnostic_list_so_far_under_parameters, std::shared_ptr<ast::Param> parameter_under_scan_for_defaults) mutable { return [&]() {
auto diagnostics_after_parameter = ((parameter_under_scan_for_defaults->has_default && (!param_destructure_expand::parameter_binding_is_plain_identifier(parameter_under_scan_for_defaults))) ? (diagnostic_list_so_far_under_parameters.concat(mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("default values are not supported for destructuring parameters", 61), ast::expr_span(body), diagnostic_codes::diagnostic_code_e039())})) : (diagnostic_list_so_far_under_parameters));
return [&]() -> auto {
  if (parameter_under_scan_for_defaults->has_default)   {
    return [&]() {
auto param_default_expression = parameter_under_scan_for_defaults->default_;
return [&]() -> auto {
  if ((!default_expr_mvp_ok(param_default_expression)))   {
    return diagnostics_after_parameter.concat(mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("parameter default must be a literal in this version", 51), ast::expr_span(param_default_expression), diagnostic_codes::diagnostic_code_e040())});
  } else   {
    return [&]() {
auto default_infer = infer::infer_expr(param_default_expression, default_infer_context);
auto with_infer_errors = ast::diagnostics_append(diagnostics_after_parameter, default_infer.errors);
auto parameter_expected_type = registry::type_from_annotation_with_registry(ast::param_type_value(parameter_under_scan_for_defaults), registry);
return (((type_is_checkable(parameter_expected_type, registry) && type_is_checkable(default_infer.inferred_type, registry)) && (!semantic_type_structure::types_structurally_equal(parameter_expected_type, default_infer.inferred_type))) ? (with_infer_errors.concat(mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code((((mlc::String("parameter default: expected ", 28) + semantic_type_structure::type_description(parameter_expected_type)) + mlc::String(", got ", 6)) + semantic_type_structure::type_description(default_infer.inferred_type)), ast::expr_span(param_default_expression), diagnostic_codes::diagnostic_code_e041())})) : (with_infer_errors));
}();
  }
}();
}();
  } else   {
    return diagnostics_after_parameter;
  }
}();
}(); });
}
bool type_parameter_name_known(mlc::Array<mlc::String> type_parameter_names, mlc::String candidate_name) noexcept{
  return type_parameter_names.any([=](mlc::String type_parameter_name_under_scan) mutable { return (type_parameter_name_under_scan == candidate_name); });
}
mlc::Array<ast::Diagnostic> where_clause_unknown_parameter_diagnostics(mlc::Array<mlc::String> type_parameter_names, mlc::Array<ast::WhereClauseBound> where_entries, ast::Span source_span) noexcept{
  auto initial_where_clause_diagnostics = mlc::Array<ast::Diagnostic>{};
  return where_entries.fold(initial_where_clause_diagnostics, [=](mlc::Array<ast::Diagnostic> diagnostics_accumulated_so_far, ast::WhereClauseBound where_entry_under_scan) mutable { return [&]() -> mlc::Array<ast::Diagnostic> {
  if (type_parameter_name_known(type_parameter_names, where_entry_under_scan.parameter_name))   {
    return diagnostics_accumulated_so_far;
  } else   {
    return diagnostics_accumulated_so_far.concat(mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(((mlc::String("where clause names unknown type parameter '", 43) + mlc::to_string(where_entry_under_scan.parameter_name)) + mlc::String("'", 1)), source_span, diagnostic_codes::diagnostic_code_e076())});
  }
}(); });
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
  return [&]() -> mlc::HashMap<mlc::String, bool> {
auto __match_subject = method_declaration_under_scan;
if (std::holds_alternative<ast::DeclFn>((*__match_subject))) {
const ast::DeclFn& declFn = std::get<ast::DeclFn>((*__match_subject));
auto [function_name, __1, __2, __3, __4, __5, __6] = declFn; return [&]() {
global_names_so_far.set(function_name, true);
return global_names_so_far;
}();
}
return global_names_so_far;
std::abort();
}();
}
mlc::HashMap<mlc::String, bool> global_names_merge_single_declaration_into_map(mlc::HashMap<mlc::String, bool> global_names_so_far, std::shared_ptr<ast::Decl> declaration_under_scan) noexcept{
  return std::visit(overloaded{[&](const ast::DeclFn& declFn) { auto [name, __1, __2, __3, __4, __5, __6] = declFn; return [&]() {
global_names_so_far.set(name, true);
return global_names_so_far;
}(); },
[&](const ast::DeclType& declType) { auto [name, __1, variants, __3, __4] = declType; return [&]() {
global_names_so_far.set(name, true);
return variants.fold(global_names_so_far, [=](mlc::HashMap<mlc::String, bool> names_after_type_definition, std::shared_ptr<ast::TypeVariant> variant_shared_under_scan) mutable { return global_names_merge_type_constructor_variant_into_map(names_after_type_definition, variant_shared_under_scan); });
}(); },
[&](const ast::DeclTypeAlias& declTypeAlias) { auto [name, __1, __2, __3] = declTypeAlias; return [&]() {
global_names_so_far.set(name, true);
return global_names_so_far;
}(); },
[&](const ast::DeclTrait& declTrait) { auto [name, __1, methods, __3] = declTrait; return [&]() {
global_names_so_far.set(name, true);
return methods.fold(global_names_so_far, [=](mlc::HashMap<mlc::String, bool> names_after_trait_definition, std::shared_ptr<ast::Decl> method_declaration_shared_under_scan) mutable { return global_names_merge_trait_method_into_map(names_after_trait_definition, method_declaration_shared_under_scan); });
}(); },
[&](const ast::DeclExtend& declExtend) { auto [__0, __1, __2, __3] = declExtend; return global_names_so_far; },
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
  names.set(mlc::String("read_line", 9), true);
  names.set(mlc::String("spawn_task", 10), true);
  names.set(mlc::String("make_channel", 12), true);
  names.set(mlc::String("Channel", 7), true);
  names.set(mlc::String("Arc", 3), true);
  names.set(mlc::String("Mutex", 5), true);
  names.set(mlc::String("File", 4), true);
  names.set(mlc::String("Profile", 7), true);
  names.set(mlc::String("Shared", 6), true);
  names.set(mlc::String("Weak", 4), true);
  names.set(mlc::String("Map", 3), true);
  names.set(mlc::String("Ok", 2), true);
  names.set(mlc::String("Err", 3), true);
  names.set(mlc::String("Result", 6), true);
  return program.decls.fold(names, [=](mlc::HashMap<mlc::String, bool> global_names_so_far, std::shared_ptr<ast::Decl> declaration_shared_under_program) mutable { return global_names_merge_single_declaration_into_map(global_names_so_far, declaration_shared_under_program); });
}
bool type_is_checkable(std::shared_ptr<registry::Type> type_value, registry::TypeRegistry type_registry) noexcept{
  return [&]() -> bool {
auto __match_subject = type_value;
if (std::holds_alternative<registry::TI32>((*__match_subject))) {
const registry::TI32& tI32 = std::get<registry::TI32>((*__match_subject));
return true;
}
if (std::holds_alternative<registry::TString>((*__match_subject))) {
const registry::TString& tString = std::get<registry::TString>((*__match_subject));
return true;
}
if (std::holds_alternative<registry::TBool>((*__match_subject))) {
const registry::TBool& tBool = std::get<registry::TBool>((*__match_subject));
return true;
}
if (std::holds_alternative<registry::TUnit>((*__match_subject))) {
const registry::TUnit& tUnit = std::get<registry::TUnit>((*__match_subject));
return true;
}
if (std::holds_alternative<registry::TI64>((*__match_subject))) {
const registry::TI64& tI64 = std::get<registry::TI64>((*__match_subject));
return true;
}
if (std::holds_alternative<registry::TF64>((*__match_subject))) {
const registry::TF64& tF64 = std::get<registry::TF64>((*__match_subject));
return true;
}
if (std::holds_alternative<registry::TU8>((*__match_subject))) {
const registry::TU8& tU8 = std::get<registry::TU8>((*__match_subject));
return true;
}
if (std::holds_alternative<registry::TUsize>((*__match_subject))) {
const registry::TUsize& tUsize = std::get<registry::TUsize>((*__match_subject));
return true;
}
if (std::holds_alternative<registry::TChar>((*__match_subject))) {
const registry::TChar& tChar = std::get<registry::TChar>((*__match_subject));
return true;
}
if (std::holds_alternative<registry::TArray>((*__match_subject))) {
const registry::TArray& tArray = std::get<registry::TArray>((*__match_subject));
auto [inner] = tArray; return type_is_checkable(inner, type_registry);
}
if (std::holds_alternative<registry::TPair>((*__match_subject))) {
const registry::TPair& tPair = std::get<registry::TPair>((*__match_subject));
auto [a, b] = tPair; return (type_is_checkable(a, type_registry) && type_is_checkable(b, type_registry));
}
if (std::holds_alternative<registry::TTuple>((*__match_subject))) {
const registry::TTuple& tTuple = std::get<registry::TTuple>((*__match_subject));
auto [type_parameters] = tTuple; return type_parameters.all([=](std::shared_ptr<registry::Type> type_parameter) mutable { return type_is_checkable(type_parameter, type_registry); });
}
if (std::holds_alternative<registry::TGeneric>((*__match_subject))) {
const registry::TGeneric& tGeneric = std::get<registry::TGeneric>((*__match_subject));
auto [__0, type_arguments] = tGeneric; return type_arguments.all([=](std::shared_ptr<registry::Type> type_argument) mutable { return type_is_checkable(type_argument, type_registry); });
}
if (std::holds_alternative<registry::TFn>((*__match_subject))) {
const registry::TFn& tFn = std::get<registry::TFn>((*__match_subject));
auto [__0, __1] = tFn; return true;
}
if (std::holds_alternative<registry::TShared>((*__match_subject))) {
const registry::TShared& tShared = std::get<registry::TShared>((*__match_subject));
auto [__0] = tShared; return true;
}
if (std::holds_alternative<registry::TNamed>((*__match_subject))) {
const registry::TNamed& tNamed = std::get<registry::TNamed>((*__match_subject));
auto [name] = tNamed; return type_registry.record_index.field_types.has(registry::resolve_nominal_type_name_for_fields(type_registry, name, mlc::Array<mlc::String>{}));
}
if (std::holds_alternative<registry::TAssoc>((*__match_subject))) {
const registry::TAssoc& tAssoc = std::get<registry::TAssoc>((*__match_subject));
auto [__0, __1] = tAssoc; return true;
}
return false;
std::abort();
}();
}
bool CheckOut_has_errors(CheckOut self) noexcept;
bool CheckOut_has_errors(CheckOut self) noexcept{
return (self.errors.length() > 0);
}
mlc::Array<ast::Diagnostic> accumulate_diagnostics_for_single_extend_method(mlc::String extend_type_name, std::shared_ptr<ast::Decl> method_declaration_shared_under_scan, registry::TypeRegistry registry) noexcept{
  return [&]() -> mlc::Array<ast::Diagnostic> {
auto __match_subject = method_declaration_shared_under_scan;
if (std::holds_alternative<ast::DeclFn>((*__match_subject))) {
const ast::DeclFn& declFn = std::get<ast::DeclFn>((*__match_subject));
auto [__0, type_parameters, __2, parameters, __4, method_body, where_entries] = declFn; return [&]() {
auto method_related_diagnostics = where_clause_unknown_parameter_diagnostics(type_parameters, where_entries, ast::expr_span(method_body));
auto method_environment = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
auto parameter_index = 0;
while ((parameter_index < parameters.length())) {
  method_environment.set(ast::param_name(parameters[parameter_index]), registry::type_from_annotation_with_registry(ast::param_type_value(parameters[parameter_index]), registry));
  (parameter_index = (parameter_index + 1));
}
auto extend_context = check_context::CheckContext{method_environment, registry, extend_type_name};
auto method_parsed = infer::infer_expr(method_body, extend_context);
return ast::diagnostics_append(method_related_diagnostics, method_parsed.errors);
}();
}
return [&]() {
auto empty_extend_method_diagnostic_branch = mlc::Array<ast::Diagnostic>{};
return empty_extend_method_diagnostic_branch;
}();
std::abort();
}();
}
mlc::Array<ast::Diagnostic> type_alias_cycle_diagnostic_fold_step(mlc::Array<ast::Diagnostic> diagnostics_so_far, std::shared_ptr<ast::Decl> declaration, registry::TypeRegistry registry) noexcept{
  return [&]() -> mlc::Array<ast::Diagnostic> {
auto __match_subject = ast::decl_inner(declaration);
if (std::holds_alternative<ast::DeclTypeAlias>((*__match_subject))) {
const ast::DeclTypeAlias& declTypeAlias = std::get<ast::DeclTypeAlias>((*__match_subject));
auto [alias_name, __1, __2, __3] = declTypeAlias; return (registry::type_alias_has_cycle(registry, alias_name) ? (diagnostics_so_far.concat(mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code((mlc::String("cyclic type alias ", 18) + alias_name), ast::decl_name_span(declaration), diagnostic_codes::diagnostic_code_e081())})) : (diagnostics_so_far));
}
return diagnostics_so_far;
std::abort();
}();
}
mlc::Array<ast::Diagnostic> type_alias_cycle_diagnostics(ast::Program program, registry::TypeRegistry registry) noexcept{
  return program.decls.fold([&]() {
auto empty_diagnostics = mlc::Array<ast::Diagnostic>{};
return empty_diagnostics;
}(), [=](mlc::Array<ast::Diagnostic> diagnostics_so_far, std::shared_ptr<ast::Decl> declaration) mutable { return type_alias_cycle_diagnostic_fold_step(diagnostics_so_far, declaration, registry); });
}
Check_fn_locals_fold_state check_fn_locals_parameter_fold_step(Check_fn_locals_fold_state state, std::shared_ptr<ast::Param> parameter, registry::TypeRegistry registry) noexcept{
  auto parameter_name = ast::param_name(parameter);
  auto type_environment = state.type_environment;
  type_environment.set(parameter_name, registry::type_from_annotation_with_registry(ast::param_type_value(parameter), registry));
  return Check_fn_locals_fold_state{state.locals.concat(mlc::Array<mlc::String>{parameter_name}), type_environment};
}
Program_check_gathered gather_program_check(ast::Program entry, ast::Program full_program) noexcept{
  auto all_diagnostics = trait_param_expand::trait_and_type_name_conflict_diagnostics(full_program);
  auto destructured_full_program = param_destructure_expand::expand_parameter_destructuring_in_program(full_program);
  (all_diagnostics = ast::diagnostics_append(all_diagnostics, param_destructure_expand::extern_parameter_destructure_diagnostics(full_program)));
  auto expanded_entry_program = param_destructure_expand::expand_parameter_destructuring_in_program(trait_param_expand::expand_trait_as_param_entry_using_full(entry, destructured_full_program));
  auto globals = collect_globals(destructured_full_program);
  auto registry = registry::build_registry(destructured_full_program);
  (all_diagnostics = ast::diagnostics_append(all_diagnostics, type_alias_cycle_diagnostics(destructured_full_program, registry)));
  (all_diagnostics = ast::diagnostics_append(all_diagnostics, cycle_lint::shared_cycle_lint_diagnostics(destructured_full_program, registry)));
  auto declaration_index = 0;
  while ((declaration_index < expanded_entry_program.decls.length()))   {
    std::visit(overloaded{[&](const ast::DeclFn& declFn) -> void { auto [name, type_parameters, trait_bounds, parameters, return_type_annotation, body, where_clause_bounds_entries] = declFn; [&]() {
(all_diagnostics = ast::diagnostics_append(all_diagnostics, where_clause_unknown_parameter_diagnostics(type_parameters, where_clause_bounds_entries, ast::expr_span(body))));
(all_diagnostics = ast::diagnostics_append(all_diagnostics, param_default_diagnostics(type_parameters, parameters, body, registry)));
auto locals_after_type_parameters = type_parameters.fold([&]() {
auto empty_locals = mlc::Array<mlc::String>{};
return empty_locals;
}(), [=](mlc::Array<mlc::String> locals_so_far, mlc::String type_parameter_name) mutable { return [&]() {
globals.set(type_parameter_name, true);
return locals_so_far.concat(mlc::Array<mlc::String>{type_parameter_name});
}(); });
auto locals_fold_result = parameters.fold(Check_fn_locals_fold_state{locals_after_type_parameters, {}}, [=](Check_fn_locals_fold_state state, std::shared_ptr<ast::Param> parameter) mutable { return check_fn_locals_parameter_fold_step(state, parameter, registry); });
auto locals = locals_fold_result.locals;
auto type_environment = locals_fold_result.type_environment;
auto body_partial_application = partial_application_desugar::partial_application_desugar_expr(body);
(all_diagnostics = ast::diagnostics_append(all_diagnostics, names::check_names_expr(body_partial_application, locals, globals)));
(all_diagnostics = ast::diagnostics_append(all_diagnostics, check_mutations::check_fn_body_mutations(parameters, body_partial_application)));
auto inference_context = check_context::check_context_new(type_environment, registry);
auto body_parsed = infer::infer_expr(body_partial_application, inference_context);
auto expected_type = registry::type_from_annotation_with_registry(return_type_annotation, registry);
auto actual_type = body_parsed.inferred_type;
(all_diagnostics = ast::diagnostics_append(all_diagnostics, body_parsed.errors));
if (((type_is_checkable(expected_type, registry) && type_is_checkable(actual_type, registry)) && (!semantic_type_structure::types_assignment_compatible(expected_type, actual_type)))) {
  (all_diagnostics = ast::diagnostics_append(all_diagnostics, mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code((((mlc::String("return type: expected ", 22) + semantic_type_structure::type_description(expected_type)) + mlc::String(", got ", 6)) + semantic_type_structure::type_description(actual_type)), ast::expr_span(body), diagnostic_codes::diagnostic_code_e004())}));
}
std::make_tuple();
return std::make_tuple();
}(); },
[&](const ast::DeclType& declType) -> void { auto [__0, type_parameters, variants, derive_trait_names, __4] = declType; [&]() {
(all_diagnostics = ast::diagnostics_append(all_diagnostics, derive_validation::derive_clause_diagnostics(type_parameters, variants, derive_trait_names, ast::decl_span(expanded_entry_program.decls[declaration_index]))));
(all_diagnostics = ast::diagnostics_append(all_diagnostics, record_field_default_value_diagnostics(type_parameters, variants, registry)));
std::make_tuple();
return std::make_tuple();
}(); },
[&](const ast::DeclTypeAlias& declTypeAlias) -> void { auto [__0, __1, __2, __3] = declTypeAlias; [&]() {
std::make_tuple();
return std::make_tuple();
}(); },
[&](const ast::DeclTrait& declTrait) -> void { auto [__0, __1, __2, __3] = declTrait; [&]() {
std::make_tuple();
return std::make_tuple();
}(); },
[&](const ast::DeclExtend& declExtend) -> void { auto [extend_type_name, __1, methods, __3] = declExtend; [&]() {
(all_diagnostics = methods.fold(all_diagnostics, [=](mlc::Array<ast::Diagnostic> diagnostics_so_far_across_extend_methods, std::shared_ptr<ast::Decl> method_shared_under_extend) mutable { return ast::diagnostics_append(diagnostics_so_far_across_extend_methods, accumulate_diagnostics_for_single_extend_method(extend_type_name, method_shared_under_extend, registry)); }));
std::make_tuple();
return std::make_tuple();
}(); },
[&](const ast::DeclImport& declImport) -> void { auto [__0, __1] = declImport; [&]() {
std::make_tuple();
return std::make_tuple();
}(); },
[&](const ast::DeclExported& declExported) -> void { auto [__0] = declExported; [&]() {
std::make_tuple();
return std::make_tuple();
}(); },
[&](const ast::DeclAssocType& declAssocType) -> void { auto [__0, __1] = declAssocType; [&]() {
std::make_tuple();
return std::make_tuple();
}(); },
[&](const ast::DeclAssocBind& declAssocBind) -> void { auto [__0, __1, __2] = declAssocBind; [&]() {
std::make_tuple();
return std::make_tuple();
}(); }
}, (*ast::decl_inner(expanded_entry_program.decls[declaration_index])));
    (declaration_index = (declaration_index + 1));
  }
  return Program_check_gathered{all_diagnostics, registry};
}
bool diagnostic_is_error(ast::Diagnostic diagnostic) noexcept{
  return (diagnostic.severity == mlc::String("error", 5));
}
mlc::Array<ast::Diagnostic> error_diagnostics_only(mlc::Array<ast::Diagnostic> diagnostics) noexcept{
  return diagnostics.filter([=](ast::Diagnostic diagnostic) mutable { return diagnostic_is_error(diagnostic); });
}
ast::Result<CheckOut, mlc::Array<mlc::String>> check_program_against_full(ast::Program entry, ast::Program full_program) noexcept{
  auto gathered = gather_program_check(entry, full_program);
  auto error_diagnostics = error_diagnostics_only(gathered.diagnostics);
  if ((error_diagnostics.length() > 0))   {
    return ast::Err<mlc::Array<mlc::String>>{ast::diagnostics_to_strings(error_diagnostics)};
  } else   {
    return ast::Ok<CheckOut>{CheckOut{{}, gathered.registry}};
  }
}
mlc::Array<ast::Diagnostic> program_diagnostics(ast::Program program) noexcept{
  return gather_program_check(program, program).diagnostics;
}
ast::Result<CheckOut, mlc::Array<mlc::String>> check_with_context(ast::Program entry, ast::Program full) noexcept{
  return check_program_against_full(entry, full);
}
ast::Result<CheckOut, mlc::Array<mlc::String>> check(ast::Program program) noexcept{
  return check_program_against_full(program, program);
}

} // namespace check
