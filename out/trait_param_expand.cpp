#define main mlc_user_main
#include "trait_param_expand.hpp"

namespace trait_param_expand {

mlc::Array<ast::Diagnostic> trait_type_conflicts_from_declaration(std::shared_ptr<ast::Decl> declaration, mlc::HashMap<mlc::String, bool> trait_declaration_names, mlc::HashMap<mlc::String, bool> nominal_type_declaration_names) noexcept{
return std::visit(overloaded{[&](const ast::DeclType& declType) { auto [type_name, __1, __2, __3] = declType; return [&]() {
auto out = mlc::Array<ast::Diagnostic>{};
if (trait_declaration_names.has(type_name)) {
out.push_back(ast::diagnostic_error(((mlc::String("the name \"", 10) + type_name) + mlc::String("\" is declared as both a type and a trait", 40)), ast::span_unknown()));
}
nominal_type_declaration_names.set(type_name, true);
return out;
}(); },
[&](const ast::DeclTrait& declTrait) { auto [trait_name, __1, __2] = declTrait; return [&]() {
auto out = mlc::Array<ast::Diagnostic>{};
if (nominal_type_declaration_names.has(trait_name)) {
out.push_back(ast::diagnostic_error(((mlc::String("the name \"", 10) + trait_name) + mlc::String("\" is declared as both a type and a trait", 40)), ast::span_unknown()));
}
trait_declaration_names.set(trait_name, true);
return out;
}(); },
[&](const ast::DeclExported& declExported) { auto [inner] = declExported; return trait_type_conflicts_from_declaration(inner, trait_declaration_names, nominal_type_declaration_names); },
[&](const auto& __v) { return [&]() {
auto empty_diag = mlc::Array<ast::Diagnostic>{};
return empty_diag;
}(); }
}, (*declaration));
}
mlc::Array<ast::Diagnostic> trait_and_type_name_conflict_diagnostics(ast::Program program) noexcept{
auto trait_declaration_names = mlc::HashMap<mlc::String, bool>();
auto nominal_type_declaration_names = mlc::HashMap<mlc::String, bool>();
auto conflicts = mlc::Array<ast::Diagnostic>{};
auto declaration_index = 0;
while ((declaration_index < program.decls.length())) {
auto from_decl = trait_type_conflicts_from_declaration(program.decls[declaration_index], trait_declaration_names, nominal_type_declaration_names);
auto merge_index = 0;
while ((merge_index < from_decl.length())) {
conflicts.push_back(from_decl[merge_index]);
merge_index = (merge_index + 1);
}
declaration_index = (declaration_index + 1);
}
return conflicts;
}
int scan_decl_for_trait_and_type_maps(std::shared_ptr<ast::Decl> declaration, mlc::HashMap<mlc::String, bool> trait_declaration_names, mlc::HashMap<mlc::String, bool> nominal_type_declaration_names) noexcept{
return std::visit(overloaded{[&](const ast::DeclType& declType) { auto [type_name, __1, __2, __3] = declType; return [&]() {
nominal_type_declaration_names.set(type_name, true);
return 0;
}(); },
[&](const ast::DeclTrait& declTrait) { auto [trait_name, __1, __2] = declTrait; return [&]() {
trait_declaration_names.set(trait_name, true);
return 0;
}(); },
[&](const ast::DeclExported& declExported) { auto [inner] = declExported; return scan_decl_for_trait_and_type_maps(inner, trait_declaration_names, nominal_type_declaration_names); },
[&](const auto& __v) { return 0; }
}, (*declaration));
}
void fill_trait_and_nominal_maps(ast::Program program, mlc::HashMap<mlc::String, bool> trait_names, mlc::HashMap<mlc::String, bool> nominal_type_names) noexcept{
auto declaration_index = 0;
return [&]() {
while ((declaration_index < program.decls.length())) {
scan_decl_for_trait_and_type_maps(program.decls[declaration_index], trait_names, nominal_type_names);
declaration_index = (declaration_index + 1);
}
}();
}
mlc::Array<mlc::Array<mlc::String>> align_trait_bounds_matrix(mlc::Array<mlc::String> type_parameter_names, mlc::Array<mlc::Array<mlc::String>> trait_bounds_rows) noexcept{
auto aligned = mlc::Array<mlc::Array<mlc::String>>{};
auto row_index = 0;
while ((row_index < trait_bounds_rows.length())) {
aligned.push_back(trait_bounds_rows[row_index]);
row_index = (row_index + 1);
}
while ((aligned.length() < type_parameter_names.length())) {
auto empty_row = mlc::Array<mlc::String>{};
aligned.push_back(empty_row);
}
return aligned;
}
mlc::Array<mlc::Array<mlc::String>> append_string_rows(mlc::Array<mlc::Array<mlc::String>> destination, mlc::Array<mlc::Array<mlc::String>> source) noexcept{
auto index = 0;
while ((index < source.length())) {
destination.push_back(source[index]);
index = (index + 1);
}
return destination;
}
mlc::Array<mlc::String> append_strings(mlc::Array<mlc::String> destination, mlc::Array<mlc::String> source) noexcept{
auto index = 0;
while ((index < source.length())) {
destination.push_back(source[index]);
index = (index + 1);
}
return destination;
}
TraitExpandChunk expand_type_expression_for_trait_param(std::shared_ptr<ast::TypeExpr> type_expression, mlc::HashMap<mlc::String, bool> explicit_type_parameter_environment, mlc::HashMap<mlc::String, bool> trait_declaration_names, mlc::HashMap<mlc::String, bool> nominal_type_declaration_names, int synthetic_counter) noexcept{
return std::visit(overloaded{[&](const ast::TyNamed& tyNamed) { auto [name] = tyNamed; return (explicit_type_parameter_environment.has(name) ? TraitExpandChunk{type_expression, synthetic_counter, {}, {}} : ((trait_declaration_names.has(name) && (!nominal_type_declaration_names.has(name))) ? [&]() {
auto synthetic_name = (mlc::String("__trait_param_", 14) + mlc::to_string(synthetic_counter));
auto single_trait_row = mlc::Array{name};
auto bounds_wrapper = mlc::Array{single_trait_row};
return TraitExpandChunk{std::make_shared<ast::TypeExpr>(ast::TyNamed{synthetic_name}), (synthetic_counter + 1), mlc::Array{synthetic_name}, bounds_wrapper};
}() : TraitExpandChunk{type_expression, synthetic_counter, {}, {}})); },
[&](const ast::TyArray& tyArray) { auto [inner] = tyArray; return [&]() {
auto inner_chunk = expand_type_expression_for_trait_param(inner, explicit_type_parameter_environment, trait_declaration_names, nominal_type_declaration_names, synthetic_counter);
return TraitExpandChunk{std::make_shared<ast::TypeExpr>(ast::TyArray{inner_chunk.type_expression}), inner_chunk.next_counter, inner_chunk.appended_type_parameter_names, inner_chunk.appended_trait_bounds_rows};
}(); },
[&](const ast::TyShared& tyShared) { auto [inner] = tyShared; return [&]() {
auto inner_chunk = expand_type_expression_for_trait_param(inner, explicit_type_parameter_environment, trait_declaration_names, nominal_type_declaration_names, synthetic_counter);
return TraitExpandChunk{std::make_shared<ast::TypeExpr>(ast::TyShared{inner_chunk.type_expression}), inner_chunk.next_counter, inner_chunk.appended_type_parameter_names, inner_chunk.appended_trait_bounds_rows};
}(); },
[&](const ast::TyGeneric& tyGeneric) { auto [base_name, type_arguments] = tyGeneric; return [&]() {
auto counter = synthetic_counter;
auto rebuilt_arguments = mlc::Array<std::shared_ptr<ast::TypeExpr>>{};
auto combined_appended_names = mlc::Array<mlc::String>{};
auto combined_bounds = mlc::Array<mlc::Array<mlc::String>>{};
auto argument_index = 0;
while ((argument_index < type_arguments.length())) {
auto argument_chunk = expand_type_expression_for_trait_param(type_arguments[argument_index], explicit_type_parameter_environment, trait_declaration_names, nominal_type_declaration_names, counter);
rebuilt_arguments.push_back(argument_chunk.type_expression);
counter = argument_chunk.next_counter;
combined_appended_names = append_strings(combined_appended_names, argument_chunk.appended_type_parameter_names);
combined_bounds = append_string_rows(combined_bounds, argument_chunk.appended_trait_bounds_rows);
argument_index = (argument_index + 1);
}
return TraitExpandChunk{std::make_shared<ast::TypeExpr>(ast::TyGeneric{base_name, rebuilt_arguments}), counter, combined_appended_names, combined_bounds};
}(); },
[&](const ast::TyFn& tyFn) { auto [parameter_types, return_type_expression] = tyFn; return [&]() {
auto counter = synthetic_counter;
auto rebuilt_parameter_types = mlc::Array<std::shared_ptr<ast::TypeExpr>>{};
auto combined_appended_names = mlc::Array<mlc::String>{};
auto combined_bounds = mlc::Array<mlc::Array<mlc::String>>{};
auto parameter_index = 0;
while ((parameter_index < parameter_types.length())) {
auto parameter_chunk = expand_type_expression_for_trait_param(parameter_types[parameter_index], explicit_type_parameter_environment, trait_declaration_names, nominal_type_declaration_names, counter);
rebuilt_parameter_types.push_back(parameter_chunk.type_expression);
counter = parameter_chunk.next_counter;
combined_appended_names = append_strings(combined_appended_names, parameter_chunk.appended_type_parameter_names);
combined_bounds = append_string_rows(combined_bounds, parameter_chunk.appended_trait_bounds_rows);
parameter_index = (parameter_index + 1);
}
auto return_chunk = expand_type_expression_for_trait_param(return_type_expression, explicit_type_parameter_environment, trait_declaration_names, nominal_type_declaration_names, counter);
return TraitExpandChunk{std::make_shared<ast::TypeExpr>(ast::TyFn{rebuilt_parameter_types, return_chunk.type_expression}), return_chunk.next_counter, append_strings(combined_appended_names, return_chunk.appended_type_parameter_names), append_string_rows(combined_bounds, return_chunk.appended_trait_bounds_rows)};
}(); },
[&](const ast::TyAssoc& tyAssoc) { auto [__0, __1] = tyAssoc; return TraitExpandChunk{type_expression, synthetic_counter, {}, {}}; },
[&](const ast::TyI32& tyI32) { return TraitExpandChunk{type_expression, synthetic_counter, {}, {}}; },
[&](const ast::TyString& tyString) { return TraitExpandChunk{type_expression, synthetic_counter, {}, {}}; },
[&](const ast::TyBool& tyBool) { return TraitExpandChunk{type_expression, synthetic_counter, {}, {}}; },
[&](const ast::TyUnit& tyUnit) { return TraitExpandChunk{type_expression, synthetic_counter, {}, {}}; }
}, (*type_expression));
}
std::shared_ptr<ast::Decl> expand_decl_fn_trait_parameters(mlc::String function_name, mlc::Array<mlc::String> type_parameter_names, mlc::Array<mlc::Array<mlc::String>> trait_bounds_rows, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<ast::TypeExpr> return_type_expression, std::shared_ptr<ast::Expr> body_expression, mlc::HashMap<mlc::String, bool> trait_declaration_names, mlc::HashMap<mlc::String, bool> nominal_type_declaration_names) noexcept{
auto explicit_type_parameter_environment = mlc::HashMap<mlc::String, bool>();
auto explicit_index = 0;
while ((explicit_index < type_parameter_names.length())) {
explicit_type_parameter_environment.set(type_parameter_names[explicit_index], true);
explicit_index = (explicit_index + 1);
}
auto expanded_type_parameter_names = mlc::Array<mlc::String>{};
auto copy_index = 0;
while ((copy_index < type_parameter_names.length())) {
expanded_type_parameter_names.push_back(type_parameter_names[copy_index]);
copy_index = (copy_index + 1);
}
auto expanded_trait_bounds = align_trait_bounds_matrix(type_parameter_names, trait_bounds_rows);
auto synthetic_counter = 0;
auto expanded_parameters = mlc::Array<std::shared_ptr<ast::Param>>{};
auto parameter_index = 0;
while ((parameter_index < parameters.length())) {
auto parameter_chunk = expand_type_expression_for_trait_param(ast::param_typ(parameters[parameter_index]), explicit_type_parameter_environment, trait_declaration_names, nominal_type_declaration_names, synthetic_counter);
synthetic_counter = parameter_chunk.next_counter;
expanded_type_parameter_names = append_strings(expanded_type_parameter_names, parameter_chunk.appended_type_parameter_names);
expanded_trait_bounds = append_string_rows(expanded_trait_bounds, parameter_chunk.appended_trait_bounds_rows);
auto original_parameter = parameters[parameter_index];
expanded_parameters.push_back(std::make_shared<ast::Param>(ast::Param{ast::param_name(original_parameter), original_parameter->is_mut, parameter_chunk.type_expression, original_parameter->has_default, original_parameter->default_}));
parameter_index = (parameter_index + 1);
}
auto return_chunk = expand_type_expression_for_trait_param(return_type_expression, explicit_type_parameter_environment, trait_declaration_names, nominal_type_declaration_names, synthetic_counter);
expanded_type_parameter_names = append_strings(expanded_type_parameter_names, return_chunk.appended_type_parameter_names);
expanded_trait_bounds = append_string_rows(expanded_trait_bounds, return_chunk.appended_trait_bounds_rows);
return std::make_shared<ast::Decl>(ast::DeclFn{function_name, expanded_type_parameter_names, expanded_trait_bounds, expanded_parameters, return_chunk.type_expression, body_expression});
}
mlc::Array<std::shared_ptr<ast::Decl>> expand_extend_methods(mlc::Array<std::shared_ptr<ast::Decl>> methods, mlc::HashMap<mlc::String, bool> trait_declaration_names, mlc::HashMap<mlc::String, bool> nominal_type_declaration_names) noexcept{
auto result = mlc::Array<std::shared_ptr<ast::Decl>>{};
auto method_index = 0;
while ((method_index < methods.length())) {
result.push_back(expand_decl_shared(methods[method_index], trait_declaration_names, nominal_type_declaration_names));
method_index = (method_index + 1);
}
return result;
}
std::shared_ptr<ast::Decl> expand_decl_shared(std::shared_ptr<ast::Decl> declaration, mlc::HashMap<mlc::String, bool> trait_declaration_names, mlc::HashMap<mlc::String, bool> nominal_type_declaration_names) noexcept{
return std::visit(overloaded{[&](const ast::DeclFn& declFn) { auto [function_name, type_parameter_names, trait_bounds_rows, parameters, return_type_expression, body_expression] = declFn; return expand_decl_fn_trait_parameters(function_name, type_parameter_names, trait_bounds_rows, parameters, return_type_expression, body_expression, trait_declaration_names, nominal_type_declaration_names); },
[&](const ast::DeclExported& declExported) { auto [inner] = declExported; return std::make_shared<ast::Decl>(ast::DeclExported{expand_decl_shared(inner, trait_declaration_names, nominal_type_declaration_names)}); },
[&](const ast::DeclExtend& declExtend) { auto [extended_type_name, implemented_trait_name, methods] = declExtend; return std::make_shared<ast::Decl>(ast::DeclExtend{extended_type_name, implemented_trait_name, expand_extend_methods(methods, trait_declaration_names, nominal_type_declaration_names)}); },
[&](const auto& __v) { return declaration; }
}, (*declaration));
}
mlc::Array<std::shared_ptr<ast::Decl>> expand_declarations_with_trait_and_nominal_maps(mlc::Array<std::shared_ptr<ast::Decl>> declarations, mlc::HashMap<mlc::String, bool> trait_declaration_names, mlc::HashMap<mlc::String, bool> nominal_type_declaration_names) noexcept{
auto expanded_declarations = mlc::Array<std::shared_ptr<ast::Decl>>{};
auto declaration_index = 0;
while ((declaration_index < declarations.length())) {
expanded_declarations.push_back(expand_decl_shared(declarations[declaration_index], trait_declaration_names, nominal_type_declaration_names));
declaration_index = (declaration_index + 1);
}
return expanded_declarations;
}
mlc::Array<std::shared_ptr<ast::Decl>> expand_declarations_with_trait_context(mlc::Array<std::shared_ptr<ast::Decl>> declarations, ast::Program program_for_maps) noexcept{
auto trait_declaration_names = mlc::HashMap<mlc::String, bool>();
auto nominal_type_declaration_names = mlc::HashMap<mlc::String, bool>();
fill_trait_and_nominal_maps(program_for_maps, trait_declaration_names, nominal_type_declaration_names);
return expand_declarations_with_trait_and_nominal_maps(declarations, trait_declaration_names, nominal_type_declaration_names);
}
ast::Program expand_trait_as_param_entry_using_full(ast::Program entry, ast::Program full_program) noexcept{
return ast::Program{expand_declarations_with_trait_context(entry.decls, full_program)};
}
ast::Program expand_trait_as_param_program(ast::Program program) noexcept{
return ast::Program{expand_declarations_with_trait_context(program.decls, program)};
}

} // namespace trait_param_expand
