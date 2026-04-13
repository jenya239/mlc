#include "mlc.hpp"

#define main mlc_user_main
#include "main.hpp"

#include "lexer.hpp"
#include "decls.hpp"
#include "check.hpp"
#include "transform_decl.hpp"
#include "registry.hpp"
#include "module.hpp"
#include "context.hpp"
#include "decl_index.hpp"
#include "cpp_naming.hpp"
#include "ast.hpp"

namespace mlc_main {

using namespace lexer;
using namespace decls;
using namespace check;
using namespace transform_decl;
using namespace registry;
using namespace module;
using namespace context;
using namespace decl_index;
using namespace cpp_naming;
using namespace ast;
using namespace ast_tokens;

struct LoadResult {mlc::Array<decl_index::LoadItem> items;mlc::Array<mlc::String> errors;};

struct MergeResult {ast::Program prog;mlc::Array<mlc::String> errors;mlc::Array<decl_index::LoadItem> items;};

mlc::String dirname(mlc::String path) noexcept;

mlc::String resolve_dotdot(mlc::String path) noexcept;

mlc::String resolve_import_path(mlc::String base_path, mlc::String import_path) noexcept;

bool path_in_loaded(mlc::String path, mlc::Array<mlc::String> loaded) noexcept;

LoadResult load_module_impl(mlc::String path, mlc::Array<mlc::String> loaded, mlc::HashMap<mlc::String, LoadResult>& cache) noexcept;

LoadResult load_module(mlc::String path, mlc::HashMap<mlc::String, LoadResult>& cache) noexcept;

MergeResult merge_program(mlc::String entry_path, ast::Program prog) noexcept;

ast::Result<mlc::String, mlc::Array<mlc::String>> compile_modular(mlc::String entry_path, mlc::String out_dir) noexcept;

mlc::String compile_modular_loop(mlc::Array<decl_index::LoadItem> items, ast::Program full_prog, mlc::String out_dir) noexcept;

mlc::String format_errs(mlc::String label, mlc::Array<mlc::String> errors) noexcept;

int main() noexcept;

mlc::String dirname(mlc::String path) noexcept{
int last_slash = -1;
int index = 0;
while (index < path.length()){
{
if (path.char_at(index) == mlc::String("/")){
{
last_slash = index;
}
}
index = index + 1;
}
}
return last_slash <= 0 ? mlc::String("") : path.substring(0, last_slash);
}

mlc::String resolve_dotdot(mlc::String path) noexcept{
mlc::String p = path;
int i = 0;
while (i + 4 <= p.length()){
{
if (p.substring(i, 4) == mlc::String("/../")){
{
int j = i - 1;
while (j >= 0 && p.char_at(j) != mlc::String("/")){
{
j = j - 1;
}
}
mlc::String prefix = j <= 0 ? mlc::String("") : p.substring(0, j);
mlc::String suffix = p.substring(i + 4, p.length() - i - 4);
p = prefix == mlc::String("") ? suffix : prefix + mlc::String("/") + suffix;
i = 0;
}
}
i = i + 1;
}
}
if (p.length() >= 3 && p.substring(0, 3) == mlc::String("../")){
{
p = p.substring(3, p.length() - 3);
}
}
return p;
}

mlc::String resolve_import_path(mlc::String base_path, mlc::String import_path) noexcept{
mlc::String base_dir = dirname(base_path);
mlc::String rest = import_path.length() >= 2 && import_path.substring(0, 2) == mlc::String("./") ? import_path.substring(2, import_path.length() - 2) : import_path;
mlc::String with_ext = rest.length() >= 4 && rest.substring(rest.length() - 4, 4) == mlc::String(".mlc") ? rest : rest + mlc::String(".mlc");
mlc::String raw = base_dir == mlc::String("") ? with_ext : base_dir + mlc::String("/") + with_ext;
return resolve_dotdot(raw);
}

bool path_in_loaded(mlc::String path, mlc::Array<mlc::String> loaded) noexcept{
bool found = false;
int i = 0;
while (i < loaded.size()){
{
if (loaded[i] == path){
{
found = true;
}
}
i = i + 1;
}
}
return found;
}

LoadResult load_module_impl(mlc::String path, mlc::Array<mlc::String> loaded, mlc::HashMap<mlc::String, LoadResult>& cache) noexcept{
mlc::String norm_path = resolve_dotdot(path);
return cache.has(norm_path) ? cache.get(norm_path) : path_in_loaded(norm_path, loaded) ? LoadResult{{}, mlc::Array<mlc::String>{mlc::String("circular: ") + norm_path}} : [&]() -> LoadResult { 
  mlc::Array<mlc::String> cur_loaded = loaded;
  cur_loaded.push_back(norm_path);
  mlc::String source = mlc::file::read_to_string(path);
  return source.length() == 0 && !mlc::file::exists(path) ? LoadResult{{}, mlc::Array<mlc::String>{mlc::String("file not found: ") + path}} : [&]() -> LoadResult { 
  ast_tokens::LexOut lex = lexer::tokenize(source);
  return ast_tokens::LexOut_has_errors(lex) ? LoadResult{{}, mlc::Array<mlc::String>{mlc::String("lex ") + path + mlc::String(": ") + lex.errors[0]}} : [&]() -> LoadResult { 
  ast::Program prog = decls::parse_program_with_source_path(lex.tokens, norm_path);
  mlc::Array<decl_index::LoadItem> items = {};
  mlc::Array<mlc::String> seen = {};
  mlc::Array<std::shared_ptr<ast::Decl>> my_decls = {};
  mlc::Array<mlc::String> my_imports = {};
  mlc::Array<decl_index::NamespaceImportAlias> my_namespace_import_aliases = {};
  mlc::Array<mlc::String> all_errors = {};
  int index = 0;
  while (index < prog.decls.size()){
{
[&]() -> void { if (std::holds_alternative<ast::DeclImport>((*prog.decls[index]))) { auto _v_declimport = std::get<ast::DeclImport>((*prog.decls[index])); auto [import_path, symbols] = _v_declimport; return [&]() { 
  mlc::String resolved = resolve_dotdot(resolve_import_path(path, import_path));
  my_imports.push_back(resolved);
  if (symbols.size() >= 2 && symbols[0] == mlc::String("*")){
{
my_namespace_import_aliases.push_back(decl_index::NamespaceImportAlias{symbols[1], resolved});
}
}
  LoadResult dep_result = load_module_impl(resolved, cur_loaded, cache);
  all_errors = ast::errs_append(all_errors, dep_result.errors);
  int dep_i = 0;
  return [&]() { 
  while (dep_i < dep_result.items.size()){
{
decl_index::LoadItem item = dep_result.items[dep_i];
if (!path_in_loaded(item.path, seen)){
{
seen.push_back(item.path);
items.push_back(item);
}
}
dep_i = dep_i + 1;
}
}
 }();
 }(); } if (std::holds_alternative<ast::DeclExported>((*prog.decls[index]))) { auto _v_declexported = std::get<ast::DeclExported>((*prog.decls[index])); auto [d] = _v_declexported; return my_decls.push_back(prog.decls[index]); } return my_decls.push_back(prog.decls[index]); }();
index = index + 1;
}
}
  items.push_back(decl_index::LoadItem{norm_path, my_decls, my_imports, my_namespace_import_aliases});
  LoadResult result = LoadResult{items, all_errors};
  cache.set(norm_path, result);
  return result;
 }();
 }();
 }();
}

LoadResult load_module(mlc::String path, mlc::HashMap<mlc::String, LoadResult>& cache) noexcept{return load_module_impl(path, {}, cache);}

MergeResult merge_program(mlc::String entry_path, ast::Program prog) noexcept{
mlc::Array<std::shared_ptr<ast::Decl>> all_decls = {};
mlc::Array<mlc::String> all_errors = {};
mlc::Array<mlc::String> seen_paths = {};
mlc::Array<decl_index::LoadItem> items_ordered = {};
mlc::Array<std::shared_ptr<ast::Decl>> entry_decls = {};
mlc::Array<mlc::String> entry_imports = {};
mlc::Array<decl_index::NamespaceImportAlias> entry_namespace_import_aliases = {};
mlc::HashMap<mlc::String, LoadResult> cache = mlc::HashMap<mlc::String, LoadResult>();
int index = 0;
while (index < prog.decls.size()){
{
[&]() -> void { if (std::holds_alternative<ast::DeclImport>((*prog.decls[index]))) { auto _v_declimport = std::get<ast::DeclImport>((*prog.decls[index])); auto [path, symbols] = _v_declimport; return [&]() { 
  mlc::String resolved = resolve_dotdot(resolve_import_path(entry_path, path));
  entry_imports.push_back(resolved);
  if (symbols.size() >= 2 && symbols[0] == mlc::String("*")){
{
entry_namespace_import_aliases.push_back(decl_index::NamespaceImportAlias{symbols[1], resolved});
}
}
  LoadResult dep_result = load_module(resolved, cache);
  all_errors = ast::errs_append(all_errors, dep_result.errors);
  int dep_i = 0;
  return [&]() { 
  while (dep_i < dep_result.items.size()){
{
decl_index::LoadItem item = dep_result.items[dep_i];
if (!path_in_loaded(item.path, seen_paths)){
{
seen_paths.push_back(item.path);
items_ordered.push_back(item);
int d = 0;
[&]() { 
  while (d < item.decls.size()){
{
all_decls.push_back(item.decls[d]);
d = d + 1;
}
}
 }();
}
}
dep_i = dep_i + 1;
}
}
 }();
 }(); } if (std::holds_alternative<ast::DeclExported>((*prog.decls[index]))) { auto _v_declexported = std::get<ast::DeclExported>((*prog.decls[index])); auto [d] = _v_declexported; return [&]() { 
  entry_decls.push_back(d);
  return all_decls.push_back(d);
 }(); } return [&]() { 
  entry_decls.push_back(prog.decls[index]);
  return all_decls.push_back(prog.decls[index]);
 }(); }();
index = index + 1;
}
}
mlc::String norm_entry = resolve_dotdot(entry_path);
items_ordered.push_back(decl_index::LoadItem{norm_entry, entry_decls, entry_imports, entry_namespace_import_aliases});
return MergeResult{ast::Program{all_decls}, all_errors, items_ordered};
}

ast::Result<mlc::String, mlc::Array<mlc::String>> compile_modular(mlc::String entry_path, mlc::String out_dir) noexcept{
mlc::String src = mlc::file::read_to_string(entry_path);
ast_tokens::LexOut lex = lexer::tokenize(src);
return ast_tokens::LexOut_has_errors(lex) ? ast::Result<mlc::String, mlc::Array<mlc::String>>(ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("lex: ") + lex.errors[0]})) : ast::Result<mlc::String, mlc::Array<mlc::String>>([&]() -> ast::Result<mlc::String, mlc::Array<mlc::String>> { 
  mlc::String source_path = resolve_dotdot(entry_path);
  ast::Program prog = decls::parse_program_with_source_path(lex.tokens, source_path);
  MergeResult merged = merge_program(entry_path, prog);
  return merged.errors.size() > 0 ? ast::Result<mlc::String, mlc::Array<mlc::String>>(ast::Err<mlc::Array<mlc::String>>(merged.errors)) : ast::Result<mlc::String, mlc::Array<mlc::String>>([&]() -> ast::Result<mlc::String, mlc::Array<mlc::String>> { 
  int n = merged.items.size();
  decl_index::LoadItem entry_item = merged.items[n - 1];
  ast::Program entry_prog = ast::Program{entry_item.decls};
  auto __try__checked = check::check_with_context(entry_prog, merged.prog);
  if (std::holds_alternative<ast::Err<mlc::Array<mlc::String>>>(__try__checked)) return ast::Result<mlc::String, mlc::Array<mlc::String>>(std::get<ast::Err<mlc::Array<mlc::String>>>(__try__checked));
  check::CheckOut _checked = std::get<ast::Ok<check::CheckOut>>(__try__checked).field0;
  return ast::Ok<mlc::String>(compile_modular_loop(merged.items, merged.prog, out_dir));
 }());
 }());
}

