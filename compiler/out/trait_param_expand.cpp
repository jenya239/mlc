#include "trait_param_expand.hpp"

#include "ast.hpp"
#include "diagnostic_codes.hpp"

namespace trait_param_expand {

using namespace ast;
using namespace diagnostic_codes;
using namespace ast_tokens;

struct Trait_name_conflict_fold_state {mlc::HashMap<mlc::String, bool> trait_declaration_names;mlc::HashMap<mlc::String, bool> nominal_type_declaration_names;mlc::Array<ast::Diagnostic> conflicts;};

struct TraitExpandChunk {std::shared_ptr<ast::TypeExpr> type_expression;int next_counter;mlc::Array<mlc::String> appended_type_parameter_names;mlc::Array<mlc::Array<mlc::String>> appended_trait_bounds_rows;};

struct TypeArgumentAccumulationForGeneric {int synthetic_counter_under_generic;mlc::Array<std::shared_ptr<ast::TypeExpr>> rebuilt_generic_type_arguments_so_far;mlc::Array<mlc::String> combined_appended_type_parameter_names_so_far;mlc::Array<mlc::Array<mlc::String>> combined_trait_bounds_rows_so_far;};

struct FunctionParameterAccumulationUnderTyFn {int synthetic_counter_under_function_type;mlc::Array<std::shared_ptr<ast::TypeExpr>> rebuilt_function_parameter_types_so_far;mlc::Array<mlc::String> combined_appended_type_parameter_names_under_function_type;mlc::Array<mlc::Array<mlc::String>> combined_trait_bounds_rows_under_function_type;};

struct ExpandedFunctionParametersTraversalState {int synthetic_counter;mlc::Array<mlc::String> expanded_type_parameter_names;mlc::Array<mlc::Array<mlc::String>> expanded_trait_bounds;mlc::Array<std::shared_ptr<ast::Param>> expanded_parameters;};

struct Trait_nominal_maps_fold_state {mlc::HashMap<mlc::String, bool> trait_declaration_names;mlc::HashMap<mlc::String, bool> nominal_type_declaration_names;};

std::shared_ptr<ast::Decl> declaration_without_export_wrappers(std::shared_ptr<ast::Decl> declaration) noexcept;

trait_param_expand::Trait_name_conflict_fold_state trait_name_conflict_fold_step(trait_param_expand::Trait_name_conflict_fold_state fold_state, std::shared_ptr<ast::Decl> current_declaration) noexcept;

mlc::Array<ast::Diagnostic> trait_and_type_name_conflict_diagnostics(ast::Program program) noexcept;

mlc::Array<mlc::Array<mlc::String>> align_trait_bounds_matrix(mlc::Array<mlc::String> type_parameter_names, mlc::Array<mlc::Array<mlc::String>> trait_bounds_rows) noexcept;

trait_param_expand::TraitExpandChunk expand_type_expression_under_array_or_shared_wrapper(std::shared_ptr<ast::TypeExpr> inner_type_expression_under_wrapper, mlc::HashMap<mlc::String, bool> explicit_type_parameter_environment, mlc::HashMap<mlc::String, bool> trait_declaration_names, mlc::HashMap<mlc::String, bool> nominal_type_declaration_names, int synthetic_counter, bool output_outer_is_shared_pointer_type) noexcept;

trait_param_expand::TraitExpandChunk expand_type_expression_for_trait_param(std::shared_ptr<ast::TypeExpr> type_expression, mlc::HashMap<mlc::String, bool> explicit_type_parameter_environment, mlc::HashMap<mlc::String, bool> trait_declaration_names, mlc::HashMap<mlc::String, bool> nominal_type_declaration_names, int synthetic_counter) noexcept;

std::shared_ptr<ast::Decl> expand_decl_fn_trait_parameters(mlc::String function_name, mlc::Array<mlc::String> type_parameter_names, mlc::Array<mlc::Array<mlc::String>> trait_bounds_rows, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<ast::TypeExpr> return_type_expression, std::shared_ptr<ast::Expr> body_expression, mlc::Array<ast::WhereClauseBound> where_clause_bounds_entries, mlc::HashMap<mlc::String, bool> trait_declaration_names, mlc::HashMap<mlc::String, bool> nominal_type_declaration_names) noexcept;

mlc::Array<std::shared_ptr<ast::Decl>> expand_extend_methods(mlc::Array<std::shared_ptr<ast::Decl>> methods, mlc::HashMap<mlc::String, bool> trait_declaration_names, mlc::HashMap<mlc::String, bool> nominal_type_declaration_names) noexcept;

std::shared_ptr<ast::Decl> expand_decl_shared(std::shared_ptr<ast::Decl> declaration, mlc::HashMap<mlc::String, bool> trait_declaration_names, mlc::HashMap<mlc::String, bool> nominal_type_declaration_names) noexcept;

mlc::Array<std::shared_ptr<ast::Decl>> expand_declarations_with_trait_and_nominal_maps(mlc::Array<std::shared_ptr<ast::Decl>> declarations, mlc::HashMap<mlc::String, bool> trait_declaration_names, mlc::HashMap<mlc::String, bool> nominal_type_declaration_names) noexcept;

trait_param_expand::Trait_nominal_maps_fold_state trait_nominal_maps_fold_step(trait_param_expand::Trait_nominal_maps_fold_state fold_state, std::shared_ptr<ast::Decl> current_declaration) noexcept;

trait_param_expand::TraitNominalMaps build_trait_nominal_maps(ast::Program program) noexcept;

mlc::Array<std::shared_ptr<ast::Decl>> expand_declarations_with_trait_nominal_maps(mlc::Array<std::shared_ptr<ast::Decl>> declarations, trait_param_expand::TraitNominalMaps maps) noexcept;

mlc::Array<std::shared_ptr<ast::Decl>> expand_declarations_with_trait_context(mlc::Array<std::shared_ptr<ast::Decl>> declarations, ast::Program program_for_maps) noexcept;

ast::Program expand_trait_as_param_entry_using_full(ast::Program entry, ast::Program full_program) noexcept;

ast::Program expand_trait_as_param_program(ast::Program program) noexcept;

std::shared_ptr<ast::Decl> declaration_without_export_wrappers(std::shared_ptr<ast::Decl> declaration) noexcept{return [&]() -> std::shared_ptr<ast::Decl> { if (std::holds_alternative<ast::DeclExported>((*declaration))) { auto _v_declexported = std::get<ast::DeclExported>((*declaration)); auto [exported_declaration] = _v_declexported; return declaration_without_export_wrappers(exported_declaration); } return declaration; }();}

trait_param_expand::Trait_name_conflict_fold_state trait_name_conflict_fold_step(trait_param_expand::Trait_name_conflict_fold_state fold_state, std::shared_ptr<ast::Decl> current_declaration) noexcept{
return [&]() -> trait_param_expand::Trait_name_conflict_fold_state { if (std::holds_alternative<ast::DeclType>((*current_declaration))) { auto _v_decltype = std::get<ast::DeclType>((*current_declaration)); auto [type_name, _w0, _w1, _w2, _w3] = _v_decltype; return [&]() -> trait_param_expand::Trait_name_conflict_fold_state { 
  mlc::Array<ast::Diagnostic> conflicts_after_type = fold_state.trait_declaration_names.has(type_name) ? fold_state.conflicts.concat(mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("the name \"") + type_name + mlc::String("\" is declared as both a type and a trait"), ast::decl_name_span(current_declaration), diagnostic_codes::diagnostic_code_e073())}) : fold_state.conflicts;
  return trait_param_expand::Trait_name_conflict_fold_state{fold_state.trait_declaration_names, [&]() -> mlc::HashMap<mlc::String, bool> { 
  mlc::HashMap<mlc::String, bool> nominal_names = fold_state.nominal_type_declaration_names;
  nominal_names.set(type_name, true);
  return nominal_names;
 }(), conflicts_after_type};
 }(); } if (std::holds_alternative<ast::DeclTypeAlias>((*current_declaration))) { auto _v_decltypealias = std::get<ast::DeclTypeAlias>((*current_declaration)); auto [type_name, _w0, _w1, _w2] = _v_decltypealias; return [&]() -> trait_param_expand::Trait_name_conflict_fold_state { 
  mlc::Array<ast::Diagnostic> conflicts_after_alias = fold_state.trait_declaration_names.has(type_name) ? fold_state.conflicts.concat(mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("the name \"") + type_name + mlc::String("\" is declared as both a type and a trait"), ast::decl_name_span(current_declaration), diagnostic_codes::diagnostic_code_e073())}) : fold_state.conflicts;
  return trait_param_expand::Trait_name_conflict_fold_state{fold_state.trait_declaration_names, [&]() -> mlc::HashMap<mlc::String, bool> { 
  mlc::HashMap<mlc::String, bool> nominal_names = fold_state.nominal_type_declaration_names;
  nominal_names.set(type_name, true);
  return nominal_names;
 }(), conflicts_after_alias};
 }(); } if (std::holds_alternative<ast::DeclTrait>((*current_declaration))) { auto _v_decltrait = std::get<ast::DeclTrait>((*current_declaration)); auto [trait_name, _w0, _w1, _w2] = _v_decltrait; return [&]() -> trait_param_expand::Trait_name_conflict_fold_state { 
  mlc::Array<ast::Diagnostic> conflicts_after_trait = fold_state.nominal_type_declaration_names.has(trait_name) ? fold_state.conflicts.concat(mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("the name \"") + trait_name + mlc::String("\" is declared as both a type and a trait"), ast::decl_name_span(current_declaration), diagnostic_codes::diagnostic_code_e073())}) : fold_state.conflicts;
  return trait_param_expand::Trait_name_conflict_fold_state{[&]() -> mlc::HashMap<mlc::String, bool> { 
  mlc::HashMap<mlc::String, bool> trait_names = fold_state.trait_declaration_names;
  trait_names.set(trait_name, true);
  return trait_names;
 }(), fold_state.nominal_type_declaration_names, conflicts_after_trait};
 }(); } return fold_state; }();
}

