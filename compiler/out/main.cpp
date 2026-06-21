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

mlc::String dirname(mlc::String path) noexcept{
  auto last_slash_index = (-1);
  auto path_character_index = 0;
  while ((path_character_index < path.length()))   {
    if ((path.char_at(path_character_index) == mlc::String("/", 1)))     {
      (last_slash_index = path_character_index);
    }
    (path_character_index = (path_character_index + 1));
  }
  if ((last_slash_index <= 0))   {
    return mlc::String("", 0);
  } else   {
    return path.substring(0, last_slash_index);
  }
}
mlc::String resolve_dotdot(mlc::String path) noexcept{
  auto remaining_path = path;
  auto scan_index = 0;
  while (((scan_index + 4) <= remaining_path.length()))   {
    if ((remaining_path.substring(scan_index, 4) == mlc::String("/../", 4)))     {
      auto reverse_segment_index = (scan_index - 1);
      while (((reverse_segment_index >= 0) && (remaining_path.char_at(reverse_segment_index) != mlc::String("/", 1))))       {
        (reverse_segment_index = (reverse_segment_index - 1));
      }
      auto prefix_part = ((reverse_segment_index <= 0) ? (mlc::String("", 0)) : (remaining_path.substring(0, reverse_segment_index)));
      auto suffix_part = remaining_path.substring((scan_index + 4), ((remaining_path.length() - scan_index) - 4));
      (remaining_path = ((prefix_part == mlc::String("", 0)) ? (suffix_part) : (((((mlc::String("", 0) + mlc::to_string(prefix_part)) + mlc::String("/", 1)) + mlc::to_string(suffix_part)) + mlc::String("", 0)))));
      (scan_index = 0);
    }
    (scan_index = (scan_index + 1));
  }
  if (((remaining_path.length() >= 3) && (remaining_path.substring(0, 3) == mlc::String("../", 3))))   {
    (remaining_path = remaining_path.substring(3, (remaining_path.length() - 3)));
  }
  return remaining_path;
}
mlc::String resolve_import_path(mlc::String base_path, mlc::String import_path) noexcept{
  auto base_dir = dirname(base_path);
  auto rest = (((import_path.length() >= 2) && (import_path.substring(0, 2) == mlc::String("./", 2))) ? (import_path.substring(2, (import_path.length() - 2))) : (import_path));
  auto with_extension = (((rest.length() >= 4) && (rest.substring((rest.length() - 4), 4) == mlc::String(".mlc", 4))) ? (rest) : (((mlc::String("", 0) + mlc::to_string(rest)) + mlc::String(".mlc", 4))));
  auto combined_before_normalization = ((base_dir == mlc::String("", 0)) ? (with_extension) : (((((mlc::String("", 0) + mlc::to_string(base_dir)) + mlc::String("/", 1)) + mlc::to_string(with_extension)) + mlc::String("", 0))));
  return resolve_dotdot(combined_before_normalization);
}
mlc::Array<mlc::String> prefix_parse_errors(mlc::String source_path, mlc::Array<mlc::String> messages) noexcept{
  auto prefixed = mlc::Array<mlc::String>{};
  auto index = 0;
  while ((index < messages.length()))   {
    prefixed.push_back(((((mlc::String("parse ", 6) + mlc::to_string(source_path)) + mlc::String(": ", 2)) + mlc::to_string(messages[index])) + mlc::String("", 0)));
    (index = (index + 1));
  }
  return prefixed;
}
LoadResult load_module_impl(mlc::String path, mlc::HashMap<mlc::String, bool>& loaded, mlc::HashMap<mlc::String, LoadResult>& cache, bool profile_enabled) noexcept{
  auto norm_path = resolve_dotdot(path);
  if (cache.has(norm_path))   {
    return cache.get(norm_path);
  } else if (loaded.has(norm_path))   {
    return LoadResult{{}, mlc::Array<mlc::String>{((mlc::String("circular: ", 10) + mlc::to_string(norm_path)) + mlc::String("", 0))}};
  } else   {
    loaded.set(norm_path, true);
    profile::profile_maybe_begin(profile_enabled, mlc::String("load_io", 7));
    auto source = mlc::file::read_to_string(path);
    profile::profile_maybe_end(profile_enabled, mlc::String("load_io", 7));
    if (((source.length() == 0) && (!mlc::file::exists(path))))     {
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
        auto items = mlc::Array<decl_index::LoadItem>{};
        auto loaded_paths_seen = mlc::HashMap<mlc::String, bool>();
        auto module_declarations = mlc::Array<std::shared_ptr<ast::Decl>>{};
        auto my_imports = mlc::Array<mlc::String>{};
        auto my_namespace_import_aliases = mlc::Array<decl_index::NamespaceImportAlias>{};
        auto all_errors = mlc::Array<mlc::String>{};
        auto index = 0;
        while ((index < program.decls.length()))         {
          std::visit(overloaded{[&](const ast::DeclImport& declImport) -> void { auto [import_path, symbols] = declImport; [&]() {
auto resolved = resolve_dotdot(resolve_import_path(path, import_path));
my_imports.push_back(resolved);
if (((symbols.length() >= 2) && (symbols[0] == mlc::String("*", 1)))) {
  my_namespace_import_aliases.push_back(decl_index::NamespaceImportAlias{symbols[1], resolved});
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
(dep_item_index = (dep_item_index + 1));
}
}();
}(); },
[&](const ast::DeclExported& declExported) -> void { auto [__0] = declExported; module_declarations.push_back(program.decls[index]); },
[&](const auto& __v) -> void { module_declarations.push_back(program.decls[index]); }
}, (*program.decls[index]));
          (index = (index + 1));
        }
        items.push_back(decl_index::LoadItem{norm_path, module_declarations, my_imports, my_namespace_import_aliases});
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
MergeResult merge_program(mlc::String entry_path, ast::Program program, bool profile_enabled) noexcept{
  auto merged_declarations = mlc::Array<std::shared_ptr<ast::Decl>>{};
  auto all_errors = mlc::Array<mlc::String>{};
  auto seen_paths = mlc::HashMap<mlc::String, bool>();
  auto items_ordered = mlc::Array<decl_index::LoadItem>{};
  auto entry_decls = mlc::Array<std::shared_ptr<ast::Decl>>{};
  auto entry_imports = mlc::Array<mlc::String>{};
  auto entry_namespace_import_aliases = mlc::Array<decl_index::NamespaceImportAlias>{};
  auto cache = mlc::HashMap<mlc::String, LoadResult>();
  auto index = 0;
  while ((index < program.decls.length()))   {
    std::visit(overloaded{[&](const ast::DeclImport& declImport) -> void { auto [path, symbols] = declImport; [&]() {
auto resolved = resolve_dotdot(resolve_import_path(entry_path, path));
entry_imports.push_back(resolved);
if (((symbols.length() >= 2) && (symbols[0] == mlc::String("*", 1)))) {
  entry_namespace_import_aliases.push_back(decl_index::NamespaceImportAlias{symbols[1], resolved});
}
auto dependency_parsed = load_module(resolved, cache, profile_enabled);
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
(decl_inner_index = (decl_inner_index + 1));
}
}();
}
(dep_item_index = (dep_item_index + 1));
}
}();
}(); },
[&](const ast::DeclExported& declExported) -> void { auto [exported_declaration] = declExported; [&]() {
entry_decls.push_back(exported_declaration);
return merged_declarations.push_back(exported_declaration);
}(); },
[&](const auto& __v) -> void { [&]() {
entry_decls.push_back(program.decls[index]);
return merged_declarations.push_back(program.decls[index]);
}(); }
}, (*program.decls[index]));
    (index = (index + 1));
  }
  auto norm_entry = resolve_dotdot(entry_path);
  items_ordered.push_back(decl_index::LoadItem{norm_entry, entry_decls, entry_imports, entry_namespace_import_aliases});
  return MergeResult{ast::Program{merged_declarations}, all_errors, items_ordered};
}
ast::Result<mlc::String, mlc::Array<mlc::String>> compile_modular(mlc::String entry_path, mlc::String out_dir, bool profile_enabled, bool check_only, bool emit_compile_commands) noexcept{
  profile::profile_reset_if_enabled(profile_enabled);
  profile::profile_maybe_begin(profile_enabled, mlc::String("total", 5));
  profile::profile_maybe_begin(profile_enabled, mlc::String("load_io", 7));
  auto entry_source = mlc::file::read_to_string(entry_path);
  profile::profile_maybe_end(profile_enabled, mlc::String("load_io", 7));
  profile::profile_maybe_begin(profile_enabled, mlc::String("lex", 3));
  auto lexer_output = lexer::tokenize(entry_source);
  profile::profile_maybe_end(profile_enabled, mlc::String("lex", 3));
  if (ast_tokens::LexOut_has_errors(lexer_output))   {
    profile::profile_maybe_end(profile_enabled, mlc::String("total", 5));
    profile::profile_finish(profile_enabled);
    return ast::Err{mlc::Array<mlc::String>{((mlc::String("lex: ", 5) + mlc::to_string(lexer_output.errors[0])) + mlc::String("", 0))}};
  } else   {
    auto source_path = resolve_dotdot(entry_path);
    profile::profile_maybe_begin(profile_enabled, mlc::String("parse", 5));
    auto parse_parsed = decls::parse_program_with_errors(lexer_output.tokens, source_path);
    profile::profile_maybe_end(profile_enabled, mlc::String("parse", 5));
    if ((check_only && (parse_parsed.errors.length() > 0)))     {
      profile::profile_maybe_end(profile_enabled, mlc::String("total", 5));
      profile::profile_finish(profile_enabled);
      return ast::Err{prefix_parse_errors(source_path, parse_parsed.errors)};
    } else     {
      profile::profile_maybe_begin(profile_enabled, mlc::String("merge", 5));
      auto merged = merge_program(entry_path, parse_parsed.program, profile_enabled);
      profile::profile_maybe_end(profile_enabled, mlc::String("merge", 5));
      if ((merged.errors.length() > 0))       {
        profile::profile_maybe_end(profile_enabled, mlc::String("total", 5));
        profile::profile_finish(profile_enabled);
        return ast::Err{merged.errors};
      } else       {
        auto merged_item_count = merged.items.length();
        auto entry_load_item = merged.items[(merged_item_count - 1)];
        auto entry_only_program = ast::Program{entry_load_item.decls};
        auto pipeline_input = pipeline::ModularCompileInput{merged.items, merged.program, entry_only_program, out_dir, profile_enabled, check_only, emit_compile_commands};
        auto pipeline_parsed = ({ auto __q = pipeline::run_modular_compiler_pipeline(pipeline_input); if (std::get_if<1>(&__q)) return *std::get_if<1>(&__q); std::get<0>(__q).field0; });
        profile::profile_maybe_end(profile_enabled, mlc::String("total", 5));
        profile::profile_finish(profile_enabled);
        return ast::Ok{mlc::String("", 0)};
      }
    }
  }
}
mlc::String format_errors(mlc::String label, mlc::Array<mlc::String> errors) noexcept{
  return errors.map([=](mlc::String message_line) mutable { return ((((mlc::String("", 0) + mlc::to_string(label)) + mlc::String(": ", 2)) + mlc::to_string(message_line)) + mlc::String("\n", 1)); }).join(mlc::String("", 0));
}
bool is_format_subcommand(mlc::String argument) noexcept{
  return [&]() -> bool {
auto __match_subject = argument;
if ((__match_subject == mlc::String("fmt", 3))) {
  return true;
} else {
  return false;
}
}();
}
int main(int argc, char** argv) noexcept{
  mlc::io::set_args(std::vector<mlc::String>((argv + 1), (argv + argc)));
  auto command_line_arguments = mlc::io::args();
  if ((command_line_arguments.length() == 0))   {
    mlc::io::println(compile_options::compile_usage_message());
    mlc::io::exit(1);
  }
  if (is_format_subcommand(command_line_arguments[0]))   {
    if ((command_line_arguments.length() < 2))     {
      mlc::io::println(format_cli::format_usage_message());
      mlc::io::exit(1);
    }
    return std::visit(overloaded{[&]<typename __mlcT>(const ast::Ok<__mlcT>& ok) { auto [formatted_source] = ok; return [&]() {
mlc::io::print(formatted_source);
return 0;
}(); },
[&]<typename __mlcT>(const ast::Err<__mlcT>& err) { auto [errors] = err; return [&]() {
mlc::io::print(format_errors(mlc::String("error", 5), errors));
mlc::io::exit(1);
return 0;
}(); }
}, format_cli::format_source_file(command_line_arguments[1]));
  } else   {
    auto options = compile_options::parse_compile_options(command_line_arguments);
    if ((options.entry_path.length() == 0))     {
      mlc::io::println(compile_options::compile_usage_message());
      mlc::io::exit(1);
    }
    return std::visit(overloaded{[&]<typename __mlcT>(const ast::Ok<__mlcT>& ok) { auto [__0] = ok; return 0; },
[&]<typename __mlcT>(const ast::Err<__mlcT>& err) { auto [errors] = err; return [&]() {
mlc::io::print(format_errors(mlc::String("error", 5), errors));
mlc::io::exit(1);
return 0;
}(); }
}, compile_modular(options.entry_path, options.out_directory, options.profile_enabled, options.check_only, options.emit_compile_commands));
  }
}

} // namespace mlc_main

#undef main

static void mlc_cli_set_args(int argc, char** argv) {
  std::vector<mlc::String> arguments;
  arguments.reserve(argc > 0 ? argc - 1 : 0);
  for (int i = 1; i < argc; ++i) { arguments.emplace_back(argv[i]); }
  mlc::io::set_args(std::move(arguments));
}

int main(int argc, char** argv) {
  mlc_cli_set_args(argc, argv);
  return ::mlc_main::mlc_user_main(argc, argv);
}
