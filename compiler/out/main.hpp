#ifndef MAIN_HPP
#define MAIN_HPP

#include "mlc.hpp"
#include <variant>
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

struct LoadResult {
  mlc::Array<decl_index::LoadItem> items;
  mlc::Array<mlc::String> errors;
};
struct MergeResult {
  ast::Program program;
  mlc::Array<mlc::String> errors;
  mlc::Array<decl_index::LoadItem> items;
};
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

} // namespace mlc_main

#endif // MAIN_HPP
