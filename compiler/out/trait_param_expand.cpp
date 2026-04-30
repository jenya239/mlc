#include "trait_param_expand.hpp"

#include "ast.hpp"

namespace trait_param_expand {

using namespace ast;
using namespace ast_tokens;

struct TraitExpandChunk {std::shared_ptr<ast::TypeExpr> type_expression;int next_counter;mlc::Array<mlc::String> appended_type_parameter_names;mlc::Array<mlc::Array<mlc::String>> appended_trait_bounds_rows;};

mlc::Array<ast::Diagnostic> trait_and_type_name_conflict_diagnostics(ast::Program program) noexcept;

void fill_trait_and_nominal_maps(ast::Program program, mlc::HashMap<mlc::String, bool> trait_names, mlc::HashMap<mlc::String, bool> nominal_type_names) noexcept;

mlc::Array<mlc::Array<mlc::String>> align_trait_bounds_matrix(mlc::Array<mlc::String> type_parameter_names, mlc::Array<mlc::Array<mlc::String>> trait_bounds_rows) noexcept;

mlc::Array<mlc::Array<mlc::String>> append_string_rows(mlc::Array<mlc::Array<mlc::String>> destination, mlc::Array<mlc::Array<mlc::String>> source) noexcept;

mlc::Array<mlc::String> append_strings(mlc::Array<mlc::String> destination, mlc::Array<mlc::String> source) noexcept;

trait_param_expand::TraitExpandChunk expand_type_expression_for_trait_param(std::shared_ptr<ast::TypeExpr> type_expression, mlc::HashMap<mlc::String, bool> explicit_type_parameter_environment, mlc::HashMap<mlc::String, bool> trait_declaration_names, mlc::HashMap<mlc::String, bool> nominal_type_declaration_names, int synthetic_counter) noexcept;

std::shared_ptr<ast::Decl> expand_decl_fn_trait_parameters(mlc::String function_name, mlc::Array<mlc::String> type_parameter_names, mlc::Array<mlc::Array<mlc::String>> trait_bounds_rows, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<ast::TypeExpr> return_type_expression, std::shared_ptr<ast::Expr> body_expression, mlc::Array<ast::WhereClauseBound> where_clause_bounds_entries, mlc::HashMap<mlc::String, bool> trait_declaration_names, mlc::HashMap<mlc::String, bool> nominal_type_declaration_names) noexcept;

mlc::Array<std::shared_ptr<ast::Decl>> expand_extend_methods(mlc::Array<std::shared_ptr<ast::Decl>> methods, mlc::HashMap<mlc::String, bool> trait_declaration_names, mlc::HashMap<mlc::String, bool> nominal_type_declaration_names) noexcept;

std::shared_ptr<ast::Decl> expand_decl_shared(std::shared_ptr<ast::Decl> declaration, mlc::HashMap<mlc::String, bool> trait_declaration_names, mlc::HashMap<mlc::String, bool> nominal_type_declaration_names) noexcept;

mlc::Array<std::shared_ptr<ast::Decl>> expand_declarations_with_trait_and_nominal_maps(mlc::Array<std::shared_ptr<ast::Decl>> declarations, mlc::HashMap<mlc::String, bool> trait_declaration_names, mlc::HashMap<mlc::String, bool> nominal_type_declaration_names) noexcept;

mlc::Array<std::shared_ptr<ast::Decl>> expand_declarations_with_trait_context(mlc::Array<std::shared_ptr<ast::Decl>> declarations, ast::Program program_for_maps) noexcept;

ast::Program expand_trait_as_param_entry_using_full(ast::Program entry, ast::Program full_program) noexcept;

ast::Program expand_trait_as_param_program(ast::Program program) noexcept;

mlc::Array<ast::Diagnostic> trait_and_type_name_conflict_diagnostics(ast::Program program) noexcept{
mlc::HashMap<mlc::String, bool> trait_declaration_names = mlc::HashMap<mlc::String, bool>();
mlc::HashMap<mlc::String, bool> nominal_type_declaration_names = mlc::HashMap<mlc::String, bool>();
mlc::Array<ast::Diagnostic> conflicts = {};
int declaration_index = 0;
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
 }(); } if (std::holds_alternative<ast::DeclType>((*current_declaration))) { auto _v_decltype = std::get<ast::DeclType>((*current_declaration)); auto [type_name, _w0, _w1, _w2] = _v_decltype; return [&]() -> std::tuple<> { 
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
 }(); } return std::make_tuple(); }();
