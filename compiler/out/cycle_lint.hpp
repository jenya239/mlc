#ifndef CYCLE_LINT_HPP
#define CYCLE_LINT_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
#include "registry.hpp"
#include "diagnostic_codes.hpp"
namespace cycle_lint {

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

} // namespace cycle_lint

#endif // CYCLE_LINT_HPP
