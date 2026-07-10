#define main mlc_user_main
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

LoadResult load_module_impl(mlc::String path, mlc::HashMap<mlc::String, bool>& loaded, mlc::HashMap<mlc::String, LoadResult>& cache, bool profile_enabled) noexcept{
  auto norm_path = path_normalize::resolve_dotdot(path);
  if (cache.has(norm_path))   {
    return cache.get(norm_path);
  } else if (loaded.has(norm_path))   {
    return LoadResult{{}, mlc::Array<mlc::String>{((mlc::String("circular: ", 10) + mlc::to_string(norm_path)) + mlc::String("", 0))}};
  } else if (header_import::is_cpp_header_path(norm_path))   {
    loaded.set(norm_path, true);
    profile::profile_maybe_begin(profile_enabled, mlc::String("load_io", 7));
    auto header_loaded = header_import::load_cpp_header_decls(norm_path);
    profile::profile_maybe_end(profile_enabled, mlc::String("load_io", 7));
    if ((header_loaded.errors.length() > 0))     {
      return LoadResult{{}, header_loaded.errors};
    } else     {
      auto header_items = mlc::Array<load_item::LoadItem>{load_item::LoadItem{norm_path, header_loaded.declarations, {}, {}}};
      auto load_parsed = LoadResult{header_items, {}};
      cache.set(norm_path, load_parsed);
      return load_parsed;
    }
  } else if ((!path_normalize::driver_source_path_is_safe(path)))   {
    return LoadResult{{}, mlc::Array<mlc::String>{((mlc::String("driver: unsafe path ", 20) + mlc::to_string(path)) + mlc::String("", 0))}};
  } else   {
    loaded.set(norm_path, true);
    profile::profile_maybe_begin(profile_enabled, mlc::String("load_io", 7));
    auto source = [&]() -> mlc::String {
  if ((path == mlc::String("-", 1)))   {
    return mlc::io::read_all();
  } else   {
    return mlc::file::read_to_string(path);
  }
}();
    profile::profile_maybe_end(profile_enabled, mlc::String("load_io", 7));
    if ((((source.length() == 0) && (path != mlc::String("-", 1))) && (!mlc::file::exists(path))))     {
      return LoadResult{{}, mlc::Array<mlc::String>{((mlc::String("file not found: ", 16) + mlc::to_string(path)) + mlc::String("", 0))}};
    } else     {
      profile::profile_maybe_begin(profile_enabled, mlc::String("lex", 3));
      auto lexer_output = lexer::tokenize(source);
      profile::profile_maybe_end(profile_enabled, mlc::String("lex", 3));
      if (ast_tokens::LexOut_has_errors(lexer_output))       {
        return LoadResult{{}, mlc::Array<mlc::String>{((((mlc::String("lex ", 4) + mlc::to_string(path)) + mlc::String(": ", 2)) + mlc::to_string(lexer_output.errors[0])) + mlc::String("", 0))}};
      } else       {
        profile::profile_maybe_begin(profile_enabled, mlc::String("parse", 5));
        auto parse_parsed = decls::parse_program_with_errors(lexer_output.tokens, norm_path);
        profile::profile_maybe_end(profile_enabled, mlc::String("parse", 5));
        auto program = parse_parsed.program;
        auto items = mlc::Array<load_item::LoadItem>{};
        auto loaded_paths_seen = mlc::HashMap<mlc::String, bool>();
        auto module_declarations = mlc::Array<std::shared_ptr<ast::Decl>>{};
        auto my_imports = mlc::Array<mlc::String>{};
        auto my_namespace_import_aliases = mlc::Array<load_item::NamespaceImportAlias>{};
        auto all_errors = mlc::Array<mlc::String>{};
        auto index = 0;
        while ((index < program.decls.length()))         {
          [&]() {
auto __match_subject = program.decls[index];
if (std::holds_alternative<ast::DeclImport>((*__match_subject))) {
const ast::DeclImport& declImport = std::get<ast::DeclImport>((*__match_subject));
auto [import_path, symbols] = declImport; [&]() {
auto resolved = path_normalize::resolve_dotdot(path_normalize::resolve_import_path(path, import_path));
my_imports.push_back(resolved);
if (((symbols.length() >= 2) && (symbols[0] == mlc::String("*", 1)))) {
  my_namespace_import_aliases.push_back(load_item::NamespaceImportAlias{symbols[1], resolved});
}
auto dependency_parsed = load_module_impl(resolved, loaded, cache, profile_enabled);
(all_errors = ast::errs_append(all_errors, dependency_parsed.errors));
auto dep_item_index = 0;
return [&]() {
while ((dep_item_index < dependency_parsed.items.length())) {
auto dependency_item = dependency_parsed.items[dep_item_index];
if ((!loaded_paths_seen.has(dependency_item.path))) {
  loaded_paths_seen.set(dependency_item.path, true);
  items.push_back(dependency_item);
}
(dep_item_index = mlc::arith::checked_add(dep_item_index, 1));
}
}();
}();
return;
}
if (std::holds_alternative<ast::DeclExported>((*__match_subject))) {
const ast::DeclExported& declExported = std::get<ast::DeclExported>((*__match_subject));
auto [__0] = declExported; module_declarations.push_back(program.decls[index]);
return;
}
module_declarations.push_back(program.decls[index]);
return;
std::abort();
}();
          (index = mlc::arith::checked_add(index, 1));
        }
        items.push_back(load_item::LoadItem{norm_path, module_declarations, my_imports, my_namespace_import_aliases});
        auto load_parsed = LoadResult{items, all_errors};
        cache.set(norm_path, load_parsed);
        return load_parsed;
      }
    }
  }
}
LoadResult load_module(mlc::String path, mlc::HashMap<mlc::String, LoadResult>& cache, bool profile_enabled) noexcept{
  auto loaded = mlc::HashMap<mlc::String, bool>();
  return load_module_impl(path, loaded, cache, profile_enabled);
}

} // namespace module_loader
