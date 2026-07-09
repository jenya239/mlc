#define main mlc_user_main
#include "transform_decl.hpp"

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

using namespace ast;
using namespace trait_param_expand;
using namespace param_destructure_expand;
using namespace registry;
using namespace semantic_ir;
using namespace load_item;
using namespace transform;
using namespace transform_stmts;
using namespace escape_analysis;
using namespace ast_tokens;

bool string_list_contains(mlc::Array<mlc::String> haystack, mlc::String needle) noexcept{
  auto index = 0;
  while ((index < haystack.length()))   {
    if ((haystack[index] == needle))     {
      return true;
    }
    (index = (index + 1));
  }
  return false;
}
semantic_ir::FnEscapeInfo fn_escape_info_for_declaration(std::shared_ptr<ast::Decl> declaration, mlc::Array<mlc::String> functions_used_as_values, bool apply_escape_templates) noexcept{
  if ((!apply_escape_templates))   {
    return semantic_ir::empty_fn_escape_info();
  }
  auto non_escaping = escape_analysis::non_escaping_params_for_decl_if_template_safe(declaration, functions_used_as_values);
  if ((non_escaping.length() == 0))   {
    return semantic_ir::empty_fn_escape_info();
  }
  return [&]() -> semantic_ir::FnEscapeInfo {
auto __match_subject = declaration;
if (std::holds_alternative<ast::DeclFn>((*__match_subject))) {
const ast::DeclFn& declFn = std::get<ast::DeclFn>((*__match_subject));
auto [__0, __1, __2, parameters, __4, __5, __6] = declFn; return [&]() {
auto synthetic_type_params = mlc::Array<mlc::String>{};
auto param_template_type_names = mlc::HashMap<mlc::String, mlc::String>();
auto index = 0;
while ((index < parameters.length())) {
  auto parameter_name = parameters[index]->name;
  if (string_list_contains(non_escaping, parameter_name))   {
    auto template_type_name = (mlc::String("__F", 3) + mlc::to_string(index));
    synthetic_type_params.push_back(template_type_name);
    param_template_type_names.set(parameter_name, template_type_name);
  }
  (index = (index + 1));
}
return semantic_ir::FnEscapeInfo{synthetic_type_params, param_template_type_names};
}();
}
return semantic_ir::empty_fn_escape_info();
std::abort();
}();
}
std::shared_ptr<semantic_ir::SemanticDeclaration> transform_decl(std::shared_ptr<ast::Decl> declaration, registry::TypeRegistry registry, mlc::Array<mlc::String> functions_used_as_values, bool apply_escape_templates) noexcept{
  return std::visit(overloaded{[&](const ast::DeclFn& declFn) { auto [name, type_params, trait_bounds, params, return_type_expr, body, where_clause_bounds_entries] = declFn; return [&]() {
auto param_env = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
auto index = 0;
while ((index < params.length())) {
  param_env.set(params[index]->name, registry::type_from_annotation_with_registry(params[index]->type_value, registry));
  (index = (index + 1));
}
auto return_type = registry::type_from_annotation_with_registry(return_type_expr, registry);
auto initial_context = transform::TransformContext{param_env, registry, {}};
auto typed_body = transform::transform_expr(body, initial_context, [=](mlc::Array<std::shared_ptr<ast::Stmt>> statements, transform::TransformContext transform_context) mutable { return transform_stmts::transform_stmts(statements, transform_context); });
auto coerced_body = transform::coerce_expr_to_type(typed_body, return_type);
return std::make_shared<semantic_ir::SemanticDeclaration>(semantic_ir::SemanticDeclarationFn{name, type_params, trait_bounds, params, return_type, coerced_body, where_clause_bounds_entries, fn_escape_info_for_declaration(declaration, functions_used_as_values, apply_escape_templates), ast::expr_span(body)});
}(); },
[&](const ast::DeclType& declType) { auto [type_name, type_params, variants, derive_traits, name_span] = declType; return std::make_shared<semantic_ir::SemanticDeclaration>(semantic_ir::SemanticDeclarationType{type_name, type_params, variants, derive_traits, name_span}); },
[&](const ast::DeclTypeAlias& declTypeAlias) { auto [type_name, type_params, type_expression, name_span] = declTypeAlias; return std::make_shared<semantic_ir::SemanticDeclaration>(semantic_ir::SemanticDeclarationTypeAlias{type_name, type_params, type_expression, name_span}); },
[&](const ast::DeclTrait& declTrait) { auto [trait_name, type_params, methods, name_span] = declTrait; return [&]() {
auto typed_methods = transform_decls(methods, registry, functions_used_as_values, false);
return std::make_shared<semantic_ir::SemanticDeclaration>(semantic_ir::SemanticDeclarationTrait{trait_name, type_params, typed_methods, name_span});
}(); },
[&](const ast::DeclExtend& declExtend) { auto [type_name, trait_name, methods, name_span] = declExtend; return [&]() {
auto typed_methods = transform_decls(methods, registry, functions_used_as_values, false);
return std::make_shared<semantic_ir::SemanticDeclaration>(semantic_ir::SemanticDeclarationExtend{type_name, trait_name, typed_methods, name_span});
}(); },
[&](const ast::DeclImport& declImport) { auto [path, names] = declImport; return std::make_shared<semantic_ir::SemanticDeclaration>(semantic_ir::SemanticDeclarationImport{path, names}); },
[&](const ast::DeclExternLib& declExternLib) { auto [library_name, span] = declExternLib; return std::make_shared<semantic_ir::SemanticDeclaration>(semantic_ir::SemanticDeclarationExternLib{library_name, span}); },
[&](const ast::DeclExported& declExported) { auto [exported_declaration] = declExported; return std::make_shared<semantic_ir::SemanticDeclaration>(semantic_ir::SemanticDeclarationExported{transform_decl(exported_declaration, registry, functions_used_as_values, apply_escape_templates)}); },
[&](const ast::DeclAssocType& declAssocType) { auto [__0, __1] = declAssocType; return std::make_shared<semantic_ir::SemanticDeclaration>(semantic_ir::SemanticDeclarationImport{mlc::String("", 0), {}}); },
[&](const ast::DeclAssocBind& declAssocBind) { auto [name, type_expr, span] = declAssocBind; return std::make_shared<semantic_ir::SemanticDeclaration>(semantic_ir::SemanticDeclarationAssocBind{name, registry::type_from_annotation_with_registry(type_expr, registry), span}); }
}, (*declaration));
}
mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> transform_decls(mlc::Array<std::shared_ptr<ast::Decl>> declarations, registry::TypeRegistry registry, mlc::Array<mlc::String> functions_used_as_values, bool apply_escape_templates) noexcept{
  return declarations.fold([&]() {
auto empty_declarations = mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{};
return empty_declarations;
}(), [=](mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> result, std::shared_ptr<ast::Decl> declaration) mutable { return result.concat(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>>{transform_decl(declaration, registry, functions_used_as_values, apply_escape_templates)}); });
}
semantic_ir::SemanticProgram transform_program(ast::Program program, registry::TypeRegistry registry) noexcept{
  return semantic_ir::SemanticProgram{transform_decls(program.decls, registry, escape_analysis::function_names_used_as_values(program), true)};
}
mlc::Array<semantic_ir::SemanticNamespaceImportAlias> to_semantic_namespace_aliases(mlc::Array<load_item::NamespaceImportAlias> items) noexcept{
  return items.fold([&]() {
auto empty_aliases = mlc::Array<semantic_ir::SemanticNamespaceImportAlias>{};
return empty_aliases;
}(), [=](mlc::Array<semantic_ir::SemanticNamespaceImportAlias> result, load_item::NamespaceImportAlias entry) mutable { return result.concat(mlc::Array<semantic_ir::SemanticNamespaceImportAlias>{semantic_ir::SemanticNamespaceImportAlias{entry.alias, entry.module_path}}); });
}
mlc::Array<semantic_ir::SemanticLoadItem> transform_load_items(mlc::Array<load_item::LoadItem> items, registry::TypeRegistry registry, trait_param_expand::TraitNominalMaps trait_maps) noexcept{
  return items.fold([&]() {
auto empty_items = mlc::Array<semantic_ir::SemanticLoadItem>{};
return empty_items;
}(), [=](mlc::Array<semantic_ir::SemanticLoadItem> result, load_item::LoadItem item) mutable { return [&]() {
auto destructured_entry_declarations = param_destructure_expand::expand_parameter_destructuring_in_program(ast::Program{item.decls}).decls;
auto expanded_declarations = trait_param_expand::expand_declarations_with_trait_nominal_maps(destructured_entry_declarations, trait_maps);
auto functions_used_as_values = escape_analysis::function_names_used_as_values(ast::Program{expanded_declarations});
auto typed_declarations = transform_decls(expanded_declarations, registry, functions_used_as_values, true);
return result.concat(mlc::Array<semantic_ir::SemanticLoadItem>{semantic_ir::SemanticLoadItem{item.path, typed_declarations, item.imports, to_semantic_namespace_aliases(item.namespace_import_aliases)}});
}(); });
}

} // namespace transform_decl
