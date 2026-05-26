#ifndef TRANSFORM_DECL_HPP
#define TRANSFORM_DECL_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "trait_param_expand.hpp"
#include "param_destructure_expand.hpp"
#include "registry.hpp"
#include "semantic_ir.hpp"
#include "decl_index.hpp"
#include "transform.hpp"
#include "transform_stmts.hpp"

namespace transform_decl {

struct RecordLitPart;
struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;
struct CppStmt;
struct CppExpr;

semantic_ir::SProgram transform_program(ast::Program program, registry::TypeRegistry registry) noexcept;

mlc::Array<semantic_ir::SLoadItem> transform_load_items(mlc::Array<decl_index::LoadItem> items, registry::TypeRegistry registry, trait_param_expand::TraitNominalMaps trait_maps) noexcept;

} // namespace transform_decl

#endif // TRANSFORM_DECL_HPP
