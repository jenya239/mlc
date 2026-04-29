#define main mlc_user_main
#include "transform_decl.hpp"

namespace transform_decl {

std::shared_ptr<semantic_ir::SDecl> transform_decl(std::shared_ptr<ast::Decl> declaration, registry::TypeRegistry registry) noexcept{
return std::visit(overloaded{[&](const ast::DeclFn& declFn) { auto [name, type_params, trait_bounds, params, return_type_expr, body] = declFn; return [&]() {
auto param_env = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
auto index = 0;
while ((index < params.length())) {
param_env.set(params[index]->name, registry::type_from_annotation(params[index]->typ));
index = (index + 1);
}
auto return_type = registry::type_from_annotation(return_type_expr);
auto initial_context = transform::TransformContext{param_env, registry};
auto typed_body = transform::transform_expr(body, initial_context, [=](auto stmts, auto ctx) { return transform_stmts::transform_stmts(stmts, ctx); });
return std::make_shared<semantic_ir::SDecl>(semantic_ir::SDeclFn{name, type_params, trait_bounds, params, return_type, typed_body});
}(); },
[&](const ast::DeclType& declType) { auto [type_name, type_params, variants, derive_traits] = declType; return std::make_shared<semantic_ir::SDecl>(semantic_ir::SDeclType{type_name, type_params, variants, derive_traits}); },
[&](const ast::DeclTrait& declTrait) { auto [trait_name, type_params, methods] = declTrait; return [&]() {
auto typed_methods = transform_decls(methods, registry);
return std::make_shared<semantic_ir::SDecl>(semantic_ir::SDeclTrait{trait_name, type_params, typed_methods});
}(); },
[&](const ast::DeclExtend& declExtend) { auto [type_name, trait_name, methods] = declExtend; return [&]() {
auto typed_methods = transform_decls(methods, registry);
return std::make_shared<semantic_ir::SDecl>(semantic_ir::SDeclExtend{type_name, trait_name, typed_methods});
}(); },
[&](const ast::DeclImport& declImport) { auto [path, names] = declImport; return std::make_shared<semantic_ir::SDecl>(semantic_ir::SDeclImport{path, names}); },
[&](const ast::DeclExported& declExported) { auto [inner] = declExported; return std::make_shared<semantic_ir::SDecl>(semantic_ir::SDeclExported{transform_decl(inner, registry)}); }
}, (*declaration));
}
mlc::Array<std::shared_ptr<semantic_ir::SDecl>> transform_decls(mlc::Array<std::shared_ptr<ast::Decl>> declarations, registry::TypeRegistry registry) noexcept{
auto result = mlc::Array<std::shared_ptr<semantic_ir::SDecl>>{};
auto index = 0;
while ((index < declarations.length())) {
result.push_back(transform_decl(declarations[index], registry));
index = (index + 1);
}
return result;
}
semantic_ir::SProgram transform_program(ast::Program program, registry::TypeRegistry registry) noexcept{
return semantic_ir::SProgram{transform_decls(program.decls, registry)};
}
mlc::Array<semantic_ir::SNamespaceImportAlias> to_semantic_namespace_aliases(mlc::Array<decl_index::NamespaceImportAlias> items) noexcept{
auto result = mlc::Array<semantic_ir::SNamespaceImportAlias>{};
auto index = 0;
while ((index < items.length())) {
auto entry = items[index];
result.push_back(semantic_ir::SNamespaceImportAlias{entry.alias, entry.module_path});
index = (index + 1);
}
return result;
}
mlc::Array<semantic_ir::SLoadItem> transform_load_items(mlc::Array<decl_index::LoadItem> items, registry::TypeRegistry registry) noexcept{
auto result = mlc::Array<semantic_ir::SLoadItem>{};
auto index = 0;
while ((index < items.length())) {
auto item = items[index];
auto typed_decls = transform_decls(item.decls, registry);
result.push_back(semantic_ir::SLoadItem{item.path, typed_decls, item.imports, to_semantic_namespace_aliases(item.namespace_import_aliases)});
index = (index + 1);
}
return result;
}

} // namespace transform_decl
