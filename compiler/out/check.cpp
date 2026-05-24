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
#include "record_field_default_initializer.hpp"
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
using namespace record_field_default_initializer;
using namespace partial_application_desugar;
using namespace ast_tokens;

bool param_defaults_in_tail(mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept;

bool default_expr_mvp_ok(std::shared_ptr<ast::Expr> e) noexcept;

mlc::Array<ast::Diagnostic> accumulate_diagnostics_when_record_fields_have_defaults_under_generic_type_parameters(mlc::Array<ast::Diagnostic> diagnostics_accumulator_so_far, std::shared_ptr<ast::TypeVariant> variant_under_generic_scan) noexcept;

mlc::Array<ast::Diagnostic> accumulate_diagnostics_after_literal_record_default_checks(mlc::Array<ast::Diagnostic> diagnostics_accumulator_so_far, std::shared_ptr<ast::TypeVariant> variant_under_literal_scan, check_context::CheckContext record_default_inference_context, registry::TypeRegistry registry) noexcept;

mlc::Array<ast::Diagnostic> record_field_default_value_diagnostics(mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, registry::TypeRegistry registry) noexcept;

bool is_extern_body(std::shared_ptr<ast::Expr> e) noexcept;

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

ast::Result<check::CheckOut, mlc::Array<mlc::String>> check_program_against_full(ast::Program entry, ast::Program full_program) noexcept;

ast::Result<check::CheckOut, mlc::Array<mlc::String>> check_with_context(ast::Program entry, ast::Program full) noexcept;

ast::Result<check::CheckOut, mlc::Array<mlc::String>> check(ast::Program program) noexcept;

bool param_defaults_in_tail(mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept{
bool optional_began = false;
int i = 0;
while (i < parameters.size()){
{
bool has_default_now = parameters[i]->has_default;
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
i = i + 1;
}
}
return true;
}

bool default_expr_mvp_ok(std::shared_ptr<ast::Expr> e) noexcept{return [&]() { if (std::holds_alternative<ast::ExprInt>((*e)._)) { auto _v_exprint = std::get<ast::ExprInt>((*e)._); auto [_w0, _w1] = _v_exprint; return true; } if (std::holds_alternative<ast::ExprStr>((*e)._)) { auto _v_exprstr = std::get<ast::ExprStr>((*e)._); auto [_w0, _w1] = _v_exprstr; return true; } if (std::holds_alternative<ast::ExprBool>((*e)._)) { auto _v_exprbool = std::get<ast::ExprBool>((*e)._); auto [_w0, _w1] = _v_exprbool; return true; } if (std::holds_alternative<ast::ExprUnit>((*e)._)) { auto _v_exprunit = std::get<ast::ExprUnit>((*e)._); auto [_w0] = _v_exprunit; return true; } if (std::holds_alternative<ast::ExprFloat>((*e)._)) { auto _v_exprfloat = std::get<ast::ExprFloat>((*e)._); auto [_w0, _w1] = _v_exprfloat; return true; } if (std::holds_alternative<ast::ExprI64>((*e)._)) { auto _v_expri64 = std::get<ast::ExprI64>((*e)._); auto [_w0, _w1] = _v_expri64; return true; } if (std::holds_alternative<ast::ExprU8>((*e)._)) { auto _v_expru8 = std::get<ast::ExprU8>((*e)._); auto [_w0, _w1] = _v_expru8; return true; } if (std::holds_alternative<ast::ExprUsize>((*e)._)) { auto _v_exprusize = std::get<ast::ExprUsize>((*e)._); auto [_w0, _w1] = _v_exprusize; return true; } if (std::holds_alternative<ast::ExprChar>((*e)._)) { auto _v_exprchar = std::get<ast::ExprChar>((*e)._); auto [_w0, _w1] = _v_exprchar; return true; } return false; }();}

mlc::Array<ast::Diagnostic> accumulate_diagnostics_when_record_fields_have_defaults_under_generic_type_parameters(mlc::Array<ast::Diagnostic> diagnostics_accumulator_so_far, std::shared_ptr<ast::TypeVariant> variant_under_generic_scan) noexcept{return std::visit(overloaded{
  [&](const VarRecord& varrecord) -> mlc::Array<ast::Diagnostic> { auto [_w0, field_definitions_under_variant, _w1] = varrecord; return field_definitions_under_variant.fold(diagnostics_accumulator_so_far, [](mlc::Array<ast::Diagnostic> diagnostic_list_so_far_under_record, std::shared_ptr<ast::FieldDef> record_field_definition_under_scan) mutable { return [&]() -> mlc::Array<ast::Diagnostic> { 
  if (record_field_definition_under_scan->has_default_expression){
{
diagnostic_list_so_far_under_record.push_back(ast::diagnostic_error(mlc::String("record field default values are not supported when the type has generic parameters"), ast::span_unknown()));
}
}
  return diagnostic_list_so_far_under_record;
 }(); }); },
  [&](const VarTuple& vartuple) -> mlc::Array<ast::Diagnostic> { auto [_w0, _w1, _w2] = vartuple; return diagnostics_accumulator_so_far; },
  [&](const VarUnit& varunit) -> mlc::Array<ast::Diagnostic> { auto [_w0, _w1] = varunit; return diagnostics_accumulator_so_far; }
}, (*variant_under_generic_scan));}

mlc::Array<ast::Diagnostic> accumulate_diagnostics_after_literal_record_default_checks(mlc::Array<ast::Diagnostic> diagnostics_accumulator_so_far, std::shared_ptr<ast::TypeVariant> variant_under_literal_scan, check_context::CheckContext record_default_inference_context, registry::TypeRegistry registry) noexcept{return std::visit(overloaded{
  [&](const VarRecord& varrecord) -> mlc::Array<ast::Diagnostic> { auto [_w0, field_definitions_under_literal_scan, _w1] = varrecord; return field_definitions_under_literal_scan.fold(diagnostics_accumulator_so_far, [record_default_inference_context, registry](mlc::Array<ast::Diagnostic> diagnostic_list_so_far_under_fields, std::shared_ptr<ast::FieldDef> record_field_under_literal_definition) mutable { return !record_field_under_literal_definition->has_default_expression ? diagnostic_list_so_far_under_fields : [&]() -> mlc::Array<ast::Diagnostic> { 
  std::shared_ptr<ast::Expr> default_ast_expression = record_field_under_literal_definition->default_expression;
  mlc::Array<ast::Diagnostic> diagnostics_after_field = diagnostic_list_so_far_under_fields;
  return !record_field_default_initializer::record_field_default_expression_acceptable_for_codegen(default_ast_expression) ? [&]() -> mlc::Array<ast::Diagnostic> { 
  diagnostics_after_field.push_back(ast::diagnostic_error(mlc::String("record field default expression cannot be lowered to a C++ member initializer (literals, identifiers, + - * / %)"), ast::expr_span(default_ast_expression)));
  return diagnostics_after_field;
 }() : [&]() -> mlc::Array<ast::Diagnostic> { 
  infer_result::InferResult inferred_default_result = infer::infer_expr(default_ast_expression, record_default_inference_context);
  diagnostics_after_field = ast::diagnostics_append(diagnostics_after_field, inferred_default_result.errors);
  std::shared_ptr<registry::Type> annotated_field_type = registry::type_from_annotation(record_field_under_literal_definition->typ);
  if (type_is_checkable(annotated_field_type, registry) && type_is_checkable(inferred_default_result.inferred_type, registry) && !semantic_type_structure::types_structurally_equal(annotated_field_type, inferred_default_result.inferred_type)){
{
diagnostics_after_field.push_back(ast::diagnostic_error(mlc::String("record field default type mismatch for field \"") + record_field_under_literal_definition->name + mlc::String("\""), ast::expr_span(default_ast_expression)));
}
}
  return diagnostics_after_field;
 }();
 }(); }); },
  [&](const VarTuple& vartuple) -> mlc::Array<ast::Diagnostic> { auto [_w0, _w1, _w2] = vartuple; return diagnostics_accumulator_so_far; },
  [&](const VarUnit& varunit) -> mlc::Array<ast::Diagnostic> { auto [_w0, _w1] = varunit; return diagnostics_accumulator_so_far; }
}, (*variant_under_literal_scan));}

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

bool is_extern_body(std::shared_ptr<ast::Expr> e) noexcept{return [&]() { if (std::holds_alternative<ast::ExprExtern>((*e)._)) { auto _v_exprextern = std::get<ast::ExprExtern>((*e)._); auto [_w0] = _v_exprextern; return true; } return false; }();}

mlc::Array<ast::Diagnostic> param_default_diagnostics(mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<ast::Expr> body, registry::TypeRegistry registry) noexcept{
mlc::Array<ast::Diagnostic> out = {};
if (!param_defaults_in_tail(parameters)){
{
out.push_back(ast::diagnostic_error(mlc::String("parameter defaults must be trailing"), ast::expr_span(body)));
}
}
bool has_any_default = parameters.any([](std::shared_ptr<ast::Param> parameter_under_signature) mutable { return parameter_under_signature->has_default; });
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
return parameters.fold(out, [body, default_infer_context, registry](mlc::Array<ast::Diagnostic> diagnostic_list_so_far_under_parameters, std::shared_ptr<ast::Param> parameter_under_scan_for_defaults) mutable { return [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> diagnostics_after_parameter = diagnostic_list_so_far_under_parameters;
  if (parameter_under_scan_for_defaults->has_default && !param_destructure_expand::parameter_binding_is_plain_identifier(parameter_under_scan_for_defaults)){
{
diagnostics_after_parameter.push_back(ast::diagnostic_error(mlc::String("default values are not supported for destructuring parameters"), ast::expr_span(body)));
}
}
  if (parameter_under_scan_for_defaults->has_default){
{
std::shared_ptr<ast::Expr> param_default_expression = parameter_under_scan_for_defaults->default_;
if (!default_expr_mvp_ok(param_default_expression)){
diagnostics_after_parameter.push_back(ast::diagnostic_error(mlc::String("parameter default must be a literal in this version"), ast::expr_span(param_default_expression)));
} else {
infer_result::InferResult default_infer = infer::infer_expr(param_default_expression, default_infer_context);
diagnostics_after_parameter = ast::diagnostics_append(diagnostics_after_parameter, default_infer.errors);
std::shared_ptr<registry::Type> parameter_expected_type = registry::type_from_annotation(ast::param_typ(parameter_under_scan_for_defaults));
if (type_is_checkable(parameter_expected_type, registry) && type_is_checkable(default_infer.inferred_type, registry) && !semantic_type_structure::types_structurally_equal(parameter_expected_type, default_infer.inferred_type)){
diagnostics_after_parameter.push_back(ast::diagnostic_error(mlc::String("parameter default: expected ") + semantic_type_structure::type_description(parameter_expected_type) + mlc::String(", got ") + semantic_type_structure::type_description(default_infer.inferred_type), ast::expr_span(param_default_expression)));
}
}
}
}
  return diagnostics_after_parameter;
 }(); });
}

bool type_parameter_name_known(mlc::Array<mlc::String> type_parameter_names, mlc::String candidate_name) noexcept{return type_parameter_names.any([candidate_name](mlc::String type_parameter_name_under_scan) mutable { return type_parameter_name_under_scan == candidate_name; });}

mlc::Array<ast::Diagnostic> where_clause_unknown_parameter_diagnostics(mlc::Array<mlc::String> type_parameter_names, mlc::Array<ast::WhereClauseBound> where_entries, ast::Span source_span) noexcept{
mlc::Array<ast::Diagnostic> initial_where_clause_diagnostics = {};
return where_entries.fold(initial_where_clause_diagnostics, [type_parameter_names, source_span](mlc::Array<ast::Diagnostic> diagnostics_accumulated_so_far, ast::WhereClauseBound where_entry_under_scan) mutable { return type_parameter_name_known(type_parameter_names, where_entry_under_scan.parameter_name) ? diagnostics_accumulated_so_far : [&]() -> mlc::Array<ast::Diagnostic> { 
  diagnostics_accumulated_so_far.push_back(ast::diagnostic_error(mlc::String("where clause names unknown type parameter \"") + where_entry_under_scan.parameter_name + mlc::String("\""), source_span));
  return diagnostics_accumulated_so_far;
 }(); });
}

mlc::HashMap<mlc::String, bool> global_names_merge_type_constructor_variant_into_map(mlc::HashMap<mlc::String, bool> global_names_so_far, std::shared_ptr<ast::TypeVariant> type_constructor_variant_under_scan) noexcept{return std::visit(overloaded{
  [&](const VarUnit& varunit) -> mlc::HashMap<mlc::String, bool> { auto [variant_name, _w0] = varunit; return [&]() -> mlc::HashMap<mlc::String, bool> { 
  global_names_so_far.set(variant_name, true);
  return global_names_so_far;
 }(); },
  [&](const VarTuple& vartuple) -> mlc::HashMap<mlc::String, bool> { auto [variant_name, _w0, _w1] = vartuple; return [&]() -> mlc::HashMap<mlc::String, bool> { 
  global_names_so_far.set(variant_name, true);
  return global_names_so_far;
 }(); },
  [&](const VarRecord& varrecord) -> mlc::HashMap<mlc::String, bool> { auto [variant_name, _w0, _w1] = varrecord; return [&]() -> mlc::HashMap<mlc::String, bool> { 
  global_names_so_far.set(variant_name, true);
  return global_names_so_far;
 }(); }
}, (*type_constructor_variant_under_scan));}

mlc::HashMap<mlc::String, bool> global_names_merge_trait_method_into_map(mlc::HashMap<mlc::String, bool> global_names_so_far, std::shared_ptr<ast::Decl> method_declaration_under_scan) noexcept{return [&]() -> mlc::HashMap<mlc::String, bool> { if (std::holds_alternative<ast::DeclFn>((*method_declaration_under_scan))) { auto _v_declfn = std::get<ast::DeclFn>((*method_declaration_under_scan)); auto [function_name, _w0, _w1, _w2, _w3, _w4, _w5] = _v_declfn; return [&]() -> mlc::HashMap<mlc::String, bool> { 
  global_names_so_far.set(function_name, true);
  return global_names_so_far;
 }(); } return global_names_so_far; }();}

mlc::HashMap<mlc::String, bool> global_names_merge_single_declaration_into_map(mlc::HashMap<mlc::String, bool> global_names_so_far, std::shared_ptr<ast::Decl> declaration_under_scan) noexcept{return std::visit(overloaded{
  [&](const DeclFn& declfn) -> mlc::HashMap<mlc::String, bool> { auto [name, _w0, _w1, _w2, _w3, _w4, _w5] = declfn; return [&]() -> mlc::HashMap<mlc::String, bool> { 
  global_names_so_far.set(name, true);
  return global_names_so_far;
 }(); },
  [&](const DeclType& decltype_) -> mlc::HashMap<mlc::String, bool> { auto [name, _w0, variants, _w1] = decltype_; return [&]() -> mlc::HashMap<mlc::String, bool> { 
  global_names_so_far.set(name, true);
  return variants.fold(global_names_so_far, [](mlc::HashMap<mlc::String, bool> names_after_type_definition, std::shared_ptr<ast::TypeVariant> variant_shared_under_scan) mutable { return global_names_merge_type_constructor_variant_into_map(names_after_type_definition, variant_shared_under_scan); });
 }(); },
  [&](const DeclTrait& decltrait) -> mlc::HashMap<mlc::String, bool> { auto [name, _w0, methods] = decltrait; return [&]() -> mlc::HashMap<mlc::String, bool> { 
  global_names_so_far.set(name, true);
  return methods.fold(global_names_so_far, [](mlc::HashMap<mlc::String, bool> names_after_trait_definition, std::shared_ptr<ast::Decl> method_declaration_shared_under_scan) mutable { return global_names_merge_trait_method_into_map(names_after_trait_definition, method_declaration_shared_under_scan); });
 }(); },
  [&](const DeclExtend& declextend) -> mlc::HashMap<mlc::String, bool> { auto [_w0, _w1, _w2] = declextend; return global_names_so_far; },
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
names.set(mlc::String("File"), true);
names.set(mlc::String("Profile"), true);
names.set(mlc::String("Shared"), true);
names.set(mlc::String("Map"), true);
names.set(mlc::String("Ok"), true);
names.set(mlc::String("Err"), true);
names.set(mlc::String("Result"), true);
return program.decls.fold(names, [](mlc::HashMap<mlc::String, bool> global_names_so_far, std::shared_ptr<ast::Decl> declaration_shared_under_program) mutable { return global_names_merge_single_declaration_into_map(global_names_so_far, declaration_shared_under_program); });
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

mlc::Array<ast::Diagnostic> accumulate_diagnostics_for_single_extend_method(mlc::String extend_type_name, std::shared_ptr<ast::Decl> method_declaration_shared_under_scan, registry::TypeRegistry registry) noexcept{return [&]() -> mlc::Array<ast::Diagnostic> { if (std::holds_alternative<ast::DeclFn>((*method_declaration_shared_under_scan))) { auto _v_declfn = std::get<ast::DeclFn>((*method_declaration_shared_under_scan)); auto [_w0, type_params, _w1, params, _w2, method_body, where_entries] = _v_declfn; return [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> method_related_diagnostics = where_clause_unknown_parameter_diagnostics(type_params, where_entries, ast::expr_span(method_body));
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> method_environment = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
  int parameter_index = 0;
  while (parameter_index < params.size()){
{
method_environment.set(ast::param_name(params[parameter_index]), registry::type_from_annotation(ast::param_typ(params[parameter_index])));
parameter_index = parameter_index + 1;
}
}
  check_context::CheckContext extend_context = check_context::CheckContext{method_environment, registry, extend_type_name};
  infer_result::InferResult method_inference_result = infer::infer_expr(method_body, extend_context);
  return ast::diagnostics_append(method_related_diagnostics, method_inference_result.errors);
 }(); } return [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> empty_extend_method_diagnostic_branch = {};
  return empty_extend_method_diagnostic_branch;
 }(); }();}

ast::Result<check::CheckOut, mlc::Array<mlc::String>> check_program_against_full(ast::Program entry, ast::Program full_program) noexcept{
mlc::Array<ast::Diagnostic> all_diagnostics = trait_param_expand::trait_and_type_name_conflict_diagnostics(full_program);
ast::Program destructured_full_program = param_destructure_expand::expand_parameter_destructuring_in_program(full_program);
all_diagnostics = ast::diagnostics_append(all_diagnostics, param_destructure_expand::extern_parameter_destructure_diagnostics(full_program));
ast::Program expanded_entry_program = param_destructure_expand::expand_parameter_destructuring_in_program(trait_param_expand::expand_trait_as_param_entry_using_full(entry, destructured_full_program));
mlc::HashMap<mlc::String, bool> globals = collect_globals(destructured_full_program);
registry::TypeRegistry registry = registry::build_registry(destructured_full_program);
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
  std::shared_ptr<ast::Expr> body_partial_application = partial_application_desugar::partial_application_desugar_expr(body);
  all_diagnostics = ast::diagnostics_append(all_diagnostics, names::check_names_expr(body_partial_application, locals, globals));
  all_diagnostics = ast::diagnostics_append(all_diagnostics, check_mutations::check_fn_body_mutations(parameters, body_partial_application));
  check_context::CheckContext inference_context = check_context::check_context_new(type_environment, registry);
  infer_result::InferResult inference_result = infer::infer_expr(body_partial_application, inference_context);
  std::shared_ptr<registry::Type> expected_type = registry::type_from_annotation(return_type_annotation);
  std::shared_ptr<registry::Type> actual_type = inference_result.inferred_type;
  mlc::Array<ast::Diagnostic> return_type_errors = type_is_checkable(expected_type, registry) && type_is_checkable(actual_type, registry) && !semantic_type_structure::types_structurally_equal(expected_type, actual_type) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("return type: expected ") + semantic_type_structure::type_description(expected_type) + mlc::String(", got ") + semantic_type_structure::type_description(actual_type), ast::expr_span(body))} : mlc::Array<ast::Diagnostic>{};
  all_diagnostics = ast::diagnostics_append(ast::diagnostics_append(all_diagnostics, inference_result.errors), return_type_errors);
  return std::make_tuple();
 }(); },
  [&](const DeclType& decltype_) -> std::tuple<> { auto [_w0, type_parameters, variants, derive_trait_names] = decltype_; return [&]() -> std::tuple<> { 
  all_diagnostics = ast::diagnostics_append(all_diagnostics, derive_validation::derive_clause_diagnostics(type_parameters, variants, derive_trait_names, ast::span_unknown()));
  all_diagnostics = ast::diagnostics_append(all_diagnostics, record_field_default_value_diagnostics(type_parameters, variants, registry));
  return std::make_tuple();
 }(); },
  [&](const DeclTrait& decltrait) -> std::tuple<> { auto [_w0, _w1, _w2] = decltrait; return std::make_tuple(); },
  [&](const DeclExtend& declextend) -> std::tuple<> { auto [extend_type_name, _w0, methods] = declextend; return [&]() -> std::tuple<> { 
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
return all_diagnostics.size() > 0 ? ast::Result<check::CheckOut, mlc::Array<mlc::String>>(ast::Err<mlc::Array<mlc::String>>(ast::diagnostics_to_strings(all_diagnostics))) : ast::Result<check::CheckOut, mlc::Array<mlc::String>>(ast::Ok<check::CheckOut>(check::CheckOut{{}, registry}));
}

ast::Result<check::CheckOut, mlc::Array<mlc::String>> check_with_context(ast::Program entry, ast::Program full) noexcept{return check_program_against_full(entry, full);}

ast::Result<check::CheckOut, mlc::Array<mlc::String>> check(ast::Program program) noexcept{return check_program_against_full(program, program);}

} // namespace check
