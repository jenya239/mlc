#ifndef TRAIT_PARAM_EXPAND_HPP
#define TRAIT_PARAM_EXPAND_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"

namespace trait_param_expand {

struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

mlc::Array<ast::Diagnostic> trait_and_type_name_conflict_diagnostics(ast::Program program) noexcept;

mlc::Array<std::shared_ptr<ast::Decl>> expand_declarations_with_trait_context(mlc::Array<std::shared_ptr<ast::Decl>> declarations, ast::Program program_for_maps) noexcept;

ast::Program expand_trait_as_param_entry_using_full(ast::Program entry, ast::Program full_program) noexcept;

ast::Program expand_trait_as_param_program(ast::Program program) noexcept;

} // namespace trait_param_expand

#endif // TRAIT_PARAM_EXPAND_HPP
