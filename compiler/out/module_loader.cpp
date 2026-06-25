#include "module_loader.hpp"

#include "ast.hpp"
#include "lexer.hpp"
#include "decls.hpp"
#include "load_item.hpp"
#include "profile.hpp"
#include "header_import.hpp"
#include "path_normalize.hpp"

namespace module_loader {

using namespace ast;
using namespace lexer;
using namespace decls;
using namespace load_item;
using namespace profile;
using namespace header_import;
using namespace path_normalize;
using namespace ast_tokens;

module_loader::LoadResult load_module_impl(mlc::String path, mlc::HashMap<mlc::String, bool>& loaded, mlc::HashMap<mlc::String, module_loader::LoadResult>& cache, bool profile_enabled) noexcept;

module_loader::LoadResult load_module(mlc::String path, mlc::HashMap<mlc::String, module_loader::LoadResult>& cache, bool profile_enabled) noexcept;

module_loader::LoadResult load_module_impl(mlc::String path, mlc::HashMap<mlc::String, bool>& loaded, mlc::HashMap<mlc::String, module_loader::LoadResult>& cache, bool profile_enabled) noexcept{
mlc::String norm_path = path_normalize::resolve_dotdot(path);
return cache.has(norm_path) ? cache.get(norm_path) : loaded.has(norm_path) ? module_loader::LoadResult{{}, mlc::Array<mlc::String>{mlc::String("circular: ") + norm_path}} : header_import::is_cpp_header_path(norm_path) ? [&]() -> module_loader::LoadResult { 
  loaded.set(norm_path, true);
  profile::profile_maybe_begin(profile_enabled, mlc::String("load_io"));
  header_import::HeaderImportResult header_loaded = header_import::load_cpp_header_decls(norm_path);
  profile::profile_maybe_end(profile_enabled, mlc::String("load_io"));
  return header_loaded.errors.size() > 0 ? module_loader::LoadResult{{}, header_loaded.errors} : [&]() -> module_loader::LoadResult { 
  mlc::Array<load_item::LoadItem> header_items = mlc::Array<load_item::LoadItem>{load_item::LoadItem{norm_path, header_loaded.declarations, {}, {}}};
  module_loader::LoadResult load_parsed = module_loader::LoadResult{header_items, {}};
  cache.set(norm_path, load_parsed);
  return load_parsed;
 }();
 }() : !path_normalize::driver_source_path_is_safe(path) ? module_loader::LoadResult{{}, mlc::Array<mlc::String>{mlc::String("driver: unsafe path ") + path}} : [&]() -> module_loader::LoadResult { 
  loaded.set(norm_path, true);
  profile::profile_maybe_begin(profile_enabled, mlc::String("load_io"));
  mlc::String source = mlc::file::read_to_string(path);
  profile::profile_maybe_end(profile_enabled, mlc::String("load_io"));
  return source.length() == 0 && !mlc::file::exists(path) ? module_loader::LoadResult{{}, mlc::Array<mlc::String>{mlc::String("file not found: ") + path}} : [&]() -> module_loader::LoadResult { 
  profile::profile_maybe_begin(profile_enabled, mlc::String("lex"));
  ast_tokens::LexOut lexer_output = lexer::tokenize(source);
  profile::profile_maybe_end(profile_enabled, mlc::String("lex"));
  return ast_tokens::LexOut_has_errors(lexer_output) ? module_loader::LoadResult{{}, mlc::Array<mlc::String>{mlc::String("lex ") + path + mlc::String(": ") + lexer_output.errors[0]}} : [&]() -> module_loader::LoadResult { 
  profile::profile_maybe_begin(profile_enabled, mlc::String("parse"));
  predicates::ProgramParseValue parse_parsed = decls::parse_program_with_errors(lexer_output.tokens, norm_path);
  profile::profile_maybe_end(profile_enabled, mlc::String("parse"));
  ast::Program program = parse_parsed.program;
  mlc::Array<load_item::LoadItem> items = {};
  mlc::HashMap<mlc::String, bool> loaded_paths_seen = mlc::HashMap<mlc::String, bool>();
  mlc::Array<std::shared_ptr<ast::Decl>> module_declarations = {};
  mlc::Array<mlc::String> my_imports = {};
  mlc::Array<load_item::NamespaceImportAlias> my_namespace_import_aliases = {};
  mlc::Array<mlc::String> all_errors = {};
  int index = 0;
  while (index < program.decls.size()){
{
[&]() -> void { if (std::holds_alternative<ast::DeclImport>((*program.decls[index]))) { auto _v_declimport = std::get<ast::DeclImport>((*program.decls[index])); auto [import_path, symbols] = _v_declimport; return [&]() { 
  mlc::String resolved = path_normalize::resolve_dotdot(path_normalize::resolve_import_path(path, import_path));
  my_imports.push_back(resolved);
  if (symbols.size() >= 2 && symbols[0] == mlc::String("*")){
{
my_namespace_import_aliases.push_back(load_item::NamespaceImportAlias{symbols[1], resolved});
}
}
  module_loader::LoadResult dependency_parsed = load_module_impl(resolved, loaded, cache, profile_enabled);
  all_errors = ast::errs_append(all_errors, dependency_parsed.errors);
  int dep_item_index = 0;
  return [&]() { 
  while (dep_item_index < dependency_parsed.items.size()){
{
load_item::LoadItem dependency_item = dependency_parsed.items[dep_item_index];
if (!loaded_paths_seen.has(dependency_item.path)){
{
loaded_paths_seen.set(dependency_item.path, true);
items.push_back(dependency_item);
}
}
dep_item_index = dep_item_index + 1;
}
}
 }();
 }(); } if (std::holds_alternative<ast::DeclExported>((*program.decls[index]))) { auto _v_declexported = std::get<ast::DeclExported>((*program.decls[index])); auto [_w0] = _v_declexported; return module_declarations.push_back(program.decls[index]); } return module_declarations.push_back(program.decls[index]); }();
index = index + 1;
}
}
  items.push_back(load_item::LoadItem{norm_path, module_declarations, my_imports, my_namespace_import_aliases});
  module_loader::LoadResult load_parsed = module_loader::LoadResult{items, all_errors};
  cache.set(norm_path, load_parsed);
  return load_parsed;
 }();
 }();
 }();
}

module_loader::LoadResult load_module(mlc::String path, mlc::HashMap<mlc::String, module_loader::LoadResult>& cache, bool profile_enabled) noexcept{
mlc::HashMap<mlc::String, bool> loaded = mlc::HashMap<mlc::String, bool>();
return load_module_impl(path, loaded, cache, profile_enabled);
}

} // namespace module_loader
