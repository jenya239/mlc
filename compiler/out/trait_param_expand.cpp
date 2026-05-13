#include "trait_param_expand.hpp"

#include "ast.hpp"

namespace trait_param_expand {

using namespace ast;
using namespace ast_tokens;

struct TraitExpandChunk {std::shared_ptr<ast::TypeExpr> type_expression;int next_counter;mlc::Array<mlc::String> appended_type_parameter_names;mlc::Array<mlc::Array<mlc::String>> appended_trait_bounds_rows;};

struct TypeArgumentAccumulationForGeneric {int synthetic_counter_under_generic;mlc::Array<std::shared_ptr<ast::TypeExpr>> rebuilt_generic_type_arguments_so_far;mlc::Array<mlc::String> combined_appended_type_parameter_names_so_far;mlc::Array<mlc::Array<mlc::String>> combined_trait_bounds_rows_so_far;};

struct FunctionParameterAccumulationUnderTyFn {int synthetic_counter_under_function_type;mlc::Array<std::shared_ptr<ast::TypeExpr>> rebuilt_function_parameter_types_so_far;mlc::Array<mlc::String> combined_appended_type_parameter_names_under_function_type;mlc::Array<mlc::Array<mlc::String>> combined_trait_bounds_rows_under_function_type;};

struct ExpandedFunctionParametersTraversalState {int synthetic_counter;mlc::Array<mlc::String> expanded_type_parameter_names;mlc::Array<mlc::Array<mlc::String>> expanded_trait_bounds;mlc::Array<std::shared_ptr<ast::Param>> expanded_parameters;};

void each_program_declaration_depth_first(ast::Program program, std::function<void(std::shared_ptr<ast::Decl>)> handle_declaration) noexcept;

mlc::Array<ast::Diagnostic> trait_and_type_name_conflict_diagnostics(ast::Program program) noexcept;

void fill_trait_and_nominal_maps(ast::Program program, mlc::HashMap<mlc::String, bool> trait_names, mlc::HashMap<mlc::String, bool> nominal_type_names) noexcept;

mlc::Array<mlc::Array<mlc::String>> align_trait_bounds_matrix(mlc::Array<mlc::String> type_parameter_names, mlc::Array<mlc::Array<mlc::String>> trait_bounds_rows) noexcept;

trait_param_expand::TraitExpandChunk expand_type_expression_for_trait_param(std::shared_ptr<ast::TypeExpr> type_expression, mlc::HashMap<mlc::String, bool> explicit_type_parameter_environment, mlc::HashMap<mlc::String, bool> trait_declaration_names, mlc::HashMap<mlc::String, bool> nominal_type_declaration_names, int synthetic_counter) noexcept;

std::shared_ptr<ast::Decl> expand_decl_fn_trait_parameters(mlc::String function_name, mlc::Array<mlc::String> type_parameter_names, mlc::Array<mlc::Array<mlc::String>> trait_bounds_rows, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<ast::TypeExpr> return_type_expression, std::shared_ptr<ast::Expr> body_expression, mlc::Array<ast::WhereClauseBound> where_clause_bounds_entries, mlc::HashMap<mlc::String, bool> trait_declaration_names, mlc::HashMap<mlc::String, bool> nominal_type_declaration_names) noexcept;

mlc::Array<std::shared_ptr<ast::Decl>> expand_extend_methods(mlc::Array<std::shared_ptr<ast::Decl>> methods, mlc::HashMap<mlc::String, bool> trait_declaration_names, mlc::HashMap<mlc::String, bool> nominal_type_declaration_names) noexcept;

std::shared_ptr<ast::Decl> expand_decl_shared(std::shared_ptr<ast::Decl> declaration, mlc::HashMap<mlc::String, bool> trait_declaration_names, mlc::HashMap<mlc::String, bool> nominal_type_declaration_names) noexcept;

mlc::Array<std::shared_ptr<ast::Decl>> expand_declarations_with_trait_and_nominal_maps(mlc::Array<std::shared_ptr<ast::Decl>> declarations, mlc::HashMap<mlc::String, bool> trait_declaration_names, mlc::HashMap<mlc::String, bool> nominal_type_declaration_names) noexcept;

mlc::Array<std::shared_ptr<ast::Decl>> expand_declarations_with_trait_context(mlc::Array<std::shared_ptr<ast::Decl>> declarations, ast::Program program_for_maps) noexcept;

ast::Program expand_trait_as_param_entry_using_full(ast::Program entry, ast::Program full_program) noexcept;

ast::Program expand_trait_as_param_program(ast::Program program) noexcept;

void each_program_declaration_depth_first(ast::Program program, std::function<void(std::shared_ptr<ast::Decl>)> handle_declaration) noexcept{
int declaration_index = 0;
return [&]() { 
  while (declaration_index < program.decls.size()){
{
mlc::Array<std::shared_ptr<ast::Decl>> declaration_stack = {};
declaration_stack.push_back(program.decls[declaration_index]);
int stack_index = 0;
while (stack_index < declaration_stack.size()){
{
std::shared_ptr<ast::Decl> current_declaration = declaration_stack[stack_index];
[&]() -> std::tuple<> { if (std::holds_alternative<ast::DeclExported>((*current_declaration))) { auto _v_declexported = std::get<ast::DeclExported>((*current_declaration)); auto [inner] = _v_declexported; return [&]() -> std::tuple<> { 
  declaration_stack.push_back(inner);
  return std::make_tuple();
 }(); } return [&]() -> std::tuple<> { 
  handle_declaration(current_declaration);
  return std::make_tuple();
 }(); }();
stack_index = stack_index + 1;
}
}
declaration_index = declaration_index + 1;
}
}
 }();
}

mlc::Array<ast::Diagnostic> trait_and_type_name_conflict_diagnostics(ast::Program program) noexcept{
mlc::HashMap<mlc::String, bool> trait_declaration_names = mlc::HashMap<mlc::String, bool>();
mlc::HashMap<mlc::String, bool> nominal_type_declaration_names = mlc::HashMap<mlc::String, bool>();
mlc::Array<ast::Diagnostic> conflicts = {};
each_program_declaration_depth_first(program, [&trait_declaration_names, &conflicts, &nominal_type_declaration_names](std::shared_ptr<ast::Decl> current_declaration) mutable { return [&]() -> std::tuple<> { if (std::holds_alternative<ast::DeclType>((*current_declaration))) { auto _v_decltype = std::get<ast::DeclType>((*current_declaration)); auto [type_name, _w0, _w1, _w2] = _v_decltype; return [&]() -> std::tuple<> { 
  if (trait_declaration_names.has(type_name)){
{
conflicts.push_back(ast::diagnostic_error(mlc::String("the name \"") + type_name + mlc::String("\" is declared as both a type and a trait"), ast::span_unknown()));
}
}
  nominal_type_declaration_names.set(type_name, true);
  return std::make_tuple();
 }(); } if (std::holds_alternative<ast::DeclTrait>((*current_declaration))) { auto _v_decltrait = std::get<ast::DeclTrait>((*current_declaration)); auto [trait_name, _w0, _w1] = _v_decltrait; return [&]() -> std::tuple<> { 
  if (nominal_type_declaration_names.has(trait_name)){
{
conflicts.push_back(ast::diagnostic_error(mlc::String("the name \"") + trait_name + mlc::String("\" is declared as both a type and a trait"), ast::span_unknown()));
}
}
  trait_declaration_names.set(trait_name, true);
  return std::make_tuple();
 }(); } return std::make_tuple(); }(); });
return conflicts;
}

void fill_trait_and_nominal_maps(ast::Program program, mlc::HashMap<mlc::String, bool> trait_names, mlc::HashMap<mlc::String, bool> nominal_type_names) noexcept{return each_program_declaration_depth_first(program, [&nominal_type_names, &trait_names](std::shared_ptr<ast::Decl> current_declaration) mutable { return [&]() -> std::tuple<> { if (std::holds_alternative<ast::DeclType>((*current_declaration))) { auto _v_decltype = std::get<ast::DeclType>((*current_declaration)); auto [type_name, _w0, _w1, _w2] = _v_decltype; return [&]() -> std::tuple<> { 
  nominal_type_names.set(type_name, true);
  return std::make_tuple();
 }(); } if (std::holds_alternative<ast::DeclTrait>((*current_declaration))) { auto _v_decltrait = std::get<ast::DeclTrait>((*current_declaration)); auto [trait_name, _w0, _w1] = _v_decltrait; return [&]() -> std::tuple<> { 
  trait_names.set(trait_name, true);
  return std::make_tuple();
 }(); } return std::make_tuple(); }(); });}

mlc::Array<mlc::Array<mlc::String>> align_trait_bounds_matrix(mlc::Array<mlc::String> type_parameter_names, mlc::Array<mlc::Array<mlc::String>> trait_bounds_rows) noexcept{
mlc::Array<mlc::Array<mlc::String>> aligned = trait_bounds_rows.concat({});
int filled_row_count = aligned.size();
while (filled_row_count < type_parameter_names.size()){
{
aligned.push_back({});
filled_row_count = filled_row_count + 1;
}
}
return aligned;
}

trait_param_expand::TraitExpandChunk expand_type_expression_for_trait_param(std::shared_ptr<ast::TypeExpr> type_expression, mlc::HashMap<mlc::String, bool> explicit_type_parameter_environment, mlc::HashMap<mlc::String, bool> trait_declaration_names, mlc::HashMap<mlc::String, bool> nominal_type_declaration_names, int synthetic_counter) noexcept{return std::visit(overloaded{
  [&](const TyNamed& tynamed) -> trait_param_expand::TraitExpandChunk { auto [name] = tynamed; return explicit_type_parameter_environment.has(name) ? trait_param_expand::TraitExpandChunk{type_expression, synthetic_counter, {}, {}} : trait_declaration_names.has(name) && !nominal_type_declaration_names.has(name) ? [&]() -> trait_param_expand::TraitExpandChunk { 
  mlc::String synthetic_name = mlc::String("__trait_param_") + mlc::to_string(synthetic_counter);
  mlc::Array<mlc::String> single_trait_row = mlc::Array<mlc::String>{name};
  mlc::Array<mlc::Array<mlc::String>> bounds_wrapper = mlc::Array<mlc::Array<mlc::String>>{single_trait_row};
  return trait_param_expand::TraitExpandChunk{std::make_shared<ast::TypeExpr>(ast::TyNamed(synthetic_name)), synthetic_counter + 1, mlc::Array<mlc::String>{synthetic_name}, bounds_wrapper};
 }() : trait_param_expand::TraitExpandChunk{type_expression, synthetic_counter, {}, {}}; },
  [&](const TyArray& tyarray) -> trait_param_expand::TraitExpandChunk { auto [inner] = tyarray; return [&]() -> trait_param_expand::TraitExpandChunk { 
  trait_param_expand::TraitExpandChunk inner_chunk = expand_type_expression_for_trait_param(inner, explicit_type_parameter_environment, trait_declaration_names, nominal_type_declaration_names, synthetic_counter);
  return trait_param_expand::TraitExpandChunk{std::make_shared<ast::TypeExpr>(ast::TyArray(inner_chunk.type_expression)), inner_chunk.next_counter, inner_chunk.appended_type_parameter_names, inner_chunk.appended_trait_bounds_rows};
 }(); },
  [&](const TyShared& tyshared) -> trait_param_expand::TraitExpandChunk { auto [inner] = tyshared; return [&]() -> trait_param_expand::TraitExpandChunk { 
  trait_param_expand::TraitExpandChunk inner_chunk = expand_type_expression_for_trait_param(inner, explicit_type_parameter_environment, trait_declaration_names, nominal_type_declaration_names, synthetic_counter);
  return trait_param_expand::TraitExpandChunk{std::make_shared<ast::TypeExpr>(ast::TyShared(inner_chunk.type_expression)), inner_chunk.next_counter, inner_chunk.appended_type_parameter_names, inner_chunk.appended_trait_bounds_rows};
 }(); },
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
  trait_param_expand::TraitExpandChunk parameter_traversal_expression_chunk = expand_type_expression_for_trait_param(ast::param_typ(parameter_under_traversal_shared), explicit_type_parameter_environment, trait_declaration_names, nominal_type_declaration_names, expanded_state_before_traversal_step.synthetic_counter);
  return trait_param_expand::ExpandedFunctionParametersTraversalState{parameter_traversal_expression_chunk.next_counter, expanded_state_before_traversal_step.expanded_type_parameter_names.concat(parameter_traversal_expression_chunk.appended_type_parameter_names), expanded_state_before_traversal_step.expanded_trait_bounds.concat(parameter_traversal_expression_chunk.appended_trait_bounds_rows), expanded_state_before_traversal_step.expanded_parameters.concat(mlc::Array<std::shared_ptr<ast::Param>>{std::make_shared<ast::Param>(ast::Param{ast::param_name(parameter_under_traversal_shared), parameter_under_traversal_shared->is_mut, parameter_traversal_expression_chunk.type_expression, parameter_under_traversal_shared->has_default, parameter_under_traversal_shared->default_, parameter_under_traversal_shared->param_pattern})})};
 }(); });
trait_param_expand::TraitExpandChunk return_chunk_under_function_declaration = expand_type_expression_for_trait_param(return_type_expression, explicit_type_parameter_environment, trait_declaration_names, nominal_type_declaration_names, after_parameter_list_expanded.synthetic_counter);
return std::make_shared<ast::Decl>(ast::DeclFn(function_name, after_parameter_list_expanded.expanded_type_parameter_names.concat(return_chunk_under_function_declaration.appended_type_parameter_names), after_parameter_list_expanded.expanded_trait_bounds.concat(return_chunk_under_function_declaration.appended_trait_bounds_rows), after_parameter_list_expanded.expanded_parameters, return_chunk_under_function_declaration.type_expression, body_expression, where_clause_bounds_entries));
}

