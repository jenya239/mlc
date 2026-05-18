#define main mlc_user_main
#include "main.hpp"

namespace mlc_main {

mlc::String dirname(mlc::String path) noexcept{
auto last_slash_index = (-1);
auto path_character_index = 0;
while ((path_character_index < path.length())) {
if ((path.char_at(path_character_index) == mlc::String("/", 1))) {
last_slash_index = path_character_index;
}
path_character_index = (path_character_index + 1);
}
if ((last_slash_index <= 0)) {
return mlc::String("", 0);
} else {
return path.substring(0, last_slash_index);
}
}
mlc::String resolve_dotdot(mlc::String path) noexcept{
auto remaining_path = path;
auto scan_index = 0;
while (((scan_index + 4) <= remaining_path.length())) {
if ((remaining_path.substring(scan_index, 4) == mlc::String("/../", 4))) {
auto reverse_segment_index = (scan_index - 1);
while (((reverse_segment_index >= 0) && (remaining_path.char_at(reverse_segment_index) != mlc::String("/", 1)))) {
reverse_segment_index = (reverse_segment_index - 1);
}
auto prefix_part = ((reverse_segment_index <= 0) ? (mlc::String("", 0)) : (remaining_path.substring(0, reverse_segment_index)));
auto suffix_part = remaining_path.substring((scan_index + 4), ((remaining_path.length() - scan_index) - 4));
remaining_path = ((prefix_part == mlc::String("", 0)) ? (suffix_part) : (((((mlc::String("", 0) + mlc::to_string(prefix_part)) + mlc::String("/", 1)) + mlc::to_string(suffix_part)) + mlc::String("", 0))));
scan_index = 0;
}
scan_index = (scan_index + 1);
}
if (((remaining_path.length() >= 3) && (remaining_path.substring(0, 3) == mlc::String("../", 3)))) {
remaining_path = remaining_path.substring(3, (remaining_path.length() - 3));
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
bool path_in_loaded(mlc::String path, mlc::Array<mlc::String> loaded) noexcept{
return loaded.any([=](mlc::String existing_path) mutable { return (existing_path == path); });
}
LoadResult load_module_impl(mlc::String path, mlc::Array<mlc::String> loaded, mlc::HashMap<mlc::String, LoadResult>& cache) noexcept{
auto norm_path = resolve_dotdot(path);
if (cache.has(norm_path)) {
return cache.get(norm_path);
} else if (path_in_loaded(norm_path, loaded)) {
return LoadResult{{}, mlc::Array<mlc::String>{((mlc::String("circular: ", 10) + mlc::to_string(norm_path)) + mlc::String("", 0))}};
} else {
auto cur_loaded = loaded;
cur_loaded.push_back(norm_path);
auto source = mlc::file::read_to_string(path);
if (((source.length() == 0) && (!mlc::file::exists(path)))) {
return LoadResult{{}, mlc::Array<mlc::String>{((mlc::String("file not found: ", 16) + mlc::to_string(path)) + mlc::String("", 0))}};
} else {
auto lex = lexer::tokenize(source);
if (ast_tokens::LexOut_has_errors(lex)) {
return LoadResult{{}, mlc::Array<mlc::String>{((((mlc::String("lex ", 4) + mlc::to_string(path)) + mlc::String(": ", 2)) + mlc::to_string(lex.errors[0])) + mlc::String("", 0))}};
} else {
auto prog = decls::parse_program_with_source_path(lex.tokens, norm_path);
auto items = mlc::Array<decl_index::LoadItem>{};
auto seen = mlc::Array<mlc::String>{};
auto my_decls = mlc::Array<std::shared_ptr<ast::Decl>>{};
auto my_imports = mlc::Array<mlc::String>{};
auto my_namespace_import_aliases = mlc::Array<decl_index::NamespaceImportAlias>{};
auto all_errors = mlc::Array<mlc::String>{};
auto index = 0;
while ((index < prog.decls.length())) {
std::visit(overloaded{[&](const ast::DeclImport& declImport) { auto [import_path, symbols] = declImport; return [&]() {
auto resolved = resolve_dotdot(resolve_import_path(path, import_path));
my_imports.push_back(resolved);
if (((symbols.length() >= 2) && (symbols[0] == mlc::String("*", 1)))) {
my_namespace_import_aliases.push_back(decl_index::NamespaceImportAlias{symbols[1], resolved});
}
auto dep_result = load_module_impl(resolved, cur_loaded, cache);
all_errors = ast::errs_append(all_errors, dep_result.errors);
auto unit_placeholder = /* unit */;
unit_placeholder = dep_result.items.fold(unit_placeholder, [=](void _, decl_index::LoadItem dependency_item) mutable { return [&]() {
if ((!path_in_loaded(dependency_item.path, seen))) {
seen.push_back(dependency_item.path);
items.push_back(dependency_item);
}
/* unit */;
return /* unit */;
}(); });
return /* unit */;
}(); },
[&](const ast::DeclExported& declExported) { auto [d] = declExported; return my_decls.push_back(prog.decls[index]); },
[&](const auto& __v) { return my_decls.push_back(prog.decls[index]); }
}, (*prog.decls[index]));
index = (index + 1);
}
items.push_back(decl_index::LoadItem{norm_path, my_decls, my_imports, my_namespace_import_aliases});
auto result = LoadResult{items, all_errors};
cache.set(norm_path, result);
return result;
}
}
}

}
LoadResult load_module(mlc::String path, mlc::HashMap<mlc::String, LoadResult>& cache) noexcept{
return load_module_impl(path, mlc::Array<mlc::String>{}, cache);
}
MergeResult merge_program(mlc::String entry_path, ast::Program prog) noexcept{
auto all_decls = mlc::Array<std::shared_ptr<ast::Decl>>{};
auto all_errors = mlc::Array<mlc::String>{};
auto seen_paths = mlc::Array<mlc::String>{};
auto items_ordered = mlc::Array<decl_index::LoadItem>{};
auto entry_decls = mlc::Array<std::shared_ptr<ast::Decl>>{};
auto entry_imports = mlc::Array<mlc::String>{};
auto entry_namespace_import_aliases = mlc::Array<decl_index::NamespaceImportAlias>{};
auto cache = mlc::HashMap<mlc::String, LoadResult>();
auto index = 0;
while ((index < prog.decls.length())) {
std::visit(overloaded{[&](const ast::DeclImport& declImport) { auto [path, symbols] = declImport; return [&]() {
auto resolved = resolve_dotdot(resolve_import_path(entry_path, path));
entry_imports.push_back(resolved);
if (((symbols.length() >= 2) && (symbols[0] == mlc::String("*", 1)))) {
entry_namespace_import_aliases.push_back(decl_index::NamespaceImportAlias{symbols[1], resolved});
}
auto dep_result = load_module(resolved, cache);
all_errors = ast::errs_append(all_errors, dep_result.errors);
auto dep_item_index = 0;
return [&]() {
while ((dep_item_index < dep_result.items.length())) {
auto dependency_item = dep_result.items[dep_item_index];
if ((!path_in_loaded(dependency_item.path, seen_paths))) {
seen_paths.push_back(dependency_item.path);
items_ordered.push_back(dependency_item);
auto decl_inner_index = 0;
[&]() {
while ((decl_inner_index < dependency_item.decls.length())) {
all_decls.push_back(dependency_item.decls[decl_inner_index]);
decl_inner_index = (decl_inner_index + 1);
}
}();
}
dep_item_index = (dep_item_index + 1);
}
}();
}(); },
[&](const ast::DeclExported& declExported) { auto [d] = declExported; return [&]() {
entry_decls.push_back(d);
return all_decls.push_back(d);
}(); },
[&](const auto& __v) { return [&]() {
entry_decls.push_back(prog.decls[index]);
return all_decls.push_back(prog.decls[index]);
}(); }
}, (*prog.decls[index]));
index = (index + 1);
}
auto norm_entry = resolve_dotdot(entry_path);
items_ordered.push_back(decl_index::LoadItem{norm_entry, entry_decls, entry_imports, entry_namespace_import_aliases});
return MergeResult{ast::Program{all_decls}, all_errors, items_ordered};
}
ast::Result<mlc::String, mlc::Array<mlc::String>> compile_modular(mlc::String entry_path, mlc::String out_dir) noexcept{
auto lex = lexer::tokenize(mlc::file::read_to_string(entry_path));
if (ast_tokens::LexOut_has_errors(lex)) {
return ast::Err{mlc::Array<mlc::String>{((mlc::String("lex: ", 5) + mlc::to_string(lex.errors[0])) + mlc::String("", 0))}};
} else {
auto source_path = resolve_dotdot(entry_path);
auto parsed_program = decls::parse_program_with_source_path(lex.tokens, source_path);
auto merged = merge_program(entry_path, parsed_program);
if ((merged.errors.length() > 0)) {
return ast::Err{merged.errors};
} else {
auto merged_item_count = merged.items.length();
auto entry_load_item = merged.items[(merged_item_count - 1)];
auto entry_only_program = ast::Program{entry_load_item.decls};
auto __try_0 = check::check_with_context(entry_only_program, merged.prog);
if (std::get_if<1>(&__try_0)) return *std::get_if<1>(&__try_0);
auto _checked = std::get<0>(__try_0).field0;
return ast::Ok{compile_modular_loop(merged.items, merged.prog, out_dir)};
}
}
}
mlc::String compile_modular_loop(mlc::Array<decl_index::LoadItem> items, ast::Program full_prog, mlc::String out_dir) noexcept{
auto expanded_merge_program = param_destructure_expand::expand_parameter_destructuring_in_program(full_prog);
auto registry = registry::build_registry(expanded_merge_program);
auto transformed_items = transform_decl::transform_load_items(items, registry, expanded_merge_program);
auto precomputed = module::precompute(expanded_merge_program, items);
auto unit_placeholder = /* unit */;
unit_placeholder = transformed_items.fold(unit_placeholder, [=](void _, semantic_ir::SLoadItem transformed_load_item) mutable { return [&]() {
auto generated_output = module::gen_module(transformed_load_item, items, expanded_merge_program, precomputed);
auto module_base = cpp_naming::path_to_module_base(transformed_load_item.path);
auto header_path = ((out_dir.length() > 0) ? (((((mlc::String("", 0) + mlc::to_string(out_dir)) + mlc::String("/", 1)) + mlc::to_string(module_base)) + mlc::String(".hpp", 4))) : (((mlc::String("", 0) + mlc::to_string(module_base)) + mlc::String(".hpp", 4))));
auto implementation_path = ((out_dir.length() > 0) ? (((((mlc::String("", 0) + mlc::to_string(out_dir)) + mlc::String("/", 1)) + mlc::to_string(module_base)) + mlc::String(".cpp", 4))) : (((mlc::String("", 0) + mlc::to_string(module_base)) + mlc::String(".cpp", 4))));
mlc::file::write_string(header_path, generated_output.h);
mlc::file::write_string(implementation_path, generated_output.c);
/* unit */;
return /* unit */;
}(); });
return mlc::String("", 0);
}
mlc::String format_errs(mlc::String label, mlc::Array<mlc::String> errors) noexcept{
return errors.map([=](mlc::String message_line) mutable { return ((((mlc::String("", 0) + mlc::to_string(label)) + mlc::String(": ", 2)) + mlc::to_string(message_line)) + mlc::String("\n", 1)); }).join(mlc::String("", 0));
}
int main(int argc, char** argv) noexcept{
mlc::io::set_args(std::vector<mlc::String>(argv + 1, argv + argc));
auto command_line_arguments = mlc::io::args();
if ((command_line_arguments.length() == 0)) {
mlc::io::println(mlc::String("Usage: mlcc <source.mlc> [-o out_dir]", 37));
mlc::io::exit(1);
}
auto entry_path = (((command_line_arguments.length() >= 3) && (command_line_arguments[0] == mlc::String("-o", 2))) ? (command_line_arguments[2]) : (command_line_arguments[0]));
auto out_dir = (((command_line_arguments.length() >= 3) && (command_line_arguments[0] == mlc::String("-o", 2))) ? (command_line_arguments[1]) : ((((command_line_arguments.length() >= 3) && (command_line_arguments[1] == mlc::String("-o", 2))) ? (command_line_arguments[2]) : (mlc::String("out", 3)))));
return std::visit(overloaded{[&](const ast::Ok<auto>& ok) { auto [__0] = ok; return 0; },
[&](const ast::Err<auto>& err) { auto [errors] = err; return [&]() {
mlc::io::print(format_errs(mlc::String("error", 5), errors));
mlc::io::exit(1);
return 0;
}(); }
}, compile_modular(entry_path, out_dir));
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
