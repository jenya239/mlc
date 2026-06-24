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
#include "record_defaults.hpp"
#include "partial_application_desugar.hpp"

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
using namespace record_defaults;
using namespace partial_application_desugar;
using namespace ast_tokens;

struct Check_fn_locals_fold_state {mlc::Array<mlc::String> locals;mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment;};

struct Program_check_gathered {mlc::Array<ast::Diagnostic> diagnostics;registry::TypeRegistry registry;};

bool param_defaults_in_tail(mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept;

bool default_expr_mvp_ok(std::shared_ptr<ast::Expr> expression) noexcept;

mlc::Array<ast::Diagnostic> accumulate_diagnostics_when_record_fields_have_defaults_under_generic_type_parameters(mlc::Array<ast::Diagnostic> diagnostics_accumulator_so_far, std::shared_ptr<ast::TypeVariant> variant_under_generic_scan) noexcept;

mlc::Array<ast::Diagnostic> accumulate_diagnostics_after_literal_record_default_checks(mlc::Array<ast::Diagnostic> diagnostics_accumulator_so_far, std::shared_ptr<ast::TypeVariant> variant_under_literal_scan, check_context::CheckContext record_default_inference_context, registry::TypeRegistry registry) noexcept;

mlc::Array<ast::Diagnostic> record_field_default_value_diagnostics(mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, registry::TypeRegistry registry) noexcept;

bool is_extern_body(std::shared_ptr<ast::Expr> expression) noexcept;

mlc::Array<ast::Diagnostic> param_default_diagnostics(mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<ast::Expr> body, registry::TypeRegistry registry) noexcept;

bool type_parameter_name_known(mlc::Array<mlc::String> type_parameter_names, mlc::String candidate_name) noexcept;

mlc::Array<ast::Diagnostic> where_clause_unknown_parameter_diagnostics(mlc::Array<mlc::String> type_parameter_names, mlc::Array<ast::WhereClauseBound> where_entries, ast::Span source_span) noexcept;

mlc::HashMap<mlc::String, bool> global_names_merge_type_constructor_variant_into_map(mlc::HashMap<mlc::String, bool> global_names_so_far, std::shared_ptr<ast::TypeVariant> type_constructor_variant_under_scan) noexcept;

mlc::HashMap<mlc::String, bool> global_names_merge_trait_method_into_map(mlc::HashMap<mlc::String, bool> global_names_so_far, std::shared_ptr<ast::Decl> method_declaration_under_scan) noexcept;

mlc::HashMap<mlc::String, bool> global_names_merge_single_declaration_into_map(mlc::HashMap<mlc::String, bool> global_names_so_far, std::shared_ptr<ast::Decl> declaration_under_scan) noexcept;

mlc::HashMap<mlc::String, bool> collect_globals(ast::Program program) noexcept;

bool type_is_checkable(std::shared_ptr<registry::Type> type_value, registry::TypeRegistry registry) noexcept;

bool CheckOut_has_errors(check::CheckOut self) noexcept;

mlc::Array<ast::Diagnostic> accumulate_diagnostics_for_single_extend_method(mlc::String extend_type_name, std::shared_ptr<ast::Decl> method_declaration_shared_under_scan, registry::TypeRegistry registry) noexcept;

mlc::Array<ast::Diagnostic> type_alias_cycle_diagnostic_fold_step(mlc::Array<ast::Diagnostic> diagnostics_so_far, std::shared_ptr<ast::Decl> declaration, registry::TypeRegistry registry) noexcept;

mlc::Array<ast::Diagnostic> type_alias_cycle_diagnostics(ast::Program program, registry::TypeRegistry registry) noexcept;

check::Check_fn_locals_fold_state check_fn_locals_parameter_fold_step(check::Check_fn_locals_fold_state state, std::shared_ptr<ast::Param> parameter, registry::TypeRegistry registry) noexcept;

check::Program_check_gathered gather_program_check(ast::Program entry, ast::Program full_program) noexcept;

ast::Result<check::CheckOut, mlc::Array<mlc::String>> check_program_against_full(ast::Program entry, ast::Program full_program) noexcept;

mlc::Array<ast::Diagnostic> program_diagnostics(ast::Program program) noexcept;

ast::Result<check::CheckOut, mlc::Array<mlc::String>> check_with_context(ast::Program entry, ast::Program full) noexcept;

ast::Result<check::CheckOut, mlc::Array<mlc::String>> check(ast::Program program) noexcept;

bool param_defaults_in_tail(mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept{
bool optional_began = false;
int parameter_index = 0;
while (parameter_index < parameters.size()){
{
bool has_default_now = parameters[parameter_index]->has_default;
if (!has_default_now && optional_began){
{
return false;
}
}
if (has_default_now){
{
optional_began = true;
}
}
parameter_index = parameter_index + 1;
}
}
return true;
}

bool default_expr_mvp_ok(std::shared_ptr<ast::Expr> expression) noexcept{return [&]() { if (std::holds_alternative<ast::ExprInt>((*expression)._)) { auto _v_exprint = std::get<ast::ExprInt>((*expression)._); auto [_w0, _w1] = _v_exprint; return true; } else if (std::holds_alternative<ast::ExprStr>((*expression)._)) { auto _v_exprstr = std::get<ast::ExprStr>((*expression)._); return true; } else if (std::holds_alternative<ast::ExprBool>((*expression)._)) { auto _v_exprbool = std::get<ast::ExprBool>((*expression)._); return true; } else if (std::holds_alternative<ast::ExprUnit>((*expression)._)) { auto _v_exprunit = std::get<ast::ExprUnit>((*expression)._); return true; } if (std::holds_alternative<ast::ExprFloat>((*expression)._)) { auto _v_exprfloat = std::get<ast::ExprFloat>((*expression)._); auto [_w0, _w1] = _v_exprfloat; return true; } else if (std::holds_alternative<ast::ExprI64>((*expression)._)) { auto _v_expri64 = std::get<ast::ExprI64>((*expression)._); return true; } else if (std::holds_alternative<ast::ExprU8>((*expression)._)) { auto _v_expru8 = std::get<ast::ExprU8>((*expression)._); return true; } else if (std::holds_alternative<ast::ExprUsize>((*expression)._)) { auto _v_exprusize = std::get<ast::ExprUsize>((*expression)._); return true; } else if (std::holds_alternative<ast::ExprChar>((*expression)._)) { auto _v_exprchar = std::get<ast::ExprChar>((*expression)._); return true; } return false; }();}

mlc::Array<ast::Diagnostic> accumulate_diagnostics_when_record_fields_have_defaults_under_generic_type_parameters(mlc::Array<ast::Diagnostic> diagnostics_accumulator_so_far, std::shared_ptr<ast::TypeVariant> variant_under_generic_scan) noexcept{return [&]() -> mlc::Array<ast::Diagnostic> { if (std::holds_alternative<ast::VarRecord>((*variant_under_generic_scan))) { auto _v_varrecord = std::get<ast::VarRecord>((*variant_under_generic_scan)); auto [_w0, field_definitions_under_variant, _w1] = _v_varrecord; return field_definitions_under_variant.fold(diagnostics_accumulator_so_far, [](mlc::Array<ast::Diagnostic> diagnostic_list_so_far_under_record, std::shared_ptr<ast::FieldDef> record_field_definition_under_scan) mutable { return record_field_definition_under_scan->has_default_expression ? diagnostic_list_so_far_under_record.concat(mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("record field default values are not supported when the type has generic parameters"), ast::expr_span(record_field_definition_under_scan->default_expression), diagnostic_codes::diagnostic_code_e042())}) : diagnostic_list_so_far_under_record; }); } if (std::holds_alternative<ast::VarTuple>((*variant_under_generic_scan))) { auto _v_vartuple = std::get<ast::VarTuple>((*variant_under_generic_scan)); auto [_w0, _w1, _w2] = _v_vartuple; return diagnostics_accumulator_so_far; } else if (std::holds_alternative<ast::VarUnit>((*variant_under_generic_scan))) { auto _v_varunit = std::get<ast::VarUnit>((*variant_under_generic_scan)); return diagnostics_accumulator_so_far; } }();}

