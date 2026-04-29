#include "transform_decl.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "semantic_ir.hpp"
#include "decl_index.hpp"
#include "transform.hpp"
#include "transform_stmts.hpp"

namespace transform_decl {

using namespace ast;
using namespace registry;
using namespace semantic_ir;
using namespace decl_index;
using namespace transform;
using namespace transform_stmts;
using namespace ast_tokens;

std::shared_ptr<semantic_ir::SDecl> transform_decl(std::shared_ptr<ast::Decl> declaration, registry::TypeRegistry registry) noexcept;

mlc::Array<std::shared_ptr<semantic_ir::SDecl>> transform_decls(mlc::Array<std::shared_ptr<ast::Decl>> declarations, registry::TypeRegistry registry) noexcept;

semantic_ir::SProgram transform_program(ast::Program program, registry::TypeRegistry registry) noexcept;

mlc::Array<semantic_ir::SNamespaceImportAlias> to_semantic_namespace_aliases(mlc::Array<decl_index::NamespaceImportAlias> items) noexcept;

mlc::Array<semantic_ir::SLoadItem> transform_load_items(mlc::Array<decl_index::LoadItem> items, registry::TypeRegistry registry) noexcept;

std::shared_ptr<semantic_ir::SDecl> transform_decl(std::shared_ptr<ast::Decl> declaration, registry::TypeRegistry registry) noexcept{return std::visit(overloaded{
  [&](const DeclFn& declfn) -> std::shared_ptr<semantic_ir::SDecl> { auto [name, type_params, trait_bounds, params, return_type_expr, body] = declfn; return [&]() -> std::shared_ptr<semantic_ir::SDecl> { 
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> param_env = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
  int index = 0;
  while (index < params.size()){
{
param_env.set(params[index]->name, registry::type_from_annotation(params[index]->typ));
index = index + 1;
}
}
  std::shared_ptr<registry::Type> return_type = registry::type_from_annotation(return_type_expr);
  transform::TransformContext initial_context = transform::TransformContext{param_env, registry};
  std::shared_ptr<semantic_ir::SExpr> typed_body = transform::transform_expr(body, initial_context, [](mlc::Array<std::shared_ptr<ast::Stmt>> stmts, transform::TransformContext ctx)  { return transform_stmts::transform_stmts(stmts, ctx); });
  std::shared_ptr<semantic_ir::SExpr> coerced_body = transform::coerce_expr_to_type(typed_body, return_type);
  return std::make_shared<semantic_ir::SDecl>(semantic_ir::SDeclFn(name, type_params, trait_bounds, params, return_type, coerced_body));
 }(); },
  [&](const DeclType& decltype_) -> std::shared_ptr<semantic_ir::SDecl> { auto [type_name, type_params, variants, derive_traits] = decltype_; return std::make_shared<semantic_ir::SDecl>(semantic_ir::SDeclType(type_name, type_params, variants, derive_traits)); },
  [&](const DeclTrait& decltrait) -> std::shared_ptr<semantic_ir::SDecl> { auto [trait_name, type_params, methods] = decltrait; return [&]() -> std::shared_ptr<semantic_ir::SDecl> { 
  mlc::Array<std::shared_ptr<semantic_ir::SDecl>> typed_methods = transform_decls(methods, registry);
  return std::make_shared<semantic_ir::SDecl>(semantic_ir::SDeclTrait(trait_name, type_params, typed_methods));
 }(); },
  [&](const DeclExtend& declextend) -> std::shared_ptr<semantic_ir::SDecl> { auto [type_name, trait_name, methods] = declextend; return [&]() -> std::shared_ptr<semantic_ir::SDecl> { 
  mlc::Array<std::shared_ptr<semantic_ir::SDecl>> typed_methods = transform_decls(methods, registry);
  return std::make_shared<semantic_ir::SDecl>(semantic_ir::SDeclExtend(type_name, trait_name, typed_methods));
 }(); },
  [&](const DeclImport& declimport) -> std::shared_ptr<semantic_ir::SDecl> { auto [path, names] = declimport; return std::make_shared<semantic_ir::SDecl>(semantic_ir::SDeclImport(path, names)); },
  [&](const DeclExported& declexported) -> std::shared_ptr<semantic_ir::SDecl> { auto [inner] = declexported; return std::make_shared<semantic_ir::SDecl>(semantic_ir::SDeclExported(transform_decl(inner, registry))); }
}, (*declaration));}

mlc::Array<std::shared_ptr<semantic_ir::SDecl>> transform_decls(mlc::Array<std::shared_ptr<ast::Decl>> declarations, registry::TypeRegistry registry) noexcept{
mlc::Array<std::shared_ptr<semantic_ir::SDecl>> result = {};
int index = 0;
while (index < declarations.size()){
{
result.push_back(transform_decl(declarations[index], registry));
index = index + 1;
}
}
return result;
}

semantic_ir::SProgram transform_program(ast::Program program, registry::TypeRegistry registry) noexcept{return semantic_ir::SProgram{transform_decls(program.decls, registry)};}

mlc::Array<semantic_ir::SNamespaceImportAlias> to_semantic_namespace_aliases(mlc::Array<decl_index::NamespaceImportAlias> items) noexcept{
mlc::Array<semantic_ir::SNamespaceImportAlias> result = {};
int index = 0;
while (index < items.size()){
{
decl_index::NamespaceImportAlias entry = items[index];
result.push_back(semantic_ir::SNamespaceImportAlias{entry.alias, entry.module_path});
index = index + 1;
}
}
return result;
}

mlc::Array<semantic_ir::SLoadItem> transform_load_items(mlc::Array<decl_index::LoadItem> items, registry::TypeRegistry registry) noexcept{
mlc::Array<semantic_ir::SLoadItem> result = {};
int index = 0;
while (index < items.size()){
{
decl_index::LoadItem item = items[index];
mlc::Array<std::shared_ptr<semantic_ir::SDecl>> typed_decls = transform_decls(item.decls, registry);
result.push_back(semantic_ir::SLoadItem{item.path, typed_decls, item.imports, to_semantic_namespace_aliases(item.namespace_import_aliases)});
index = index + 1;
}
}
return result;
}

} // namespace transform_decl
