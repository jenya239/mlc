#define main mlc_user_main
#include "program_merge.hpp"

#include "ast.hpp"
#include "load_item.hpp"
#include "path_normalize.hpp"
#include "module_loader.hpp"

namespace program_merge {

using namespace ast;
using namespace load_item;
using namespace path_normalize;
using namespace module_loader;
using namespace ast_tokens;

MergeResult merge_program_with_cache(mlc::String entry_path, ast::Program program, mlc::HashMap<mlc::String, module_loader::LoadResult>& load_cache, bool profile_enabled) noexcept{
  auto merged_declarations = mlc::Array<std::shared_ptr<ast::Decl>>{};
  auto all_errors = mlc::Array<mlc::String>{};
  auto seen_paths = mlc::HashMap<mlc::String, bool>();
  auto items_ordered = mlc::Array<load_item::LoadItem>{};
  auto entry_decls = mlc::Array<std::shared_ptr<ast::Decl>>{};
  auto entry_imports = mlc::Array<mlc::String>{};
  auto entry_namespace_import_aliases = mlc::Array<load_item::NamespaceImportAlias>{};
  auto index = 0;
  while ((index < program.decls.length()))   {
    [&]() {
auto __match_subject = program.decls[index];
if (std::holds_alternative<ast::DeclImport>((*__match_subject))) {
const ast::DeclImport& declImport = std::get<ast::DeclImport>((*__match_subject));
auto [path, symbols] = declImport; [&]() {
auto resolved = path_normalize::resolve_dotdot(path_normalize::resolve_import_path(entry_path, path));
entry_imports.push_back(resolved);
if (((symbols.length() >= 2) && (symbols[0] == mlc::String("*", 1)))) {
  entry_namespace_import_aliases.push_back(load_item::NamespaceImportAlias{symbols[1], resolved});
}
auto dependency_parsed = module_loader::load_module(resolved, load_cache, profile_enabled);
(all_errors = ast::errs_append(all_errors, dependency_parsed.errors));
auto dep_item_index = 0;
return [&]() {
while ((dep_item_index < dependency_parsed.items.length())) {
auto dependency_item = dependency_parsed.items[dep_item_index];
if ((!seen_paths.has(dependency_item.path))) {
  seen_paths.set(dependency_item.path, true);
  items_ordered.push_back(dependency_item);
  auto decl_inner_index = 0;
  [&]() {
while ((decl_inner_index < dependency_item.decls.length())) {
merged_declarations.push_back(dependency_item.decls[decl_inner_index]);
(decl_inner_index = mlc::arith::checked_add(decl_inner_index, 1));
}
}();
}
(dep_item_index = mlc::arith::checked_add(dep_item_index, 1));
}
}();
}();
return;
}
if (std::holds_alternative<ast::DeclExported>((*__match_subject))) {
const ast::DeclExported& declExported = std::get<ast::DeclExported>((*__match_subject));
auto [exported_declaration] = declExported; [&]() {
entry_decls.push_back(exported_declaration);
return merged_declarations.push_back(exported_declaration);
}();
return;
}
[&]() {
entry_decls.push_back(program.decls[index]);
return merged_declarations.push_back(program.decls[index]);
}();
return;
std::abort();
}();
    (index = mlc::arith::checked_add(index, 1));
  }
  auto norm_entry = path_normalize::resolve_dotdot(entry_path);
  items_ordered.push_back(load_item::LoadItem{norm_entry, entry_decls, entry_imports, entry_namespace_import_aliases});
  return MergeResult{ast::Program{merged_declarations}, all_errors, items_ordered};
}
MergeResult merge_program(mlc::String entry_path, ast::Program program, bool profile_enabled) noexcept{
  auto load_cache = mlc::HashMap<mlc::String, module_loader::LoadResult>();
  return merge_program_with_cache(entry_path, program, load_cache, profile_enabled);
}

} // namespace program_merge
