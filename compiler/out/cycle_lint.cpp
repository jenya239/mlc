#define main mlc_user_main
#include "cycle_lint.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "diagnostic_codes.hpp"

namespace cycle_lint {

using namespace ast;
using namespace registry;
using namespace diagnostic_codes;
using namespace ast_tokens;

mlc::Array<mlc::String> empty_name_list() noexcept{
  auto empty = mlc::Array<mlc::String>{};
  return empty;
}
mlc::Array<ast::Diagnostic> empty_diagnostic_list() noexcept{
  auto empty = mlc::Array<ast::Diagnostic>{};
  return empty;
}
mlc::Array<mlc::String> append_unique_name(mlc::Array<mlc::String> names, mlc::String name) noexcept{
  if ((name.length() == 0))   {
    return names;
  } else if (names.contains(name))   {
    return names;
  } else   {
    return names.concat(mlc::Array<mlc::String>{name});
  }
}
mlc::Array<mlc::String> named_targets_through_shared(std::shared_ptr<registry::Type> type_value, registry::TypeRegistry registry) noexcept{
  return [&]() -> mlc::Array<mlc::String> {
auto __match_subject = type_value;
if (std::holds_alternative<registry::TShared>((*__match_subject))) {
const registry::TShared& tShared = std::get<registry::TShared>((*__match_subject));
auto [inner] = tShared; return named_targets_inside_shared(inner, registry);
}
if (std::holds_alternative<registry::TArray>((*__match_subject))) {
const registry::TArray& tArray = std::get<registry::TArray>((*__match_subject));
auto [inner] = tArray; return named_targets_through_shared(inner, registry);
}
if (std::holds_alternative<registry::TGeneric>((*__match_subject))) {
const registry::TGeneric& tGeneric = std::get<registry::TGeneric>((*__match_subject));
auto [generic_name, type_arguments] = tGeneric; return named_targets_from_generic_through_shared(generic_name, type_arguments, registry);
}
if (std::holds_alternative<registry::TPair>((*__match_subject))) {
const registry::TPair& tPair = std::get<registry::TPair>((*__match_subject));
auto [left, right] = tPair; return named_targets_through_shared(left, registry).concat(named_targets_through_shared(right, registry));
}
if (std::holds_alternative<registry::TTuple>((*__match_subject))) {
const registry::TTuple& tTuple = std::get<registry::TTuple>((*__match_subject));
auto [members] = tTuple; return named_targets_from_tuple_through_shared(members, registry);
}
return empty_name_list();
std::abort();
}();
}
mlc::Array<mlc::String> named_targets_from_generic_through_shared(mlc::String generic_name, mlc::Array<std::shared_ptr<registry::Type>> type_arguments, registry::TypeRegistry registry) noexcept{
  if ((generic_name == mlc::String("Weak", 4)))   {
    return empty_name_list();
  }
  auto accumulated = empty_name_list();
  auto argument_index = 0;
  while ((argument_index < type_arguments.length()))   {
    (accumulated = accumulated.concat(named_targets_through_shared(type_arguments[argument_index], registry)));
    (argument_index = (argument_index + 1));
  }
  return accumulated;
}
mlc::Array<mlc::String> named_targets_from_tuple_through_shared(mlc::Array<std::shared_ptr<registry::Type>> members, registry::TypeRegistry registry) noexcept{
  auto accumulated = empty_name_list();
  auto member_index = 0;
  while ((member_index < members.length()))   {
    (accumulated = accumulated.concat(named_targets_through_shared(members[member_index], registry)));
    (member_index = (member_index + 1));
  }
  return accumulated;
}
mlc::Array<mlc::String> named_targets_inside_shared(std::shared_ptr<registry::Type> type_value, registry::TypeRegistry registry) noexcept{
  return [&]() -> mlc::Array<mlc::String> {
auto __match_subject = type_value;
if (std::holds_alternative<registry::TNamed>((*__match_subject))) {
const registry::TNamed& tNamed = std::get<registry::TNamed>((*__match_subject));
auto [name] = tNamed; return named_target_from_named(name, registry);
}
if (std::holds_alternative<registry::TShared>((*__match_subject))) {
const registry::TShared& tShared = std::get<registry::TShared>((*__match_subject));
auto [inner] = tShared; return named_targets_inside_shared(inner, registry);
}
if (std::holds_alternative<registry::TArray>((*__match_subject))) {
const registry::TArray& tArray = std::get<registry::TArray>((*__match_subject));
auto [inner] = tArray; return named_targets_inside_shared(inner, registry);
}
if (std::holds_alternative<registry::TGeneric>((*__match_subject))) {
const registry::TGeneric& tGeneric = std::get<registry::TGeneric>((*__match_subject));
auto [generic_name, type_arguments] = tGeneric; return named_targets_from_generic_inside_shared(generic_name, type_arguments, registry);
}
if (std::holds_alternative<registry::TPair>((*__match_subject))) {
const registry::TPair& tPair = std::get<registry::TPair>((*__match_subject));
auto [left, right] = tPair; return named_targets_inside_shared(left, registry).concat(named_targets_inside_shared(right, registry));
}
if (std::holds_alternative<registry::TTuple>((*__match_subject))) {
const registry::TTuple& tTuple = std::get<registry::TTuple>((*__match_subject));
auto [members] = tTuple; return named_targets_from_tuple_inside_shared(members, registry);
}
return empty_name_list();
std::abort();
}();
}
mlc::Array<mlc::String> named_target_from_named(mlc::String name, registry::TypeRegistry registry) noexcept{
  auto resolved = registry::resolve_nominal_type_name_for_fields(registry, name, mlc::Array<mlc::String>{});
  if (registry::TypeRegistry_has_fields(registry, resolved))   {
    return mlc::Array<mlc::String>{resolved};
  } else   {
    return empty_name_list();
  }
}
mlc::Array<mlc::String> named_targets_from_generic_inside_shared(mlc::String generic_name, mlc::Array<std::shared_ptr<registry::Type>> type_arguments, registry::TypeRegistry registry) noexcept{
  if ((generic_name == mlc::String("Weak", 4)))   {
    return empty_name_list();
  }
  auto accumulated = empty_name_list();
  auto argument_index = 0;
  while ((argument_index < type_arguments.length()))   {
    (accumulated = accumulated.concat(named_targets_inside_shared(type_arguments[argument_index], registry)));
    (argument_index = (argument_index + 1));
  }
  return accumulated;
}
mlc::Array<mlc::String> named_targets_from_tuple_inside_shared(mlc::Array<std::shared_ptr<registry::Type>> members, registry::TypeRegistry registry) noexcept{
  auto accumulated = empty_name_list();
  auto member_index = 0;
  while ((member_index < members.length()))   {
    (accumulated = accumulated.concat(named_targets_inside_shared(members[member_index], registry)));
    (member_index = (member_index + 1));
  }
  return accumulated;
}
mlc::Array<mlc::String> neighbors_for(mlc::HashMap<mlc::String, mlc::Array<mlc::String>> adjacency, mlc::String type_name) noexcept{
  if (adjacency.has(type_name))   {
    return adjacency.get(type_name);
  } else   {
    return empty_name_list();
  }
}
mlc::Array<mlc::String> append_record_type_name_if_present(mlc::Array<mlc::String> accumulated, std::shared_ptr<ast::Decl> declaration, registry::TypeRegistry registry) noexcept{
  return [&]() -> mlc::Array<mlc::String> {
auto __match_subject = ast::decl_inner(declaration);
if (std::holds_alternative<ast::DeclType>((*__match_subject))) {
const ast::DeclType& declType = std::get<ast::DeclType>((*__match_subject));
auto [type_name, __1, __2, __3, __4] = declType; return (registry::TypeRegistry_has_fields(registry, type_name) ? (append_unique_name(accumulated, type_name)) : (accumulated));
}
return accumulated;
std::abort();
}();
}
mlc::Array<mlc::String> record_type_names_from_program(ast::Program program, registry::TypeRegistry registry) noexcept{
  auto accumulated = empty_name_list();
  auto declaration_index = 0;
  while ((declaration_index < program.decls.length()))   {
    (accumulated = append_record_type_name_if_present(accumulated, program.decls[declaration_index], registry));
    (declaration_index = (declaration_index + 1));
  }
  return accumulated;
}
mlc::HashMap<mlc::String, mlc::Array<mlc::String>> build_shared_adjacency(mlc::Array<mlc::String> type_names, registry::TypeRegistry registry) noexcept{
  auto adjacency = mlc::HashMap<mlc::String, mlc::Array<mlc::String>>();
  auto type_index = 0;
  while ((type_index < type_names.length()))   {
    auto type_name = type_names[type_index];
    auto field_map = registry::TypeRegistry_fields_for(registry, type_name);
    auto field_names = registry::TypeRegistry_record_field_names_ordered_for(registry, type_name);
    auto targets = mlc::Array<mlc::String>{};
    auto field_index = 0;
    while ((field_index < field_names.length()))     {
      auto field_name = field_names[field_index];
      if (field_map.has(field_name))       {
        auto field_targets = named_targets_through_shared(field_map.get(field_name), registry);
        auto target_index = 0;
        [&]() {
while ((target_index < field_targets.length())) {
(targets = append_unique_name(targets, field_targets[target_index]));
(target_index = (target_index + 1));
}
}();
      }
      (field_index = (field_index + 1));
    }
    adjacency.set(type_name, targets);
    (type_index = (type_index + 1));
  }
  return adjacency;
}
bool can_reach_self(mlc::String start_name, mlc::String current_name, mlc::HashMap<mlc::String, mlc::Array<mlc::String>> adjacency, mlc::Array<mlc::String> path) noexcept{
  auto neighbors = neighbors_for(adjacency, current_name);
  auto neighbor_index = 0;
  auto found = false;
  while (((neighbor_index < neighbors.length()) && (!found)))   {
    auto neighbor = neighbors[neighbor_index];
    if ((neighbor == start_name))     {
      (found = true);
    } else if ((!path.contains(neighbor)))     {
      (found = can_reach_self(start_name, neighbor, adjacency, path.concat(mlc::Array<mlc::String>{neighbor})));
    }
    (neighbor_index = (neighbor_index + 1));
  }
  return found;
}
bool type_participates_in_shared_cycle(mlc::String type_name, mlc::HashMap<mlc::String, mlc::Array<mlc::String>> adjacency) noexcept{
  return can_reach_self(type_name, type_name, adjacency, mlc::Array<mlc::String>{type_name});
}
mlc::String cycle_warning_message(mlc::String type_name) noexcept{
  return ((mlc::String("recursive type cycle involving ", 31) + type_name) + mlc::String(" through Shared without Weak on the back edge; may leak. Consider Weak on the back edge.", 88));
}
mlc::Array<ast::Diagnostic> append_warning_for_matching_type(mlc::Array<ast::Diagnostic> accumulated, std::shared_ptr<ast::Decl> declaration, mlc::String type_name) noexcept{
  return [&]() -> mlc::Array<ast::Diagnostic> {
auto __match_subject = ast::decl_inner(declaration);
if (std::holds_alternative<ast::DeclType>((*__match_subject))) {
const ast::DeclType& declType = std::get<ast::DeclType>((*__match_subject));
auto [declared_name, __1, __2, __3, __4] = declType; return ((declared_name == type_name) ? (accumulated.concat(mlc::Array<ast::Diagnostic>{ast::diagnostic_new_with_code(mlc::String("warning", 7), cycle_warning_message(type_name), ast::decl_name_span(declaration), diagnostic_codes::diagnostic_code_w_cycle())})) : (accumulated));
}
return accumulated;
std::abort();
}();
}
mlc::Array<ast::Diagnostic> warning_for_type_declaration(ast::Program program, mlc::String type_name) noexcept{
  auto accumulated = empty_diagnostic_list();
  auto declaration_index = 0;
  while ((declaration_index < program.decls.length()))   {
    (accumulated = append_warning_for_matching_type(accumulated, program.decls[declaration_index], type_name));
    (declaration_index = (declaration_index + 1));
  }
  return accumulated;
}
mlc::Array<ast::Diagnostic> shared_cycle_lint_diagnostics(ast::Program program, registry::TypeRegistry registry) noexcept{
  auto type_names = record_type_names_from_program(program, registry);
  auto adjacency = build_shared_adjacency(type_names, registry);
  auto diagnostics = mlc::Array<ast::Diagnostic>{};
  auto emitted = mlc::Array<mlc::String>{};
  auto type_index = 0;
  while ((type_index < type_names.length()))   {
    auto type_name = type_names[type_index];
    if ((type_participates_in_shared_cycle(type_name, adjacency) && (!emitted.contains(type_name))))     {
      (diagnostics = ast::diagnostics_append(diagnostics, warning_for_type_declaration(program, type_name)));
      (emitted = emitted.concat(mlc::Array<mlc::String>{type_name}));
    }
    (type_index = (type_index + 1));
  }
  return diagnostics;
}

} // namespace cycle_lint