mlc::Array<ast::Diagnostic> accumulate_diagnostics_after_literal_record_default_checks(mlc::Array<ast::Diagnostic> diagnostics_accumulator_so_far, std::shared_ptr<ast::TypeVariant> variant_under_literal_scan, check_context::CheckContext record_default_inference_context, registry::TypeRegistry registry) noexcept{return [&]() -> mlc::Array<ast::Diagnostic> { if (std::holds_alternative<ast::VarRecord>((*variant_under_literal_scan))) { auto _v_varrecord = std::get<ast::VarRecord>((*variant_under_literal_scan)); auto [_w0, field_definitions_under_literal_scan, _w1] = _v_varrecord; return field_definitions_under_literal_scan.fold(diagnostics_accumulator_so_far, [record_default_inference_context, registry](mlc::Array<ast::Diagnostic> diagnostic_list_so_far_under_fields, std::shared_ptr<ast::FieldDef> record_field_under_literal_definition) mutable { return !record_field_under_literal_definition->has_default_expression ? diagnostic_list_so_far_under_fields : !record_defaults::record_field_default_expression_acceptable_for_codegen(record_field_under_literal_definition->default_expression) ? diagnostic_list_so_far_under_fields.concat(mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("record field default expression cannot be lowered to a C++ member initializer (literals, identifiers, + - * / %)"), ast::expr_span(record_field_under_literal_definition->default_expression), diagnostic_codes::diagnostic_code_e043())}) : [&]() -> mlc::Array<ast::Diagnostic> { 
  std::shared_ptr<ast::Expr> default_ast_expression = record_field_under_literal_definition->default_expression;
  infer_result::InferResult default_parsed = infer::infer_expr(default_ast_expression, record_default_inference_context);
  mlc::Array<ast::Diagnostic> with_infer_errors = ast::diagnostics_append(diagnostic_list_so_far_under_fields, default_parsed.errors);
  std::shared_ptr<registry::Type> annotated_field_type = registry::type_from_annotation_with_registry(record_field_under_literal_definition->type_value, registry);
  return type_is_checkable(annotated_field_type, registry) && type_is_checkable(default_parsed.inferred_type, registry) && !semantic_type_structure::types_structurally_equal(annotated_field_type, default_parsed.inferred_type) ? with_infer_errors.concat(mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("record field default type mismatch for field \"") + record_field_under_literal_definition->name + mlc::String("\""), ast::expr_span(default_ast_expression), diagnostic_codes::diagnostic_code_e044())}) : with_infer_errors;
 }(); }); } if (std::holds_alternative<ast::VarTuple>((*variant_under_literal_scan))) { auto _v_vartuple = std::get<ast::VarTuple>((*variant_under_literal_scan)); auto [_w0, _w1, _w2] = _v_vartuple; return diagnostics_accumulator_so_far; } else if (std::holds_alternative<ast::VarUnit>((*variant_under_literal_scan))) { auto _v_varunit = std::get<ast::VarUnit>((*variant_under_literal_scan)); return diagnostics_accumulator_so_far; } }();}

