#ifndef PROGRAM_MERGE_HPP
#define PROGRAM_MERGE_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
#include "load_item.hpp"
#include "path_normalize.hpp"
#include "module_loader.hpp"
namespace program_merge {

struct MergeResult {
  ast::Program program;
  mlc::Array<mlc::String> errors;
  mlc::Array<load_item::LoadItem> items;
};
MergeResult merge_program_with_cache(mlc::String entry_path, ast::Program program, mlc::HashMap<mlc::String, module_loader::LoadResult>& load_cache, bool profile_enabled) noexcept;
MergeResult merge_program(mlc::String entry_path, ast::Program program, bool profile_enabled) noexcept;

} // namespace program_merge

#endif // PROGRAM_MERGE_HPP