mlc::Array<ast::Diagnostic> trait_and_type_name_conflict_diagnostics(ast::Program program) noexcept{
trait_param_expand::Trait_name_conflict_fold_state initial_fold_state = trait_param_expand::Trait_name_conflict_fold_state{mlc::HashMap<mlc::String, bool>(), mlc::HashMap<mlc::String, bool>(), [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> empty_conflicts = {};
  return empty_conflicts;
 }()};
trait_param_expand::Trait_name_conflict_fold_state final_fold_state = program.decls.fold(initial_fold_state, [](trait_param_expand::Trait_name_conflict_fold_state fold_state_so_far, std::shared_ptr<ast::Decl> root_declaration) mutable { return trait_name_conflict_fold_step(fold_state_so_far, declaration_without_export_wrappers(root_declaration)); });
return final_fold_state.conflicts;
}

mlc::Array<mlc::Array<mlc::String>> align_trait_bounds_matrix(mlc::Array<mlc::String> type_parameter_names, mlc::Array<mlc::Array<mlc::String>> trait_bounds_rows) noexcept{
mlc::Array<mlc::Array<mlc::String>> copied_trait_bound_rows = trait_bounds_rows.concat({});
mlc::Array<mlc::String> empty_trait_bound_row = {};
return type_parameter_names.drop(copied_trait_bound_rows.size()).fold(copied_trait_bound_rows, [empty_trait_bound_row](mlc::Array<mlc::Array<mlc::String>> accumulated_aligned_matrix, mlc::String _unused_type_parameter_padding_key) mutable { return accumulated_aligned_matrix.concat(mlc::Array<mlc::Array<mlc::String>>{empty_trait_bound_row}); });
}