mlc::Array<ast::Diagnostic> record_field_default_value_diagnostics(mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, registry::TypeRegistry registry) noexcept{
mlc::Array<ast::Diagnostic> diagnostics_accumulator = {};
if (type_parameters.size() > 0){
{
diagnostics_accumulator = variants.fold(diagnostics_accumulator, [](mlc::Array<ast::Diagnostic> diagnostics_so_far_across_variants, std::shared_ptr<ast::TypeVariant> variant_shared_under_generic_pass) mutable { return accumulate_diagnostics_when_record_fields_have_defaults_under_generic_type_parameters(diagnostics_so_far_across_variants, variant_shared_under_generic_pass); });
}
}
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> empty_record_default_environment = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
check_context::CheckContext record_default_inference_context = check_context::check_context_new(empty_record_default_environment, registry);
diagnostics_accumulator = variants.fold(diagnostics_accumulator, [record_default_inference_context, registry](mlc::Array<ast::Diagnostic> diagnostics_so_far_across_variants, std::shared_ptr<ast::TypeVariant> variant_shared_under_literal_pass) mutable { return accumulate_diagnostics_after_literal_record_default_checks(diagnostics_so_far_across_variants, variant_shared_under_literal_pass, record_default_inference_context, registry); });
return diagnostics_accumulator;
}

bool is_extern_body(std::shared_ptr<ast::Expr> expression) noexcept{return [&]() { if (std::holds_alternative<ast::ExprExtern>((*expression)._)) { auto _v_exprextern = std::get<ast::ExprExtern>((*expression)._); auto [_w0] = _v_exprextern; return true; } return false; }();}

mlc::Array<ast::Diagnostic> param_default_diagnostics(mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<ast::Expr> body, registry::TypeRegistry registry) noexcept{
mlc::Array<ast::Diagnostic> trailing_default_diagnostics = !param_defaults_in_tail(parameters) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("parameter defaults must be trailing"), ast::expr_span(body), diagnostic_codes::diagnostic_code_e036())} : [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> empty_diagnostics = {};
  return empty_diagnostics;
 }();