mlc::Array<std::shared_ptr<ast::Decl>> expand_extend_methods(mlc::Array<std::shared_ptr<ast::Decl>> methods, mlc::HashMap<mlc::String, bool> trait_declaration_names, mlc::HashMap<mlc::String, bool> nominal_type_declaration_names) noexcept{return methods.map([trait_declaration_names, nominal_type_declaration_names](std::shared_ptr<ast::Decl> method_declaration_under_extend) mutable { return expand_decl_shared(method_declaration_under_extend, trait_declaration_names, nominal_type_declaration_names); });}

std::shared_ptr<ast::Decl> expand_decl_shared(std::shared_ptr<ast::Decl> declaration, mlc::HashMap<mlc::String, bool> trait_declaration_names, mlc::HashMap<mlc::String, bool> nominal_type_declaration_names) noexcept{return [&]() -> std::shared_ptr<ast::Decl> { if (std::holds_alternative<ast::DeclFn>((*declaration))) { auto _v_declfn = std::get<ast::DeclFn>((*declaration)); auto [function_name, type_parameter_names, trait_bounds_rows, parameters, return_type_expression, body_expression, where_clause_bounds_entries] = _v_declfn; return expand_decl_fn_trait_parameters(function_name, type_parameter_names, trait_bounds_rows, parameters, return_type_expression, body_expression, where_clause_bounds_entries, trait_declaration_names, nominal_type_declaration_names); } if (std::holds_alternative<ast::DeclExported>((*declaration))) { auto _v_declexported = std::get<ast::DeclExported>((*declaration)); auto [inner] = _v_declexported; return std::make_shared<ast::Decl>(ast::DeclExported(expand_decl_shared(inner, trait_declaration_names, nominal_type_declaration_names))); } if (std::holds_alternative<ast::DeclExtend>((*declaration))) { auto _v_declextend = std::get<ast::DeclExtend>((*declaration)); auto [extended_type_name, implemented_trait_name, methods] = _v_declextend; return std::make_shared<ast::Decl>(ast::DeclExtend(extended_type_name, implemented_trait_name, expand_extend_methods(methods, trait_declaration_names, nominal_type_declaration_names))); } return declaration; }();}