trait_param_expand::TraitExpandChunk expand_type_expression_under_array_or_shared_wrapper(std::shared_ptr<ast::TypeExpr> inner_type_expression_under_wrapper, mlc::HashMap<mlc::String, bool> explicit_type_parameter_environment, mlc::HashMap<mlc::String, bool> trait_declaration_names, mlc::HashMap<mlc::String, bool> nominal_type_declaration_names, int synthetic_counter, bool output_outer_is_shared_pointer_type) noexcept{
trait_param_expand::TraitExpandChunk inner_expansion_chunk = expand_type_expression_for_trait_param(inner_type_expression_under_wrapper, explicit_type_parameter_environment, trait_declaration_names, nominal_type_declaration_names, synthetic_counter);
return output_outer_is_shared_pointer_type ? trait_param_expand::TraitExpandChunk{std::make_shared<ast::TypeExpr>(ast::TyShared(inner_expansion_chunk.type_expression)), inner_expansion_chunk.next_counter, inner_expansion_chunk.appended_type_parameter_names, inner_expansion_chunk.appended_trait_bounds_rows} : trait_param_expand::TraitExpandChunk{std::make_shared<ast::TypeExpr>(ast::TyArray(inner_expansion_chunk.type_expression)), inner_expansion_chunk.next_counter, inner_expansion_chunk.appended_type_parameter_names, inner_expansion_chunk.appended_trait_bounds_rows};
}

