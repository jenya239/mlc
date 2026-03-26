#ifndef MODULE_HPP
#define MODULE_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "context.hpp"
#include "decl.hpp"

namespace module {

struct Expr;
struct Stmt;

context::PrecomputedCtx precompute(ast::Program prog, mlc::Array<context::LoadItem> all_items) noexcept;

mlc::String gen_program(ast::Program prog) noexcept;

context::GenModuleOut gen_module(context::LoadItem item, mlc::Array<context::LoadItem> all_items, ast::Program full_prog, context::PrecomputedCtx precomp) noexcept;

} // namespace module

#endif // MODULE_HPP