bool has_any_default = parameters.any([](std::shared_ptr<ast::Param> parameter_under_signature) mutable { return parameter_under_signature->has_default; });
mlc::Array<ast::Diagnostic> generic_default_diagnostics = type_parameters.size() > 0 && has_any_default ? trailing_default_diagnostics.concat(mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("default parameters are not allowed on generic functions yet"), ast::expr_span(body), diagnostic_codes::diagnostic_code_e037())}) : trailing_default_diagnostics;
mlc::Array<ast::Diagnostic> initial_default_diagnostics = is_extern_body(body) && has_any_default ? generic_default_diagnostics.concat(mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("default parameters are not allowed on extern functions"), ast::expr_span(body), diagnostic_codes::diagnostic_code_e038())}) : generic_default_diagnostics;
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> empty_type_environment = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
check_context::CheckContext default_infer_context = check_context::check_context_new(empty_type_environment, registry);
return parameters.fold(initial_default_diagnostics, [body, default_infer_context, registry](mlc::Array<ast::Diagnostic> diagnostic_list_so_far_under_parameters, std::shared_ptr<ast::Param> parameter_under_scan_for_defaults) mutable { return [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> diagnostics_after_parameter = parameter_under_scan_for_defaults->has_default && !param_destructure_expand::parameter_binding_is_plain_identifier(parameter_under_scan_for_defaults) ? diagnostic_list_so_far_under_parameters.concat(mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("default values are not supported for destructuring parameters"), ast::expr_span(body), diagnostic_codes::diagnostic_code_e039())}) : diagnostic_list_so_far_under_parameters;
  return parameter_under_scan_for_defaults->has_default ? [&]() -> mlc::Array<ast::Diagnostic> { 
  std::shared_ptr<ast::Expr> param_default_expression = parameter_under_scan_for_defaults->default_;
  return !default_expr_mvp_ok(param_default_expression) ? diagnostics_after_parameter.concat(mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("parameter default must be a literal in this version"), ast::expr_span(param_default_expression), diagnostic_codes::diagnostic_code_e040())}) : [&]() -> mlc::Array<ast::Diagnostic> { 
  infer_result::InferResult default_infer = infer::infer_expr(param_default_expression, default_infer_context);
  mlc::Array<ast::Diagnostic> with_infer_errors = ast::diagnostics_append(diagnostics_after_parameter, default_infer.errors);
  std::shared_ptr<registry::Type> parameter_expected_type = registry::type_from_annotation_with_registry(ast::param_type_value(parameter_under_scan_for_defaults), registry);
  return type_is_checkable(parameter_expected_type, registry) && type_is_checkable(default_infer.inferred_type, registry) && !semantic_type_structure::types_structurally_equal(parameter_expected_type, default_infer.inferred_type) ? with_infer_errors.concat(mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("parameter default: expected ") + semantic_type_structure::type_description(parameter_expected_type) + mlc::String(", got ") + semantic_type_structure::type_description(default_infer.inferred_type), ast::expr_span(param_default_expression), diagnostic_codes::diagnostic_code_e041())}) : with_infer_errors;
 }();
 }() : diagnostics_after_parameter;
 }(); });
}

bool type_parameter_name_known(mlc::Array<mlc::String> type_parameter_names, mlc::String candidate_name) noexcept{return type_parameter_names.any([candidate_name](mlc::String type_parameter_name_under_scan) mutable { return type_parameter_name_under_scan == candidate_name; });}

mlc::Array<ast::Diagnostic> where_clause_unknown_parameter_diagnostics(mlc::Array<mlc::String> type_parameter_names, mlc::Array<ast::WhereClauseBound> where_entries, ast::Span source_span) noexcept{
mlc::Array<ast::Diagnostic> initial_where_clause_diagnostics = {};
return where_entries.fold(initial_where_clause_diagnostics, [type_parameter_names, source_span](mlc::Array<ast::Diagnostic> diagnostics_accumulated_so_far, ast::WhereClauseBound where_entry_under_scan) mutable { return type_parameter_name_known(type_parameter_names, where_entry_under_scan.parameter_name) ? diagnostics_accumulated_so_far : diagnostics_accumulated_so_far.concat(mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("where clause names unknown type parameter '") + where_entry_under_scan.parameter_name + mlc::String("'"), source_span, diagnostic_codes::diagnostic_code_e076())}); });
}

mlc::HashMap<mlc::String, bool> global_names_merge_type_constructor_variant_into_map(mlc::HashMap<mlc::String, bool> global_names_so_far, std::shared_ptr<ast::TypeVariant> type_constructor_variant_under_scan) noexcept{return [&]() -> mlc::HashMap<mlc::String, bool> { if (std::holds_alternative<ast::VarUnit>((*type_constructor_variant_under_scan))) { auto _v_varunit = std::get<ast::VarUnit>((*type_constructor_variant_under_scan)); auto [variant_name, _w0] = _v_varunit; return [&]() -> mlc::HashMap<mlc::String, bool> { 
  global_names_so_far.set(variant_name, true);
  return global_names_so_far;
 }(); } else if (std::holds_alternative<ast::VarTuple>((*type_constructor_variant_under_scan))) { auto _v_vartuple = std::get<ast::VarTuple>((*type_constructor_variant_under_scan)); auto [variant_name, _w0, _w1] = _v_vartuple; return [&]() -> mlc::HashMap<mlc::String, bool> { 
  global_names_so_far.set(variant_name, true);
  return global_names_so_far;
 }(); } else if (std::holds_alternative<ast::VarRecord>((*type_constructor_variant_under_scan))) { auto _v_varrecord = std::get<ast::VarRecord>((*type_constructor_variant_under_scan)); auto [variant_name, _w0, _w1] = _v_varrecord; return [&]() -> mlc::HashMap<mlc::String, bool> { 
  global_names_so_far.set(variant_name, true);
  return global_names_so_far;
 }(); } }();}