stack_index = stack_index + 1;
}
}
declaration_index = declaration_index + 1;
}
}
return conflicts;
}

void fill_trait_and_nominal_maps(ast::Program program, mlc::HashMap<mlc::String, bool> trait_names, mlc::HashMap<mlc::String, bool> nominal_type_names) noexcept{
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
 }(); } if (std::holds_alternative<ast::DeclType>((*current_declaration))) { auto _v_decltype = std::get<ast::DeclType>((*current_declaration)); auto [type_name, _w0, _w1, _w2] = _v_decltype; return [&]() -> std::tuple<> { 
  nominal_type_names.set(type_name, true);
  return std::make_tuple();
 }(); } if (std::holds_alternative<ast::DeclTrait>((*current_declaration))) { auto _v_decltrait = std::get<ast::DeclTrait>((*current_declaration)); auto [trait_name, _w0, _w1] = _v_decltrait; return [&]() -> std::tuple<> { 
  trait_names.set(trait_name, true);
  return std::make_tuple();
 }(); } return std::make_tuple(); }();
stack_index = stack_index + 1;
}
}
declaration_index = declaration_index + 1;
}
}
 }();
}

mlc::Array<mlc::Array<mlc::String>> align_trait_bounds_matrix(mlc::Array<mlc::String> type_parameter_names, mlc::Array<mlc::Array<mlc::String>> trait_bounds_rows) noexcept{
mlc::Array<mlc::Array<mlc::String>> aligned = {};
int row_index = 0;
while (row_index < trait_bounds_rows.size()){
{
aligned.push_back(trait_bounds_rows[row_index]);
row_index = row_index + 1;
}
}
while (aligned.size() < type_parameter_names.size()){
{
mlc::Array<mlc::String> empty_row = {};
aligned.push_back(empty_row);
}
}
return aligned;
}

mlc::Array<mlc::Array<mlc::String>> append_string_rows(mlc::Array<mlc::Array<mlc::String>> destination, mlc::Array<mlc::Array<mlc::String>> source) noexcept{
int index = 0;
while (index < source.size()){
{
destination.push_back(source[index]);
index = index + 1;
}
}
return destination;
}

mlc::Array<mlc::String> append_strings(mlc::Array<mlc::String> destination, mlc::Array<mlc::String> source) noexcept{
int index = 0;
while (index < source.size()){
{
destination.push_back(source[index]);
index = index + 1;
}
}
return destination;
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
  int counter = synthetic_counter;
  mlc::Array<std::shared_ptr<ast::TypeExpr>> rebuilt_arguments = {};
  mlc::Array<mlc::String> combined_appended_names = {};
  mlc::Array<mlc::Array<mlc::String>> combined_bounds = {};
  int argument_index = 0;
  while (argument_index < type_arguments.size()){
{
trait_param_expand::TraitExpandChunk argument_chunk = expand_type_expression_for_trait_param(type_arguments[argument_index], explicit_type_parameter_environment, trait_declaration_names, nominal_type_declaration_names, counter);
rebuilt_arguments.push_back(argument_chunk.type_expression);
counter = argument_chunk.next_counter;
combined_appended_names = append_strings(combined_appended_names, argument_chunk.appended_type_parameter_names);
combined_bounds = append_string_rows(combined_bounds, argument_chunk.appended_trait_bounds_rows);
argument_index = argument_index + 1;
}
}
  return trait_param_expand::TraitExpandChunk{std::make_shared<ast::TypeExpr>(ast::TyGeneric(base_name, rebuilt_arguments)), counter, combined_appended_names, combined_bounds};
 }(); },
  [&](const TyFn& tyfn) -> trait_param_expand::TraitExpandChunk { auto [parameter_types, return_type_expression] = tyfn; return [&]() -> trait_param_expand::TraitExpandChunk { 
  int counter = synthetic_counter;
  mlc::Array<std::shared_ptr<ast::TypeExpr>> rebuilt_parameter_types = {};
  mlc::Array<mlc::String> combined_appended_names = {};
  mlc::Array<mlc::Array<mlc::String>> combined_bounds = {};
  int parameter_index = 0;
  while (parameter_index < parameter_types.size()){
{
trait_param_expand::TraitExpandChunk parameter_chunk = expand_type_expression_for_trait_param(parameter_types[parameter_index], explicit_type_parameter_environment, trait_declaration_names, nominal_type_declaration_names, counter);
rebuilt_parameter_types.push_back(parameter_chunk.type_expression);
counter = parameter_chunk.next_counter;
combined_appended_names = append_strings(combined_appended_names, parameter_chunk.appended_type_parameter_names);
combined_bounds = append_string_rows(combined_bounds, parameter_chunk.appended_trait_bounds_rows);
parameter_index = parameter_index + 1;
}
}
  trait_param_expand::TraitExpandChunk return_chunk = expand_type_expression_for_trait_param(return_type_expression, explicit_type_parameter_environment, trait_declaration_names, nominal_type_declaration_names, counter);
  return trait_param_expand::TraitExpandChunk{std::make_shared<ast::TypeExpr>(ast::TyFn(rebuilt_parameter_types, return_chunk.type_expression)), return_chunk.next_counter, append_strings(combined_appended_names, return_chunk.appended_type_parameter_names), append_string_rows(combined_bounds, return_chunk.appended_trait_bounds_rows)};
 }(); },
  [&](const TyAssoc& tyassoc) -> trait_param_expand::TraitExpandChunk { auto [_w0, _w1] = tyassoc; return trait_param_expand::TraitExpandChunk{type_expression, synthetic_counter, {}, {}}; },
  [&](const TyI32& tyi32) -> trait_param_expand::TraitExpandChunk { return trait_param_expand::TraitExpandChunk{type_expression, synthetic_counter, {}, {}}; },
  [&](const TyString& tystring) -> trait_param_expand::TraitExpandChunk { return trait_param_expand::TraitExpandChunk{type_expression, synthetic_counter, {}, {}}; },
  [&](const TyBool& tybool) -> trait_param_expand::TraitExpandChunk { return trait_param_expand::TraitExpandChunk{type_expression, synthetic_counter, {}, {}}; },
  [&](const TyUnit& tyunit) -> trait_param_expand::TraitExpandChunk { return trait_param_expand::TraitExpandChunk{type_expression, synthetic_counter, {}, {}}; }
}, (*type_expression));}