mlc::String compile_modular_loop(mlc::Array<decl_index::LoadItem> items, ast::Program full_prog, mlc::String out_dir) noexcept{
registry::TypeRegistry registry = registry::build_registry(full_prog);
mlc::Array<semantic_ir::SLoadItem> s_items = transform_decl::transform_load_items(items, registry);
context::PrecomputedCtx precomp = module::precompute(full_prog, items);
int i = 0;
while (i < s_items.size()){
{
semantic_ir::SLoadItem s_item = s_items[i];
context::GenModuleOut out = module::gen_module(s_item, items, full_prog, precomp);
mlc::String base = cpp_naming::path_to_module_base(s_item.path);
mlc::String hpp_path = out_dir.length() > 0 ? out_dir + mlc::String("/") + base + mlc::String(".hpp") : base + mlc::String(".hpp");
mlc::String cpp_path = out_dir.length() > 0 ? out_dir + mlc::String("/") + base + mlc::String(".cpp") : base + mlc::String(".cpp");
mlc::file::write_string(hpp_path, out.h);
mlc::file::write_string(cpp_path, out.c);
i = i + 1;
}
}
return mlc::String("");
}

mlc::String format_errs(mlc::String label, mlc::Array<mlc::String> errors) noexcept{
mlc::String output = mlc::String("");
int i = 0;
while (i < errors.size()){
{
output = output + label + mlc::String(": ") + errors[i] + mlc::String("\n");
i = i + 1;
}
}
return output;
}