trait_param_expand::TraitExpandChunk expand_type_expression_for_trait_param(std::shared_ptr<ast::TypeExpr> type_expression, mlc::HashMap<mlc::String, bool> explicit_type_parameter_environment, mlc::HashMap<mlc::String, bool> trait_declaration_names, mlc::HashMap<mlc::String, bool> nominal_type_declaration_names, int synthetic_counter) noexcept{return std::visit(overloaded{
  [&](const TyNamed& tynamed) -> trait_param_expand::TraitExpandChunk { auto [name] = tynamed; return explicit_type_parameter_environment.has(name) ? trait_param_expand::TraitExpandChunk{type_expression, synthetic_counter, {}, {}} : trait_declaration_names.has(name) && !nominal_type_declaration_names.has(name) ? [&]() -> trait_param_expand::TraitExpandChunk { 
  mlc::String synthetic_name = mlc::String("__trait_param_") + mlc::to_string(synthetic_counter);
  mlc::Array<mlc::String> single_trait_row = mlc::Array<mlc::String>{name};
  mlc::Array<mlc::Array<mlc::String>> bounds_wrapper = mlc::Array<mlc::Array<mlc::String>>{single_trait_row};
  return trait_param_expand::TraitExpandChunk{std::make_shared<ast::TypeExpr>(ast::TyNamed(synthetic_name)), synthetic_counter + 1, mlc::Array<mlc::String>{synthetic_name}, bounds_wrapper};
 }() : trait_param_expand::TraitExpandChunk{type_expression, synthetic_counter, {}, {}}; },
  [&](const TyArray& tyarray) -> trait_param_expand::TraitExpandChunk { auto [inner_under_array] = tyarray; return expand_type_expression_under_array_or_shared_wrapper(inner_under_array, explicit_type_parameter_environment, trait_declaration_names, nominal_type_declaration_names, synthetic_counter, false); },
  [&](const TyShared& tyshared) -> trait_param_expand::TraitExpandChunk { auto [inner_under_shared] = tyshared; return expand_type_expression_under_array_or_shared_wrapper(inner_under_shared, explicit_type_parameter_environment, trait_declaration_names, nominal_type_declaration_names, synthetic_counter, true); },
  [&](const TyGeneric& tygeneric) -> trait_param_expand::TraitExpandChunk { auto [base_name, type_arguments] = tygeneric; return [&]() -> trait_param_expand::TraitExpandChunk { 
  trait_param_expand::TypeArgumentAccumulationForGeneric after_generic_arguments = type_arguments.fold(trait_param_expand::TypeArgumentAccumulationForGeneric{synthetic_counter, {}, {}, {}}, [explicit_type_parameter_environment, trait_declaration_names, nominal_type_declaration_names](trait_param_expand::TypeArgumentAccumulationForGeneric accumulation_before_step, std::shared_ptr<ast::TypeExpr> generic_argument_under_step) mutable { return [&]() -> trait_param_expand::TypeArgumentAccumulationForGeneric { 
  trait_param_expand::TraitExpandChunk expanded_argument_under_generic = expand_type_expression_for_trait_param(generic_argument_under_step, explicit_type_parameter_environment, trait_declaration_names, nominal_type_declaration_names, accumulation_before_step.synthetic_counter_under_generic);
  return trait_param_expand::TypeArgumentAccumulationForGeneric{expanded_argument_under_generic.next_counter, accumulation_before_step.rebuilt_generic_type_arguments_so_far.concat(mlc::Array<std::shared_ptr<ast::TypeExpr>>{expanded_argument_under_generic.type_expression}), accumulation_before_step.combined_appended_type_parameter_names_so_far.concat(expanded_argument_under_generic.appended_type_parameter_names), accumulation_before_step.combined_trait_bounds_rows_so_far.concat(expanded_argument_under_generic.appended_trait_bounds_rows)};
 }(); });
  return trait_param_expand::TraitExpandChunk{std::make_shared<ast::TypeExpr>(ast::TyGeneric(base_name, after_generic_arguments.rebuilt_generic_type_arguments_so_far)), after_generic_arguments.synthetic_counter_under_generic, after_generic_arguments.combined_appended_type_parameter_names_so_far, after_generic_arguments.combined_trait_bounds_rows_so_far};
 }(); },
  [&](const TyFn& tyfn) -> trait_param_expand::TraitExpandChunk { auto [parameter_types, return_type_expression] = tyfn; return [&]() -> trait_param_expand::TraitExpandChunk { 
  trait_param_expand::FunctionParameterAccumulationUnderTyFn rebuilt_after_parameters_under_function_type = parameter_types.fold(trait_param_expand::FunctionParameterAccumulationUnderTyFn{synthetic_counter, {}, {}, {}}, [explicit_type_parameter_environment, trait_declaration_names, nominal_type_declaration_names](trait_param_expand::FunctionParameterAccumulationUnderTyFn accumulation_before_parameter_step, std::shared_ptr<ast::TypeExpr> function_parameter_type_under_step) mutable { return [&]() -> trait_param_expand::FunctionParameterAccumulationUnderTyFn { 
  trait_param_expand::TraitExpandChunk expanded_function_parameter_under_step = expand_type_expression_for_trait_param(function_parameter_type_under_step, explicit_type_parameter_environment, trait_declaration_names, nominal_type_declaration_names, accumulation_before_parameter_step.synthetic_counter_under_function_type);
  return trait_param_expand::FunctionParameterAccumulationUnderTyFn{expanded_function_parameter_under_step.next_counter, accumulation_before_parameter_step.rebuilt_function_parameter_types_so_far.concat(mlc::Array<std::shared_ptr<ast::TypeExpr>>{expanded_function_parameter_under_step.type_expression}), accumulation_before_parameter_step.combined_appended_type_parameter_names_under_function_type.concat(expanded_function_parameter_under_step.appended_type_parameter_names), accumulation_before_parameter_step.combined_trait_bounds_rows_under_function_type.concat(expanded_function_parameter_under_step.appended_trait_bounds_rows)};
 }(); });
  trait_param_expand::TraitExpandChunk return_chunk_after_function_parameter_expansion = expand_type_expression_for_trait_param(return_type_expression, explicit_type_parameter_environment, trait_declaration_names, nominal_type_declaration_names, rebuilt_after_parameters_under_function_type.synthetic_counter_under_function_type);
  return trait_param_expand::TraitExpandChunk{std::make_shared<ast::TypeExpr>(ast::TyFn(rebuilt_after_parameters_under_function_type.rebuilt_function_parameter_types_so_far, return_chunk_after_function_parameter_expansion.type_expression)), return_chunk_after_function_parameter_expansion.next_counter, rebuilt_after_parameters_under_function_type.combined_appended_type_parameter_names_under_function_type.concat(return_chunk_after_function_parameter_expansion.appended_type_parameter_names), rebuilt_after_parameters_under_function_type.combined_trait_bounds_rows_under_function_type.concat(return_chunk_after_function_parameter_expansion.appended_trait_bounds_rows)};
 }(); },
  [&](const TyAssoc& tyassoc) -> trait_param_expand::TraitExpandChunk { auto [_w0, _w1] = tyassoc; return trait_param_expand::TraitExpandChunk{type_expression, synthetic_counter, {}, {}}; },
  [&](const TyI32& tyi32) -> trait_param_expand::TraitExpandChunk { return trait_param_expand::TraitExpandChunk{type_expression, synthetic_counter, {}, {}}; },
  [&](const TyString& tystring) -> trait_param_expand::TraitExpandChunk { return trait_param_expand::TraitExpandChunk{type_expression, synthetic_counter, {}, {}}; },
  [&](const TyBool& tybool) -> trait_param_expand::TraitExpandChunk { return trait_param_expand::TraitExpandChunk{type_expression, synthetic_counter, {}, {}}; },
  [&](const TyUnit& tyunit) -> trait_param_expand::TraitExpandChunk { return trait_param_expand::TraitExpandChunk{type_expression, synthetic_counter, {}, {}}; }
}, (*type_expression));}

