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

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

struct LoadResult {mlc::Array<load_item::LoadItem> items;mlc::Array<mlc::String> errors;};

module_loader::LoadResult load_module(mlc::String path, mlc::HashMap<mlc::String, module_loader::LoadResult>& cache, bool profile_enabled) noexcept;

} // namespace module_loader

#endif // MODULE_LOADER_HPP