std::shared_ptr<ast::Decl> expand_decl_fn_trait_parameters(mlc::String function_name, mlc::Array<mlc::String> type_parameter_names, mlc::Array<mlc::Array<mlc::String>> trait_bounds_rows, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<ast::TypeExpr> return_type_expression, std::shared_ptr<ast::Expr> body_expression, mlc::Array<ast::WhereClauseBound> where_clause_bounds_entries, mlc::HashMap<mlc::String, bool> trait_declaration_names, mlc::HashMap<mlc::String, bool> nominal_type_declaration_names) noexcept{
mlc::HashMap<mlc::String, bool> explicit_type_parameter_environment = mlc::HashMap<mlc::String, bool>();
int explicit_index = 0;
while (explicit_index < type_parameter_names.size()){
{
explicit_type_parameter_environment.set(type_parameter_names[explicit_index], true);
explicit_index = explicit_index + 1;
}
}
mlc::Array<mlc::String> expanded_type_parameter_names = {};
int copy_index = 0;
while (copy_index < type_parameter_names.size()){
{
expanded_type_parameter_names.push_back(type_parameter_names[copy_index]);
copy_index = copy_index + 1;
}
}
mlc::Array<mlc::Array<mlc::String>> expanded_trait_bounds = align_trait_bounds_matrix(type_parameter_names, trait_bounds_rows);
int synthetic_counter = 0;
mlc::Array<std::shared_ptr<ast::Param>> expanded_parameters = {};
int parameter_index = 0;
while (parameter_index < parameters.size()){
{
trait_param_expand::TraitExpandChunk parameter_chunk = expand_type_expression_for_trait_param(ast::param_typ(parameters[parameter_index]), explicit_type_parameter_environment, trait_declaration_names, nominal_type_declaration_names, synthetic_counter);
synthetic_counter = parameter_chunk.next_counter;
expanded_type_parameter_names = append_strings(expanded_type_parameter_names, parameter_chunk.appended_type_parameter_names);
expanded_trait_bounds = append_string_rows(expanded_trait_bounds, parameter_chunk.appended_trait_bounds_rows);
std::shared_ptr<ast::Param> original_parameter = parameters[parameter_index];
expanded_parameters.push_back(std::make_shared<ast::Param>(ast::Param{ast::param_name(original_parameter), original_parameter->is_mut, parameter_chunk.type_expression, original_parameter->has_default, original_parameter->default_}));
parameter_index = parameter_index + 1;
}
}
trait_param_expand::TraitExpandChunk return_chunk = expand_type_expression_for_trait_param(return_type_expression, explicit_type_parameter_environment, trait_declaration_names, nominal_type_declaration_names, synthetic_counter);
expanded_type_parameter_names = append_strings(expanded_type_parameter_names, return_chunk.appended_type_parameter_names);
expanded_trait_bounds = append_string_rows(expanded_trait_bounds, return_chunk.appended_trait_bounds_rows);
return std::make_shared<ast::Decl>(ast::DeclFn(function_name, expanded_type_parameter_names, expanded_trait_bounds, expanded_parameters, return_chunk.type_expression, body_expression, where_clause_bounds_entries));
}

