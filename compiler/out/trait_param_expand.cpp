#define main mlc_user_main
#include "trait_param_expand.hpp"

#include "ast.hpp"
#include "diagnostic_codes.hpp"

namespace trait_param_expand {

using namespace ast;
using namespace diagnostic_codes;
using namespace ast_tokens;

std::shared_ptr<ast::Decl> declaration_without_export_wrappers(std::shared_ptr<ast::Decl> declaration) noexcept{
  return [&]() -> std::shared_ptr<ast::Decl> {
auto __match_subject = declaration;
if (std::holds_alternative<ast::DeclExported>((*__match_subject))) {
const ast::DeclExported& declExported = std::get<ast::DeclExported>((*__match_subject));
auto [exported_declaration] = declExported; return declaration_without_export_wrappers(exported_declaration);
}
return declaration;
std::abort();
}();
}
Trait_name_conflict_fold_state trait_name_conflict_fold_step(Trait_name_conflict_fold_state fold_state, std::shared_ptr<ast::Decl> current_declaration) noexcept{
  return [&]() -> Trait_name_conflict_fold_state {
auto __match_subject = current_declaration;
if (std::holds_alternative<ast::DeclType>((*__match_subject))) {
const ast::DeclType& declType = std::get<ast::DeclType>((*__match_subject));
auto [type_name, __1, __2, __3, __4] = declType; return [&]() {
auto conflicts_after_type = (fold_state.trait_declaration_names.has(type_name) ? (fold_state.conflicts.concat(mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(((mlc::String("the name \"", 10) + type_name) + mlc::String("\" is declared as both a type and a trait", 40)), ast::decl_name_span(current_declaration), diagnostic_codes::diagnostic_code_e073())})) : (fold_state.conflicts));
return Trait_name_conflict_fold_state{fold_state.trait_declaration_names, [&]() {
auto nominal_names = fold_state.nominal_type_declaration_names;
nominal_names.set(type_name, true);
return nominal_names;
}(), conflicts_after_type};
}();
}
if (std::holds_alternative<ast::DeclTypeAlias>((*__match_subject))) {
const ast::DeclTypeAlias& declTypeAlias = std::get<ast::DeclTypeAlias>((*__match_subject));
auto [type_name, __1, __2, __3] = declTypeAlias; return [&]() {
auto conflicts_after_alias = (fold_state.trait_declaration_names.has(type_name) ? (fold_state.conflicts.concat(mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(((mlc::String("the name \"", 10) + type_name) + mlc::String("\" is declared as both a type and a trait", 40)), ast::decl_name_span(current_declaration), diagnostic_codes::diagnostic_code_e073())})) : (fold_state.conflicts));
return Trait_name_conflict_fold_state{fold_state.trait_declaration_names, [&]() {
auto nominal_names = fold_state.nominal_type_declaration_names;
nominal_names.set(type_name, true);
return nominal_names;
}(), conflicts_after_alias};
}();
}
if (std::holds_alternative<ast::DeclTrait>((*__match_subject))) {
const ast::DeclTrait& declTrait = std::get<ast::DeclTrait>((*__match_subject));
auto [trait_name, __1, __2, __3] = declTrait; return [&]() {
auto conflicts_after_trait = (fold_state.nominal_type_declaration_names.has(trait_name) ? (fold_state.conflicts.concat(mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(((mlc::String("the name \"", 10) + trait_name) + mlc::String("\" is declared as both a type and a trait", 40)), ast::decl_name_span(current_declaration), diagnostic_codes::diagnostic_code_e073())})) : (fold_state.conflicts));
return Trait_name_conflict_fold_state{[&]() {
auto trait_names = fold_state.trait_declaration_names;
trait_names.set(trait_name, true);
return trait_names;
}(), fold_state.nominal_type_declaration_names, conflicts_after_trait};
}();
}
return fold_state;
std::abort();
}();
}
mlc::Array<ast::Diagnostic> trait_and_type_name_conflict_diagnostics(ast::Program program) noexcept{
  auto initial_fold_state = Trait_name_conflict_fold_state{{}, {}, [&]() {
auto empty_conflicts = mlc::Array<ast::Diagnostic>{};
return empty_conflicts;
}()};
  auto final_fold_state = program.decls.fold(initial_fold_state, [=](Trait_name_conflict_fold_state fold_state_so_far, std::shared_ptr<ast::Decl> root_declaration) mutable { return trait_name_conflict_fold_step(fold_state_so_far, declaration_without_export_wrappers(root_declaration)); });
  return final_fold_state.conflicts;
}
mlc::Array<mlc::Array<mlc::String>> align_trait_bounds_matrix(mlc::Array<mlc::String> type_parameter_names, mlc::Array<mlc::Array<mlc::String>> trait_bounds_rows) noexcept{
  auto copied_trait_bound_rows = trait_bounds_rows.concat({});
  auto empty_trait_bound_row = mlc::Array<mlc::String>{};
  return type_parameter_names.drop(copied_trait_bound_rows.length()).fold(copied_trait_bound_rows, [=](auto accumulated_aligned_matrix, mlc::String _unused_type_parameter_padding_key) mutable { return accumulated_aligned_matrix.concat(mlc::Array<mlc::Array<mlc::String>>{empty_trait_bound_row}); });
}
TraitExpandChunk expand_type_expression_under_array_or_shared_wrapper(std::shared_ptr<ast::TypeExpr> inner_type_expression_under_wrapper, mlc::HashMap<mlc::String, bool> explicit_type_parameter_environment, mlc::HashMap<mlc::String, bool> trait_declaration_names, mlc::HashMap<mlc::String, bool> nominal_type_declaration_names, int synthetic_counter, bool output_outer_is_shared_pointer_type) noexcept{
  auto inner_expansion_chunk = expand_type_expression_for_trait_param(inner_type_expression_under_wrapper, explicit_type_parameter_environment, trait_declaration_names, nominal_type_declaration_names, synthetic_counter);
  if (output_outer_is_shared_pointer_type)   {
    return TraitExpandChunk{std::make_shared<ast::TypeExpr>(ast::TyShared{inner_expansion_chunk.type_expression}), inner_expansion_chunk.next_counter, inner_expansion_chunk.appended_type_parameter_names, inner_expansion_chunk.appended_trait_bounds_rows};
  } else   {
    return TraitExpandChunk{std::make_shared<ast::TypeExpr>(ast::TyArray{inner_expansion_chunk.type_expression}), inner_expansion_chunk.next_counter, inner_expansion_chunk.appended_type_parameter_names, inner_expansion_chunk.appended_trait_bounds_rows};
  }
}
TraitExpandChunk expand_type_expression_for_trait_param(std::shared_ptr<ast::TypeExpr> type_expression, mlc::HashMap<mlc::String, bool> explicit_type_parameter_environment, mlc::HashMap<mlc::String, bool> trait_declaration_names, mlc::HashMap<mlc::String, bool> nominal_type_declaration_names, int synthetic_counter) noexcept{
  return std::visit(overloaded{[&](const ast::TyNamed& tyNamed) -> TraitExpandChunk { auto [name] = tyNamed; return (explicit_type_parameter_environment.has(name) ? (TraitExpandChunk{type_expression, synthetic_counter, {}, {}}) : ([&]() -> TraitExpandChunk {
  if ((trait_declaration_names.has(name) && (!nominal_type_declaration_names.has(name))))   {
    return [&]() {
auto synthetic_name = ((mlc::String("__trait_param_", 14) + mlc::to_string(mlc::to_string(synthetic_counter))) + mlc::String("", 0));
auto single_trait_row = mlc::Array<mlc::String>{name};
auto bounds_wrapper = mlc::Array<mlc::Array<mlc::String>>{single_trait_row};
return TraitExpandChunk{std::make_shared<ast::TypeExpr>(ast::TyNamed{synthetic_name}), mlc::arith::checked_add(synthetic_counter, 1), mlc::Array<mlc::String>{synthetic_name}, bounds_wrapper};
}();
  } else   {
    return TraitExpandChunk{type_expression, synthetic_counter, {}, {}};
  }
}())); },
[&](const ast::TyArray& tyArray) -> TraitExpandChunk { auto [inner_under_array] = tyArray; return expand_type_expression_under_array_or_shared_wrapper(inner_under_array, explicit_type_parameter_environment, trait_declaration_names, nominal_type_declaration_names, synthetic_counter, false); },
[&](const ast::TyShared& tyShared) -> TraitExpandChunk { auto [inner_under_shared] = tyShared; return expand_type_expression_under_array_or_shared_wrapper(inner_under_shared, explicit_type_parameter_environment, trait_declaration_names, nominal_type_declaration_names, synthetic_counter, true); },
[&](const ast::TyGeneric& tyGeneric) -> TraitExpandChunk { auto [base_name, type_arguments] = tyGeneric; return [&]() {
auto after_generic_arguments = type_arguments.fold(TypeArgumentAccumulationForGeneric{synthetic_counter, {}, {}, {}}, [=](TypeArgumentAccumulationForGeneric accumulation_before_step, std::shared_ptr<ast::TypeExpr> generic_argument_under_step) mutable { return [&]() {
auto expanded_argument_under_generic = expand_type_expression_for_trait_param(generic_argument_under_step, explicit_type_parameter_environment, trait_declaration_names, nominal_type_declaration_names, accumulation_before_step.synthetic_counter_under_generic);
return TypeArgumentAccumulationForGeneric{expanded_argument_under_generic.next_counter, accumulation_before_step.rebuilt_generic_type_arguments_so_far.concat(mlc::Array<std::shared_ptr<ast::TypeExpr>>{expanded_argument_under_generic.type_expression}), accumulation_before_step.combined_appended_type_parameter_names_so_far.concat(expanded_argument_under_generic.appended_type_parameter_names), accumulation_before_step.combined_trait_bounds_rows_so_far.concat(expanded_argument_under_generic.appended_trait_bounds_rows)};
}(); });
return TraitExpandChunk{std::make_shared<ast::TypeExpr>(ast::TyGeneric{base_name, after_generic_arguments.rebuilt_generic_type_arguments_so_far}), after_generic_arguments.synthetic_counter_under_generic, after_generic_arguments.combined_appended_type_parameter_names_so_far, after_generic_arguments.combined_trait_bounds_rows_so_far};
}(); },
[&](const ast::TyFn& tyFn) -> TraitExpandChunk { auto [parameter_types, return_type_expression] = tyFn; return [&]() {
auto rebuilt_after_parameters_under_function_type = parameter_types.fold(FunctionParameterAccumulationUnderTyFn{synthetic_counter, {}, {}, {}}, [=](FunctionParameterAccumulationUnderTyFn accumulation_before_parameter_step, std::shared_ptr<ast::TypeExpr> function_parameter_type_under_step) mutable { return [&]() {
auto expanded_function_parameter_under_step = expand_type_expression_for_trait_param(function_parameter_type_under_step, explicit_type_parameter_environment, trait_declaration_names, nominal_type_declaration_names, accumulation_before_parameter_step.synthetic_counter_under_function_type);
return FunctionParameterAccumulationUnderTyFn{expanded_function_parameter_under_step.next_counter, accumulation_before_parameter_step.rebuilt_function_parameter_types_so_far.concat(mlc::Array<std::shared_ptr<ast::TypeExpr>>{expanded_function_parameter_under_step.type_expression}), accumulation_before_parameter_step.combined_appended_type_parameter_names_under_function_type.concat(expanded_function_parameter_under_step.appended_type_parameter_names), accumulation_before_parameter_step.combined_trait_bounds_rows_under_function_type.concat(expanded_function_parameter_under_step.appended_trait_bounds_rows)};
}(); });
auto return_chunk_after_function_parameter_expansion = expand_type_expression_for_trait_param(return_type_expression, explicit_type_parameter_environment, trait_declaration_names, nominal_type_declaration_names, rebuilt_after_parameters_under_function_type.synthetic_counter_under_function_type);
return TraitExpandChunk{std::make_shared<ast::TypeExpr>(ast::TyFn{rebuilt_after_parameters_under_function_type.rebuilt_function_parameter_types_so_far, return_chunk_after_function_parameter_expansion.type_expression}), return_chunk_after_function_parameter_expansion.next_counter, rebuilt_after_parameters_under_function_type.combined_appended_type_parameter_names_under_function_type.concat(return_chunk_after_function_parameter_expansion.appended_type_parameter_names), rebuilt_after_parameters_under_function_type.combined_trait_bounds_rows_under_function_type.concat(return_chunk_after_function_parameter_expansion.appended_trait_bounds_rows)};
}(); },
[&](const ast::TyAssoc& tyAssoc) -> TraitExpandChunk { auto [__0, __1] = tyAssoc; return TraitExpandChunk{type_expression, synthetic_counter, {}, {}}; },
[&](const ast::TyI32& tyI32) -> TraitExpandChunk { return TraitExpandChunk{type_expression, synthetic_counter, {}, {}}; },
[&](const ast::TyString& tyString) -> TraitExpandChunk { return TraitExpandChunk{type_expression, synthetic_counter, {}, {}}; },
[&](const ast::TyBool& tyBool) -> TraitExpandChunk { return TraitExpandChunk{type_expression, synthetic_counter, {}, {}}; },
[&](const ast::TyUnit& tyUnit) -> TraitExpandChunk { return TraitExpandChunk{type_expression, synthetic_counter, {}, {}}; }
}, (*type_expression));
}
std::shared_ptr<ast::Decl> expand_decl_fn_trait_parameters(mlc::String function_name, mlc::Array<mlc::String> type_parameter_names, mlc::Array<mlc::Array<mlc::String>> trait_bounds_rows, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<ast::TypeExpr> return_type_expression, std::shared_ptr<ast::Expr> body_expression, mlc::Array<ast::WhereClauseBound> where_clause_bounds_entries, mlc::HashMap<mlc::String, bool> trait_declaration_names, mlc::HashMap<mlc::String, bool> nominal_type_declaration_names) noexcept{
  auto explicit_type_parameter_environment = mlc::HashMap<mlc::String, bool>();
  type_parameter_names.fold(0, [=](int _, mlc::String type_parameter_single_name) mutable { return [&]() {
explicit_type_parameter_environment.set(type_parameter_single_name, true);
return 0;
}(); });
  auto initial_expanded_function_type_parameter_names = type_parameter_names.concat({});
  auto after_parameter_list_expanded = parameters.fold(ExpandedFunctionParametersTraversalState{0, initial_expanded_function_type_parameter_names, align_trait_bounds_matrix(type_parameter_names, trait_bounds_rows), {}}, [=](ExpandedFunctionParametersTraversalState expanded_state_before_traversal_step, std::shared_ptr<ast::Param> parameter_under_traversal_shared) mutable { return [&]() {
auto parameter_traversal_expression_chunk = expand_type_expression_for_trait_param(ast::param_type_value(parameter_under_traversal_shared), explicit_type_parameter_environment, trait_declaration_names, nominal_type_declaration_names, expanded_state_before_traversal_step.synthetic_counter);
return ExpandedFunctionParametersTraversalState{parameter_traversal_expression_chunk.next_counter, expanded_state_before_traversal_step.expanded_type_parameter_names.concat(parameter_traversal_expression_chunk.appended_type_parameter_names), expanded_state_before_traversal_step.expanded_trait_bounds.concat(parameter_traversal_expression_chunk.appended_trait_bounds_rows), expanded_state_before_traversal_step.expanded_parameters.concat(mlc::Array<std::shared_ptr<ast::Param>>{std::make_shared<ast::Param>(ast::Param{ast::param_name(parameter_under_traversal_shared), parameter_under_traversal_shared->is_mut, parameter_traversal_expression_chunk.type_expression, parameter_under_traversal_shared->has_default, parameter_under_traversal_shared->default_, parameter_under_traversal_shared->param_pattern})})};
}(); });
  auto return_chunk_under_function_declaration = expand_type_expression_for_trait_param(return_type_expression, explicit_type_parameter_environment, trait_declaration_names, nominal_type_declaration_names, after_parameter_list_expanded.synthetic_counter);
  return std::make_shared<ast::Decl>(ast::DeclFn{function_name, after_parameter_list_expanded.expanded_type_parameter_names.concat(return_chunk_under_function_declaration.appended_type_parameter_names), after_parameter_list_expanded.expanded_trait_bounds.concat(return_chunk_under_function_declaration.appended_trait_bounds_rows), after_parameter_list_expanded.expanded_parameters, return_chunk_under_function_declaration.type_expression, body_expression, where_clause_bounds_entries});
}
mlc::Array<std::shared_ptr<ast::Decl>> expand_extend_methods(mlc::Array<std::shared_ptr<ast::Decl>> methods, mlc::HashMap<mlc::String, bool> trait_declaration_names, mlc::HashMap<mlc::String, bool> nominal_type_declaration_names) noexcept{
  return methods.map([=](std::shared_ptr<ast::Decl> method_declaration_under_extend) mutable { return expand_decl_shared(method_declaration_under_extend, trait_declaration_names, nominal_type_declaration_names); });
}
std::shared_ptr<ast::Decl> expand_decl_shared(std::shared_ptr<ast::Decl> declaration, mlc::HashMap<mlc::String, bool> trait_declaration_names, mlc::HashMap<mlc::String, bool> nominal_type_declaration_names) noexcept{
  return [&]() -> std::shared_ptr<ast::Decl> {
auto __match_subject = declaration;
if (std::holds_alternative<ast::DeclFn>((*__match_subject))) {
const ast::DeclFn& declFn = std::get<ast::DeclFn>((*__match_subject));
auto [function_name, type_parameter_names, trait_bounds_rows, parameters, return_type_expression, body_expression, where_clause_bounds_entries] = declFn; return expand_decl_fn_trait_parameters(function_name, type_parameter_names, trait_bounds_rows, parameters, return_type_expression, body_expression, where_clause_bounds_entries, trait_declaration_names, nominal_type_declaration_names);
}
if (std::holds_alternative<ast::DeclExported>((*__match_subject))) {
const ast::DeclExported& declExported = std::get<ast::DeclExported>((*__match_subject));
auto [exported_declaration] = declExported; return std::make_shared<ast::Decl>(ast::DeclExported{expand_decl_shared(exported_declaration, trait_declaration_names, nominal_type_declaration_names)});
}
if (std::holds_alternative<ast::DeclExtend>((*__match_subject))) {
const ast::DeclExtend& declExtend = std::get<ast::DeclExtend>((*__match_subject));
auto [extended_type_name, implemented_trait_name, methods, name_span] = declExtend; return std::make_shared<ast::Decl>(ast::DeclExtend{extended_type_name, implemented_trait_name, expand_extend_methods(methods, trait_declaration_names, nominal_type_declaration_names), name_span});
}
return declaration;
std::abort();
}();
}
mlc::Array<std::shared_ptr<ast::Decl>> expand_declarations_with_trait_and_nominal_maps(mlc::Array<std::shared_ptr<ast::Decl>> declarations, mlc::HashMap<mlc::String, bool> trait_declaration_names, mlc::HashMap<mlc::String, bool> nominal_type_declaration_names) noexcept{
  return declarations.map([=](std::shared_ptr<ast::Decl> declaration_under_expansion_candidate) mutable { return expand_decl_shared(declaration_under_expansion_candidate, trait_declaration_names, nominal_type_declaration_names); });
}
Trait_nominal_maps_fold_state trait_nominal_maps_fold_step(Trait_nominal_maps_fold_state fold_state, std::shared_ptr<ast::Decl> current_declaration) noexcept{
  return [&]() -> Trait_nominal_maps_fold_state {
auto __match_subject = current_declaration;
if (std::holds_alternative<ast::DeclType>((*__match_subject))) {
const ast::DeclType& declType = std::get<ast::DeclType>((*__match_subject));
auto [type_name, __1, __2, __3, __4] = declType; return [&]() {
auto nominal_names = fold_state.nominal_type_declaration_names;
nominal_names.set(type_name, true);
return Trait_nominal_maps_fold_state{fold_state.trait_declaration_names, nominal_names};
}();
}
if (std::holds_alternative<ast::DeclTypeAlias>((*__match_subject))) {
const ast::DeclTypeAlias& declTypeAlias = std::get<ast::DeclTypeAlias>((*__match_subject));
auto [type_name, __1, __2, __3] = declTypeAlias; return [&]() {
auto nominal_names = fold_state.nominal_type_declaration_names;
nominal_names.set(type_name, true);
return Trait_nominal_maps_fold_state{fold_state.trait_declaration_names, nominal_names};
}();
}
if (std::holds_alternative<ast::DeclTrait>((*__match_subject))) {
const ast::DeclTrait& declTrait = std::get<ast::DeclTrait>((*__match_subject));
auto [trait_name, __1, __2, __3] = declTrait; return [&]() {
auto trait_names = fold_state.trait_declaration_names;
trait_names.set(trait_name, true);
return Trait_nominal_maps_fold_state{trait_names, fold_state.nominal_type_declaration_names};
}();
}
return fold_state;
std::abort();
}();
}
TraitNominalMaps build_trait_nominal_maps(ast::Program program) noexcept{
  auto initial_fold_state = Trait_nominal_maps_fold_state{{}, {}};
  auto final_fold_state = program.decls.fold(initial_fold_state, [=](Trait_nominal_maps_fold_state fold_state_so_far, std::shared_ptr<ast::Decl> root_declaration) mutable { return trait_nominal_maps_fold_step(fold_state_so_far, declaration_without_export_wrappers(root_declaration)); });
  return TraitNominalMaps{final_fold_state.trait_declaration_names, final_fold_state.nominal_type_declaration_names};
}
mlc::Array<std::shared_ptr<ast::Decl>> expand_declarations_with_trait_nominal_maps(mlc::Array<std::shared_ptr<ast::Decl>> declarations, TraitNominalMaps maps) noexcept{
  return expand_declarations_with_trait_and_nominal_maps(declarations, maps.trait_declaration_names, maps.nominal_type_declaration_names);
}
mlc::Array<std::shared_ptr<ast::Decl>> expand_declarations_with_trait_context(mlc::Array<std::shared_ptr<ast::Decl>> declarations, ast::Program program_for_maps) noexcept{
  return expand_declarations_with_trait_nominal_maps(declarations, build_trait_nominal_maps(program_for_maps));
}
ast::Program expand_trait_as_param_entry_using_full(ast::Program entry, ast::Program full_program) noexcept{
  return ast::Program{expand_declarations_with_trait_context(entry.decls, full_program)};
}
ast::Program expand_trait_as_param_program(ast::Program program) noexcept{
  return ast::Program{expand_declarations_with_trait_context(program.decls, program)};
}

} // namespace trait_param_expand