mlc::HashMap<mlc::String, bool> global_names_merge_trait_method_into_map(mlc::HashMap<mlc::String, bool> global_names_so_far, std::shared_ptr<ast::Decl> method_declaration_under_scan) noexcept{return [&]() -> mlc::HashMap<mlc::String, bool> { if (std::holds_alternative<ast::DeclFn>((*method_declaration_under_scan))) { auto _v_declfn = std::get<ast::DeclFn>((*method_declaration_under_scan)); auto [function_name, _w0, _w1, _w2, _w3, _w4, _w5] = _v_declfn; return [&]() -> mlc::HashMap<mlc::String, bool> { 
  global_names_so_far.set(function_name, true);
  return global_names_so_far;
 }(); } return global_names_so_far; }();}

mlc::HashMap<mlc::String, bool> global_names_merge_single_declaration_into_map(mlc::HashMap<mlc::String, bool> global_names_so_far, std::shared_ptr<ast::Decl> declaration_under_scan) noexcept{return std::visit(overloaded{
  [&](const DeclFn& declfn) -> mlc::HashMap<mlc::String, bool> { auto [name, _w0, _w1, _w2, _w3, _w4, _w5] = declfn; return [&]() -> mlc::HashMap<mlc::String, bool> { 
  global_names_so_far.set(name, true);
  return global_names_so_far;
 }(); },
  [&](const DeclType& decltype_) -> mlc::HashMap<mlc::String, bool> { auto [name, _w0, variants, _w1, _w2] = decltype_; return [&]() -> mlc::HashMap<mlc::String, bool> { 
  global_names_so_far.set(name, true);
  return variants.fold(global_names_so_far, [](mlc::HashMap<mlc::String, bool> names_after_type_definition, std::shared_ptr<ast::TypeVariant> variant_shared_under_scan) mutable { return global_names_merge_type_constructor_variant_into_map(names_after_type_definition, variant_shared_under_scan); });
 }(); },
  [&](const DeclTypeAlias& decltypealias) -> mlc::HashMap<mlc::String, bool> { auto [name, _w0, _w1, _w2] = decltypealias; return [&]() -> mlc::HashMap<mlc::String, bool> { 
  global_names_so_far.set(name, true);
  return global_names_so_far;
 }(); },
  [&](const DeclTrait& decltrait) -> mlc::HashMap<mlc::String, bool> { auto [name, _w0, methods, _w1] = decltrait; return [&]() -> mlc::HashMap<mlc::String, bool> { 
  global_names_so_far.set(name, true);
  return methods.fold(global_names_so_far, [](mlc::HashMap<mlc::String, bool> names_after_trait_definition, std::shared_ptr<ast::Decl> method_declaration_shared_under_scan) mutable { return global_names_merge_trait_method_into_map(names_after_trait_definition, method_declaration_shared_under_scan); });
 }(); },
  [&](const DeclExtend& declextend) -> mlc::HashMap<mlc::String, bool> { auto [_w0, _w1, _w2, _w3] = declextend; return global_names_so_far; },
  [&](const DeclImport& declimport) -> mlc::HashMap<mlc::String, bool> { auto [_w0, _w1] = declimport; return global_names_so_far; },
  [&](const DeclExported& declexported) -> mlc::HashMap<mlc::String, bool> { auto [_w0] = declexported; return global_names_so_far; },
  [&](const DeclAssocType& declassoctype) -> mlc::HashMap<mlc::String, bool> { auto [_w0, _w1] = declassoctype; return global_names_so_far; },
  [&](const DeclAssocBind& declassocbind) -> mlc::HashMap<mlc::String, bool> { auto [_w0, _w1, _w2] = declassocbind; return global_names_so_far; }
}, (*ast::decl_inner(declaration_under_scan)));}

