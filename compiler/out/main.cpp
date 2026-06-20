#include "mlc.hpp"

#define main mlc_user_main
#include "main.hpp"

#include "lexer.hpp"
#include "decls.hpp"
#include "decl_index.hpp"
#include "decl_index.hpp"
#include "ast.hpp"
#include "pipeline.hpp"
#include "compile_options.hpp"
#include "format_cli.hpp"
#include "profile.hpp"

namespace mlc_main {

using namespace lexer;
using namespace decls;
using namespace decl_index;
using namespace decl_index;
using namespace ast;
using namespace pipeline;
using namespace compile_options;
using namespace format_cli;
using namespace profile;
using namespace ast_tokens;

struct LoadResult {mlc::Array<decl_index::LoadItem> items;mlc::Array<mlc::String> errors;};

struct MergeResult {ast::Program program;mlc::Array<mlc::String> errors;mlc::Array<decl_index::LoadItem> items;};

mlc::String dirname(mlc::String path) noexcept;

mlc::String resolve_dotdot(mlc::String path) noexcept;

mlc::String resolve_import_path(mlc::String base_path, mlc::String import_path) noexcept;

mlc::Array<mlc::String> prefix_parse_errors(mlc::String source_path, mlc::Array<mlc::String> messages) noexcept;

LoadResult load_module_impl(mlc::String path, mlc::HashMap<mlc::String, bool>& loaded, mlc::HashMap<mlc::String, LoadResult>& cache, bool profile_enabled) noexcept;

LoadResult load_module(mlc::String path, mlc::HashMap<mlc::String, LoadResult>& cache, bool profile_enabled) noexcept;

MergeResult merge_program(mlc::String entry_path, ast::Program program, bool profile_enabled) noexcept;

ast::Result<mlc::String, mlc::Array<mlc::String>> compile_modular(mlc::String entry_path, mlc::String out_dir, bool profile_enabled, bool check_only, bool emit_compile_commands) noexcept;

mlc::String format_errors(mlc::String label, mlc::Array<mlc::String> errors) noexcept;

bool is_format_subcommand(mlc::String argument) noexcept;

int main(int argc, char** argv) noexcept;

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

mlc::Array<mlc::String> prefix_parse_errors(mlc::String source_path, mlc::Array<mlc::String> messages) noexcept{
mlc::Array<mlc::String> prefixed = {};
int index = 0;
while (index < messages.size()){
{
prefixed.push_back(mlc::String("parse ") + source_path + mlc::String(": ") + messages[index]);
index = index + 1;
}
}
return prefixed;
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
  ast_tokens::LexOut lexer_output = lexer::tokenize(source);
  profile::profile_maybe_end(profile_enabled, mlc::String("lex"));
  return ast_tokens::LexOut_has_errors(lexer_output) ? LoadResult{{}, mlc::Array<mlc::String>{mlc::String("lex ") + path + mlc::String(": ") + lexer_output.errors[0]}} : [&]() -> LoadResult { 
  profile::profile_maybe_begin(profile_enabled, mlc::String("parse"));
  predicates::ProgramParseValue parse_parsed = decls::parse_program_with_errors(lexer_output.tokens, norm_path);
  profile::profile_maybe_end(profile_enabled, mlc::String("parse"));
  ast::Program program = parse_parsed.program;
  mlc::Array<decl_index::LoadItem> items = {};
  mlc::HashMap<mlc::String, bool> loaded_paths_seen = mlc::HashMap<mlc::String, bool>();
  mlc::Array<std::shared_ptr<ast::Decl>> module_declarations = {};
  mlc::Array<mlc::String> my_imports = {};
  mlc::Array<decl_index::NamespaceImportAlias> my_namespace_import_aliases = {};
  mlc::Array<mlc::String> all_errors = {};
  int index = 0;
  while (index < program.decls.size()){
{
[&]() -> void { if (std::holds_alternative<ast::DeclImport>((*program.decls[index]))) { auto _v_declimport = std::get<ast::DeclImport>((*program.decls[index])); auto [import_path, symbols] = _v_declimport; return [&]() { 
  mlc::String resolved = resolve_dotdot(resolve_import_path(path, import_path));
  my_imports.push_back(resolved);
  if (symbols.size() >= 2 && symbols[0] == mlc::String("*")){
{
my_namespace_import_aliases.push_back(decl_index::NamespaceImportAlias{symbols[1], resolved});
}
}
  LoadResult dependency_parsed = load_module_impl(resolved, loaded, cache, profile_enabled);
  all_errors = ast::errs_append(all_errors, dependency_parsed.errors);
  int dep_item_index = 0;
  return [&]() { 
  while (dep_item_index < dependency_parsed.items.size()){
{
decl_index::LoadItem dependency_item = dependency_parsed.items[dep_item_index];
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
  items.push_back(decl_index::LoadItem{norm_path, module_declarations, my_imports, my_namespace_import_aliases});
  LoadResult load_parsed = LoadResult{items, all_errors};
  cache.set(norm_path, load_parsed);
  return load_parsed;
 }();
 }();
 }();
}

LoadResult load_module(mlc::String path, mlc::HashMap<mlc::String, LoadResult>& cache, bool profile_enabled) noexcept{
mlc::HashMap<mlc::String, bool> loaded = mlc::HashMap<mlc::String, bool>();
return load_module_impl(path, loaded, cache, profile_enabled);
}

MergeResult merge_program(mlc::String entry_path, ast::Program program, bool profile_enabled) noexcept{
mlc::Array<std::shared_ptr<ast::Decl>> merged_declarations = {};
mlc::Array<mlc::String> all_errors = {};
mlc::HashMap<mlc::String, bool> seen_paths = mlc::HashMap<mlc::String, bool>();
mlc::Array<decl_index::LoadItem> items_ordered = {};
mlc::Array<std::shared_ptr<ast::Decl>> entry_decls = {};
mlc::Array<mlc::String> entry_imports = {};
mlc::Array<decl_index::NamespaceImportAlias> entry_namespace_import_aliases = {};
mlc::HashMap<mlc::String, LoadResult> cache = mlc::HashMap<mlc::String, LoadResult>();
int index = 0;
while (index < program.decls.size()){
{
[&]() -> void { if (std::holds_alternative<ast::DeclImport>((*program.decls[index]))) { auto _v_declimport = std::get<ast::DeclImport>((*program.decls[index])); auto [path, symbols] = _v_declimport; return [&]() { 
  mlc::String resolved = resolve_dotdot(resolve_import_path(entry_path, path));
  entry_imports.push_back(resolved);
  if (symbols.size() >= 2 && symbols[0] == mlc::String("*")){
{
entry_namespace_import_aliases.push_back(decl_index::NamespaceImportAlias{symbols[1], resolved});
}
}
  LoadResult dependency_parsed = load_module(resolved, cache, profile_enabled);
  all_errors = ast::errs_append(all_errors, dependency_parsed.errors);
  int dep_item_index = 0;
  return [&]() { 
  while (dep_item_index < dependency_parsed.items.size()){
{
decl_index::LoadItem dependency_item = dependency_parsed.items[dep_item_index];
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
 }();
 }(); } if (std::holds_alternative<ast::DeclExported>((*program.decls[index]))) { auto _v_declexported = std::get<ast::DeclExported>((*program.decls[index])); auto [exported_declaration] = _v_declexported; return [&]() { 
  entry_decls.push_back(exported_declaration);
  return merged_declarations.push_back(exported_declaration);
 }(); } return [&]() { 
  entry_decls.push_back(program.decls[index]);
  return merged_declarations.push_back(program.decls[index]);
 }(); }();
index = index + 1;
}
}
mlc::String norm_entry = resolve_dotdot(entry_path);
items_ordered.push_back(decl_index::LoadItem{norm_entry, entry_decls, entry_imports, entry_namespace_import_aliases});
return MergeResult{ast::Program{merged_declarations}, all_errors, items_ordered};
}

ast::Result<mlc::String, mlc::Array<mlc::String>> compile_modular(mlc::String entry_path, mlc::String out_dir, bool profile_enabled, bool check_only, bool emit_compile_commands) noexcept{
profile::profile_reset_if_enabled(profile_enabled);
profile::profile_maybe_begin(profile_enabled, mlc::String("total"));
profile::profile_maybe_begin(profile_enabled, mlc::String("load_io"));
mlc::String entry_source = mlc::file::read_to_string(entry_path);
profile::profile_maybe_end(profile_enabled, mlc::String("load_io"));
profile::profile_maybe_begin(profile_enabled, mlc::String("lex"));
ast_tokens::LexOut lexer_output = lexer::tokenize(entry_source);
profile::profile_maybe_end(profile_enabled, mlc::String("lex"));
return ast_tokens::LexOut_has_errors(lexer_output) ? ast::Result<mlc::String, mlc::Array<mlc::String>>([&]() -> ast::Result<mlc::String, mlc::Array<mlc::String>> { 
  profile::profile_maybe_end(profile_enabled, mlc::String("total"));
  profile::profile_finish(profile_enabled);
  return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("lex: ") + lexer_output.errors[0]});
 }()) : ast::Result<mlc::String, mlc::Array<mlc::String>>([&]() -> ast::Result<mlc::String, mlc::Array<mlc::String>> { 
  mlc::String source_path = resolve_dotdot(entry_path);
  profile::profile_maybe_begin(profile_enabled, mlc::String("parse"));
  predicates::ProgramParseValue parse_parsed = decls::parse_program_with_errors(lexer_output.tokens, source_path);
  profile::profile_maybe_end(profile_enabled, mlc::String("parse"));
  return check_only && parse_parsed.errors.size() > 0 ? ast::Result<mlc::String, mlc::Array<mlc::String>>([&]() -> ast::Result<mlc::String, mlc::Array<mlc::String>> { 
  profile::profile_maybe_end(profile_enabled, mlc::String("total"));
  profile::profile_finish(profile_enabled);
  return ast::Err<mlc::Array<mlc::String>>(prefix_parse_errors(source_path, parse_parsed.errors));
 }()) : ast::Result<mlc::String, mlc::Array<mlc::String>>([&]() -> ast::Result<mlc::String, mlc::Array<mlc::String>> { 
  profile::profile_maybe_begin(profile_enabled, mlc::String("merge"));
  MergeResult merged = merge_program(entry_path, parse_parsed.program, profile_enabled);
  profile::profile_maybe_end(profile_enabled, mlc::String("merge"));
  return merged.errors.size() > 0 ? ast::Result<mlc::String, mlc::Array<mlc::String>>([&]() -> ast::Result<mlc::String, mlc::Array<mlc::String>> { 
  profile::profile_maybe_end(profile_enabled, mlc::String("total"));
  profile::profile_finish(profile_enabled);
  return ast::Err<mlc::Array<mlc::String>>(merged.errors);
 }()) : ast::Result<mlc::String, mlc::Array<mlc::String>>([&]() -> ast::Result<mlc::String, mlc::Array<mlc::String>> { 
  int merged_item_count = merged.items.size();
  decl_index::LoadItem entry_load_item = merged.items[merged_item_count - 1];
  ast::Program entry_only_program = ast::Program{entry_load_item.decls};
  pipeline::ModularCompileInput pipeline_input = pipeline::ModularCompileInput{merged.items, merged.program, entry_only_program, out_dir, profile_enabled, check_only, emit_compile_commands};
  auto __try_pipeline_parsed = pipeline::run_modular_compiler_pipeline(pipeline_input);
  if (std::holds_alternative<ast::Err<mlc::Array<mlc::String>>>(__try_pipeline_parsed)) return ast::Result<mlc::String, mlc::Array<mlc::String>>(std::get<ast::Err<mlc::Array<mlc::String>>>(__try_pipeline_parsed));
  mlc::String pipeline_parsed = std::get<ast::Ok<mlc::String>>(__try_pipeline_parsed).field0;
  profile::profile_maybe_end(profile_enabled, mlc::String("total"));
  profile::profile_finish(profile_enabled);
  return ast::Ok<mlc::String>(mlc::String(""));
 }());
 }());
 }());
}

mlc::String format_errors(mlc::String label, mlc::Array<mlc::String> errors) noexcept{return errors.map([label](mlc::String message_line) mutable { return label + mlc::String(": ") + message_line + mlc::String("\n"); }).join(mlc::String(""));}

bool is_format_subcommand(mlc::String argument) noexcept{return [&]() { if (argument == mlc::String("fmt")) { return true; } return false; }();}

int main(int argc, char** argv) noexcept{
mlc::io::set_args(std::vector<mlc::String>(argv + 1, argv + argc));
mlc::Array<mlc::String> command_line_arguments = mlc::io::args();
if (command_line_arguments.size() == 0){
{
mlc::io::println(compile_options::compile_usage_message());
mlc::io::exit(1);
}
}
return is_format_subcommand(command_line_arguments[0]) ? [&]() -> int { 
  if (command_line_arguments.size() < 2){
{
mlc::io::println(format_cli::format_usage_message());
mlc::io::exit(1);
}
}
  return std::visit(overloaded{
  [&](const ast::Ok<mlc::String>& ok) -> int { auto [formatted_source] = ok; return [&]() -> int { 
  mlc::io::print(formatted_source);
  return 0;
 }(); },
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> int { auto [errors] = err; return [&]() -> int { 
  mlc::io::print(format_errors(mlc::String("error"), errors));
  mlc::io::exit(1);
  return 0;
 }(); }
}, format_cli::format_source_file(command_line_arguments[1]));
 }() : [&]() -> int { 
  compile_options::CompileOptions options = compile_options::parse_compile_options(command_line_arguments);
  if (options.entry_path.length() == 0){
{
mlc::io::println(compile_options::compile_usage_message());
mlc::io::exit(1);
}
}
  return std::visit(overloaded{
  [&](const ast::Ok<mlc::String>& ok) -> int { auto [_w0] = ok; return 0; },
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> int { auto [errors] = err; return [&]() -> int { 
  mlc::io::print(format_errors(mlc::String("error"), errors));
  mlc::io::exit(1);
  return 0;
 }(); }
}, compile_modular(options.entry_path, options.out_directory, options.profile_enabled, options.check_only, options.emit_compile_commands));
 }();
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
