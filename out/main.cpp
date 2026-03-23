#define main mlc_user_main
#include "main.hpp"

namespace mlc_main {

mlc::String dirname(mlc::String path) noexcept{
auto last_slash = (-1);
auto index = 0;
while ((index < path.length())) {
if ((path.char_at(index) == mlc::String("/", 1))) {
last_slash = index;
}
index = (index + 1);
}
if ((last_slash <= 0)) {
return mlc::String("", 0);
} else {
return path.substring(0, last_slash);
}
}
mlc::String resolve_dotdot(mlc::String path) noexcept{
auto p = path;
auto i = 0;
while (((i + 4) <= p.length())) {
if ((p.substring(i, 4) == mlc::String("/../", 4))) {
auto j = (i - 1);
while (((j >= 0) && (p.char_at(j) != mlc::String("/", 1)))) {
j = (j - 1);
}
auto prefix = ((j <= 0) ? mlc::String("", 0) : p.substring(0, j));
auto suffix = p.substring((i + 4), ((p.length() - i) - 4));
p = ((prefix == mlc::String("", 0)) ? suffix : ((prefix + mlc::String("/", 1)) + suffix));
i = 0;
}
i = (i + 1);
}
if (((p.length() >= 3) && (p.substring(0, 3) == mlc::String("../", 3)))) {
p = p.substring(3, (p.length() - 3));
}
return p;
}
mlc::String resolve_import_path(mlc::String base_path, mlc::String import_path) noexcept{
auto base_dir = dirname(base_path);
auto rest = (((import_path.length() >= 2) && (import_path.substring(0, 2) == mlc::String("./", 2))) ? import_path.substring(2, (import_path.length() - 2)) : import_path);
auto with_ext = (((rest.length() >= 4) && (rest.substring((rest.length() - 4), 4) == mlc::String(".mlc", 4))) ? rest : (rest + mlc::String(".mlc", 4)));
auto raw = ((base_dir == mlc::String("", 0)) ? with_ext : ((base_dir + mlc::String("/", 1)) + with_ext));
return resolve_dotdot(raw);
}
bool path_in_loaded(mlc::String path, mlc::Array<mlc::String> loaded) noexcept{
auto found = false;
auto i = 0;
while ((i < loaded.length())) {
if ((loaded[i] == path)) {
found = true;
}
i = (i + 1);
}
return found;
}
LoadResult load_module_impl(mlc::String path, mlc::Array<mlc::String> loaded, mlc::HashMap<mlc::String, LoadResult>& cache) noexcept{
auto norm_path = resolve_dotdot(path);
if (cache.has(norm_path)) {
return cache.get(norm_path);
} else if (path_in_loaded(norm_path, loaded)) {
return LoadResult{{}, mlc::Array{(mlc::String("circular: ", 10) + norm_path)}};
} else {
auto cur_loaded = loaded;
cur_loaded.push_back(norm_path);
auto source = mlc::file::read_to_string(path);
if (((source.length() == 0) && (!mlc::file::exists(path)))) {
return LoadResult{{}, mlc::Array{(mlc::String("file not found: ", 16) + path)}};
} else {
auto lex = lexer::tokenize(source);
if (ast_tokens::LexOut_has_errors(lex)) {
return LoadResult{{}, mlc::Array{(((mlc::String("lex ", 4) + path) + mlc::String(": ", 2)) + lex.errors[0])}};
} else {
auto prog = decls::parse_program(lex.tokens);
auto items = mlc::Array<codegen::LoadItem>{};
auto seen = mlc::Array<mlc::String>{};
auto my_decls = mlc::Array<std::shared_ptr<ast::Decl>>{};
auto my_imports = mlc::Array<mlc::String>{};
auto all_errors = mlc::Array<mlc::String>{};
auto index = 0;
while ((index < prog.decls.length())) {
std::visit(overloaded{[&](const ast::DeclImport& declImport) { auto [import_path, __1] = declImport; return [&]() {
auto resolved = resolve_dotdot(resolve_import_path(path, import_path));
my_imports.push_back(resolved);
auto dep_result = load_module_impl(resolved, cur_loaded, cache);
all_errors = ast::errs_append(all_errors, dep_result.errors);
auto dep_i = 0;
return [&]() {
while ((dep_i < dep_result.items.length())) {
auto item = dep_result.items[dep_i];
if ((!path_in_loaded(item.path, seen))) {
seen.push_back(item.path);
items.push_back(item);
}
dep_i = (dep_i + 1);
}
}();
}(); },
[&](const ast::DeclExported& declExported) { auto [d] = declExported; return my_decls.push_back(prog.decls[index]); },
[&](const auto& __v) { return my_decls.push_back(prog.decls[index]); }
}, (*prog.decls[index]));
index = (index + 1);
}
items.push_back(codegen::LoadItem{norm_path, my_decls, my_imports});
auto result = LoadResult{items, all_errors};
cache.set(norm_path, result);
return result;
}
}
}

}
LoadResult load_module(mlc::String path, mlc::HashMap<mlc::String, LoadResult>& cache) noexcept{
return load_module_impl(path, {}, cache);
}
mlc::String path_to_module_base(mlc::String path) noexcept{
auto last_slash = (-1);
auto last_dot = path.length();
auto i = 0;
while ((i < path.length())) {
if ((path.char_at(i) == mlc::String("/", 1))) {
last_slash = i;
} else if (((path.char_at(i) == mlc::String(".", 1)) && (i > last_slash))) {
last_dot = i;
}

i = (i + 1);
}
if ((last_dot > last_slash)) {
return path.substring((last_slash + 1), ((last_dot - last_slash) - 1));
} else {
return path.substring((last_slash + 1), ((path.length() - last_slash) - 1));
}
}
MergeResult merge_program(mlc::String entry_path, ast::Program prog) noexcept{
auto all_decls = mlc::Array<std::shared_ptr<ast::Decl>>{};
auto all_errors = mlc::Array<mlc::String>{};
auto seen_paths = mlc::Array<mlc::String>{};
auto items_ordered = mlc::Array<codegen::LoadItem>{};
auto entry_decls = mlc::Array<std::shared_ptr<ast::Decl>>{};
auto entry_imports = mlc::Array<mlc::String>{};
auto cache = mlc::HashMap<mlc::String, LoadResult>();
auto index = 0;
while ((index < prog.decls.length())) {
std::visit(overloaded{[&](const ast::DeclImport& declImport) { auto [path, __1] = declImport; return [&]() {
auto resolved = resolve_dotdot(resolve_import_path(entry_path, path));
entry_imports.push_back(resolved);
auto dep_result = load_module(resolved, cache);
all_errors = ast::errs_append(all_errors, dep_result.errors);
auto dep_i = 0;
return [&]() {
while ((dep_i < dep_result.items.length())) {
auto item = dep_result.items[dep_i];
if ((!path_in_loaded(item.path, seen_paths))) {
seen_paths.push_back(item.path);
items_ordered.push_back(item);
auto d = 0;
[&]() {
while ((d < item.decls.length())) {
all_decls.push_back(item.decls[d]);
d = (d + 1);
}
}();
}
dep_i = (dep_i + 1);
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
items_ordered.push_back(codegen::LoadItem{norm_entry, entry_decls, entry_imports});
return MergeResult{ast::Program{all_decls}, all_errors, items_ordered};
}
ast::Result<mlc::String, mlc::Array<mlc::String>> compile_modular(mlc::String entry_path, mlc::String out_dir) noexcept{
auto src = mlc::file::read_to_string(entry_path);
auto lex = lexer::tokenize(src);
if (ast_tokens::LexOut_has_errors(lex)) {
return ast::Err{mlc::Array{(mlc::String("lex: ", 5) + lex.errors[0])}};
} else {
auto prog = decls::parse_program(lex.tokens);
auto merged = merge_program(entry_path, prog);
if ((merged.errors.length() > 0)) {
return ast::Err{merged.errors};
} else {
auto n = merged.items.length();
auto entry_item = merged.items[(n - 1)];
auto entry_prog = ast::Program{entry_item.decls};
auto __try_0 = infer::check_with_context(entry_prog, merged.prog);
if (std::get_if<1>(&__try_0)) return *std::get_if<1>(&__try_0);
auto _chk = std::get<0>(__try_0).field0;
return ast::Ok{compile_modular_loop(merged.items, merged.prog, out_dir)};
}
}
}
mlc::String compile_modular_loop(mlc::Array<codegen::LoadItem> items, ast::Program full_prog, mlc::String out_dir) noexcept{
auto precomp = codegen::precompute(full_prog, items);
auto i = 0;
while ((i < items.length())) {
auto item = items[i];
auto out = codegen::gen_module(item, items, full_prog, precomp);
auto base = path_to_module_base(item.path);
auto hpp_path = ((out_dir.length() > 0) ? (((out_dir + mlc::String("/", 1)) + base) + mlc::String(".hpp", 4)) : (base + mlc::String(".hpp", 4)));
auto cpp_path = ((out_dir.length() > 0) ? (((out_dir + mlc::String("/", 1)) + base) + mlc::String(".cpp", 4)) : (base + mlc::String(".cpp", 4)));
mlc::file::write_string(hpp_path, out.h);
mlc::file::write_string(cpp_path, out.c);
i = (i + 1);
}
return mlc::String("", 0);
}
mlc::String format_errs(mlc::String label, mlc::Array<mlc::String> errors) noexcept{
auto output = mlc::String("", 0);
auto i = 0;
while ((i < errors.length())) {
output = ((((output + label) + mlc::String(": ", 2)) + errors[i]) + mlc::String("\n", 1));
i = (i + 1);
}
return output;
}
int main(int argc, char** argv) noexcept{
mlc::io::set_args(std::vector<mlc::String>(argv + 1, argv + argc));
auto a = mlc::io::args();
if ((a.length() == 0)) {
mlc::io::println(mlc::String("Usage: mlcc <source.mlc> [-o out_dir]", 37));
mlc::io::exit(1);
}
auto entry_path = (((a.length() >= 3) && (a[0] == mlc::String("-o", 2))) ? a[2] : a[0]);
auto out_dir = (((a.length() >= 3) && (a[0] == mlc::String("-o", 2))) ? a[1] : (((a.length() >= 3) && (a[1] == mlc::String("-o", 2))) ? a[2] : mlc::String("out", 3)));
return std::visit(overloaded{[&](const ast::Ok<auto>& ok) { auto [__0] = ok; return 0; },
[&](const ast::Err<auto>& err) { auto [errors] = err; return [&]() {
mlc::io::print(format_errs(mlc::String("error", 5), errors));
return mlc::io::exit(1);
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