mlc::HashMap<mlc::String, bool> collect_globals(ast::Program program) noexcept{
mlc::HashMap<mlc::String, bool> names = mlc::HashMap<mlc::String, bool>();
names.set(mlc::String("true"), true);
names.set(mlc::String("false"), true);
names.set(mlc::String("exit"), true);
names.set(mlc::String("print"), true);
names.set(mlc::String("println"), true);
names.set(mlc::String("args"), true);
names.set(mlc::String("read_line"), true);
names.set(mlc::String("File"), true);
names.set(mlc::String("Profile"), true);
names.set(mlc::String("Shared"), true);
names.set(mlc::String("Map"), true);
names.set(mlc::String("Ok"), true);
names.set(mlc::String("Err"), true);
names.set(mlc::String("Result"), true);
return program.decls.fold(names, [](mlc::HashMap<mlc::String, bool> global_names_so_far, std::shared_ptr<ast::Decl> declaration_shared_under_program) mutable { return global_names_merge_single_declaration_into_map(global_names_so_far, declaration_shared_under_program); });
}

bool type_is_checkable(std::shared_ptr<registry::Type> type_value, registry::TypeRegistry registry) noexcept{return [&]() { if (std::holds_alternative<registry::TI32>((*type_value))) {  return true; } if (std::holds_alternative<registry::TString>((*type_value))) {  return true; } if (std::holds_alternative<registry::TBool>((*type_value))) {  return true; } if (std::holds_alternative<registry::TUnit>((*type_value))) {  return true; } if (std::holds_alternative<registry::TI64>((*type_value))) {  return true; } if (std::holds_alternative<registry::TF64>((*type_value))) {  return true; } if (std::holds_alternative<registry::TU8>((*type_value))) {  return true; } if (std::holds_alternative<registry::TUsize>((*type_value))) {  return true; } if (std::holds_alternative<registry::TChar>((*type_value))) {  return true; } if (std::holds_alternative<registry::TArray>((*type_value))) { auto _v_tarray = std::get<registry::TArray>((*type_value)); auto [inner] = _v_tarray; return type_is_checkable(inner, registry); } if (std::holds_alternative<registry::TPair>((*type_value))) { auto _v_tpair = std::get<registry::TPair>((*type_value)); auto [a, b] = _v_tpair; return type_is_checkable(a, registry) && type_is_checkable(b, registry); } if (std::holds_alternative<registry::TTuple>((*type_value))) { auto _v_ttuple = std::get<registry::TTuple>((*type_value)); auto [type_parameters] = _v_ttuple; return type_parameters.all([registry](std::shared_ptr<registry::Type> type_parameter) mutable { return type_is_checkable(type_parameter, registry); }); } if (std::holds_alternative<registry::TGeneric>((*type_value))) { auto _v_tgeneric = std::get<registry::TGeneric>((*type_value)); auto [_w0, type_arguments] = _v_tgeneric; return type_arguments.all([registry](std::shared_ptr<registry::Type> type_argument) mutable { return type_is_checkable(type_argument, registry); }); } if (std::holds_alternative<registry::TFn>((*type_value))) { auto _v_tfn = std::get<registry::TFn>((*type_value)); auto [_w0, _w1] = _v_tfn; return true; } if (std::holds_alternative<registry::TShared>((*type_value))) { auto _v_tshared = std::get<registry::TShared>((*type_value)); auto [_w0] = _v_tshared; return true; } if (std::holds_alternative<registry::TNamed>((*type_value))) { auto _v_tnamed = std::get<registry::TNamed>((*type_value)); auto [name] = _v_tnamed; return registry::TypeRegistry_has_fields(registry, name); } if (std::holds_alternative<registry::TAssoc>((*type_value))) { auto _v_tassoc = std::get<registry::TAssoc>((*type_value)); auto [_w0, _w1] = _v_tassoc; return true; } return false; }();}

bool CheckOut_has_errors(check::CheckOut self) noexcept{return self.errors.size() > 0;}

mlc::Array<ast::Diagnostic> accumulate_diagnostics_for_single_extend_method(mlc::String extend_type_name, std::shared_ptr<ast::Decl> method_declaration_shared_under_scan, registry::TypeRegistry registry) noexcept{return [&]() -> mlc::Array<ast::Diagnostic> { if (std::holds_alternative<ast::DeclFn>((*method_declaration_shared_under_scan))) { auto _v_declfn = std::get<ast::DeclFn>((*method_declaration_shared_under_scan)); auto [_w0, type_parameters, _w1, parameters, _w2, method_body, where_entries] = _v_declfn; return [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> method_related_diagnostics = where_clause_unknown_parameter_diagnostics(type_parameters, where_entries, ast::expr_span(method_body));
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> method_environment = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
  int parameter_index = 0;
  while (parameter_index < parameters.size()){
{
method_environment.set(ast::param_name(parameters[parameter_index]), registry::type_from_annotation_with_registry(ast::param_type_value(parameters[parameter_index]), registry));
parameter_index = parameter_index + 1;
}
}
  check_context::CheckContext extend_context = check_context::CheckContext{method_environment, registry, extend_type_name};
  infer_result::InferResult method_parsed = infer::infer_expr(method_body, extend_context);
  return ast::diagnostics_append(method_related_diagnostics, method_parsed.errors);
 }(); } return [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> empty_extend_method_diagnostic_branch = {};
  return empty_extend_method_diagnostic_branch;
 }(); }();}

