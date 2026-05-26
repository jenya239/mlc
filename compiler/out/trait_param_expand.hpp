#ifndef TRAIT_PARAM_EXPAND_HPP
#define TRAIT_PARAM_EXPAND_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"

namespace trait_param_expand {

struct RecordLitPart;
struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;
struct CppStmt;
struct CppExpr;

struct TraitNominalMaps {mlc::HashMap<mlc::String, bool> trait_declaration_names;mlc::HashMap<mlc::String, bool> nominal_type_declaration_names;};

mlc::Array<ast::Diagnostic> trait_and_type_name_conflict_diagnostics(ast::Program program) noexcept;

trait_param_expand::TraitNominalMaps build_trait_nominal_maps(ast::Program program) noexcept;

mlc::Array<std::shared_ptr<ast::Decl>> expand_declarations_with_trait_nominal_maps(mlc::Array<std::shared_ptr<ast::Decl>> declarations, trait_param_expand::TraitNominalMaps maps) noexcept;

mlc::Array<std::shared_ptr<ast::Decl>> expand_declarations_with_trait_context(mlc::Array<std::shared_ptr<ast::Decl>> declarations, ast::Program program_for_maps) noexcept;

ast::Program expand_trait_as_param_entry_using_full(ast::Program entry, ast::Program full_program) noexcept;

ast::Program expand_trait_as_param_program(ast::Program program) noexcept;

} // namespace trait_param_expand

#endif // TRAIT_PARAM_EXPAND_HPP
