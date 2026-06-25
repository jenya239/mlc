#include "transform_decl.hpp"

#include "ast.hpp"
#include "trait_param_expand.hpp"
#include "param_destructure_expand.hpp"
#include "registry.hpp"
#include "semantic_ir.hpp"
#include "load_item.hpp"
#include "transform.hpp"
#include "transform_stmts.hpp"

namespace transform_decl {

using namespace ast;
using namespace trait_param_expand;
using namespace param_destructure_expand;
using namespace registry;
using namespace semantic_ir;
using namespace load_item;
using namespace transform;
using namespace transform_stmts;
using namespace ast_tokens;

std::shared_ptr<semantic_ir::SemanticDeclaration> transform_decl(std::shared_ptr<ast::Decl> declaration, registry::TypeRegistry registry) noexcept;

mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> transform_decls(mlc::Array<std::shared_ptr<ast::Decl>> declarations, registry::TypeRegistry registry) noexcept;

semantic_ir::SemanticProgram transform_program(ast::Program program, registry::TypeRegistry registry) noexcept;

mlc::Array<semantic_ir::SemanticNamespaceImportAlias> to_semantic_namespace_aliases(mlc::Array<load_item::NamespaceImportAlias> items) noexcept;

mlc::Array<semantic_ir::SemanticLoadItem> transform_load_items(mlc::Array<load_item::LoadItem> items, registry::TypeRegistry registry, trait_param_expand::TraitNominalMaps trait_maps) noexcept;

std::shared_ptr<semantic_ir::SemanticDeclaration> transform_decl(std::shared_ptr<ast::Decl> declaration, registry::TypeRegistry registry) noexcept{return std::visit(overloaded{
  [&](const DeclFn& declfn) -> std::shared_ptr<semantic_ir::SemanticDeclaration> { auto [name, type_params, trait_bounds, params, return_type_expr, body, where_clause_bounds_entries] = declfn; return [&]() -> std::shared_ptr<semantic_ir::SemanticDeclaration> { 
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> param_env = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
  int index = 0;
  while (index < params.size()){
{
param_env.set(params[index]->name, registry::type_from_annotation_with_registry(params[index]->type_value, registry));
index = index + 1;
}
}
  std::shared_ptr<registry::Type> return_type = registry::type_from_annotation_with_registry(return_type_expr, registry);
  transform::TransformContext initial_context = transform::TransformContext{param_env, registry, {}};
  std::shared_ptr<semantic_ir::SemanticExpression> typed_body = transform::transform_expr(body, initial_context, [](mlc::Array<std::shared_ptr<ast::Stmt>> statements, transform::TransformContext transform_context) mutable { return transform_stmts::transform_stmts(statements, transform_context); });
  std::shared_ptr<semantic_ir::SemanticExpression> coerced_body = transform::coerce_expr_to_type(typed_body, return_type);
  return std::make_shared<semantic_ir::SemanticDeclaration>(semantic_ir::SemanticDeclarationFn(name, type_params, trait_bounds, params, return_type, coerced_body, where_clause_bounds_entries, ast::expr_span(body)));
 }(); },
  [&](const DeclType& decltype_) -> std::shared_ptr<semantic_ir::SemanticDeclaration> { auto [type_name, type_params, variants, derive_traits, name_span] = decltype_; return std::make_shared<semantic_ir::SemanticDeclaration>(semantic_ir::SemanticDeclarationType(type_name, type_params, variants, derive_traits, name_span)); },
  [&](const DeclTypeAlias& decltypealias) -> std::shared_ptr<semantic_ir::SemanticDeclaration> { auto [type_name, type_params, type_expression, name_span] = decltypealias; return std::make_shared<semantic_ir::SemanticDeclaration>(semantic_ir::SemanticDeclarationTypeAlias(type_name, type_params, type_expression, name_span)); },
  [&](const DeclTrait& decltrait) -> std::shared_ptr<semantic_ir::SemanticDeclaration> { auto [trait_name, type_params, methods, name_span] = decltrait; return [&]() -> std::shared_ptr<semantic_ir::SemanticDeclaration> { 
  mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> typed_methods = transform_decls(methods, registry);
  return std::make_shared<semantic_ir::SemanticDeclaration>(semantic_ir::SemanticDeclarationTrait(trait_name, type_params, typed_methods, name_span));
 }(); },
  [&](const DeclExtend& declextend) -> std::shared_ptr<semantic_ir::SemanticDeclaration> { auto [type_name, trait_name, methods, name_span] = declextend; return [&]() -> std::shared_ptr<semantic_ir::SemanticDeclaration> { 
  mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> typed_methods = transform_decls(methods, registry);
  return std::make_shared<semantic_ir::SemanticDeclaration>(semantic_ir::SemanticDeclarationExtend(type_name, trait_name, typed_methods, name_span));
 }(); },
  [&](const DeclImport& declimport) -> std::shared_ptr<semantic_ir::SemanticDeclaration> { auto [path, names] = declimport; return std::make_shared<semantic_ir::SemanticDeclaration>(semantic_ir::SemanticDeclarationImport(path, names)); },
  [&](const DeclExported& declexported) -> std::shared_ptr<semantic_ir::SemanticDeclaration> { auto [exported_declaration] = declexported; return std::make_shared<semantic_ir::SemanticDeclaration>(semantic_ir::SemanticDeclarationExported(transform_decl(exported_declaration, registry))); },
  [&](const DeclAssocType& declassoctype) -> std::shared_ptr<semantic_ir::SemanticDeclaration> { auto [_w0, _w1] = declassoctype; return std::make_shared<semantic_ir::SemanticDeclaration>(semantic_ir::SemanticDeclarationImport(mlc::String(""), {})); },
  [&](const DeclAssocBind& declassocbind) -> std::shared_ptr<semantic_ir::SemanticDeclaration> { auto [name, type_expr, span] = declassocbind; return std::make_shared<semantic_ir::SemanticDeclaration>(semantic_ir::SemanticDeclarationAssocBind(name, registry::type_from_annotation_with_registry(type_expr, registry), span)); }
}, (*declaration));}

mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> transform_decls(mlc::Array<std::shared_ptr<ast::Decl>> declarations, registry::TypeRegistry registry) noexcept{return declarations.fold([&]() -> mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> { 
  mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> empty_declarations = {};
  return empty_declarations;
 }(), [registry](mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> result, std::shared_ptr<ast::Decl> declaration) mutable { return result.concat(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{transform_decl(declaration, registry)}); });}

semantic_ir::SemanticProgram transform_program(ast::Program program, registry::TypeRegistry registry) noexcept{return semantic_ir::SemanticProgram{transform_decls(program.decls, registry)};}

mlc::Array<semantic_ir::SemanticNamespaceImportAlias> to_semantic_namespace_aliases(mlc::Array<load_item::NamespaceImportAlias> items) noexcept{return items.fold([&]() -> mlc::Array<semantic_ir::SemanticNamespaceImportAlias> { 
  mlc::Array<semantic_ir::SemanticNamespaceImportAlias> empty_aliases = {};
  return empty_aliases;
 }(), [](mlc::Array<semantic_ir::SemanticNamespaceImportAlias> result, load_item::NamespaceImportAlias entry) mutable { return result.concat(mlc::Array<semantic_ir::SemanticNamespaceImportAlias>{semantic_ir::SemanticNamespaceImportAlias{entry.alias, entry.module_path}}); });}

mlc::Array<semantic_ir::SemanticLoadItem> transform_load_items(mlc::Array<load_item::LoadItem> items, registry::TypeRegistry registry, trait_param_expand::TraitNominalMaps trait_maps) noexcept{return items.fold([&]() -> mlc::Array<semantic_ir::SemanticLoadItem> { 
  mlc::Array<semantic_ir::SemanticLoadItem> empty_items = {};
  return empty_items;
 }(), [trait_maps, registry](mlc::Array<semantic_ir::SemanticLoadItem> result, load_item::LoadItem item) mutable { return [&]() -> mlc::Array<semantic_ir::SemanticLoadItem> { 
  mlc::Array<std::shared_ptr<ast::Decl>> destructured_entry_declarations = param_destructure_expand::expand_parameter_destructuring_in_program(ast::Program{item.decls}).decls;
  mlc::Array<std::shared_ptr<ast::Decl>> expanded_declarations = trait_param_expand::expand_declarations_with_trait_nominal_maps(destructured_entry_declarations, trait_maps);
  mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> typed_declarations = transform_decls(expanded_declarations, registry);
  return result.concat(mlc::Array<semantic_ir::SemanticLoadItem>{semantic_ir::SemanticLoadItem{item.path, typed_declarations, item.imports, to_semantic_namespace_aliases(item.namespace_import_aliases)}});
 }(); });}

} // namespace transform_decl