mlc::Array<ast::Diagnostic> type_alias_cycle_diagnostic_fold_step(mlc::Array<ast::Diagnostic> diagnostics_so_far, std::shared_ptr<ast::Decl> declaration, registry::TypeRegistry registry) noexcept{return [&]() -> mlc::Array<ast::Diagnostic> { if (std::holds_alternative<ast::DeclTypeAlias>((*ast::decl_inner(declaration)))) { auto _v_decltypealias = std::get<ast::DeclTypeAlias>((*ast::decl_inner(declaration))); auto [alias_name, _w0, _w1, _w2] = _v_decltypealias; return registry::type_alias_has_cycle(registry, alias_name) ? diagnostics_so_far.concat(mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("cyclic type alias ") + alias_name, ast::decl_name_span(declaration), diagnostic_codes::diagnostic_code_e081())}) : diagnostics_so_far; } return diagnostics_so_far; }();}

mlc::Array<ast::Diagnostic> type_alias_cycle_diagnostics(ast::Program program, registry::TypeRegistry registry) noexcept{return program.decls.fold([&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> empty_diagnostics = {};
  return empty_diagnostics;
 }(), [registry](mlc::Array<ast::Diagnostic> diagnostics_so_far, std::shared_ptr<ast::Decl> declaration) mutable { return type_alias_cycle_diagnostic_fold_step(diagnostics_so_far, declaration, registry); });}

check::Check_fn_locals_fold_state check_fn_locals_parameter_fold_step(check::Check_fn_locals_fold_state state, std::shared_ptr<ast::Param> parameter, registry::TypeRegistry registry) noexcept{
mlc::String parameter_name = ast::param_name(parameter);
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment = state.type_environment;
type_environment.set(parameter_name, registry::type_from_annotation_with_registry(ast::param_type_value(parameter), registry));
return check::Check_fn_locals_fold_state{state.locals.concat(mlc::Array<mlc::String>{parameter_name}), type_environment};
}