std::shared_ptr<ast::Decl> expand_decl_fn_trait_parameters(mlc::String function_name, mlc::Array<mlc::String> type_parameter_names, mlc::Array<mlc::Array<mlc::String>> trait_bounds_rows, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<ast::TypeExpr> return_type_expression, std::shared_ptr<ast::Expr> body_expression, mlc::Array<ast::WhereClauseBound> where_clause_bounds_entries, mlc::HashMap<mlc::String, bool> trait_declaration_names, mlc::HashMap<mlc::String, bool> nominal_type_declaration_names) noexcept{
mlc::HashMap<mlc::String, bool> explicit_type_parameter_environment = mlc::HashMap<mlc::String, bool>();
type_parameter_names.fold(0, [&explicit_type_parameter_environment](int _, mlc::String type_parameter_single_name) mutable { return [&]() -> int { 
  explicit_type_parameter_environment.set(type_parameter_single_name, true);
  return 0;
 }(); });
mlc::Array<mlc::String> initial_expanded_function_type_parameter_names = type_parameter_names.concat({});
trait_param_expand::ExpandedFunctionParametersTraversalState after_parameter_list_expanded = parameters.fold(trait_param_expand::ExpandedFunctionParametersTraversalState{0, initial_expanded_function_type_parameter_names, align_trait_bounds_matrix(type_parameter_names, trait_bounds_rows), {}}, [explicit_type_parameter_environment, trait_declaration_names, nominal_type_declaration_names](trait_param_expand::ExpandedFunctionParametersTraversalState expanded_state_before_traversal_step, std::shared_ptr<ast::Param> parameter_under_traversal_shared) mutable { return [&]() -> trait_param_expand::ExpandedFunctionParametersTraversalState { 
  trait_param_expand::TraitExpandChunk parameter_traversal_expression_chunk = expand_type_expression_for_trait_param(ast::param_type_value(parameter_under_traversal_shared), explicit_type_parameter_environment, trait_declaration_names, nominal_type_declaration_names, expanded_state_before_traversal_step.synthetic_counter);
  return trait_param_expand::ExpandedFunctionParametersTraversalState{parameter_traversal_expression_chunk.next_counter, expanded_state_before_traversal_step.expanded_type_parameter_names.concat(parameter_traversal_expression_chunk.appended_type_parameter_names), expanded_state_before_traversal_step.expanded_trait_bounds.concat(parameter_traversal_expression_chunk.appended_trait_bounds_rows), expanded_state_before_traversal_step.expanded_parameters.concat(mlc::Array<std::shared_ptr<ast::Param>>{std::make_shared<ast::Param>(ast::Param{ast::param_name(parameter_under_traversal_shared), parameter_under_traversal_shared->is_mut, parameter_traversal_expression_chunk.type_expression, parameter_under_traversal_shared->has_default, parameter_under_traversal_shared->default_, parameter_under_traversal_shared->param_pattern})})};
 }(); });
trait_param_expand::TraitExpandChunk return_chunk_under_function_declaration = expand_type_expression_for_trait_param(return_type_expression, explicit_type_parameter_environment, trait_declaration_names, nominal_type_declaration_names, after_parameter_list_expanded.synthetic_counter);
return std::make_shared<ast::Decl>(ast::DeclFn(function_name, after_parameter_list_expanded.expanded_type_parameter_names.concat(return_chunk_under_function_declaration.appended_type_parameter_names), after_parameter_list_expanded.expanded_trait_bounds.concat(return_chunk_under_function_declaration.appended_trait_bounds_rows), after_parameter_list_expanded.expanded_parameters, return_chunk_under_function_declaration.type_expression, body_expression, where_clause_bounds_entries));
}