mlc::Array<std::shared_ptr<ast::Decl>> expand_declarations_with_trait_and_nominal_maps(mlc::Array<std::shared_ptr<ast::Decl>> declarations, mlc::HashMap<mlc::String, bool> trait_declaration_names, mlc::HashMap<mlc::String, bool> nominal_type_declaration_names) noexcept{return declarations.map([trait_declaration_names, nominal_type_declaration_names](std::shared_ptr<ast::Decl> declaration_under_expansion_candidate) mutable { return expand_decl_shared(declaration_under_expansion_candidate, trait_declaration_names, nominal_type_declaration_names); });}

mlc::Array<std::shared_ptr<ast::Decl>> expand_declarations_with_trait_context(mlc::Array<std::shared_ptr<ast::Decl>> declarations, ast::Program program_for_maps) noexcept{
mlc::HashMap<mlc::String, bool> trait_declaration_names = mlc::HashMap<mlc::String, bool>();
mlc::HashMap<mlc::String, bool> nominal_type_declaration_names = mlc::HashMap<mlc::String, bool>();
fill_trait_and_nominal_maps(program_for_maps, trait_declaration_names, nominal_type_declaration_names);
return expand_declarations_with_trait_and_nominal_maps(declarations, trait_declaration_names, nominal_type_declaration_names);
}

ast::Program expand_trait_as_param_entry_using_full(ast::Program entry, ast::Program full_program) noexcept{return ast::Program{expand_declarations_with_trait_context(entry.decls, full_program)};}

ast::Program expand_trait_as_param_program(ast::Program program) noexcept{return ast::Program{expand_declarations_with_trait_context(program.decls, program)};}

} // namespace trait_param_expand