int main(int argc, char** argv) noexcept{
mlc::io::set_args(std::vector<mlc::String>(argv + 1, argv + argc));
mlc::Array<mlc::String> a = mlc::io::args();
if (a.size() == 0){
{
mlc::io::println(mlc::String("Usage: mlcc <source.mlc> [-o out_dir]"));
mlc::io::exit(1);
}
}
mlc::String entry_path = a.size() >= 3 && a[0] == mlc::String("-o") ? a[2] : a[0];
mlc::String out_dir = a.size() >= 3 && a[0] == mlc::String("-o") ? a[1] : a.size() >= 3 && a[1] == mlc::String("-o") ? a[2] : mlc::String("out");
return std::visit(overloaded{
  [&](const ast::Ok<mlc::String>& ok) -> int { auto [_w0] = ok; return 0; },
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> int { auto [errors] = err; return [&]() -> int { 
  mlc::io::print(format_errs(mlc::String("error"), errors));
  return mlc::io::exit(1);
 }(); }
}, compile_modular(entry_path, out_dir));
}

} // namespace mlc_main

#undef main

static void mlc_cli_set_args(int argc, char** argv) {
  std::vector<mlc::String> arguments;
  arguments.reserve(argc > 0 ? argc - 1 : 0);
  for (int i = 1; i < argc; ++i) {
    arguments.emplace_back(argv[i]);
  }
  mlc::io::set_args(std::move(arguments));
}

int main(int argc, char** argv) {
  mlc_cli_set_args(argc, argv);
  return ::mlc_main::mlc_user_main(argc, argv);
}
