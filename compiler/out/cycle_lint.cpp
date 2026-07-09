#include "cycle_lint.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "diagnostic_codes.hpp"

namespace cycle_lint {

using namespace ast;
using namespace registry;
using namespace diagnostic_codes;
using namespace ast_tokens;

mlc::Array<mlc::String> empty_name_list() noexcept;

mlc::Array<ast::Diagnostic> empty_diagnostic_list() noexcept;

mlc::Array<mlc::String> append_unique_name(mlc::Array<mlc::String> names, mlc::String name) noexcept;

mlc::Array<mlc::String> named_targets_through_shared(std::shared_ptr<registry::Type> type_value, registry::TypeRegistry registry) noexcept;

mlc::Array<mlc::String> named_targets_from_generic_through_shared(mlc::String generic_name, mlc::Array<std::shared_ptr<registry::Type>> type_arguments, registry::TypeRegistry registry) noexcept;

mlc::Array<mlc::String> named_targets_from_tuple_through_shared(mlc::Array<std::shared_ptr<registry::Type>> members, registry::TypeRegistry registry) noexcept;

mlc::Array<mlc::String> named_targets_inside_shared(std::shared_ptr<registry::Type> type_value, registry::TypeRegistry registry) noexcept;

mlc::Array<mlc::String> named_target_from_named(mlc::String name, registry::TypeRegistry registry) noexcept;

mlc::Array<mlc::String> named_targets_from_generic_inside_shared(mlc::String generic_name, mlc::Array<std::shared_ptr<registry::Type>> type_arguments, registry::TypeRegistry registry) noexcept;

mlc::Array<mlc::String> named_targets_from_tuple_inside_shared(mlc::Array<std::shared_ptr<registry::Type>> members, registry::TypeRegistry registry) noexcept;

mlc::Array<mlc::String> neighbors_for(mlc::HashMap<mlc::String, mlc::Array<mlc::String>> adjacency, mlc::String type_name) noexcept;

mlc::Array<mlc::String> append_record_type_name_if_present(mlc::Array<mlc::String> accumulated, std::shared_ptr<ast::Decl> declaration, registry::TypeRegistry registry) noexcept;

mlc::Array<mlc::String> record_type_names_from_program(ast::Program program, registry::TypeRegistry registry) noexcept;

mlc::HashMap<mlc::String, mlc::Array<mlc::String>> build_shared_adjacency(mlc::Array<mlc::String> type_names, registry::TypeRegistry registry) noexcept;

bool can_reach_self(mlc::String start_name, mlc::String current_name, mlc::HashMap<mlc::String, mlc::Array<mlc::String>> adjacency, mlc::Array<mlc::String> path) noexcept;

bool type_participates_in_shared_cycle(mlc::String type_name, mlc::HashMap<mlc::String, mlc::Array<mlc::String>> adjacency) noexcept;

mlc::String cycle_warning_message(mlc::String type_name) noexcept;

mlc::Array<ast::Diagnostic> append_warning_for_matching_type(mlc::Array<ast::Diagnostic> accumulated, std::shared_ptr<ast::Decl> declaration, mlc::String type_name) noexcept;

mlc::Array<ast::Diagnostic> warning_for_type_declaration(ast::Program program, mlc::String type_name) noexcept;

mlc::Array<ast::Diagnostic> shared_cycle_lint_diagnostics(ast::Program program, registry::TypeRegistry registry) noexcept;

mlc::Array<mlc::String> empty_name_list() noexcept{
mlc::Array<mlc::String> empty = {};
return empty;
}

mlc::Array<ast::Diagnostic> empty_diagnostic_list() noexcept{
mlc::Array<ast::Diagnostic> empty = {};
return empty;
}

mlc::Array<mlc::String> append_unique_name(mlc::Array<mlc::String> names, mlc::String name) noexcept{return name.length() == 0 ? names : names.contains(name) ? names : names.concat(mlc::Array<mlc::String>{name});}

mlc::Array<mlc::String> named_targets_through_shared(std::shared_ptr<registry::Type> type_value, registry::TypeRegistry registry) noexcept{return [&]() -> mlc::Array<mlc::String> { if (std::holds_alternative<registry::TShared>((*type_value))) { auto _v_tshared = std::get<registry::TShared>((*type_value)); auto [inner] = _v_tshared; return named_targets_inside_shared(inner, registry); } if (std::holds_alternative<registry::TArray>((*type_value))) { auto _v_tarray = std::get<registry::TArray>((*type_value)); auto [inner] = _v_tarray; return named_targets_through_shared(inner, registry); } if (std::holds_alternative<registry::TGeneric>((*type_value))) { auto _v_tgeneric = std::get<registry::TGeneric>((*type_value)); auto [generic_name, type_arguments] = _v_tgeneric; return named_targets_from_generic_through_shared(generic_name, type_arguments, registry); } if (std::holds_alternative<registry::TPair>((*type_value))) { auto _v_tpair = std::get<registry::TPair>((*type_value)); auto [left, right] = _v_tpair; return named_targets_through_shared(left, registry).concat(named_targets_through_shared(right, registry)); } if (std::holds_alternative<registry::TTuple>((*type_value))) { auto _v_ttuple = std::get<registry::TTuple>((*type_value)); auto [members] = _v_ttuple; return named_targets_from_tuple_through_shared(members, registry); } return empty_name_list(); }();}

mlc::Array<mlc::String> named_targets_from_generic_through_shared(mlc::String generic_name, mlc::Array<std::shared_ptr<registry::Type>> type_arguments, registry::TypeRegistry registry) noexcept{
if (generic_name == mlc::String("Weak")){
{
return empty_name_list();
}
}
mlc::Array<mlc::String> accumulated = empty_name_list();
int argument_index = 0;
while (argument_index < type_arguments.size()){
{
accumulated = accumulated.concat(named_targets_through_shared(type_arguments[argument_index], registry));
argument_index = argument_index + 1;
}
}
return accumulated;
}

mlc::Array<mlc::String> named_targets_from_tuple_through_shared(mlc::Array<std::shared_ptr<registry::Type>> members, registry::TypeRegistry registry) noexcept{
mlc::Array<mlc::String> accumulated = empty_name_list();
int member_index = 0;
while (member_index < members.size()){
{
accumulated = accumulated.concat(named_targets_through_shared(members[member_index], registry));
member_index = member_index + 1;
}
}
return accumulated;
}

mlc::Array<mlc::String> named_targets_inside_shared(std::shared_ptr<registry::Type> type_value, registry::TypeRegistry registry) noexcept{return [&]() -> mlc::Array<mlc::String> { if (std::holds_alternative<registry::TNamed>((*type_value))) { auto _v_tnamed = std::get<registry::TNamed>((*type_value)); auto [name] = _v_tnamed; return named_target_from_named(name, registry); } if (std::holds_alternative<registry::TShared>((*type_value))) { auto _v_tshared = std::get<registry::TShared>((*type_value)); auto [inner] = _v_tshared; return named_targets_inside_shared(inner, registry); } if (std::holds_alternative<registry::TArray>((*type_value))) { auto _v_tarray = std::get<registry::TArray>((*type_value)); auto [inner] = _v_tarray; return named_targets_inside_shared(inner, registry); } if (std::holds_alternative<registry::TGeneric>((*type_value))) { auto _v_tgeneric = std::get<registry::TGeneric>((*type_value)); auto [generic_name, type_arguments] = _v_tgeneric; return named_targets_from_generic_inside_shared(generic_name, type_arguments, registry); } if (std::holds_alternative<registry::TPair>((*type_value))) { auto _v_tpair = std::get<registry::TPair>((*type_value)); auto [left, right] = _v_tpair; return named_targets_inside_shared(left, registry).concat(named_targets_inside_shared(right, registry)); } if (std::holds_alternative<registry::TTuple>((*type_value))) { auto _v_ttuple = std::get<registry::TTuple>((*type_value)); auto [members] = _v_ttuple; return named_targets_from_tuple_inside_shared(members, registry); } return empty_name_list(); }();}

mlc::Array<mlc::String> named_target_from_named(mlc::String name, registry::TypeRegistry registry) noexcept{
mlc::String resolved = registry::resolve_nominal_type_name_for_fields(registry, name, {});
return registry::TypeRegistry_has_fields(registry, resolved) ? mlc::Array<mlc::String>{resolved} : empty_name_list();
}

mlc::Array<mlc::String> named_targets_from_generic_inside_shared(mlc::String generic_name, mlc::Array<std::shared_ptr<registry::Type>> type_arguments, registry::TypeRegistry registry) noexcept{
if (generic_name == mlc::String("Weak")){
{
return empty_name_list();
}
}
mlc::Array<mlc::String> accumulated = empty_name_list();
int argument_index = 0;
while (argument_index < type_arguments.size()){
{
accumulated = accumulated.concat(named_targets_inside_shared(type_arguments[argument_index], registry));
argument_index = argument_index + 1;
}
}
return accumulated;
}

mlc::Array<mlc::String> named_targets_from_tuple_inside_shared(mlc::Array<std::shared_ptr<registry::Type>> members, registry::TypeRegistry registry) noexcept{
mlc::Array<mlc::String> accumulated = empty_name_list();
int member_index = 0;
while (member_index < members.size()){
{
accumulated = accumulated.concat(named_targets_inside_shared(members[member_index], registry));
member_index = member_index + 1;
}
}
return accumulated;
}

mlc::Array<mlc::String> neighbors_for(mlc::HashMap<mlc::String, mlc::Array<mlc::String>> adjacency, mlc::String type_name) noexcept{return adjacency.has(type_name) ? adjacency.get(type_name) : empty_name_list();}

mlc::Array<mlc::String> append_record_type_name_if_present(mlc::Array<mlc::String> accumulated, std::shared_ptr<ast::Decl> declaration, registry::TypeRegistry registry) noexcept{return [&]() -> mlc::Array<mlc::String> { if (std::holds_alternative<ast::DeclType>((*ast::decl_inner(declaration)))) { auto _v_decltype = std::get<ast::DeclType>((*ast::decl_inner(declaration))); auto [type_name, _w0, _w1, _w2, _w3] = _v_decltype; return registry::TypeRegistry_has_fields(registry, type_name) ? append_unique_name(accumulated, type_name) : accumulated; } return accumulated; }();}

mlc::Array<mlc::String> record_type_names_from_program(ast::Program program, registry::TypeRegistry registry) noexcept{
mlc::Array<mlc::String> accumulated = empty_name_list();
int declaration_index = 0;
while (declaration_index < program.decls.size()){
{
accumulated = append_record_type_name_if_present(accumulated, program.decls[declaration_index], registry);
declaration_index = declaration_index + 1;
}
}
return accumulated;
}

mlc::HashMap<mlc::String, mlc::Array<mlc::String>> build_shared_adjacency(mlc::Array<mlc::String> type_names, registry::TypeRegistry registry) noexcept{
mlc::HashMap<mlc::String, mlc::Array<mlc::String>> adjacency = mlc::HashMap<mlc::String, mlc::Array<mlc::String>>();
int type_index = 0;
while (type_index < type_names.size()){
{
mlc::String type_name = type_names[type_index];
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> field_map = registry::TypeRegistry_fields_for(registry, type_name);
mlc::Array<mlc::String> field_names = registry::TypeRegistry_record_field_names_ordered_for(registry, type_name);
mlc::Array<mlc::String> targets = {};
int field_index = 0;
while (field_index < field_names.size()){
{
mlc::String field_name = field_names[field_index];
if (field_map.has(field_name)){
{
mlc::Array<mlc::String> field_targets = named_targets_through_shared(field_map.get(field_name), registry);
int target_index = 0;
[&]() { 
  while (target_index < field_targets.size()){
{
targets = append_unique_name(targets, field_targets[target_index]);
target_index = target_index + 1;
}
}
 }();
}
}
field_index = field_index + 1;
}
}
adjacency.set(type_name, targets);
type_index = type_index + 1;
}
}
return adjacency;
}

bool can_reach_self(mlc::String start_name, mlc::String current_name, mlc::HashMap<mlc::String, mlc::Array<mlc::String>> adjacency, mlc::Array<mlc::String> path) noexcept{
mlc::Array<mlc::String> neighbors = neighbors_for(adjacency, current_name);
int neighbor_index = 0;
bool found = false;
while (neighbor_index < neighbors.size() && !found){
{
mlc::String neighbor = neighbors[neighbor_index];
if (neighbor == start_name){
{
found = true;
}
} else {
{
if (!path.contains(neighbor)){
found = can_reach_self(start_name, neighbor, adjacency, path.concat(mlc::Array<mlc::String>{neighbor}));
}
}
}
neighbor_index = neighbor_index + 1;
}
}
return found;
}

bool type_participates_in_shared_cycle(mlc::String type_name, mlc::HashMap<mlc::String, mlc::Array<mlc::String>> adjacency) noexcept{return can_reach_self(type_name, type_name, adjacency, mlc::Array<mlc::String>{type_name});}

mlc::String cycle_warning_message(mlc::String type_name) noexcept{return mlc::String("recursive type cycle involving ") + type_name + mlc::String(" through Shared without Weak on the back edge; may leak. Consider Weak on the back edge.");}

mlc::Array<ast::Diagnostic> append_warning_for_matching_type(mlc::Array<ast::Diagnostic> accumulated, std::shared_ptr<ast::Decl> declaration, mlc::String type_name) noexcept{return [&]() -> mlc::Array<ast::Diagnostic> { if (std::holds_alternative<ast::DeclType>((*ast::decl_inner(declaration)))) { auto _v_decltype = std::get<ast::DeclType>((*ast::decl_inner(declaration))); auto [declared_name, _w0, _w1, _w2, _w3] = _v_decltype; return declared_name == type_name ? accumulated.concat(mlc::Array<ast::Diagnostic>{ast::diagnostic_new_with_code(mlc::String("warning"), cycle_warning_message(type_name), ast::decl_name_span(declaration), diagnostic_codes::diagnostic_code_w_cycle())}) : accumulated; } return accumulated; }();}

mlc::Array<ast::Diagnostic> warning_for_type_declaration(ast::Program program, mlc::String type_name) noexcept{
mlc::Array<ast::Diagnostic> accumulated = empty_diagnostic_list();
int declaration_index = 0;
while (declaration_index < program.decls.size()){
{
accumulated = append_warning_for_matching_type(accumulated, program.decls[declaration_index], type_name);
declaration_index = declaration_index + 1;
}
}
return accumulated;
}

mlc::Array<ast::Diagnostic> shared_cycle_lint_diagnostics(ast::Program program, registry::TypeRegistry registry) noexcept{
mlc::Array<mlc::String> type_names = record_type_names_from_program(program, registry);
mlc::HashMap<mlc::String, mlc::Array<mlc::String>> adjacency = build_shared_adjacency(type_names, registry);
mlc::Array<ast::Diagnostic> diagnostics = {};
mlc::Array<mlc::String> emitted = {};
int type_index = 0;
while (type_index < type_names.size()){
{
mlc::String type_name = type_names[type_index];
if (type_participates_in_shared_cycle(type_name, adjacency) && !emitted.contains(type_name)){
{
diagnostics = ast::diagnostics_append(diagnostics, warning_for_type_declaration(program, type_name));
emitted = emitted.concat(mlc::Array<mlc::String>{type_name});
}
}
type_index = type_index + 1;
}
}
return diagnostics;
}

} // namespace cycle_lint
