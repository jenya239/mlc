#ifndef MODULE_LOADER_HPP
#define MODULE_LOADER_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
#include "lexer.hpp"
#include "decls.hpp"
#include "load_item.hpp"
#include "profile.hpp"
#include "header_import.hpp"
#include "path_normalize.hpp"
namespace module_loader {

struct LoadResult {
  mlc::Array<load_item::LoadItem> items;
  mlc::Array<mlc::String> errors;
};
LoadResult load_module_impl(mlc::String path, mlc::HashMap<mlc::String, bool>& loaded, mlc::HashMap<mlc::String, LoadResult>& cache, bool profile_enabled) noexcept;
LoadResult load_module(mlc::String path, mlc::HashMap<mlc::String, LoadResult>& cache, bool profile_enabled) noexcept;

} // namespace module_loader

#endif // MODULE_LOADER_HPP