mlc::Array<std::shared_ptr<ast::Decl>> expand_extend_methods(mlc::Array<std::shared_ptr<ast::Decl>> methods, mlc::HashMap<mlc::String, bool> trait_declaration_names, mlc::HashMap<mlc::String, bool> nominal_type_declaration_names) noexcept{return methods.map([trait_declaration_names, nominal_type_declaration_names](std::shared_ptr<ast::Decl> method_declaration_under_extend) mutable { return expand_decl_shared(method_declaration_under_extend, trait_declaration_names, nominal_type_declaration_names); });}

std::shared_ptr<ast::Decl> expand_decl_shared(std::shared_ptr<ast::Decl> declaration, mlc::HashMap<mlc::String, bool> trait_declaration_names, mlc::HashMap<mlc::String, bool> nominal_type_declaration_names) noexcept{return [&]() -> std::shared_ptr<ast::Decl> { if (std::holds_alternative<ast::DeclFn>((*declaration))) { auto _v_declfn = std::get<ast::DeclFn>((*declaration)); auto [function_name, type_parameter_names, trait_bounds_rows, parameters, return_type_expression, body_expression, where_clause_bounds_entries] = _v_declfn; return expand_decl_fn_trait_parameters(function_name, type_parameter_names, trait_bounds_rows, parameters, return_type_expression, body_expression, where_clause_bounds_entries, trait_declaration_names, nominal_type_declaration_names); } if (std::holds_alternative<ast::DeclExported>((*declaration))) { auto _v_declexported = std::get<ast::DeclExported>((*declaration)); auto [exported_declaration] = _v_declexported; return std::make_shared<ast::Decl>(ast::DeclExported(expand_decl_shared(exported_declaration, trait_declaration_names, nominal_type_declaration_names))); } if (std::holds_alternative<ast::DeclExtend>((*declaration))) { auto _v_declextend = std::get<ast::DeclExtend>((*declaration)); auto [extended_type_name, implemented_trait_name, methods, name_span] = _v_declextend; return std::make_shared<ast::Decl>(ast::DeclExtend(extended_type_name, implemented_trait_name, expand_extend_methods(methods, trait_declaration_names, nominal_type_declaration_names), name_span)); } return declaration; }();}

