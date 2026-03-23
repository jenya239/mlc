#ifndef MAIN_HPP
#define MAIN_HPP

#include "mlc.hpp"
#include <variant>

#include "lexer.hpp"
#include "decls.hpp"
#include "infer.hpp"
#include "codegen.hpp"
#include "ast.hpp"

namespace mlc_main {

struct LoadResult {mlc::Array<codegen::LoadItem> items;mlc::Array<mlc::String> errors;};
struct MergeResult {ast::Program prog;mlc::Array<mlc::String> errors;mlc::Array<codegen::LoadItem> items;};
mlc::String dirname(mlc::String path) noexcept;
mlc::String resolve_dotdot(mlc::String path) noexcept;
mlc::String resolve_import_path(mlc::String base_path, mlc::String import_path) noexcept;
bool path_in_loaded(mlc::String path, mlc::Array<mlc::String> loaded) noexcept;
LoadResult load_module_impl(mlc::String path, mlc::Array<mlc::String> loaded, mlc::HashMap<mlc::String, LoadResult>& cache) noexcept;
LoadResult load_module(mlc::String path, mlc::HashMap<mlc::String, LoadResult>& cache) noexcept;
mlc::String path_to_module_base(mlc::String path) noexcept;
MergeResult merge_program(mlc::String entry_path, ast::Program prog) noexcept;
ast::Result<mlc::String, mlc::Array<mlc::String>> compile_modular(mlc::String entry_path, mlc::String out_dir) noexcept;
mlc::String compile_modular_loop(mlc::Array<codegen::LoadItem> items, ast::Program full_prog, mlc::String out_dir) noexcept;
mlc::String format_errs(mlc::String label, mlc::Array<mlc::String> errors) noexcept;
int main(int argc, char** argv) noexcept;

} // namespace mlc_main

#endif // MAIN_HPP
