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

program_merge::MergeResult merge_program_with_cache(mlc::String entry_path, ast::Program program, mlc::HashMap<mlc::String, module_loader::LoadResult>& load_cache, bool profile_enabled) noexcept;

program_merge::MergeResult merge_program(mlc::String entry_path, ast::Program program, bool profile_enabled) noexcept;

program_merge::MergeResult merge_program_with_cache(mlc::String entry_path, ast::Program program, mlc::HashMap<mlc::String, module_loader::LoadResult>& load_cache, bool profile_enabled) noexcept{
mlc::Array<std::shared_ptr<ast::Decl>> merged_declarations = {};
mlc::Array<mlc::String> all_errors = {};
mlc::HashMap<mlc::String, bool> seen_paths = mlc::HashMap<mlc::String, bool>();
mlc::Array<load_item::LoadItem> items_ordered = {};
mlc::Array<std::shared_ptr<ast::Decl>> entry_decls = {};
mlc::Array<mlc::String> entry_imports = {};
mlc::Array<load_item::NamespaceImportAlias> entry_namespace_import_aliases = {};
int index = 0;
while (index < program.decls.size()){
{
[&]() -> void { if (std::holds_alternative<ast::DeclImport>((*program.decls[index]))) { auto _v_declimport = std::get<ast::DeclImport>((*program.decls[index])); auto [path, symbols] = _v_declimport; mlc::String resolved = path_normalize::resolve_dotdot(path_normalize::resolve_import_path(entry_path, path)); entry_imports.push_back(resolved); if (symbols.size() >= 2 && symbols[0] == mlc::String("*")){
{
entry_namespace_import_aliases.push_back(load_item::NamespaceImportAlias{symbols[1], resolved});
}
} module_loader::LoadResult dependency_parsed = module_loader::load_module(resolved, load_cache, profile_enabled); all_errors = ast::errs_append(all_errors, dependency_parsed.errors); int dep_item_index = 0; [&]() { 
  while (dep_item_index < dependency_parsed.items.size()){
{
load_item::LoadItem dependency_item = dependency_parsed.items[dep_item_index];
if (!seen_paths.has(dependency_item.path)){
{
seen_paths.set(dependency_item.path, true);
items_ordered.push_back(dependency_item);
int decl_inner_index = 0;
[&]() { 
  while (decl_inner_index < dependency_item.decls.size()){
{
merged_declarations.push_back(dependency_item.decls[decl_inner_index]);
decl_inner_index = decl_inner_index + 1;
}
}
 }();
}
}
dep_item_index = dep_item_index + 1;
}
}
 }(); return; } if (std::holds_alternative<ast::DeclExported>((*program.decls[index]))) { auto _v_declexported = std::get<ast::DeclExported>((*program.decls[index])); auto [exported_declaration] = _v_declexported; entry_decls.push_back(exported_declaration); merged_declarations.push_back(exported_declaration); return; } entry_decls.push_back(program.decls[index]); merged_declarations.push_back(program.decls[index]); return; }();
index = index + 1;
}
}
mlc::String norm_entry = path_normalize::resolve_dotdot(entry_path);
items_ordered.push_back(load_item::LoadItem{norm_entry, entry_decls, entry_imports, entry_namespace_import_aliases});
return program_merge::MergeResult{ast::Program{merged_declarations}, all_errors, items_ordered};
}

program_merge::MergeResult merge_program(mlc::String entry_path, ast::Program program, bool profile_enabled) noexcept{
mlc::HashMap<mlc::String, module_loader::LoadResult> load_cache = mlc::HashMap<mlc::String, module_loader::LoadResult>();
return merge_program_with_cache(entry_path, program, load_cache, profile_enabled);
}

} // namespace program_merge