mlc::Array<std::shared_ptr<ast::Decl>> expand_declarations_with_trait_and_nominal_maps(mlc::Array<std::shared_ptr<ast::Decl>> declarations, mlc::HashMap<mlc::String, bool> trait_declaration_names, mlc::HashMap<mlc::String, bool> nominal_type_declaration_names) noexcept{return declarations.map([trait_declaration_names, nominal_type_declaration_names](std::shared_ptr<ast::Decl> declaration_under_expansion_candidate) mutable { return expand_decl_shared(declaration_under_expansion_candidate, trait_declaration_names, nominal_type_declaration_names); });}

trait_param_expand::Trait_nominal_maps_fold_state trait_nominal_maps_fold_step(trait_param_expand::Trait_nominal_maps_fold_state fold_state, std::shared_ptr<ast::Decl> current_declaration) noexcept{
return [&]() -> trait_param_expand::Trait_nominal_maps_fold_state { if (std::holds_alternative<ast::DeclType>((*current_declaration))) { auto _v_decltype = std::get<ast::DeclType>((*current_declaration)); auto [type_name, _w0, _w1, _w2, _w3] = _v_decltype; return [&]() -> trait_param_expand::Trait_nominal_maps_fold_state { 
  mlc::HashMap<mlc::String, bool> nominal_names = fold_state.nominal_type_declaration_names;
  nominal_names.set(type_name, true);
  return trait_param_expand::Trait_nominal_maps_fold_state{fold_state.trait_declaration_names, nominal_names};
 }(); } if (std::holds_alternative<ast::DeclTypeAlias>((*current_declaration))) { auto _v_decltypealias = std::get<ast::DeclTypeAlias>((*current_declaration)); auto [type_name, _w0, _w1, _w2] = _v_decltypealias; return [&]() -> trait_param_expand::Trait_nominal_maps_fold_state { 
  mlc::HashMap<mlc::String, bool> nominal_names = fold_state.nominal_type_declaration_names;
  nominal_names.set(type_name, true);
  return trait_param_expand::Trait_nominal_maps_fold_state{fold_state.trait_declaration_names, nominal_names};
 }(); } if (std::holds_alternative<ast::DeclTrait>((*current_declaration))) { auto _v_decltrait = std::get<ast::DeclTrait>((*current_declaration)); auto [trait_name, _w0, _w1, _w2] = _v_decltrait; return [&]() -> trait_param_expand::Trait_nominal_maps_fold_state { 
  mlc::HashMap<mlc::String, bool> trait_names = fold_state.trait_declaration_names;
  trait_names.set(trait_name, true);
  return trait_param_expand::Trait_nominal_maps_fold_state{trait_names, fold_state.nominal_type_declaration_names};
 }(); } return fold_state; }();
}

