#ifndef TRANSFORM_DECL_HPP
#define TRANSFORM_DECL_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "registry.hpp"
#include "semantic_ir.hpp"
#include "decl_index.hpp"
#include "transform.hpp"
#include "transform_stmts.hpp"

namespace transform_decl {

std::shared_ptr<semantic_ir::SDecl> transform_decl(std::shared_ptr<ast::Decl> declaration, registry::TypeRegistry registry) noexcept;
mlc::Array<std::shared_ptr<semantic_ir::SDecl>> transform_decls(mlc::Array<std::shared_ptr<ast::Decl>> declarations, registry::TypeRegistry registry) noexcept;
semantic_ir::SProgram transform_program(ast::Program program, registry::TypeRegistry registry) noexcept;
mlc::Array<semantic_ir::SNamespaceImportAlias> to_semantic_namespace_aliases(mlc::Array<decl_index::NamespaceImportAlias> items) noexcept;
mlc::Array<semantic_ir::SLoadItem> transform_load_items(mlc::Array<decl_index::LoadItem> items, registry::TypeRegistry registry) noexcept;

} // namespace transform_decl

#endif // TRANSFORM_DECL_HPP
