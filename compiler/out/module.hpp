#ifndef MODULE_HPP
#define MODULE_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "context.hpp"
#include "cpp_naming.hpp"
#include "decl.hpp"

namespace module {

struct Expr;
struct Stmt;

context::PrecomputedCtx precompute(ast::Program prog, mlc::Array<decl_index::LoadItem> all_items) noexcept;

mlc::String gen_program(ast::Program prog) noexcept;

context::GenModuleOut gen_module(decl_index::LoadItem item, mlc::Array<decl_index::LoadItem> all_items, ast::Program full_prog, context::PrecomputedCtx precomp) noexcept;

} // namespace module

#endif // MODULE_HPP
