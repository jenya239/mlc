#ifndef CHECK_HPP
#define CHECK_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "names.hpp"
#include "check_mutations.hpp"
#include "registry.hpp"
#include "infer.hpp"
#include "check_context.hpp"
#include "semantic_type_structure.hpp"

namespace check {

struct CheckOut {mlc::Array<mlc::String> errors;registry::TypeRegistry registry;};
bool param_defaults_in_tail(mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept;
void __skip__() noexcept;
void __skip__() noexcept;
bool default_expr_mvp_ok(std::shared_ptr<ast::Expr> e) noexcept;
bool is_extern_body(std::shared_ptr<ast::Expr> e) noexcept;
mlc::Array<ast::Diagnostic> param_default_diagnostics(mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<ast::Expr> body, registry::TypeRegistry registry) noexcept;
mlc::HashMap<mlc::String, bool> collect_globals(ast::Program program) noexcept;
bool type_is_checkable(std::shared_ptr<registry::Type> type_value, registry::TypeRegistry registry) noexcept;
bool CheckOut_has_errors(CheckOut self) noexcept;
ast::Result<CheckOut, mlc::Array<mlc::String>> check_program_against_full(ast::Program entry, ast::Program full_program) noexcept;
ast::Result<CheckOut, mlc::Array<mlc::String>> check_with_context(ast::Program entry, ast::Program full) noexcept;
ast::Result<CheckOut, mlc::Array<mlc::String>> check(ast::Program program) noexcept;

} // namespace check

#endif // CHECK_HPP
