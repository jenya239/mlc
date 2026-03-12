#include "mlc.hpp"

#define main mlc_user_main
#include "main.hpp"

#include "lexer.hpp"
#include "decls.hpp"
#include "infer.hpp"
#include "codegen.hpp"
#include "ast.hpp"

namespace mlc_main {

using namespace lexer;
using namespace decls;
using namespace infer;
using namespace codegen;
using namespace ast;
using namespace ast_tokens;

struct LoadItem;

struct LoadResult;

struct MergeResult;

mlc::String dirname(mlc::String path) noexcept;

mlc::String resolve_dotdot(mlc::String path) noexcept;

mlc::String resolve_import_path(mlc::String base_path, mlc::String import_path) noexcept;

bool path_in_loaded(mlc::String path, mlc::Array<mlc::String> loaded) noexcept;

LoadResult load_module_impl(mlc::String path, mlc::Array<mlc::String> loaded, mlc::HashMap<mlc::String, LoadResult> cache) noexcept;

LoadResult load_module(mlc::String path, mlc::HashMap<mlc::String, LoadResult> cache) noexcept;

MergeResult merge_program(mlc::String entry_path, ast::Program prog) noexcept;

mlc::String compile_source(mlc::String src, mlc::String entry_path) noexcept;

mlc::String format_errs(mlc::String label, mlc::Array<mlc::String> errors) noexcept;

int main() noexcept;

struct LoadItem {mlc::String path;mlc::Array<std::shared_ptr<ast::Decl>> decls;};

struct LoadResult {mlc::Array<LoadItem> items;mlc::Array<mlc::String> errors;};

struct MergeResult {ast::Program prog;mlc::Array<mlc::String> errors;};

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

LoadResult load_module_impl(mlc::String path, mlc::Array<mlc::String> loaded, mlc::HashMap<mlc::String, LoadResult> cache) noexcept{
mlc::String norm_path = resolve_dotdot(path);
return cache.has(norm_path) ? cache.get(norm_path) : path_in_loaded(norm_path, loaded) ? LoadResult{{}, mlc::Array<mlc::String>{mlc::String("circular: ") + norm_path}} : [&]() -> LoadResult { 
  loaded.push_back(norm_path);
  mlc::String source = mlc::file::read_to_string(path);
  return source.length() == 0 && !mlc::file::exists(path) ? LoadResult{{}, mlc::Array<mlc::String>{mlc::String("file not found: ") + path}} : [&]() -> LoadResult { 
  ast_tokens::LexOut lex = lexer::tokenize(source);
  return ast_tokens::LexOut_has_errors(lex) ? LoadResult{{}, mlc::Array<mlc::String>{mlc::String("lex ") + path + mlc::String(": ") + lex.errors[0]}} : [&]() -> LoadResult { 
  ast::Program prog = decls::parse_program(lex.tokens);
  mlc::Array<LoadItem> items = {};
  mlc::Array<mlc::String> seen = {};
  mlc::Array<std::shared_ptr<ast::Decl>> my_decls = {};
  mlc::Array<mlc::String> all_errors = {};
  int index = 0;
  while (index < prog.decls.size()){
{
[&]() -> void { if (std::holds_alternative<ast::DeclImport>((*prog.decls[index]))) { auto _v_declimport = std::get<ast::DeclImport>((*prog.decls[index])); auto [import_path, _w0] = _v_declimport; return [&]() { 
  mlc::String resolved = resolve_dotdot(resolve_import_path(path, import_path));
  LoadResult dep_result = load_module_impl(resolved, loaded, cache);
  all_errors = ast::errs_append(all_errors, dep_result.errors);
  int dep_i = 0;
  return [&]() { 
  while (dep_i < dep_result.items.size()){
{
LoadItem item = dep_result.items[dep_i];
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
 }(); } if (std::holds_alternative<ast::DeclExported>((*prog.decls[index]))) { auto _v_declexported = std::get<ast::DeclExported>((*prog.decls[index])); auto [d] = _v_declexported; return my_decls.push_back(d); } return my_decls.push_back(prog.decls[index]); }();
index = index + 1;
}
}
  items.push_back(LoadItem{norm_path, my_decls});
  LoadResult result = LoadResult{items, all_errors};
  cache.set(norm_path, result);
  return result;
 }();
 }();
 }();
}

LoadResult load_module(mlc::String path, mlc::HashMap<mlc::String, LoadResult> cache) noexcept{return load_module_impl(path, {}, cache);}

MergeResult merge_program(mlc::String entry_path, ast::Program prog) noexcept{
mlc::Array<std::shared_ptr<ast::Decl>> all_decls = {};
mlc::Array<mlc::String> all_errors = {};
mlc::Array<mlc::String> seen_paths = {};
mlc::HashMap<mlc::String, LoadResult> cache = mlc::HashMap<mlc::String, LoadResult>();
int index = 0;
while (index < prog.decls.size()){
{
[&]() -> void { if (std::holds_alternative<ast::DeclImport>((*prog.decls[index]))) { auto _v_declimport = std::get<ast::DeclImport>((*prog.decls[index])); auto [path, _w0] = _v_declimport; return [&]() { 
  mlc::String resolved = resolve_import_path(entry_path, path);
  LoadResult dep_result = load_module(resolved, cache);
  all_errors = ast::errs_append(all_errors, dep_result.errors);
  int dep_i = 0;
  return [&]() { 
  while (dep_i < dep_result.items.size()){
{
LoadItem item = dep_result.items[dep_i];
if (!path_in_loaded(item.path, seen_paths)){
{
seen_paths.push_back(item.path);
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
 }(); } if (std::holds_alternative<ast::DeclExported>((*prog.decls[index]))) { auto _v_declexported = std::get<ast::DeclExported>((*prog.decls[index])); auto [d] = _v_declexported; return all_decls.push_back(d); } return all_decls.push_back(prog.decls[index]); }();
index = index + 1;
}
}
return MergeResult{ast::Program{all_decls}, all_errors};
}

mlc::String compile_source(mlc::String src, mlc::String entry_path) noexcept{
ast_tokens::LexOut lex = lexer::tokenize(src);
return ast_tokens::LexOut_has_errors(lex) ? format_errs(mlc::String("lex"), lex.errors) : [&]() -> mlc::String { 
  ast::Program prog = decls::parse_program(lex.tokens);
  MergeResult merged_result = merge_program(entry_path, prog);
  return merged_result.errors.size() > 0 ? format_errs(mlc::String("import"), merged_result.errors) : [&]() -> mlc::String { 
  infer::CheckOut chk = infer::check(merged_result.prog);
  return infer::CheckOut_has_errors(chk) ? format_errs(mlc::String("check"), chk.errors) : codegen::gen_program(merged_result.prog);
 }();
 }();
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
mlc::Array<mlc::String> cli_args = mlc::io::args();
if (cli_args.size() == 0){
{
mlc::io::println(mlc::String("Usage: mlcc <source.mlc>"));
mlc::io::exit(1);
}
}
mlc::String src = mlc::file::read_to_string(cli_args[0]);
mlc::io::print(compile_source(src, cli_args[0]));
return 0;
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