trait_param_expand::TraitNominalMaps build_trait_nominal_maps(ast::Program program) noexcept{
trait_param_expand::Trait_nominal_maps_fold_state initial_fold_state = trait_param_expand::Trait_nominal_maps_fold_state{mlc::HashMap<mlc::String, bool>(), mlc::HashMap<mlc::String, bool>()};
trait_param_expand::Trait_nominal_maps_fold_state final_fold_state = program.decls.fold(initial_fold_state, [](trait_param_expand::Trait_nominal_maps_fold_state fold_state_so_far, std::shared_ptr<ast::Decl> root_declaration) mutable { return trait_nominal_maps_fold_step(fold_state_so_far, declaration_without_export_wrappers(root_declaration)); });
return trait_param_expand::TraitNominalMaps{final_fold_state.trait_declaration_names, final_fold_state.nominal_type_declaration_names};
}

mlc::Array<std::shared_ptr<ast::Decl>> expand_declarations_with_trait_nominal_maps(mlc::Array<std::shared_ptr<ast::Decl>> declarations, trait_param_expand::TraitNominalMaps maps) noexcept{return expand_declarations_with_trait_and_nominal_maps(declarations, maps.trait_declaration_names, maps.nominal_type_declaration_names);}

mlc::Array<std::shared_ptr<ast::Decl>> expand_declarations_with_trait_context(mlc::Array<std::shared_ptr<ast::Decl>> declarations, ast::Program program_for_maps) noexcept{return expand_declarations_with_trait_nominal_maps(declarations, build_trait_nominal_maps(program_for_maps));}

ast::Program expand_trait_as_param_entry_using_full(ast::Program entry, ast::Program full_program) noexcept{return ast::Program{expand_declarations_with_trait_context(entry.decls, full_program)};}

ast::Program expand_trait_as_param_program(ast::Program program) noexcept{return ast::Program{expand_declarations_with_trait_context(program.decls, program)};}

} // namespace trait_param_expand
