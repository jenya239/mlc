#ifndef TRANSFORM_DECL_HPP
#define TRANSFORM_DECL_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
#include "trait_param_expand.hpp"
#include "param_destructure_expand.hpp"
#include "registry.hpp"
#include "semantic_ir.hpp"
#include "load_item.hpp"
#include "transform.hpp"
#include "transform_stmts.hpp"
#include "escape_analysis.hpp"
namespace transform_decl {

bool string_list_contains(mlc::Array<mlc::String> haystack, mlc::String needle) noexcept;
semantic_ir::FnEscapeInfo fn_escape_info_for_declaration(std::shared_ptr<ast::Decl> declaration, mlc::Array<mlc::String> functions_used_as_values, bool apply_escape_templates) noexcept;
std::shared_ptr<semantic_ir::SemanticDeclaration> transform_decl(std::shared_ptr<ast::Decl> declaration, registry::TypeRegistry registry, mlc::Array<mlc::String> functions_used_as_values, bool apply_escape_templates) noexcept;
mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> transform_decls(mlc::Array<std::shared_ptr<ast::Decl>> declarations, registry::TypeRegistry registry, mlc::Array<mlc::String> functions_used_as_values, bool apply_escape_templates) noexcept;
semantic_ir::SemanticProgram transform_program(ast::Program program, registry::TypeRegistry registry) noexcept;
mlc::Array<semantic_ir::SemanticNamespaceImportAlias> to_semantic_namespace_aliases(mlc::Array<load_item::NamespaceImportAlias> items) noexcept;
mlc::Array<semantic_ir::SemanticLoadItem> transform_load_items(mlc::Array<load_item::LoadItem> items, registry::TypeRegistry registry, trait_param_expand::TraitNominalMaps trait_maps) noexcept;

} // namespace transform_decl

#endif // TRANSFORM_DECL_HPP
