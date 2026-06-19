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

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

semantic_ir::SemanticProgram transform_program(ast::Program program, registry::TypeRegistry registry) noexcept;

mlc::Array<semantic_ir::SemanticLoadItem> transform_load_items(mlc::Array<decl_index::LoadItem> items, registry::TypeRegistry registry, trait_param_expand::TraitNominalMaps trait_maps) noexcept;

} // namespace transform_decl

#endif // TRANSFORM_DECL_HPP
