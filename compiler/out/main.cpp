#include "mlc.hpp"

#define main mlc_user_main
#include "main.hpp"

#include "lexer.hpp"
#include "decls.hpp"
#include "check.hpp"
#include "transform_decl.hpp"
#include "registry.hpp"
#include "trait_param_expand.hpp"
#include "module.hpp"
#include "decl_index.hpp"
#include "decl_index.hpp"
#include "cpp_naming.hpp"
#include "param_destructure_expand.hpp"
#include "ast.hpp"
#include "profile.hpp"

namespace mlc_main {

using namespace lexer;
using namespace decls;
using namespace check;
using namespace transform_decl;
using namespace registry;
using namespace trait_param_expand;
using namespace module;
using namespace decl_index;
using namespace decl_index;
using namespace cpp_naming;
using namespace param_destructure_expand;
using namespace ast;
using namespace profile;
using namespace ast_tokens;

struct CompileOptions {mlc::String entry_path;mlc::String out_directory;bool profile_enabled;};

struct LoadResult {mlc::Array<decl_index::LoadItem> items;mlc::Array<mlc::String> errors;};

struct MergeResult {ast::Program prog;mlc::Array<mlc::String> errors;mlc::Array<decl_index::LoadItem> items;};

CompileOptions parse_compile_options(mlc::Array<mlc::String> arguments) noexcept;

mlc::String dirname(mlc::String path) noexcept;

mlc::String resolve_dotdot(mlc::String path) noexcept;

mlc::String resolve_import_path(mlc::String base_path, mlc::String import_path) noexcept;

LoadResult load_module_impl(mlc::String path, mlc::HashMap<mlc::String, bool>& loaded, mlc::HashMap<mlc::String, LoadResult>& cache, bool profile_enabled) noexcept;

LoadResult load_module(mlc::String path, mlc::HashMap<mlc::String, LoadResult>& cache, bool profile_enabled) noexcept;

MergeResult merge_program(mlc::String entry_path, ast::Program prog, bool profile_enabled) noexcept;

mlc::String compile_modular_loop(mlc::Array<decl_index::LoadItem> items, ast::Program full_prog, mlc::String out_dir, bool profile_enabled, registry::TypeRegistry registry) noexcept;

ast::Result<mlc::String, mlc::Array<mlc::String>> compile_modular(mlc::String entry_path, mlc::String out_dir, bool profile_enabled) noexcept;

mlc::String format_errs(mlc::String label, mlc::Array<mlc::String> errors) noexcept;

int main(int argc, char** argv) noexcept;

CompileOptions parse_compile_options(mlc::Array<mlc::String> arguments) noexcept{
bool profile_enabled = false;
mlc::String out_directory = mlc::String("out");
mlc::String entry_path = mlc::String("");
int index = 0;
while (index < arguments.size()){
{
mlc::String argument = arguments[index];
if (argument == mlc::String("--profile")){
{
profile_enabled = true;
}
} else {
{
if (argument == mlc::String("-o") && index + 1 < arguments.size()){
out_directory = arguments[index + 1];
index = index + 1;
} else {
if (entry_path.length() == 0){
entry_path = argument;
}
}
}
}
index = index + 1;
}
}
return CompileOptions{entry_path, out_directory, profile_enabled};
}

mlc::String dirname(mlc::String path) noexcept{
int last_slash_index = -1;
int path_character_index = 0;
while (path_character_index < path.length()){
{
if (path.char_at(path_character_index) == mlc::String("/")){
{
last_slash_index = path_character_index;
}
}
path_character_index = path_character_index + 1;
}
}
return last_slash_index <= 0 ? mlc::String("") : path.substring(0, last_slash_index);
}

mlc::String resolve_dotdot(mlc::String path) noexcept{
mlc::String remaining_path = path;
int scan_index = 0;
while (scan_index + 4 <= remaining_path.length()){
{
if (remaining_path.substring(scan_index, 4) == mlc::String("/../")){
{
int reverse_segment_index = scan_index - 1;
while (reverse_segment_index >= 0 && remaining_path.char_at(reverse_segment_index) != mlc::String("/")){
{
reverse_segment_index = reverse_segment_index - 1;
}
}
mlc::String prefix_part = reverse_segment_index <= 0 ? mlc::String("") : remaining_path.substring(0, reverse_segment_index);
mlc::String suffix_part = remaining_path.substring(scan_index + 4, remaining_path.length() - scan_index - 4);
remaining_path = prefix_part == mlc::String("") ? suffix_part : prefix_part + mlc::String("/") + suffix_part;
scan_index = 0;
}
}
scan_index = scan_index + 1;
}
}
if (remaining_path.length() >= 3 && remaining_path.substring(0, 3) == mlc::String("../")){
{
remaining_path = remaining_path.substring(3, remaining_path.length() - 3);
}
}
return remaining_path;
}

mlc::String resolve_import_path(mlc::String base_path, mlc::String import_path) noexcept{
mlc::String base_dir = dirname(base_path);
mlc::String rest = import_path.length() >= 2 && import_path.substring(0, 2) == mlc::String("./") ? import_path.substring(2, import_path.length() - 2) : import_path;
mlc::String with_extension = rest.length() >= 4 && rest.substring(rest.length() - 4, 4) == mlc::String(".mlc") ? rest : rest + mlc::String(".mlc");
mlc::String combined_before_normalization = base_dir == mlc::String("") ? with_extension : base_dir + mlc::String("/") + with_extension;
return resolve_dotdot(combined_before_normalization);
}

LoadResult load_module_impl(mlc::String path, mlc::HashMap<mlc::String, bool>& loaded, mlc::HashMap<mlc::String, LoadResult>& cache, bool profile_enabled) noexcept{
mlc::String norm_path = resolve_dotdot(path);
return cache.has(norm_path) ? cache.get(norm_path) : loaded.has(norm_path) ? LoadResult{{}, mlc::Array<mlc::String>{mlc::String("circular: ") + norm_path}} : [&]() -> LoadResult { 
  loaded.set(norm_path, true);
  profile::profile_maybe_begin(profile_enabled, mlc::String("load_io"));
  mlc::String source = mlc::file::read_to_string(path);
  profile::profile_maybe_end(profile_enabled, mlc::String("load_io"));
  return source.length() == 0 && !mlc::file::exists(path) ? LoadResult{{}, mlc::Array<mlc::String>{mlc::String("file not found: ") + path}} : [&]() -> LoadResult { 
  profile::profile_maybe_begin(profile_enabled, mlc::String("lex"));
  ast_tokens::LexOut lex = lexer::tokenize(source);
  profile::profile_maybe_end(profile_enabled, mlc::String("lex"));
  return ast_tokens::LexOut_has_errors(lex) ? LoadResult{{}, mlc::Array<mlc::String>{mlc::String("lex ") + path + mlc::String(": ") + lex.errors[0]}} : [&]() -> LoadResult { 
  profile::profile_maybe_begin(profile_enabled, mlc::String("parse"));
  ast::Program prog = decls::parse_program_with_source_path(lex.tokens, norm_path);
  profile::profile_maybe_end(profile_enabled, mlc::String("parse"));
  mlc::Array<decl_index::LoadItem> items = {};
  mlc::HashMap<mlc::String, bool> seen = mlc::HashMap<mlc::String, bool>();
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
  LoadResult dep_result = load_module_impl(resolved, loaded, cache, profile_enabled);
  all_errors = ast::errs_append(all_errors, dep_result.errors);
  int dep_item_index = 0;
  return [&]() { 
  while (dep_item_index < dep_result.items.size()){
{
decl_index::LoadItem dependency_item = dep_result.items[dep_item_index];
if (!seen.has(dependency_item.path)){
{
seen.set(dependency_item.path, true);
items.push_back(dependency_item);
}
}
dep_item_index = dep_item_index + 1;
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

LoadResult load_module(mlc::String path, mlc::HashMap<mlc::String, LoadResult>& cache, bool profile_enabled) noexcept{
mlc::HashMap<mlc::String, bool> loaded = mlc::HashMap<mlc::String, bool>();
return load_module_impl(path, loaded, cache, profile_enabled);
}

MergeResult merge_program(mlc::String entry_path, ast::Program prog, bool profile_enabled) noexcept{
mlc::Array<std::shared_ptr<ast::Decl>> all_decls = {};
mlc::Array<mlc::String> all_errors = {};
mlc::HashMap<mlc::String, bool> seen_paths = mlc::HashMap<mlc::String, bool>();
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
  LoadResult dep_result = load_module(resolved, cache, profile_enabled);
  all_errors = ast::errs_append(all_errors, dep_result.errors);
  int dep_item_index = 0;
  return [&]() { 
  while (dep_item_index < dep_result.items.size()){
{
decl_index::LoadItem dependency_item = dep_result.items[dep_item_index];
if (!seen_paths.has(dependency_item.path)){
{
seen_paths.set(dependency_item.path, true);
items_ordered.push_back(dependency_item);
int decl_inner_index = 0;
[&]() { 
  while (decl_inner_index < dependency_item.decls.size()){
{
all_decls.push_back(dependency_item.decls[decl_inner_index]);
decl_inner_index = decl_inner_index + 1;
}
}
 }();
}
}
dep_item_index = dep_item_index + 1;
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

mlc::String compile_modular_loop(mlc::Array<decl_index::LoadItem> items, ast::Program full_prog, mlc::String out_dir, bool profile_enabled, registry::TypeRegistry registry) noexcept{
profile::profile_maybe_begin(profile_enabled, mlc::String("expand_destructure"));
ast::Program expanded_merge_program = param_destructure_expand::expand_parameter_destructuring_in_program(full_prog);
profile::profile_maybe_end(profile_enabled, mlc::String("expand_destructure"));
trait_param_expand::TraitNominalMaps trait_maps = trait_param_expand::build_trait_nominal_maps(expanded_merge_program);
profile::profile_maybe_begin(profile_enabled, mlc::String("transform"));
mlc::Array<semantic_ir::SLoadItem> transformed_items = transform_decl::transform_load_items(items, registry, trait_maps);
profile::profile_maybe_end(profile_enabled, mlc::String("transform"));
profile::profile_maybe_begin(profile_enabled, mlc::String("precompute"));
context::PrecomputedCtx precomputed = module::precompute(expanded_merge_program, items);
profile::profile_maybe_end(profile_enabled, mlc::String("precompute"));
profile::profile_maybe_begin(profile_enabled, mlc::String("codegen"));
std::tuple<> unit_placeholder = std::make_tuple();
unit_placeholder = transformed_items.fold(unit_placeholder, [items, expanded_merge_program, precomputed, out_dir](std::tuple<> _, semantic_ir::SLoadItem transformed_load_item) mutable { return [&]() -> std::tuple<> { 
  context::GenModuleOut generated_output = module::gen_module(transformed_load_item, items, expanded_merge_program, precomputed);
  mlc::String module_base = cpp_naming::path_to_module_base(transformed_load_item.path);
  mlc::String header_path = out_dir.length() > 0 ? out_dir + mlc::String("/") + module_base + mlc::String(".hpp") : module_base + mlc::String(".hpp");
  mlc::String implementation_path = out_dir.length() > 0 ? out_dir + mlc::String("/") + module_base + mlc::String(".cpp") : module_base + mlc::String(".cpp");
  mlc::file::write_string(header_path, generated_output.h);
  mlc::file::write_string(implementation_path, generated_output.c);
  return std::make_tuple();
 }(); });
profile::profile_maybe_end(profile_enabled, mlc::String("codegen"));
return mlc::String("");
}

ast::Result<mlc::String, mlc::Array<mlc::String>> compile_modular(mlc::String entry_path, mlc::String out_dir, bool profile_enabled) noexcept{
profile::profile_reset_if_enabled(profile_enabled);
profile::profile_maybe_begin(profile_enabled, mlc::String("total"));
profile::profile_maybe_begin(profile_enabled, mlc::String("load_io"));
mlc::String entry_source = mlc::file::read_to_string(entry_path);
profile::profile_maybe_end(profile_enabled, mlc::String("load_io"));
profile::profile_maybe_begin(profile_enabled, mlc::String("lex"));
ast_tokens::LexOut lex = lexer::tokenize(entry_source);
profile::profile_maybe_end(profile_enabled, mlc::String("lex"));
return ast_tokens::LexOut_has_errors(lex) ? ast::Result<mlc::String, mlc::Array<mlc::String>>([&]() -> ast::Result<mlc::String, mlc::Array<mlc::String>> { 
  profile::profile_maybe_end(profile_enabled, mlc::String("total"));
  profile::profile_finish(profile_enabled);
  return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("lex: ") + lex.errors[0]});
 }()) : ast::Result<mlc::String, mlc::Array<mlc::String>>([&]() -> ast::Result<mlc::String, mlc::Array<mlc::String>> { 
  mlc::String source_path = resolve_dotdot(entry_path);
  profile::profile_maybe_begin(profile_enabled, mlc::String("parse"));
  ast::Program parsed_program = decls::parse_program_with_source_path(lex.tokens, source_path);
  profile::profile_maybe_end(profile_enabled, mlc::String("parse"));
  profile::profile_maybe_begin(profile_enabled, mlc::String("merge"));
  MergeResult merged = merge_program(entry_path, parsed_program, profile_enabled);
  profile::profile_maybe_end(profile_enabled, mlc::String("merge"));
  return merged.errors.size() > 0 ? ast::Result<mlc::String, mlc::Array<mlc::String>>([&]() -> ast::Result<mlc::String, mlc::Array<mlc::String>> { 
  profile::profile_maybe_end(profile_enabled, mlc::String("total"));
  profile::profile_finish(profile_enabled);
  return ast::Err<mlc::Array<mlc::String>>(merged.errors);
 }()) : ast::Result<mlc::String, mlc::Array<mlc::String>>([&]() -> ast::Result<mlc::String, mlc::Array<mlc::String>> { 
  int merged_item_count = merged.items.size();
  decl_index::LoadItem entry_load_item = merged.items[merged_item_count - 1];
  ast::Program entry_only_program = ast::Program{entry_load_item.decls};
  profile::profile_maybe_begin(profile_enabled, mlc::String("check"));
  auto __try_checked = check::check_with_context(entry_only_program, merged.prog);
  if (std::holds_alternative<ast::Err<mlc::Array<mlc::String>>>(__try_checked)) return ast::Result<mlc::String, mlc::Array<mlc::String>>(std::get<ast::Err<mlc::Array<mlc::String>>>(__try_checked));
  check::CheckOut checked = std::get<ast::Ok<check::CheckOut>>(__try_checked).field0;
  profile::profile_maybe_end(profile_enabled, mlc::String("check"));
  compile_modular_loop(merged.items, merged.prog, out_dir, profile_enabled, checked.registry);
  profile::profile_maybe_end(profile_enabled, mlc::String("total"));
  profile::profile_finish(profile_enabled);
  return ast::Ok<mlc::String>(mlc::String(""));
 }());
 }());
}

mlc::String format_errs(mlc::String label, mlc::Array<mlc::String> errors) noexcept{return errors.map([label](mlc::String message_line) mutable { return label + mlc::String(": ") + message_line + mlc::String("\n"); }).join(mlc::String(""));}

int main(int argc, char** argv) noexcept{
mlc::io::set_args(std::vector<mlc::String>(argv + 1, argv + argc));
mlc::Array<mlc::String> command_line_arguments = mlc::io::args();
if (command_line_arguments.size() == 0){
{
mlc::io::println(mlc::String("Usage: mlcc [--profile] <source.mlc> [-o out_dir]"));
mlc::io::exit(1);
}
}
CompileOptions options = parse_compile_options(command_line_arguments);
if (options.entry_path.length() == 0){
{
mlc::io::println(mlc::String("Usage: mlcc [--profile] <source.mlc> [-o out_dir]"));
mlc::io::exit(1);
}
}
return std::visit(overloaded{
  [&](const ast::Ok<mlc::String>& ok) -> int { auto [_w0] = ok; return 0; },
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> int { auto [errors] = err; return [&]() -> int { 
  mlc::io::print(format_errs(mlc::String("error"), errors));
  mlc::io::exit(1);
  return 0;
 }(); }
}, compile_modular(options.entry_path, options.out_directory, options.profile_enabled));
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
