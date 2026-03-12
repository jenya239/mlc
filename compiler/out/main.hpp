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

struct Expr;
struct Stmt;

struct LoadResult {mlc::Array<codegen::LoadItem> items;mlc::Array<mlc::String> errors;};

struct MergeResult {ast::Program prog;mlc::Array<mlc::String> errors;mlc::Array<codegen::LoadItem> items;};

} // namespace mlc_main

#endif // MAIN_HPP
