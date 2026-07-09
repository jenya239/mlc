#ifndef ORPHAN_LINT_HPP
#define ORPHAN_LINT_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
#include "registry.hpp"
#include "diagnostic_codes.hpp"
namespace orphan_lint {

mlc::Array<ast::Diagnostic> empty_diagnostic_list() noexcept;
bool extend_module_owns_type_or_trait(registry::TypeRegistry registry, mlc::String type_name, mlc::String trait_name, mlc::String extend_path) noexcept;
bool extend_is_orphan_impl(registry::TypeRegistry registry, mlc::String type_name, mlc::String trait_name, mlc::String extend_path) noexcept;
ast::Diagnostic orphan_diagnostic_for_extend(mlc::String type_name, mlc::String trait_name, ast::Span extend_span) noexcept;
mlc::Array<ast::Diagnostic> orphan_diagnostics_for_declaration(std::shared_ptr<ast::Decl> declaration, registry::TypeRegistry registry) noexcept;
mlc::Array<ast::Diagnostic> orphan_impl_diagnostics(ast::Program program, registry::TypeRegistry registry) noexcept;

} // namespace orphan_lint

#endif // ORPHAN_LINT_HPP