check::Program_check_gathered gather_program_check(ast::Program entry, ast::Program full_program) noexcept{
mlc::Array<ast::Diagnostic> all_diagnostics = trait_param_expand::trait_and_type_name_conflict_diagnostics(full_program);
ast::Program destructured_full_program = param_destructure_expand::expand_parameter_destructuring_in_program(full_program);
all_diagnostics = ast::diagnostics_append(all_diagnostics, param_destructure_expand::extern_parameter_destructure_diagnostics(full_program));
ast::Program expanded_entry_program = param_destructure_expand::expand_parameter_destructuring_in_program(trait_param_expand::expand_trait_as_param_entry_using_full(entry, destructured_full_program));
mlc::HashMap<mlc::String, bool> globals = collect_globals(destructured_full_program);
registry::TypeRegistry registry = registry::build_registry(destructured_full_program);
all_diagnostics = ast::diagnostics_append(all_diagnostics, type_alias_cycle_diagnostics(destructured_full_program, registry));
int declaration_index = 0;
while (declaration_index < expanded_entry_program.decls.size()){
{
std::visit(overloaded{
  [&](const DeclFn& declfn) -> std::tuple<> { auto [name, type_parameters, trait_bounds, parameters, return_type_annotation, body, where_clause_bounds_entries] = declfn; return [&]() -> std::tuple<> { 
  all_diagnostics = ast::diagnostics_append(all_diagnostics, where_clause_unknown_parameter_diagnostics(type_parameters, where_clause_bounds_entries, ast::expr_span(body)));
  all_diagnostics = ast::diagnostics_append(all_diagnostics, param_default_diagnostics(type_parameters, parameters, body, registry));
  mlc::Array<mlc::String> locals_after_type_parameters = type_parameters.fold([&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> empty_locals = {};
  return empty_locals;
 }(), [&globals](mlc::Array<mlc::String> locals_so_far, mlc::String type_parameter_name) mutable { return [&]() -> mlc::Array<mlc::String> { 
  globals.set(type_parameter_name, true);
  return locals_so_far.concat(mlc::Array<mlc::String>{type_parameter_name});
 }(); });
  check::Check_fn_locals_fold_state locals_fold_result = parameters.fold(check::Check_fn_locals_fold_state{locals_after_type_parameters, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>()}, [registry](check::Check_fn_locals_fold_state state, std::shared_ptr<ast::Param> parameter) mutable { return check_fn_locals_parameter_fold_step(state, parameter, registry); });
  mlc::Array<mlc::String> locals = locals_fold_result.locals;
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment = locals_fold_result.type_environment;
  std::shared_ptr<ast::Expr> body_partial_application = partial_application_desugar::partial_application_desugar_expr(body);
  all_diagnostics = ast::diagnostics_append(all_diagnostics, names::check_names_expr(body_partial_application, locals, globals));
  all_diagnostics = ast::diagnostics_append(all_diagnostics, check_mutations::check_fn_body_mutations(parameters, body_partial_application));
  check_context::CheckContext inference_context = check_context::check_context_new(type_environment, registry);
  infer_result::InferResult body_parsed = infer::infer_expr(body_partial_application, inference_context);
  std::shared_ptr<registry::Type> expected_type = registry::type_from_annotation_with_registry(return_type_annotation, registry);
  std::shared_ptr<registry::Type> actual_type = body_parsed.inferred_type;
  mlc::Array<ast::Diagnostic> return_type_errors = type_is_checkable(expected_type, registry) && type_is_checkable(actual_type, registry) && !semantic_type_structure::types_structurally_equal(expected_type, actual_type) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("return type: expected ") + semantic_type_structure::type_description(expected_type) + mlc::String(", got ") + semantic_type_structure::type_description(actual_type), ast::expr_span(body), diagnostic_codes::diagnostic_code_e004())} : mlc::Array<ast::Diagnostic>{};
  all_diagnostics = ast::diagnostics_append(ast::diagnostics_append(all_diagnostics, body_parsed.errors), return_type_errors);
  return std::make_tuple();
 }(); },
  [&](const DeclType& decltype_) -> std::tuple<> { auto [_w0, type_parameters, variants, derive_trait_names, _w1] = decltype_; return [&]() -> std::tuple<> { 
  ast::Span declaration_span = ast::decl_span(expanded_entry_program.decls[declaration_index]);
  all_diagnostics = ast::diagnostics_append(all_diagnostics, derive_validation::derive_clause_diagnostics(type_parameters, variants, derive_trait_names, declaration_span));
  all_diagnostics = ast::diagnostics_append(all_diagnostics, record_field_default_value_diagnostics(type_parameters, variants, registry));
  return std::make_tuple();
 }(); },
  [&](const DeclTypeAlias& decltypealias) -> std::tuple<> { auto [_w0, _w1, _w2, _w3] = decltypealias; return std::make_tuple(); },
  [&](const DeclTrait& decltrait) -> std::tuple<> { auto [_w0, _w1, _w2, _w3] = decltrait; return std::make_tuple(); },
  [&](const DeclExtend& declextend) -> std::tuple<> { auto [extend_type_name, _w0, methods, _w1] = declextend; return [&]() -> std::tuple<> { 
  all_diagnostics = methods.fold(all_diagnostics, [extend_type_name, registry](mlc::Array<ast::Diagnostic> diagnostics_so_far_across_extend_methods, std::shared_ptr<ast::Decl> method_shared_under_extend) mutable { return ast::diagnostics_append(diagnostics_so_far_across_extend_methods, accumulate_diagnostics_for_single_extend_method(extend_type_name, method_shared_under_extend, registry)); });
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
return check::Program_check_gathered{all_diagnostics, registry};
}

ast::Result<check::CheckOut, mlc::Array<mlc::String>> check_program_against_full(ast::Program entry, ast::Program full_program) noexcept{
check::Program_check_gathered gathered = gather_program_check(entry, full_program);
return gathered.diagnostics.size() > 0 ? ast::Result<check::CheckOut, mlc::Array<mlc::String>>(ast::Err<mlc::Array<mlc::String>>(ast::diagnostics_to_strings(gathered.diagnostics))) : ast::Result<check::CheckOut, mlc::Array<mlc::String>>(ast::Ok<check::CheckOut>(check::CheckOut{{}, gathered.registry}));
}

mlc::Array<ast::Diagnostic> program_diagnostics(ast::Program program) noexcept{return gather_program_check(program, program).diagnostics;}

ast::Result<check::CheckOut, mlc::Array<mlc::String>> check_with_context(ast::Program entry, ast::Program full) noexcept{return check_program_against_full(entry, full);}

ast::Result<check::CheckOut, mlc::Array<mlc::String>> check(ast::Program program) noexcept{return check_program_against_full(program, program);}

} // namespace check