mlc::Array<std::shared_ptr<ast::Decl>> expand_extend_methods(mlc::Array<std::shared_ptr<ast::Decl>> methods, mlc::HashMap<mlc::String, bool> trait_declaration_names, mlc::HashMap<mlc::String, bool> nominal_type_declaration_names) noexcept{
mlc::Array<std::shared_ptr<ast::Decl>> result = {};
int method_index = 0;
while (method_index < methods.size()){
{
result.push_back(expand_decl_shared(methods[method_index], trait_declaration_names, nominal_type_declaration_names));
method_index = method_index + 1;
}
}
return result;
}

std::shared_ptr<ast::Decl> expand_decl_shared(std::shared_ptr<ast::Decl> declaration, mlc::HashMap<mlc::String, bool> trait_declaration_names, mlc::HashMap<mlc::String, bool> nominal_type_declaration_names) noexcept{return [&]() -> std::shared_ptr<ast::Decl> { if (std::holds_alternative<ast::DeclFn>((*declaration))) { auto _v_declfn = std::get<ast::DeclFn>((*declaration)); auto [function_name, type_parameter_names, trait_bounds_rows, parameters, return_type_expression, body_expression, where_clause_bounds_entries] = _v_declfn; return expand_decl_fn_trait_parameters(function_name, type_parameter_names, trait_bounds_rows, parameters, return_type_expression, body_expression, where_clause_bounds_entries, trait_declaration_names, nominal_type_declaration_names); } if (std::holds_alternative<ast::DeclExported>((*declaration))) { auto _v_declexported = std::get<ast::DeclExported>((*declaration)); auto [inner] = _v_declexported; return std::make_shared<ast::Decl>(ast::DeclExported(expand_decl_shared(inner, trait_declaration_names, nominal_type_declaration_names))); } if (std::holds_alternative<ast::DeclExtend>((*declaration))) { auto _v_declextend = std::get<ast::DeclExtend>((*declaration)); auto [extended_type_name, implemented_trait_name, methods] = _v_declextend; return std::make_shared<ast::Decl>(ast::DeclExtend(extended_type_name, implemented_trait_name, expand_extend_methods(methods, trait_declaration_names, nominal_type_declaration_names))); } return declaration; }();}

mlc::Array<std::shared_ptr<ast::Decl>> expand_declarations_with_trait_and_nominal_maps(mlc::Array<std::shared_ptr<ast::Decl>> declarations, mlc::HashMap<mlc::String, bool> trait_declaration_names, mlc::HashMap<mlc::String, bool> nominal_type_declaration_names) noexcept{
mlc::Array<std::shared_ptr<ast::Decl>> expanded_declarations = {};
int declaration_index = 0;
while (declaration_index < declarations.size()){
{
expanded_declarations.push_back(expand_decl_shared(declarations[declaration_index], trait_declaration_names, nominal_type_declaration_names));
declaration_index = declaration_index + 1;
}
}
return expanded_declarations;
}

mlc::Array<std::shared_ptr<ast::Decl>> expand_declarations_with_trait_context(mlc::Array<std::shared_ptr<ast::Decl>> declarations, ast::Program program_for_maps) noexcept{
mlc::HashMap<mlc::String, bool> trait_declaration_names = mlc::HashMap<mlc::String, bool>();
mlc::HashMap<mlc::String, bool> nominal_type_declaration_names = mlc::HashMap<mlc::String, bool>();
fill_trait_and_nominal_maps(program_for_maps, trait_declaration_names, nominal_type_declaration_names);
return expand_declarations_with_trait_and_nominal_maps(declarations, trait_declaration_names, nominal_type_declaration_names);
}

ast::Program expand_trait_as_param_entry_using_full(ast::Program entry, ast::Program full_program) noexcept{return ast::Program{expand_declarations_with_trait_context(entry.decls, full_program)};}

ast::Program expand_trait_as_param_program(ast::Program program) noexcept{return ast::Program{expand_declarations_with_trait_context(program.decls, program)};}

} // namespace trait_param_expand
